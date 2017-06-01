/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"

// Geant4 includes used in functions
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"

DetectorGeometryBase::DetectorGeometryBase(const std::string& type, const std::string& name, const IInterface* parent)
  : AthAlgTool(type,name,parent),
    m_subDetTools(this),
    m_notifierSvc("G4GeometryNotifierSvc", name),
    m_theParent(nullptr),
    m_detectorName(""),
    m_isWorld(0),
    m_rotateX(0.0),
    m_rotateY(0.0),
    m_rotateZ(0.0),
    m_offsetX(0.0),
    m_offsetY(0.0),
    m_offsetZ(0.0)
{
  ATH_MSG_DEBUG( "DetectorGeometryBase Constructor for " << name );
  declareProperty( "GeometryNotifierSvc", m_notifierSvc, "");
  declareProperty( "SubDetectors" , m_subDetTools , "Tool handle array of all subdetector tools" );
  declareProperty( "DetectorName" , m_detectorName , "Detector name (same as the Tool name if not set");
  declareProperty( "RotateX" , m_rotateX , "Rotation around the X-axis");
  declareProperty( "RotateY" , m_rotateY , "Rotation around the Y-axis");
  declareProperty( "RotateZ" , m_rotateZ , "Rotation around the Z-axis");
  declareProperty( "OffsetX" , m_offsetX , "Offset in the X-direction");
  declareProperty( "OffsetY" , m_offsetY , "Offset in the Y-direction");
  declareProperty( "OffsetZ" , m_offsetZ , "Offset in the Z-direction");
}

// Athena method, called at initialization time
StatusCode DetectorGeometryBase::initialize()
{
  ATH_MSG_VERBOSE( name() << "::initialize(): starting." );
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
  ATH_MSG_DEBUG( name() << "::initialize() (Base class method): Detector name = " << m_detectorName );
  CHECK(m_notifierSvc.retrieve());

  //  This fires initialize() for each of those tools
  if (m_subDetTools.size())
    {
      ATH_MSG_DEBUG( name() << "::initialize(): Initializing list of " << m_subDetTools.size() << " detectors." );
      CHECK( m_subDetTools.retrieve() );
    }
  else
    {
      ATH_MSG_DEBUG( name() << "::initialize(): no sub-detectors to initialize." );
    }

  ATH_MSG_VERBOSE( name() << "::initialize(): finished." );
  return StatusCode::SUCCESS;
}

void DetectorGeometryBase::Build()
{
  ATH_MSG_VERBOSE( name() << "::Build() (Base class method): Starting" );
  SetEnvelope();
  m_notifierSvc->SetCurrentDetectorName(m_detectorName);
  BuildGeometry();
  SetRotationAndOffset();
  PositionInParent();
  BuildSubDetectors();
  ATH_MSG_VERBOSE( name() << "::Build() (Base class method): Finished" );
  return;
}

void DetectorGeometryBase::BuildGeometry()
{
  ATH_MSG_VERBOSE( "DetectorGeometryBase::BuildGeometry(): Using base-class method. Anything going wrong?");
}

