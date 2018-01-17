/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonHolesOnTrackTool.cxx
//   Implementation file for class MuonHolesOnTrackTool
///////////////////////////////////////////////////////////////////
// S.Todorova
///////////////////////////////////////////////////////////////////

#include "MuonTGRecTools/MuonHolesOnTrackTool.h"

// Gaudi includes
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
//#include "TrkEventPrimitives/LocalPosition.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
//#include "TrkEventPrimitives/CovarianceMatrix.h"
// #include "TrkParameters/MeasuredPerigee.h"
// #include "TrkParameters/MeasuredTrackParameters.h"
// #include "TrkParameters/Perigee.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/DistanceSolution.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
// #include "TrkParameters/AtaCylinder.h"
// #include "TrkParameters/AtaDisc.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "TrkTrack/Track.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrackRecord/TrackRecord.h"
#include <vector>

//#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

// Constructor with parameters:
Muon::MuonHolesOnTrackTool::MuonHolesOnTrackTool(const std::string &type, const std::string &name, const IInterface* parent ) :
  AthAlgTool(type,name,parent),
  m_trackingGeometryName("MuonTest"),
  m_outlierLim(50.),
  m_identifyHoles(true),
  m_softLowerCut(0),
  m_stopSearch(1),
  m_measTool("Muon::MuonTGMeasurementTool/MuonTGMeasurementTool"),
  m_extrapolator("Trk::Extrapolator/Extrapolator"),
  m_rotCreator("Trk::RIO_OnTrackCreator/RIO_OnTrackCreator"),
  m_StoreGate(0),
  m_msEntrance(0),
  m_parUpdate(false)
{  
  declareInterface<Trk::ITrackHoleSearchTool>(this);

  m_sortingRefPoint.push_back(0.);
  m_sortingRefPoint.push_back(0.);
  m_sortingRefPoint.push_back(0.);

  // Get parameter values from jobOptions file
  declareProperty("OutlierResidualLimit",m_outlierLim);
  declareProperty("ExtrapolatorName", m_extrapolator);
  declareProperty("RIO_OnTrackCreator", m_rotCreator);
  declareProperty("TrackingGeometryName", m_trackingGeometryName);     // this is dummy now and will be removed
  declareProperty("MuonTGMeasurementTool",m_measTool);
  //declareProperty("sortingRefereePoint",m_sortingRefPoint);
  declareProperty("DoParameterUpdate",m_parUpdate);
  declareProperty("DoHolesIdentification",m_identifyHoles);
  declareProperty("LowerTrackMomentumCut",m_softLowerCut);
  declareProperty("StopHolesSearchMode",m_stopSearch);
}

// Initialize method:
StatusCode Muon::MuonHolesOnTrackTool::initialize()
{

  // Get the messaging service, print where you are
  ATH_MSG_INFO("MuonHolesOnTrackTool::initialize()");

  StatusCode sc;

  sc = service("DetectorStore", m_detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Detector service not found !");
    return StatusCode::FAILURE;
  } 

  sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("StoreGate service not found !");
    return StatusCode::FAILURE;
  } 
 
  sc = m_detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve MdtIdHelper");
	  return sc;
  }

  sc = m_detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve RpcIdHelper");
	  return sc;
  }

  sc = m_detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve CscIdHelper");
	  return sc;
  }

  sc = m_detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve TgcIdHelper");
	  return sc;
  }
  
  sc = m_detStore->retrieve(m_muonMgr);
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve MuonDetectorManager...");
	  return sc;
  }

  // get measurement tool
  sc = m_measTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find TG measurement tool ");
  } else {
    ATH_MSG_INFO("TG measurement tool booked ");
  }

  // get extrapolator
  sc = m_extrapolator.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not find extrapolator tool. Exiting.");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO("Extrapolator tool booked ");
  }

  // get rotCreator tool
  sc = m_rotCreator.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Could not find RIO_OnTrackCreator tool ");
  } else {
    ATH_MSG_INFO(" RIO_OnTrackCreator tool booked ");
  }

  return StatusCode::SUCCESS;
}
// 

