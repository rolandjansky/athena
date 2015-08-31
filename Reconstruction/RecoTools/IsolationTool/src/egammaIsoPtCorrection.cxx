/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaIsoPtCorrection.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include <cmath> 


// -------------------------------------------------------------
// Constructor 
// -------------------------------------------------------------
egammaIsoPtCorrection::egammaIsoPtCorrection(const std::string& type,
		     const std::string& name,
		     const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  // declare Interface
  declareInterface<IegammaIsoPtCorrection>(this);
  // Eta binning
  declareProperty("EtaBins",m_EtaBins, "Eta binning");
  // Pt Correction parameter for electron etcone
  declareProperty("ElectronPtCorrection15",m_ElectronPtCorrection15, "Pt correction(Slope) for etcone15");
  declareProperty("ElectronPtCorrection20",m_ElectronPtCorrection20, "Pt correction(Slope) for etcone20");
  declareProperty("ElectronPtCorrection25",m_ElectronPtCorrection25, "Pt correction(Slope) for etcone25");
  declareProperty("ElectronPtCorrection30",m_ElectronPtCorrection30, "Pt correction(Slope) for etcone30");
  declareProperty("ElectronPtCorrection35",m_ElectronPtCorrection35, "Pt correction(Slope) for etcone35");
  declareProperty("ElectronPtCorrection40",m_ElectronPtCorrection40, "Pt correction(Slope) for etcone40");
  declareProperty("ElectronPtCorrectiontp40",m_ElectronPtCorrectiontp40, "Pt correction(Slope) etcone40 topo cluster");
  // Offset parameter for electron etcone
  declareProperty("ElectronOffset15",m_ElectronOffset15, "Offset for etcone15");
  declareProperty("ElectronOffset20",m_ElectronOffset20, "Offset for etcone20");
  declareProperty("ElectronOffset25",m_ElectronOffset25, "Offset for etcone25");
  declareProperty("ElectronOffset30",m_ElectronOffset30, "Offset for etcone30");
  declareProperty("ElectronOffset35",m_ElectronOffset35, "Offset for etcone35");
  declareProperty("ElectronOffset40",m_ElectronOffset40, "Offset for etcone40");
  declareProperty("ElectronOffsettp40",m_ElectronOffsettp40, "Offset for etcone40 topo cluster");
  // Pt Correction parameter for photon etcone
  declareProperty("PhotonConvertedPtCorrection15",m_PhotonConvertedPtCorrection15, "Pt correction(Slope) for etcone15");
  declareProperty("PhotonConvertedPtCorrection20",m_PhotonConvertedPtCorrection20, "Pt correction(Slope) for etcone20");
  declareProperty("PhotonConvertedPtCorrection25",m_PhotonConvertedPtCorrection25, "Pt correction(Slope) for etcone25");
  declareProperty("PhotonConvertedPtCorrection30",m_PhotonConvertedPtCorrection30, "Pt correction(Slope) for etcone30");
  declareProperty("PhotonConvertedPtCorrection35",m_PhotonConvertedPtCorrection35, "Pt correction(Slope) for etcone35");
  declareProperty("PhotonConvertedPtCorrection40",m_PhotonConvertedPtCorrection40, "Pt correction(Slope) for etcone40");
  declareProperty("PhotonConvertedPtCorrectiontp40",m_PhotonConvertedPtCorrectiontp40, "Pt correction(Slope) for etcone40 topo cluster");
  // Offset parameter for photon etcone
  declareProperty("PhotonConvertedOffset15",m_PhotonConvertedOffset15, "Offset for etcone15");
  declareProperty("PhotonConvertedOffset20",m_PhotonConvertedOffset20, "Offset for etcone20");
  declareProperty("PhotonConvertedOffset25",m_PhotonConvertedOffset25, "Offset for etcone25");
  declareProperty("PhotonConvertedOffset30",m_PhotonConvertedOffset30, "Offset for etcone30");
  declareProperty("PhotonConvertedOffset35",m_PhotonConvertedOffset35, "Offset for etcone35");
  declareProperty("PhotonConvertedOffset40",m_PhotonConvertedOffset40, "Offset for etcone40");
  declareProperty("PhotonConvertedOffsettp40",m_PhotonConvertedOffsettp40, "Offset for etcone40 topo cluster");
  // Pt Correction parameter for unconverted photon etcone
  declareProperty("PhotonUnconvertedPtCorrection15",m_PhotonUnconvertedPtCorrection15, "Pt correction(Slope) for etcone15");
  declareProperty("PhotonUnconvertedPtCorrection20",m_PhotonUnconvertedPtCorrection20, "Pt correction(Slope) for etcone20");
  declareProperty("PhotonUnconvertedPtCorrection25",m_PhotonUnconvertedPtCorrection25, "Pt correction(Slope) for etcone25");
  declareProperty("PhotonUnconvertedPtCorrection30",m_PhotonUnconvertedPtCorrection30, "Pt correction(Slope) for etcone30");
  declareProperty("PhotonUnconvertedPtCorrection35",m_PhotonUnconvertedPtCorrection35, "Pt correction(Slope) for etcone35");
  declareProperty("PhotonUnconvertedPtCorrection40",m_PhotonUnconvertedPtCorrection40, "Pt correction(Slope) for etcone40");
  declareProperty("PhotonUnconvertedPtCorrectiontp40",m_PhotonUnconvertedPtCorrectiontp40, "Pt correction(Slope) for etconetp40");
  // Offset parameter for unconverted photon etcone
  declareProperty("PhotonUnconvertedOffset15",m_PhotonUnconvertedOffset15, "Offset for etcone15");
  declareProperty("PhotonUnconvertedOffset20",m_PhotonUnconvertedOffset20, "Offset for etcone20");
  declareProperty("PhotonUnconvertedOffset25",m_PhotonUnconvertedOffset25, "Offset for etcone25");
  declareProperty("PhotonUnconvertedOffset30",m_PhotonUnconvertedOffset30, "Offset for etcone30");
  declareProperty("PhotonUnconvertedOffset35",m_PhotonUnconvertedOffset35, "Offset for etcone35");
  declareProperty("PhotonUnconvertedOffset40",m_PhotonUnconvertedOffset40, "Offset for etcone40");
  declareProperty("PhotonUnconvertedOffsettp40",m_PhotonUnconvertedOffsettp40, "Offset for etconetp40");

}

