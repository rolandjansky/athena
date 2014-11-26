/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
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
  :  egammaMonToolBase(type,name,parent)
{
  // Name of the electron collection
  declareProperty("ElectronContainer", m_ElectronContainer = "ElectronCollection", "Name of the electron collection" );
  declareProperty("massPeak", m_MassPeak = 91188, "Zee pak position" );
  declareProperty("electronEtCut",m_ElectronEtCut = 15*GeV, "Et cut for electrons");
  declareProperty("massLowerCut", m_MassLowerCut = 70*GeV,"Lower mass cut");
  declareProperty("massUpperCut", m_MassUpperCut = 110*GeV,"Upper mass cut");
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
  MonGroup electronGroup(this,"egamma/tagandprobe",run); // to be re-booked every new run
  
  // Create sub groups
  MonGroup electronTrkGroup      (this,"egamma/tagandprobe/Track",     run); // to be re-booked every new run
  MonGroup electronIdGroup       (this,"egamma/tagandprobe/ID",        run); // to be re-booked every new run
  MonGroup electronIsoGroup      (this,"egamma/tagandprobe/Isolation", run); // to be re-booked every new run

  // Mass plots
  bookTH1FperRegion(m_hvMass, electronGroup,"electronTagAndProbeMass",     "Zee mass",100,m_MassLowerCut,m_MassUpperCut,start,end);

  // EFFICIENCIES IN MAIN PANEL
  bookTH1F(m_hIDEt,       electronGroup,"electronTagAndProbeIDvsEt",   "LOOSE electron ID efficiency vs transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH1F(m_hIDEta,      electronGroup,"electronTagAndProbeIDvsEta",  "LOOSE electron ID efficiency vs #eta", 64, -3.2, 3.2);
  bookTH1F(m_hISOEt,      electronGroup,"electronTagAndProbeISOvsEt",  "LOOSE electron ISO efficiency vs transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH1F(m_hISOEta,     electronGroup,"electronTagAndProbeISOvsEta", "LOOSE electron ISO efficiency vs #eta", 64, -3.2, 3.2);

  // COMPLEMENTARY HISTOGRAMS FOR EXPERT PANEL
  bookTH1F(m_hN,          electronGroup,"electronTagAndProbeN",          "Number of LOOSE electrons",40, 0.0, 40.0);
  bookTH1F(m_hEt,         electronGroup,"electronTagAndProbeEt",         "LOOSE electron transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hEtaPhi,     electronGroup,"electronTagAndProbeEtaPhi",     "LOOSE electron #eta,#phi map", 64, -3.2, 3.2, 64, -3.2, 3.2);
  bookTH1F(m_hEta,        electronGroup,"electronTagAndProbeEta",        "LOOSE electron #eta", 64, -3.2, 3.2);
  bookTH1F(m_hPhi,        electronGroup,"electronTagAndProbePhi",        "LOOSE electron #phi", 64, -3.2, 3.2);

  // TRACK PANEL
  bookTH1FperRegion(m_hvDeltaEta1     , electronTrkGroup,"electronTagAndProbeDeltaEta1",     "PROBE electron track match #Delta #eta (1st sampling) ;#Delta #eta;Nevents", 50,-0.05,0.05,start,end);
  bookTH1FperRegion(m_hvDeltaPhi2     , electronTrkGroup,"electronTagAndProbeDeltaPhi2",     "PROBE electron track match #Delta #phi (2st sampling) ;#Delta #phi;Nevents", 50,-0.15,0.1,start,end);
  bookTH1FperRegion(m_hvNOfBLayerHits , electronTrkGroup,"electronTagAndProbeNOfBLayerHits", "PROBE electron number of track B-Layer Hits ;N B layer hits;Nevents", 6,-0.5,5.5,start,end);
  bookTH1FperRegion(m_hvNOfSiHits,      electronTrkGroup,"electronTagAndProbeNOfSiHits",     "PROBE electron number of track precision Hits ;N Si hits;Nevents", 26,-0.5,25.5,start,end);
  bookTH1FperRegion(m_hvNOfTRTHits    , electronTrkGroup,"electronTagAndProbeNOfTRTHits",    "PROBE electron number of TRT Hits ;N TRT hits;Nevents", 51,-0.5,50.5,start,end);

  // ID PANEL
  bookTH1FperRegion(m_hvEhad1         , electronIdGroup,"electronTagAndProbeEhad1",          "PROBE electron energy leakage in 1st sampling of hadronic cal. ;E had1; Nevents", 50, -1000., 10000.,start,end);
  bookTH1FperRegion(m_hvEoverP        , electronIdGroup,"electronTagAndProbeEoverP",         "PROBE electron match track E over P ;E/p;Nevents", 50,0,5,start,end);
  bookTH1FperRegion(m_hvCoreEM        , electronIdGroup,"electronTagAndProbeCoreEM",         "PROBE electron core energy in EM calorimeter ;E [MeV]; Nevents",50, -5000., 250000.,start,end);
  bookTH1FperRegion(m_hvF1            , electronIdGroup,"electronTagAndProbeF1",             "PROBE electron fractional energy in 1st sampling;F1; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvF2            , electronIdGroup,"electronTagAndProbeF2",             "PROBE electron fractional energy in 2nd sampling;F2; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvF3            , electronIdGroup,"electronTagAndProbeF3",             "PROBE electron fractional energy in 3rd sampling;F3; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvRe233e237     , electronIdGroup,"electronTagAndProbeRe233e237",      "PROBE electron uncor. energy fraction in 3x3/3x7 cells in em sampling 2 ;R 3x3/3x7; Nevents", 50, 0., 2.,start,end);
  bookTH1FperRegion(m_hvRe237e277     , electronIdGroup,"electronTagAndProbeRe237e277",      "PROBE electron uncor. energy fraction in 3x7/7x7 cells in em sampling 2 ;R 3x7/7x7; Nevents", 50, 0., 2.,start,end);

  // ISO PANEL
  bookTH1FperRegion(m_hvCaloIso       , electronIsoGroup,"electronTagAndProbeCaloIso",       "PROBE electron calorimeter isolation;E_{iso} [MeV];Nevents", 50,-10,40.0,start,end);
  bookTH1FperRegion(m_hvTrackIso      , electronIsoGroup,"electronTagAndProbeTrackIso",      "PROBE electron track isolation;E_{trk iso} [MeV];Nevents",   50,-10,40.0,start,end);

  return StatusCode::SUCCESS;
}

StatusCode ZeeTaPMonTool::fillHistograms()
{
  ATH_MSG_DEBUG("ZeeTaPMonTool::fillHistograms()");
  //check whether Lar signalled event bad
  if(hasBadLar()) {
    ATH_MSG_DEBUG("ZeeTaPMonTool::hasBadLar()");
    return StatusCode::SUCCESS;
  }
  
  StatusCode sc;

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

  int n_tot = 0;
  float lead_et=-999, subl_et=-999;
  const xAOD::Electron *lead_el = 0;
  const xAOD::Electron *subl_el = 0;

  for (; e_iter!=e_end; e_iter++) {
    if(!(*e_iter)) continue;
    if((*e_iter)->author(xAOD::EgammaParameters::AuthorElectron)==0) continue;

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

  // Et cuts
  if(lead_et<m_ElectronEtCut || lead_et<0) { m_hN->Fill(n_tot); return sc; }
  if(subl_et<m_ElectronEtCut || subl_et<0) { m_hN->Fill(n_tot); return sc; }

  // Mass window
  float mass     = (lead_el->p4()+subl_el->p4()).M(); 
  ATH_MSG_DEBUG("Zee mass & cuts:                    (" << mass << ", " << m_MassLowerCut << ", " << m_MassUpperCut << ")");
  if(mass<m_MassLowerCut || mass>m_MassUpperCut) { m_hN->Fill(n_tot); return sc; }

  // Basic kinematics
  float lead_eta = lead_el->eta();
  float lead_phi = lead_el->phi();      
  float subl_eta = subl_el->eta();
  float subl_phi = subl_el->phi();      
  ATH_MSG_DEBUG("Leading electron (eta,phi,et,q):    (" << lead_eta << ", " << lead_phi << ", " << lead_et << ", " << lead_el->charge() << ")");
  ATH_MSG_DEBUG("Subleading electron (eta,phi,et,q): (" << subl_eta << ", " << subl_phi << ", " << subl_et << ", " << subl_el->charge() << ")");

  // Eta cuts
  if(fabs(lead_eta)>2.47 || fabs(subl_eta)>2.47) { m_hN->Fill(n_tot); return sc; }

  // Check charges
  if (lead_el->charge()*subl_el->charge()>=0) { m_hN->Fill(n_tot); return sc; }
  
  // Check ID status for both candidates
  bool lead_isTight;
  if(!lead_el->passSelection(lead_isTight,"Tight")) lead_isTight = false;
  bool subl_isTight;
  if(!subl_el->passSelection(subl_isTight,"Tight")) subl_isTight = false;

  // If leading electron is TightPP use subleading as probe
  if(lead_isTight) { // TightPP: says menu not defined for electrons
    ++n_tot;
    fillElectronProbe(subl_el,subl_isTight,mass);
  }
  // If subleading electron is TightPP use leading as probe
  if(subl_isTight) { // TightPP: says menu not defined for electrons
    ++n_tot;
    fillElectronProbe(lead_el,lead_isTight,mass);
  }

  // Fill number of electrons histograms
  m_hN->Fill(n_tot);

  return sc;
}

void ZeeTaPMonTool::fillElectronProbe(const xAOD::Electron *el, bool isTight, float mass)
{
  float et  = el->pt();
  float eta = el->eta();
  float phi = el->phi();      

  int ir = GetRegion(eta);
  if(m_hEt)     m_hEt->Fill(et);
  if(m_hEtaPhi) m_hEtaPhi->Fill(eta,phi);
  if(m_hEta)    m_hEta->Fill(eta);
  if(m_hPhi)    m_hPhi->Fill(phi);
  fillTH1FperRegion(m_hvMass,ir,mass);

  if(isTight) {
    m_hIDEt->Fill(et);
    m_hIDEta->Fill(eta);
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

  // Associated track details
  const xAOD::TrackParticle *t = el->trackParticle();
  double trackp = 0; 
  unsigned char numberOfBLayerHits=-1;
  unsigned char numberOfPixelHits=-1;
  unsigned char numberOfSCTHits=-1;
  unsigned char numberOfTRTHits=-1;
  if(t) {
    trackp = t->pt()/cosh(t->eta());
    // retrieve track summary information
    if( t->summaryValue(numberOfBLayerHits,xAOD::numberOfBLayerHits) ) {
      fillTH1FperRegion(m_hvNOfBLayerHits,ir,numberOfBLayerHits);
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
  fillEfficiencies(m_hIDEt,   m_hEt);
  fillEfficiencies(m_hIDEta,  m_hEta);
  fillEfficiencies(m_hISOEt,  m_hEt);
  fillEfficiencies(m_hISOEta, m_hEta);

  return StatusCode::SUCCESS;
}