DataVector<const Trk::TrackStateOnSurface>* Muon::MuonHolesOnTrackTool::getHolesOnTrack(const Trk::Track& input_track, const Trk::ParticleHypothesis particle)  const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonHolesOnTrackTool::getHolesOnTrack");

  // get MSEntry and ordered TSoS 
  DataVector<const Trk::TrackStateOnSurface>* tSoS=0;   
  const Trk::TrackParameters* msEntry = getMSEntry(&input_track,tSoS);
  // cancel if no msEntry found
  if (!msEntry) 
  	{
	delete tSoS;
	return 0;
	}
  //
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iter = tSoS->begin();

  bool isSoft = false;
 
  for (;iter!=tSoS->end();iter++) {
    if ( (*iter)->type(Trk::TrackStateOnSurface::Measurement) ) {
      if ((*iter)->trackParameters() && (*iter)->trackParameters()->momentum().mag() < m_softLowerCut) { 
        isSoft = true;
	break;
      }
    }
  }
  
  //Discard soft tracks that fool the extrapolator - difficult to follow
  if (isSoft == true){
    ATH_MSG_DEBUG(" Track has Momentum lower than " << m_softLowerCut << " MeV/c:  " 
	<< " abandoning the hole search ");
    delete msEntry; delete tSoS;
    return 0;
  }

  //
  DataVector<const Trk::TrackStateOnSurface>*  holes= new DataVector<const Trk::TrackStateOnSurface>;
  std::vector<const Trk::TrackStateOnSurface*>*  temp_holes= new std::vector<const Trk::TrackStateOnSurface*>;

  // loop over active layers
  int nAttempts = 0;
  bool expMeas = true;
  const Trk::TrackParameters* currPar = msEntry;
  const Trk::DetachedTrackingVolume* enclosingStation = 0;
  // defined refPars as the point where a failed search can be reinitialized ( typically, where last hit on track was found )
  const Trk::TrackParameters* refPars = currPar->clone();

  while ( currPar ) {

    // find next expected track record entry to control extrapolation
    const Trk::Layer* nextLayer = 0;
    if (expMeas) {
      nextLayer = nextExpectedLayer(currPar,tSoS);
      if (!nextLayer) expMeas = false;
    }
    if (!expMeas && m_stopSearch==2) break;
 
    std::pair<const Trk::TrackParameters*,const Trk::Layer*> next = m_extrapolator->extrapolateToNextActiveLayer(*currPar,
               Trk::alongMomentum,true,particle) ;

    const Trk::TrackParameters* nextPar = next.first;
    const Trk::Layer* layer = next.second;
    // stop extrapolation - could be done in a better way - return current position for further extrapolation ?
    if ( !layer ) {
      delete nextPar;
      nextPar = 0;
    } else if (nextPar) {
      if (!expMeas && m_stopSearch==1 && enclosingStation 
	    && !enclosingStation->trackingVolume()->inside(nextPar->position(),0.001)) {delete nextPar; break; } 
      enclosingStation = layer->enclosingDetachedTrackingVolume();
    }

    if(nextPar){
      if(nextPar->momentum().unit().dot(currPar->momentum().unit()) < 0) nAttempts++;
    }
    else nAttempts++;
    
    if(nAttempts > 30){
      ATH_MSG_INFO("getHolesOnTrack: too many attempts extrapolating; aborting extrapolation " 
	  << " and returning NO holes !!");
      //break;
      delete currPar; delete nextPar;
      for (unsigned int ih=0; ih < temp_holes->size(); ih++) delete ((*temp_holes)[ih]);   
      temp_holes->clear(); delete temp_holes;
      //for (unsigned int ih=0; ih < holes->size(); ih++) delete ((*holes)[ih]);   
      holes->clear(); delete holes;
      delete tSoS;
      return 0;
    }
    
    const Trk::TrackParameters* updatePar = 0;

    // check if extrapolation ok
    bool failedExtr = false;
    if (nextLayer) {
      if (!nextPar) {
        failedExtr = true;
      } else if (layer != nextLayer ) {
	Trk::DistanceSolution dist = nextLayer->surfaceRepresentation().straightLineDistanceEstimate(nextPar->position(),nextPar->momentum().unit());
	if (  (dist.numberOfSolutions()>0 && dist.first()<=0) ||
	      (dist.numberOfSolutions()==0 && dist.toPointOfClosestApproach()<0) )   failedExtr = true;
      }
      if (failedExtr && refPars) {
	// try to recover
	//std::cout <<"extrapolate to missed surface:"<<nextLayer->surfaceRepresentation().center() << std::endl; 
	const Trk::TrackParameters* repPar = m_extrapolator->extrapolate(*refPars,nextLayer->surfaceRepresentation(),
									 Trk::alongMomentum,false,particle) ;
	for (unsigned int ih=0; ih < temp_holes->size(); ih++) delete ((*temp_holes)[ih]);   
	temp_holes->clear();
	//std::cout << "trying to recover with parameters:"<< repPar->position() << std::endl;
	if (repPar) {
	  int assocMeas = 0;
	  double res = -1.;
	  updatePar = findAssociatedMeasurement(repPar,nextLayer,tSoS,assocMeas,res);
	  //std::cout << "update?"<<updatePar <<","<<res<<","<<assocMeas <<std::endl;
	  delete nextPar;
	  nextPar = updatePar;
          if (updatePar) {delete refPars; refPars=updatePar->clone();}
	  layer = nextLayer;
	  failedExtr = false;
	  updatePar = 0;
	  delete repPar;	  
	}
      }
    }   
    if ( failedExtr ) {
      ATH_MSG_WARNING("MuonHolesOnTrackTool::getHolesOnTrack: lost in extrapolation, abort");
      for (unsigned int ih=0; ih < temp_holes->size(); ih++) delete ((*temp_holes)[ih]);
      temp_holes->clear();
      delete temp_holes; temp_holes=0;
      //for (unsigned int ih=0; ih < holes->size(); ih++) delete ((*holes)[ih]);
      delete holes;
      delete tSoS;
      delete currPar;
      delete nextPar;
      return 0;
    }
    if (!nextPar) break;
    
    // we are interested in muon layers only !
    if (layer) {
      const Trk::DetachedTrackingVolume* detVol = layer->enclosingDetachedTrackingVolume();
      if (!detVol) ATH_MSG_ERROR("MuonHolesOnTrackTool::getHolesOnTrack: no enclosing detached volume?!");
      if (!detVol) layer=0;
    }
    if (!layer) {
      ATH_MSG_WARNING("MuonHolesOnTrackTool::getHolesOnTrack: no associated layer?!");
    } else {
      int assocMeas = 0;
      double res = -1.;
      updatePar = findAssociatedMeasurement(nextPar,layer,tSoS,assocMeas,res);
      // create hole :assocMeas>0 signals associated measurement OR hole (1/eta,2/phi,3/both)
       
      unsigned int lId = layer->layerType();
      Identifier layId(lId);
      if ( assocMeas<1  &&  lId && m_mdtIdHelper->is_mdt(layId)  ) {
        bool measPhi = false;
	const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
        if (hole) temp_holes->push_back(hole); 
      }

      if ( assocMeas<3  &&  lId && !m_mdtIdHelper->is_mdt(layId) ) {
        // create a hole 
        if ( m_tgcIdHelper->is_tgc(layId) && assocMeas<2) {
          if (m_tgcIdHelper->gasGap(layId)==2 && m_tgcIdHelper->gasGapMax(layId)==3) assocMeas+=2;
          //if (layId == 0) std::cout << "TGC layer not identified, this phi hole can be fake" << std::endl;
        }
	if (assocMeas<2){
	  bool measPhi = true;
	  const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
	  if (hole) temp_holes->push_back(hole); 
	}
	if (assocMeas==0 || assocMeas==2){
	  bool measPhi = false;
	  const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
	  if (hole) temp_holes->push_back(hole); 
	}
      }
      // collect holes if extrapolation looks reasonably enough 
      if (assocMeas>0 && res < 50. )  {
	for (unsigned int ih=0; ih < temp_holes->size(); ih++) holes->push_back((*temp_holes)[ih]);   
	temp_holes->clear();
	//std::cout << "saving temp holes:" << holes->size() << std::endl;
      }
    }
    delete currPar;
    currPar=nextPar;
    if (updatePar) { currPar=updatePar; delete refPars; refPars = updatePar->clone(); delete nextPar; }
  }
  // collect remaining holes
  for (unsigned int ih=0; ih < temp_holes->size(); ih++) holes->push_back((*temp_holes)[ih]);  

  ATH_MSG_DEBUG("MuonHolesOnTrackTool::getHolesOnTrack: number of holes on this track:"<< holes->size() );
  delete currPar;
  delete tSoS;
  delete temp_holes; temp_holes = 0;
  delete refPars;
  return holes;
}

