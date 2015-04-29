/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonTGMeasurementTool.cxx
//   Implementation file for class MuonTGMeasurementTool
///////////////////////////////////////////////////////////////////
// S.Todorova
///////////////////////////////////////////////////////////////////

#include "MuonTGRecTools/MuonTGMeasurementTool.h"

// Gaudi includes
#include "StoreGate/StoreGate.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/FitQualityOnSurface.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"
#include "TrkParameters/TrackParameters.h"

//#include "TrkEventPrimitives/MaterialEffectsOnTrack.h"
// #include "TrkParameters/MeasuredPerigee.h"
// #include "TrkParameters/MeasuredTrackParameters.h"
// #include "TrkParameters/MeasuredAtaPlane.h"
// #include "TrkParameters/MeasuredAtaStraightLine.h"
// #include "TrkParameters/Perigee.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkSurfaces/DistanceSolution.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
// #include "TrkParameters/AtaCylinder.h"
// #include "TrkParameters/AtaPlane.h"
// #include "TrkParameters/AtaDisc.h"
// #include "TrkParameters/AtaStraightLine.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrackRecord/TrackRecord.h"
#include <vector>

//#include "MuonDigitContainer/MdtDigitContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"

// Constructor with parameters:
Muon::MuonTGMeasurementTool::MuonTGMeasurementTool(const std::string &type, const std::string &name, const IInterface* parent ) :
  AthAlgTool(type,name,parent),
  m_trackingGeometry(0),
  m_trackingGeometryName("AtlasTrackingGeometry"),
  m_ExtrapolatorName(" "),
  //m_assocMeasName("MuonTGMeasAssocAlg"),
  m_StoreGate(0),
  m_extrapolator(0),
  m_alignedMode(true)
{  
  declareInterface<Muon::IMuonTGMeasTool>(this);

  // Get parameter values from jobOptions file
  declareProperty("ExtrapolatorName", m_ExtrapolatorName);
  declareProperty("TrackingGeometryName", m_trackingGeometryName);
  declareProperty("AlignedMode", m_alignedMode);
}

// Initialize method:
StatusCode Muon::MuonTGMeasurementTool::initialize()
{

  // Get the messaging service, print where you are
  ATH_MSG_INFO("MuonTGMeasurementTool::initialize()");

  StatusCode sc;

  // Get an Identifier helper object
  StoreGateSvc* detStore(0);
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Detector service not found !");
    return StatusCode::FAILURE;
  } 

  sc=service("StoreGateSvc",m_StoreGate);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("StoreGate service not found !");
    return StatusCode::FAILURE;
  } 
  // Store Gate active store
  sc = service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR(" Cannot get ActiveStoreSvc ");
    return sc ;
  }
 
  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve MdtIdHelper");
	  return sc;
  }

  sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve RpcIdHelper");
	  return sc;
  }

  sc = detStore->retrieve(m_cscIdHelper,"CSCIDHELPER");
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve RpcIdHelper");
	  return sc;
  }

  sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve RpcIdHelper");
	  return sc;
  }
  
  sc = detStore->retrieve(m_muonMgr);
  if (sc.isFailure())
  {
	  ATH_MSG_ERROR("Cannot retrieve MuonDetectorManager...");
	  return sc;
  }

  // define projection matrices
  m_tgcProjEta = new AmgMatrix(5,5);
  m_tgcProjEta->setIdentity();
  (*m_tgcProjEta)(0,0) = 0.; (*m_tgcProjEta)(1,1) = 0.;
  (*m_tgcProjEta)(0,1) = 1.; (*m_tgcProjEta)(1,0) =-1.;
  m_tgcProjPhi = new AmgMatrix(5,5);
  m_tgcProjPhi->setIdentity();

  //std::cout << "dump projection matrix tgc,eta:" << (*m_tgcProjEta) << std::endl;

  //std::cout << "dump projection matrix tgc,phi:" << (*m_tgcProjPhi) << std::endl;

  m_rpcProjEta = new AmgMatrix(5,5);
  m_rpcProjEta->setIdentity();
  (*m_rpcProjEta)(0,0) = 0.; (*m_rpcProjEta)(1,1) = 0.;
  (*m_rpcProjEta)(0,1) = 1.; (*m_rpcProjEta)(1,0) = 1.;
  m_rpcProjPhi = new AmgMatrix(5,5);
  m_rpcProjPhi->setIdentity();

  //(*m_rpcProjPhi)[0][0]= -1.;
  //std::cout << "dump projection matrix rpc,eta:" << (*m_rpcProjEta) << std::endl;

  //std::cout << "dump projection matrix rpc,phi:" << (*m_rpcProjPhi) << std::endl;


 
  return StatusCode::SUCCESS;
}

// Finalize method:
StatusCode Muon::MuonTGMeasurementTool::finalize() 
{
  // Get the messaging service, print where you are
  ATH_MSG_INFO("MuonTGMeasurementTool::finalize()");
  //delete m_tpMinFinder;
  delete m_tgcProjEta;
  delete m_tgcProjPhi;
  delete m_rpcProjEta;
  delete m_rpcProjPhi;
  return StatusCode::SUCCESS;
}

const std::vector<const Trk::PrepRawData*>* Muon::MuonTGMeasurementTool::getMeasurementOnLayer(const Trk::Layer* lay) const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("Muon::MuonTGMeasurementTool::getMeasurementOnLayer");
  const std::vector<const Trk::PrepRawData*>* hitsOnLayer = 0; 
  // 
  if (!m_trackingGeometry) {   
    StatusCode sc = getTrackingGeometry();
    if ( sc.isFailure() ) return hitsOnLayer; 
  } 

  /*
  std::cout << "re-check MuonTGHits:" <<m_hits<< std::endl;
  if (m_hits) {
    for (unsigned is = 0; is < m_hits->size(); is++) {
      std::cout << " MuonTGHits:station:" <<  (*m_hits)[is]->first->name()<< std::endl;
      const std::vector<PairOfLayerPrd*>* layHit = (*m_hits)[is]->second;
      for (unsigned il = 0; il < layHit->size(); il++ ) {
	std::cout << " MuonTGHits:layer:" << il << "," << (*layHit)[il]->first->surfaceRepresentation().center() << "," << (*layHit)[il]->second->size() << std::endl;      
      }  
    }
  }
  */

  if (m_hits && lay) {
    const Trk::DetachedTrackingVolume* station = lay->enclosingDetachedTrackingVolume();
    if (!station) ATH_MSG_WARNING("no enclosing station found");
    if (station) {
      ATH_MSG_DEBUG("enclosing station found:"<< station->name() );
      unsigned int ist=0;
      while ( ist < m_hits->size() ) {
	if ((*m_hits)[ist]->first == station ){
	  unsigned int il=0;                  
	  while ( il < (*m_hits)[ist]->second->size() ) {                 
	    if ((*((*m_hits)[ist]->second))[il]->first == lay ){
	      hitsOnLayer = new std::vector<const Trk::PrepRawData*>(*((*((*m_hits)[ist]->second))[il]->second)); break;
	    }
            il++; 
	  }    
	  break;
	} 
        ist++;
      }   
    }
  }

  if (hitsOnLayer) ATH_MSG_DEBUG("Muon::MuonTGMeasurementTool returns "<< hitsOnLayer->size() << " measurements on layer");

  return hitsOnLayer;
}

const std::vector<const Trk::PrepRawData*>* Muon::MuonTGMeasurementTool::getEtaPhiMeasurementOnLayer(const Trk::Layer* lay, bool phi) const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("Muon::MuonTGMeasurementTool::getEtaPhiMeasurementOnLayer");
  const std::vector<const Trk::PrepRawData*>* meas = getMeasurementOnLayer(lay);
  std::vector<const Trk::PrepRawData*>* hitsOnLayer = new std::vector<const Trk::PrepRawData*>;
   
  if ( meas) {
    for (unsigned int ih=0; ih<meas->size(); ih++) {
      const Identifier id = (*meas)[ih]->identify();
      if ( m_mdtIdHelper->is_mdt(id) ) hitsOnLayer->push_back( (*meas)[ih] );
      if ( m_rpcIdHelper->is_rpc(id) ) {
	if ( m_rpcIdHelper->measuresPhi(id) == phi)  hitsOnLayer->push_back( (*meas)[ih] );
      }
      if ( m_cscIdHelper->is_csc(id) ) {
	if ( m_cscIdHelper->measuresPhi(id) == phi)  hitsOnLayer->push_back( (*meas)[ih] );
      }
      if ( m_tgcIdHelper->is_tgc(id) ) {
	if ( m_tgcIdHelper->isStrip(id) == phi)  hitsOnLayer->push_back( (*meas)[ih] );
      }
    } 
    delete meas;
  } 
  
//  return new std::vector<const Trk::PrepRawData*>(*hitsOnLayer);
  return hitsOnLayer;
}

const Muon::MuonTGSegments* Muon::MuonTGMeasurementTool::getAllSegments() const
{
  return m_segments;
}

const Muon::MuonTGHits* Muon::MuonTGMeasurementTool::getAllHits() const
{
  return m_hits;
}
 
