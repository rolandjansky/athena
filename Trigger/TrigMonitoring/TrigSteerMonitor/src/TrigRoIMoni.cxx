/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <bitset>
#include <algorithm>
#include <TH1F.h>
#include <TH2F.h>

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ITHistSvc.h"

#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/LvlConverter.h"
#include "TrigSteering/TrigSteer.h"

#include "TrigInterfaces/AlgoConfig.h"

#include "TrigSteering/SteeringChain.h"
#include "TrigSteering/Sequence.h"
#include "TrigSteering/Signature.h"
#include "TrigNavigation/TriggerElement.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigT1Result/RoIBResult.h"

#include "TrigNavigation/Navigation.h"
#include "TrigNavigation/Navigation.icc"
#include "TrigNavigation/NavigationCore.icc"
#include "TrigNavigation/TriggerElement.h"

#include "TrigConfHLTData/HLTUtils.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigSteerMonitor/TrigRoIMoni.h"

#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/RecJetRoI.h"

using namespace std;
using namespace HLT;

#include <iostream>

#include "TrigConfHLTData/HLTSignature.h"
#include <boost/regex.hpp>
#include <cmath>


TrigRoIMoni::TrigRoIMoni(const std::string & type, const std::string & name,
                         const IInterface* parent)
  :  TrigMonitorToolBase(type, name, parent),
     m_trigConfigSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
     m_gotL1Config(false),
     m_lvl1Tool("HLT::Lvl1ResultAccessTool/Lvl1ResultAccessTool",this),
     m_trigLvl(""),
     m_threshMultiMax(100)
   
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("HistoPath", m_histoPathexpert = "/EXPERT/TrigSteering");
  declareProperty("ThreshMultiMax", m_threshMultiMax = 100, "Upper y-boundary of threshold multi. hist.");

}


/* ******************************** initialize ************************************** */
StatusCode TrigRoIMoni::initialize()
{
   ATH_CHECK(TrigMonitorToolBase::initialize());

   m_parentAlg = dynamic_cast<const HLT::TrigSteer*>(parent());
   if ( !m_parentAlg ) {
     ATH_MSG_ERROR("Unable to cast the parent algorithm to HLT::TrigSteer !");
     return StatusCode::FAILURE;
   }

   m_trigLvl = m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::L2 ? "L2" : m_parentAlg->getAlgoConfig()->getHLTLevel() == HLT::EF ? "EF" : "HLT" ;

   // Get the trigger configuration service
   StatusCode sc = m_trigConfigSvc.retrieve();
   m_gotL1Config = true;
   if ( sc.isFailure() ) {
     ATH_MSG_ERROR("Couldn't connect to " << m_trigConfigSvc.typeAndName());
     m_gotL1Config = false;
   } 
  
   m_lvl1Tool->updateConfig(true, true, true).ignore();

   // Get the muon RPC RecRoI service
   ATH_CHECK(service(LVL1::ID_RecRpcRoiSvc, m_recRPCRoiSvc));

   // Get the muon TGC RecRoI service
   ATH_CHECK(service(LVL1::ID_RecTgcRoiSvc, m_recTGCRoiSvc));

   return StatusCode::SUCCESS;
}


/* ******************************** bookHists  ************************************** */

StatusCode TrigRoIMoni::bookHists()
{
  ATH_CHECK(bookHistograms( false, false, true ));
  return StatusCode::SUCCESS;
}

// Helper function for booking threshold multiplicity histogram
static void InsertThresholdNames( const std::vector<TrigConf::TriggerThreshold*> & thVector, 
                                  std::map<std::string, std::pair<int, unsigned int> > &thNames, 
                                  int &thresholdBin)
{
   // MM, 1.8.2011: first put names into an intermediate map keyed by name
   //               for alphabetical ordering then into the map

   std::map<std::string, unsigned int> thNameSet;
   for( TrigConf::TriggerThreshold* thr : thVector ) {
      
      if(thr==0) continue;

      const std::string &name = thr->name();

      if(thNames.find(name) != thNames.end()) 
         continue;

      if(thNameSet.find(name) != thNameSet.end()) 
         continue;

      uint32_t id = TrigConf::HLTUtils::string2hash( name );
      thNameSet[name] = id;

   }


   for(std::map<std::string, unsigned int> ::const_iterator thIt = thNameSet.begin();
       thIt != thNameSet.end(); thIt++, thresholdBin++) {
      thNames[ thIt->first ] = std::pair<int, unsigned int>(thresholdBin, thIt->second );
   }
  
}

