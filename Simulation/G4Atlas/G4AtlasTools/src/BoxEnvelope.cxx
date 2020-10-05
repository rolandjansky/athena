/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasTools/BoxEnvelope.h"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"

// Geant4 includes used in functions

BoxEnvelope::BoxEnvelope(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent)
{
}

void BoxEnvelope::BuildGeometry()
{
  ATH_MSG_VERBOSE( "BoxEnvelope::BuildGeometry: Name " << this->name() );
  ATH_MSG_VERBOSE( "BoxEnvelope::BuildGeometry: "<<m_dX<<" "<<m_dY<<" "<<m_dZ );

  // Get nist material manager
  G4NistManager *nist = G4NistManager::Instance();

  G4Material *mat = nist->FindOrBuildMaterial(m_materialName.value());

  G4VSolid *solid = new G4Box(m_detectorName.value(),m_dX,m_dY,m_dZ);
  if (m_numberOfHoles)
    {
      if(m_holeNames.size() != m_numberOfHoles)
        {
          ATH_MSG_ERROR("Size of m_holeNames("<<m_holeNames.size()<<") differs from no. of holes "<<m_numberOfHoles<<"!!!");
        }
      if(m_hole_dX.size() != m_numberOfHoles)
        {
          ATH_MSG_ERROR("Size of m_hole_dX("<<m_hole_dX.size()<<") differs from no. of holes "<<m_numberOfHoles<<"!!!");
        }
      if(m_hole_dY.size() != m_numberOfHoles)
        {
          ATH_MSG_ERROR("Size of m_hole_dY("<<m_hole_dY.size()<<") differs from no. of holes "<<m_numberOfHoles<<"!!!");
        }
      if(m_hole_dZ.size() != m_numberOfHoles)
        {
          ATH_MSG_ERROR("Size of m_hole_dZ("<<m_hole_dZ.size()<<") differs from no. of holes "<<m_numberOfHoles<<"!!!");
        }
      if(m_holePosX.size() != m_numberOfHoles)
        {
          ATH_MSG_ERROR("Size of m_holePosX("<<m_holePosX.size()<<") differs from no. of holes "<<m_numberOfHoles<<"!!!");
        }
      if(m_holePosY.size() != m_numberOfHoles)
        {
          ATH_MSG_ERROR("Size of m_holePosY("<<m_holePosY.size()<<") differs from no. of holes "<<m_numberOfHoles<<"!!!");
        }
      if(m_holePosZ.size() != m_numberOfHoles)
        {
          ATH_MSG_ERROR("Size of m_holePosZ("<<m_holePosZ.size()<<") differs from no. of holes "<<m_numberOfHoles<<"!!!");
        }
      for(unsigned int nSub(0); nSub<m_numberOfHoles; ++nSub)
        {
          G4ThreeVector size=G4ThreeVector();
          std::string n=m_holeNames.value().at(nSub);
          G4VSolid *temp=new G4Box("",m_hole_dX.value().at(nSub), m_hole_dY.value().at(nSub), m_hole_dZ.value().at(nSub));
          G4ThreeVector tPos=G4ThreeVector(m_holePosX.value().at(nSub), m_holePosY.value().at(nSub), m_holePosZ.value().at(nSub));
          solid=new G4SubtractionSolid(m_detectorName.value(),solid,temp,0,tPos);
        }
    }

  G4LogicalVolume* logicWorld = new G4LogicalVolume(solid,mat,m_detectorName.value());

  m_envelope.theEnvelope=logicWorld;
}
