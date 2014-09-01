/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumesSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDetDescrSvc/TrackingVolumesSvc.h"
#include "TrkVolumes/Volume.h"
//#include "TrkVolumes/VolumesDummy.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/EntryLayerProvider.h"
#include "TrkDetDescrSvc/TrackingVolumesSvc.h"
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrUtils/BinnedArray.h"
// GaudiKernel && Control
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
// needed for callback function
#include "EventInfo/TagInfo.h"

/** Constructor **/
Trk::TrackingVolumesSvc::TrackingVolumesSvc(const std::string& name,ISvcLocator* svc) : 
    Service(name,svc),
    m_pDetStore(0),
    m_buildGeometryFromTagInfo(false)
{ 
    m_volumes.reserve(Trk::ITrackingVolumesSvc::NumIdentifiers);
    m_volumeNames.reserve(Trk::ITrackingVolumesSvc::NumIdentifiers);
    
    //set defaults
    m_volumeNames.push_back("CalorimeterEntryLayer");
    m_volumeNames.push_back("MuonSpectrometerEntryLayer");
    m_volumeNames.push_back("MuonSpectrometerExitLayer");
    
    // For the moment, assuming volumes are at 0,0,0 and perfectly aligned (i.e. passing 0)
    // Adding values by hand - this should be changed (i.e. retrieved from a database?) 
    // EJWM
    
    m_volumes[Trk::ITrackingVolumesSvc::CalorimeterEntryLayer] 
        = new Trk::Volume(0, new Trk::CylinderVolumeBounds(1100.0, 3200.0));
    m_volumes[Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer] 
        = new Trk::Volume(0, new Trk::CylinderVolumeBounds(4250.0, 6779.0));
    m_volumes[Trk::ITrackingVolumesSvc::MuonSpectrometerExitLayer]
        = new Trk::Volume(0, new Trk::CylinderVolumeBounds(15000.0, 21000.0)); // FIXME! Put in correct values. EJWM
    
  // the name of the TrackingVolume to be built --------------------------------
  //declareProperty( "VolumeNames",       m_volumeNames, "The names of the TrackingVolume to be built"); 
  // EJWM - no point configuring this if the volume dimensions can't be configured. EJWM. 
  
  // steering && configuration ---------------------------------------------------
  declareProperty( "BuildVolumesFromTagInfo",   m_buildGeometryFromTagInfo);
}


/** Destructor **/
Trk::TrackingVolumesSvc::~TrackingVolumesSvc()
{}

/** Initialize Service */
StatusCode Trk::TrackingVolumesSvc::initialize()
{
  MsgStream log(msgSvc(), name());

  // initialization of the base class
  StatusCode result = Service::initialize();
  if (result.isFailure()) 
  {
    log << MSG::FATAL << "Unable to initialize the service!" << endreq;
    return result;
  }

  // get the DetectorStore
  result = service("DetectorStore", m_pDetStore );  
  if (result.isFailure()) 
  {
    log << MSG::FATAL << "DetectorStore service not found!" << endreq;
    return result;
  }

  // ---------------------------------------------------------------------------------------------------

  if (m_buildGeometryFromTagInfo){
    // register the Callback
    const DataHandle<TagInfo> tagInfoH;
    std::string key = "ESDtags";
    result =  m_pDetStore->regFcn(&ITrackingVolumesSvc::trackingVolumesInit,dynamic_cast<ITrackingVolumesSvc*>(this),tagInfoH,key);
  } else {

      log << MSG::INFO << "Building Geometry at initialisation time." << endreq;

      // call with dummy parameters
      int par1 = 0;
      std::list<std::string> par2;
 
      // build with no dependency on COOL
      result = trackingVolumesInit(par1,par2);
      if (result.isFailure())
      { 
         log << MSG::FATAL << "Unable to build the TrackingVolume!" << endreq;
         return result;
      }
  }

  log << MSG::INFO << "initialize() successful! " << endreq;

  return result;
}


StatusCode Trk::TrackingVolumesSvc::trackingVolumesInit(IOVSVC_CALLBACK_ARGS)
{

  StatusCode result = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), name());
   
  for (unsigned int id=0 ; id!=ITrackingVolumesSvc::NumIdentifiers; id++){
    // create Volume.   
      result = m_pDetStore->record(m_volumes[id], m_volumeNames[id]);
      if (result.isFailure()){
        log << MSG::WARNING << "Couldn't write Volume "<<m_volumeNames[id]<<" to DetectorStore." << endreq;
      } else
        log << MSG::INFO << "initialize() successful: TrackingVolume '" << m_volumeNames[id] << "' built and written to DetectorStore." << endreq;

    }

  return result;
}

/** Finalize Service */
StatusCode Trk::TrackingVolumesSvc::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalize() successful." << endreq;
  return StatusCode::SUCCESS;
}

/** Query the interfaces. */
StatusCode Trk::TrackingVolumesSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_ITrackingVolumesSvc == riid )
    *ppvInterface = (ITrackingVolumesSvc*)this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}