const std::vector<const Trk::Segment*>* Muon::MuonTGMeasurementTool::getSegments(const Trk::DetachedTrackingVolume* station) const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("Muon::MuonTGMeasurementTool::getSegments");
  const std::vector<const Trk::Segment*>* segments = 0; 
  // 
  if (!m_trackingGeometry) {   
    StatusCode sc = getTrackingGeometry();
    if ( sc.isFailure() ) return segments; 
  } 

  /*
  std::cout << "re-check MuonTGHits:" <<m_hits<< std::endl;
  if (m_hits) {
    for (unsigned is = 0; is < m_hits->size(); is++) {
      std::cout << " MuonTGHits:station:" <<  (*m_hits)[is]->first->name()<< std::endl;
      const std::vector<PairOfLayerPrd*>* layHit = (*m_hits)[is]->second;
      for (unsigned il = 0; il < layHit->size(); il++ ) {
	std::cout << " MuonTGHits:layer:" << il << "," << (*layHit)[il]->first->surfaceRepresentation().center() << "," << (*layHit)[il]->second->size() << std::endl;      
      }  
    }
  }
  */

  if (m_segments && station) {
    unsigned int ist=0;
    while ( ist < m_segments->size() ) {
      if ((*m_segments)[ist]->first == station ){
	segments = new std::vector<const Trk::Segment*>(*((*m_segments)[ist]->second)); 
	break;
      } 
      ist++;   
    }
  }

  if (segments) ATH_MSG_DEBUG("Muon::MuonTGMeasurementTool returns "<< segments->size() << " segments");

  return segments;
}

const Trk::TrackParameters* Muon::MuonTGMeasurementTool::layerToDetEl(const Trk::Layer* lay, const Trk::TrackParameters* parm, Identifier id) const
{
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG("MuonTGMeasurementTool::layerToDetEl");
    const Trk::TrackParameters* projPar = 0;
    //const Amg::Vector2D* locPos = 0;
    // check input
    if (!lay || !parm || !id.get_identifier32().get_compact() ) return projPar;

    // get tracking geometry
    if (!m_trackingGeometry) {   
      StatusCode sc = getTrackingGeometry();
      if ( sc.isFailure() ) return projPar; 
    } 

    // check compatibility of layer info and required id ? this was already done when associating !
    if (!lay->layerType()) return projPar;
    Identifier layId(lay->layerType()); 
   
    unsigned int hitType = 0;
    if ( m_mdtIdHelper->is_mdt(id) ) hitType = 1;    
    if ( m_mdtIdHelper->is_rpc(id) ) hitType = 2;    
    if ( m_mdtIdHelper->is_csc(id) ) hitType = 3;    
    if ( m_mdtIdHelper->is_tgc(id) ) hitType = 4;

    unsigned int layType = 0;
    if ( m_mdtIdHelper->is_mdt(layId) ) layType = 1;    
    if ( m_mdtIdHelper->is_rpc(layId) ) layType = 2;    
    if ( m_mdtIdHelper->is_csc(layId) ) layType = 3;    
    if ( m_mdtIdHelper->is_tgc(layId) ) layType = 4;

    if ( layType != hitType ) return projPar; 

    if ( hitType==0 ) {
      ATH_MSG_ERROR("unknown hit technology");
      return projPar; 
    } else {
      ATH_MSG_DEBUG("hit technology:"<< hitType );
    }   

    ATH_MSG_DEBUG("extrapolated covariance:" << parm->covariance() );   

    //std::cout << "layerToDetEl::hit type:" << hitType << std::endl;

    if ( hitType == 1) {
      const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(id);			
      if (!mdtROE) {
        ATH_MSG_WARNING( name() << "MDT readout element not found");
        return projPar;
      }
      // local position of tube
      int tube = m_mdtIdHelper->tube(id); 
      Amg::Vector2D locWire(0.,lay->getRef()+(tube-1)*30.035);
      if (fabs(lay->getRef())>10e6) {
        double sign = (lay->getRef()>0) ? 1.:-1.;
        int dec = int(lay->getRef()/1.e5);
        double ref0 = dec/1.e5;
        double ref1 = lay->getRef()-dec*1e5-0.5*(sign+1)*1e5;
        locWire[0]=ref0; locWire[1]=ref1;
        int tube = m_mdtIdHelper->tube(id);
        int tubeMax = m_mdtIdHelper->tubeMax(id);
        if ( tube>6 && tubeMax-tube>5 ) locWire[0]=0.; 
      }
      if (sqrt(locWire[0]*locWire[0]+locWire[1]*locWire[1])>2000.) {
	ATH_MSG_WARNING( name() << " wire shift out bounds for MDT tube :" << m_mdtIdHelper->stationName(id) <<","<<
	  m_mdtIdHelper->stationEta(id) <<","<< m_mdtIdHelper->stationPhi(id) <<": abandon projection");
        return projPar;
      }
      // direction at the layer
      Amg::Vector3D dir =  parm->momentum().unit();
      Amg::Vector3D locDir = (lay->surfaceRepresentation().transform().inverse()) * dir;
      Amg::Vector3D wireDir(1.,0.,0.);
      //
      double ND = dir.dot(lay->surfaceRepresentation().normal());
      double DL = locDir.dot(wireDir);
      double A  = sqrt(1.-DL*DL);   
      AmgMatrix(5,5) mdtProj;
      mdtProj.setIdentity();
      //
      const Trk::StraightLineSurface* tubeSurf = dynamic_cast<const Trk::StraightLineSurface*> (&(mdtROE->surface(id)));
      double ori = (lay->surfaceRepresentation().transform().inverse()*tubeSurf->transform()).rotation()(1,1)>0. ? -1.:1.;
      if ( A > 10e-6 ) {
        mdtProj(0,1)= ND/A; mdtProj(1,0) = ori; mdtProj(1,1)= DL/A*sqrt(A*A-ND*ND)/A; mdtProj(0,0)=0.;
      } else {
        mdtProj(0,1)= 1.; mdtProj(1,0) = ori; mdtProj(1,1)=0.; mdtProj(0,0)=0.;
      } 
      //
      Amg::VectorX locPar = parm->parameters();
      locPar[0] -= locWire[0];
      locPar[1] -= locWire[1];
      Amg::VectorX pPar = mdtProj * locPar;
      ATH_MSG_DEBUG("projected parameters(layer->MDT):"<< pPar );

      if ( parm->covariance() ) {

	AmgMatrix(5,5)* projEM = new AmgMatrix(5,5);
	*projEM = parm->covariance()->similarity(mdtProj);
	ATH_MSG_DEBUG("projected covariance(layer->MDT):" << (*projEM) );
	projPar = new Trk::AtaStraightLine(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*tubeSurf,projEM);

      } else {
        projPar = new Trk::AtaStraightLine(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*tubeSurf );             
      }
    }

    if ( hitType == 2) {
      //
      const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(id);			
      if (!rpcROE) return projPar;
      const Trk::PlaneSurface* stripSurf = dynamic_cast<const Trk::PlaneSurface*> (&(rpcROE->surface(id)));
      if (!stripSurf) return projPar;
      // decode ref position
      Amg::VectorX locPar = parm->parameters();
      // projection matrix
      AmgMatrix(5,5)* pMx = 0;
      if (m_rpcIdHelper->measuresPhi(id)) pMx = m_rpcProjPhi;
      else                                pMx = m_rpcProjEta;
      // projected parameters 
      // double eta = (m_rpcIdHelper->stationEta(id)<0) ? -1. : 1.;
      double eta = 1.;
      double sign = (m_rpcIdHelper->measuresPhi(id) && m_rpcIdHelper->doubletPhi(id)==2 ) ? -1. : 1.;
      //double zswap = ( (eta>0 && m_rpcIdHelper->doubletZ(id)==1) ||
      //                 (eta<0 && m_rpcIdHelper->doubletZ(id)==2) )  ? -1. : 1.;
      // double zswap = ( (eta>0 && m_rpcIdHelper->doubletZ(id)==1) )  ? -1. : 1.;
      double zswap = (lay->getRef()> 10000.) ? -1. : 1.;
      double ref = (zswap < 0.) ? lay->getRef()-20000. :  lay->getRef();
      locPar[0] -= sign*ref;
      Amg::VectorX pPar = (*pMx) * locPar;
      if (m_rpcIdHelper->measuresPhi(id)) pPar[0] *= eta;
      else                                pPar[1] *= -eta;
      if (m_rpcIdHelper->measuresPhi(id)) pPar[1] *= zswap;
      else                                pPar[0] *= zswap;

      AmgMatrix(5,5)* projEM = new AmgMatrix(5,5);
      if ( parm->covariance() ) {
      
	*projEM = parm->covariance()->similarity(*pMx);
	projPar = new Trk::AtaPlane(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*stripSurf,projEM);
  
      } else {    
	projPar = new Trk::AtaPlane(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*stripSurf);
        projEM = 0; 
      }
      
      if (m_alignedMode && (parm->position()-projPar->position()).mag()>0.001 ) {
        ATH_MSG_DEBUG("geometrical RPC projection (layerToDetEl) for hit : " << m_rpcIdHelper->measuresPhi(id) << "," << 
	  m_rpcIdHelper->stationName(id) << "," << m_rpcIdHelper->stationEta(id) <<","<<m_rpcIdHelper->stationPhi(id) << "," << 
	  m_rpcIdHelper->doubletPhi(id) <<"," <<m_rpcIdHelper->doubletR(id)<<","<< m_rpcIdHelper->doubletZ(id) );
        Amg::Vector2D locPos;
        const Amg::Vector3D globPos = parm->position();
	bool onSurface = stripSurf->globalToLocal(globPos, globPos, locPos);
        if (onSurface) {
          pPar[0]= locPos[0]; pPar[1]= locPos[1];
          delete projPar; projPar=0;
	  projPar = projEM ? new Trk::AtaPlane(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*stripSurf,projEM ):
	    new Trk::AtaPlane(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*stripSurf);
	} else {
          delete projPar;
          return 0;
        }
      }      
    }

    if ( hitType == 3) {
      // local position of detEl
      const MuonGM::CscReadoutElement* cscROE = m_muonMgr->getCscReadoutElement(id);			
      if (!cscROE) {
        ATH_MSG_WARNING( name() << "CSC readout element not found");
        return projPar;
      }
      //Trk::DistanceSolution distSol = cscROE->surface(id).straightLineDistanceEstimate(parm->position(),parm->momentum().unit()); 
      //std::cout << "distance:"<< distSol.first()<<","<<distSol.currentDistance()<<std::endl;

      const Trk::PlaneSurface* stripSurf = dynamic_cast<const Trk::PlaneSurface*> (&(cscROE->surface(id)));
      if (!stripSurf) return projPar;
      // dealing with displaced planes, possibly sligthly displaced
      double diff = m_cscIdHelper->measuresPhi(id) ? 1.55 : -1.55 ;      
      // distance between planes (assuming parallel planes)
      Amg::Vector3D layNormal = lay->surfaceRepresentation().normal();
      double DN = parm->momentum().dot(layNormal);
      double t = diff/DN;
      // displacement ( as projection of strip surface center on the layer )     
      const Amg::Vector2D csc_shift(0.,lay->getRef());
      // correct local position due to plane distance : use method independent on misalignment
      Amg::Vector3D corrLocPos = parm->position() - t*parm->momentum() + t*DN*layNormal;
      Amg::Vector2D locCorrLay;
      bool onSurface = lay->surfaceRepresentation().globalToLocal(corrLocPos, corrLocPos, locCorrLay);

      if (!onSurface) {
        ATH_MSG_WARNING( name() << ": misplaced CSC "<< id );
        return projPar;
      }
      
      Amg::VectorX parProj(5);
      parProj[0]= locCorrLay[Trk::locX]-csc_shift[Trk::locX]; 
      parProj[1]= locCorrLay[Trk::locY]-csc_shift[Trk::locY];
      parProj[2]= parm->parameters()[Trk::phi];
      parProj[3]= parm->parameters()[Trk::theta];
      parProj[4]= parm->parameters()[Trk::qOverP];
      //    
      AmgMatrix(5,5)* pMx = 0;
      if ( m_cscIdHelper->measuresPhi(id) )  pMx = m_tgcProjPhi;
      else                                   pMx = m_tgcProjEta;
      Amg::VectorX locPar = (*pMx)*parProj;
      ATH_MSG_DEBUG("projected parameters (layer->CSC):" << m_cscIdHelper->measuresPhi(id) <<"," << locPar );

      if (parm->covariance()) {
      
	AmgMatrix(5,5)* projEM = new AmgMatrix(5,5);
	*projEM = parm->covariance()->similarity(*pMx);

	ATH_MSG_DEBUG("projected covariance (layer->CSC):" << projEM );
	
	projPar = new Trk::AtaPlane(locPar[0],locPar[1],locPar[2],locPar[3],locPar[4],*stripSurf,projEM );

      } else
	projPar = new Trk::AtaPlane(locPar[0],locPar[1],locPar[2],locPar[3],locPar[4],*stripSurf);

      ATH_MSG_DEBUG("test CSC projection:layerToDetEl:" << parm->position()<<"," << projPar->position() ); 
    }

    if ( hitType == 4) {
      // local position at layer
      //const Amg::Vector2D locLay; 
      // bool onSurface = lay->surfaceRepresentation().globalToLocal(parm->position(),locLay,locLay);
      // local position of detEl
      const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(id);			
      if (!tgcROE) {
        ATH_MSG_WARNING( name() << "TGC readout element not found");
        return projPar;
      }
      const Trk::PlaneSurface* stripSurf = dynamic_cast<const Trk::PlaneSurface*> (&(tgcROE->surface(id)));
      if (!stripSurf) return projPar;
      //
      AmgMatrix(5,5)* pMx = 0;
      if ( m_tgcIdHelper->isStrip(id) )  pMx = m_tgcProjPhi;
      else                               pMx = m_tgcProjEta;
      Amg::VectorX locPar = (*pMx)*parm->parameters();
      ATH_MSG_DEBUG("projected parameters (layer->TGC):" << m_tgcIdHelper->isStrip(id) <<"," << locPar <<"," << stripSurf );

      AmgMatrix(5,5)* projEM = 0;
      bool bcov = false;

      if (parm->covariance()) {
 	*projEM = parm->covariance()->similarity(*pMx);
        bcov = true;	
	ATH_MSG_DEBUG("projected covariance (layer->TGC):" << (*projEM) );
	projPar = new Trk::AtaPlane(locPar[0],locPar[1],locPar[2],locPar[3],locPar[4],*stripSurf,projEM );
      
      } else {
	projPar = new Trk::AtaPlane(locPar[0],locPar[1],locPar[2],locPar[3],locPar[4],*stripSurf);
      }

      // verify
      if ( m_alignedMode && (parm->position() - projPar->position()).mag() > 0.001 ) {
	ATH_MSG_DEBUG("geometrical TGC projection ( layer2detEl ):" << 	m_tgcIdHelper->stationName(id) << ","<<	m_tgcIdHelper->stationEta(id)
	    <<","<< 	m_tgcIdHelper->stationPhi(id) <<","<< m_tgcIdHelper->isStrip(id) );
	Amg::Vector2D locPos;
	const Amg::Vector3D globPos = parm->position();
	bool onSurface = stripSurf->globalToLocal(globPos, globPos, locPos);
	if (onSurface) {
          locPar[0]= locPos[0]; locPar[1]= locPos[1];
	  delete projPar; projPar=0;
	  projPar = bcov ? new Trk::AtaPlane(locPar[0],locPar[1],locPar[2],locPar[3],locPar[4],*stripSurf,projEM) :
	      new Trk::AtaPlane(locPar[0],locPar[1],locPar[2],locPar[3],locPar[4],*stripSurf);
	} else {
	  delete projPar;
	  return 0;
	}
      }
    }
    
    return projPar;
}

