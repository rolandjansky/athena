/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//      2015-05-20 Author: Bertrand LAFORGE (LPNHE Paris)
//
//      NAME:    ZeeTaPMonTool.cxx
//      PACKAGE: offline/Reconstruction/egamma/egammaPerformance
//      PURPOSE: Simple tag and probe method to check electron 
//               ID and ISO performance
//          
/////////////////////////////////////////////////////////////


#include "egammaPerformance/ZeeTaPMonTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "TH1F.h"

using CLHEP::GeV;

ZeeTaPMonTool::ZeeTaPMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  :  egammaMonToolBase(type,name,parent),
     m_hNZcandidates(nullptr),
     m_electronGroup(nullptr),
     m_electronTrkGroup(nullptr),
     m_electronIdGroup(nullptr),
     m_electronIsoGroup(nullptr),
     m_electronLBGroup(nullptr),
     m_electronEffGroup(nullptr),
     m_hMass(nullptr),
     m_hIDEt(nullptr),
     m_hIDEta(nullptr),
     m_hIDPhi(nullptr),
     m_effhIDEt(nullptr),
     m_effhIDEta(nullptr),
     m_effhIDPhi(nullptr),
     m_hISOEt(nullptr),
     m_hISOEta(nullptr),
     m_hISOPhi(nullptr),
     m_effhISOEt(nullptr),
     m_effhISOEta(nullptr),
     m_effhISOPhi(nullptr),
     m_hN(nullptr),
     m_hEt(nullptr),
     m_hEta(nullptr),
     m_hPhi(nullptr),
     m_hEtaPhi(nullptr),
     m_hLB_N(nullptr)
{
  // Name of the electron collection
  declareProperty("ElectronContainer", m_ElectronContainer = "Electrons", "Name of the electron collection" );
  declareProperty("massPeak", m_MassPeak = 91188, "Resonance peak position" );
  declareProperty("electronEtCut",m_ElectronEtCut = 15*GeV, "Et cut for electrons");
  declareProperty("massLowerCut", m_MassLowerCut = 70*GeV,"Lower mass cut");
  declareProperty("massUpperCut", m_MassUpperCut = 110*GeV,"Upper mass cut");
  declareProperty("PhiBinning", m_PhiBinning = 64,"Number of bins for phi");

  m_lumiBlockNumber = 0;
  m_nZCandidatesInCurrentLB = 0;
  m_nZCandidates = 0; 
}

ZeeTaPMonTool::~ZeeTaPMonTool()
{
}

