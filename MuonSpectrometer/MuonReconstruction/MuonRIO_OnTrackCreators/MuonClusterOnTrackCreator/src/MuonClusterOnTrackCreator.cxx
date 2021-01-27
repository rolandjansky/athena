/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Implementation file for class MuonClusterOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for MuonClusterOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 20/07/2004
///////////////////////////////////////////////////////////////////

#include "MuonClusterOnTrackCreator.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonPrepRawData/MMPrepData.h"

#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include <sstream>

using std::atan2;

#define SIG_VEL 4.80000  // ns/m
#define C_VEL   3.33564  // ns/m

namespace Muon {

  MuonClusterOnTrackCreator::MuonClusterOnTrackCreator
  (const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_errorScalingTool("Trk::RIO_OnTrackErrorScalingTool/RIO_OnTrackErrorScalingTool"),
      m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool")
  {
    // algtool interface - necessary!
    declareInterface<IMuonClusterOnTrackCreator>(this);
    declareInterface<IRIO_OnTrackCreator>(this);

    // --- some job options here.
    declareProperty("doCSC",  m_doCsc = true);
    declareProperty("doRPC",  m_doRpc = true);
    declareProperty("doTGC",  m_doTgc = true);
    declareProperty("DoRpcErrorScaling", m_scaleRpcCov = false );
    declareProperty("DoTgcErrorScaling", m_scaleTgcCov = false );
    declareProperty("DoCscErrorScaling", m_scaleCscCov = false );
    declareProperty("ErrorScalingTool", m_errorScalingTool );
    declareProperty("DoFixedErrorTgcEta", m_doFixedErrorTgcEta = false );
    declareProperty("DoFixedErrorRpcEta", m_doFixedErrorRpcEta = false );
    declareProperty("DoFixedErrorCscEta", m_doFixedErrorCscEta = false );
    declareProperty("DoFixedErrorTgcPhi", m_doFixedErrorTgcPhi = false );
    declareProperty("DoFixedErrorRpcPhi", m_doFixedErrorRpcPhi = false );
    declareProperty("DoFixedErrorCscPhi", m_doFixedErrorCscPhi = false );
    declareProperty("FixedErrorTgcEta", m_fixedErrorTgcEta = 5. );
    declareProperty("FixedErrorRpcEta", m_fixedErrorRpcEta = 5. );
    declareProperty("FixedErrorCscEta", m_fixedErrorCscEta = 5. );
    declareProperty("FixedErrorTgcPhi", m_fixedErrorTgcPhi = 5. );
    declareProperty("FixedErrorRpcPhi", m_fixedErrorRpcPhi = 5. );
    declareProperty("FixedErrorCscPhi", m_fixedErrorCscPhi = 5. );
  }


  MuonClusterOnTrackCreator::~MuonClusterOnTrackCreator(){}


  StatusCode MuonClusterOnTrackCreator::initialize()
  {
    if( AthAlgTool::initialize().isFailure() ){
      ATH_MSG_ERROR ( " AthAlgTool::initialize() failed " );
      return StatusCode::FAILURE;
    } 
    
    // Get DetectorStore service
    //
    if ( m_idHelper.retrieve().isFailure() ) {
      ATH_MSG_ERROR ( " Cannot retrieve  " << m_idHelper );
      return StatusCode::FAILURE;
    }

    // get error scaling tool
    //
    if(m_errorScalingTool.retrieve().isFailure())   {
      ATH_MSG_WARNING ( "Can not get error scaling tool " << m_errorScalingTool
			<< ", will trigger failure." );
      return StatusCode::SUCCESS;
    } else {
      m_scaleCscCov = m_errorScalingTool->needToScaleCsc();
      m_scaleRpcCov = m_errorScalingTool->needToScaleRpc();
      m_scaleTgcCov = m_errorScalingTool->needToScaleTgc();
      ATH_MSG_INFO ( "initialise() successful in " << name() );
    }

    return StatusCode::SUCCESS;
  }

  StatusCode MuonClusterOnTrackCreator::finalize()
  {
    return AthAlgTool::finalize();
  }



  const MuonClusterOnTrack* MuonClusterOnTrackCreator::createRIO_OnTrack(const Trk::PrepRawData& RIO,
									 const Amg::Vector3D& GP) const

  {
    MuonClusterOnTrack* MClT = nullptr;
 
    // check whether PrepRawData has detector element, if not there print warning 
    const Trk::TrkDetElementBase* EL = RIO.detectorElement();
    if( !EL ){
      ATH_MSG_WARNING ( "RIO does not have associated detectorElement!, cannot produce ROT" );
      return nullptr;
    }

    // MuClusterOnTrack production
    //
    // in RIO_OnTrack the local param and cov should have the same dimension
    Trk::LocalParameters locpar(RIO.localPosition ());

    if (RIO.localCovariance().cols()!=RIO.localCovariance().rows()){
      ATH_MSG_WARNING ( "Rows and colums not equal!");
      if (m_idHelper->isRpc(RIO.identify())){
        std::stringstream ss;
        ss << "RPC hit with (r,c)="<<RIO.localCovariance().rows()<<","<<RIO.localCovariance().cols();
        ATH_MSG_WARNING(ss.str().c_str() );
      }
    }

    if (RIO.localCovariance().cols() > 1 || 
	(m_idHelper->isTgc(RIO.identify()) && m_idHelper->measuresPhi(RIO.identify()))) {
      ATH_MSG_VERBOSE ( "Making 2-dim local parameters: " << m_idHelper->toString(RIO.identify()) );
    } else {
      Trk::DefinedParameter  radiusPar(RIO.localPosition().x(),Trk::locX);
      locpar = Trk::LocalParameters(radiusPar);
      ATH_MSG_VERBOSE ( "Making 1-dim local parameters: "  << m_idHelper->toString(RIO.identify()) );
    }

    

    Amg::Vector2D lp;
    double positionAlongStrip = 0;
    double positionAlongz     = 0;

    if( !EL->surface(RIO.identify()).globalToLocal(GP,GP,lp) ){
      Amg::Vector3D lpos = RIO.detectorElement()->surface(RIO.identify()).transform().inverse()*GP;
      ATH_MSG_WARNING ( "Extrapolated GlobalPosition not on detector surface! Distance " << lpos.z() );
      lp[Trk::locX]  = lpos.x();
      lp[Trk::locY]  = lpos.y();
      positionAlongz = lpos.z();
    }
    positionAlongStrip = lp[Trk::locY];
     
    // Error matrix production - expect more intelligent code here.
    //
    // Amg::MatrixX* cov  = 0;
    // Amg::MatrixX  oldLocalCov = RIO.localCovariance();
    // if ( m_idHelper->isCsc(RIO.identify()) && m_scaleCscCov ) {
    //   cov = m_errorScalingTool->createScaledCscCovariance(oldLocalCov);
    //   ATH_MSG_VERBOSE ( "CSC: new cov(0,0) is " << (*cov)(0,0) );
    // }
    // if ( m_idHelper->isRpc(RIO.identify()) && m_scaleRpcCov ) {
    //   cov = m_errorScalingTool->createScaledRpcCovariance(oldLocalCov);
    //   ATH_MSG_VERBOSE ( "RPC: new cov(0,0) is " << (*cov)(0,0)  );
    // }
    // if ( m_idHelper->isTgc(RIO.identify()) && m_scaleTgcCov ) {
    //   cov = m_errorScalingTool->createScaledTgcCovariance(oldLocalCov);
    //   ATH_MSG_VERBOSE ( "TGC: new cov(0,0) is " << (*cov)(0,0)  );
    // }
    // Amg::MatrixX loce = (cov)                   ?
    //   Amg::MatrixX(*cov)                       :
    //   Amg::MatrixX(RIO.localCovariance()) ;
    // delete cov;
    Amg::MatrixX loce = RIO.localCovariance();
    ATH_MSG_DEBUG ( "All: new err matrix is " << loce );
  
    if(  m_idHelper->isRpc(RIO.identify()) ){
      
      // cast to RpcPrepData
      const RpcPrepData* MClus   = dynamic_cast<const RpcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type RpcPrepData, cannot create ROT" );
	return nullptr;
      }
        
      bool measphi = m_idHelper->measuresPhi(RIO.identify());
      
      double fixedError = 1.;
      bool scale = false;
      // check whether to scale eta/phi hit
      if( m_doFixedErrorRpcEta && !measphi ){
	scale = true;
	fixedError = m_fixedErrorRpcEta;
      }else if (m_doFixedErrorRpcPhi && measphi) {
	scale = true;
	fixedError = m_fixedErrorRpcPhi;
      }
      if( scale ) {
	Amg::MatrixX mat(1,1);
	mat(0,0)  = fixedError*fixedError;
	loce=mat;
      }
      
       
      const MuonGM::RpcReadoutElement* rpc_readout_element = MClus->detectorElement(); 
      Amg::Vector3D posi          = rpc_readout_element->stripPos(RIO.identify());
      
      //let's correct rpc time subtracting delay due to the induced electric signal propagation along strip   
      double correct_time_along_strip = 0;
      if(measphi==0){correct_time_along_strip = rpc_readout_element -> distanceToEtaReadout( GP )/1000.* SIG_VEL;}
      else {correct_time_along_strip = rpc_readout_element -> distanceToPhiReadout( GP )/1000.* SIG_VEL;}
      if(positionAlongz)correct_time_along_strip = 0; // no correction if extrapolated GlobalPosition not on detector surface!
      
      //let's evaluate the average  delay due to the induced electric signal propagation along strip 
      double av_correct_time_along_strip = 0;
      if(measphi==0){av_correct_time_along_strip = rpc_readout_element -> distanceToEtaReadout( posi )/1000.* SIG_VEL;}
      else {av_correct_time_along_strip = rpc_readout_element -> distanceToPhiReadout( posi )/1000.* SIG_VEL;}
      
      //let's evaluate [real TOF - nominal TOF]
      double real_TOF_onRPCgap    = GP.mag()  /1000.* C_VEL;
      double nominal_TOF_onRPCgap = posi.mag()/1000.* C_VEL;
      
      //let's evaluate the total time correction
      double correct_time_tot = real_TOF_onRPCgap-nominal_TOF_onRPCgap+correct_time_along_strip-av_correct_time_along_strip;
    
      MClT = new RpcClusterOnTrack(MClus,locpar,loce,positionAlongStrip, MClus->time()-correct_time_tot);
      
      ATH_MSG_DEBUG (" correct_time_along_strip " << correct_time_along_strip<<" av_correct_time_along_strip " << av_correct_time_along_strip<<" real_TOF_onRPCgap " << real_TOF_onRPCgap<<" nominal_TOF_onRPCgap " << nominal_TOF_onRPCgap<<" MClus->time() " << MClus->time()<<" correct_time_tot " <<correct_time_tot );


    }else if( m_idHelper->isTgc(RIO.identify()) ){

      // cast to TgcPrepData
      const TgcPrepData* MClus   = dynamic_cast<const TgcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type TgcPrepData, cannot create ROT" );
	return nullptr;
      }

      // calculation of 2D error matrix for TGC phi strips
      if ( m_idHelper->measuresPhi(RIO.identify()) ){

	int stationeta=m_idHelper->stationEta(RIO.identify());
	int stripNo = m_idHelper->tgcIdHelper().channel(RIO.identify());
	int gasGap = m_idHelper->tgcIdHelper().gasGap(RIO.identify());

	const MuonGM::TgcReadoutElement *ele= MClus->detectorElement();

	double stripLength = ele->stripLength(gasGap,stripNo);
	double stripWidth=fabs(ele->stripMaxX(gasGap, stripNo, lp[Trk::locZ])-ele->stripMinX(gasGap, stripNo,  lp[Trk::locZ]));
	
	double localX1=ele->stripCtrX(gasGap, stripNo,  stripLength/2.);
	double localX2=ele->stripCtrX(gasGap, stripNo,  -stripLength/2.);
	if (stationeta>0) {
	  localX1=-localX1;
	  localX2=-localX2;
	}
	Amg::MatrixX mat(2,2);
	
	double phistereo=atan2(localX2-localX1,stripLength); 
	double Sn      = sin(phistereo); 
	double Sn2     = Sn*Sn;
	double Cs2     = 1.-Sn2;
	
	double V0      = stripWidth*stripWidth/12;
	if( m_doFixedErrorTgcPhi ) V0 = m_fixedErrorTgcPhi*m_fixedErrorTgcPhi;
	double V1      = stripLength*stripLength/12;
	mat(0,0)  = (Cs2*V0+Sn2*V1);
	mat.fillSymmetric(1,0,(Sn*sqrt(Cs2)*(V0-V1)));
	mat(1,1)  = (Sn2*V0+Cs2*V1);
	loce=mat;
      }else{
	if( m_doFixedErrorTgcEta ) {
	  Amg::MatrixX mat(1,1);
	  mat(0,0)  = m_fixedErrorTgcEta*m_fixedErrorTgcEta;
	  loce=mat;
	}
      }
          
      MClT = new TgcClusterOnTrack(MClus,locpar,loce,positionAlongStrip);

    }else if( m_idHelper->isCsc(RIO.identify()) ){

      // cast to CscPrepData
      const CscPrepData* MClus   = dynamic_cast<const CscPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type CscPrepData, cannot create ROT" );
	return nullptr;
      }