const Trk::TrackParameters* Muon::MuonTGMeasurementTool::detElToLayer(const Trk::Layer* lay, const Trk::TrackParameters* parm, Identifier id) const
{
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG("MuonTGMeasurementTool::detElToLayer");
    const Trk::TrackParameters* projPar = 0;
    //const Amg::Vector2D* locPos = 0;

    // check input
    if (!lay || !parm || !(id.get_identifier32().get_compact()>0) ) return projPar;

    // get tracking geometry
    if (!m_trackingGeometry) {   
      StatusCode sc = getTrackingGeometry();
      if ( sc.isFailure() ) return projPar; 
    } 

    // check compatibility of layer info and required id ? this was already done when associating !
    if (!lay->layerType()) return projPar;
    Identifier layId(lay->layerType());

    ATH_MSG_DEBUG("MuonTGMeasurementTool::input ok");
   
    unsigned int hitType = 0;
    if ( m_mdtIdHelper->is_mdt(id) ) hitType = 1;    
    if ( m_mdtIdHelper->is_rpc(id) ) hitType = 2;    
    if ( m_mdtIdHelper->is_csc(id) ) hitType = 3;    
    if ( m_mdtIdHelper->is_tgc(id) ) hitType = 4;

    unsigned int layType = 0;
    if ( m_mdtIdHelper->is_mdt(layId) ) layType = 1;    
    if ( m_mdtIdHelper->is_rpc(layId) ) layType = 2;    
    if ( m_mdtIdHelper->is_csc(layId) ) layType = 3;    
    if ( m_mdtIdHelper->is_tgc(layId) ) layType = 4;

    if ( layType != hitType ) return projPar; 

    if ( hitType==0 ) {
      ATH_MSG_DEBUG("unknown hit technology");
      return projPar; 
    }    

    if ( hitType == 1) {
      // local position of the tube
      int tube = m_mdtIdHelper->tube(id); 
      Amg::Vector2D locWire(0.,lay->getRef()+(tube-1)*30.035);
      if (fabs(lay->getRef())>10e6) {
        double sign = (lay->getRef()>0) ? 1.:-1.;
        int dec = int(lay->getRef()/1.e5);
        double ref0 = dec/1.e5;
        double ref1 = lay->getRef()-dec*1e5-0.5*(sign+1)*1e5;
        locWire[0]=ref0; locWire[1]=ref1;
        int tube = m_mdtIdHelper->tube(id);
        int tubeMax = m_mdtIdHelper->tubeMax(id);
        if ( tube>6 && tubeMax-tube>5 ) locWire[0]=0.; 
      }
      if (sqrt(locWire[0]*locWire[0]+locWire[1]*locWire[1])>2000.) {
	ATH_MSG_WARNING( name() << " wire shift out bounds for MDT tube :" << m_mdtIdHelper->stationName(id) <<","<<
	  m_mdtIdHelper->stationEta(id) <<","<< m_mdtIdHelper->stationPhi(id) <<": abandon projection");
        return projPar;
      }
      // local position (tube)
      //const Amg::Vector2D locPos = parm->localPosition();
      // direction at the layer
      Amg::Vector3D dir =  parm->momentum().unit();
      Amg::Vector3D locDir = (lay->surfaceRepresentation().transform().inverse()) * dir;
      Amg::Vector3D normal = lay->surfaceRepresentation().normal();
      Amg::Vector3D wireDir(1.,0.,0.);
      const Trk::PlaneSurface* laySurf = dynamic_cast<const Trk::PlaneSurface*> (&(lay->surfaceRepresentation()));
      if (!laySurf) return projPar;
      //
      double ND = dir.dot(normal);
      double DL = locDir.dot(wireDir);
      double A  = sqrt(1.-DL*DL);
      AmgMatrix(5,5) mdtProj;
      mdtProj.setIdentity();
      //
      double ori = (laySurf->transform().inverse()*parm->associatedSurface().transform()).rotation()(1,1)>0. ? -1.:1.;
      if ( A > 10e-6 ) {
        mdtProj(0,1)= ND/A; mdtProj(1,0) = ori; mdtProj(1,1)= DL/A*sqrt(A*A-ND*ND)/A; mdtProj(0,0)=0.;
      } else {
        mdtProj(0,1)= 1.; mdtProj(1,0) = ori; mdtProj(1,1)=0.; mdtProj(0,0)=0.;
      } 
      Amg::VectorX locPar = parm->parameters();
      AmgMatrix(5,5) mdtProjInv = mdtProj.inverse();
      Amg::VectorX pPar = mdtProjInv * locPar;
      pPar[0] += locWire[0];
      pPar[1] += locWire[1];
      ATH_MSG_DEBUG("back projected parameters(MDT->layer):" << pPar );

      AmgMatrix(5,5)* projEM = new AmgMatrix(5,5);
      *projEM = parm->covariance()->similarity(mdtProjInv);

      ATH_MSG_DEBUG("back projected covariance(MDT->layer):" << (*projEM) );   
      projPar = new Trk::AtaPlane(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*laySurf,projEM );
      //delete projEM;
    } 
    
    if ( hitType == 2) {

      // local position at detEl
      //const Amg::Vector2D locPos = parm->localPosition();
      const Trk::PlaneSurface* laySurf = dynamic_cast<const Trk::PlaneSurface*> (&(lay->surfaceRepresentation()));
      if (!laySurf) return projPar;
      //
      Amg::VectorX locPar = parm->parameters();
      AmgMatrix(5,5)* pMx = 0;
      if (m_rpcIdHelper->measuresPhi(id)) pMx = m_rpcProjPhi;
      else                                pMx = m_rpcProjEta;

      //double eta = (m_rpcIdHelper->stationEta(id)<0) ? -1. : 1.;
      double eta = 1.;
      double sign = (m_rpcIdHelper->measuresPhi(id) && m_rpcIdHelper->doubletPhi(id)==2 ) ? -1. : 1.;

      double ref = (lay->getRef()>10000.) ? lay->getRef()-20000. : lay->getRef(); 
      double zswap = (lay->getRef()>10000.) ? -1. : 1.;

      if (m_rpcIdHelper->measuresPhi(id)) locPar[0] *= eta;
      else                                locPar[1] *= -eta;
      if (m_rpcIdHelper->measuresPhi(id)) locPar[1] *= zswap;
      else                                locPar[0] *= zswap;

      Amg::VectorX pPar = (*pMx) * locPar;
      pPar[0] +=  sign*ref; 

      //  
      ATH_MSG_DEBUG("back projected parameters(RPC->layer):" << m_rpcIdHelper->measuresPhi(id)<<"," << pPar );

      AmgMatrix(5,5)* projEM = new AmgMatrix(5,5);
      *projEM = parm->covariance()->similarityT(*pMx);

      ATH_MSG_DEBUG("back projected covariance(RPC->layer):" << (*projEM) );
      projPar = new Trk::AtaPlane(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*laySurf,projEM );
      //delete projEM;

      if ( (parm->position()-projPar->position()).mag()>0.001 ) {
        ATH_MSG_DEBUG("geometrical RPC projection (detElToLayer) for hit : " << m_rpcIdHelper->measuresPhi(id) << "," << 
	  m_rpcIdHelper->stationName(id) << "," << m_rpcIdHelper->stationEta(id) <<","<<m_rpcIdHelper->stationPhi(id) << "," << 
	  m_rpcIdHelper->doubletPhi(id) <<"," <<m_rpcIdHelper->doubletR(id)<<","<< m_rpcIdHelper->doubletZ(id) );
	Amg::Vector2D locPos;
	const Amg::Vector3D globPos = parm->position();
	bool onSurface = laySurf->globalToLocal(globPos, globPos, locPos);
        if (onSurface) {
          pPar[0]= locPos[0]; pPar[1]= locPos[1];
	  delete projPar;
	  projPar = new Trk::AtaPlane(pPar[0],pPar[1],pPar[2],pPar[3],pPar[4],*laySurf,projEM);
	  //delete projEM;
	} else {
	  delete projPar;
          return 0;
        }
      }
    }

    if ( hitType == 3) {
      // local position of detEl
      //const Amg::Vector2D locPos = parm->localPosition();
      const Trk::PlaneSurface* laySurf = dynamic_cast<const Trk::PlaneSurface*> (&(lay->surfaceRepresentation()));
      if (!laySurf) return projPar;
      // dealing with parallel displaced planes
      double diff = m_cscIdHelper->measuresPhi(id) ? 1.55 : -1.55 ;      
      //
      Amg::Vector3D layNormal = lay->surfaceRepresentation().normal();
      double DN = parm->momentum().dot(layNormal);
      double t = diff/DN;
      // displacement of planes
      const Amg::Vector2D csc_shift(0.,lay->getRef());
      // projection : take into account possible misalignment ;
      AmgMatrix(5,5)* pMx = 0;
      if ( m_cscIdHelper->measuresPhi(id) )  pMx = m_tgcProjPhi;
      else                                   pMx = m_tgcProjEta;
      AmgMatrix(5,5) pMxInv = pMx->inverse();
      Amg::VectorX parProj = pMxInv*parm->parameters();
      Amg::Vector3D corrLocPos = lay->surfaceRepresentation().center()-t*parm->momentum() + t*DN*layNormal;
      Amg::Vector2D locCorrLay;
      bool onSurface = lay->surfaceRepresentation().globalToLocal(corrLocPos, corrLocPos, locCorrLay);
      if(onSurface && locCorrLay.size()>0) { 
        parProj[0] += locCorrLay[Trk::locX]+csc_shift[Trk::locX];
        parProj[1] += locCorrLay[Trk::locY]+csc_shift[Trk::locY];
        ATH_MSG_DEBUG("back projected parameters(CSC->layer):"<<m_cscIdHelper->measuresPhi(id)<<"," << parProj );
      }
      AmgMatrix(5,5)* projEM = new AmgMatrix(5,5);
      *projEM = parm->covariance()->similarity(pMxInv);

      ATH_MSG_DEBUG("back projected covariance(CSC->layer):"<<(*projEM) );
      projPar = new Trk::AtaPlane(parProj[0],parProj[1],parProj[2],parProj[3],parProj[4],*laySurf,projEM );

      ATH_MSG_DEBUG("test CSC projection:detElToLayer:" << parm->position()
	  <<"," << projPar->position() ); 
    }

    if ( hitType == 4) {
      const Trk::PlaneSurface* laySurf = dynamic_cast<const Trk::PlaneSurface*> (&(lay->surfaceRepresentation()));
      if(!laySurf) { 
        return projPar;
      }
      //projPar = new Trk::AtaPlane( *(laySurf->localToGlobal(*locLay)),parm->momentum(),parm->charge(),*laySurf);
      //
      AmgMatrix(5,5)* pMx = 0;
      if ( m_tgcIdHelper->isStrip(id) )  pMx = m_tgcProjPhi;
      else                               pMx = m_tgcProjEta;
      AmgMatrix(5,5) pMxInv = pMx->inverse();
      Amg::VectorX locPar = pMxInv * parm->parameters();
      ATH_MSG_DEBUG("back projected parameters(TGC->layer):" << m_tgcIdHelper->isStrip(id)<<"," << locPar );

      AmgMatrix(5,5)* projEM = new AmgMatrix(5,5);
      *projEM = parm->covariance()->similarity(pMxInv);

      ATH_MSG_DEBUG("back projected covariance(TGC->layer):" << (*projEM) );
      projPar = new Trk::AtaPlane(locPar[0],locPar[1],locPar[2],locPar[3],locPar[4],*laySurf,projEM);

      // verify
      if ( m_alignedMode && (parm->position() - projPar->position()).mag() > 0.001 ) {
	ATH_MSG_DEBUG("geometrical TGC projection ( detEl2Layer ):" << 	m_tgcIdHelper->stationName(id) << ","<<	m_tgcIdHelper->stationEta(id)
            <<","<< 	m_tgcIdHelper->stationPhi(id) <<","<< m_tgcIdHelper->isStrip(id) );
	Amg::Vector2D locPos;
	const Amg::Vector3D globPos = parm->position();
	bool onSurface = laySurf->globalToLocal(globPos, globPos, locPos);
        if (onSurface) {
          locPar[0]= locPos[0]; locPar[1]= locPos[1];
	  delete projPar;
	  projPar = 0;
	  projPar = new Trk::AtaPlane(locPar[0],locPar[1],locPar[2],locPar[3],locPar[4],*laySurf,projEM );
	} else {
	  delete projPar;
          return 0;
        }
      }
    }
    return projPar;
}

