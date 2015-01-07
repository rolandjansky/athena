/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



///////////////////////////////////////////////////////////////////
//   Implementation file for class CscClusterOnTrackCreator
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// AlgTool used for MuonClusterOnTrack object production
///////////////////////////////////////////////////////////////////
// Version 1.0 20/07/2004
///////////////////////////////////////////////////////////////////

#include "CscClusterOnTrackCreator.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "MuonPrepRawData/RpcPrepData.h"
#include "MuonPrepRawData/TgcPrepData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonPrepRawData/CscStripPrepDataCollection.h"

#include "CscClusterization/ICscStripFitter.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "CscClusterization/ICscClusterUtilTool.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

using std::vector;
using std::setw;
using std::atan2;
using Muon::CscStripPrepDataContainer;
using Muon::CscStripPrepDataCollection;
using Muon::CscStripPrepData;
using Amg::Vector3D;
using Amg::Transform3D;

namespace Muon {

  
  CscClusterOnTrackCreator::CscClusterOnTrackCreator
  (const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_muonMgr(0),
      m_cscIdHelper(0),
      m_rpcIdHelper(0),
      m_tgcIdHelper(0),
      m_errorScalingTool("Trk::RIO_OnTrackErrorScalingTool/RIO_OnTrackErrorScalingTool"),
      m_stripFitter("CalibCscStripFitter/CalibCscStripFitter"),
      m_clusterFitter("QratCscClusterFitter/QratCscClusterFitter"),
      m_clusterUtilTool("CscClusterUtilTool/CscClusterUtilTool"),
      m_have_csc_tools(false)
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
    declareProperty("CscStripFitter",   m_stripFitter );
    declareProperty("CscClusterFitter", m_clusterFitter );
    declareProperty("CscClusterUtilTool", m_clusterUtilTool );
    declareProperty("FixedError",         m_fixedError = 5. );
    declareProperty("ErrorScaler",        m_errorScaler = 1. );
    declareProperty("ErrorScalerBeta",        m_errorScalerBeta = 0. );
    declareProperty("CscStripPrepDataLocation",  m_cscStripLocation = "CSC_Measurements");
    declareProperty("MinimumError", m_minimumError = 0.05 );
  }


  CscClusterOnTrackCreator::~CscClusterOnTrackCreator(){}

  /////////////////////////////////
  StatusCode CscClusterOnTrackCreator::initialize()  {

    if ( AthAlgTool::initialize().isFailure() ){
      ATH_MSG_ERROR ( " AlgTool::initialize failed " );
      return StatusCode::FAILURE;
    }

    if ( detStore()->retrieve( m_muonMgr ).isFailure() ) {
	ATH_MSG_ERROR ( " Cannot retrieve MuonGeoModel " );
	return StatusCode::FAILURE;
    }

    if (m_doCsc) {
      m_cscIdHelper = m_muonMgr->cscIdHelper();
    } else {
      ATH_MSG_WARNING ( "CSC deactivated via option." );
    }
    if (m_doRpc) {
      m_rpcIdHelper = m_muonMgr->rpcIdHelper();
    } else {
      ATH_MSG_WARNING ( "RPC deactivated via option." );
    }
    if (m_doTgc) {
      m_tgcIdHelper = m_muonMgr->tgcIdHelper();
    } else {
      ATH_MSG_WARNING ( "TGC deactivated via option." );
    }

    // -- check this, maybe the helpers are not necessary after all.

    // get error scaling tool
    //
    if ( m_errorScalingTool.retrieve().isFailure() )   {
      ATH_MSG_WARNING ( "Can not get error scaling tool " << m_errorScalingTool
                        << ", will trigger failure." );
      return StatusCode::SUCCESS;
    } else {
      m_scaleCscCov = m_errorScalingTool->needToScaleCsc();
      m_scaleRpcCov = m_errorScalingTool->needToScaleRpc();
      m_scaleTgcCov = m_errorScalingTool->needToScaleTgc();
      ATH_MSG_DEBUG ( "initialise() successful in " << name() );
    }

    if ( m_stripFitter.retrieve().isFailure() ) {
      ATH_MSG_WARNING ( "CscClusterOnTrackCreator " << name()
                        << ": unable to retrieve strip fitter " << m_stripFitter );
    } else {
      ATH_MSG_DEBUG ( "CscClusterOnTrackCreator " << name()
                     << ": retrieved " << m_stripFitter );
      m_have_csc_tools = true;
    }

    if ( m_clusterFitter.retrieve().isFailure() )   {
      ATH_MSG_WARNING ( "CscClusterOnTrackCreator " << name()
                        << ": unable to retrieve cluster fitter " << m_clusterFitter );
      m_have_csc_tools = false;
    } else {
      ATH_MSG_DEBUG ( "CscClusterOnTrackCreator " << name()
                     << ": retrieved " << m_clusterFitter );
    }


    if ( m_clusterUtilTool.retrieve().isFailure() )   {
      ATH_MSG_WARNING ( "CscClusterOnTrackCreator " << name()
                        << ": unable to retrieve cluster fitter " << m_clusterUtilTool );
      m_have_csc_tools = false;
    } else {
      ATH_MSG_DEBUG ( "CscClusterOnTrackCreator " << name()
                     << ": retrieved " << m_clusterUtilTool );
    }

    ATH_MSG_DEBUG (" sqrt( sigma_nominal^2 * alpha^2 + beta^2) where alpha(ErrorScaler) is " <<  m_errorScaler
		   << " and beta(ErrorScalerBeta) is " << m_errorScalerBeta );
    
    

    return StatusCode::SUCCESS;
  }