/* ******************************** book Histograms ************************************* */
StatusCode TrigRoIMoni::bookHistograms( bool/* isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool /*isNewRun*/ )
{
   TrigMonGroup expertHistograms( this, m_parentAlg->name(), expert );

   // roi & threshold multiplicity hists
   if (m_trigLvl == "L2" || m_trigLvl == "HLT") { 

     m_etaphi_EM_all = new 
       TrigLBNHist<TH2I>(TH2I("AllRoIsEtaPhiEM", 
                              ("phi vs eta for all L1 EM RoIs "+m_trigLvl).c_str(), 
                              51, -2.55, 2.55,
                              64,  -M_PI*(1.-1./64.), M_PI*(1.+1./64.)));

     if ( expertHistograms.regHist((ITrigLBNHist*)m_etaphi_EM_all).isFailure()) {
       ATH_MSG_WARNING("Cannot register " << m_etaphi_EM_all->GetName());
     }
     m_etaphi_J_all = new  
       TrigLBNHist<TH2I>(TH2I("AllRoIsEtaPhiJ", 
                              ("phi vs eta for all L1 jet RoIs "+m_trigLvl).c_str(), 
                              31, -3.1, 3.1,
                              32,  -M_PI*(1.-1./32.), M_PI*(1.+1./32.)));
      
     if ( expertHistograms.regHist((ITrigLBNHist*)m_etaphi_J_all).isFailure()) {
       ATH_MSG_WARNING("Cannot register " << m_etaphi_J_all->GetName());
     }
      
     m_etaphi_JF_all = new 
       TrigLBNHist<TH2I>(TH2I("AllRoIsEtaPhiJF", 
                              ("phi vs eta for all L1 forward jet RoIs "+m_trigLvl).c_str(),
                              2, -5., 5.,
                              32,  -M_PI*(1.-1./32.), M_PI*(1.+1./32.)));

     m_etaphi_JF_all->GetXaxis()->SetBinLabel(1, "eta < -3.2");
     m_etaphi_JF_all->GetXaxis()->SetBinLabel(2, "eta > 3.2");
     if ( expertHistograms.regHist((ITrigLBNHist*)m_etaphi_JF_all).isFailure()) {
       ATH_MSG_WARNING("Cannot register " << m_etaphi_JF_all->GetName());
     }
      
     m_etaphi_HA_all = new     
       TrigLBNHist<TH2I>(TH2I("AllRoIsEtaPhiHA", 
                              ("phi vs eta for all L1 tau RoIs "+m_trigLvl).c_str(), 51, -2.55, 2.55,
                              64,  -M_PI*(1.-1./64.), M_PI*(1.+1./64.)));
            
     if ( expertHistograms.regHist((ITrigLBNHist*)m_etaphi_HA_all).isFailure()) {
       ATH_MSG_WARNING("Cannot register " << m_etaphi_HA_all->GetName());
     }
      
     m_etaphi_MU_all = new     
       TrigLBNHist<TH2I>(TH2I("AllRoIsEtaPhiMU", 
                              ("phi vs eta for all L1 MU RoIs "+m_trigLvl).c_str(), 
                              50, -2.5, 2.5,
                              64,  -M_PI, M_PI));

     if ( expertHistograms.regHist((ITrigLBNHist*)m_etaphi_MU_all).isFailure()) {
       ATH_MSG_WARNING("Cannot register " << m_etaphi_MU_all->GetName());
     }

     m_histoverflow = SetupOverflowHist(); 
     if ( m_histoverflow == 0 || expertHistograms.regHist(m_histoverflow).isFailure()) {
       ATH_MSG_WARNING("Either can't book or can't register RoIsOverflow");
     } 
      
     m_histLinkMultiplicity = SetupLinkMultiplicityHist(); 
     if ( m_histLinkMultiplicity == 0 || expertHistograms.regHist(m_histLinkMultiplicity).isFailure()) {
       ATH_MSG_WARNING("Either can't book or can't register RoIsLinkMultiplicity");
     }

     // Get a list of all thresholds for the threshold multiplicity histogram
     m_histThresholdMultiplicity = 0;
     if( m_gotL1Config ) {
        
       const TrigConf::ThresholdConfig* thresholds = m_trigConfigSvc->thresholdConfig();

       int thresholdBin = 0;
       m_thresholdNames.clear();

       InsertThresholdNames(thresholds->getClusterThresholdVector(), m_thresholdNames, thresholdBin);
       InsertThresholdNames(thresholds->getJetThresholdVector(), m_thresholdNames, thresholdBin);
       InsertThresholdNames(thresholds->getFJetThresholdVector(), m_thresholdNames, thresholdBin);
       InsertThresholdNames(thresholds->getJbThresholdVector(), m_thresholdNames, thresholdBin);
       InsertThresholdNames(thresholds->getJfThresholdVector(), m_thresholdNames, thresholdBin);
       InsertThresholdNames(thresholds->getTotEtVector(), m_thresholdNames, thresholdBin);
       InsertThresholdNames(thresholds->getJetEtVector(), m_thresholdNames, thresholdBin);
       InsertThresholdNames(thresholds->getMissEtVector(), m_thresholdNames, thresholdBin);
       InsertThresholdNames(thresholds->getMuonThresholdVector(), m_thresholdNames, thresholdBin);

       /*
         for(std::map<std::string, std::pair<int, unsigned int> >::const_iterator nameIt = 
         m_thresholdNames.begin();
         nameIt != m_thresholdNames.end(); nameIt++) {
         std::cout << "MMMM Threshold: " << nameIt->first << ": " << nameIt->second.first  
         << ", id: " << nameIt->second.second << std::endl;
         }
       */

       if(m_thresholdNames.size() > 0) {

         m_histThresholdMultiplicity =  new TH2I( "NumOfL1Thresholds", 
                                                  "Number of LVL1 thresholds per events",
                                                  m_thresholdNames.size(), 0, m_thresholdNames.size(),
                                                  m_threshMultiMax, 0, m_threshMultiMax);
  
         if ( expertHistograms.regHist(m_histThresholdMultiplicity).isFailure()) {
           ATH_MSG_WARNING("Cannot register " << m_histThresholdMultiplicity->GetName());

           delete m_histThresholdMultiplicity;
           m_histThresholdMultiplicity = 0;
         } else {
           TAxis *xax = m_histThresholdMultiplicity->GetXaxis();
           for(std::map<std::string, std::pair<int, unsigned int> >::const_iterator nIt = 
                 m_thresholdNames.begin(); nIt != m_thresholdNames.end(); nIt++) {

             xax->SetBinLabel(nIt->second.first + 1, nIt->first.c_str());
           }
         }
       }
     }
   }

   float detamin=-0.7,detamax=0.7;
   float dphimin=-1,dphimax=1;
   //MM: phi limits changed 1.5.12; the limits are shifted (as above) since ecal gives quantized numbers
   float phimin=-M_PI*(1.-1./64.),phimax=M_PI*(1.+1./64.); 
  
   float etamin=-5.,etamax=5.;
  
   std::string tmpstring("N Initial RoI in Event ");
   tmpstring+=m_trigLvl;
   TString htit(Form(tmpstring.c_str()));

   m_histonroi =  new TH1F( "NInitialRoIsPerEvent",htit.Data(),50,-0.5,49.5);
  
   if ( expertHistograms.regHist(m_histonroi).isFailure())
     ATH_MSG_WARNING("Cannot register " << m_histoPathexpert << m_histonroi ->GetName());
             
   tmpstring="Updates of RoI positions with respect to L1 (phi) ";
   tmpstring+=m_trigLvl;
   htit=Form(tmpstring.c_str());

   m_histodeta =  new TH1F( "RoIsDEta",htit.Data(),100,detamin,detamax);
 
   if ( expertHistograms.regHist(m_histodeta).isFailure())
     ATH_MSG_WARNING("Cannot register " << m_histoPathexpert << m_histodeta ->GetName());
  
   tmpstring="Updates of RoI positions with respect to L1 (eta) ";
   tmpstring+=m_trigLvl;
   htit=Form(tmpstring.c_str());

   m_histodphi =  new TH1F( "RoIsDPhi",htit.Data(), 32,dphimin,dphimax);
  
   if ( expertHistograms.regHist(m_histodphi).isFailure())
     ATH_MSG_WARNING("Cannot register " << m_histoPathexpert << m_histodphi->GetName());
  
   tmpstring="L1 RoIs eta ";
   tmpstring+=m_trigLvl;
   htit=Form(tmpstring.c_str());

   m_histoeta0 =  new TH1F( "RoIsL1Eta",htit.Data(), 25,etamin,etamax);
  
   if ( expertHistograms.regHist(m_histoeta0).isFailure())
     ATH_MSG_WARNING("Cannot register " << m_histoPathexpert << m_histoeta0->GetName());
  
   tmpstring="L1 RoIs phi ";
   tmpstring+=m_trigLvl;
   htit=Form(tmpstring.c_str());
   m_histophi0 =  new TH1F( "RoIsL1Phi",htit.Data(), 32,phimin,phimax);
  
   if ( expertHistograms.regHist(m_histophi0).isFailure())
     ATH_MSG_WARNING("Cannot register " << m_histoPathexpert << m_histophi0->GetName());
  
   tmpstring="L1 RoIs phi vs eta ";
   tmpstring+=m_trigLvl;
   htit=Form(tmpstring.c_str());
   m_histo_eta0_phi0 =  new TH2F( "RoIsL1PhiEta", htit.Data(), 25,etamin,etamax, 32,phimin,phimax);
  
   if ( expertHistograms.regHist(m_histo_eta0_phi0).isFailure())
     ATH_MSG_WARNING("Cannot register " << m_histoPathexpert << m_histophi0->GetName());
  
   return StatusCode::SUCCESS;
}