const Trk::Track* Muon::MuonHolesOnTrackTool::getTrackWithHoles(const Trk::Track& input_track , const Trk::ParticleHypothesis particle) const 
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonHolesOnTrackTool::getTrackWithHoles");
  
  //
  DataVector<const Trk::TrackStateOnSurface>*  holes= getHolesOnTrack(input_track, particle);

  // get MSEntry and ordered TSoS 
  DataVector<const Trk::TrackStateOnSurface>* tSoSOrd;   
  const Trk::TrackParameters* msEntry = getMSEntry(&input_track,tSoSOrd);
  Muon::TrackStateOnSurfaceComparisonFunction fOrd(msEntry->momentum());
 
  if (!holes)  holes= new DataVector<const Trk::TrackStateOnSurface>;
  for (unsigned int i=0; i<tSoSOrd->size(); i++) holes->push_back(new Trk::TrackStateOnSurface(*(*tSoSOrd)[i]));
  DataVector<const Trk::TrackStateOnSurface>::iterator hit  = holes->begin();
  DataVector<const Trk::TrackStateOnSurface>::iterator hend = holes->end();
  // order function for this track already defined ?
  sort( hit, hend, fOrd );
  // create track
  return new Trk::Track(input_track.info(),holes, new Trk::FitQuality(*(input_track.fitQuality())));
}

