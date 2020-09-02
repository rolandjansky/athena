/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscClusterOnTrackCreator.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/Surface.h"

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

#include "TrkRIO_OnTrack/check_cast.h"

using Muon::CscStripPrepDataContainer;
using Muon::CscStripPrepDataCollection;
using Muon::CscStripPrepData;
using Amg::Vector3D;
using Amg::Transform3D;

namespace Muon {

  
  CscClusterOnTrackCreator::CscClusterOnTrackCreator
  (const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_stripFitter("CalibCscStripFitter/CalibCscStripFitter", this),
      m_clusterFitter("QratCscClusterFitter/QratCscClusterFitter", this),
      m_clusterUtilTool("CscClusterUtilTool/CscClusterUtilTool", this),
      m_have_csc_tools(false)
  {
    // algtool interface - necessary!
    declareInterface<IMuonClusterOnTrackCreator>(this);
    declareInterface<IRIO_OnTrackCreator>(this);

    // --- some job options here.
    declareProperty("doCSC",  m_doCsc = true);
    declareProperty("doRPC",  m_doRpc = true);
    declareProperty("doTGC",  m_doTgc = true);
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

    ATH_CHECK( m_idHelperSvc.retrieve() );

    if (!m_idHelperSvc->hasCSC()) {
      ATH_MSG_ERROR("The given detector layout does not contain any CSC chamber, there must be something wrong in the configuration, since the CscClusterOnTrackCreator cannot be needed.");
      return StatusCode::FAILURE;
    }

    // get error scaling tool
    //
    if (!m_cscErrorScalingKey.key().empty() ) {
      ATH_CHECK( m_cscErrorScalingKey.initialize() );
    }
    if (!m_tgcErrorScalingKey.key().empty() ) {
      ATH_CHECK( m_tgcErrorScalingKey.initialize() );
    }
    if (!m_rpcErrorScalingKey.key().empty() ) {
      ATH_CHECK( m_rpcErrorScalingKey.initialize() );
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
    Amg::MatrixX  loce = RIO.localCovariance();

    if ( m_doCsc
         && m_idHelperSvc->isCsc(RIO.identify())
         && !m_cscErrorScalingKey.key().empty()) {
      SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_cscErrorScalingKey );
      loce = check_cast<MuonEtaPhiRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( loce, Trk::distPhi);
      ATH_MSG_VERBOSE ( "CSC: new cov(0,0) is " << loce(0,0) );
    }
    if ( m_doRpc
	 && m_idHelperSvc->isRpc(RIO.identify())
	 && !m_rpcErrorScalingKey.key().empty()) {
      SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_rpcErrorScalingKey );
      loce = check_cast<MuonEtaPhiRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( loce, Trk::distPhi);
      ATH_MSG_VERBOSE ( "RPC: new cov(0,0) is " << loce(0,0) );
    }
    if ( m_doTgc
	 && m_idHelperSvc->isTgc(RIO.identify())
	 && !m_tgcErrorScalingKey.key().empty() ) {
      SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_tgcErrorScalingKey );
      loce = check_cast<MuonEtaPhiRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( loce, Trk::distPhi);
      ATH_MSG_VERBOSE ( "TGC: new cov(1,1) is " << loce(0,0) );
    }