/* ******************************** fill Histograms ************************************** */
StatusCode TrigRoIMoni::fillHists()
{
  
  
  if( !m_histonroi ||  !m_histodeta || !m_histodphi || !m_histoeta0 
      || !m_histophi0 || !m_histo_eta0_phi0 ){
    ATH_MSG_WARNING("pointers to histograms not ok, dont Fill !");
    return StatusCode::FAILURE;  
  }
  
  if (m_trigLvl == "L2" || m_trigLvl == "HLT") {
    FillT1ResultHists();

    // Fill the threshold counts hist
    if( m_histThresholdMultiplicity != 0) {
      HLT::Navigation *nav = m_parentAlg->getAlgoConfig()->getNavigation();

      for(std::map<std::string, std::pair<int, unsigned int> >::const_iterator nameIt = 
            m_thresholdNames.begin();
          nameIt != m_thresholdNames.end(); nameIt++) {

        unsigned found = nav->countAllOfType(nameIt->second.second, false);
        m_histThresholdMultiplicity->Fill(nameIt->second.first, found);
      }
    }
  }

  FillEtaPhiPlots();
  
  /*
    The rest of the code in this function fills the deta and dphi plots. All of the other
    plots are now filled in FillEtaPhiPlots.
  */

  // get DirectSuccessors of Initial Node from Navigation   (= Initial RoIs)
  const std::vector<HLT::TriggerElement*> rois = m_parentAlg->getAlgoConfig()->getNavigation()->
    getDirectSuccessors(m_parentAlg->getAlgoConfig()->getNavigation()->getInitialNode());
  std::vector<HLT::TriggerElement*> tes_step2; //TriggerElements one step down?
  
  
  //loop over these direct successors = Initial RoIs on Trigger Level (!= Inital Node)
  int nte=0;
  
  std::vector<HLT::TriggerElement*>::const_iterator roiIt;
  
  for ( roiIt = rois.begin(); roiIt != rois.end(); ++ roiIt) {   
    
    if(*roiIt == 0)
      continue;
    
    nte++;
    // get Label of these Intital RoIs
    std::string label; 
    TrigConf::HLTTriggerElement::getLabel ((*roiIt)->getId(),label);

    
    /*
      
    now get Features of inital RoI 
    
    */
    std::vector<const TrigRoiDescriptor*> roiDescriptorofDirectSuccessor;   
    float eta0=-99.,phi0=-99.;
    //getFeatures of this Initial RoI and loop it(them)
    int niniroi=0;
    
    if( m_parentAlg->getAlgoConfig()->getNavigation()->getFeatures((*roiIt), roiDescriptorofDirectSuccessor, "initialRoI")){
      
      std::vector<const TrigRoiDescriptor*>::const_iterator roiDescriptorIt;
      
      for(roiDescriptorIt= roiDescriptorofDirectSuccessor.begin(); 
          roiDescriptorIt != roiDescriptorofDirectSuccessor.end(); ++ roiDescriptorIt){
        
        if(*roiDescriptorIt == 0) continue;
        
        niniroi++;
        
        //store eta0, phi0
        eta0=(*roiDescriptorIt)->eta();
        phi0=(*roiDescriptorIt)->phi();
        
      }
    }
    
    /*
      now get all Features seeded by one inital Node , loop these RoIs  and plot delta eta and phi
    */
    
    float eta=-99.,phi=-99.,deltaeta=-99.,deltaphi=-99.; //with respect to seeding RoIs eta0,phi0 
    //call GetFeatures in RoI
    std::vector<const TrigRoiDescriptor*> roiDescriptor;      
    
    bool stroi = m_parentAlg->getAlgoConfig()->getNavigation()->
      getFeaturesInRoI((*roiIt), roiDescriptor); //,0,&labels);
    
    if( stroi ){
      ATH_MSG_DEBUG(nte << ". TE " << label << "  found features "<<roiDescriptor.size());
      
      std::vector<const TrigRoiDescriptor*>::const_iterator roiDescriptorIt;
      //loop rois
      for(roiDescriptorIt= roiDescriptor.begin(); roiDescriptorIt != roiDescriptor.end(); ++ roiDescriptorIt){
        if(*roiDescriptorIt == 0) continue;
        
        eta=(*roiDescriptorIt)->eta();
        phi=(*roiDescriptorIt)->phi();
        deltaeta=eta0-eta;
        deltaphi=phi0-phi;
        ATH_MSG_DEBUG("found RoIInfo for  "<< // labels[ (*roiDescriptorIt )]<<
                      " roiWord "<<(*roiDescriptorIt)->roiWord()<<
                      " phi,eta "<<phi<<","<<eta<<"  Delta "<<deltaphi<<","<<deltaeta);
        
        //fill histos  m_histodeta and m_histodphi
        m_histodeta->Fill(deltaeta);
        m_histodphi->Fill(deltaphi);
      }
    }else{
      ATH_MSG_DEBUG("for "<<nte<<". TE "<<label<<" no features found");
    }
  }//loop rois
  
  return StatusCode::SUCCESS;
}

