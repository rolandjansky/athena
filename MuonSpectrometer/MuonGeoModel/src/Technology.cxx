/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "MuonGeoModel/Technology.h"

#include "MuonGeoModel/MYSQL.h"

namespace MuonGM {

    std::string Technology::GetName() { return m_name; }

    Technology::Technology(std::string s) : m_name(std::move(s)), thickness(0.) {
        MYSQL *mysql = MYSQL::GetPointer();
        mysql->StoreTechnology(this);
    }

    Technology::~Technology() {}

} // namespace MuonGM
