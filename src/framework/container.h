#pragma once
#include <memory>

#include "row.h"

namespace sqlwrapper {

class Container {
 public:
  virtual void append(Row* obj, bool local = true) = 0;
  virtual Row* at(size_t idx) = 0;
  virtual size_t size() = 0;
};

class DataContainer : Container {
 private:
  std::vector<std::shared_ptr<Row>> mData;

 public:
  DataContainer() { ; }
  ~DataContainer() {
    mData.clear();
    std::vector<std::shared_ptr<Row>>().swap(mData);
  }

  virtual void append(Row* obj, bool local = true) {
    if (local) {
      mData.push_back(std::shared_ptr<Row>(obj->clone()));
    } else {
      mData.push_back(std::shared_ptr<Row>(obj));
    }
  }

  virtual Row* at(size_t idx) { return mData[idx].get(); }

  Row* operator[](size_t idx) { return mData[idx].get(); }

  virtual size_t size() { return mData.size(); }
};

}  // namespace sqlwrapper
