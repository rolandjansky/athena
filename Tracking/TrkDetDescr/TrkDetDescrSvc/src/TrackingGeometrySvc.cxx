/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingGeometrySvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkDetDescrSvc/TrackingGeometryMirror.h"
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

// monitor memory usage
#ifdef TRKDETDESCR_MEMUSAGE   
#include <unistd.h>
#endif


/** Constructor **/
Trk::TrackingGeometrySvc::TrackingGeometrySvc(const std::string& name,ISvcLocator* svc) : 
    AthService(name,svc),
    m_geometryProcessors(this)
{ 
  // geometry processors to validation / distort the TrackingGeometry ------------
  declareProperty( "GeometryProcessors",           m_geometryProcessors);
}


/** Destructor **/
Trk::TrackingGeometrySvc::~TrackingGeometrySvc()
{}

/** Initialize Service */
StatusCode Trk::TrackingGeometrySvc::initialize()
{
  m_autoRetrieveTools = false;
  m_checkToolDeps = false;
  
  // get the DetectorStore
  ATH_CHECK( service("DetectorStore", m_pDetStore ) );

  // alternative mode which only uses an externally provided TrackingGeometry rather than building its own
  if (m_useConditionsData) {
     for (auto &handle : m_geometryProcessors ) {
         handle.disable();
     }
     m_trackingGeometryBuilder.disable();
     return StatusCode::SUCCESS;
  }

  if (m_geometryProcessors.retrieve().isFailure()){
      ATH_MSG_FATAL( "Could not retrieve " << m_geometryProcessors );
      return StatusCode::FAILURE;
  }

  if (m_buildGeometryFromTagInfo){
     ATH_MSG_INFO( "Building Geometry from TagInfo" );
     // register a callback on TagInfo updates
     ServiceHandle<ITagInfoMgr> tagInfoMgr("TagInfoMgr", name());
     ATH_CHECK(tagInfoMgr.retrieve());
     tagInfoMgr->addListener(this);
  } else {
     ATH_MSG_INFO( "Building Geometry at initialisation time." );
     // build with no dependency on COOL
     if(trackingGeometryInit(false).isFailure()) {
        ATH_MSG_FATAL( "Unable to build the TrackingGeometry!" );
        return StatusCode::FAILURE;
     }
  }
  ATH_MSG_INFO( "initialize() successful! " );
  return StatusCode::SUCCESS;
}


void Trk::TrackingGeometrySvc::tagInfoUpdated()
{
   ATH_MSG_INFO("tagInfoUpdated() callback trigerred");
   trackingGeometryInit().ignore();
}


StatusCode Trk::TrackingGeometrySvc::trackingGeometryInit(bool needsInit)
{
   ATH_MSG_INFO( "Trk::TrackingGeometrySvc::trackingGeometryInit" );

    if (m_useConditionsData) {
       ATH_MSG_FATAL("Logic error: TrackingGeometry init callback called despite being configured to use external TrackingGeometries provided by a conditions algorithm.");
    }
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

    // only build if the callback string was true
    // (MN: I set this to alwaus true if called on TagInfo update)
    if (needsInit) {
       ATH_MSG_INFO( "trackingGeometryInit - initialize on TagInfoMgr callback" );
        // cleanup the geometry if you have one 
        // (will delete what is in detector store, because new one will overwrite old one)
        m_trackingGeometry = nullptr;
    
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
          const Trk::TrackingGeometry* oldTrackingGeometry = nullptr;
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

      if (msgLvl(MSG::VERBOSE)) {
         Trk::TrackingGeometryMirror mirror(atlasTrackingGeometry);
         mirror.dump(msg(MSG::VERBOSE), name()+" TrackingGeometry dump ");
         mirror.cleanup();
      }
      // record the resulting TrackingGeometry 
      if (m_pDetStore->record(atlasTrackingGeometry, m_trackingGeometryName, false).isFailure() ){
         ATH_MSG_WARNING( "Couldn't write TrackingGeometry to DetectorStore." );
      } else
        ATH_MSG_DEBUG( "initialize() successful: TrackingGeometry '" << m_trackingGeometryName << "' built and written to DetectorStore." );
    }
    return StatusCode::SUCCESS;
}

void Trk::TrackingGeometrySvc::setTrackingGeometry(const Trk::TrackingGeometry *ptr) {
   if (!m_useConditionsData) {
      ATH_MSG_FATAL("Logic error:  external TrackingGeometry provided despite being configured to build an internal one.");
   }
   if (m_pDetStore->contains<Trk::TrackingGeometry>(m_trackingGeometryName)) {
      const TrackingGeometry *mirror;
      if (m_pDetStore->retrieve(mirror, m_trackingGeometryName).isFailure()) {
         ATH_MSG_FATAL("TrackingGeometry " << m_trackingGeometryName << " exists, but cannot be retrieved.");
      }
      TrackingGeometry *non_const_mirror ATLAS_NOT_THREAD_SAFE = const_cast<TrackingGeometry *>(mirror);
      static_cast<Trk::TrackingGeometryMirror *>(non_const_mirror)->update(const_cast<TrackingGeometry *>(ptr));
      if (msgLvl(MSG::VERBOSE)) {
         ATH_MSG_VERBOSE( "Setting TrackingGeometry from cond alg ptr="  << static_cast<const void *>(ptr));
         static_cast<const Trk::TrackingGeometryMirror *>(mirror)->dump(msg(MSG::VERBOSE), "external TrackingGeometry dump ");
      }
   }
   else {
      std::unique_ptr<Trk::TrackingGeometryMirror> mirror=std::make_unique<Trk::TrackingGeometryMirror>(const_cast<TrackingGeometry *>(ptr));
      if (msgLvl(MSG::VERBOSE)) {
         ATH_MSG_VERBOSE( "Setting TrackingGeometry from cond alg ptr="  << static_cast<const void *>(ptr));
         mirror->dump(msg(MSG::VERBOSE), "external TrackingGeometry dump ");
      }

      // record the resulting TrackingGeometry
      if (m_pDetStore->record(static_cast<TrackingGeometry *>(mirror.release()), m_trackingGeometryName, false).isFailure() ){
         ATH_MSG_WARNING( "Couldn't write TrackingGeometry to DetectorStore." );
      } else {
         ATH_MSG_DEBUG( "initialize() successful: TrackingGeometry '" << m_trackingGeometryName << "' built and written to DetectorStore." );
      }
   }
   m_trackingGeometry=ptr;
}
void Trk::TrackingGeometrySvc::trackingGeometryNotSet() const {
   ATH_MSG_WARNING( "TrackingGeometry not set  ptr="  << static_cast<const void *>(m_trackingGeometry));
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

    if (m_useConditionsData) {
       const TrackingGeometry *clone=nullptr;
       if (m_pDetStore->retrieve(clone, m_trackingGeometryName).isFailure() ){
          ATH_MSG_WARNING( "Failed to retrieve TrackingGeometry " << m_trackingGeometryName << " from DetectorStore." );
       }
       if (clone) {
          static_cast<Trk::TrackingGeometryMirror *>(const_cast<TrackingGeometry *>(clone))->cleanup();
          ATH_MSG_WARNING( "Cleaned TrackingGeometryMirror " << m_trackingGeometryName << "." );
       }
    }

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