const Trk::RIO_OnTrack* Muon::MuonTGMeasurementTool::measToLayer(const Trk::Layer* lay, const Trk::TrackParameters* parm, const Trk::RIO_OnTrack* rio) const
{
    // Get the messaging service, print where you are
    ATH_MSG_DEBUG("MuonTGMeasurementTool::measToLayer");
    const Trk::RIO_OnTrack* projRIO = 0;

    // check input
    if (!lay || !parm || !rio ) return projRIO;

    // get tracking geometry
    if (!m_trackingGeometry) {   
      StatusCode sc = getTrackingGeometry();
      if ( sc.isFailure() ) return projRIO; 
    } 

    // check compatibility of layer info and required id ? this was already done when associating !
    Identifier id = rio->identify();

    ATH_MSG_DEBUG("MuonTGMeasurementTool::input ok");
   
    unsigned int hitType = 0;
    if ( m_mdtIdHelper->is_mdt(id) ) hitType = 1;    
    if ( m_mdtIdHelper->is_rpc(id) ) hitType = 2;    
    if ( m_mdtIdHelper->is_csc(id) ) hitType = 3;    
    if ( m_mdtIdHelper->is_tgc(id) ) hitType = 4;


    if ( hitType == 1) {
      // local position of the tube
      int tube = m_mdtIdHelper->tube(id); 
      Amg::Vector2D locWire(0.,lay->getRef()+(tube-1)*30.035);
      if (fabs(lay->getRef())>10e6) {
        double sign = (lay->getRef()>0) ? 1.:-1.;
        int dec = int(lay->getRef()/1.e5);
        double ref0 = dec/1.e5;
        double ref1 = lay->getRef()-dec*1e5-0.5*(sign+1)*1e5;
        locWire[0]=ref0; locWire[1]=ref1;
        int tube = m_mdtIdHelper->tube(id);
        int tubeMax = m_mdtIdHelper->tubeMax(id);
        if ( tube>6 && tubeMax-tube>5 ) locWire[0]=0.; 
      }
      // direction at the layer
      Amg::Vector3D dir =  parm->momentum().unit();
      Amg::Vector3D locDir = (lay->surfaceRepresentation().transform().inverse()) * dir;
      Amg::Vector3D normal = lay->surfaceRepresentation().normal();
      Amg::Vector3D wireDir(1.,0.,0.);
      //
      double ND = dir.dot(normal);
      double DL = locDir.dot(wireDir);
      double A  = sqrt(1.-DL*DL);
      double A_ND = A/ND;
      // 
      double locLay = A_ND * rio->localParameters()[Trk::locR] + locWire[1];
      // create (fake!) rio ( rio image on TG layer )
      IdentifierHash idHash(0);
      const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(id);			
      Amg::MatrixX* cov = new Amg::MatrixX();
      *cov = A_ND*A_ND*rio->localCovariance();
      Muon::MdtDriftCircleStatus status = Muon::MdtStatusDriftTime;
      const Muon::MdtPrepData* mdtPrd = new Muon::MdtPrepData( id , idHash, Amg::Vector2D(locLay,0.), cov, mdtROE, 0, 0, status );
      Muon::MuonDriftCircleErrorStrategyInput stratbits(MuonDriftCircleErrorStrategy::UnknownStrategy);
      Muon::MuonDriftCircleErrorStrategy strat(stratbits);
      const Muon::MdtDriftCircleOnTrack* mdtRio = new Muon::MdtDriftCircleOnTrack( mdtPrd,Trk::LocalParameters( Trk::DefinedParameter(locLay,Trk::locY) ),
										   mdtPrd->localCovariance(),
										   0.,Trk::DECIDED, 0.0, strat);  
      return mdtRio;
    } 
 
    else if ( hitType == 2) {
      //
      double eta = (m_rpcIdHelper->stationEta(id)<0) ? -1. : 1.;
      double sign = (m_rpcIdHelper->measuresPhi(id) && m_rpcIdHelper->doubletPhi(id)==2 ) ? -1. : 1.;

      double locPos = rio->localParameters()[Trk::locX];

      if ( m_rpcIdHelper->measuresPhi(id) )   locPos *= eta;
  
      double ref = (lay->getRef()>10000.) ? lay->getRef()-20000. : lay->getRef(); 
      double zswap = (lay->getRef()>10000.) ? -1. : 1.;

      if ( m_rpcIdHelper->measuresPhi(id) ) locPos +=  sign*ref;
      else                                  locPos *= zswap;    
      //
      const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(id);			
      const IdentifierHash idHash(0);
      std::vector<Identifier> rdoList;
      rdoList.push_back(id);
      const Muon::RpcPrepData* rpcPrd = new Muon::RpcPrepData( id , idHash, Amg::Vector2D(locPos,0.), rdoList, new Amg::MatrixX(rio->localCovariance()), rpcROE, float(0.), 0, 0);
      const Muon::RpcClusterOnTrack* rpcRio = 0;
      if (m_rpcIdHelper->measuresPhi(id))
	rpcRio = new Muon::RpcClusterOnTrack( rpcPrd,
					      Trk::LocalParameters(Trk::DefinedParameter(locPos,Trk::locX)),
					      rpcPrd->localCovariance(), parm->localPosition()[Trk::locY]);  
      else
	rpcRio = new Muon::RpcClusterOnTrack( rpcPrd,
					      Trk::LocalParameters(Trk::DefinedParameter(locPos,Trk::locY)),
					      rpcPrd->localCovariance(), parm->localPosition()[Trk::locX]);  
      return rpcRio;
    }


    else if ( hitType == 3) {
      // dealing with parallel displaced planes
      double diff = m_cscIdHelper->measuresPhi(id) ? 1.55 : -1.55 ;      
      //
      Amg::Vector3D layNormal = lay->surfaceRepresentation().normal();
      double DN = parm->momentum().dot(layNormal);
      double t = diff/DN;
      // displacement of planes
      const Amg::Vector2D csc_shift(0.,lay->getRef());
      Amg::Vector3D corrLocPos = lay->surfaceRepresentation().center()-t*parm->momentum() + t*DN*layNormal;
      Amg::Vector2D locCorrLay;
      bool onSurface = lay->surfaceRepresentation().globalToLocal(corrLocPos, corrLocPos, locCorrLay);
      //
      //if( !locCorrLay.size()==2) { ??
      if( !onSurface ) {
	return projRIO;
      }
      double locPos; 
      if (m_cscIdHelper->measuresPhi(id)) {
         locPos = rio->localParameters()[Trk::locX]+locCorrLay[Trk::locX]+csc_shift[Trk::locX];
      } else { 
         locPos = rio->localParameters()[Trk::locX]+locCorrLay[Trk::locY]+csc_shift[Trk::locY];
      }
      //
      const MuonGM::CscReadoutElement* cscROE = m_muonMgr->getCscReadoutElement(id);			
      IdentifierHash idHash(0);
      std::vector<Identifier> rdoList;
      rdoList.push_back(id);
      Muon::CscClusterStatus status = Muon::CscStatusSimple;
      const Muon::CscPrepData* cscPrd = new Muon::CscPrepData( id , idHash, Amg::Vector2D(locPos,0.), rdoList, new Amg::MatrixX(rio->localCovariance()), cscROE,0,0.,status);
      const Muon::CscClusterOnTrack* cscRio = 0;
      if (m_cscIdHelper->measuresPhi(id))
	cscRio = new Muon::CscClusterOnTrack( cscPrd,
					      Trk::LocalParameters(Trk::DefinedParameter(locPos,Trk::locX)),
					      cscPrd->localCovariance(), parm->localPosition()[Trk::locY],cscPrd->status());  
      else
	cscRio = new Muon::CscClusterOnTrack( cscPrd,
					      Trk::LocalParameters(Trk::DefinedParameter(locPos,Trk::locY)),
					      cscPrd->localCovariance(), parm->localPosition()[Trk::locX],cscPrd->status());  
      return cscRio;
    }
    
    
    else if ( hitType==4 ) {
      //
      double locPos = rio->localParameters()[Trk::locX];
      /*
	if (m_tgcIdHelper->isStrip(id)) {
	locPos = rio->localParameters()[Trk::locX];
	} else { 
	locPos = rio->localParameters()[Trk::locY];
	}
      */
      //
      const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(id);
      IdentifierHash idHash(0);
      std::vector<Identifier> rdoList;
      rdoList.push_back(id);
      const Muon::TgcPrepData* tgcPrd = new Muon::TgcPrepData( id , idHash,Amg::Vector2D(locPos,0.), rdoList, new Amg::MatrixX(rio->localCovariance()), tgcROE);
      const Muon::TgcClusterOnTrack* tgcRio = 0;
      if (m_tgcIdHelper->isStrip(id)) {
        Amg::Vector2D loc(locPos,parm->localPosition()[Trk::locY]);
	tgcRio = new Muon::TgcClusterOnTrack( tgcPrd,
					      Trk::LocalParameters(Trk::DefinedParameter(locPos,Trk::locX)),
					      tgcPrd->localCovariance(),
					      parm->localPosition()[Trk::locY]);  
      } else {
	tgcRio = new Muon::TgcClusterOnTrack( tgcPrd,
					      Trk::LocalParameters(Trk::DefinedParameter(locPos,Trk::locY)),
					      tgcPrd->localCovariance(),parm->localPosition()[Trk::locX]);  
      }
      return tgcRio;
    }

    ATH_MSG_DEBUG("unknown hit technology");
    return projRIO;  

}