  StatusCode CscClusterOnTrackCreator::finalize()
  {
    return AthAlgTool::finalize();
  }

  ////////////////////////////////////////////////////////
  const MuonClusterOnTrack* CscClusterOnTrackCreator::
  createRIO_OnTrack(const Trk::PrepRawData& RIO, const Amg::Vector3D& GP) const {
    
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
    if (RIO.localCovariance().cols() > 1 ) {
      ATH_MSG_VERBOSE ( "Making 2-dim local parameters" );
    } else {
      Trk::DefinedParameter  radiusPar(RIO.localPosition().x(),Trk::locX);
      locpar = Trk::LocalParameters(radiusPar);
      ATH_MSG_VERBOSE ( "Making 1-dim local parameters" );
    }

    
    Amg::Vector2D lp;
    double positionAlongStrip = 0;

    if( !EL->surface(RIO.identify()).globalToLocal(GP,GP,lp) ){
      Amg::Vector3D lpos = RIO.detectorElement()->surface(RIO.identify()).transform().inverse()*GP;
      ATH_MSG_WARNING ( "Extrapolated GlobalPosition not on detector surface! Distance " << lpos.z() );
      lp[Trk::locX] = lpos.x();
      lp[Trk::locY] = lpos.y();
    }
    positionAlongStrip = lp[Trk::locY];  

    // Error matrix production - expect more intelligent code here.
    //
    Amg::MatrixX* cov  = 0;
    Amg::MatrixX  oldLocalCov = RIO.localCovariance();
    
    if ( m_cscIdHelper
         && m_cscIdHelper->is_csc(RIO.identify())
         && m_scaleCscCov ) {
      cov = m_errorScalingTool->createScaledCscCovariance(oldLocalCov);
      ATH_MSG_VERBOSE ( "CSC: new cov(0,0) is " << (*cov)(0,0) );
    }
    if ( m_rpcIdHelper
	 && m_rpcIdHelper->is_rpc(RIO.identify())
	 && m_scaleRpcCov ) {
      cov = m_errorScalingTool->createScaledRpcCovariance(oldLocalCov);
      ATH_MSG_VERBOSE ( "RPC: new cov(0,0) is " << (*cov)(0,0) );
    }
    if ( m_tgcIdHelper
	 && m_tgcIdHelper->is_tgc(RIO.identify())
	 && m_scaleTgcCov ) {
      cov = m_errorScalingTool->createScaledTgcCovariance(oldLocalCov);
      ATH_MSG_VERBOSE ( "TGC: new cov(1,1) is " << (*cov)(0,0) );
    }

    Amg::MatrixX loce = (cov)                   ?
      Amg::MatrixX(*cov)                       :
      Amg::MatrixX(RIO.localCovariance()) ;

    if(  m_rpcIdHelper && m_rpcIdHelper->is_rpc(RIO.identify()) ){
      // cast to RpcPrepData
      const RpcPrepData* MClus   = dynamic_cast<const RpcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type RpcPrepData, cannot create ROT" );
	return 0;
      }
      MClT = new RpcClusterOnTrack(MClus,locpar,loce,positionAlongStrip);

    }else if( m_tgcIdHelper && m_tgcIdHelper->is_tgc(RIO.identify()) ){
      
      // cast to TgcPrepData
      const TgcPrepData* MClus   = dynamic_cast<const TgcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type TgcPrepData, cannot create ROT" );
	return 0;
      }