// ====================================================================
// DESTRUCTOR:
egammaIsoPtCorrection::~egammaIsoPtCorrection()
{ 
}

// ====================================================================
// INITIALIZE METHOD:
StatusCode egammaIsoPtCorrection::initialize()
{
  ATH_MSG_DEBUG(" Initializing egammaIsoPtCorrection");
  return StatusCode::SUCCESS;
}

// ====================================================================
// FINALIZE METHOD:
StatusCode egammaIsoPtCorrection::finalize()
{
  ATH_MSG_DEBUG(" Finalizing egammaIsoPtCorrection");
  return StatusCode::SUCCESS;
}

// ====================================================================
double egammaIsoPtCorrection::CalcPtCorrection(const xAOD::Egamma* eg, double dR, std::string cltype ) {

  std::pair<double,double> correction;
  m_type = cltype;
   
  const xAOD::CaloCluster* cl = eg->caloCluster();
 
  const double eta2 = fabs(cl->etaBE(2));
  int eta_bin = GetEtaBin(eta2);
  if(eta_bin >= (int)m_EtaBins.size()) return 0.;
  
  // in case people pass in .1 instead of 10
   if(dR < 1) dR *= 100;
   
   // make it an integer
   int dR_int = (int)dR;
   
   // get correction for a given dR
   if(xAOD::EgammaHelpers::isElectron(eg)) {
     getElectronCorrection(dR_int, eta_bin, correction);
   } else if (xAOD::EgammaHelpers::isConvertedPhoton(eg)) {
     getPhotonConvertedCorrection(dR_int, eta_bin, correction);
   } else {
     getPhotonUnconvertedCorrection(dR_int, eta_bin, correction);
   }
   
   // apply the correction to et.
   double et = 0.;
   const double etaForPt =  cl->eta();
   if (fabs(etaForPt) < 999.)
     et = cosh(etaForPt)!=0. ? cl->e()/cosh(etaForPt) : 0.;
   
   // corrected et : (et*slope) + offset 
   return (correction.first*et)+(correction.second);
   
}

// =======================================================
int egammaIsoPtCorrection::GetEtaBin(double eta_w) const {
  
  double eta2 = fabs(eta_w);


  int ibin_eta = -1;

  for (unsigned int ibinEta=0;ibinEta<m_EtaBins.size()-1;ibinEta++) {
    if( (eta2 >= m_EtaBins[ibinEta]) && (eta2 < m_EtaBins[ibinEta+1]) ) ibin_eta = ibinEta;
  }

  return ibin_eta;
}

