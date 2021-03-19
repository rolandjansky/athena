/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Technology.h"

#include "MuonGeoModel/MYSQL.h"

namespace MuonGM {

    std::string Technology::GetName() { return m_name; }

    Technology::Technology(std::string s) : m_name(s), thickness(0.) {
        MYSQL *mysql = MYSQL::GetPointer();
        mysql->StoreTechnology(this);
    }

    Technology::~Technology() {}

} // namespace MuonGM