const Trk::Track* Muon::MuonHolesOnTrackTool::getTrackWithHolesAndOutliers(const Trk::Track& input_track , const Trk::ParticleHypothesis particle ) const 
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonHolesOnTrackTool::getTrackWithHolesAndOutliers");
  
  // get MSEntry and ordered TSoS 
  DataVector<const Trk::TrackStateOnSurface>* tSoS;   
  const Trk::TrackParameters* msEntry = getMSEntry(&input_track,tSoS);
  /*
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iter = tSoS->begin();
  for (;iter!=tSoS->end();iter++) {
    if ( (*iter)->type(Trk::TrackStateOnSurface::Measurement) ) {
      std::cout << "ordered measurements?" << iter-tSoS->begin() << "," 
		<< (*iter)->measurementOnTrack()->associatedSurface().center() << std::endl;
      if ((*iter)->trackParameters()) 
      std::cout << "track parameters?" << iter-tSoS->begin() << "," 
		<< (*iter)->trackParameters()->position() <<"," <<(*iter)->trackParameters()->momentum()  << std::endl;
    }
  }
  */
  //
  DataVector<const Trk::TrackStateOnSurface>*  holes= new DataVector<const Trk::TrackStateOnSurface>;
  std::vector<const Trk::TrackStateOnSurface*>*  temp_holes= new std::vector<const Trk::TrackStateOnSurface*>;

  // loop over active layers
  int nAttempts = 0;
  const Trk::TrackParameters* currPar = msEntry;
  while ( currPar ) {

    // find next expected track record entry to control extrapolation
    const Trk::Layer* nextLayer = nextExpectedLayer(currPar,tSoS);

    std::pair<const Trk::TrackParameters*,const Trk::Layer*> next = m_extrapolator->extrapolateToNextActiveLayer(*currPar,
               Trk::alongMomentum,true,particle) ;
    const Trk::TrackParameters* nextPar = next.first;
    const Trk::Layer* layer = next.second;
    // stop extrapolation - could be done in a better way - return current position for further extrapolation ?
    if ( !layer ) nextPar = 0;

    if(nextPar){
      if(nextPar->momentum().unit().dot(currPar->momentum().unit()) < 0) nAttempts++;
    }
    else nAttempts++;

    if(nAttempts > 30){
      ATH_MSG_INFO("getHolesOnTrack: too many attempts extrapolating; aborting extrapolation " 
	  << " and returning NO holes !!");
      break;
      return 0;
    }
    
    const Trk::TrackParameters* updatePar = 0;

    // check if extrapolation ok
    bool failedExtr = false;
    if (nextLayer) {
      if (!nextPar) {
        failedExtr = true;
      } else if (layer != nextLayer ) {
	Trk::DistanceSolution dist = nextLayer->surfaceRepresentation().straightLineDistanceEstimate(nextPar->position(),nextPar->momentum().unit());
	if (  (dist.numberOfSolutions()>0 && dist.first()<=0) ||
	      (dist.numberOfSolutions()==0 && dist.toPointOfClosestApproach()<0) )   failedExtr = true;
      }  
      if (failedExtr) {
	// try to recover
	//std::cout <<"extrapolate to missed surface:"<<nextLayer->surfaceRepresentation().center() << std::endl; 
	const Trk::TrackParameters* repPar = m_extrapolator->extrapolate(*currPar,nextLayer->surfaceRepresentation(),
									 Trk::alongMomentum,false,particle) ;
	temp_holes->clear();
	//std::cout << "trying to recover with parameters:"<< repPar->position() << std::endl;
	if (repPar) {
	  int assocMeas = 0;
	  double res = -1.;
	  updatePar = findAssociatedMeasurement(repPar,nextLayer,tSoS,assocMeas,res);
	  //std::cout << "update?"<<updatePar <<","<<res<<","<<assocMeas <<std::endl;
	  if (updatePar) { 
	    //std::cout << "updated position:" << updatePar->position() << std::endl;
	    nextPar = updatePar;
            layer = nextLayer;
	    updatePar = 0;
	    failedExtr = false;
	  } else {
	    nextPar = updatePar;
            layer = nextLayer;
	    failedExtr = false;
	  }	  
          delete repPar;
	}
      }
    }   
    if ( failedExtr ) {
      ATH_MSG_WARNING( name() << "lost in extrapolation, abort");
      delete temp_holes;
      return 0;
    }
    if (!nextPar) break;

    // we are interested in muon layers only !
    if (layer) {
      const Trk::DetachedTrackingVolume* detVol = layer->enclosingDetachedTrackingVolume();
      if (!detVol) ATH_MSG_ERROR( name() << "no enclosing detached volume?!");
      if (!detVol) layer=0;
    }
    if (!layer) {
      ATH_MSG_WARNING( name() << " no associated layer?!");
    } else {
      int assocMeas = 0;
      double res = -1.;
      updatePar = findAssociatedMeasurement(nextPar,layer,tSoS,assocMeas,res);
      // create hole :assocMeas>0 signals associated measurement OR hole (1/eta,2/phi,3/both)
       
      unsigned int lId = layer->layerType();  
      Identifier layId(lId);
      if ( assocMeas<1  &&  lId && m_mdtIdHelper->is_mdt(layId)  ) {
        bool measPhi = false;
	const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
        if (hole) temp_holes->push_back(hole); 
	const Trk::TrackStateOnSurface* outlier = createOutlier(nextPar,layer,measPhi);
        if (outlier) temp_holes->push_back(outlier); 
      }

      if ( assocMeas<3  &&  lId && !m_mdtIdHelper->is_mdt(layId) ) {
        // create a hole 
        if ( m_tgcIdHelper->is_tgc(layId) && assocMeas<2) {
          if (m_tgcIdHelper->gasGap(layId)==2 && m_tgcIdHelper->gasGapMax(layId)==3) assocMeas+=2;
          //if (layId == 0) std::cout << "TGC layer not identified, this phi hole can be fake" << std::endl;
        }
	if (assocMeas<2){
	  bool measPhi = true;
	  const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
	  if (hole) temp_holes->push_back(hole); 
	  const Trk::TrackStateOnSurface* outlier = createOutlier(nextPar,layer,measPhi);
	  if (outlier) temp_holes->push_back(outlier); 
	}
	if (assocMeas==0 || assocMeas==2){
	  bool measPhi = false;
	  const Trk::TrackStateOnSurface* hole = createHole(nextPar,layer,measPhi);
	  if (hole) temp_holes->push_back(hole); 
	  const Trk::TrackStateOnSurface* outlier = createOutlier(nextPar,layer,measPhi);
	  if (outlier) temp_holes->push_back(outlier); 
	}
      }
      // collect holes if extrapolation looks reasonably enough 
      if (assocMeas>0 && res < 50. )  {
	for (unsigned int ih=0; ih < temp_holes->size(); ih++) holes->push_back((*temp_holes)[ih]);   
	temp_holes->clear();
	//std::cout << "saving temp holes:" << holes->size() << std::endl;
      }
    }
    currPar=nextPar;
    if (updatePar) currPar=updatePar;
  }
  // collect remaining temp_holes && copy hits
  for (unsigned int ih=0; ih < temp_holes->size(); ih++) holes->push_back((*temp_holes)[ih]);  
  for (unsigned int ih=0; ih < tSoS->size(); ih++) holes->push_back(new Trk::TrackStateOnSurface(*((*tSoS)[ih])));  

  delete temp_holes;
  // order
  Muon::TrackStateOnSurfaceComparisonFunction fOrd(msEntry->momentum());
 
  DataVector<const Trk::TrackStateOnSurface>::iterator hit  = holes->begin();
  DataVector<const Trk::TrackStateOnSurface>::iterator hend = holes->end();
  // 
  sort( hit, hend, fOrd );

  // create track
  return new Trk::Track(input_track.info(),holes, new Trk::FitQuality(*(input_track.fitQuality())));
}

