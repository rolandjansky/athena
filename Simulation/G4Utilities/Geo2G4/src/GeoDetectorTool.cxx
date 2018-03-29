/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "GeoDetectorTool.h"
#include "Geo2G4SvcAccessor.h"
#include "G4AtlasInterfaces/Geo2G4SvcBase.h"
#include "Geo2G4Builder.h"
#include "VolumeBuilder.h"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4GDMLParser.hh"

// Geant4 includes used in functions

GeoDetectorTool::GeoDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent),m_blParamOn(false),m_blGetTopTransform(true),m_geoDetectorName("")
{
  m_topTransform.setIdentity();
  ATH_MSG_DEBUG( "GeoDetectorTool constructor for " << name );
  declareProperty("GeoDetectorName",m_geoDetectorName, "Name of the detector in GeoModel, if different from G4.");
  declareProperty("GDMLFileOut",m_dumpGDMLFile,"File name where the GDML description for the detector will be dumped.");

}

StatusCode GeoDetectorTool::initialize()
{
  ATH_MSG_VERBOSE( name() << " GeoDetectorTool::initialize(): Starting" );
  if(m_detectorName.empty())
    {
      m_detectorName = this->name();
      // re-initialize m_detectorName in order to take the real detector name rather than the path to it
      size_t ipos=m_detectorName.find_last_of(".");
      size_t length=m_detectorName.size();
      if (ipos<length)
        {
          ATH_MSG_VERBOSE( "m_detectorName: " << m_detectorName << " needs to be reset.");
          m_detectorName=m_detectorName.substr(ipos+1,length-ipos-1);
          ATH_MSG_VERBOSE( "m_detectorName default value reset to " << m_detectorName);
        }
    }
  ATH_MSG_DEBUG( name() << "GeoDetectorTool::initialize() : Detector name = " << m_detectorName );
  if(m_geoDetectorName.empty())
    {
      m_geoDetectorName = m_detectorName;
    }
  ATH_MSG_DEBUG( name() << "GeoDetectorTool::initialize() : Geo Detector name = " << m_geoDetectorName );

  Geo2G4SvcAccessor accessor;
  Geo2G4SvcBase *g=accessor.GetGeo2G4Svc();
  m_builderName=g->GetDefaultBuilder()->GetKey();
  m_blGetTopTransform = g->UseTopTransforms();
  ATH_MSG_VERBOSE( name() << " GeoDetectorTool::initialize(): Finished" );
  return StatusCode::SUCCESS;
}


void GeoDetectorTool::BuildGeometry()
{
  ATH_MSG_VERBOSE( name() << " GeoDetectorTool::BuildGeometry(): Starting" );
  G4LogicalVolume* temp = this->Convert();
  m_envelope.theEnvelope=temp;

  if (this->IsTopTransform())
    {
      this->SetInitialTransformation();
    }
  ATH_MSG_VERBOSE( name() << " GeoDetectorTool::BuildGeometry(): Finished" );
}

void GeoDetectorTool::PositionInParent()
{
  ATH_MSG_DEBUG( name() << " GeoDetectorTool::PositionInParent(): Starting" );

  DetectorGeometryBase::PositionInParent();
  if (!m_dumpGDMLFile.empty()) {
    G4GDMLParser parser;
    parser.Write(m_dumpGDMLFile,m_envelope.thePositionedVolume);
  }
}

G4LogicalVolume* GeoDetectorTool::Convert()
{
  ATH_MSG_VERBOSE( name() << " GeoDetectorTool::Convert(): Starting" );
  ATH_MSG_DEBUG( name() << " GeoDetectorTool::Convert(): GeoDetectorName:builderName = "<<m_geoDetectorName<<":"<<m_builderName);
  Geo2G4Builder Builder(m_geoDetectorName);
  VolumeBuilder *b=Builder.GetVolumeBuilder(m_builderName);
  b->SetParam(m_blParamOn);
  G4LogicalVolume *temp=Builder.BuildTree();
  if(this->IsTopTransform())
    {
      m_topTransform = Builder.GetDetectorTransform();
    }

  ATH_MSG_VERBOSE( name() << " GeoDetectorTool::Convert(): Finished" );
  return temp;
}

bool GeoDetectorTool::IsTopTransform()
{
  return m_blGetTopTransform;
}

void GeoDetectorTool::SetInitialTransformation()
{
  ATH_MSG_VERBOSE( name() << " GeoDetectorTool::SetInitialTransformation(): Starting" );
  if (!m_envelope.theRotation)
    {
      ATH_MSG_VERBOSE( name() << " GeoDetectorTool::SetInitialTransformation(): Creating new G4RotationMatrix" );
      m_envelope.theRotation=new G4RotationMatrix;
    }
  *(m_envelope.theRotation)=m_topTransform.getRotation().inverse();
  m_envelope.thePosition=m_topTransform.getTranslation();
  ATH_MSG_VERBOSE( name() << " GeoDetectorTool::SetInitialTransformation(): Finished" );
}
