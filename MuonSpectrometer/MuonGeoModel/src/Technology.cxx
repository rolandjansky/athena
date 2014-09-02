/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Technology.h"
#include "MuonGeoModel/MYSQL.h"

namespace MuonGM {

std::string Technology::GetName()
{
	return name;
}

Technology::Technology(std::string s): name(s)
{
	MYSQL *mysql=MYSQL::GetPointer();
        mysql->StoreTechnology(this);
}

Technology::~Technology()
{
}
} // namespace MuonGM
