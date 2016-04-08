/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingGeometrySvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkDetDescrSvc/TrackingGeometrySvc.h"
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
#include "TrkDetDescrUtils/BinnedArray.h"
// GaudiKernel && Control
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
// needed for callback function
#include "EventInfo/TagInfo.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

// monitor memory usage
#ifdef TRKDETDESCR_MEMUSAGE   
#include <unistd.h>
#endif

/** Constructor **/
Trk::TrackingGeometrySvc::TrackingGeometrySvc(const std::string& name,ISvcLocator* svc) : 
    AthService(name,svc),
    m_pSvcLocator(0),
    m_pDetStore(0),
    m_trackingGeometryBuilder(""),
    m_trackingGeometry(0),
    m_trackingGeometryName("AtlasTrackingGeometry"),
    m_geometryProcessors(),
#ifdef TRKDETDESCR_MEMUSAGE   
    m_memoryLogger(),
    m_changeVsize(0.),
    m_changeRss(0.),
#endif        
    m_callbackStringForced(false),
    m_callbackString(""),
    m_callbackStringCheck(true),
    m_rerunOnCallback(false),
    m_buildGeometryFromTagInfo(true)
{ 
  // the geometry builder --------------------------------------------------------
  declareProperty( "GeometryBuilder",              m_trackingGeometryBuilder);
  // the name of the TrackingGeometry to be built --------------------------------
  declareProperty( "TrackingGeometryName",         m_trackingGeometryName);
  // geometry processors to validation / distort the TrackingGeometry ------------
  declareProperty( "GeometryProcessors",           m_geometryProcessors);
  // steering && configuration ---------------------------------------------------
  declareProperty( "BuildGeometryFromTagInfo",     m_buildGeometryFromTagInfo);
  // callback string steering -----------------------------------------------------
  declareProperty( "CallbackStringForced",         m_callbackStringForced);
  declareProperty( "CallbackString",               m_callbackString);
  declareProperty( "CallbackStringCheck",          m_callbackStringCheck);
  declareProperty( "RerunOnCallback",              m_rerunOnCallback);  
}


/** Destructor **/
Trk::TrackingGeometrySvc::~TrackingGeometrySvc()
{}

/** Initialize Service */
StatusCode Trk::TrackingGeometrySvc::initialize()
{
  if (m_geometryProcessors.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_geometryProcessors );
      return StatusCode::FAILURE;
  }

  // get the DetectorStore
  if (service("DetectorStore", m_pDetStore ).isFailure()) 
  {
    ATH_MSG_FATAL( "DetectorStore service not found!" );
    return StatusCode::FAILURE;
  }

  // get the key -- from StoreGate (DetectorStore)
  std::vector< std::string > tagInfoKeys = m_pDetStore->keys<TagInfo> ();
  std::string tagInfoKey = "";

  if(tagInfoKeys.size()==0)   ATH_MSG_WARNING( " No TagInfo keys in DetectorStore ");
  else {
    if(tagInfoKeys.size() > 1) ATH_MSG_WARNING( " More than one TagInfo key in the DetectorStore, using the first one " );
    tagInfoKey = tagInfoKeys[0];
  }

  if (!m_callbackStringForced) m_callbackString = tagInfoKey;
        
  if (m_buildGeometryFromTagInfo){
    // register the Callback
    const DataHandle<TagInfo> tagInfoH;
    if ( (m_pDetStore->regFcn(&ITrackingGeometrySvc::trackingGeometryInit,
        dynamic_cast<ITrackingGeometrySvc*>(this),tagInfoH,m_callbackString)).isFailure() ){
      ATH_MSG_WARNING( "Unable to register regFcn callback from DetectorStore" );
    } 

    // If GeoModelSvc has been configured to pick up geometry version automatically 
    // we just need to make sure that trackingGeometryInit gets called after IGeoModelSvc::geoInit
    const IGeoModelSvc *geoModel=0;
    if(service("GeoModelSvc", geoModel).isFailure()) {
      ATH_MSG_ERROR( "Could not locate GeoModelSvc" );
      return StatusCode::FAILURE;
    }
    if (!geoModel->geoInitialized() && 
       (m_pDetStore->regFcn(&IGeoModelSvc::geoInit, geoModel, &ITrackingGeometrySvc::trackingGeometryInit,
                           dynamic_cast<ITrackingGeometrySvc*>(this))).isFailure()){
      ATH_MSG_WARNING( "Unable to register trackingGeometryInit callback after IGeoModelSvc::geoInit" );
    }
  } else {
    ATH_MSG_INFO( "Building Geometry at initialisation time." );
    // call with dummy parameters
    int par1 = 0;
    std::list<std::string> par2;
    // build with no dependency on COOL
    if (trackingGeometryInit(par1,par2).isFailure()) { 
      ATH_MSG_FATAL( "Unable to build the TrackingGeometry!" );
      return StatusCode::FAILURE;
    }
  }
  ATH_MSG_INFO( "initialize() successful! " );
  return StatusCode::SUCCESS;
}


