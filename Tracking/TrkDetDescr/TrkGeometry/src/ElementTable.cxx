/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ElementTable.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkGeometry/ElementTable.h"

#include <iomanip>

#include "GaudiKernel/MsgStream.h"

Trk::ElementTable::~ElementTable() {
  auto elIter = m_table.begin();
  for (; elIter != m_table.end(); delete (*elIter), ++elIter)
    ;
}

MsgStream& Trk::operator<<(MsgStream& sl, const Trk::ElementTable& etab) {
  std::string none = "None";
  sl << " ------------------------- Trk::ElementTable "
        "-------------------------- "
     << std::endl;
  sl << " - Listing " << etab.size() << " elements " << std::endl;
  for (unsigned int iel = 0; iel < static_cast<unsigned int>(UCHAR_MAX); ++iel)
    sl << " - Id : " << std::setw(3) << iel << " - Properties: "
       << (etab.element(iel) ? etab.element(iel)->toString() : none)
       << " - Name : " << etab.elementName(iel) << std::endl;
  sl << " ---------------------------------------------------------------------"
        "- "
     << endmsg;
  return sl;
}

std::ostream& Trk::operator<<(std::ostream& sl, const Trk::ElementTable& etab) {
  std::string none = "None";
  sl << " ------------------------- Trk::ElementTable "
        "-------------------------- "
     << std::endl;
  sl << " - Listing " << etab.size() << " elements " << std::endl;
  for (unsigned int iel = 0; iel < static_cast<unsigned int>(UCHAR_MAX); ++iel)
    sl << " - Id : " << std::setw(3) << iel << " - Properties: "
       << (etab.element(iel) ? etab.element(iel)->toString() : none)
       << " - Name : " << etab.elementName(iel) << std::endl;
  sl << " ---------------------------------------------------------------------"
        "- "
     << std::endl;
  return sl;
}

