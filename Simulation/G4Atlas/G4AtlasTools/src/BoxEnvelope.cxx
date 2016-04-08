/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  : DetectorGeometryBase(type,name,parent),m_materialName("Air"),m_numberOfHoles(0)
{
  ATH_MSG_VERBOSE( "BoxEnvelope constructor for " << name );
  declareProperty("dX",m_dX=0.,"Envelope dX");
  declareProperty("dY",m_dY=0.,"Envelope dY");
  declareProperty("dZ",m_dZ=0.,"Envelope dZ");
  declareProperty("Material",m_materialName,"Envelope Material");
  declareProperty("NumberOfHoles",m_numberOfHoles,"Number of hole volumes to subtract from the initial envelope volume.");
  declareProperty("HoleNames", m_holeNames,"");
  declareProperty("Hole_dX",   m_hole_dX,"");
  declareProperty("Hole_dY",   m_hole_dY,"");
  declareProperty("Hole_dZ",   m_hole_dZ,"");
  declareProperty("HolePosX",  m_holePosX,"");
  declareProperty("HolePosY",  m_holePosY,"");
  declareProperty("HolePosZ",  m_holePosZ,"");
}

// StatusCode BoxEnvelope::initialize()
// {
//   return StatusCode::SUCCESS;
// }


void BoxEnvelope::BuildGeometry()
{
  ATH_MSG_VERBOSE( "BoxEnvelope::BuildGeometry: "<<m_dX<<" "<<m_dY<<" "<<m_dZ );

  // Get nist material manager
  G4NistManager *nist = G4NistManager::Instance();

  G4Material *mat = nist->FindOrBuildMaterial(m_materialName);

  G4VSolid *solid = new G4Box(m_detectorName,m_dX,m_dY,m_dZ);
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
          std::string n=m_holeNames.at(nSub);
          G4VSolid *temp=new G4Box("",m_hole_dX.at(nSub), m_hole_dY.at(nSub), m_hole_dZ.at(nSub));
          G4ThreeVector tPos=G4ThreeVector(m_holePosX.at(nSub), m_holePosY.at(nSub), m_holePosZ.at(nSub));
          solid=new G4SubtractionSolid(m_detectorName,solid,temp,0,tPos);
        }
    }

  G4LogicalVolume* logicWorld = new G4LogicalVolume(solid,mat,m_detectorName);

  m_envelope.theEnvelope=logicWorld;
}
