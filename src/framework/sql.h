#pragma once
#include "container.h"
#include "interface.h"

namespace sqlwrapper {

enum Action { Insert, Delete, Update, Select };

typedef struct {
  DataType type;
  const char* field;
} DataInfo;

typedef struct {
  Action id;
  const char* sql;
  std::vector<int> idx;  // point the idx of field in (in) used by sql
  DataInfo in;
  DataInfo out;
} DaoInfo;

class SQLInstance {
 private:
  std::string m_ErrMsg;
  int m_ErrCode;
  int m_ErrDetailCode;
  SqlWrapper* m_db;

  bool doInsert(std::string sql, std::vector<int>& idx, Container* in);
  bool doUpdate(std::string sql, std::vector<int>& idx, Row* in);
  bool doDelete(std::string sql, std::vector<int>& idx, Row* in);
  bool doSelect(std::string sql, std::vector<int>& idx, Row* in,
                std::string clazz, Container* out);

 public:
  SQLInstance(SqlWrapper *conn, std::string dbName, std::string user, std::string pass,
              std::string port);
  ~SQLInstance();
  bool InvokeRDB(DaoInfo* dao, ...);
};

}  // namespace sqlwrapper