const Identifier Muon::MuonTGMeasurementTool::nearestDetEl(const Trk::Layer* lay, const Trk::TrackParameters* parm, bool measPhi,double& pitch) const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonTGMeasurementTool::nearestDetEl");
  Identifier nid(0);
  // check input
  if (!lay || !parm || !lay->layerType() ) return nid;
  
  // get tracking geometry
  if (!m_trackingGeometry) {   
    StatusCode sc = getTrackingGeometry();
    if ( sc.isFailure() ) return nid; 
  } 

  // check compatibility of layer info and required id ? this was already done when associating !
  Identifier layId(lay->layerType());
  
  unsigned int hitType = 0;
  if ( m_mdtIdHelper->is_mdt(layId) ) hitType = 1;    
  if ( m_mdtIdHelper->is_rpc(layId) ) hitType = 2;    
  if ( m_mdtIdHelper->is_csc(layId) ) hitType = 3;    
  if ( m_mdtIdHelper->is_tgc(layId) ) hitType = 4;

  if ( hitType==0 ) {
    ATH_MSG_DEBUG("unknown hit technology");
    return nid; 
  }    
  
  if ( hitType == 1) {
    const MuonGM::MdtReadoutElement* mdtROE = m_muonMgr->getMdtReadoutElement(layId);			
    if (!mdtROE) return nid;
    int tMax = mdtROE->getNtubesperlayer();
    // local position at layer
    Amg::Vector2D locLay;
    const Amg::Vector3D globPos = parm->position();
    bool onSurface = lay->surfaceRepresentation().globalToLocal(globPos, globPos, locLay);
    if (!onSurface || measPhi ) {
      return nid;
    }
    // nearest tube index
    double refL = lay->getRef();
    if (fabs(refL)>10e6) {
      double sign = (lay->getRef()>0) ? 1.:-1.;
      int dec = int(lay->getRef()/1.e5);
      refL = lay->getRef()-dec*1e5-0.5*(sign+1)*1e5;
    }
    double dloc = locLay[Trk::locY]-refL+15.0175;
    int itube = int( dloc/30.035 ) + 1;
    if (itube < 1 || itube > tMax || dloc<0.) {return nid;} 
    // tube id
    Identifier nearId = m_mdtIdHelper->channelID(m_mdtIdHelper->stationName(layId),
						 m_mdtIdHelper->stationEta(layId),
						 m_mdtIdHelper->stationPhi(layId),
						 m_mdtIdHelper->multilayer(layId),
						 m_mdtIdHelper->tubeLayer(layId),
						 itube);
    // check if position within active volume 
    if (fabs(locLay[Trk::locX])>0.5*mdtROE->getActiveTubeLength(m_mdtIdHelper->tubeLayer(layId),itube)) {
      return nid; 
    }
    // 
    if (m_mdtIdHelper->valid(nearId)) return nearId;
    else return nid;
  }

  if ( hitType == 2) {
    // identify phi doublet first
    Identifier phiId(0);
    // both phi and eta needs to be within bounds !!!
    int doubletPhi = 1;
    bool foundDoubletPhi = false;
    const MuonGM::RpcReadoutElement* rpcROE = 0;    
    while (!foundDoubletPhi && doubletPhi<3) { 
      Identifier refPhi1 = m_rpcIdHelper->channelID(m_rpcIdHelper->stationName(layId),
						    m_rpcIdHelper->stationEta(layId),
						    m_rpcIdHelper->stationPhi(layId),
						    m_rpcIdHelper->doubletR(layId),	
						    m_rpcIdHelper->doubletZ(layId),
						    doubletPhi,
						    m_rpcIdHelper->gasGap(layId),
						    1,1);
      rpcROE = m_muonMgr->getRpcReadoutElement(refPhi1);    
      if (!rpcROE) return nid;
      if (!m_rpcIdHelper->valid(refPhi1)) return nid; 
      int  nStripPhi = rpcROE->Nstrips(1);  
      
      Identifier refPhiN = m_rpcIdHelper->channelID(m_rpcIdHelper->stationName(layId),
						    m_rpcIdHelper->stationEta(layId),
						    m_rpcIdHelper->stationPhi(layId),
						    m_rpcIdHelper->doubletR(layId),	
						    m_rpcIdHelper->doubletZ(layId),
						    doubletPhi,
						    m_rpcIdHelper->gasGap(layId),
						    1,nStripPhi);
      //
      Amg::Vector2D loc1; 
      const Amg::Vector3D globPos1 = rpcROE->stripPos(refPhi1);
      bool onSurface1 = rpcROE->surface(refPhi1).globalToLocal(globPos1, globPos1, loc1);
      Amg::Vector2D locN; 
      const Amg::Vector3D globPosN = rpcROE->stripPos(refPhiN);
      bool onSurfaceN = rpcROE->surface(refPhiN).globalToLocal(globPosN, globPosN, locN);
      Amg::Vector2D loc; 
      const Amg::Vector3D glob = parm->position();
      bool onSurface = rpcROE->surface(refPhiN).globalToLocal(glob, glob, loc);
      
      int strip = 0;
      if ( onSurface1 && onSurfaceN  && onSurface ) {
	pitch = (locN[Trk::locX]-loc1[Trk::locX])/fmax(1,nStripPhi-1); 
	double dstrip = ( loc[Trk::locX]-loc1[Trk::locX])/pitch+0.5;
	strip = dstrip>=0. ? int(dstrip)+1 : 0;  
	
	if (strip> 0 && strip <= nStripPhi) {
	  // correct doublet
	  phiId =  m_rpcIdHelper->channelID(m_rpcIdHelper->stationName(layId),
					    m_rpcIdHelper->stationEta(layId),
					    m_rpcIdHelper->stationPhi(layId),
					    m_rpcIdHelper->doubletR(layId),	
					    m_rpcIdHelper->doubletZ(layId),
					    doubletPhi,
					    m_rpcIdHelper->gasGap(layId),
					    1,strip);
          foundDoubletPhi=true;
 
	} else {
	  doubletPhi += 1;
	} 
      } else doubletPhi += 1; 
    }
    // no valid phi strip - skip search
    if (!foundDoubletPhi) return nid;

    // eta strip
    Identifier etaId(0);    
    int doubletZ = 1;
    while (doubletZ<4) { 
      Identifier refEta1 = m_rpcIdHelper->channelID(m_rpcIdHelper->stationName(layId),
						    m_rpcIdHelper->stationEta(layId),
						    m_rpcIdHelper->stationPhi(layId),
						    m_rpcIdHelper->doubletR(layId),	
						    doubletZ,
						    doubletPhi,
						    m_rpcIdHelper->gasGap(layId),
						    0,1);
      rpcROE = m_muonMgr->getRpcReadoutElement(refEta1);    
      if (!rpcROE) return nid;
      if (!m_rpcIdHelper->valid(refEta1)) return nid; 
      int nStrips = rpcROE->Nstrips(0);  
      
      Identifier refEtaN = m_rpcIdHelper->channelID(m_rpcIdHelper->stationName(layId),
						    m_rpcIdHelper->stationEta(layId),
						    m_rpcIdHelper->stationPhi(layId),
						    m_rpcIdHelper->doubletR(layId),	
						    doubletZ,
						    doubletPhi,
						    m_rpcIdHelper->gasGap(layId),
						    0,nStrips);
      //

      Amg::Vector2D loc1; 
      const Amg::Vector3D globPos1 = rpcROE->stripPos(refEta1);
      bool onSurface1 = rpcROE->surface(refEta1).globalToLocal(globPos1, globPos1, loc1);
      Amg::Vector2D locN; 
      const Amg::Vector3D globPosN = rpcROE->stripPos(refEtaN);
      bool onSurfaceN = rpcROE->surface(refEtaN).globalToLocal(globPosN, globPosN, locN);
      Amg::Vector2D loc; 
      const Amg::Vector3D glob = parm->position();
      bool onSurface = rpcROE->surface(refEtaN).globalToLocal(glob, glob, loc);

      int strip = 0;
      if (onSurface && onSurface1 && onSurfaceN ) {
	pitch = (locN[Trk::locX]-loc1[Trk::locX])/fmax(1,nStrips-1); 
	//strip = int( (refPar->localPosition()[Trk::locX]-loc1[Trk::locX])/pitch+0.5 )+1;      
	double dstrip = ( loc[Trk::locX]-loc1[Trk::locX])/pitch+0.5;
	strip = dstrip>=0. ? int(dstrip)+1 : 0;  
	
	if (strip > 0 && strip <= nStrips) {
	  etaId = m_rpcIdHelper->channelID(m_rpcIdHelper->stationName(layId),
					   m_rpcIdHelper->stationEta(layId),
					   m_rpcIdHelper->stationPhi(layId),
					   m_rpcIdHelper->doubletR(layId),	
					   doubletZ,
					   doubletPhi,
					   m_rpcIdHelper->gasGap(layId),
					   0,strip);
	  if (measPhi) return phiId;
	  else return etaId; 
	} 
      }
      doubletZ += 1;
    }

    return nid;       
  }

  if ( hitType == 3) {
    // ref id
    Identifier refId = m_cscIdHelper->channelID(m_cscIdHelper->stationName(layId),
						m_cscIdHelper->stationEta(layId),
						m_cscIdHelper->stationPhi(layId),
						m_cscIdHelper->chamberLayer(layId),	
						m_cscIdHelper->wireLayer(layId),
						measPhi,1);
    if (!m_cscIdHelper->valid(refId)) return nid; 
    // residual in ref frame
    const Trk::TrackParameters* refPar = layerToDetEl(lay,parm,refId);
    if (!refPar) return nid;
    //
    const MuonGM::CscReadoutElement* cscROE = m_muonMgr->getCscReadoutElement(refId);
    if (!cscROE) {delete refPar; return nid;}
    pitch = cscROE->StripPitch(measPhi);
    int nStrips = m_cscIdHelper->stripMax(refId);
    if (nStrips < 1) {delete refPar; return nid;}

    Identifier refIdN = m_cscIdHelper->channelID(m_cscIdHelper->stationName(refId),
						 m_cscIdHelper->stationEta(refId),
						 m_cscIdHelper->stationPhi(refId),
						 m_cscIdHelper->chamberLayer(refId),	
						 m_cscIdHelper->wireLayer(refId),
						 measPhi,nStrips);
    //
    Amg::Vector3D loc1 = cscROE->surface(refId).transform().inverse()*cscROE->stripPos(refId);
    Amg::Vector3D locN = cscROE->surface(refIdN).transform().inverse()*cscROE->stripPos(refIdN);
    int strip = 0;
    //if ( loc1 && locN ) {
    pitch = (locN[0]-loc1[0])/fmax(1,nStrips-1); 
    strip = int( (refPar->localPosition()[Trk::locX]-loc1[0])/pitch+0.5 )+1;      
    //} else {
    //  ATH_MSG_DEBUG("local position of boundary elements not retrieved, return 0 ");
    //  delete refPar;
    //  return nid;
    //}
    delete refPar; refPar=0;
    if ( strip>0 && strip <= nStrips ) { 
      // strip id
      Identifier nearId =  m_cscIdHelper->channelID(m_cscIdHelper->stationName(layId),
						    m_cscIdHelper->stationEta(layId),
						    m_cscIdHelper->stationPhi(layId),
						    m_cscIdHelper->chamberLayer(layId),	
						    m_cscIdHelper->wireLayer(layId),
						    measPhi,strip);
      if (fabs(residual(parm,nearId))> 0.5*pitch ) ATH_MSG_DEBUG("nearest CSC channel residual too large: "
						       << residual(parm,nearId) );
      return nearId;    
    }
  }

  if ( hitType == 4) {    // ref id
    if (  measPhi && m_tgcIdHelper->gasGap(layId)==2 && m_tgcIdHelper->gasGapMax(layId)==3 ) return nid;  // no phi strips here
     // ref id
    Identifier refId = layId;
    if (measPhi) refId = m_tgcIdHelper->channelID(m_tgcIdHelper->stationName(layId),
						  m_tgcIdHelper->stationEta(layId),
						  m_tgcIdHelper->stationPhi(layId),
						  m_tgcIdHelper->gasGap(layId),
						  measPhi,1);
    if (!m_tgcIdHelper->valid(refId)) return nid; 
    // residual in ref frame
    const Trk::TrackParameters* refPar = layerToDetEl(lay,parm,refId);
    if (!refPar) return nid;
    //
    const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(layId);
    if (!tgcROE) {delete refPar; return nid;}
    int nStrips = m_tgcIdHelper->channelMax(refId);
   
    if ( nStrips < 1 ) {delete refPar; return nid;}

    Identifier refIdN = m_tgcIdHelper->channelID(m_tgcIdHelper->stationName(layId),
						 m_tgcIdHelper->stationEta(layId),
						 m_tgcIdHelper->stationPhi(layId),
						 m_tgcIdHelper->gasGap(layId),
						 measPhi,nStrips);
    //
    Amg::Vector2D loc1; 
    const Amg::Vector3D glob1 = tgcROE->channelPos(refId); 
    bool onSurface1 = tgcROE->surface(refId).globalToLocal(glob1, glob1, loc1);
    Amg::Vector2D locN; 
    const Amg::Vector3D globN = tgcROE->channelPos(refIdN); 
    bool onSurfaceN = tgcROE->surface(refIdN).globalToLocal(globN, globN, locN);

    int strip = 0;
    if ( onSurface1 && onSurfaceN ) {
      pitch = (locN[Trk::locX]-loc1[Trk::locX])/fmax(1,nStrips-1); 
      strip = int( (refPar->localPosition()[Trk::locX]-loc1[Trk::locX])/pitch+0.5 )+1;      
    } else {
      ATH_MSG_DEBUG("local position of boundary elements not retrieved, return 0 ");
      delete refPar;
      return nid;
    }

    //std::cout << "strip:"<< strip<< std::endl;    
    if ( strip > 0 && strip <= nStrips) {
      // check second coordinate for active volume
      //std::cout << "checking second coordinate:"<<(tgcROE->WireLength(m_tgcIdHelper->gasGap(layId),strip)-tgcROE->frameXwidth()) << std::endl;
      if (!measPhi && fabs(refPar->localPosition()[Trk::locY])>(tgcROE->WireLength(m_tgcIdHelper->gasGap(layId),strip)-tgcROE->frameXwidth()) ) {
        delete refPar;
        return nid;
      }
      Identifier nearId = m_tgcIdHelper->channelID(m_tgcIdHelper->stationName(layId),
						   m_tgcIdHelper->stationEta(layId),
						   m_tgcIdHelper->stationPhi(layId),
						   m_tgcIdHelper->gasGap(layId),  
						   measPhi,strip);
      Amg::Vector3D stripposition=tgcROE->surface(nearId).transform().inverse()*tgcROE->channelPos(nearId);
      Amg::Vector3D localhit=tgcROE->surface(nearId).transform().inverse()*parm->position(); 

      //std::cout << "id verification:channel,local parms:"<< stripposition <<","<< localhit << std::endl;
      int plane = m_tgcIdHelper->gasGap(nearId);
      if (m_tgcIdHelper->isStrip(nearId)) pitch = tgcROE->StripPitch(plane,m_tgcIdHelper->channel(nearId),localhit[1]); 
      //std::cout << "pitch:"<< pitch << std::endl;
      int last = 0;
      while ( fabs(stripposition[0] - localhit[0])>0.5*pitch ) {
	if (stripposition[0]<localhit[0]) {
          if (last!=-1) {strip += 1; last=1;}
          else break;
	} else {
          if (last!=1) { strip -= 1; last=-1;}
          else break;
	}
	if ( strip<1 || strip > nStrips) break;
	nearId = m_tgcIdHelper->channelID(m_tgcIdHelper->stationName(layId),
					  m_tgcIdHelper->stationEta(layId),
					  m_tgcIdHelper->stationPhi(layId),
					  m_tgcIdHelper->gasGap(layId),  
					  measPhi,strip);
	stripposition=tgcROE->surface(nearId).transform().inverse()*tgcROE->channelPos(nearId);
	localhit=tgcROE->surface(nearId).transform().inverse()*parm->position(); 
	if (m_tgcIdHelper->isStrip(nearId)) pitch = tgcROE->StripPitch(plane,m_tgcIdHelper->channel(nearId),localhit[1]); 
	//std::cout <<fabs(stripposition[0] - localhit[0])<<","<< 0.5*pitch<< std::endl;  
      }
      delete refPar;
      if ( strip<1 || strip > nStrips) return nid;
      if (m_tgcIdHelper->valid(nearId)) return nearId;    
      else return nid;
    }
    delete refPar;
  }
  return nid;
}

