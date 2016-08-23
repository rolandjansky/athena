/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MuonAlignGenTools/MuonAlignRefitTool.h"
#include "MuonAlignGenTools/MuonAlignHelperTool.h"

#include "GaudiKernel/IMessageSvc.h"

#include "TrkGeometry/MagneticFieldProperties.h"
//#include "TrkMagFieldInterfaces/IMagneticFieldTool.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

#include "DataModel/DataVector.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/Track.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventUtils/MeasurementTypeID.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRecToolInterfaces/IMuonSegmentTrackBuilder.h"

#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "TrkExInterfaces/IPropagator.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
//#include "MuonCombiTrackMaker/SortMeasurementsByPosition.h"
//#include "MuonRecToolInterfaces/IMuonRefitTool.h"
#include "MuonPrepRawData/CscPrepData.h"

#include "TrkVolumes/Volume.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Muon {
  
  //________________________________________________________________________
  MuonAlignRefitTool::MuonAlignRefitTool(const std::string& type,
					 const std::string& name,
					 const IInterface* parent)
    : AthAlgTool(type,name,parent)      
      //, m_propagator("Trk::StraightLinePropagator/MuonStraightLinePropagator")
      //, m_magFieldTool("Trk::MagneticFieldTool/AtlasMagneticFieldTool")
    , m_trackBuilder("Muon::MuonSegmentTrackBuilder/MuonSegmentTrackBuilder")
    , m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
    , m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool")
    , m_muonIdHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
    , m_refitTool("Muon::MuonRefitTool/MuonRefitTool")
    , m_muonAlignHelperTool("Muon::MuonAlignHelperTool/MuonAlignHelperTool")
    , m_trackingVolumesSvc("TrackingVolumesSvc/TrackingVolumesSvc",name)
  {

    declareInterface<MuonAlignRefitTool>(this);

    //declareProperty("Propagator",              m_propagator, "propagator");

    declareProperty("MuonRefitTool",           m_refitTool);

    declareProperty("TrackBuilder",            m_trackBuilder);
    declareProperty("TrackingVolumesSvc",      m_trackingVolumesSvc);

    declareProperty("RemoveBIS8Hits",          m_removeBIS8Hits   = false);
    declareProperty("RemoveBEEHits",           m_removeBEEHits    = false);
    declareProperty("RemoveEEHits",            m_removeEEHits     = false);
    declareProperty("RemoveTGCHits",           m_removeTGCHits    = false);
    declareProperty("RemoveTGCEtaHits",        m_removeTGCEtaHits = true);
    declareProperty("RemoveRPCEtaHits",        m_removeRPCEtaHits = true);
    declareProperty("RemoveCSCHits",           m_removeCSCHits    = false);
    declareProperty("RemoveSpoiledCSCHits",    m_removeSpoiledCSCHits = true);
    declareProperty("RemoveCSCPhiHits",        m_removeCSCPhiHits = false);
    declareProperty("RemoveSmallBarrelHits",   m_removeSmallBarrelHits = false);
    declareProperty("RemoveBarrelHits",        m_removeBarrelHits = false);
    declareProperty("RemoveEndcapHits",        m_removeEndcapHits = false);
    declareProperty("RemoveTRTHits",           m_removeTRTHits    = false);
    declareProperty("RemoveScattering",        m_removeScattering = false);

    declareProperty("DeclusterTGCHits",        m_declusterTGCHits = true);

    declareProperty("RecreateStartingParameters", m_recreateStartingParameters=true);
    declareProperty("ExtrapolateToMuonEntry",  m_extrapolateToMuonEntry = true);

    declareProperty("RedoErrorScaling",        m_redoErrorScaling = false);
  }
  
  //________________________________________________________________________
  MuonAlignRefitTool::~MuonAlignRefitTool() 
  {
    
  }

  //________________________________________________________________________
  StatusCode MuonAlignRefitTool::initialize()
  {

    StatusCode sc=StatusCode::SUCCESS;


    if ( !m_printer.retrieve().isSuccess() ) {
      ATH_MSG_ERROR ("Unable to retrieve" << m_printer);
      return StatusCode::FAILURE;
    }

    if ( !m_helper.retrieve().isSuccess() ) {
      ATH_MSG_ERROR ("Unable to retrieve" << m_helper);
      return StatusCode::FAILURE;
    }

    if ( !m_muonIdHelper.retrieve().isSuccess() ) {
      ATH_MSG_ERROR ("Unable to retrieve" << m_muonIdHelper);
      return StatusCode::FAILURE;
    }

    // Set up ATLAS ID helper 
    if (detStore()->retrieve(m_idHelper, "AtlasID").isFailure()) {
      ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
      return StatusCode::FAILURE;
    } 
    m_measTypeIdHelper=new Trk::MeasurementTypeID(m_idHelper);
    
    /*
    sc = m_propagator.retrieve();
    if ( sc.isSuccess() ) {
      ATH_MSG_DEBUG("Retrieved " << m_propagator);
    }else{
      ATH_MSG_FATAL("Could not get " << m_propagator); 
      return StatusCode::FAILURE;
    }
    */

    sc = m_trackBuilder.retrieve();
    if (sc.isSuccess()){
      ATH_MSG_DEBUG("Retrieved " << m_trackBuilder);
    }else{
      ATH_MSG_FATAL(" Could not get " << m_trackBuilder);
      return sc;
    }
    
    /*
    if (m_magFieldTool.retrieve().isSuccess())
      ATH_MSG_INFO("Retrieved "<<m_magFieldTool);
    else {
      ATH_MSG_WARNING("Failed to retrieve "<<m_magFieldTool);
      return StatusCode::FAILURE;
    }
    m_magFieldProperties = new Trk::MagneticFieldProperties(&(*m_magFieldTool),Trk::RealisticField);
    */

    if (m_muonAlignHelperTool.retrieve().isSuccess()){
      ATH_MSG_DEBUG("Retrieved " << m_muonAlignHelperTool);
    }else{
      ATH_MSG_FATAL(" Could not get " << m_muonAlignHelperTool);
      return StatusCode::FAILURE;
    }
    //return StatusCode::SUCCESS;

    // need to know which TrackingVolume we are in: indet/calo/spectrometer
    if (m_trackingVolumesSvc.retrieve().isFailure())
      {
        ATH_MSG_FATAL( "Failed to retrieve Svc " << m_trackingVolumesSvc );
        return StatusCode::FAILURE;
      } 
    else
    {
      ATH_MSG_INFO( "Retrieved Svc " << m_trackingVolumesSvc );
      m_calVolume   = new Trk::Volume(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
      m_indetVolume = new Trk::Volume(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));
    }
    return StatusCode::SUCCESS;
  }
  
  //________________________________________________________________________
  StatusCode MuonAlignRefitTool::finalize()
  {
    //delete m_magFieldProperties;

    return StatusCode::SUCCESS;
  }
  
  
  //________________________________________________________________________
  Trk::Track* MuonAlignRefitTool::pTCorrectTrack(const Trk::Track& track, 
						  double qOverP, bool removePerigee)  const
  {
    ATH_MSG_DEBUG("in pTCorrectTrack with qOverP: "<<qOverP);

    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces =
      new DataVector<const Trk::TrackStateOnSurface>;
    bool firstTSOS=true;

    const DataVector<const Trk::TrackStateOnSurface>* oldTSOS=track.trackStateOnSurfaces();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=oldTSOS->begin(); 
    for (; iTsos!=oldTSOS->end(); ++iTsos) {

      // always remove perigee
      if (removePerigee && (*iTsos)->type(Trk::TrackStateOnSurface::Perigee)) continue;

      if (firstTSOS) {
	
	ATH_MSG_DEBUG("origTSOS: "<<**iTsos);
	
	// get track parameters modified with ID momentum
	const Trk::TrackParameters* parm = (*iTsos)->trackParameters();
	AmgVector(5) pars=parm->parameters();
	
	ATH_MSG_DEBUG("pOverQ: "<<1./qOverP/1000.);
	pars[Trk::qOverP]=qOverP;
	
	//const Trk::TrackParameters* newparm=parm->cloneToNew(pars);
	
	const Trk::TrackParameters* newparm = 
	  parm->associatedSurface().createTrackParameters(pars[Trk::loc1],pars[Trk::loc2],pars[Trk::phi],
							  pars[Trk::theta],pars[Trk::qOverP],
							  new AmgSymMatrix(5)(*parm->covariance()));
	

	const Trk::MeasurementBase*     meas=(**iTsos).measurementOnTrack() ?
	  (**iTsos).measurementOnTrack()->clone():0;
	const Trk::FitQualityOnSurface* fqos=(**iTsos).fitQualityOnSurface() ? 
	  new Trk::FitQualityOnSurface(*(**iTsos).fitQualityOnSurface()) : 0;
	const Trk::MaterialEffectsBase* meb = (**iTsos).materialEffectsOnTrack() ? (**iTsos).materialEffectsOnTrack()->clone() : 0;

	std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
	for (int i=0;i<(int)Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes;i++) {
	  if ((**iTsos).type(Trk::TrackStateOnSurface::TrackStateOnSurfaceType(i)))
	    typePattern.set(i);
	}	
	const Trk::TrackStateOnSurface* newTsos=new Trk::TrackStateOnSurface(meas,newparm,fqos,meb,typePattern);
	
	ATH_MSG_DEBUG("newTSOS: "<<*newTsos);	
	trackStateOnSurfaces->push_back(newTsos);
	firstTSOS=false;
      }
      else 
	trackStateOnSurfaces->push_back((*iTsos)->clone());
      
    }
    Trk::Track* newTrack = new Trk::Track(Trk::TrackInfo(track.info()),
					  trackStateOnSurfaces,
					  track.fitQuality()->clone());
    return newTrack;
  } 
 

  //________________________________________________________________________
  const Trk::Track* MuonAlignRefitTool::refitForAlignment(const Trk::Track& track)
  {
    //IMuonRefitTool::Settings settings;
    IMuonRefitTool::Settings settings;
    settings.refit                      = true;
    settings.removeOutliers             = false;
    settings.discardNotCleanedTracks    = false;
    settings.recalibrate                = false;
    settings.updateErrors               = false;
    settings.updateTriggerErrors        = false;
    settings.prepareForFit              = true;
    settings.recreateStartingParameters = m_recreateStartingParameters;
    settings.removeOtherSectors         = false;
    settings.deweightOtherSectors       = false;
    settings.extrapolateToMuonEntry     = m_extrapolateToMuonEntry;
    settings.removeTriggerEtaHits       = true;

    ATH_MSG_DEBUG("calling refit");
    const Trk::Track* refitTrack=m_refitTool->refit(track,&settings);
    if (!refitTrack || refitTrack==&track) {
      ATH_MSG_DEBUG("initial refit failed");
      return 0;
    }

    // always redo competing ROTs to make sure they have the correct alignment position
    bool doMdts=m_redoErrorScaling; // for error scaling
    bool doCompetingClusters=true;  // this should always be done to get the correct alignment
    if (m_redoErrorScaling) 
      ATH_MSG_WARNING("error scaling not done properly for CSC hits yet!");

    ATH_MSG_DEBUG("calling recalibrateHitsOnTrack");
    Trk::Track* recalTrack=m_trackBuilder->recalibrateHitsOnTrack(*refitTrack,doMdts,
								  doCompetingClusters);
    if (!recalTrack) {
      ATH_MSG_DEBUG("track recalibration failed");
      delete refitTrack;
      return 0;
    }
    
    ATH_MSG_DEBUG("calling refit again");
    const Trk::Track* finalRefitTrack=m_refitTool->refit(*recalTrack,&settings);
    if (!finalRefitTrack || finalRefitTrack==recalTrack) {
      ATH_MSG_DEBUG("final refit failed");
      delete recalTrack;
      return 0;
    }
    
    ATH_MSG_DEBUG("returning finalRefitTrack");
    return finalRefitTrack;
  }
 
  //________________________________________________________________________
  const Trk::Track* MuonAlignRefitTool::prepareHitsOnTrack(const Trk::Track& track)
  {
    ATH_MSG_DEBUG("in prepareHitsOnTrack");

    const DataVector<const Trk::TrackStateOnSurface>* oldTSOS=track.trackStateOnSurfaces();
    
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces =
      new DataVector<const Trk::TrackStateOnSurface>;
    
    std::set<Identifier> tgcLayerIds;
    int nMdtMeas(0);

    bool firstScat=true;
    bool firstMeas=true;

    // search first valid TSOS first
    DataVector<const Trk::TrackStateOnSurface>::const_iterator iTsos=oldTSOS->begin(); 
    for (iTsos=oldTSOS->begin(); iTsos!=oldTSOS->end(); ++iTsos) {
      
      // get measurement, continue if hit to be removed
      const Trk::MeasurementBase*      mesb     = (*iTsos)->measurementOnTrack();
      Trk::TrackState::MeasurementType measType = m_measTypeIdHelper->defineType(mesb);

      ATH_MSG_VERBOSE("(before) measType="<<measType<<", type: "<<(*iTsos)->dumpType());

	  
      // removal of specific types of measurements
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement) &&
	  ((measType==Trk::TrackState::TGC && m_removeTGCHits) ||
	   (measType==Trk::TrackState::TRT && m_removeTRTHits) ||
	   (measType==Trk::TrackState::CSC && m_removeCSCHits) )) continue;
            
      // decluster TGC hits
      if (measType==Trk::TrackState::TGC && m_declusterTGCHits) {
	
	// loop over RIO in CRIO, create TSOS from each one
	const Trk::MeasurementBase*             mesb       = (*iTsos)->measurementOnTrack();	
	const Trk::CompetingRIOsOnTrack*        crio       = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);

	// special case of TGC hit not in CompetingROT
	if (!crio) { 
	  ATH_MSG_DEBUG("TGC with no crio!"); 
	  const Trk::RIO_OnTrack* rio=dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	  if (!rio) { ATH_MSG_ERROR("no crio or rio for TGC hit!"); continue; }
	  const Trk::MeasurementBase*     meas=dynamic_cast<const Trk::MeasurementBase*>(rio)->clone();
	  const Trk::TrackParameters*     tpar=(**iTsos).trackParameters()->clone();
	  const Trk::FitQualityOnSurface* fqos=(**iTsos).fitQualityOnSurface() ? 
	    new Trk::FitQualityOnSurface(*(**iTsos).fitQualityOnSurface()) : 0;	  


	  // keep first scatterer, and remaining scatterers only if not removing scattering	  
	  const Trk::MaterialEffectsBase* meb = 0;
	  if ((**iTsos).materialEffectsOnTrack()) {
	    meb = firstScat&&firstMeas ? (**iTsos).materialEffectsOnTrack()->clone() : materialEffects(**iTsos);
	    firstScat=false;
	  }
	  ATH_MSG_DEBUG("meb: "<<meb);
	  	  	  
	  const Trk::TrackStateOnSurface* newTsos=
	    new Trk::TrackStateOnSurface(meas,tpar,fqos,meb);
	  
	  trackStateOnSurfaces->push_back(newTsos);
	  continue;
	}
	
	// normal CompetingROT
	for (unsigned int irio=0;irio<crio->numberOfContainedROTs();irio++) {
	  const Trk::RIO_OnTrack* rio=&crio->rioOnTrack(irio);
	  if (!rio) continue;
	  Identifier layId = m_muonIdHelper->layerId(rio->identify());
	  ATH_MSG_DEBUG("have ID "<<layId<<" for rioId "<<rio->identify());
	  std::pair<std::set<Identifier>::iterator, bool> pr=tgcLayerIds.insert(layId);
	  if (pr.second == false) continue;
	  ATH_MSG_DEBUG("adding TGC RIO with rioId "<<rio->identify());
	  
	  // attach any material effects to first RIO
	  const Trk::MeasurementBase*     meas=dynamic_cast<const Trk::MeasurementBase*>(rio)->clone();
	  const Trk::TrackParameters*     tpar=(**iTsos).trackParameters()->clone();
	  const Trk::FitQualityOnSurface* fqos=(**iTsos).fitQualityOnSurface() ? 
	    new Trk::FitQualityOnSurface(*(**iTsos).fitQualityOnSurface()) : 0;
	  const Trk::MaterialEffectsBase* meb =0;

	  // keep first scatterer, and remaining scatterers only if not removing scattering
	  if (irio==0) {
	    if ((**iTsos).materialEffectsOnTrack()) {
	      meb = firstScat&&firstMeas ? (**iTsos).materialEffectsOnTrack()->clone() : materialEffects(**iTsos);
	      firstScat=false;
	    }
	  }
	  ATH_MSG_DEBUG("meb: "<<meb);
	
	  const Trk::TrackStateOnSurface* newTsos=
	    new Trk::TrackStateOnSurface(meas,tpar,fqos,meb);
	  trackStateOnSurfaces->push_back(newTsos);
	}	
      }
      
      // remove bad CSC hits
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement) &&	  
	  measType==Trk::TrackState::CSC && 
	  (m_removeSpoiledCSCHits || m_removeCSCPhiHits)) {
	
	ATH_MSG_DEBUG("checking CSC hit");
	
	const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	if (rio) {
	  const CscPrepData* csc_prp_data = dynamic_cast<const CscPrepData*>(rio);	
	  if (csc_prp_data) {
	    bool is_measphi_strip = m_muonIdHelper->cscIdHelper().measuresPhi(csc_prp_data->identify());
	    if (is_measphi_strip&&m_removeCSCPhiHits) continue;                           
	    if (!is_measphi_strip && 
		csc_prp_data->status() != CscStatusUnspoiled &&
		m_removeSpoiledCSCHits) {
	      ATH_MSG_DEBUG("spoiled CSC hit");
	      continue;
	    }
	  }
	}
      }
      
      // remove TGC and/or RPC eta hits
      if ((*iTsos)->type(Trk::TrackStateOnSurface::Measurement) &&
	  ((measType==Trk::TrackState::TGC && m_removeTGCEtaHits) ||
	   (measType==Trk::TrackState::RPC && m_removeRPCEtaHits))) {
	
	const Trk::RIO_OnTrack*          rio  = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=&crio->rioOnTrack(0);
	if (rio) {
	  if (!m_muonIdHelper->measuresPhi(rio->identify())) continue;
	}
      }
            
      // removal of barrel or endcap-specific hits
      if ( ( m_removeBarrelHits || m_removeEndcapHits || 
	     m_removeSmallBarrelHits || 
	     m_removeBIS8Hits || m_removeBEEHits || m_removeEEHits ) && 
	   (*iTsos)->type(Trk::TrackStateOnSurface::Measurement) ) {
	
	const Trk::MeasurementBase*      mesb = (*iTsos)->measurementOnTrack();	
	const Trk::RIO_OnTrack*           rio = dynamic_cast<const Trk::RIO_OnTrack*>(mesb);
	const Trk::CompetingRIOsOnTrack* crio = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(mesb);
	if (!rio && crio) rio=&crio->rioOnTrack(0);
	if (rio) {
	  if (m_muonIdHelper->isMuon(rio->detectorElement()->identify())) { 
	    if (!m_muonAlignHelperTool->isEndcap(rio->detectorElement()->identify())) { 

	      // barrel hit, not BIS8
	      if (!m_muonAlignHelperTool->isBIS8(rio->detectorElement()->identify())) { 
		
		if (m_removeBarrelHits) {
		  ATH_MSG_VERBOSE("removing barrel muon hit");
		  continue;
		}
		if (m_removeSmallBarrelHits) { 
		  Identifier id=rio->detectorElement()->identify();
		  const std::string stName=
		    m_muonIdHelper->mdtIdHelper().stationNameString(m_muonIdHelper->mdtIdHelper().stationName(id));
		  if (stName[2]!='L') {
		    ATH_MSG_VERBOSE("removing small barrel hit");
		    continue;
		  }
		}
	      }

	      // BIS8
	      else {
		ATH_MSG_DEBUG("have BIS8 hit");
		if (m_removeBIS8Hits) {
		  ATH_MSG_DEBUG("removing BIS8 muon hit");
		  continue;
		}
	      }
	    }
	    else {
	      if (m_removeEndcapHits) {
		ATH_MSG_VERBOSE("removing endcap muon hit");
		continue;
	      }
	    }
	    
	    ATH_MSG_VERBOSE("keeping hit in "
			    << m_muonIdHelper->toStringGasGap(rio->identify()));
	  }
	  if (m_removeBEEHits||m_removeEEHits) {
	    const std::string stName=
	      m_muonIdHelper->mdtIdHelper().stationNameString(m_muonIdHelper->mdtIdHelper().stationName(rio->identify()));
	    if (m_removeBEEHits && stName.substr(0,3)=="BEE") continue;
	    if (m_removeEEHits  && stName.substr(0,2)=="EE") continue;
	  }  
	}
      }
      
      // if MDT hit, increment nMdtMeas
      if ((**iTsos).type(Trk::TrackStateOnSurface::Measurement) &&
	  (measType==Trk::TrackState::MDT) &&
	  !(**iTsos).type(Trk::TrackStateOnSurface::Outlier) &&
	  !(**iTsos).type(Trk::TrackStateOnSurface::Hole)) nMdtMeas++;
      
      // have a good TSOS, create a new TSOS from old one
      ATH_MSG_VERBOSE("cloning TSOS");

      if ((**iTsos).type(Trk::TrackStateOnSurface::Measurement)) firstMeas=false;
      
      const Trk::TrackStateOnSurface* newTsos=0;
      
      if (!m_removeScattering) { 
	newTsos = (**iTsos).clone();
      }      
      else {
	const Trk::MeasurementBase*     mesb = (**iTsos).measurementOnTrack() ? (**iTsos).measurementOnTrack()->clone() : 0;
	const Trk::TrackParameters*     tpar = (**iTsos).measurementOnTrack() ? (**iTsos).trackParameters()->clone() : 0;
	const Trk::FitQualityOnSurface* fqos = (**iTsos).fitQualityOnSurface() ? 
	  new Trk::FitQualityOnSurface(*(**iTsos).fitQualityOnSurface()) : 0;
	
	const Trk::MaterialEffectsBase* meb = 0;
	  // keep first scatterer, and remaining scatterers only if not removing scattering
	if ((**iTsos).materialEffectsOnTrack()) {
	  meb = firstScat&&firstMeas ? (**iTsos).materialEffectsOnTrack()->clone() : materialEffects(**iTsos);
	  if (!firstMeas) firstScat=false;
	}
	ATH_MSG_DEBUG("meb: "<<meb);

	newTsos = new Trk::TrackStateOnSurface(mesb,tpar,fqos,meb);
      }

      if (newTsos) trackStateOnSurfaces->push_back(newTsos);      
    }

    // see if enough MDT hits remaining after removal
    if (nMdtMeas<7) {
      for (int i=0;i<(int)trackStateOnSurfaces->size();i++)
	delete (*trackStateOnSurfaces)[i];
      ATH_MSG_DEBUG("only "<<nMdtMeas<<" MDT hits, not making track");
      return 0;
    }
    
    // create new track with good TSOS
    const Trk::Track* newTrack = new Trk::Track(Trk::TrackInfo(track.info()),
                                                trackStateOnSurfaces,
                                                track.fitQuality()->clone());
    
    ATH_MSG_DEBUG("created newTrack:");
    ATH_MSG_DEBUG(m_printer->print(*newTrack)<<endmsg<<
		  m_printer->printStations(*newTrack)<<endmsg);//<<

    return newTrack;
  }

  //________________________________________________________________________
  const Trk::MaterialEffectsBase* MuonAlignRefitTool::materialEffects(const Trk::TrackStateOnSurface& tsos) const
  {
    ATH_MSG_DEBUG("materialEffects");
    bool cloneMEB=false;

    const Trk::MaterialEffectsBase* meb  = tsos.materialEffectsOnTrack();

    if (meb) {	    
      
      /*
      if (m_removeScattering) {
	// keep scattering only if in ID or calorimeter
	Trk::GlobalPosition position =
	  tsos.materialEffectsOnTrack()->associatedSurface().globalReferencePoint();
	if (m_indetVolume->inside(position) || m_calVolume->inside(position)) 
	  cloneMEB=true;
	
      }
      else cloneMEB=true;
      */
      if (!m_removeScattering) cloneMEB=true;

      // clone calorimeter energy loss
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(meb);
      if (meot && !meot->scatteringAngles()) cloneMEB=true;
    }

    return cloneMEB ? meb->clone() : 0;
  }

} // end namespace
