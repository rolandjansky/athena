/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**********************************************************************
 * AsgTool: TrigEgammaL2CaloSelectorTool
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Joao Victor Pinto    <jodafons@cern.ch>
 * Date: Feb 2015
 * Description:
 *
 **********************************************************************/
#include "TrigEgammaEmulationTool/TrigEgammaL2CaloSelectorTool.h"
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "boost/algorithm/string.hpp"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace Trig;
//**********************************************************************

TrigEgammaL2CaloSelectorTool::
TrigEgammaL2CaloSelectorTool( const std::string& myname )
    : TrigEgammaSelectorBaseTool(myname)
{
  // Tight, Medium and Loose pidmap from python config
  declareProperty("HADETthr",       m_hadeTthr        );
  declareProperty("CARCOREthr",     m_carcorethr      );
  declareProperty("CAERATIOthr",    m_caeratiothr     );
  declareProperty("EtaBins",        m_etabin          );
  declareProperty("dETACLUSTERthr", m_detacluster=0.1 );
  declareProperty("dPHICLUSTERthr", m_dphicluster=0.1 );
  declareProperty("ETthr",          m_eTthr=999.0     );
  declareProperty("ET2thr",         m_eT2thr=999.0    );
  declareProperty("HADET2thr",      m_hadeT2thr=999.0 );
  declareProperty("F1thr",          m_F1thr=0.005     ); 
  declareProperty("WETA2thr",       m_WETA2thr=99999. );
  declareProperty("WSTOTthr",       m_WSTOTthr=99999. );
  declareProperty("F3thr",          m_F3thr=99999.    );

  m_emTauRois=nullptr;
}
//**********************************************************************
StatusCode TrigEgammaL2CaloSelectorTool::initialize() {

  StatusCode sc = TrigEgammaSelectorBaseTool::initialize();
  if(sc.isFailure()){
    ATH_MSG_WARNING("TrigEgammaSelectorBaseTool::initialize() failed");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//!==========================================================================
StatusCode TrigEgammaL2CaloSelectorTool::finalize() {
    return StatusCode::SUCCESS;
}
//!==========================================================================
bool TrigEgammaL2CaloSelectorTool::emulation(const xAOD::TrigEMCluster *emCluster,  bool &pass, const Trig::Info &info)
{
  pass=false;
  ATH_MSG_DEBUG("Emulation L2 Calo step...");
  //add protections
  if(!emCluster){
    ATH_MSG_DEBUG("TrigEMCluster is nullptr");
    return false;
  }

  m_emTauRois=nullptr; 
  if ((m_storeGate->retrieve(m_emTauRois,"LVL1EmTauRoIs")).isFailure() ) {
      ATH_MSG_ERROR("Failed to retrieve LVL1EmTauRoIs ");
      return false;
  }

  setEtThr( (info.thrHLT - 3) );
  if(!emulationL2(emCluster, pass))  return false;
  
  ATH_MSG_DEBUG("trigger: " << info.trigName << " decision: " << int(pass));
  return true;
}

//!==========================================================================
bool TrigEgammaL2CaloSelectorTool::emulationL2(const xAOD::TrigEMCluster *pClus,  bool &pass)
{
  pass=false;
  const xAOD::EmTauRoI *pEmTauRoi=nullptr;
 
  for(const auto& emTauRoi : *m_emTauRois){
    //match by roiword between L1 and L2Calo
    if(emTauRoi->roiWord() == pClus->RoIword()){
      pEmTauRoi=emTauRoi;
      break;    
    }
  }
 
  if(!pEmTauRoi){
    ATH_MSG_DEBUG("EmTauRoI is nullptr");
    return false;
  }

  // initialise monitoring variables for each event
  float dPhi          = -1.0;
  //float dEta          = -1.0;
  float eT_T2Calo     = -1.0;
  float hadET_T2Calo  = -1.0;
  float rCore         = -1.0;
  float energyRatio   = -1.0;
  float Weta2         = -1.0;
  float Wstot         = -1.0;
  float F3            = -1.0;
  float F1            = -1.0;
  int   PassedCuts    = -1;


  // fill local variables for RoI reference position
  double etaRef = pEmTauRoi->eta();
  double phiRef = pEmTauRoi->phi();
  // correct phi the to right range (probably not needed anymore)   
  if ( fabs(phiRef) > M_PI ) phiRef -= 2*M_PI; // correct phi if outside range 
  PassedCuts++; //Input

  if ( !pClus ) {
    ATH_MSG_WARNING("No EM cluster in RoI");
    return true;
  }

  // increment event counter
  PassedCuts++; //// the ROI at least contais the cluster

  float absEta = fabs( pClus->eta() );
  int etaBin = -1;
  //double monEta = pClus->eta();
  //double monPhi = pClus->phi();
  for (std::size_t iBin = 0; iBin < m_etabin.size()-1; iBin++)
    if ( absEta > m_etabin[iBin] && absEta < m_etabin[iBin+1] ) etaBin = iBin;
 
  // find if electron is in calorimeter crack
  bool inCrack = ( absEta > 2.37 || ( absEta > 1.37 && absEta < 1.52) );
 
  //dEta =  pClus->eta() - etaRef;
  // Deal with angle diferences greater than Pi
  dPhi =  fabs(pClus->phi() - phiRef);
  dPhi = (dPhi < M_PI ? dPhi : 2*M_PI - dPhi );

  // calculate cluster quantities // definition taken from TrigElectron constructor     
  if ( pClus->emaxs1() + pClus->e2tsts1() > 0 )
    energyRatio = ( pClus->emaxs1() - pClus->e2tsts1() ) / ( pClus->emaxs1() + pClus->e2tsts1() );

  // (VD) here the definition is a bit different to account for the cut of e277 @ EF
  if ( pClus->e277()!= 0.) rCore = pClus->e237() / pClus->e277();

  //fraction of energy deposited in 1st sampling
  if ( fabs(pClus->energy()) > 0.00001) F1 = (pClus->energy(CaloSampling::EMB1)+pClus->energy(CaloSampling::EME1))/pClus->energy();
  eT_T2Calo  = pClus->et();
 
  if ( eT_T2Calo!=0 && pClus->eta()!=0 ) hadET_T2Calo = pClus->ehad1()/cosh(fabs(pClus->eta()))/eT_T2Calo;
 
  //extract Weta2 varable
  Weta2 = pClus->weta2();
  //extract Wstot varable
  Wstot = pClus->wstot();

  //extract F3 (backenergy i EM calorimeter
  float e0 = pClus->energy(CaloSampling::PreSamplerB) + pClus->energy(CaloSampling::PreSamplerE);
  float e1 = pClus->energy(CaloSampling::EMB1) + pClus->energy(CaloSampling::EME1);
  float e2 = pClus->energy(CaloSampling::EMB2) + pClus->energy(CaloSampling::EME2);
  float e3 = pClus->energy(CaloSampling::EMB3) + pClus->energy(CaloSampling::EME3);
  float eallsamples = e0+e1+e2+e3;
  F3= fabs(eallsamples)>0. ? e3/eallsamples : 0.;

  // apply cuts: DeltaEta(clus-ROI)
  if ( fabs(pClus->eta() - etaRef) > m_detacluster ) return true;
  PassedCuts++; //Deta
 
  // DeltaPhi(clus-ROI)
  if( dPhi > m_dphicluster ) return true;
  PassedCuts++; //DPhi

  // eta range
  if ( etaBin==-1 ) {  // VD
    ATH_MSG_DEBUG("Cluster eta: " << absEta << " outside eta range " << m_etabin[m_etabin.size()-1] );
    return true;
  } else {
    ATH_MSG_DEBUG("eta bin used for cuts " << etaBin);
  }
  PassedCuts++; // passed eta cut
  // Rcore
  if ( rCore < m_carcorethr[etaBin] )  return true;
  PassedCuts++; //Rcore
  // Eratio
  if ( inCrack || F1<m_F1thr ) {
    ATH_MSG_DEBUG("TrigEMCluster: InCrack= " << inCrack << " F1=" );
  } else {
    if ( energyRatio < m_caeratiothr[etaBin] ) return true;
  }
  PassedCuts++; //Eratio
  if(inCrack) energyRatio = -1; //Set default value in crack for monitoring.
 
  // ET_em
  ATH_MSG_DEBUG("Et_T2Calo = " << eT_T2Calo*1e-3 << " < " << m_eTthr << " GeV");
  if ( eT_T2Calo*1e-3 < m_eTthr) return true;
  PassedCuts++; // ET_em
 
  float hadET_cut = 0.0; 
  // find which ET_had to apply : this depends on the ET_em and the eta bin
  if ( eT_T2Calo >  m_eT2thr ) {
    hadET_cut = m_hadeT2thr ;
  } else {
    hadET_cut = m_hadeTthr[etaBin];
  }
  // ET_had
  if ( hadET_T2Calo > hadET_cut ) return true;
  PassedCuts++; //ET_had
  // F1
  // if ( F1 < m_F1thr[0]) return true;  //(VD) not cutting on this variable, only used to select whether to cut or not on eRatio
  PassedCuts++; //F1
  //Weta2
  if ( Weta2 > m_WETA2thr) return true;
  PassedCuts++; //Weta2
  //Wstot
  if ( Wstot >= m_WSTOTthr) return true;
  PassedCuts++; //Wstot
  //F3
  if ( F3 > m_F3thr) return true;
  PassedCuts++; //F3
  // got this far => passed!
  pass = true;
  return true;
}
//!==========================================================================