TH1F *TrigRoIMoni::SetupOverflowHist()
{
  TH1F *hist = new TH1F("RoIsOverflow","L1 overflow conditions", 16, 0., 16.);
  if(hist == 0)
    return 0;

  TAxis *xax = hist->GetXaxis();
  xax->SetBinLabel(1,  "any link");
  xax->SetBinLabel(2,  "any CP link");
  xax->SetBinLabel(3,  "any JE link");
  xax->SetBinLabel(4,  "Mu link");
  xax->SetBinLabel(5,  "CP link 1");
  xax->SetBinLabel(6,  "CP link 2");
  xax->SetBinLabel(7,  "CP link 3");
  xax->SetBinLabel(8,  "CP link 4");
  xax->SetBinLabel(9,  "JE link 1");
  xax->SetBinLabel(10, "JE link 2");
  xax->SetBinLabel(11, "Mu SUP1");
  xax->SetBinLabel(12, "Mu SUP2");
  xax->SetBinLabel(13, "MU Sorter");
  xax->SetBinLabel(14, "CPM/EM to CMX");
  xax->SetBinLabel(15, "CPM/TAU to CMX");
  xax->SetBinLabel(16, "JEM to CMX");

  return hist;
}

TH2F *TrigRoIMoni::SetupLinkMultiplicityHist()
{
  TH2F *hist = new TH2F("RoIsLinkMultiplicity","RoI multiplicity on L1 links", 7, 0., 7., 50, 0., 50.);
  if(hist == 0)
    return 0;

  TAxis *xax = hist->GetXaxis();
  xax->SetBinLabel(1,  "Mu link");
  xax->SetBinLabel(2,  "CP link 1");
  xax->SetBinLabel(3,  "CP link 2");
  xax->SetBinLabel(4,  "CP link 3");
  xax->SetBinLabel(5,  "CP link 4");
  xax->SetBinLabel(6,  "JE link 1");
  xax->SetBinLabel(7,  "JE link 2");

  return hist;
}