const Trk::Layer* Muon::MuonTGMeasurementTool::associatedLayer(Identifier id, Amg::Vector3D& gp) const
{
  // Get the messaging service, print where you are
  ATH_MSG_DEBUG("MuonTGMeasurementTool::associatedLayer");
  const Trk::Layer* lay = 0;
  // check input
  if (!id.get_identifier32().get_compact() ) return lay;
  
  // get tracking geometry
  if (!m_trackingGeometry) {   
    StatusCode sc = getTrackingGeometry();
    if ( sc.isFailure() ) return lay; 
  } 
  
  // rely on having misalignment uncertainty covered by span safety marge ( don't loose station from static volume 
  //  when misaligned
  const Trk::TrackingVolume* staticVol = m_trackingGeometry->lowestStaticTrackingVolume(gp);
  const Trk::DetachedTrackingVolume* station = 0;
  if (staticVol && staticVol->confinedDetachedVolumes()) {
    const std::vector<const Trk::DetachedTrackingVolume*>* detTV = staticVol->confinedDetachedVolumes(); 
    for (unsigned int i=0;i<detTV->size();i++) { 
      if ((*detTV)[i]->layerRepresentation() && (*detTV)[i]->layerRepresentation()->layerType()>0 ) {
        Identifier stId((*detTV)[i]->layerRepresentation()->layerType());
        if (  m_mdtIdHelper->stationName(stId) == m_mdtIdHelper->stationName(id) 
	      && m_mdtIdHelper->stationEta(stId) == m_mdtIdHelper->stationEta(id) 
	      && m_mdtIdHelper->stationPhi(stId) == m_mdtIdHelper->stationPhi(id) ) {
	  station = (*detTV)[i]; break;
        }
      }
    }
  }
  
  if (station) lay = associatedLayer(id,station->trackingVolume()); 
  
  return lay;
}


