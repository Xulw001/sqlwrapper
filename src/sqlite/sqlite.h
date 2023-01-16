#pragma once
#include <sqlite3.h>

#include "../framework/interface.h"

namespace sqlwrapper {

class SqliteWrapper {
 private:
  sqlite3* m_conn;
  uint32_t m_retry;
  uint32_t m_timeout;
  sqlite3_stmt* m_stmt;

  std::string errStr;
  int errCode;

 public:
  SqliteWrapper(uint32_t retry = 5, uint32_t timeout = 1)
      : m_retry(retry), m_timeout(timeout) {
    ;
  }

  virtual ~SqliteWrapper() { Close(); }

  virtual SQLErr Open(std::string& dbName, std::string& user, std::string& pass,
                      std::string& port);
  virtual SQLErr Close();
  virtual SQLErr Begin();
  virtual SQLErr Commit();
  virtual SQLErr RollBack();
  virtual SQLErr PrepareStatement(std::string& sql);
  virtual SQLErr Bind(std::vector<int>& idx, Row* obj);
  virtual SQLErr Prase(Row* obj);
  virtual SQLErr Execute();
  virtual SQLErr Next();
  virtual SQLErr Statement(std::string& sql);
  virtual int GetLastError(std::string& err);

  void SetLastError(int action, const char* err);
  void SetLastError(int action);
  void ClearLastError();
};

}  // namespace sqlwrapper