      // calculation of 2D error matrix for TGC phi strips
      if ( m_tgcIdHelper->isStrip(RIO.identify())){

	int stationeta=m_tgcIdHelper->stationEta(RIO.identify());
	int stripNo = m_tgcIdHelper->channel(RIO.identify());
	int gasGap = m_tgcIdHelper->gasGap(RIO.identify());

	const MuonGM::TgcReadoutElement *ele= MClus->detectorElement();

	double stripLength = ele->stripLength(gasGap,stripNo);
	double stripWidth=fabs(ele->stripMaxX(gasGap, stripNo, lp[Trk::locZ])-ele->stripMinX(gasGap, stripNo,  lp[Trk::locZ]));
	
	double localX1=ele->stripCtrX(gasGap, stripNo,  stripLength/2.);
	double localX2=ele->stripCtrX(gasGap, stripNo,  -stripLength/2.);
	if (stationeta>0) {
	  localX1=-localX1;
	  localX2=-localX2;
	}
	Amg::MatrixX mat = Amg::MatrixX(2,2);
	
	double phistereo=atan2(localX2-localX1,stripLength); 
	double Sn      = sin(phistereo); 
	double Sn2     = Sn*Sn;
	double Cs2     = 1.-Sn2;
	
	double V0      = stripWidth*stripWidth/12;
	double V1      = stripLength*stripLength/12;
	mat(0,0) = (Cs2*V0+Sn2*V1);
	mat.fillSymmetric(1,0,(Sn*sqrt(Cs2)*(V0-V1)));
	mat(1,1) = (Sn2*V0+Cs2*V1);
	loce=mat;
      }
          