// =======================================================
void egammaIsoPtCorrection::getPhotonConvertedCorrection(int dR, int indx, std::pair<double,double> &correction) const {

 int nsize = m_PhotonConvertedPtCorrection15.size()-1;
 
 if ( (indx < 0) || (indx > nsize) ) {
     ATH_MSG_DEBUG("egammaIsoPtCorrection::getPhotonConvertedCorrection indx is out of eta range:  "<< indx);
     return;
 }
 
 switch ( dR ) {
  case 15 :
    correction = std::make_pair(m_PhotonConvertedPtCorrection15[indx], m_PhotonConvertedOffset15[indx]);
      break;
  case 20: 
      correction = std::make_pair(m_PhotonConvertedPtCorrection20[indx], m_PhotonConvertedOffset20[indx]);
      break;
  case 25: 
      correction = std::make_pair(m_PhotonConvertedPtCorrection25[indx], m_PhotonConvertedOffset25[indx]);
      break;
  case 30:
      correction = std::make_pair(m_PhotonConvertedPtCorrection30[indx], m_PhotonConvertedOffset35[indx]);
      break;
  case 40:
      correction = std::make_pair(m_PhotonConvertedPtCorrection40[indx], m_PhotonConvertedOffset40[indx]);
      if (m_type=="topo") correction = std::make_pair(m_PhotonConvertedPtCorrectiontp40[indx], m_PhotonConvertedOffsettp40[indx]);
      break;
  default:
      ATH_MSG_DEBUG("egammaIsoPtCorrection: dR is out of range to get photon correction: "<<dR);
      break;
 } 


}

// =======================================================
void egammaIsoPtCorrection::getPhotonUnconvertedCorrection(int dR, int indx, std::pair<double,double> &correction) const {

 int nsize = m_PhotonUnconvertedPtCorrection15.size()-1;

 if ( (indx < 0) || (indx > nsize) ) {
    ATH_MSG_DEBUG("egammaIsoPtCorrection::getPhotonUnconvertedCorrection indx is out of eta range:  "<< indx);
    return;
 }
 
 switch ( dR ) {
  case 15 :
      correction = std::make_pair(m_PhotonUnconvertedPtCorrection15[indx], m_PhotonUnconvertedOffset15[indx]);
      break;
  case 20: 
      correction = std::make_pair(m_PhotonUnconvertedPtCorrection20[indx], m_PhotonUnconvertedOffset20[indx]);
      break;
  case 25: 
      correction = std::make_pair(m_PhotonUnconvertedPtCorrection25[indx], m_PhotonUnconvertedOffset25[indx]);
      break;
  case 30:
      correction = std::make_pair(m_PhotonUnconvertedPtCorrection30[indx], m_PhotonUnconvertedOffset30[indx]);
      break;
  case 35:
      correction = std::make_pair(m_PhotonUnconvertedPtCorrection35[indx], m_PhotonUnconvertedOffset35[indx]);
      break;
  case 40:
      correction = std::make_pair(m_PhotonUnconvertedPtCorrection40[indx], m_PhotonUnconvertedOffset40[indx]);
      if (m_type=="topo") correction = std::make_pair(m_PhotonUnconvertedPtCorrectiontp40[indx], m_PhotonUnconvertedOffsettp40[indx]);
      break;
  default:
      ATH_MSG_DEBUG("egammaIsoPtCorrection: dR is out of range to get photon correction: "<<dR);
      break;
 } 


}

// =======================================================
void egammaIsoPtCorrection::getElectronCorrection(int dR, int indx, std::pair<double,double> &correction) const {

 int nsize = m_ElectronPtCorrection15.size()-1;

 if ( (indx < 0) || (indx > nsize) ) {
    ATH_MSG_DEBUG("egammaIsoPtCorrection::getElectronCorrection indx is out of eta range:  "<< indx);   
    return;
 }

 switch ( dR ) {
  case 15 :
      correction = std::make_pair(m_ElectronPtCorrection15[indx], m_ElectronOffset15[indx]);
      break;
  case 20: 
      correction = std::make_pair(m_ElectronPtCorrection20[indx], m_ElectronOffset20[indx]);
      break;
  case 25:  
      correction = std::make_pair(m_ElectronPtCorrection25[indx], m_ElectronOffset25[indx]);
      break;
  case 30:
      correction = std::make_pair(m_ElectronPtCorrection30[indx], m_ElectronOffset30[indx]);
      break;
  case 35:
      correction = std::make_pair(m_ElectronPtCorrection35[indx], m_ElectronOffset35[indx]);
      break;
  case 40:
      correction = std::make_pair(m_ElectronPtCorrection40[indx], m_ElectronOffset40[indx]);
      if (m_type=="topo") correction = std::make_pair(m_ElectronPtCorrectiontp40[indx], m_ElectronOffsettp40[indx]);
      break;
  default:
      ATH_MSG_DEBUG("egammaIsoPtCorrection: dR is out of range to get electron correction: "<<dR);
      break;
 }  

  
} 
