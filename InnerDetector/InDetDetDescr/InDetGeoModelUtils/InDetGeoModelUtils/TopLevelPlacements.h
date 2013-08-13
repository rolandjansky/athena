/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETGEOMODELUTILS_TOPLEVELPLACEMENTS_H
#define INDETGEOMODELUTILS_TOPLEVELPLACEMENTS_H

#include "CLHEP/Geometry/Transform3D.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include <map>
#include <string>

class IRDBRecord;

class TopLevelPlacements
{
  
public:

  TopLevelPlacements(IRDBRecordset_ptr topLevelTable);
  ~TopLevelPlacements();

  bool  present(const std::string & partName) const;
  const HepGeom::Transform3D & transform(const std::string & partName) const;
  

private:
  
  class Part {
  public:
    std::string label;
    HepGeom::Transform3D transform;
  };

  void fillPlacements(IRDBRecordset_ptr topLevelTable);
  HepGeom::Transform3D partTransform(const IRDBRecord* record) const;
  Part * getPart(const std::string & partName) const;

  std::map<std::string, Part *> m_parts;
  bool m_noTopLevelTable;

  static HepGeom::Transform3D s_identityTransform;

};

#endif // INDETGEOMODELUTILS_TOPLEVELPLACEMENTS_H
