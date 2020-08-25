/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "GDMLDetectorTool.h"

#include "G4GDMLParser.hh"
#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

// Geant4 includes used in functions

GDMLDetectorTool::GDMLDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent),m_GDMLFileName(""),m_blGetTopTransform(true),m_geoDetectorName("")
{
  m_topTransform.setIdentity();
  ATH_MSG_DEBUG( "GDMLDetectorTool constructor for " << name );
  declareProperty("GeoDetectorName",m_geoDetectorName, "Name of the detector in GeoModel, if different from G4.");
  declareProperty("GDMLFileName",m_GDMLFileName,"Name of the GDML file to be used as input.");

}

StatusCode GDMLDetectorTool::initialize()
{
  ATH_MSG_DEBUG( name() << " GDMLDetectorTool::initialize(): Starting" );

  if(m_detectorName.empty())
    {
      m_detectorName = this->name();
      // re-initialize m_detectorName in order to take the real detector name rather than the path to it
      size_t ipos=m_detectorName.value().find_last_of(".");
      size_t length=m_detectorName.value().size();
      if (ipos<length)
        {
          ATH_MSG_DEBUG( "m_detectorName: " << m_detectorName.value() << " needs to be reset.");
          m_detectorName=m_detectorName.value().substr(ipos+1,length-ipos-1);
          ATH_MSG_DEBUG( "m_detectorName default value reset to " << m_detectorName.value());
        }
    }
  ATH_MSG_DEBUG( name() << "GDMLDetectorTool::initialize() : Detector name = " << m_detectorName.value()<<" File name: "<<m_GDMLFileName );
  if(m_geoDetectorName.empty())
    {
      m_geoDetectorName = m_detectorName.value();
    }
  if(m_GDMLFileName.empty())
    {
          m_GDMLFileName = m_detectorName.value()+".gdml";
    }
  ATH_MSG_DEBUG( name() << "GDMLDetectorTool::initialize() : Geo Detector name = " << m_geoDetectorName<<" File name: "<<m_GDMLFileName );

  ATH_MSG_DEBUG( name() << " GDMLDetectorTool::initialize(): Finished" );
  return StatusCode::SUCCESS;
}


void GDMLDetectorTool::BuildGeometry()
{
  ATH_MSG_VERBOSE( name() << " GDMLDetectorTool::BuildGeometry(): Starting" );

  G4GDMLParser parser;
  parser.Read(m_GDMLFileName.c_str(),false);
  m_envelope.theEnvelope=parser.GetWorldVolume()->GetLogicalVolume();

  ATH_MSG_VERBOSE( name() << " GDMLDetectorTool::BuildGeometry(): Finished" );
}

bool GDMLDetectorTool::IsTopTransform()
{
  return m_blGetTopTransform;
}

void GDMLDetectorTool::SetInitialTransformation()
{
  ATH_MSG_VERBOSE( name() << " GDMLDetectorTool::SetInitialTransformation(): Starting" );
  if (!m_envelope.theRotation)
    {
      ATH_MSG_VERBOSE( name() << " GDMLDetectorTool::SetInitialTransformation(): Creating new G4RotationMatrix" );
      m_envelope.theRotation=new G4RotationMatrix;
    }
  *(m_envelope.theRotation)=m_topTransform.getRotation().inverse();
  m_envelope.thePosition=m_topTransform.getTranslation();
  ATH_MSG_VERBOSE( name() << " GDMLDetectorTool::SetInitialTransformation(): Finished" );
}