    if(  m_doRpc && m_idHelperSvc->isRpc(RIO.identify()) ){
      // cast to RpcPrepData
      const RpcPrepData* MClus   = dynamic_cast<const RpcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type RpcPrepData, cannot create ROT" );
	return 0;
      }
      MClT = new RpcClusterOnTrack(MClus,locpar,loce,positionAlongStrip);

    }else if( m_doTgc && m_idHelperSvc->isTgc(RIO.identify()) ){
      
      // cast to TgcPrepData
      const TgcPrepData* MClus   = dynamic_cast<const TgcPrepData*> (&RIO);
      if (!MClus) {
	ATH_MSG_WARNING ( "RIO not of type TgcPrepData, cannot create ROT" );
	return 0;
      }

      // calculation of 2D error matrix for TGC phi strips
      if ( m_idHelperSvc->tgcIdHelper().isStrip(RIO.identify())){

	int stationeta=m_idHelperSvc->tgcIdHelper().stationEta(RIO.identify());
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
	Amg::MatrixX mat = Amg::MatrixX(2,2);
	
	double phistereo=std::atan2(localX2-localX1,stripLength); 
	double Sn      = std::sin(phistereo); 
	double Sn2     = Sn*Sn;
	double Cs2     = 1.-Sn2;
	
	double V0      = stripWidth*stripWidth/12;
	double V1      = stripLength*stripLength/12;
	mat(0,0) = (Cs2*V0+Sn2*V1);
	mat.fillSymmetric(1,0,(Sn*std::sqrt(Cs2)*(V0-V1)));
	mat(1,1) = (Sn2*V0+Cs2*V1);
	loce=mat;
      }
          
      MClT = new TgcClusterOnTrack(MClus,locpar,loce,positionAlongStrip);

    } else if ( m_doCsc && m_idHelperSvc->isCsc(RIO.identify()) ){
      
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

    if ( ! ( m_doCsc && m_idHelperSvc->isCsc(RIO.identify()) ) ) {
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
	(m_idHelperSvc->isTgc(RIO.identify()) && m_idHelperSvc->tgcIdHelper().isStrip(RIO.identify()))) {
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
    Amg::MatrixX  loce = RIO.localCovariance();
    
    if ( m_doCsc 
	 && m_idHelperSvc->isCsc(RIO.identify())
         && !m_cscErrorScalingKey.key().empty()) {
      SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_cscErrorScalingKey );
      loce = check_cast<MuonEtaPhiRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( loce, Trk::distPhi);
      ATH_MSG_VERBOSE ( "CSC: new cov(0,0) is " << loce(0,0) );
    }

    // postion Error is re-estimate only for precision fit cluster (eta)
    if ( m_have_csc_tools 
         && MClus->status() != Muon::CscStatusUnspoiled
         && MClus->status() != Muon::CscStatusSplitUnspoiled ) {
      // current not changing CscClusterStatus but passing status of RIO
      MClT = new CscClusterOnTrack(MClus,locpar,loce,positionAlongStrip,MClus->status(),MClus->timeStatus(),MClus->time());
      
    } else {
      
      const MuonGM::CscReadoutElement *ele= MClus->detectorElement();
      Transform3D globalToLocal = ele->transform(MClus->identify()).inverse();
      Vector3D d(globalToLocal*GD);
      double tantheta = d.x()/d.z();

      std::vector<ICscClusterFitter::Result> results, results0;
      results = m_clusterUtilTool->getRefitCluster(MClus,tantheta);
      results0 = m_clusterUtilTool->getRefitCluster(MClus,0);
      
      ICscClusterFitter::Result res, res0;
      res = results[0];
      res0 = results0[0]; // result at normal angle to make error blown correctly in case of cosmic
      int fitresult = res.fitStatus;
      if ( fitresult ) {
        ATH_MSG_VERBOSE ( "  Precision fit failed which was succeeded: return="
                          << "cluStatus: " << res.clusterStatus << "fitStatus: " << res.fitStatus );
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
      double errorCorrected = std::sqrt( nominal_error*nominal_error*m_errorScaler*m_errorScaler + m_errorScalerBeta*m_errorScalerBeta);
      if( errorCorrected < m_minimumError ) errorCorrected = m_minimumError;

      Amg::MatrixX newloce( Amg::MatrixX(1,1) );
      newloce.setIdentity();
      newloce *= errorCorrected*errorCorrected;
      if (!m_cscErrorScalingKey.key().empty()) {
        SG::ReadCondHandle<RIO_OnTrackErrorScaling> error_scaling( m_cscErrorScalingKey );
        newloce = check_cast<MuonEtaPhiRIO_OnTrackErrorScaling>(*error_scaling)->getScaledCovariance( newloce, Trk::distPhi);
      }
      
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

  /// These functions are provided from the interface
  const ToolHandle<ICscStripFitter>& CscClusterOnTrackCreator::GetICscStripFitter() const {return m_stripFitter;}
  const ToolHandle<ICscClusterFitter>& CscClusterOnTrackCreator::GetICscClusterFitter() const {return m_clusterFitter;}
  const ToolHandle<ICscClusterUtilTool>& CscClusterOnTrackCreator::GetICscClusterUtilTool() const {return m_clusterUtilTool;}

}
