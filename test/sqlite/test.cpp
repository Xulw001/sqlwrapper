// SQLWrapper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRTDBG_MAP_ALLOC
#include "test.h"

#include <crtdbg.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>

#include "framework/sql.h"
#include "sqlite/sqlite.h"

using namespace sqlwrapper;

DEF_DATA_COLUMN_BEGIN(Student)
DEF_DATA_COLUMN_HANDER(Name, STRING)
DEF_DATA_COLUMN_HANDER(Age, INTEGER)
DEF_DATA_COLUMN_END

int main() {
  _CrtSetBreakAlloc(151);
  DEF_DATA_FACTORY_HANDER(Student);
  DaoInfo stds[] = {
      {
          Insert,
          "insert into Student values(?, ?)",
          std::vector<int>{0, 1},
          OBJECT,
          "Student",
      },  // in
      {
          Update,
          "update Student set name = ?, age = ? where name = ?",
          std::vector<int>{0, 1, 0},
          OBJECT,
          "Student",
      },  // in
      {
          Delete,
          "delete from Student where name = ?",
          std::vector<int>{0},
          OBJECT,
          "Student",
      },  // in
      {
          Select,
          "select * from Student",
          std::vector<int>{},
          EMPTY,
          "",
          OBJECT,
          "Student",
      },  // out
      {
          Select,
          "select * from Student where name = ?",
          std::vector<int>{0},
          OBJECT,
          "Student",
          OBJECT,
          "Student",
      },                                // in out
      {Delete, "delete from Student"},  // nothing
  };

  _CrtMemState s1, s2, s3;
  _CrtMemCheckpoint(&s1);
  for (int i = 0; i < 1; i++) {
    SqliteWrapper *wrap = new SqliteWrapper();
    DataContainer in;
    std::ostringstream ossl;
    Student obj;
    for (int i = 0; i < 10000; i++) {
      ossl << "TestName" << i;
      obj.setName(ossl.str());
      obj.setAge(20 + i);
      in.append(&obj);
      ossl.str("");
    }

    SQLInstance instance((SqlWrapper *)wrap, std::string("Test.db"),
                         std::string(""), std::string(""), std::string(""));

    instance.InvokeRDB(&stds[0], &in);

    ossl << "TestName5";
    obj.setName(ossl.str());
    obj.setAge(35);
    instance.InvokeRDB(&stds[1], &obj);

    std::string cond = "TestName6";
    obj.setName(cond);
    instance.InvokeRDB(&stds[2], &obj);

    {
      DataContainer out;
      instance.InvokeRDB(&stds[3], &out);
      for (int s = 0; s < out.size(); s++) {
        Student *o = static_cast<Student *>(out[s]);
        printf("%s %d\n", o->getName().get().c_str(), o->getAge().get());
      }
    }

    {
      DataContainer out;
      std::string cond = "TestName5";
      obj.setName(cond);
      instance.InvokeRDB(&stds[4], &obj, &out);
      for (int s = 0; s < out.size(); s++) {
        Student *o = static_cast<Student *>(out[s]);
        printf("%s %d\n", o->getName().get().c_str(), o->getAge().get());
      }
    }

    instance.InvokeRDB(&stds[5]);
  }
  _CrtMemCheckpoint(&s2);
  if (_CrtMemDifference(&s3, &s1, &s2)) {
    _CrtMemDumpStatistics(&s3);
  }
  _CrtDumpMemoryLeaks();

  return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示:
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5.
//   转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
