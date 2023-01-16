#pragma once
#include <unordered_map>

#include "object.h"

namespace sqlwrapper {

class Row {
 protected:
  std::vector<DataType> m_Type;
  std::vector<int> m_Offset;

 public:
  virtual Row* clone() = 0;

  virtual ~Row() { ; }

  DataType getType(int i) { return m_Type[i]; }

  size_t size() { return m_Offset.size(); }

  bool getBool(int i) {
    Boolean* obj = (Boolean*)((char*)this + m_Offset[i]);
    return obj->get();
  }

  int getInt(int i) {
    Integer* obj = (Integer*)((char*)this + m_Offset[i]);
    return obj->get();
  }

  int64_t getLong(int i) {
    Long* obj = (Long*)((char*)this + m_Offset[i]);
    return obj->get();
  }

  float getFloat(int i) {
    Float* obj = (Float*)((char*)this + m_Offset[i]);
    return obj->get();
  }

  double getDouble(int i) {
    Double* obj = (Double*)((char*)this + m_Offset[i]);
    return obj->get();
  }

  std::string getStr(int i) {
    String* obj = (String*)((char*)this + m_Offset[i]);
    return obj->get();
  }

  void* getBlob(int i, uint32_t& len) {
    Blob* obj = (Blob*)((char*)this + m_Offset[i]);
    len = obj->size();
    return obj->val();
  }

  void set(int i, bool v) {
    Boolean* obj = (Boolean*)((char*)this + m_Offset[i]);
    obj->set(v);
  }
  void set(int i, int v) {
    Integer* obj = (Integer*)((char*)this + m_Offset[i]);
    obj->set(v);
  }
  void set(int i, uint64_t v) {
    Long* obj = (Long*)((char*)this + m_Offset[i]);
    obj->set(v);
  }
  void set(int i, float v) {
    Float* obj = (Float*)((char*)this + m_Offset[i]);
    obj->set(v);
  }
  void set(int i, double v) {
    Double* obj = (Double*)((char*)this + m_Offset[i]);
    obj->set(v);
  }
  void set(int i, std::string v) {
    String* obj = (String*)((char*)this + m_Offset[i]);
    obj->set(v);
  }
  void set(int i, uint32_t len, void* v) {
    Blob* obj = (Blob*)((char*)this + m_Offset[i]);
    obj->set(len, v);
  }
};

#define DECL_DATA_MODEL_BEGIN(clazz)                  \
  class clazz : public Row {                          \
   public:                                            \
    virtual Row* clone() { return new clazz(*this); } \
    clazz();                                          \
    virtual ~clazz() { ; }

#define DEF_DATA_MODEL(field, type)      \
 private:                                \
  type _##field;                         \
                                         \
 public:                                 \
  type get##field() { return _##field; } \
  void set##field(type field) { _##field = field; }

#define DECL_DATA_MODEL_END \
  }                         \
  ;

#define DEF_DATA_COLUMN_BEGIN(clazz) clazz::clazz() {
#define DEF_DATA_COLUMN_HANDER(field, type) \
  m_Type.push_back(type);                   \
  m_Offset.push_back((char*)&_##field - (char*)this);
#define DEF_DATA_COLUMN_END }

};  // namespace sqlwrapper

namespace sqlwrapper {
class RowFactory {
 private:
  static std::unordered_map<std::string, std::shared_ptr<Row>> m_DataInvokeMap;

 public:
  static Row* newInstance(std::string clazz) {
    Row* obj = m_DataInvokeMap[clazz].get();
    if (obj != nullptr) {
      return obj->clone();
    }
    return nullptr;
  }

  static void Regist(std::string clazz, Row* obj) {
    m_DataInvokeMap[clazz] = std::shared_ptr<Row>(obj);
  }
};

#define DEF_DATA_FACTORY_HANDER(clazz) RowFactory::Regist(#clazz, new clazz());
};  // namespace sqlwrapper
