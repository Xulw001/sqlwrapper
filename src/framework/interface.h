#pragma once
#include "Row.h"

namespace sqlwrapper {

enum SQLErr {
  SQL_OK,
  SQL_DONE,
  SQL_ROW,
  SQL_FAILED,
  SQL_ERR,
  SQL_MISUSE,
};

class SqlWrapper {
 public:
  virtual ~SqlWrapper() { ; }
  virtual SQLErr Open(std::string& dbName, std::string& user, std::string& pass,
                      std::string& port) = 0;
  virtual SQLErr Close() = 0;
  virtual SQLErr Begin() = 0;
  virtual SQLErr Commit() = 0;
  virtual SQLErr RollBack() = 0;
  virtual SQLErr PrepareStatement(std::string& sql) = 0;
  virtual SQLErr Bind(std::vector<int>& idx, Row* obj) = 0;
  virtual SQLErr Prase(Row* obj) = 0;
  virtual SQLErr Execute() = 0;
  virtual SQLErr Next() = 0;
  virtual SQLErr Statement(std::string& sql) = 0;
  virtual int GetLastError(std::string& err) = 0;
};

}  // namespace sqlwrapper