const Trk::Layer* Muon::MuonTGMeasurementTool::associatedLayer(Identifier id, const Trk::TrackingVolume* vol ) const
{
  const Trk::Layer* lay = 0;
  // check input
  if (!vol ) return lay;
  
  if ( vol->confinedVolumes() ) {
    const std::vector<const Trk::TrackingVolume*> subVols = vol->confinedVolumes()->arrayObjects();
    std::vector<const Trk::TrackingVolume*>::const_iterator iter = subVols.begin();  
    while (!lay && iter!=subVols.end()) { 
      lay = associatedLayer(id,*iter);
      if (lay) break;
      iter++;  
    }
    if (lay) return lay;
  }
  
  if ( vol->confinedLayers() ) {
    const std::vector<const Trk::Layer*> ordLay = vol->confinedLayers()->arrayObjects();
    std::vector<const Trk::Layer*>::const_iterator iter = ordLay.begin();  
    while (!lay && iter!=ordLay.end()) { 
      lay = match(id,*iter);
      if (lay) break;
      iter++;  
    }
    if (lay) return lay;
  }
  
  if ( vol->confinedArbitraryLayers() ) {
    const std::vector<const Trk::Layer*>* unOrdLay = vol->confinedArbitraryLayers();
    std::vector<const Trk::Layer*>::const_iterator iter = unOrdLay->begin();  
    while (!lay && iter!=unOrdLay->end()) { 
      lay = match(id,*iter);
      if (lay) break;
      iter++;  
    }
    if (lay) return lay;
  }
  
  return lay; 
}