void TrigRoIMoni::FillT1ResultHists()
{
  const ROIB::RoIBResult* result;
  if (evtStore()->retrieve(result).isFailure()) {
    ATH_MSG_WARNING("Unable to retrieve RoIBResult from storeGate!");
    return;
  }  
  FillOverflowHist(result);
  FillLinkMultiplicityHist(result);
}


void TrigRoIMoni::FillOverflowHist(const ROIB::RoIBResult* result)
{
  if( m_histoverflow == 0)
    return;


  if( result->CheckOverflow() ) { 

    m_histoverflow->Fill(0.5);

    if( result->CheckEMOverflow())
      m_histoverflow->Fill(1.5);

    if( result->CheckJOverflow())
      m_histoverflow->Fill(2.5);

    if( result->CheckMuOverflow())
      m_histoverflow->Fill(3.5);

    if( result->CheckEMLinkOverflow(0))
      m_histoverflow->Fill(4.5);

    if( result->CheckEMLinkOverflow(1))
      m_histoverflow->Fill(5.5);

    if( result->CheckEMLinkOverflow(2))
      m_histoverflow->Fill(6.5);

    if( result->CheckEMLinkOverflow(3))
      m_histoverflow->Fill(7.5);

    if( result->CheckJLinkOverflow(0))
      m_histoverflow->Fill(8.5);

    if( result->CheckJLinkOverflow(1))
      m_histoverflow->Fill(9.5);
  }  
  if( result->CheckMu1Suppression())
    m_histoverflow->Fill(10.5);
  
  /* Kludge -- for now, CheckMu2Suppression indicates sorter overflow
  if( result->CheckMu2Suppression())
    m_histoverflow->Fill(11.5);
  
  if( result->CheckMuSorterOverflow())
    m_histoverflow->Fill(12.5);
  */

  if( result->CheckMu2Suppression())
    m_histoverflow->Fill(12.5);

  std::bitset<3> overflow = m_lvl1Tool->lvl1EMTauJetOverflow(*result);
  if (overflow[0]) m_histoverflow->Fill(13.5); // em
  if (overflow[1]) m_histoverflow->Fill(14.5); // tau
  if (overflow[2]) m_histoverflow->Fill(15.5); // jet                
}