// Finalize method:
StatusCode Muon::MuonHolesOnTrackTool::finalize() 
{
  // Get the messaging service, print where you are
  ATH_MSG_INFO("MuonHolesOnTrackTool::finalize()");
  //delete m_tpMinFinder;
  // clean up - crashing
  //for (unsigned int i=0;i<m_garbage.size();i++) delete m_garbage[i]; 

  return StatusCode::SUCCESS;
}

const Trk::TrackParameters* Muon::MuonHolesOnTrackTool::getMSEntry(const Track* input_track, DataVector<const Trk::TrackStateOnSurface>*& orderedTSoS) const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG( name() << "entering search for intersection with MuonSpectrometerEntrance" );

  const Trk::TrackParameters* msEntry = 0;

  if (!m_msEntrance) m_msEntrance = m_extrapolator->trackingGeometry()->trackingVolume("MuonSpectrometerEntrance");

  // retrieve TSoS 
  const DataVector<const Trk::TrackStateOnSurface>* tSoS = input_track->trackStateOnSurfaces();
  std::vector <const Trk::TrackStateOnSurface*> ntSoS;
  ntSoS.reserve(tSoS->size());
  DataVector<const Trk::TrackStateOnSurface>::const_iterator iter = tSoS->begin();
  // unpack competing muon clusters (RPC and TGC)
  for (;iter!=tSoS->end();iter++) {
    //if ( (*iter)->type(Trk::TrackStateOnSurface::Measurement) ) {
    if ( (*iter)->type(Trk::TrackStateOnSurface::Measurement) || (*iter)->type(Trk::TrackStateOnSurface::Outlier) ){
      const Muon::CompetingMuonClustersOnTrack* muclust = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*> ((*iter)->measurementOnTrack()); 
      if(muclust){
	const std::vector<const Muon::MuonClusterOnTrack*>& murots = muclust->containedROTs(); 
	//iterate over the constituent rots to check if a measurement is associated to it
	if(murots.begin() != murots.end()){
	  for (std::vector<const Muon::MuonClusterOnTrack*>::const_iterator rotIter = murots.begin(); 
	       rotIter != murots.end(); rotIter++){
	    Trk::FitQualityOnSurface* dummy_fQ=0;
	    Trk::MaterialEffectsBase* dummy_sa=0;
            const Trk::TrackParameters* par = m_extrapolator->extrapolateDirectly(*((*iter)->trackParameters()),(*rotIter)->associatedSurface(),Trk::anyDirection,false,Trk::muon);
	    const Trk::TrackStateOnSurface* nRIO=0;
            if((*iter)->type(Trk::TrackStateOnSurface::Measurement)){
	      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;  
	      typePattern.set(Trk::TrackStateOnSurface::Measurement);       		
              nRIO = new Trk::TrackStateOnSurface((*rotIter)->clone(),par,dummy_fQ,dummy_sa,typePattern);
            }
            else if((*iter)->type(Trk::TrackStateOnSurface::Outlier)){
	      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;  
	      typePattern.set(Trk::TrackStateOnSurface::Measurement);       		
	      typePattern.set(Trk::TrackStateOnSurface::Outlier);       		
              nRIO = new Trk::TrackStateOnSurface((*rotIter)->clone(),par,dummy_fQ,dummy_sa,typePattern);
            }
            //const Trk::TrackStateOnSurface* nRIO = new Trk::TrackStateOnSurface(*rotIter,par,dummy_fQ,dummy_sa,Trk::TrackStateOnSurface::Measurement);
            ntSoS.push_back(nRIO);
          }
	}
      } else {
         ntSoS.push_back((*iter)->clone());
      }
    } else {
      ntSoS.push_back((*iter)->clone());
    }
  }
  // retrieve starting point - perigee 
  const Trk::Perigee* peri = input_track->perigeeParameters();
  // order TSoS

   Amg::Vector3D refDir = peri->momentum();
   Muon::TrackStateOnSurfaceComparisonFunction fOrd(refDir);

   std::vector<const Trk::TrackStateOnSurface*>::iterator it  = ntSoS.begin();
   std::vector<const Trk::TrackStateOnSurface*>::iterator it_end  = ntSoS.end();
   sort(it,it_end,fOrd);

   orderedTSoS = new DataVector<const Trk::TrackStateOnSurface>;  

   for (; it!=it_end; ++it) {
     orderedTSoS->push_back(*it);
   }

  if (!m_msEntrance) {
    ATH_MSG_WARNING("MuonSpectrometerEntrance not found, return 0");
    return msEntry;
  } 

  DataVector<const Trk::TrackStateOnSurface>::const_iterator tit  = orderedTSoS->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tend = orderedTSoS->end();

  for ( ; tit!=tend; ++tit) {
    if ( (*tit)->trackParameters() ) {
      // MS entrance
      if ( m_extrapolator->trackingGeometry()->atVolumeBoundary( (*tit)->trackParameters()->position(), m_msEntrance, 100. ) ) {
        msEntry = (*tit)->trackParameters()->clone();
	//std::cout << "muon entrance parameters found in track record:" << msEntry->position() << std::endl;
        return msEntry;
      }
      // first muon measurement
      if ( (*tit)->type(Trk::TrackStateOnSurface::Measurement) && !m_msEntrance->inside((*tit)->trackParameters()->position(),0.) ) {
	msEntry = m_extrapolator->extrapolateToVolume(*((*tit)->trackParameters()),*m_msEntrance,Trk::oppositeMomentum,Trk::muon);
        if (!msEntry) msEntry = (*tit)->trackParameters()->clone();
        return msEntry; 
      }
    }
  }
  return msEntry;
}  

