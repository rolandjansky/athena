/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlgTool used for MuonClusterOnTrack object production
///////////////////////////////////////////////////////////////////

#include "MuonClusterOnTrackCreator.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"

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

#define SIG_VEL 4.80000  // ns/m
#define C_VEL   3.33564  // ns/m

namespace Muon {

  MuonClusterOnTrackCreator::MuonClusterOnTrackCreator
  (const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa)
  {
    // algtool interface - necessary!
    declareInterface<IMuonClusterOnTrackCreator>(this);
    declareInterface<IRIO_OnTrackCreator>(this);

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
    declareProperty("eDriftVelocityMM", m_eDriftVelocityMM = 0.047 );
  }


  MuonClusterOnTrackCreator::~MuonClusterOnTrackCreator(){}


  StatusCode MuonClusterOnTrackCreator::initialize()
  {
    if( AthAlgTool::initialize().isFailure() ){
      ATH_MSG_ERROR ( " AthAlgTool::initialize() failed " );
      return StatusCode::FAILURE;
    } 
    
    ATH_CHECK(m_idHelperSvc.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode MuonClusterOnTrackCreator::finalize()
  {
    return AthAlgTool::finalize();
  }



  const MuonClusterOnTrack* MuonClusterOnTrackCreator::createRIO_OnTrack(const Trk::PrepRawData& RIO,
									 const Amg::Vector3D& GP, const Amg::Vector3D& GD) const

  {
    MuonClusterOnTrack* MClT = 0;
 
    // check whether PrepRawData has detector element, if not there print warning 
    const Trk::TrkDetElementBase* EL = RIO.detectorElement();
    if( !EL ){
      ATH_MSG_WARNING ( "RIO does not have associated detectorElement!, cannot produce ROT" );
      return 0;
    }

    // MuClusterOnTrack production
    //
    // in RIO_OnTrack the local param and cov should have the same dimension
    Trk::LocalParameters locpar(RIO.localPosition ());

    if (RIO.localCovariance().cols()!=RIO.localCovariance().rows()){
      ATH_MSG_WARNING ( "Rows and colums not equal!");
      if (m_idHelperSvc->isRpc(RIO.identify())){
        std::stringstream ss;
        ss << "RPC hit with (r,c)="<<RIO.localCovariance().rows()<<","<<RIO.localCovariance().cols();
        ATH_MSG_WARNING(ss.str().c_str() );
      }
    }

    if (RIO.localCovariance().cols() > 1 || 
	(m_idHelperSvc->isTgc(RIO.identify()) && m_idHelperSvc->measuresPhi(RIO.identify()))) {
      ATH_MSG_VERBOSE ( "Making 2-dim local parameters: " << m_idHelperSvc->toString(RIO.identify()) );
    } else {
      Trk::DefinedParameter  radiusPar(RIO.localPosition().x(),Trk::locX);
      locpar = Trk::LocalParameters(radiusPar);
      ATH_MSG_VERBOSE ( "Making 1-dim local parameters: "  << m_idHelperSvc->toString(RIO.identify()) );
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
     
    Amg::MatrixX loce = RIO.localCovariance();
    ATH_MSG_DEBUG ( "All: new err matrix is " << loce );
  
    if(  m_idHelperSvc->isRpc(RIO.identify()) ){
      
      // cast to RpcPrepData
      const RpcPrepData* MClus   = dynamic_cast<const RpcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type RpcPrepData, cannot create ROT" );
	return 0;
      }
        
      bool measphi = m_idHelperSvc->measuresPhi(RIO.identify());
      
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


    }else if( m_idHelperSvc->isTgc(RIO.identify()) ){

      // cast to TgcPrepData
      const TgcPrepData* MClus   = dynamic_cast<const TgcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type TgcPrepData, cannot create ROT" );
	return 0;
      }

      // calculation of 2D error matrix for TGC phi strips
      if ( m_idHelperSvc->measuresPhi(RIO.identify()) ){

	int stationeta=m_idHelperSvc->stationEta(RIO.identify());
	int stripNo = m_idHelperSvc->tgcIdHelper().channel(RIO.identify());
	int gasGap = m_idHelperSvc->tgcIdHelper().gasGap(RIO.identify());

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
	
	double phistereo=std::atan2(localX2-localX1,stripLength); 
	double Sn      = std::sin(phistereo); 
	double Sn2     = Sn*Sn;
	double Cs2     = 1.-Sn2;
	
	double V0      = stripWidth*stripWidth/12;
	if( m_doFixedErrorTgcPhi ) V0 = m_fixedErrorTgcPhi*m_fixedErrorTgcPhi;
	double V1      = stripLength*stripLength/12;
	mat(0,0)  = (Cs2*V0+Sn2*V1);
	mat.fillSymmetric(1,0,(Sn*std::sqrt(Cs2)*(V0-V1)));
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

    }else if( m_idHelperSvc->isCsc(RIO.identify()) ){

      const CscPrepData* MClus   = dynamic_cast<const CscPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type CscPrepData, cannot create ROT" );
	return 0;
      }

      bool measphi = m_idHelperSvc->measuresPhi(RIO.identify());
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

    }else if( m_idHelperSvc->isMM(RIO.identify()) ){
      // cast to MMPrepData
      const MMPrepData* MClus   = dynamic_cast<const MMPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type MMPrepData, cannot create ROT" );
	return 0;
      }

      Amg::Vector2D lposMM = MClus->localPosition();

      double tOffset = 0.0;
      double zshift = m_eDriftVelocityMM * (MClus->time() - tOffset);

      ATH_MSG_DEBUG( " Drift time " << MClus->time() << " zshift " << zshift);
      if (zshift<-5.05) {
        ATH_MSG_WARNING( "Invalid drift time: zshift " << zshift << " while volume size 10: [-5;5]. Set zshift = -5.05");
	zshift = -5.05;
      } else if (zshift>5.) {
        ATH_MSG_WARNING( "Invalid drift time: zshift " << zshift << " while volume size 10: [-5;5]. Set zshift = 5.05");
	zshift = 5.05;
      }
      ATH_MSG_DEBUG( " MM Prepdata pos locX " << lposMM[0] << " locY " << lposMM[1] << " and zshift " << zshift);

      Amg::Vector3D lposTrack = RIO.detectorElement()->surface(RIO.identify()).transform().inverse()*GP;
      Amg::Vector3D GPinGasGap = GP + Amg::Vector3D(0.,0.,zshift);
      Amg::Vector3D GPshifted = GPinGasGap;
      if (std::abs(GD.z()) > 0.000001)
        GPshifted-= zshift*GD/GD.z();
      else
        ATH_MSG_WARNING ( "GD.z() is 0, mTPC correction cannot be done" );;

      ATH_MSG_DEBUG( " GD x " << GD.x() << " y " << GD.y() << " z " << GD.z());
      ATH_MSG_DEBUG( " GP x " << GP.x() << " y " << GP.y() << " z " << GP.z());
      ATH_MSG_DEBUG( " GPshifted x " << GPshifted.x() << " y " << GPshifted.y() << " z " << GPshifted.z());

      ATH_MSG_DEBUG( " lposTrack x " << lposTrack.x() << " y " << lposTrack.y() << " z " << lposTrack.z());
      Amg::Vector3D lposTrackShifted = RIO.detectorElement()->surface(RIO.identify()).transform().inverse()*GPshifted;
      ATH_MSG_DEBUG( " lposTrackShifted x " << lposTrackShifted.x() << " y " << lposTrackShifted.y() << " z " << lposTrackShifted.z());
      double locXshift = lposTrackShifted.x() - lposTrack.x();
      double locYshift = lposTrackShifted.y() - lposTrack.y();
      ATH_MSG_DEBUG( " MM ClusterOnTrack locXshift " << locXshift);

      ATH_MSG_DEBUG( " MM ClusterOnTrack old pos locX " << locpar[Trk::locX] << " Old position along strip " << positionAlongStrip);
      positionAlongStrip += locYshift;
      locpar[Trk::locX] = lposMM[0] + locXshift;
      ATH_MSG_DEBUG( " MM ClusterOnTrack new pos locX " << locpar[Trk::locX] << " New position along strip " << positionAlongStrip);

      MClT = new MMClusterOnTrack(MClus,locpar,loce,positionAlongStrip);

    }else if( m_idHelperSvc->issTgc(RIO.identify()) ){

      const sTgcPrepData* MClus   = dynamic_cast<const sTgcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type MMPrepData, cannot create ROT" );
	return 0;
      }

      
      MClT = new sTgcClusterOnTrack(MClus,locpar,loce,positionAlongStrip);
    }
    return MClT; 
  }  

  const MuonClusterOnTrack* MuonClusterOnTrackCreator::
  createRIO_OnTrack(const Trk::PrepRawData& RIO, const Amg::Vector3D& GP) const {
    const Amg::Vector3D GD = GP;
    return createRIO_OnTrack(RIO,GP,GD);
  }

  const MuonClusterOnTrack* MuonClusterOnTrackCreator::correct(const Trk::PrepRawData& RIO,const Trk::TrackParameters& TP) const 
  {
    return createRIO_OnTrack(RIO,TP.position(),TP.momentum());
  }
}