void TrigRoIMoni::FillLinkMultiplicityHist(const ROIB::RoIBResult* result)
{

  if( m_histLinkMultiplicity == 0)
    return;

  m_histLinkMultiplicity->Fill(0.5, result->muCTPIResult().roIVec().size()+0.5);

  const std::vector< ROIB::EMTauResult > &emTauRes =  result->eMTauResult();
  int link = 0;
  for(std::vector< ROIB::EMTauResult >::const_iterator item = emTauRes.begin();
      item != emTauRes.end(); item++, link++) {
    m_histLinkMultiplicity->Fill(link + 1.5, item->roIVec().size()+0.5);
    if(link == 3) break;
  }

  link = 0;
  const std::vector< ROIB::JetEnergyResult >&jeRes = result->jetEnergyResult();
  for(std::vector< ROIB::JetEnergyResult >::const_iterator item = jeRes.begin();
      item != jeRes.end(); item++, link++) {
    m_histLinkMultiplicity->Fill(link + 5.5, item->roIVec().size()+0.5);
    if(link == 1) break;
  }
}


static double fixphi(double phi) {
  if ( phi < -M_PI  || phi > M_PI ) {
    if ( phi < -M_PI )
      return phi + 2*M_PI;
    else
      return phi - 2*M_PI;
  }
  return phi;
}