StatusCode ZeeTaPMonTool::bookHistograms()
{
  ATH_MSG_DEBUG("ZeeTaPMonTool::bookHistograms()");
  int start, end;
  start = 0;
  end = ENDCAP;

  // Create groups
  m_electronGroup    = new MonGroup(this,"egamma/tagandprobe"+m_GroupExtension,           run); // to be re-booked every new run
  // Create sub groups
  m_electronTrkGroup = new MonGroup(this,"egamma/tagandprobe"+m_GroupExtension+"/Track",     run); // to be re-booked every new run
  m_electronIdGroup  = new MonGroup(this,"egamma/tagandprobe"+m_GroupExtension+"/ID",        run); // to be re-booked every new run
  m_electronEffGroup = new MonGroup(this,"egamma/tagandprobe"+m_GroupExtension+"/EfficienciesIDISO", run, ManagedMonitorToolBase::ATTRIB_MANAGED , "", "effAsPerCent"); // to be re-booked every new run
  m_electronIsoGroup = new MonGroup(this,"egamma/tagandprobe"+m_GroupExtension+"/Isolation", run); // to be re-booked every new run
  m_electronLBGroup  = new MonGroup(this,"egamma/tagandprobe"+m_GroupExtension+"/LBMon",     run); // to be re-booked every new run

  // Number of Z candidates vs eta of leading electron
  bookTH1F(m_hNZcandidates,*m_electronGroup,"electronTagAndProbeNcandidates",   "Number of "+m_GroupExtension+" candidates vs eta of leading electron",64,-3.2,3.2);

  // Mass plots
  bookTH1F(m_hMass, *m_electronGroup,"electronTagAndProbeGlobalMass", "ee invariant mass",100,m_MassLowerCut,m_MassUpperCut);
  bookTH1FperRegion(m_hvMass, *m_electronGroup,"electronTagAndProbeMass", "ee invariant mass",100,m_MassLowerCut,m_MassUpperCut,start,end);

  // EFFICIENCIES IN MAIN PANEL
  bookTH1F(m_hIDEt,  *m_electronEffGroup,"EfficiencyTagAndProbeElectronIDvsEtOld",   "LHTight electron ID efficiency vs Et [MeV]",100, -1000.0, 250000.0);
  bookTH1F(m_hIDEta, *m_electronEffGroup,"EfficiencyTagAndProbeElectronIDvsEtaOld",  "LHTight electron ID efficiency vs #eta", 64, -3.2, 3.2);
  bookTH1F(m_hIDPhi, *m_electronEffGroup,"EfficiencyTagAndProbeElectronIDvsPhiOld",  "LHTight electron ID efficiency vs #phi", m_PhiBinning, -3.2, 3.2);
  bookTH1F(m_hISOEt, *m_electronEffGroup,"EfficiencyTagAndProbeElectronISOvsEtOld",  "LHTight electron ISO efficiency vs Et [MeV]",100, -1000.0, 250000.0);
  bookTH1F(m_hISOEta,*m_electronEffGroup,"EfficiencyTagAndProbeElectronISOvsEtaOld", "LHTight electron ISO efficiency vs #eta", 64, -3.2, 3.2);
  bookTH1F(m_hISOPhi,*m_electronEffGroup,"EfficiencyTagAndProbeElectronISOvsPhiOld", "LHTight electron ISO efficiency vs #phi", m_PhiBinning, -3.2, 3.2);

  bookTProfile(m_effhIDEt,  *m_electronGroup,"EfficiencyTagAndProbeElectronIDvsEt",   "LHTight electron ID efficiency vs Et [MeV]",100, -1000.0, 250000.0, 0.,1.1);
  bookTProfile(m_effhIDEta, *m_electronGroup,"EfficiencyTagAndProbeElectronIDvsEta",  "LHTight electron ID efficiency vs #eta", 64, -3.2, 3.2, 0.,1.1);
  bookTProfile(m_effhIDPhi, *m_electronGroup,"EfficiencyTagAndProbeElectronIDvsPhi",  "LHTight electron ID efficiency vs #phi", m_PhiBinning, -3.2, 3.2, 0.,1.1);
  bookTProfile(m_effhISOEt, *m_electronGroup,"EfficiencyTagAndProbeElectronISOvsEt",  "LHTight electron ISO efficiency vs Et [MeV]",100, -1000.0, 250000.0, 0.,1.1);
  bookTProfile(m_effhISOEta,*m_electronGroup,"EfficiencyTagAndProbeElectronISOvsEta", "LHTight electron ISO efficiency vs #eta", 64, -3.2, 3.2, 0., 1.1);
  bookTProfile(m_effhISOPhi,*m_electronGroup,"EfficiencyTagAndProbeElectronISOvsPhi", "LHTight electron ISO efficiency vs #phi", m_PhiBinning, -3.2, 3.2, 0., 1.1);

  // COMPLEMENTARY HISTOGRAMS FOR EXPERT PANEL
  bookTH1F(m_hN,          *m_electronGroup,"electronTagAndProbeN",          "Number of Probe electrons",40, 0.0, 40.0);
  bookTH1F(m_hEt,         *m_electronGroup,"electronTagAndProbeEt",         "Probe electron Et [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hEtaPhi,     *m_electronGroup,"electronTagAndProbeEtaPhi",     "Probe electron #eta,#phi map", 64, -3.2, 3.2, m_PhiBinning, -3.2, 3.2);
  bookTH1F(m_hEta,        *m_electronGroup,"electronTagAndProbeEta",        "Probe electron #eta", 64, -3.2, 3.2);
  bookTH1F(m_hPhi,        *m_electronGroup,"electronTagAndProbePhi",        "Probe electron #phi", m_PhiBinning, -3.2, 3.2);

  // TRACK PANEL
  bookTH1FperRegion(m_hvDeltaEta1     , *m_electronTrkGroup,"electronTagAndProbeDeltaEta1",     "PROBE electron track match #Delta #eta (1st sampling) ;#Delta #eta;Nevents", 50,-0.05,0.05,start,end);
  bookTH1FperRegion(m_hvDeltaPhi2     , *m_electronTrkGroup,"electronTagAndProbeDeltaPhi2",     "PROBE electron track match #Delta #phi (2st sampling) ;#Delta #phi;Nevents", 50,-0.15,0.1,start,end);
  bookTH1FperRegion(m_hvNOfBLayerHits , *m_electronTrkGroup,"electronTagAndProbeNOfBLayerHits", "PROBE electron number of track B-Layer Hits ;N B layer hits;Nevents", 6,-0.5,5.5,start,end);
  bookTH1FperRegion(m_hvNOfSiHits     , *m_electronTrkGroup,"electronTagAndProbeNOfSiHits",     "PROBE electron number of track precision Hits ;N Si hits;Nevents", 26,-0.5,25.5,start,end);
  bookTH1FperRegion(m_hvNOfTRTHits    , *m_electronTrkGroup,"electronTagAndProbeNOfTRTHits",    "PROBE electron number of TRT Hits ;N TRT hits;Nevents", 51,-0.5,50.5,start,end);

  // ID PANEL
  bookTH1FperRegion(m_hvEhad1         , *m_electronIdGroup,"electronTagAndProbeEhad1",          "PROBE electron energy leakage in 1st sampling of hadronic cal. ;E had1; Nevents", 50, -1000., 10000.,start,end);
  bookTH1FperRegion(m_hvEoverP        , *m_electronIdGroup,"electronTagAndProbeEoverP",         "PROBE electron match track E over P ;E/p;Nevents", 50,0,5,start,end);
  bookTH1FperRegion(m_hvCoreEM        , *m_electronIdGroup,"electronTagAndProbeCoreEM",         "PROBE electron core energy in EM calorimeter ;E [MeV]; Nevents",50, -5000., 250000.,start,end);
  bookTH1FperRegion(m_hvF1            , *m_electronIdGroup,"electronTagAndProbeF1",             "PROBE electron fractional energy in 1st sampling;F1; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvF2            , *m_electronIdGroup,"electronTagAndProbeF2",             "PROBE electron fractional energy in 2nd sampling;F2; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvF3            , *m_electronIdGroup,"electronTagAndProbeF3",             "PROBE electron fractional energy in 3rd sampling;F3; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvRe233e237     , *m_electronIdGroup,"electronTagAndProbeRe233e237",      "PROBE electron uncor. energy fraction in 3x3/3x7 cells in em sampling 2 ;R 3x3/3x7; Nevents", 50, 0., 2.,start,end);
  bookTH1FperRegion(m_hvRe237e277     , *m_electronIdGroup,"electronTagAndProbeRe237e277",      "PROBE electron uncor. energy fraction in 3x7/7x7 cells in em sampling 2 ;R 3x7/7x7; Nevents", 50, 0., 2.,start,end);

  // ISO PANEL
  bookTH1FperRegion(m_hvCaloIso  , *m_electronIsoGroup,"electronTagAndProbeCaloIso",       "PROBE electron calorimeter isolation;E_{iso} [MeV];Nevents", 50,-10000.,40000.0,start,end);
  bookTH1FperRegion(m_hvTrackIso , *m_electronIsoGroup,"electronTagAndProbeTrackIso",      "PROBE electron track isolation;E_{trk iso} [MeV];Nevents",   50,-10000.,40000.0,start,end);

  // LUMI BLOCK MONITORING PANEL

  std::string hname = std::string("LBEvoNZcandidates");
  std::string hlongname =  std::string("Number of Z candidates vs LB");
  bookTH1F(m_hLB_N, *m_electronLBGroup, hname, hlongname, 2000, -0.5, 1999.5);

  return StatusCode::SUCCESS;
}

StatusCode ZeeTaPMonTool::fillHistograms()
{
  ATH_MSG_DEBUG("ZeeTaPMonTool::fillHistograms()");

  if (!hasGoodTrigger("Zee T&P electron")) return StatusCode::SUCCESS; 

  //check whether Lar signaled event bad
  if(hasBadLar()) {
    ATH_MSG_DEBUG("ZeeTaPMonTool::hasBadLar()");
    return StatusCode::RECOVERABLE;
  }

  //--------------------
  //figure out current LB
  //--------------------
  const DataHandle<xAOD::EventInfo> evtInfo;
  StatusCode sc = m_storeGate->retrieve(evtInfo); 
  if (sc.isFailure()) {
    ATH_MSG_ERROR("couldn't retrieve event info");
    return StatusCode::FAILURE;
  }
  
  unsigned int previousLB = m_currentLB;
  m_currentLB = evtInfo->lumiBlock();
  
  //deal with the change of LB
  if (m_currentLB>previousLB) {
    // update the by LB variables
    m_nZCandidatesPerLumiBlock.push_back(m_nZCandidatesInCurrentLB);
    // Reset counters
    m_nZCandidatesInCurrentLB=0;
  }
  
  // Get electron container
  const xAOD::ElectronContainer* electron_container=0;
  sc = m_storeGate->retrieve(electron_container, m_ElectronContainer);
  if(sc.isFailure() || !electron_container){
    ATH_MSG_VERBOSE("no electron container found in TDS");
    return sc;
  } 

  xAOD::ElectronContainer::const_iterator e_iter = electron_container->begin();
  xAOD::ElectronContainer::const_iterator e_end  = electron_container->end();
  ATH_MSG_DEBUG("Number of electrons: " << electron_container->size());

  int ngood_el = 0;
  int n_tot = 0;
  float lead_et=-999, subl_et=-999;
  const xAOD::Electron *lead_el = 0;
  const xAOD::Electron *subl_el = 0;

  for (; e_iter!=e_end; e_iter++) {

    if(!(*e_iter)) continue;
    // Formally unnecessary because all electrons in the container have these authors by construction
    if (!((*e_iter)->author(xAOD::EgammaParameters::AuthorElectron)|| (*e_iter)->author(xAOD::EgammaParameters::AuthorAmbiguous))) continue;

    // Ask these electrons to be LHLoose
    bool isGood=false;

    bool passed = (*e_iter)->passSelection(isGood,"LHLoose");
    if( passed || !isGood ) ATH_MSG_DEBUG("not a good LHLoose electron candidate found in TDS");

    // LHMedium
    // sc = (*e_iter)->passSelection(isGood,"LHMedium");
    // if(sc.isFailure() || !isGood ) ATH_MSG_DEBUG("not a good LHMedium electron candidate found in TDS");
 
    // LHTight
    // sc = (*e_iter)->passSelection(isGood,"LHTight");
    // if(sc.isFailure() || !isGood ) ATH_MSG_DEBUG("not a good electron candidate found in TDS");

    if(isGood) {
      ++ngood_el;
      // Look for two electrons
      float et  = (*e_iter)->pt();
      if(et>lead_et) {
	subl_et = lead_et;
	subl_el = lead_el;
	lead_et = et;
	lead_el = *e_iter;
      } else if(et>subl_et) {
	subl_et = et;
	subl_el = *e_iter;
      }
    }
  }


  // Fill number of electrons histograms
  if (m_hN) m_hN->Fill(ngood_el);
  
  // request at least two electrons

  if (ngood_el<2)  return StatusCode::SUCCESS; 
    
  // Et cuts
  if(lead_et<m_ElectronEtCut || lead_et<0) return StatusCode::SUCCESS; 
  if(subl_et<m_ElectronEtCut || subl_et<0) return StatusCode::SUCCESS; 
  
  // Mass window
  float mass     = (lead_el->p4()+subl_el->p4()).M(); 
  ATH_MSG_DEBUG("Zee mass & cuts:                    (" << mass << ", " << m_MassLowerCut << ", " << m_MassUpperCut << ")");
  if(mass<m_MassLowerCut || mass>m_MassUpperCut) return StatusCode::SUCCESS; 
  
  // Basic kinematics
  float lead_eta = lead_el->eta();
  float lead_phi = lead_el->phi();      
  float subl_eta = subl_el->eta();
  float subl_phi = subl_el->phi();      
  ATH_MSG_DEBUG("Leading electron (eta,phi,et,q):    (" << lead_eta << ", " << lead_phi << ", " << lead_et << ", " << lead_el->charge() << ")");
  ATH_MSG_DEBUG("Subleading electron (eta,phi,et,q): (" << subl_eta << ", " << subl_phi << ", " << subl_et << ", " << subl_el->charge() << ")");
  
  // Eta cuts
  if(fabs(lead_eta)>2.47 || fabs(subl_eta)>2.47) return StatusCode::SUCCESS;
  
  // Check charges
  if (lead_el->charge()*subl_el->charge()>=0) return StatusCode::SUCCESS; 
  
  bool lead_isLHTight = false;
  if ( !lead_el->passSelection(lead_isLHTight,"LHTight") ) return StatusCode::FAILURE;

  bool subl_isLHTight = false;
  if ( !subl_el->passSelection(subl_isLHTight,"LHTight") ) return StatusCode::FAILURE;

  bool EventZcandidateUsed = false;
  
  // If leading electron is LHTight use subleading as probe
  if(lead_isLHTight) { 
    ++n_tot;
    m_hNZcandidates->Fill(lead_eta);
    EventZcandidateUsed = true;
    // Isolation Energy 
    float topoetcone40;
    bool subl_isIsolated = false;
    if ( !subl_el->isolationValue(topoetcone40,xAOD::Iso::topoetcone40) ) return StatusCode::FAILURE;
    if ( topoetcone40 < 4.5*GeV ) subl_isIsolated = true;
    fillElectronProbe(subl_el, subl_isLHTight, subl_isIsolated, mass);
  }
  
  // If subleading electron is LHTight use leading as probe
  if(subl_isLHTight) { 
    ++n_tot;
    if (!EventZcandidateUsed) m_hNZcandidates->Fill(subl_eta);
    // Isolation Energy 
    float topoetcone40;
    bool lead_isIsolated = false;
    if ( !lead_el->isolationValue(topoetcone40,xAOD::Iso::topoetcone40) ) return StatusCode::FAILURE;
    if ( topoetcone40 < 4.5*GeV ) lead_isIsolated = true;
    fillElectronProbe(lead_el, lead_isLHTight, lead_isIsolated, mass);
  }
  
  // Fill number of Z found in the LB
  if (m_hLB_N) m_hLB_N->Fill(m_currentLB);
  
  return StatusCode::SUCCESS;
}

void ZeeTaPMonTool::fillElectronProbe(const xAOD::Electron *el, bool isTight, bool isIso, double mass)
{

  float et  = el->pt();
  float eta = el->eta();
  float phi = el->phi();      

  int ir = GetRegion(eta);
  if (m_hEt)     m_hEt->Fill(et);
  if (m_hEtaPhi) m_hEtaPhi->Fill(eta,phi);
  if (m_hEta) {
    m_hEta->Fill(eta);
  }
  else {
     ATH_MSG_DEBUG("ERRORBL   eta histo not filled !!!!");
  }

  if (m_hPhi)    m_hPhi->Fill(phi);
  fillTH1FperRegion(m_hvMass,ir,mass);
  if (m_hMass)  m_hMass->Fill(mass);

  if (isTight) {
    if (m_hIDEt) m_hIDEt->Fill(et);
    if (m_hIDEta) m_hIDEta->Fill(eta);
    if (m_hIDPhi) m_hIDPhi->Fill(phi);
    if (m_effhIDEt) m_effhIDEt->Fill(et,1.);
    if (m_effhIDEta) m_effhIDEta->Fill(eta,1.);
    if (m_effhIDPhi) m_effhIDPhi->Fill(phi,1.);
  } else {
    if (m_effhIDEt) m_effhIDEt->Fill(et,0.);
    if (m_effhIDEta) m_effhIDEta->Fill(eta,0.);
    if (m_effhIDPhi) m_effhIDPhi->Fill(phi,0.);    
  }

  if (isTight && isIso) {
    if (m_hISOEt)  m_hISOEt->Fill(et);
    if (m_hISOEta) m_hISOEta->Fill(eta);
    if (m_hISOPhi) m_hISOPhi->Fill(phi);
    if (m_effhISOEt)  m_effhISOEt->Fill(et,1.);
    if (m_effhISOEta) m_effhISOEta->Fill(eta,1.);
    if (m_effhISOPhi) m_effhISOPhi->Fill(phi,1.);
  } else {
    if (m_effhISOEt)  m_effhISOEt->Fill(et,0.);
    if (m_effhISOEta) m_effhISOEta->Fill(eta,0.);
    if (m_effhISOPhi) m_effhISOPhi->Fill(phi,0.);
  }

  // Cluster track match details
  float deltaEta1 = -999.0;
  if( el->trackCaloMatchValue(deltaEta1, xAOD::EgammaParameters::deltaEta1) ) {
    fillTH1FperRegion(m_hvDeltaEta1,ir,deltaEta1);
  }
  float deltaPhi2 = -999.0;
  if( el->trackCaloMatchValue(deltaPhi2, xAOD::EgammaParameters::deltaPhi2) ) {
    fillTH1FperRegion(m_hvDeltaPhi2,ir,deltaPhi2);
  }
    
  // Shower shape variable details
  float ehad1 = 0.0;
  float ecore = 0.0;
  float f1    = 0.0;    
  float f3    = 0.0;     
  float e233  = 0.0;
  float e237  = 0.0;
  float e277  = 0.0;
  if( el->showerShapeValue(ehad1, xAOD::EgammaParameters::ehad1) ) {
    fillTH1FperRegion(m_hvEhad1,ir,ehad1);
  }
  if( el->showerShapeValue(ecore, xAOD::EgammaParameters::ecore) ) {
    fillTH1FperRegion(m_hvCoreEM,ir,ecore);    
  }
  if( el->showerShapeValue(f1, xAOD::EgammaParameters::f1) ) {
    fillTH1FperRegion(m_hvF1,ir,f1);    
  }
  if( el->showerShapeValue(f3, xAOD::EgammaParameters::f3) ) {
    fillTH1FperRegion(m_hvF3,ir,f3);    
  }
  if( el->showerShapeValue(e237, xAOD::EgammaParameters::e237) ) {
    float Re233e237 = 0.0;
    if( e237!=0 && el->showerShapeValue(e233, xAOD::EgammaParameters::e233) ) {
      Re233e237 = e233 / e237;
    }
    fillTH1FperRegion(m_hvRe233e237,ir,Re233e237);
    float Re237e277 = 0.0;
    if( e237!=0 && el->showerShapeValue(e277, xAOD::EgammaParameters::e277) ) {
      if(e277!=0) Re237e277 = e237 / e277;
    }
    fillTH1FperRegion(m_hvRe237e277,ir,Re237e277);
  }
  
  // Isolation Energy 
  float topoetcone40;
  if( el->isolationValue(topoetcone40,xAOD::Iso::topoetcone40)) {
    fillTH1FperRegion(m_hvCaloIso,ir,topoetcone40);
  }
  float ptcone20;
  if( el->isolationValue(ptcone20,xAOD::Iso::ptcone20)) {
    fillTH1FperRegion(m_hvTrackIso,ir,ptcone20);
  }

  // Associated track details
  const xAOD::TrackParticle *t = el->trackParticle();
  double trackp = 0; 
  unsigned char numberOfInnermostPixelLayerHits=-1;
  unsigned char numberOfPixelHits=-1;
  unsigned char numberOfSCTHits=-1;
  unsigned char numberOfTRTHits=-1;
  if(t) {
    trackp = t->pt()*cosh(t->eta());
    // retrieve track summary information
    if( t->summaryValue(numberOfInnermostPixelLayerHits,xAOD::numberOfInnermostPixelLayerHits) ) {
      fillTH1FperRegion(m_hvNOfBLayerHits,ir,numberOfInnermostPixelLayerHits);
    }
    if( t->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) && t->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ) {
      fillTH1FperRegion(m_hvNOfSiHits,ir,numberOfPixelHits+numberOfSCTHits);
    }
    if( t->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits) ) {
      fillTH1FperRegion(m_hvNOfTRTHits,ir,numberOfTRTHits);
    }
  } else ATH_MSG_DEBUG( "Error attempting to retrieve associated track");

  // Associated cluster details
  const xAOD::CaloCluster *aCluster = el->caloCluster();
  if (aCluster) {
    float ep = 0;	
    if(trackp !=0) ep = aCluster->e()/trackp;
    fillTH1FperRegion(m_hvEoverP,ir,ep);
    double ec = aCluster->et()*cosh(aCluster->eta());
    float f2 = 0.0;
    if(ec!=0) f2 = aCluster->energyBE(2)/ec;
    fillTH1FperRegion(m_hvF2,ir,f2);    
  } else ATH_MSG_WARNING( "Can't get CaloCluster" );
}

StatusCode ZeeTaPMonTool::procHistograms()
{
  ATH_MSG_DEBUG("ZeeTaPMonTool::procHistograms()");
  //normalize efficiencies
  if (m_hIDEt   && m_hEt )   fillEfficiencies(m_hIDEt,   m_hEt);
  if (m_hIDEta  && m_hEta)   fillEfficiencies(m_hIDEta,  m_hEta);
  if (m_hIDPhi  && m_hPhi)   fillEfficiencies(m_hIDPhi,  m_hPhi);
  if (m_hISOEt  && m_hEt )   fillEfficiencies(m_hISOEt,  m_hEt);
  if (m_hISOEta && m_hEta)   fillEfficiencies(m_hISOEta, m_hEta);
  if (m_hISOPhi && m_hPhi)   fillEfficiencies(m_hISOPhi, m_hPhi);

  return StatusCode::SUCCESS;
}