StatusCode Trk::TrackingGeometrySvc::trackingGeometryInit(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
    // Retrieve the tracking geometry builder tool   ----------------------------------------------------    
    if (!m_trackingGeometryBuilder.empty() && m_trackingGeometryBuilder.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Failed to retrieve tool '" << m_trackingGeometryBuilder << "'. Aborting." );
        return StatusCode::FAILURE;
    } else if (m_trackingGeometryBuilder.empty()){
        ATH_MSG_FATAL( "TrackingGeometrySvc " << name() << " not properly configured. No GeometryBuilder defined. Aborting." );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_DEBUG( "Retrieved tool " << m_trackingGeometryBuilder );

    // nothing to do in this case since rerun is switched off 
    if (m_pDetStore->contains<Trk::TrackingGeometry>(m_trackingGeometryName) && !m_rerunOnCallback) 
        return StatusCode::SUCCESS;

    // avoid warning of unused parameter
    (void) I;

    bool needsInit = false;

    if (m_callbackStringCheck) {
        // check if the string is ESD for guaranteeing that misalignment has been introduced already
        for (std::list<std::string>::const_iterator itr=keys.begin(); itr!=keys.end(); ++itr) {
            if (*itr == m_callbackString) {
                needsInit = true; break;
            }
        }
    }

    // only build if the callback string was true
    if (needsInit || !m_callbackStringCheck) {
        // cleanup the geometry if you have one 
        // (will delete what is in detector store, because new one will overwrite old one)
        delete m_trackingGeometry; m_trackingGeometry = 0;
    
#ifdef TRKDETDESCR_MEMUSAGE           
        // memory monitoring    
        ATH_MSG_INFO( "[ memory usage ] Start to monitor memory usage of PID: " << getpid() );

        m_memoryLogger.refresh(getpid());    
        ATH_MSG_INFO( m_memoryLogger );
        // record the difference
        m_changeVsize = (-1.)*m_memoryLogger.vmSize();
        m_changeRss   = (-1.)*m_memoryLogger.vmRss();    
#endif

       // build the TrackingGeometry from the builder
       m_trackingGeometry = m_trackingGeometryBuilder->trackingGeometry();
    
#ifdef TRKDETDESCR_MEMUSAGE      
       ATH_MSG_INFO( "[ memory usage ] TrackingGeometry retrieved: "  );
       m_memoryLogger.refresh(getpid());    
       ATH_MSG_INFO( m_memoryLogger );                    
#endif    
      
      // cast constness away for StoreGate
      Trk::TrackingGeometry* atlasTrackingGeometry = const_cast<Trk::TrackingGeometry*>(m_trackingGeometry);

      // check if a second call back has occured
      if ( m_pDetStore->contains<Trk::TrackingGeometry>(m_trackingGeometryName) ) {
          ATH_MSG_VERBOSE( "New Callback evoked remove of existing object!" ); 
          // you need to retrieve the object first to remove it
          const Trk::TrackingGeometry* oldTrackingGeometry = 0;
          if ( m_pDetStore->retrieve(oldTrackingGeometry,m_trackingGeometryName).isFailure() )
              ATH_MSG_WARNING( "Callback evoked remove of '" << m_trackingGeometryName << "'. But retrieve did not succeed! " );
          if (oldTrackingGeometry &&  m_pDetStore->remove<Trk::TrackingGeometry>(oldTrackingGeometry).isFailure() )
              ATH_MSG_WARNING( "Callback evoked remove of '" << m_trackingGeometryName << "'. But it did not succeed!" );
          // delete is safe
          delete oldTrackingGeometry;
      }
      
      // loop over the recursive geometry processors
      ToolHandleArray<Trk::IGeometryProcessor>::iterator gpIter  = m_geometryProcessors.begin();
      ToolHandleArray<Trk::IGeometryProcessor>::iterator gpIterE = m_geometryProcessors.end();
      for ( ; gpIter != gpIterE; ++ gpIter ){
          if ( (*gpIter)->process(*atlasTrackingGeometry).isFailure() ){
              ATH_MSG_FATAL("Processing of TrackingGeometry did not succeed. Abort." );
              return StatusCode::FAILURE;
          } else {
              ATH_MSG_VERBOSE("Successfully processed the TrackingGeometry with " << (*gpIter) );
#ifdef TRKDETDESCR_MEMUSAGE      
              ATH_MSG_INFO( "[ memory usage ] After processing with " << (*gpIter) );
              m_memoryLogger.refresh(getpid());    
              ATH_MSG_INFO( m_memoryLogger );                    
#endif            
          }
      }
    
      // record the resulting TrackingGeometry 
      if (m_pDetStore->record(atlasTrackingGeometry, m_trackingGeometryName, false).isFailure() ){
         ATH_MSG_WARNING( "Couldn't write TrackingGeometry to DetectorStore." );
      } else
        ATH_MSG_DEBUG( "initialize() successful: TrackingGeometry '" << m_trackingGeometryName << "' built and written to DetectorStore." );
    }
    return StatusCode::SUCCESS;
}


/** Finalize Service */
StatusCode Trk::TrackingGeometrySvc::finalize()
{

#ifdef TRKDETDESCR_MEMUSAGE
    ATH_MSG_INFO( "[ memory usage ] Change in memory usage -------------------------------- "   );
    ATH_MSG_INFO( "[ memory usage ]    Virtual memory change (vsize) : " <<  m_changeVsize      );
    ATH_MSG_INFO( "[ memory usage ]    Real memory change (rss)      : " <<  m_changeRss        );
    ATH_MSG_INFO( "[ memory usage ] ---------------------------------------------------------"  );
#endif

    ATH_MSG_INFO( "finalize() successful." );
    return StatusCode::SUCCESS;
}

/** Query the interfaces. */
StatusCode Trk::TrackingGeometrySvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IID_ITrackingGeometrySvc == riid )
    *ppvInterface = (ITrackingGeometrySvc*)this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