/*
  Fill eta phi plots for the L1 RoIs, for L2 plot different types
  seperately.  Also the global eta/phi, eta, phi, NRoI plots (i.e. the
  one which doesn't distinguish RoI types and are filled in both L2
  and EF are also filled here.

  This function goes back to the original L1 RoI words for its
  information. This is done so that the plots will appear even if no
  chains are enabled (as foreseen for the heavy ion running).

*/

void TrigRoIMoni::FillEtaPhiPlots()
{

  bool isL2 = (m_trigLvl == "L2");
  bool isHLT = (m_trigLvl == "HLT");

  if ( (isL2 || isHLT) && (m_etaphi_EM_all== 0 ||
                           m_etaphi_J_all == 0 ||
                           m_etaphi_JF_all== 0 ||
                           m_etaphi_HA_all== 0 ||
                           m_etaphi_MU_all== 0)) {
    
    ATH_MSG_WARNING("Called FillEtaPhiPlots but >= 1 histo was not booked!");
    return;
  }


  const ROIB::RoIBResult* result;
  StatusCode sc = evtStore()->retrieve(result);
  
  if(sc.isFailure()){
    return;
  }

  int nRoIs = 0;

  // ------------------------------
  //            Muon RoIs
  // ------------------------------
  
  if(m_gotL1Config && m_recRPCRoiSvc != 0 && m_recTGCRoiSvc != 0) {
    
    const std::vector< HLT::MuonRoI >& muonRoIs = m_lvl1Tool->createMuonThresholds(*result);
    
    for (std::vector<  HLT::MuonRoI >::const_iterator muonRoI = muonRoIs.begin();
         muonRoI != muonRoIs.end(); ++muonRoI) {
      
      std::vector<TrigConf::TriggerThreshold*> muonDummy; // only need coordinates, not thresholds
      LVL1::RecMuonRoI* recRoI = new LVL1::RecMuonRoI(muonRoI->lvl1RoI().roIWord(), 
                                                      m_recRPCRoiSvc, 
                                                      m_recTGCRoiSvc, 
                                                      &muonDummy);
      double eta = recRoI->eta();
      double phi = fixphi(recRoI->phi());
      //std::cout << "MMMM new RoI, eta,phi: " << eta << ", " << phi << std::endl;

      if((isL2||isHLT) && m_etaphi_MU_all) m_etaphi_MU_all->Fill(eta, phi);

      m_histoeta0->Fill(eta);
      m_histophi0->Fill(phi);
      m_histo_eta0_phi0->Fill(eta,phi);

      nRoIs++;

      delete recRoI;
    }
  }


  // ------------------------------
  //          EMTau RoIs
  // ------------------------------
  
  
  // Get configuration (only needed for distinguishing EM from Tau)
  std::vector<TrigConf::TriggerThreshold*> caloConfig;
  
  if (m_gotL1Config) {
    std::vector<TrigConf::TriggerThreshold*> thresholds = m_trigConfigSvc->ctpConfig()->menu().thresholdVector();
    for (std::vector<TrigConf::TriggerThreshold*>::const_iterator it = thresholds.begin();
         it != thresholds.end(); ++it) {
      if ( (*it)->type() == TrigConf::L1DataDef::emType() ||
           (*it)->type() == TrigConf::L1DataDef::tauType() ) caloConfig.push_back(*it);
    }

    const std::vector< HLT::EMTauRoI >& emTauRoIs = 
      m_lvl1Tool->createEMTauThresholds(*result, false); // MM: can this safely be set to false?
    
    for (std::vector<  HLT::EMTauRoI >::const_iterator emTauRoI = emTauRoIs.begin();
         emTauRoI != emTauRoIs.end(); ++emTauRoI) {
      
      LVL1::RecEmTauRoI* recRoI = new LVL1::RecEmTauRoI(emTauRoI->lvl1RoI().roIWord(), &caloConfig);
      
      std::vector< unsigned int > *tp =  recRoI->thresholdsPassed();
      bool isEM = false, isTau = false;
      for(std::vector< unsigned int >::const_iterator ittp = tp->begin(); ittp != tp->end(); ++ittp) {
        if(recRoI->thresholdType(*ittp) == LVL1::TrigT1CaloDefs::EMAlg) isEM = true;
        if(recRoI->thresholdType(*ittp) == LVL1::TrigT1CaloDefs::TauAlg) isTau = true;
      }
      delete tp;
      
      double eta = recRoI->eta();
      double phi = fixphi(recRoI->phi());
      //std::cout << "MMMM new EMtau RoI, isEM, isTau, eta,phi: " << isEM << ", "
      //                << isTau << ", " << eta << ", " << phi << std::endl;

      if((isL2||isHLT) && isEM && m_etaphi_EM_all)  m_etaphi_EM_all->Fill(eta, phi);
      if((isL2||isHLT) && isTau && m_etaphi_HA_all) m_etaphi_HA_all->Fill(eta, phi);

      m_histoeta0->Fill(eta);
      m_histophi0->Fill(phi);
      m_histo_eta0_phi0->Fill(eta,phi);

      nRoIs++;
      delete recRoI;
    }
  }

  // ------------------------------
  //         JetEnergy RoIs
  // ------------------------------

  const std::vector< HLT::JetEnergyRoI >& jetERoIs = 
    m_lvl1Tool->createJetEnergyThresholds(*result, false); // MM: safe to set this to false?
  
  for (std::vector<  HLT::JetEnergyRoI >::const_iterator jetERoI = jetERoIs.begin();
       jetERoI != jetERoIs.end(); ++jetERoI) {
    
    if (jetERoI->type() == JetRoI || jetERoI->type() == ForwardJetRoI) {

      // Dummy configuration vector, only need coordinates, not thresholds
      std::vector<TrigConf::TriggerThreshold*> jetConfig;

      LVL1::RecJetRoI* recRoI = new LVL1::RecJetRoI(jetERoI->lvl1RoI().roIWord(), &jetConfig);
      
      double eta = recRoI->eta();
      double phi = fixphi(recRoI->phi());
      //std::cout << "MMMM new Jet RoI, eta,phi: " << eta << ", " << phi << std::endl;

      if((isL2||isHLT) && jetERoI->type() == JetRoI && m_etaphi_J_all)         m_etaphi_J_all->Fill(eta, phi);
      if((isL2||isHLT) && jetERoI->type() == ForwardJetRoI && m_etaphi_JF_all) m_etaphi_JF_all->Fill(eta, phi);

      m_histoeta0->Fill(eta);
      m_histophi0->Fill(phi);
      m_histo_eta0_phi0->Fill(eta,phi);

      nRoIs++;
      delete recRoI;
    }
  }

  //fill histo with number of (initial) RoIs in  this event 
  m_histonroi->Fill((float)nRoIs);
  

}

