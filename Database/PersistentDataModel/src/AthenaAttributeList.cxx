/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PersistentDataModel/AthenaAttributeList.h"

AthenaAttributeList::AthenaAttributeList() : coral::AttributeList()
{}

AthenaAttributeList::AthenaAttributeList(const coral::AttributeList& rhs) : coral::AttributeList(rhs)
{}

AthenaAttributeList::AthenaAttributeList(const coral::AttributeListSpecification& spec) : coral::AttributeList(spec)
{}

void AthenaAttributeList::print(std::ostream& os) const {
  os << "{";
  for (coral::AttributeList::const_iterator itr=this->begin();
      itr!=this->end();++itr) {
    if (itr!=this->begin()) os << ",";
    itr->toOutputStream(os);
  }
  os << "}";
}