const Trk::TrackParameters* Muon::MuonHolesOnTrackTool::findAssociatedMeasurement(const Trk::TrackParameters* nextPar,
										  const Trk::Layer* layer,
										  const DataVector<const Trk::TrackStateOnSurface>* tSoS,
										  int& assocMeas, double& res) const
{
  const Trk::TrackParameters* updatePar = 0;
  assocMeas = 0;

  DataVector<const Trk::TrackStateOnSurface>::const_iterator tbeg = tSoS->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tend = tSoS->end();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tit  = tSoS->begin();

  bool assoc         = false;
  bool assocMeasEtaZ = false;
  bool assocMeasPhi  = false;
  for ( tit=tbeg; tit!=tend; ++tit) {
    if ((*tit)->type(Trk::TrackStateOnSurface::Hole) ) {
      if   ( layer->isOnLayer((*tit)->trackParameters()->associatedSurface().center())   ) { 
	Identifier hid = (*tit)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier() ;
        if (hid.get_identifier32().get_compact()>0) {
          assoc = true;
	  if (m_mdtIdHelper->is_rpc(hid)) {
	    if ( m_rpcIdHelper->measuresPhi(hid) ) {assocMeasPhi = true;}
	    else { assocMeasEtaZ = true;}
	  }
	  if (m_mdtIdHelper->is_csc(hid)) {
	    if ( m_cscIdHelper->measuresPhi(hid) ) {assocMeasPhi = true;}
	    else { assocMeasEtaZ = true;}
	  }
	  if (m_mdtIdHelper->is_tgc(hid)) {
	    if ( m_tgcIdHelper->isStrip(hid) ) {assocMeasPhi = true;}
	    else { assocMeasEtaZ = true;}
	  }
	}
      }
    }
    //if ((*tit)->type(Trk::TrackStateOnSurface::Measurement) ) {
    if ((*tit)->type(Trk::TrackStateOnSurface::Measurement) || (*tit)->type(Trk::TrackStateOnSurface::Outlier)) {
      if   ( layer->isOnLayer((*tit)->measurementOnTrack()->associatedSurface().center())   ) {
	//std::cout << "associated measurement found:" << tit-tbeg << std::endl;
        assoc = true; 
	const Trk::TrackParameters* trUpdate = (*tit)->trackParameters();
        if (trUpdate) {
	  //std::cout << "track record:" << trUpdate->position() <<"," << trUpdate->momentum() << std::endl; 
	  // verify that surfaces are compatible
	  Identifier id1 = (*tit)->measurementOnTrack()->associatedSurface().associatedDetectorElementIdentifier(); 
	  Identifier id2 = trUpdate->associatedSurface().associatedDetectorElementIdentifier(); 
	  if ( id1.get_identifier32().get_compact() && id2.get_identifier32().get_compact() && id1==id2 ) {   // information consistent 
	    //std::cout << "residual:" << m_measTool->residual(layer,nextPar,id1);
	    if (m_parUpdate ){
	      // check the parameters for update
	      double oa = nextPar->momentum().unit().dot(trUpdate->momentum().unit());
	      if (oa > 0.) {
		delete updatePar;
		updatePar = m_measTool->detElToLayer( layer, trUpdate, id1);
	      }
	      //if (updatePar) std::cout << "projected parameters for update:" << updatePar->position() << std::endl;
	    }
	  } else {
	    ATH_MSG_WARNING( name() <<"track parameters in track record not compatible with hit surface ");          
	  }
	}
	else {
	  ATH_MSG_WARNING( name() <<"no associated surface (NULL pointer)");
	}
       
	const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*> ((*tit)->measurementOnTrack());
	if (rio) {
	  Identifier id = rio->identify();
          res = fabs(m_measTool->residual(nextPar,id));
	  if (m_mdtIdHelper->is_rpc(id)) {
	    if ( m_rpcIdHelper->measuresPhi(id) ) {assocMeasPhi = true;}
	    else { assocMeasEtaZ = true;}
	  }
	  if (m_mdtIdHelper->is_csc(id)) {
	    if ( m_cscIdHelper->measuresPhi(id) ) {assocMeasPhi = true;}
	    else { assocMeasEtaZ = true;}
	  }
	  if (m_mdtIdHelper->is_tgc(id)) {
	    if ( m_tgcIdHelper->isStrip(id) ) {assocMeasPhi = true;}
	    else { assocMeasEtaZ = true;}
	  }
	}
      }
    } 
  }
  if (assoc) assocMeas = 1;
  if (assocMeasPhi || assocMeasEtaZ) {
    if (!assocMeasEtaZ) assocMeas = 0;
    if (assocMeasPhi)   assocMeas = assocMeas + 2;     
  }   
  
  return updatePar;
}