void DetectorGeometryBase::SetRotationAndOffset()
{
  ATH_MSG_VERBOSE( name() << "::SetRotationAndOffset() (Base class method)");
  // Firstly do the rotation
  if (!m_envelope.theRotation)
    {
      // m_envelope.theRotation is null, so create an identity
      // rotation first.
      // FIXME probably a neater way to do this part.
      m_envelope.theRotation=new G4RotationMatrix;
      // add the extra rotations.
      m_envelope.theRotation->rotateX(m_rotateX);
      m_envelope.theRotation->rotateY(m_rotateY);
      m_envelope.theRotation->rotateZ(m_rotateZ);
      if (m_envelope.thePositionedVolume)
        {
          // Override the rotation for m_envelope.thePositionedVolume.
          m_envelope.thePositionedVolume->SetRotation(m_envelope.theRotation);
        }
    }
  else
    {
      // m_envelope.theRotation already exists, so just add the
      // extra rotations.
      m_envelope.theRotation->rotateX(m_rotateX);
      m_envelope.theRotation->rotateY(m_rotateY);
      m_envelope.theRotation->rotateZ(m_rotateZ);
    }
  // Secondly add the additional position offset to the existing
  // m_envelope.thePosition vector.
  m_envelope.thePosition+=G4ThreeVector(m_offsetX,m_offsetY,m_offsetZ);
  if (m_envelope.thePositionedVolume)
    {
      // Override the translation for m_envelope.thePositionedVolume.
      m_envelope.thePositionedVolume->SetTranslation(m_envelope.thePosition);
    }

  ATH_MSG_VERBOSE( name() << "::SetRotationAndOffset() (Base class method): Finished" );
  return;
}
void DetectorGeometryBase::PositionInParent()
{
  ATH_MSG_VERBOSE( name() << "::PositionInParent() (Base class method)");
  if (m_isWorld)
    {
      // check that the detector is built
      if (m_envelope.IsBuilt())
        {
          G4VPhysicalVolume* physWorld= new G4PVPlacement(0,G4ThreeVector(),
                                                          m_envelope.theEnvelope,m_envelope.theEnvelope->GetName(),0,false,0,false);
          m_envelope.thePositionedVolume=physWorld;
        }
    }
  else
    {
      // check that there is a parent
      if (!m_theParent)
        {
          ATH_MSG_ERROR("Parent not set for "<<m_detectorName<<"!!!!!!!!!!");
        }
      else
        {
          if (m_theParent->GetEnvelope().IsBuilt())
            {
              // G4VPhysicalVolume *physVol = new G4PVPlacement(0,G4ThreeVector(),
              //                                                m_envelope.theEnvelope,m_envelope.theEnvelope->GetName(),m_theParent->GetEnvelope().theEnvelope,false,0,false);
              // TODO: implement a possible rotation/displacement - something like this based on the old code?
              G4VPhysicalVolume *physVol = new G4PVPlacement(m_envelope.theRotation,m_envelope.thePosition,
                                                             m_envelope.theEnvelope,m_envelope.theEnvelope->GetName(),m_theParent->GetEnvelope().theEnvelope,false,0);
              m_envelope.thePositionedVolume=physVol;
            }
        }
    }
}

void DetectorGeometryBase::BuildSubDetectors()
{
  ATH_MSG_VERBOSE( name() << "::BuildSubDetectors() (Base class method): Starting");
  for (auto& subDetTool: m_subDetTools)
    {
      ATH_MSG_VERBOSE(name() << "::BuildSubDetectors() (Base class method):  Positioning "<<subDetTool->GetDetectorName()<<" within "<<m_detectorName);
      subDetTool->SetParent(this);
      subDetTool->Build();
    }
  ATH_MSG_VERBOSE( name() << "::BuildSubDetectors() (Base class method): Finished");
}

void DetectorGeometryBase::SetEnvelope()
{
}

void DetectorGeometryBase::ResetEnvelope()
{
}

void DetectorGeometryBase::SetDetectorName(const std::string s)
{
  m_detectorName=s;
}

std::string DetectorGeometryBase::GetDetectorName() const
{
  return m_detectorName;
}

void DetectorGeometryBase::SetAsWorld()
{
  m_isWorld=true;
}

void DetectorGeometryBase::SetParent(IDetectorGeometryTool* p)
{
  m_theParent=p;
}

Envelope& DetectorGeometryBase::GetEnvelope()
{
  return m_envelope;
}

G4VPhysicalVolume* DetectorGeometryBase::GetWorldVolume()
{
  if (m_isWorld)
    return m_envelope.thePositionedVolume;
  else
    {
      ATH_MSG_ERROR("trying to get World from a DetectorTool which World is not!");
      return 0;
    }
}

StatusCode
DetectorGeometryBase::queryInterface(const InterfaceID& riid, void** ppvIf)
{
  if ( riid == IDetectorGeometryTool::interfaceID() )
    {
      *ppvIf = (IDetectorGeometryTool*)this;
      addRef();
      return StatusCode::SUCCESS;
    }
  return AlgTool::queryInterface( riid, ppvIf );
}
