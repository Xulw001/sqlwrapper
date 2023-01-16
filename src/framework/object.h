#pragma once
#include <memory>
#include <string>

namespace sqlwrapper {
class Boolean {
  bool v;

 public:
  void set(bool v) { this->v = v; }

  bool get() { return v; }

  Boolean(bool v) : v(v) { ; }
  Boolean() { ; }
};

class Integer {
  int v;

 public:
  void set(int v) { this->v = v; }

  int get() { return v; }

  Integer(int v) : v(v) { ; }
  Integer() { ; }
};

class Long {
  int64_t v;

 public:
  void set(int64_t v) { this->v = v; }

  int64_t get() { return v; }

  Long(int64_t v) : v(v) { ; }
  Long() { ; }
};

class Double {
  double v;

 public:
  void set(double v) { this->v = v; }

  double get() { return v; }

  Double(double v) : v(v) { ; }
  Double() { ; }
};

class Float {
  float v;

 public:
  void set(float v) { this->v = v; }

  float get() { return v; }

  Float(float v) : v(v) { ; }
  Float() { ; }
};

class String {
  std::string v;

 public:
  void set(std::string v) { this->v = v; }

  std::string get() { return v; }

  String(std::string v) : v(v) { ; }
  String() { ; }
};

class Blob {
  std::shared_ptr<void> v;
  uint32_t len;

 public:
  void set(uint32_t len, void* v) {
    uint8_t* val = new uint8_t[len];
    memcpy(val, v, len);
    this->v = std::shared_ptr<void>(val);
    this->len = len;
  }

  Blob(uint32_t len, void* v) {
    uint8_t* val = new uint8_t[len];
    memcpy(val, v, len);
    this->v = std::shared_ptr<void>(val);
    this->len = len;
  }
  
  Blob() { ; }
  ~Blob() { v.reset(); }

  uint32_t size() { return len; }

  void* val() { return v.get(); }
};

enum DataType {
  EMPTY,
  BOOLEAN,
  INTEGER,
  LONG,
  FLOAT,
  DOUBLE,
  STRING,
  BLOB,
  OBJECT
};

}  // namespace sqlwrapper
