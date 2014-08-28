/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
********************************************************************

NAME:     ComputePointingQuantities.cxx
PACKAGE:  PhotonAnalysisUtils

AUTHOR:  JF Marchand 
CREATED:  Dec 2011

PURPOSE:  Tool to compute pointing related quantities

Updated:  

********************************************************************/

#include "PhotonAnalysisUtils/ComputePointingQuantities.h"

#include "egammaInterfaces/IEMExtrapolCaloConversion.h"
#include "PhotonAnalysisUtils/IExtrapolCaloConversion.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

#include "TrkTrack/LinkToTrack.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "VxVertex/VxTrackAtVertex.h"


using HepGeom::Point3D;


#define UNKNOWN -99999

ComputePointingQuantities::ComputePointingQuantities(const std::string& type,
			   const std::string& name,
			   const IInterface* parent)
  : AthAlgTool(type, name, parent),
    //m_EMClusterErrorsTool("EMClusterErrorsTool"),
    m_EMExtrapolCaloConversion("EMExtrapolCaloConversion"),
    m_PAUExtrapolCaloConversion("PAUExtrapolCaloConversion"),
    m_calodepth("CaloDetDescr/CaloDepthTool")
{
  declareInterface<IComputePointingQuantities>(this);

  declareProperty("EMErrorDetailContainerName", m_EMErrorDetailContainerName="egDetailContainer", "Name of the EMErrorDetail container");
  //declareProperty("EMClusterErrorsTool",       m_EMClusterErrorsTool,       "the tool to access the cluster errors");
  declareProperty("EMExtrapolCaloConversion",  m_EMExtrapolCaloConversion,  "the tool to compute pointing for converted photons");
  declareProperty("PAUExtrapolCaloConversion", m_PAUExtrapolCaloConversion, "the tool to compute pointing for converted photons");
  declareProperty("CalorimeterDepth",          m_calodepth);
  declareProperty("beamCondSvcName",           m_beamCondSvcName="BeamCondSvc");

  m_beamCondSvc = 0;

  m_PI=3.14159265359;
}

ComputePointingQuantities::~ComputePointingQuantities() {}

/////////////////////////////////////////////////////////////////

//========================================       
StatusCode ComputePointingQuantities::initialize()
{
  ATH_MSG_DEBUG(" Initializing ComputePointingQuantities");
 
  /*
  if(m_EMClusterErrorsTool.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_EMClusterErrorsTool);
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Retrieved EMClusterErrorsTool " << m_EMClusterErrorsTool);   
  }
  */
 
  if(m_EMExtrapolCaloConversion.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_EMExtrapolCaloConversion);
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Retrieved EMExtrapolCaloConversion " << m_EMExtrapolCaloConversion);   
  }

  if(m_PAUExtrapolCaloConversion.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_PAUExtrapolCaloConversion);
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Retrieved PAUExtrapolCaloConversion " << m_PAUExtrapolCaloConversion);   
  }

  if (m_calodepth.retrieve().isFailure()) {
    ATH_MSG_ERROR("Failed to retrieve " << m_calodepth);
    return StatusCode::RECOVERABLE;
  } else {
    ATH_MSG_DEBUG("Retrieved CaloDepthTool " << m_calodepth);   
  }

  StatusCode sc = service(m_beamCondSvcName,m_beamCondSvc); /*  getting beam spot */
  if (sc.isFailure() || m_beamCondSvc == 0) {
    ATH_MSG_DEBUG("Could not find BeamCondSvc. Will use (0,0,0) if no vertex is given and extrapolation is needed.");
    m_beamCondSvc = 0;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}
//========================================
StatusCode ComputePointingQuantities::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:

