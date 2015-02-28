/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./DBHelper.h"

#include <iostream>

#include "RelationalAccess/IColumn.h"
#include "RelationalAccess/ITableDescription.h"

void
TrigConf::fillQuery(coral::IQuery *q, coral::AttributeList& attList) {
   q->defineOutput(attList);
   for(coral::AttributeList::iterator attr=attList.begin(); attr!=attList.end(); ++attr)
      q->addToOutputList(attr->specification().name());
}


void
TrigConf::printTable(const coral::ITable & table) {
   const coral::ITableDescription & desc = table.description();
   for(int i=0; i<desc.numberOfColumns(); ++i) {
      const coral::IColumn & col = desc.columnDescription(i);
      std::cout << "   " << col.name() << std::endl;
   }
}
