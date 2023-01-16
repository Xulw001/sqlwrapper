#pragma once
#include "framework/row.h"

namespace sqlwrapper
{
	DECL_DATA_MODEL_BEGIN(Student)
		DEF_DATA_MODEL(Name, String)
		DEF_DATA_MODEL(Age, Integer)
	DECL_DATA_MODEL_END
}