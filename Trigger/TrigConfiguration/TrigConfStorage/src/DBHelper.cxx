/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./DBHelper.h"

void
TrigConf::fillQuery(coral::IQuery *q, coral::AttributeList& attList) {
   q->defineOutput(attList);
   for(coral::AttributeList::iterator attr=attList.begin(); attr!=attList.end(); ++attr)
      q->addToOutputList(attr->specification().name());
}

