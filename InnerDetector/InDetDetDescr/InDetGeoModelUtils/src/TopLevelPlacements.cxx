/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetGeoModelUtils/TopLevelPlacements.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"




HepGeom::Transform3D TopLevelPlacements::s_identityTransform = HepGeom::Transform3D();


TopLevelPlacements::TopLevelPlacements(IRDBRecordset_ptr topLevelTable)
  : m_noTopLevelTable(true)
{
  fillPlacements(topLevelTable);
}

TopLevelPlacements::~TopLevelPlacements() 
{
  std::map<std::string, Part *>::const_iterator iter;
  for (iter=m_parts.begin(); iter != m_parts.end(); ++iter) delete iter->second;
}

const HepGeom::Transform3D & 
TopLevelPlacements::transform(const std::string & partName) const 
{
  Part * part = getPart(partName);
  if (part) return part->transform;
  return s_identityTransform;
}


bool 
TopLevelPlacements::present(const std::string & partName) const
{
  // If no table present assume everything is present.
  if (m_noTopLevelTable) return true;
  return (getPart(partName) != 0);
}

void
TopLevelPlacements::fillPlacements(IRDBRecordset_ptr topLevelTable)
{
  if (topLevelTable.get() == 0) {
     m_noTopLevelTable = true;
     return;
  }
  m_noTopLevelTable = false;
  int numParts = topLevelTable->size();
  for (int i = 0; i < numParts; i++) {
    const IRDBRecord * record = (*topLevelTable)[i];
    std::string label =  record->getString("LABEL");

    Part * part = new Part;
    part->label = label;
    part->transform = partTransform(record);
     
    m_parts[label] = part;
    
  }
      
}


HepGeom::Transform3D 
TopLevelPlacements::partTransform(const IRDBRecord* record) const
{

  double posX =  record->getDouble("POSX") * CLHEP::mm;
  double posY =  record->getDouble("POSY") * CLHEP::mm;
  double posZ =  record->getDouble("POSZ") * CLHEP::mm;
  double rotX =  record->getDouble("ROTX") * CLHEP::degree;
  double rotY =  record->getDouble("ROTY") * CLHEP::degree;
  double rotZ =  record->getDouble("ROTZ") * CLHEP::degree;
  int rotOrder = record->getInt("ROTORDER");

  // Translation part
  HepGeom::Transform3D transform = HepGeom::Translate3D(posX,posY,posZ);

  // If rotation is zero return translation
  if (rotX == 0 && rotY == 0 && rotZ == 0) {
    return transform;
  }

  // For rotation have to look at order. 
  // 123 means rotate around X, then Y , then Z.
  // 312  means rotate around Z, then X , then Y.
  // etc

  int ixyz1 = rotOrder/100 - 1;
  int ixyz2 = (rotOrder%100) / 10 - 1;
  int ixyz3 = (rotOrder%10) - 1 ;

  if (ixyz1 < 0 || ixyz1 > 2 || 
      ixyz2 < 0 || ixyz2 > 2 ||
      ixyz3 < 0 || ixyz3 > 2) {
    std::cout << "ERROR: Invalid rotation order:" << rotOrder << std::endl;
    ixyz1 = 0;
    ixyz2 = 1;
    ixyz3 = 2;
  }
    
  // List of the three transforms
  HepGeom::Transform3D * xformList[3] = {0, 0, 0};
  if (rotX != 0) xformList[0] = new HepGeom::RotateX3D(rotX);
  if (rotY != 0) xformList[1] = new HepGeom::RotateY3D(rotY);
  if (rotZ != 0) xformList[2] = new HepGeom::RotateZ3D(rotZ);

  HepGeom::Transform3D rotation;
  if (xformList[ixyz1]) rotation = *(xformList[ixyz1]) * rotation;
  if (xformList[ixyz2]) rotation = *(xformList[ixyz2]) * rotation;
  if (xformList[ixyz3]) rotation = *(xformList[ixyz3]) * rotation;

  delete xformList[0];
  delete xformList[1];
  delete xformList[2];

  return transform * rotation;

}


TopLevelPlacements::Part * 
TopLevelPlacements::getPart(const std::string & partName) const 
{
  std::map<std::string, Part *>::const_iterator iter;
  iter = m_parts.find(partName);
  if (iter == m_parts.end()) return 0;
  return iter->second;
}


