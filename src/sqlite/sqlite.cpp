#include "sqlite.h"
#include "../common/log.h"

namespace sqlwrapper {

enum Action {
  OPEN,
  CLOSE,
  BEGIN,
  COMMIT,
  ROLLBACK,
  PREPARE,
  EXECUTE,
  NEXT,
  BIND,
  PRASE,
  STAT,
};

void SqliteWrapper::ClearLastError() {
  errStr.clear();
  errCode = SQLITE_OK;
}

void SqliteWrapper::SetLastError(int action) {
  SetLastError(action, sqlite3_errmsg(m_conn));
}

void SqliteWrapper::SetLastError(int action, const char* err) {
  switch (action) {
    case OPEN:
      LOG_ERR << "open failed, reason : " << err;
      break;
    case CLOSE:
      LOG_DBG << "close failed, reason : " << err;
      break;
    case BEGIN:
      LOG_DBG << "begin failed, reason : " << err;
      break;
    case COMMIT:
      LOG_DBG << "commit failed, reason : " << err;
      break;
    case ROLLBACK:
      LOG_DBG << "rollback failed, reason : " << err;
      break;
    case PREPARE:
      LOG_ERR << "prepare failed, reason : " << err;
      break;
    case EXECUTE:
      LOG_ERR << "execute failed, reason : " << err;
      break;
    case NEXT:
      LOG_ERR << "next failed, reason : " << err;
      break;
    case BIND:
      LOG_ERR << "bind failed, reason : " << err;
      break;
    case PRASE:
      break;
    case STAT:
      LOG_ERR << "exec failed, reason : " << err;
      break;
    default:
      break;
  }
  errCode = sqlite3_errcode(m_conn);
  errStr = err;
}

SQLErr SqliteWrapper::Open(std::string &dbName, std::string &user,
                           std::string &pass, std::string &port) {
  uint32_t retry = 0;
  while (retry < m_retry) {
    if (SQLITE_OK != sqlite3_open(dbName.c_str(), &m_conn)) {
      if (SQLITE_BUSY == sqlite3_errcode(m_conn)) {
        sqlite3_busy_timeout(m_conn, m_timeout);
        retry++;
        continue;
      }
      sqlite3_close(m_conn);
      break;
    }
    return SQL_OK;
  }
  SetLastError(OPEN);
  return SQL_ERR;
}

SQLErr SqliteWrapper::Close() {
  SQLErr err = SQL_OK;
  if (NULL != m_stmt) {
    if (SQLITE_OK != sqlite3_finalize(m_stmt)) {
      LOG_DBG << "free failed, reason : " << sqlite3_errmsg(m_conn);
      err = SQL_ERR;
    }
    m_stmt = NULL;
  }

  if (NULL != m_conn) {
    if (SQLITE_OK != sqlite3_close(m_conn)) {
      SetLastError(CLOSE);
      err = SQL_ERR;
    }
    m_conn = NULL;
  }
  
  return err;
}

SQLErr SqliteWrapper::Begin() {
  char* errmsg = NULL;
  if (SQLITE_OK != sqlite3_exec(m_conn, "begin", NULL, NULL, &errmsg)) {
    SetLastError(BEGIN, errmsg);
    sqlite3_free(errmsg);
    return SQL_FAILED;
  };
  return SQL_OK;
}

SQLErr SqliteWrapper::Commit() {
  SQLErr err = SQL_OK;
  char* errmsg = NULL;
  if (SQLITE_OK != sqlite3_exec(m_conn, "commit", NULL, NULL, &errmsg)) {
    SetLastError(COMMIT, errmsg);
    sqlite3_free(errmsg);
    err = SQL_FAILED;
  };

  if (NULL != m_stmt) {
    if (SQLITE_OK != sqlite3_finalize(m_stmt)) {
      LOG_DBG << "free failed, reason : " << sqlite3_errmsg(m_conn);
      err = SQL_ERR;
    }
    m_stmt = NULL;
  }
  return err;
}

SQLErr SqliteWrapper::RollBack() {
  SQLErr err = SQL_OK;
  char* errmsg = NULL;
  if (SQLITE_OK != sqlite3_exec(m_conn, "rollback", NULL, NULL, &errmsg)) {
    SetLastError(ROLLBACK, errmsg);
    sqlite3_free(errmsg);
    return SQL_FAILED;
  };

  if (NULL != m_stmt) {
    if (SQLITE_OK != sqlite3_finalize(m_stmt)) {
      LOG_DBG << "free failed, reason : " << sqlite3_errmsg(m_conn);
      err = SQL_ERR;
    }
    m_stmt = NULL;
  }

  return SQL_OK;
}

SQLErr SqliteWrapper::PrepareStatement(std::string &sql) {
  if (SQLITE_OK != sqlite3_prepare_v2(m_conn, sql.c_str(), -1, &m_stmt, NULL)) {
    SetLastError(PREPARE);
    return SQL_ERR;
  }
  return SQL_OK;
}

SQLErr SqliteWrapper::Bind(std::vector<int>& idx, Row* obj) {
  ClearLastError();
  for (size_t i = 0; i < idx.size(); i++) {
    switch (obj->getType(idx[i])) {
      case BOOLEAN:
        if (SQLITE_OK !=
            sqlite3_bind_int(m_stmt, i + 1, obj->getBool(idx[i]))) {
          SetLastError(BIND);
          return SQL_ERR;
        }
        break;
      case INTEGER:
        if (SQLITE_OK != sqlite3_bind_int(m_stmt, i + 1, obj->getInt(idx[i]))) {
          SetLastError(BIND);
          return SQL_ERR;
        }
        break;
      case LONG:
        if (SQLITE_OK !=
            sqlite3_bind_int64(m_stmt, i + 1, obj->getLong(idx[i]))) {
          SetLastError(BIND);
          return SQL_ERR;
        }
        break;
      case FLOAT:
        if (SQLITE_OK !=
            sqlite3_bind_double(m_stmt, i + 1, obj->getFloat(idx[i]))) {
          SetLastError(BIND);
          return SQL_ERR;
        }
        break;
      case DOUBLE:
        if (SQLITE_OK !=
            sqlite3_bind_double(m_stmt, i + 1, obj->getDouble(idx[i]))) {
          SetLastError(BIND);
          return SQL_ERR;
        }
        break;
      case STRING: {
        std::string val = obj->getStr(idx[i]);
        if (SQLITE_OK != sqlite3_bind_text(m_stmt, i + 1, val.c_str(), -1,
                                           SQLITE_TRANSIENT)) {
          SetLastError(BIND);
          return SQL_ERR;
        }
      } break;
      case BLOB: {
        uint32_t len = 0;
        const void* val = obj->getBlob(idx[i], len);
        if (SQLITE_OK !=
            sqlite3_bind_blob(m_stmt, i + 1, val, len, SQLITE_STATIC)) {
          SetLastError(BIND);
          return SQL_ERR;
        }
      } break;
      default:
        LOG_ERR << "bind failed, reason : unsupport type";
        return SQL_MISUSE;
    }
  }
  return SQL_OK;
}

SQLErr SqliteWrapper::Prase(Row* obj) {
  ClearLastError();
  for (size_t i = 0; i < obj->size(); i++) {
    switch (obj->getType(i)) {
      case BOOLEAN:
        obj->set(i, (bool)sqlite3_column_int(m_stmt, i));
        break;
      case INTEGER:
        obj->set(i, sqlite3_column_int(m_stmt, i));
        break;
      case LONG:
        obj->set(i, (uint64_t)sqlite3_column_int64(m_stmt, i));
        break;
      case FLOAT:
        obj->set(i, (float)sqlite3_column_double(m_stmt, i));
        break;
      case DOUBLE:
        obj->set(i, sqlite3_column_double(m_stmt, i));
        break;
      case STRING: {
        std::string val = "NULL";
        if (SQLITE_TEXT == sqlite3_column_type(m_stmt, i)) {
          val = reinterpret_cast<const char*>(sqlite3_column_text(m_stmt, i));
        }
        obj->set(i, val);
      } break;
      case BLOB: {
        uint32_t len = sqlite3_column_bytes(m_stmt, i);
        const void* val = sqlite3_column_blob(m_stmt, i);
        obj->set(i, len, const_cast<void*>(val));
      } break;
      default:
        LOG_ERR << "bind failed, reason : unsupport type";
        return SQL_MISUSE;
    }
  }
  return SQL_OK;
}

SQLErr SqliteWrapper::Execute() {
  ClearLastError();
  if (SQLITE_DONE == sqlite3_step(m_stmt)) {
    sqlite3_reset(m_stmt);
    return SQL_DONE;
  };
  SetLastError(EXECUTE);
  sqlite3_reset(m_stmt);
  return SQL_FAILED;
}

SQLErr SqliteWrapper::Next() {
  ClearLastError();
  int ret = sqlite3_step(m_stmt);
  if (SQLITE_DONE == ret) {
    sqlite3_reset(m_stmt);
    return SQL_DONE;
  } else if (SQLITE_ROW == ret) {
    return SQL_ROW;
  };
  SetLastError(NEXT);
  return SQL_FAILED;
}

SQLErr SqliteWrapper::Statement(std::string &sql) {
  char* errmsg = NULL;
  if (SQLITE_OK != sqlite3_exec(m_conn, sql.c_str(), NULL, NULL, &errmsg)) {
    SetLastError(STAT, errmsg);
    sqlite3_free(errmsg);
    return SQL_ERR;
  };
  return SQL_OK;
}

int SqliteWrapper::GetLastError(std::string& err) {
  err = errStr;
  return errCode;
}

}  // namespace sqlwrapper