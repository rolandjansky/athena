/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
#include "TrkDetDescrSvc/TrackingVolumesSvc.h"
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrUtils/BinnedArray.h"
// GaudiKernel && Control
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

/** Constructor **/
Trk::TrackingVolumesSvc::TrackingVolumesSvc(const std::string& a_name,ISvcLocator* svc) : 
    AthService(a_name,svc),
    m_pDetStore("DetectorStore",name())
{ 
    m_volumes.reserve(Trk::ITrackingVolumesSvc::NumIdentifiers);
    m_volumeNames.reserve(Trk::ITrackingVolumesSvc::NumIdentifiers);
    
    //set defaults
    m_volumeNames.emplace_back("CalorimeterEntryLayer");
    m_volumeNames.emplace_back("MuonSpectrometerEntryLayer");
    m_volumeNames.emplace_back("MuonSpectrometerExitLayer");
    
    // For the moment, assuming volumes are at 0,0,0 and perfectly aligned (i.e. passing 0)
    // Adding values by hand - this should be changed (i.e. retrieved from a database?) 
    // EJWM
    
    m_volumes[Trk::ITrackingVolumesSvc::CalorimeterEntryLayer] 
        = new Trk::Volume(nullptr, new Trk::CylinderVolumeBounds(1100.0, 3200.0));
    m_volumes[Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer] 
        = new Trk::Volume(nullptr, new Trk::CylinderVolumeBounds(4250.0, 6779.0));
    m_volumes[Trk::ITrackingVolumesSvc::MuonSpectrometerExitLayer]
        = new Trk::Volume(nullptr, new Trk::CylinderVolumeBounds(15000.0, 21000.0)); // FIXME! Put in correct values. EJWM
    
  // the name of the TrackingVolume to be built --------------------------------
  //declareProperty( "VolumeNames",       m_volumeNames, "The names of the TrackingVolume to be built"); 
  // EJWM - no point configuring this if the volume dimensions can't be configured. EJWM. 
  
}


/** Destructor **/
Trk::TrackingVolumesSvc::~TrackingVolumesSvc()
= default;

/** Initialize Service */
StatusCode Trk::TrackingVolumesSvc::initialize()
{
  ATH_CHECK( AthService::initialize());

  // get the DetectorStore
  ATH_CHECK( m_pDetStore.retrieve() );

  // record the volumes
  for (unsigned int id=0 ; id!=ITrackingVolumesSvc::NumIdentifiers; id++){
    // create Volume.   
    StatusCode result = m_pDetStore->record(m_volumes[id], m_volumeNames[id]);
    if (result.isFailure()){
      ATH_MSG_FATAL ( "Couldn't write Volume "<<m_volumeNames[id]<<" to DetectorStore."  );
      return result;
    } else {
      ATH_MSG_INFO ( "initialize() successful: TrackingVolume '" << m_volumeNames[id] << "' built and written to DetectorStore."  );
    }    
  }

  ATH_MSG_INFO ( "initialize() successful! "  );

  return StatusCode::SUCCESS;
}

/** Finalize Service */
StatusCode Trk::TrackingVolumesSvc::finalize()
{
  ATH_MSG_INFO ( "finalize() successful."  );
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




