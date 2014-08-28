/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
********************************************************************

NAME:     ExtrapolCaloConversion.cxx
PACKAGE:  To Decide

AUTHORS:  M. Escalier
CREATED:  Oct 2009

PURPOSE:  Calculate eta direction for conversions using a line from Calo 1st sampling+conversion vertex

Updated:  

********************************************************************/

#define UNKNOWN_BIS -99999

#include "PhotonAnalysisUtils/ExtrapolCaloConversion.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

// INCLUDE GAUDI HEADER FILES:

#include "GaudiKernel/ObjectVector.h"      
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"

#include <iostream>
#include <iomanip>


using HepGeom::Point3D;


ExtrapolCaloConversion::ExtrapolCaloConversion(const std::string& type,
			   const std::string& name,
			   const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_calodepth("CaloDetDescr/CaloDepthTool")
{
  declareInterface<IExtrapolCaloConversion>(this);

  declareProperty("CalorimeterDepth",m_calodepth);
  declareProperty("beamCondSvcName",          m_beamCondSvcName="BeamCondSvc");

  m_beamCondSvc = 0;

  m_PI=3.14159265359;
}

ExtrapolCaloConversion::~ExtrapolCaloConversion() {}

/////////////////////////////////////////////////////////////////

//========================================       
StatusCode ExtrapolCaloConversion::initialize()
{
  ATH_MSG_DEBUG(" Initializing ExtrapolCaloConversion");
 
  if (m_calodepth.retrieve().isFailure()) {
    ATH_MSG_FATAL("Cannot get CaloDepthTool");
    return StatusCode::FAILURE;
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
StatusCode ExtrapolCaloConversion::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// ATHENA EXECUTE METHOD:

StatusCode ExtrapolCaloConversion::execute(const egamma *eg,const Trk::VxCandidate* myvxcandidate,double error_etaS1)
{

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


  m_eta           = -99999.;
  m_error_eta     = -99999.;
  m_zvertex       = -99999.;
  m_error_zvertex = -99999.;

  double R_photon_front  = UNKNOWN_BIS;
  double Z_photon_front  = UNKNOWN_BIS;
  double R_photon_middle = UNKNOWN_BIS;
  double Z_photon_middle = UNKNOWN_BIS;

  double eta_cluster_2nd_sampling = eg->cluster()->etaBE(2); 
  double eta_cluster_1st_sampling = eg->cluster()->etaBE(1); 
  double phi_cluster_2nd_sampling = eg->cluster()->phiBE(2); 
  
  if ( fabs(eta_cluster_2nd_sampling)<1.5 ) { // barrel
    R_photon_front  = m_calodepth->radius(CaloCell_ID::EMB1,eta_cluster_1st_sampling,phi_cluster_2nd_sampling);
    R_photon_middle = m_calodepth->radius(CaloCell_ID::EMB2,eta_cluster_2nd_sampling,phi_cluster_2nd_sampling);
    Z_photon_front  = R_photon_front*sinh(eta_cluster_1st_sampling);
    Z_photon_middle = R_photon_middle*sinh(eta_cluster_2nd_sampling);
  } else {                                    // endcap
    Z_photon_front  = m_calodepth->radius(CaloCell_ID::EME1,eta_cluster_1st_sampling,phi_cluster_2nd_sampling);
    Z_photon_middle = m_calodepth->radius(CaloCell_ID::EME2,eta_cluster_2nd_sampling,phi_cluster_2nd_sampling);
    R_photon_front  = Z_photon_front/sinh(eta_cluster_1st_sampling);
    R_photon_middle = Z_photon_middle/sinh(eta_cluster_2nd_sampling);
  }

  double X_photon_Conversion = myvxcandidate->recVertex().position()[0];
  double Y_photon_Conversion = myvxcandidate->recVertex().position()[1];
  double Z_photon_Conversion = myvxcandidate->recVertex().position()[2];
  double R_photon_Conversion = sqrt(X_photon_Conversion*X_photon_Conversion+Y_photon_Conversion*Y_photon_Conversion);

  double DeltaPhi = atan2(Y_photon_Conversion,X_photon_Conversion)-eg->cluster()->phi();
  
  if (DeltaPhi>m_PI)
    DeltaPhi-=2.*m_PI;
  else if (DeltaPhi<-m_PI)
    DeltaPhi+=2.*m_PI;
  
  if (fabs(DeltaPhi)>m_PI/2.)
    R_photon_Conversion=-R_photon_Conversion;

  if (R_photon_Conversion-R_photon_front) {     //check division by 0 : this should never happen but prefer to be safe
    
    //-----------------------------------take into account beam shift -----------------
    double d_beam_spot = sqrt(beamSpotX*beamSpotX+beamSpotY*beamSpotY);
    double phi_beam_spot = atan2(beamSpotY,beamSpotX);
    double phi_CaloConversion = atan2(sin(eg->cluster()->phiBE(2)),cos(eg->cluster()->phiBE(2)));
    double r0_with_beam_spot = d_beam_spot*cos(phi_CaloConversion-phi_beam_spot);
    //---------------------------------------------
    m_zvertex = ( Z_photon_front*(R_photon_Conversion-r0_with_beam_spot)-
		  Z_photon_Conversion*(R_photon_front-r0_with_beam_spot) ) / (R_photon_Conversion-R_photon_front);

    m_eta = asinh( (Z_photon_front-Z_photon_Conversion)/(R_photon_front-R_photon_Conversion) );

    
    // double error_eta = 0.30e-3*sqrt(100./(eg->cluster()->e()*0.001));    // from CSC note, page 51

    double dist_vtx_to_conv = sqrt((R_photon_Conversion-r0_with_beam_spot)*(R_photon_Conversion-r0_with_beam_spot)+
				  (Z_photon_Conversion-m_zvertex)*(Z_photon_Conversion-m_zvertex));
    double dist_conv_to_s1 = sqrt((R_photon_front-R_photon_Conversion)*(R_photon_front-R_photon_Conversion)+
				 (Z_photon_front-Z_photon_Conversion)*(Z_photon_front-Z_photon_Conversion));

    double error_R_Calo_1st_Sampling_endcap = 0; // will be computed in next lines
    double error_Z_Calo_1st_Sampling_barrel = 0;


    if ( (eg->cluster()->inBarrel() && !eg->cluster()->inEndcap()) || (eg->cluster()->inBarrel() && eg->cluster()->inEndcap() && eg->cluster()->eSample(CaloSampling::EMB1) > eg->cluster()->eSample(CaloSampling::EME1) ) ) { //Barrel case

      error_Z_Calo_1st_Sampling_barrel = error_etaS1 * R_photon_front*fabs(cosh(eg->cluster()->etaBE(1)));
      double InfoErrorEta_num = error_Z_Calo_1st_Sampling_barrel;
      double InfoErrorEta_denom = sqrt((R_photon_front-R_photon_Conversion)*(R_photon_front-R_photon_Conversion)+(Z_photon_front-Z_photon_Conversion)*(Z_photon_front-Z_photon_Conversion));

      m_error_eta = InfoErrorEta_num/InfoErrorEta_denom;
      m_error_zvertex = error_Z_Calo_1st_Sampling_barrel*dist_vtx_to_conv/dist_conv_to_s1;

    } else if( ( !eg->cluster()->inBarrel() && eg->cluster()->inEndcap() ) || (  eg->cluster()->inBarrel() && eg->cluster()->inEndcap() && eg->cluster()->eSample(CaloSampling::EME1) > eg->cluster()->eSample(CaloSampling::EMB1) ) ) { //EndCap case

      error_R_Calo_1st_Sampling_endcap = error_etaS1*cosh(eg->cluster()->etaBE(1))*(R_photon_front*R_photon_front)/fabs(Z_photon_front);
      double InfoErrorEta_num = error_R_Calo_1st_Sampling_endcap*(Z_photon_front-Z_photon_Conversion);
      double InfoErrorEta_denom = (R_photon_front-R_photon_Conversion)*sqrt((R_photon_front-R_photon_Conversion)*(R_photon_front-R_photon_Conversion)+(Z_photon_front-Z_photon_Conversion)*(Z_photon_front-Z_photon_Conversion));

      m_error_eta     = InfoErrorEta_num/InfoErrorEta_denom;          
      m_error_zvertex = error_R_Calo_1st_Sampling_endcap*fabs(sinh(eg->cluster()->etaBE(1)))*dist_vtx_to_conv/dist_conv_to_s1;

    }
  }


  return StatusCode::SUCCESS; 
}