const Trk::TrackStateOnSurface* Muon::MuonHolesOnTrackTool::createHole(const Trk::TrackParameters* nextPar,
								       const Trk::Layer* layer,
								       bool measPhi) const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonHolesOnTrackTool::createHole()");

  const Trk::TrackStateOnSurface* hole = 0;
  const Trk::MaterialEffectsBase* dummy_sa=0;  
  const Trk::RIO_OnTrack* dummy_rio=0;
  const Trk::FitQualityOnSurface* dummy_fitQuality=0;
  const Trk::TrackParameters* hPar = 0;
  bool inRange = true;
  if (m_identifyHoles && m_measTool ) {
    double pitch;
    Identifier refId = m_measTool->nearestDetEl(layer,nextPar,measPhi,pitch);
    if ( refId.get_identifier32().get_compact() > 0 ) {
      hPar = m_measTool->layerToDetEl(layer,nextPar,refId);
      //if (!hPar) std::cout << "WARNING: projection of parameters failed" << std::endl;
    } else {
      inRange = false;  
    }
  } else {
    hPar = nextPar->clone();
  }
  //std::cout << "check hole creation:"<< <<std::endl;
  //if (hPar) std::cout << "check hole creation:"<<hPar->associatedSurface() <<std::endl;
  if (inRange && hPar) {
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;  
    typePattern.set(Trk::TrackStateOnSurface::Hole);       		
    hole=new Trk::TrackStateOnSurface(dummy_rio,hPar,dummy_fitQuality,dummy_sa,typePattern); 
  }
  //if (hole) std::cout << "check hole par:"<<hole->trackParameters()<< std::endl;
  //if (hole) std::cout << "check hole associated surface:"<<hole->trackParameters()->associatedSurface() << std::endl;
  return hole;
}

