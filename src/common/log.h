#pragma once
#include <fstream>
#include <iostream>

#ifdef LOG_FILE
class Log {
private:
	bool start;
	std::ofstream os;

public:
	Log(std::string path) {
		start = true;
		os.open(path, std::ios::app);
	}

	std::ostream& operator<<(std::string str) {
		if (!start) {
			os << std::endl;
		}
		else {
			start = false;
		}
		return os << str;
	}

	~Log()
	{
		os << std::endl;
		os.close();
	}
};

static Log instance("framework.log");
#define LOG_ERR instance
#else
class Log {
 public:
  Log(int type = 0):type(type) { start = true; }

  std::ostream& operator<<(std::string str) {
    if (!start) {
      std::cout << std::endl;
	  switch (type)
	  {
	  case 0:std::cout << " ====== Log Info  ====== "; break;
	  case 1:std::cout << " ====== Log Error ====== "; break;
	  case 2:std::cout << " ====== Log Debug ====== "; break;
	  default:
		  break;
	  }
    } else {
      start = false;
    }
    return std::cout << str;
  }

 private:
  bool start;
  int type;
};
static Log log_err_(1);
static Log log_deg_(2);
#define LOG_ERR log_err_
#define LOG_DBG log_deg_
#endif