      MClT = new TgcClusterOnTrack(MClus,locpar,loce,positionAlongStrip);

    } else if ( m_cscIdHelper && m_cscIdHelper->is_csc(RIO.identify()) ){
      
      // cast to CscPrepData
      const CscPrepData* MClus   = dynamic_cast<const CscPrepData*> (&RIO);
      if (!MClus) {
        ATH_MSG_WARNING ( "RIO not of type CscPrepData, cannot create ROT" );
        return 0;
      }
      
      // current not changing CscClusterStatus but passing status of RIO
      MClT = new CscClusterOnTrack(MClus,locpar,loce,positionAlongStrip,MClus->status(), MClus->timeStatus(), MClus->time());

    }
    
    return MClT; 
  }

  /////////////////////////////////////////////////////////////////
  
  const MuonClusterOnTrack* CscClusterOnTrackCreator::
  createRIO_OnTrack(const Trk::PrepRawData& RIO, const Amg::Vector3D& GP,
                    const Amg::Vector3D& GD) const {

    if ( ! ( m_cscIdHelper && m_cscIdHelper->is_csc(RIO.identify()) ) ) {
      ATH_MSG_WARNING ( "CscClusterOnTrackCreator::createRIO_OnTrack is called by the other muon tech" );
      return 0;
    }
    
    MuonClusterOnTrack* MClT = 0;
    // check whether PrepRawData has detector element, if not there print warning 
    const Trk::TrkDetElementBase* EL = RIO.detectorElement();
    if( !EL ){
      ATH_MSG_WARNING ( "RIO does not have associated detectorElement!, cannot produce ROT" );
      return MClT;
    }
    // cast to CscPrepData : Moved to the front to avoid any memory allocation before return;
    const CscPrepData* MClus   = dynamic_cast<const CscPrepData*> (&RIO); 
    if (!MClus) {
      ATH_MSG_WARNING ( "RIO not of type CscPrepData, cannot create ROT" );
      return MClT;
    }

    // MuClusterOnTrack production
    //
    // in RIO_OnTrack the local param and cov should have the same dimension
    Trk::LocalParameters locpar = Trk::LocalParameters (RIO.localPosition ());
    if (RIO.localCovariance().cols() > 1 || 
	(m_tgcIdHelper->is_tgc(RIO.identify()) && m_tgcIdHelper->isStrip(RIO.identify()))) {
      ATH_MSG_VERBOSE ( "Making 2-dim local parameters" );
    } else {
      Trk::DefinedParameter  radiusPar(RIO.localPosition().x(),Trk::locX);
      locpar = Trk::LocalParameters(radiusPar);
      ATH_MSG_VERBOSE ( "Making 1-dim local parameters" );
    }

    Amg::Vector2D lp;
    double positionAlongStrip = 0;

    if( !EL->surface(RIO.identify()).globalToLocal(GP,GP,lp) ){
      Amg::Vector3D lpos = RIO.detectorElement()->surface(RIO.identify()).transform().inverse()*GP;
      ATH_MSG_WARNING ( "Extrapolated GlobalPosition not on detector surface! Distance " << lpos.z() );
      lp[Trk::locX] = lpos.x();
      lp[Trk::locY] = lpos.y();
    }
    positionAlongStrip = lp[Trk::locY];  

    // Error matrix production - expect more intelligent code here.
    //
    Amg::MatrixX* cov  = 0;
    Amg::MatrixX  oldLocalCov = RIO.localCovariance();
    
    if ( m_cscIdHelper 
	 && m_cscIdHelper->is_csc(RIO.identify())
	 && m_scaleCscCov ) {
      cov = m_errorScalingTool->createScaledCscCovariance(oldLocalCov);
      ATH_MSG_VERBOSE ( "CSC: new cov(0,0) is " << (*cov)(0,0) );
    }

    // postion Error is re-estimate only for precision fit cluster (eta)
    if ( m_have_csc_tools 
         && MClus->status() != Muon::CscStatusUnspoiled
         && MClus->status() != Muon::CscStatusSplitUnspoiled ) {
      // current not changing CscClusterStatus but passing status of RIO
      Amg::MatrixX loce = (cov) ?  Amg::MatrixX(*cov)
	:  Amg::MatrixX(RIO.localCovariance()) ;
      delete cov;
      MClT = new CscClusterOnTrack(MClus,locpar,loce,positionAlongStrip,MClus->status(),MClus->timeStatus(),MClus->time());
      
    } else {
      
      const MuonGM::CscReadoutElement *ele= MClus->detectorElement();
      Transform3D globalToLocal = ele->transform(MClus->identify()).inverse();
      Vector3D d(globalToLocal*GD);
      double tantheta = d.x()/d.z();
      
      //        ICscClusterFitter::StripFitList sfits;
      //        sfits.erase(sfits.begin(), sfits.end());
      //        m_clusterUtilTool->getStripFits(MClus,sfits);
      
      std::vector<ICscClusterFitter::Result> results, results0;
      results = m_clusterUtilTool->getRefitCluster(MClus,tantheta);
      results0 = m_clusterUtilTool->getRefitCluster(MClus,0);
      //        results = m_clusterFitter->fit(sfits,tantheta);
      
      ICscClusterFitter::Result res, res0;
      res = results[0];
      res0 = results0[0]; // result at normal angle to make error blown correctly in case of cosmic
      int fitresult = res.fitStatus;
      if ( fitresult ) {
        ATH_MSG_VERBOSE ( "  Precision fit failed which was succeeded: return="
                          << "cluStatus: " << res.clusterStatus << "fitStatus: " << res.fitStatus );
	Amg::MatrixX loce = (cov) ?  Amg::MatrixX(*cov)
	  : Amg::MatrixX(RIO.localCovariance()) ;
	delete cov;
	return new CscClusterOnTrack(MClus,locpar,loce,positionAlongStrip,MClus->status(),MClus->timeStatus(),MClus->time());
      } else {
        ATH_MSG_VERBOSE ( "  Precision fit succeeded" );
      }
      

      ATH_MSG_VERBOSE ( "  Angle from Segment:  " << " :: tanangle :  " << tantheta );
      
      // in case that we need to scale errors for cosmic/data we want to scale only for normal part
      // v0 only scaling up intrinsic term...
      //      double errorCorrected = sqrt(res.dposition*res.dposition - res0.dposition*res0.dposition
      //                                   + m_errorScaler*m_errorScaler*res0.dposition*res0.dposition); 
      // v1 scaling up total uncertainty and add up misalignment term....like sqrt( sigma_nominal^2 * alpha^2 + beta^2)
      double nominal_error = res.dposition;
      double errorCorrected = sqrt( nominal_error*nominal_error*m_errorScaler*m_errorScaler + m_errorScalerBeta*m_errorScalerBeta);
      if( errorCorrected < m_minimumError ) errorCorrected = m_minimumError;

      Amg::MatrixX* pcov = 0;
      Amg::MatrixX mat = Amg::MatrixX(1,1);
      mat.setIdentity();
      //        mat *= res.dposition*res.dposition;
      mat *= errorCorrected*errorCorrected;
      pcov = m_errorScalingTool->createScaledCscCovariance(mat);
      //        pcov = pcov_beforeScale;
      //        Trk::ErrorMatrix* newloce = new Trk::ErrorMatrix(pcov);
      
      Amg::MatrixX newloce = (pcov)   ? Amg::MatrixX(*pcov)
	:  Amg::MatrixX(RIO.localCovariance()) ;
      delete pcov;
      ATH_MSG_VERBOSE ( "All: new err matrix is " << newloce );
      ATH_MSG_VERBOSE ( "  dpos changed ====> " << Amg::error(newloce,Trk::loc1) ); 
      
      MClT = new CscClusterOnTrack(MClus,locpar,newloce,positionAlongStrip,MClus->status(),MClus->timeStatus(),MClus->time());
      ATH_MSG_VERBOSE ( "global postion of MClT :::: " << MClT->globalPosition() );
    }


    return MClT;

    
  }



  ///////
  const MuonClusterOnTrack* CscClusterOnTrackCreator::
  correct(const Trk::PrepRawData& RIO,const Trk::TrackParameters& TP) const {
    return createRIO_OnTrack(RIO,TP.position(),TP.momentum().unit());
  }
}
