/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_DBHelper
#define TrigConf_DBHelper

#include <CoralBase/Attribute.h>
#include <CoralBase/AttributeList.h>

#include "RelationalAccess/SchemaException.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ICursor.h"
#include "RelationalAccess/IQuery.h"

#include "boost/lexical_cast.hpp"

#include <string>

namespace TrigConf {

   void fillQuery(coral::IQuery *q, coral::AttributeList& attList);

   inline std::string rmtilde(const std::string & input) { return input=="~" ? "" : input; }

   void printTable(const coral::ITable & teTable);

}

#endif