const Trk::Layer* Muon::MuonTGMeasurementTool::match(Identifier id, const Trk::Layer* lay) const
{
  const Trk::Layer* mLay = 0;
  if (!id.get_identifier32().get_compact() || !lay || !lay->layerType() ) return mLay;
  
  Identifier layId(lay->layerType());
  
  if (m_mdtIdHelper->is_mdt(id) && m_mdtIdHelper->is_mdt(layId) ) { 
    
    if (m_mdtIdHelper->multilayer(layId) == m_mdtIdHelper->multilayer(id)
	&& m_mdtIdHelper->tubeLayer(layId)  == m_mdtIdHelper->tubeLayer(id) ) return lay;
  }
  
  if (m_rpcIdHelper->is_rpc(id) && m_rpcIdHelper->is_rpc(layId) ) { 
    
    if (   m_rpcIdHelper->doubletR(layId) == m_rpcIdHelper->doubletR(id)
	   && m_rpcIdHelper->doubletZ(layId) == m_rpcIdHelper->doubletZ(id)
	   && m_rpcIdHelper->gasGap(layId) == m_rpcIdHelper->gasGap(id) ) return lay;
  }
  
  if (m_tgcIdHelper->is_tgc(id) && m_tgcIdHelper->is_tgc(layId) ) { 
    
    if ( m_tgcIdHelper->gasGap(layId) == m_tgcIdHelper->gasGap(id) ) return lay;
  }
  
  if (m_cscIdHelper->is_csc(id) && m_cscIdHelper->is_csc(layId) ) { 
    
    if (   m_cscIdHelper->chamberLayer(layId) == m_cscIdHelper->chamberLayer(id)
	   && m_cscIdHelper->wireLayer(layId) == m_cscIdHelper->wireLayer(id) ) return lay;
  }
  
  return mLay;
}

StatusCode Muon::MuonTGMeasurementTool::getTrackingGeometry() const
{
  StoreGateSvc* detStore(0);
  StatusCode sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Detector service not found !");
    return StatusCode::FAILURE;
  }
  sc = detStore->retrieve(m_trackingGeometry, m_trackingGeometryName);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not find tool "<< m_trackingGeometryName<<". Exiting.");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("tracking geometry Svc \""<<m_trackingGeometryName<<"\" booked ");
  }
  return StatusCode::SUCCESS;  
}

double Muon::MuonTGMeasurementTool::residual( const Trk::TrackParameters* layPar, const Trk::RIO_OnTrack* rio) const
{
  double res = 10000.;
  if (!layPar || !rio) return res;
  
  Amg::Vector3D gp = layPar->position();
  const Trk::Layer* layer = associatedLayer(rio->identify(),gp);
  if (!layer) return res; 

  return residual( layer,layPar,rio);
}

double Muon::MuonTGMeasurementTool::residual( const Trk::TrackParameters* layPar, Identifier& id) const
{
  double res = 10000.;
  if (!layPar || !id.get_identifier32().get_compact()) return res;

  Amg::Vector3D gp = layPar->position();
  const Trk::Layer* layer = associatedLayer(id,gp);
  if (!layer) return res; 

  return residual( layer,layPar,id);
}


double Muon::MuonTGMeasurementTool::residual( const Trk::Layer* layer, const Trk::TrackParameters* layPar, const Trk::RIO_OnTrack* rio) const
{
  double res = 10000.;
  if (!layer || !layPar || !rio) return res;
  
  const Trk::TrackParameters* detElPar = layerToDetEl( layer, layPar, rio->identify() );
  if (!detElPar) return res;
  if (m_mdtIdHelper->is_mdt(rio->identify())) {
    res = fabs( detElPar->localPosition()[Trk::locR] - rio->localParameters()[Trk::locR] );
  } else {
    res = fabs( detElPar->localPosition()[Trk::locX] - rio->localParameters()[Trk::locX]);
  } 
  delete detElPar;
  return res;
}

double Muon::MuonTGMeasurementTool::residual( const Trk::Layer* layer, const Trk::TrackParameters* layPar, Identifier id) const
{
  double res = 10000.;
  if (!layer || !layPar || !id.get_identifier32().get_compact()) return res;
  
  const Trk::TrackParameters* detElPar = layerToDetEl( layer, layPar, id );
  if (!detElPar) return res;
  if (m_mdtIdHelper->is_mdt(id)) {
    res = detElPar->localPosition()[Trk::locR] ;
  } else if (m_rpcIdHelper->is_rpc(id)) {
    const MuonGM::RpcReadoutElement* rpcROE = m_muonMgr->getRpcReadoutElement(id);
    if (rpcROE) res = detElPar->localPosition()[Trk::locX] - (detElPar->associatedSurface().transform().inverse()*(rpcROE->stripPos(id)))[Trk::locX];
  } else if (m_cscIdHelper->is_csc(id)) {
    const MuonGM::CscReadoutElement* cscROE = m_muonMgr->getCscReadoutElement(id);
    if (cscROE) res = detElPar->localPosition()[Trk::locX] - (detElPar->associatedSurface().transform().inverse()*(cscROE->stripPos(id)))[0];
  } else if (m_tgcIdHelper->is_tgc(id)) {
    if ( m_tgcIdHelper->isStrip(id) && m_tgcIdHelper->gasGap(id)==2 && m_tgcIdHelper->gasGapMax(id)==3 ) {
      delete detElPar;
      return res;  // no phi strips here
    }
    const MuonGM::TgcReadoutElement* tgcROE = m_muonMgr->getTgcReadoutElement(id);
    if (tgcROE) {
      Amg::Vector2D locPos;
      const Amg::Vector3D globPos = tgcROE->channelPos(id);
      bool onSurface = detElPar->associatedSurface().globalToLocal(globPos, globPos, locPos);
      if (onSurface) res = detElPar->localPosition()[Trk::locX] - locPos[Trk::locX];
    }
  }
  delete detElPar;
  return res;
}