const Trk::TrackStateOnSurface* Muon::MuonHolesOnTrackTool::createOutlier(const Trk::TrackParameters* nextPar,
									  const Trk::Layer* layer,
									  bool measPhi) const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonHolesOnTrackTool::createOutlier()");

  if (!m_measTool) return 0;

  const std::vector<const Trk::PrepRawData*>* measOnLayer = m_measTool->getMeasurementOnLayer(layer);
  if (measOnLayer) {
    for (unsigned int im=0; im < measOnLayer->size(); im++) {
      const Trk::RIO_OnTrack* rio = m_rotCreator->correct(*((*measOnLayer)[im]),*nextPar);
      bool rioMeasPhi=false;   
      if (rio) {
	Identifier id = rio->identify();
	if ( m_mdtIdHelper->is_rpc(id) && m_rpcIdHelper->measuresPhi(id) ) rioMeasPhi = true;
	if ( m_mdtIdHelper->is_csc(id) && m_cscIdHelper->measuresPhi(id) ) rioMeasPhi = true;
	if ( m_mdtIdHelper->is_tgc(id) && m_tgcIdHelper->isStrip(id) )     rioMeasPhi = true;
      }
      if ( (!measPhi && !rioMeasPhi) || (measPhi && rioMeasPhi) ) {
	if ( rio && m_measTool->residual(layer,nextPar,rio) < m_outlierLim ) {
	  ATH_MSG_DEBUG("getHolesOnTrack: creating outlier:" <<m_measTool->residual(layer,nextPar,rio) );
	  const Trk::MaterialEffectsBase* dummy_sa=0;
	  const Trk::FitQualityOnSurface* dummy_fitQuality=0;
	  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;  
	  typePattern.set(Trk::TrackStateOnSurface::Measurement);       		
	  typePattern.set(Trk::TrackStateOnSurface::Outlier);       		
	  const Trk::TrackStateOnSurface* outlier = new Trk::TrackStateOnSurface(rio,nextPar->clone(),dummy_fitQuality,
										       dummy_sa,typePattern);
	  delete measOnLayer;
	  return outlier; 
	} 
      }
    }
    delete measOnLayer;     
  }      

  return 0;
}

const Trk::Layer* Muon::MuonHolesOnTrackTool::nextExpectedLayer(const Trk::TrackParameters* currPar,
							    const DataVector<const Trk::TrackStateOnSurface>* tSoS) const
{
  const Trk::Layer* nextLayer = 0;
  double tol = 0.001;
  // current layer 
  const Trk::Layer* currLayer = m_extrapolator->trackingGeometry()->associatedLayer(currPar->position());
  //
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tbeg = tSoS->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tend = tSoS->end();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tit  = tSoS->begin();

  for ( tit=tbeg; tit!=tend; ++tit) {
    //if ((*tit)->type(Trk::TrackStateOnSurface::Measurement) ) {
    if ((*tit)->type(Trk::TrackStateOnSurface::Measurement) || (*tit)->type(Trk::TrackStateOnSurface::Outlier)) {
      // only muon hits matter
      if (m_msEntrance && !m_msEntrance->inside((*tit)->measurementOnTrack()->associatedSurface().center(),0.)) {
	// associated layer
	const Trk::Layer* next = m_extrapolator->trackingGeometry()->associatedLayer((*tit)->measurementOnTrack()->associatedSurface().center());
	if (!next) {
	  ATH_MSG_WARNING( name() << " no associated layer found for next measurement at position " << (*tit)->measurementOnTrack()->associatedSurface().center() );
	} else if ( next!=currLayer) { 
	  Trk::DistanceSolution distEst = next->surfaceRepresentation().straightLineDistanceEstimate(currPar->position(), currPar->momentum().unit());
	  //std::cout << "estimating distance to meas.:" << tit-tbeg <<"," << distEst.numberOfSolutions() <<"," <<
	  //  distEst.first() << "," << distEst.toPointOfClosestApproach() << std::endl; 
	  if  ( distEst.numberOfSolutions()>0 && distEst.first()> tol ) {
	    nextLayer = next;
	    break; 
	  }
	}
      }
    }
  }
  return nextLayer;
}

void Muon::MuonHolesOnTrackTool::countHoles( const Trk::Track& input_track,  std::vector<int>& information , const Trk::ParticleHypothesis particle) const 
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonHolesOnTrackTool::countHoles");
  
  //
  DataVector<const Trk::TrackStateOnSurface>*  holes= getHolesOnTrack(input_track, particle);
 
  if (!holes)  return;
  if ( information.size()<28 ) return;

  for (unsigned int i=21; i<28; i++) information[i]=0; 

  DataVector<const Trk::TrackStateOnSurface>::iterator hit  = holes->begin();
  DataVector<const Trk::TrackStateOnSurface>::iterator hend = holes->end();

  for (;hit!=hend; hit++) {

    Identifier	hid = (*hit)->trackParameters()->associatedSurface().associatedDetectorElementIdentifier() ;
    if (hid.get_identifier32().get_compact()>0) {
      if (m_mdtIdHelper->is_mdt(hid)) information[25]++;   

      else if (m_mdtIdHelper->is_rpc(hid)) {
	if ( m_rpcIdHelper->measuresPhi(hid) ) information[24]++; 
	else                                   information[23]++; 
      
      } else if (m_mdtIdHelper->is_csc(hid)) {
	if ( m_cscIdHelper->measuresPhi(hid) ) information[22]++; 
	else                                   information[21]++; 
	  
      } else if (m_mdtIdHelper->is_tgc(hid)) {
	if ( m_tgcIdHelper->isStrip(hid) )     information[27]++; 
	else                                   information[26]++; 
      }
    }
  }
  return;
}
