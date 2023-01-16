#include "sql.h"

#include <stdarg.h>

namespace sqlwrapper {

std::unordered_map<std::string, std::shared_ptr<Row>>
    RowFactory::m_DataInvokeMap;

SQLInstance::SQLInstance(SqlWrapper *conn, std::string dbName, std::string user,
                         std::string pass, std::string port)
    : m_db(conn) {
  m_db->Open(dbName, user, pass, port);
}

SQLInstance::~SQLInstance() {
  if (NULL != m_db) {
    m_db->Close();
    delete m_db;
    m_db = NULL;
  };
}

bool SQLInstance::InvokeRDB(DaoInfo *dao, ...) {
  bool sqlret = true;
  void *in = NULL, *out = NULL;
  va_list va;
  va_start(va, dao);

  if (dao->in.type != EMPTY) {
    in = va_arg(va, void *);
  }

  if (dao->id == Select) {
    out = va_arg(va, Container *);
  }

  va_end(va);

  switch (dao->id) {
    case Insert: {
      sqlret = doInsert(dao->sql, dao->idx, static_cast<Container *>(in));
    } break;
    case Update: {
      sqlret = doUpdate(dao->sql, dao->idx, static_cast<Row *>(in));
    } break;
    case Delete: {
      sqlret = doDelete(dao->sql, dao->idx, static_cast<Row *>(in));
    } break;
    case Select: {
      sqlret = doSelect(dao->sql, dao->idx, static_cast<Row *>(in),
                        dao->out.field, static_cast<Container *>(out));
    } break;
    default:
      // do error
      return false;
  }

  return true;
}

bool SQLInstance::doInsert(std::string sql, std::vector<int> &idx,
                           Container *in) {
  bool sqlerr = false;

  m_db->Begin();

  if (m_db->PrepareStatement(sql) != SQL_OK) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
    return false;
  }

  for (size_t i = 0; i < in->size(); i++) {
    if (m_db->Bind(idx, in->at(i)) != SQL_OK) {
      sqlerr = true;
      break;
    }

    if (m_db->Execute() != SQL_DONE) {
      sqlerr = true;
      break;
    }
  }

  if (sqlerr) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
    m_db->RollBack();
    return false;
  } else {
    m_db->Commit();
    return true;
  }
}

bool SQLInstance::doUpdate(std::string sql, std::vector<int> &idx, Row *in) {
  m_db->Begin();

  if (m_db->PrepareStatement(sql) != SQL_OK) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
    return false;
  }

  if (m_db->Bind(idx, in) != SQL_OK || m_db->Execute() != SQL_DONE) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
    m_db->RollBack();
    return false;
  }

  m_db->Commit();
  return true;
}

bool SQLInstance::doDelete(std::string sql, std::vector<int> &idx, Row *in) {
  m_db->Begin();

  if (m_db->PrepareStatement(sql) != SQL_OK) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
    return false;
  }

  if (m_db->Bind(idx, in) != SQL_OK || m_db->Execute() != SQL_DONE) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
    m_db->RollBack();
    return false;
  }

  m_db->Commit();
  return true;
}

bool SQLInstance::doSelect(std::string sql, std::vector<int> &idx, Row *in,
                           std::string clazz, Container *out) {
  m_db->Begin();

  if (m_db->PrepareStatement(sql) != SQL_OK) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
    return false;
  }

  if (m_db->Bind(idx, in) != SQL_OK) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
    m_db->RollBack();
    return false;
  }

  bool ret = true;
  do {
    SQLErr res = SQL_DONE;
    res = m_db->Next();
    if (res == SQL_ROW) {
      Row *obj = RowFactory::newInstance(clazz);
      if (m_db->Prase(obj) != SQL_OK) {
        delete obj;
        ret = false;
        break;
      }
      out->append(obj, false);
    } else if (res == SQL_DONE) {
      break;
    } else {
      ret = false;
      break;
    }
  } while (true);

  if (!ret) {
    m_ErrCode = m_db->GetLastError(m_ErrMsg);
  }

  m_db->Commit();
  return ret;
}

}  // namespace sqlwrapper