StatusCode ComputePointingQuantities::execute(const egamma *eg)
{
  if (!eg->cluster()) return StatusCode::FAILURE;

  m_CaloPointing_eta           = UNKNOWN;
  m_CaloPointing_sigma_eta     = UNKNOWN;
  m_CaloPointing_zvertex       = UNKNOWN;
  m_CaloPointing_sigma_zvertex = UNKNOWN;
  m_HPV_eta                    = UNKNOWN;
  m_HPV_sigma_eta              = UNKNOWN;
  m_HPV_zvertex                = UNKNOWN;
  m_HPV_sigma_zvertex          = UNKNOWN;

  /*** Compute beamSpot position and sigma ***/
  double beamSpotX = 0.,    beamSpotY = 0.;//,    beamSpotZ = 0.;
  //double beamSpotSigX = 0., beamSpotSigY = 0., beamSpotSigZ = 56.;
  if (m_beamCondSvc) {
    Amg::Vector3D bpos = m_beamCondSvc->beamPos();
    beamSpotX = bpos.x();
    beamSpotY = bpos.y();
    //beamSpotZ = bpos.z();
    //beamSpotSigX = m_beamCondSvc->beamSigma(0);
    //beamSpotSigY = m_beamCondSvc->beamSigma(1);
    //beamSpotSigZ = m_beamCondSvc->beamSigma(2);
  }  

  /*** Compute shower depth ***/
  double R_s1 = UNKNOWN, Z_s1 = UNKNOWN, R_s2 = UNKNOWN, Z_s2 = UNKNOWN;
  if ( eg->author()!=(int)(egammaParameters::AuthorFrwd) ) {
    double etas2 = eg->cluster()->etaBE(2);
    double etas1 = eg->cluster()->etaBE(1);
    double phis2 = eg->cluster()->phiBE(2);    
    if ( fabs(etas2) < 1.5 ) { // barrel
      R_s1 = m_calodepth->radius(CaloCell_ID::EMB1,etas1,phis2);
      R_s2 = m_calodepth->radius(CaloCell_ID::EMB2,etas2,phis2);
      if(fabs(etas1)>0 && fabs(etas1)<2.5)
	Z_s1 = R_s1 * sinh(etas1);
      else
	Z_s1 = UNKNOWN;

      if(fabs(etas2)>0 && fabs(etas2)<2.5)
	Z_s2 = R_s2 * sinh(etas2);
      else
	Z_s2 = UNKNOWN;
    } else {                   // endcap
      Z_s1 = m_calodepth->radius(CaloCell_ID::EME1,etas1,phis2);
      Z_s2 = m_calodepth->radius(CaloCell_ID::EME2,etas2,phis2);
      if(fabs(etas1)>0 && fabs(etas1)<2.5)
	R_s1 = Z_s1 / sinh(etas1);
      else
	R_s1 = UNKNOWN;

      if(fabs(etas2)>0 && fabs(etas2)<2.5)
	R_s2 = Z_s2 / sinh(etas2);
      else
	R_s2 = UNKNOWN;
    }
  }


  /***      Compute CaloPointing only     ***/
  double CaloPointing_zvertex       = UNKNOWN;
  double CaloPointing_sigma_zvertex = UNKNOWN;
  double CaloPointing_eta           = UNKNOWN;
  double CaloPointing_sigma_eta     = UNKNOWN;

  if ( R_s2 - R_s1 ) {     //check division by 0 : this should never happen but prefer to be safe   

    double d_beamSpot       = sqrt(beamSpotX*beamSpotX + beamSpotY*beamSpotY);
    double phi_beamSpot     = atan2(beamSpotY,beamSpotX);
    double phi_Calo         = atan2(sin(eg->cluster()->phiBE(2)),cos(eg->cluster()->phiBE(2)));
    double r0_with_beamSpot = d_beamSpot*cos(phi_Calo-phi_beamSpot);
    
    if (eg->author()!=(int)(egammaParameters::AuthorFrwd)) {

      CaloPointing_eta           = asinh( (Z_s2-Z_s1) / (R_s2-R_s1) );
      CaloPointing_zvertex       = (Z_s1*(R_s2-r0_with_beamSpot) - Z_s2*(R_s1-r0_with_beamSpot)) / (R_s2-R_s1);
      CaloPointing_sigma_zvertex = 0.5*(R_s2+R_s1)*(0.060/sqrt(eg->cluster()->e()*0.001)) / ((eg->cluster()->sinTh())*(eg->cluster()->sinTh()));      

      //make a protection for DeltaEta(method ; eta cluster)>0.15
      if ( fabs(CaloPointing_eta-eg->cluster()->eta())>0.15)
	CaloPointing_eta = eg->cluster()->eta();
      
      CaloPointing_sigma_eta = (0.060/sqrt(eg->cluster()->e()*0.001))/(fabs(eg->cluster()->sinTh()));
    } else {
      CaloPointing_zvertex = CaloPointing_sigma_zvertex = CaloPointing_eta = CaloPointing_sigma_eta = UNKNOWN;
    }
  }

  m_CaloPointing_zvertex       = CaloPointing_zvertex;
  m_CaloPointing_sigma_zvertex = CaloPointing_sigma_zvertex;
  m_CaloPointing_eta           = CaloPointing_eta;
  m_CaloPointing_sigma_eta     = CaloPointing_sigma_eta;



  /*** Check if TRT only tracks in case of conversions ***/
  const std::vector<Trk::VxTrackAtVertex*>* trklist = 0; 
  if (eg->conversion()!=0)
    trklist = eg->conversion()->vxTrackAtVertex();
  
  bool isTRTonly_trk1 = false, isTRTonly_trk2 = false;
  if (eg->conversion()!=0) {  
    for (unsigned int i = 0 ; i<trklist->size() ; i++) {
      Trk::VxTrackAtVertex* trk = (*eg->conversion()->vxTrackAtVertex())[i]; 
      if(!trk) continue;
      Trk::ITrackLink* link = trk->trackOrParticleLink();
      if(!link) continue;
      const Trk::LinkToTrackParticleBase* tr_part = dynamic_cast<const Trk::LinkToTrackParticleBase*>(link);
      if(!tr_part || !(*tr_part)) continue;
      const Rec::TrackParticle* tp = dynamic_cast<const Rec::TrackParticle*>(**tr_part); 
      if(!tp) continue;
      if ( tp->trackSummary()->get(Trk::numberOfPixelHits)+tp->trackSummary()->get(Trk::numberOfSCTHits)==0 && tp->trackSummary()->get(Trk::numberOfTRTHits)>0 ) {
	if (i==0) isTRTonly_trk1 = true; 
	if (i==1) isTRTonly_trk2 = true; 
      }
    }
  }

  /*** Fill pointing variables ***/  
  if ( ( eg->conversion()==0 ) || 
       ( eg->conversion()!=0 && ( (trklist->size()==1 && isTRTonly_trk1) || 
				  (trklist->size()==2 && isTRTonly_trk1 && isTRTonly_trk2) ) )
       ) { //use calorimeter if no conversion (unconverted photons or electrons) or if TRT standalone conversion

    m_HPV_eta                    = CaloPointing_eta;
    m_HPV_sigma_eta              = CaloPointing_sigma_eta;
    m_HPV_zvertex                = CaloPointing_zvertex;
    m_HPV_sigma_zvertex          = CaloPointing_sigma_zvertex;

  } else if ( eg->conversion()!=0 ) { //Conversion case with at least one *non*-TRT standalone

    EMErrorDetail* m_emerrordetail = const_cast<EMErrorDetail*>(eg->detail<EMErrorDetail>(m_EMErrorDetailContainerName));
    if(m_emerrordetail){

      ATH_MSG_ERROR("No longer able to compute errors using EMClusterErrorsTool in xAOD.");
      /*
	// no longer works, need a replacement for EMClusterErrorsTool?
      const EMClusterErrorsParametrizations *pars = (m_EMClusterErrorsTool) ? m_EMClusterErrorsTool->getErrorParametrizations() : 0;
      StatusCode sc = m_PAUExtrapolCaloConversion->execute(eg, eg->conversion(), m_emerrordetail->getClusterEtaPosError(eg,pars));
      if ( sc.isFailure() )
	ATH_MSG_WARNING("failure in EMExtrapolCaloConversion");
      */

      m_HPV_eta           = m_PAUExtrapolCaloConversion->returnEta();
      m_HPV_sigma_eta     = m_PAUExtrapolCaloConversion->returnErrorEta();
      m_HPV_zvertex       = m_PAUExtrapolCaloConversion->returnZvertex();
      m_HPV_sigma_zvertex = m_PAUExtrapolCaloConversion->returnErrorZvertex();
    }
  }

  return StatusCode::SUCCESS; 
}