      bool measphi = m_idHelper->measuresPhi(RIO.identify());
      double fixedError = 1.;
      bool scale = false;
      // check whether to scale eta/phi hit
      if( m_doFixedErrorCscEta && !measphi ){
	scale = true;
	fixedError = m_fixedErrorCscEta;
      }else if (m_doFixedErrorCscPhi && measphi) {
	scale = true;
	fixedError = m_fixedErrorCscPhi;
      }
      if( scale ) {
	Amg::MatrixX mat(1,1);
	mat(0,0)  = fixedError*fixedError;
	loce=mat;
      }
      // current not changing CscClusterStatus but passing status of RIO
      MClT = new CscClusterOnTrack(MClus,locpar,loce,positionAlongStrip,MClus->status(),MClus->timeStatus());

    } else if ( m_idHelper->issTgc(RIO.identify()) ) {

      // cast to sTgcPrepData
      const sTgcPrepData* MClus   = dynamic_cast<const sTgcPrepData*> (&RIO);
      if (!MClus) {
      	ATH_MSG_WARNING ( "RIO not of type sTgcPrepData, cannot create ROT" );
      	return nullptr;
      }


      MClT = new sTgcClusterOnTrack(MClus, locpar, loce, positionAlongStrip);
    } else if (m_idHelper->isMM(RIO.identify())) {
          ATH_MSG_ERROR("called MuonClusterOnTrackCreator with MM cluster");
          return nullptr;
    }
    return MClT;
  }

  const MuonClusterOnTrack* MuonClusterOnTrackCreator::
  createRIO_OnTrack(const Trk::PrepRawData& RIO, const Amg::Vector3D& GP, const Amg::Vector3D&) const {
    return createRIO_OnTrack(RIO, GP);
  }

  const MuonClusterOnTrack* MuonClusterOnTrackCreator::correct(const Trk::PrepRawData& RIO,const Trk::TrackParameters& TP) const 
  {
    return createRIO_OnTrack(RIO, TP.position(), TP.momentum());
  }
}
