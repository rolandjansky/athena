/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//
//      NAME:    electronMonTool.cxx
//      PACKAGE: offline/Reconstruction/egamma/egammaPerformance
//      PURPOSE: Shifter should look for the following defects:
//      taken from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/EgammaShifts
//          
//       in energies (Et plots for electrons and photons)
//       ===========
//          LOWSTAT : For short runs and not enough statistics when not being able
//          to judge the result using the egamma stream. Needs to be commented.
//          
//          ENERGY_(BARREL,ENDCAP,CRACK,FORWARD) : This defect is a significant energy
//          mis-calibration is observed for a certain part of the detector. This flag 
//          is then intolerable for that detector region.
//          
//          LARNOISE_(BARREL,ENDCAP,FORWARD) : If some part of the detector is miscalibrated
//          causing an energy shift of at least 5% this intolerable defect should be set. 
//          
//       in eta,phi (eta,phi 2D plots for electrons and photons)
//       ==========
//          ETAPHI_SPIKES/LOWOCCUPANCY : Non standard distribution (eta phi plane) of egamma
//          objects. Region with unusual density of electrons / photons. This could be produced
//          by a problem in the LAR calorimeter. If there is a spike in the distribution set
//          EGAMMA_ETAPHI_SPIKES as defect. Examples of runs with this problem can be found in
//          Click for image. (hot spot) and Click for image. (FEB with 100% rejection during part
//          of the run). If a HV trip could not be recovered during the run, there will be a region
//          in the eta phi plane with low occupancy. Then set the defect EGAMMA_LOWOCCUPANCY.
//          
//       in eta and phi (eta,phi 1D plots for electrons)
//       ==============
//          BEAMSPOT : When the beamspot is shifted with respect to the nominal position the
//          azimuthal distribution (phi) of tracks is different than normal. When the new
//          constants for the beam spot get uploaded and reprocessed this defect vanishes.
//          This is an intolerable defect so only use it after checking the bulk reconstruction,
//          in some cases express reconstruction can have this problem but usually is solved at
//          the bulk. Example of the run with a failure in the determination of the beam spot can
//          be found in Click for image..
//          
//          BAD_BLAYER : To be used when the distributions (eta, phi) for medium and tight electrons
//          have marked excess or defect of entries due to a problem with the b-layer. If this happens
//          the loose distributions will look fine. To distinguish between this and TRT problem more
//          details on the run and investigation will be needed. If you have to use this defect you
//          should document it in the EgammaShiftsBadRuns page. Example of the result of a b-layer
//          module with low efficiency can be found in Click for image..
//          
//          BAD_TRT : To be used when the distributions (eta, phi) for tight electrons / photons have
//          marked excess or defect of entries due to a problem with the TRT (due to the high threshold
//          requirement). If this happens the loose and medium distributions will look fine. To distinguish
//          between this and pixel (b-layer) problem more details on the run and investigation will be needed.
//          If you have to use this defect you should document it in the Runs List page. Example of the result
//          of a few RODs disabled in TRT can be found in Click for image..
//          
//       in electron ID
//       ==============
//          SHAPES : Bad shape for defining variables of electrons or photons. Distributions do not match
//          the references. As the other defects they can be recoverable or not recoverable. In case, of
//          a hardware related issue, it will be likely not recoverable, problems related with alignment
//          or calibration will be likely recoverable. Example of this defect can be found in Click for image.
//          (decrease in the number of Si Hits due to BCID problem in SCT).
//          
/////////////////////////////////////////////////////////////


#include "egammaPerformance/electronMonTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "TH1F.h"
#include "TH2F.h"

electronMonTool::electronMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  :  egammaMonToolBase(type,name,parent),
     m_hN (nullptr),
     m_hEt (nullptr),
     m_hPhi (nullptr),
     m_hEtaPhi (nullptr),
     m_hTightN (nullptr),
     m_hTightEt (nullptr),
     m_hTightEtaPhi (nullptr)
{
  // Name of the electron collection
  declareProperty("ElectronContainer", m_ElectronContainer = "ElectronCollection", "Name of the electron collection" );
}

electronMonTool::~electronMonTool()
{
}

StatusCode electronMonTool::bookHistograms()
{
  ATH_MSG_DEBUG("electronMonTool::bookHistograms()");
  int start, end;
  start = 0;
  end = ENDCAP;

  // Create groups
  MonGroup electronGroup(this,"egamma/electrons",run); // to be re-booked every new run
  
  // Create sub groups
  MonGroup electronTrkGroup      (this,"egamma/electrons/Track",            run); // to be re-booked every new run
  MonGroup electronIdGroup       (this,"egamma/electrons/ID",               run); // to be re-booked every new run

  // MAIN PANEL
  bookTH1F(m_hN,          electronGroup,"electronN",          "Number of LOOSE electrons",40, 0.0, 40.0);
  bookTH1F(m_hEt,         electronGroup,"electronEt",         "LOOSE electron transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hEtaPhi,     electronGroup,"electronEtaPhi",     "LOOSE electron #eta,#phi map", 64, -3.2, 3.2, 64, -3.2, 3.2);
  bookTH1F(m_hEta,        electronGroup,"electronEta",        "LOOSE electron #eta", 64, -3.2, 3.2);
  bookTH1F(m_hPhi,        electronGroup,"electronPhi",        "LOOSE electron #phi", 64, -3.2, 3.2);
  bookTH1F(m_hTightN,     electronGroup,"electronTightN",     "Number of TIGHT electrons",40, 0.0, 40.0);
  bookTH1F(m_hTightEt,    electronGroup,"electronTightEt",    "TIGHT electron transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hTightEtaPhi,electronGroup,"electronTightEtaPhi","TIGHT electron #eta,#phi map", 64, -3.2, 3.2, 64, -3.2, 3.2);
  bookTH1F(m_hTightEta,   electronGroup,"electronTightEta",   "TIGHT electron #eta", 64, -3.2, 3.2);
  bookTH1F(m_hTightPhi,   electronGroup,"electronTightPhi",   "TIGHT electron #phi", 64, -3.2, 3.2);
  bookTH1FperRegion(m_hvEt, electronGroup,"electronEt", "LOOSE electron transverse energy [MeV]",100, -1000.0, 250000.0,start,end);

  // TRACK PANEL
  bookTH1FperRegion(m_hvTightNOfBLayerHits , electronTrkGroup,"electronTightNOfBLayerHits",  "TIGHT electron number of track B-Layer Hits ;N B layer hits;Nevents", 6,-0.5,5.5,start,end);
  bookTH1FperRegion(m_hvTightNOfSiHits,      electronTrkGroup,"electronTightNOfSiHits", "TIGHT electron number of track precision Hits ;N Si hits;Nevents", 26,-0.5,25.5,start,end);
  bookTH1FperRegion(m_hvTightNOfTRTHits    , electronTrkGroup,"electronTightNOfTRTHits",     "TIGHT electron number of TRT Hits ;N TRT hits;Nevents", 51,-0.5,50.5,start,end);

  // ID PANEL
  bookTH1FperRegion(m_hvEhad1         , electronIdGroup,"electronEhad1",          "LOOSE electron energy leakage in 1st sampling of hadronic cal. ;E had1; Nevents", 50, -1000., 10000.,start,end);
  bookTH1FperRegion(m_hvEoverP        , electronIdGroup,"electronEoverP",         "LOOSE electron match track E over P ;E/p;Nevents", 50,0,5,start,end);
  bookTH1FperRegion(m_hvCoreEM        , electronIdGroup,"electronCoreEM",         "LOOSE electron core energy in EM calorimeter ;E [MeV]; Nevents",50, -5000., 250000.,start,end);
  bookTH1FperRegion(m_hvF1            , electronIdGroup,"electronF1",             "LOOSE electron fractional energy in 1st sampling;F1; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvF2            , electronIdGroup,"electronF2",             "LOOSE electron fractional energy in 2nd sampling;F2; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvF3            , electronIdGroup,"electronF3",             "LOOSE electron fractional energy in 3rd sampling;F3; Nevents", 50, -0.2,1.0,start,end);
  bookTH1FperRegion(m_hvRe233e237     , electronIdGroup,"electronRe233e237",      "LOOSE electron uncor. energy fraction in 3x3/3x7 cells in em sampling 2 ;R 3x3/3x7; Nevents", 50, 0., 2.,start,end);
  bookTH1FperRegion(m_hvRe237e277     , electronIdGroup,"electronRe237e277",      "LOOSE electron uncor. energy fraction in 3x7/7x7 cells in em sampling 2 ;R 3x7/7x7; Nevents", 50, 0., 2.,start,end);

  // EXPERT PER REGION PANEL
  bookTH1FperRegion(m_hvN,       electronGroup,"electronN",  "Number of LOOSE electrons",40, 0.0, 40.0,start,end);
  bookTH1FperRegion(m_hvEta,     electronGroup,"electronEta","LOOSE electron #eta",64, -3.2, 3.2,start,end);
  bookTH1FperRegion(m_hvPhi,     electronGroup,"electronPhi","LOOSE electron #phi",64, -3.2, 3.2,start,end);
  bookTH1FperRegion(m_hvTightN,  electronGroup,"electronTightN",  "Number of TIGHT electrons",40, 0.0, 40.0,start,end);
  bookTH1FperRegion(m_hvTightEt, electronGroup,"electronTightEt", "TIGHT electron transverse energy [MeV]",100, -1000.0, 250000.0,start,end);
  bookTH1FperRegion(m_hvTightEta,electronGroup,"electronTightEta","TIGHT electron #eta",64, -3.2, 3.2,start,end);
  bookTH1FperRegion(m_hvTightPhi,electronGroup,"electronTightPhi","TIGHT electron #phi",64, -3.2, 3.2,start,end);

  // EXPERT TRACK PANEL
  bookTH1FperRegion(m_hvNOfBLayerHits ,      electronTrkGroup,"electronNOfBLayerHits",  "LOOSE electron number of track B-Layer Hits ;N B layer hits;Nevents", 6,-0.5,5.5,start,end);
  bookTH1FperRegion(m_hvNOfSiHits,           electronTrkGroup,"electronNOfSiHits", "LOOSE electron number of track precision Hits ;N Si hits;Nevents", 26,-0.5,25.5,start,end);
  bookTH1FperRegion(m_hvNOfTRTHits    ,      electronTrkGroup,"electronNOfTRTHits",     "LOOSE electron number of TRT Hits ;N TRT hits;Nevents", 51,-0.5,50.5,start,end);
  bookTH1FperRegion(m_hvDeltaEta1     ,      electronTrkGroup,"electronDeltaEta1",      "LOOSE electron track match #Delta #eta (1st sampling) ;#Delta #eta;Nevents", 50,-0.05,0.05,start,end);
  bookTH1FperRegion(m_hvDeltaPhi2     ,      electronTrkGroup,"electronDeltaPhi2",      "LOOSE electron track match #Delta #phi (2st sampling) ;#Delta #phi;Nevents", 50,-0.15,0.1,start,end);
  bookTH1FperRegion(m_hvTightDeltaEta1     , electronTrkGroup,"electronTightDeltaEta1",      "TIGHT electron track match #Delta #eta (1st sampling) ;#Delta #eta;Nevents", 50,-0.05,0.05,start,end);
  bookTH1FperRegion(m_hvTightDeltaPhi2     , electronTrkGroup,"electronTightDeltaPhi2",      "TIGHT electron track match #Delta #phi (2st sampling) ;#Delta #phi;Nevents", 50,-0.15,0.1,start,end);

  return StatusCode::SUCCESS;
}

StatusCode electronMonTool::fillHistograms()
{
  ATH_MSG_DEBUG("electronMonTool::fillHistograms()");
  //check whether Lar signalled event bad
  if(hasBadLar()) {
    ATH_MSG_DEBUG("electronMonTool::hasBadLar()");
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
  int n_tot_tight = 0;
  std::vector<int> n_el, n_el_tight;
  n_el.resize(NREGION,0);
  n_el_tight.resize(NREGION,0);
  for (; e_iter!=e_end; e_iter++){
    if(!(*e_iter)) continue;
    if((*e_iter)->author(xAOD::EgammaParameters::AuthorElectron)==0) continue;

    // Basic kinematics
    float et  = (*e_iter)->pt();
    float eta = (*e_iter)->eta();
    float phi = (*e_iter)->phi();      
    int ir = GetRegion(eta);
    //ATH_MSG_DEBUG("electrons et, eta and phi" << et << " " << eta << " " << phi);
    
    ++n_tot;
    ++(n_el[ir]);
    if(m_hEt)     m_hEt->Fill(et);
    if(m_hEtaPhi) m_hEtaPhi->Fill(eta,phi);
    if(m_hEta)    m_hEta->Fill(eta);
    if(m_hPhi)    m_hPhi->Fill(phi);
    fillTH1FperRegion(m_hvEt,ir,et);
    fillTH1FperRegion(m_hvEta,ir,eta);
    fillTH1FperRegion(m_hvPhi,ir,phi);
    
    // Cluster track match details
    float deltaEta1 = -999.0;
    if( (*e_iter)->trackCaloMatchValue(deltaEta1, xAOD::EgammaParameters::deltaEta1) ) {
      fillTH1FperRegion(m_hvDeltaEta1,ir,deltaEta1);
    }
    float deltaPhi2 = -999.0;
    if( (*e_iter)->trackCaloMatchValue(deltaPhi2, xAOD::EgammaParameters::deltaPhi2) ) {
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
    if( (*e_iter)->showerShapeValue(ehad1, xAOD::EgammaParameters::ehad1) ) {
      fillTH1FperRegion(m_hvEhad1,ir,ehad1);
    }
    if( (*e_iter)->showerShapeValue(ecore, xAOD::EgammaParameters::ecore) ) {
      fillTH1FperRegion(m_hvCoreEM,ir,ecore);    
    }
    if( (*e_iter)->showerShapeValue(f1, xAOD::EgammaParameters::f1) ) {
      fillTH1FperRegion(m_hvF1,ir,f1);    
    }
    if( (*e_iter)->showerShapeValue(f3, xAOD::EgammaParameters::f3) ) {
      fillTH1FperRegion(m_hvF3,ir,f3);    
    }
    if( (*e_iter)->showerShapeValue(e237, xAOD::EgammaParameters::e237) ) {
      float Re233e237 = 0.0;
      if( e237!=0 && (*e_iter)->showerShapeValue(e233, xAOD::EgammaParameters::e233) ) {
	Re233e237 = e233 / e237;
      }
      fillTH1FperRegion(m_hvRe233e237,ir,Re233e237);
      float Re237e277 = 0.0;
      if( e237!=0 && (*e_iter)->showerShapeValue(e277, xAOD::EgammaParameters::e277) ) {
	if(e277!=0) Re237e277 = e237 / e277;
      }
      fillTH1FperRegion(m_hvRe237e277,ir,Re237e277);
    }

    // Associated track details
    const xAOD::TrackParticle *t = (*e_iter)->trackParticle();
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
    const xAOD::CaloCluster *aCluster = (*e_iter)->caloCluster();
    if (aCluster) {
      float ep = 0;	
      if(trackp !=0) ep = aCluster->e()/trackp;
      fillTH1FperRegion(m_hvEoverP,ir,ep);
      double ec = aCluster->et()*cosh(aCluster->eta());
      float f2 = 0.0;
      if(ec!=0) f2 = aCluster->energyBE(2)/ec;
      fillTH1FperRegion(m_hvF2,ir,f2);    
    } else ATH_MSG_WARNING( "Can't get CaloCluster" );

    // TIGHT electrons
    bool isTight;
    if((*e_iter)->passSelection(isTight,"Tight")) { // TightPP: says menu not defined for electrons
      if(isTight) {
	n_tot_tight++;
	++(n_el_tight[ir]);
	if(m_hTightEt)     m_hTightEt->Fill(et);
	if(m_hTightEtaPhi) m_hTightEtaPhi->Fill(eta,phi);
	if(m_hTightEta)    m_hTightEta->Fill(eta);
	if(m_hTightPhi)    m_hTightPhi->Fill(phi);
	fillTH1FperRegion(m_hvTightEt,ir,et);
	fillTH1FperRegion(m_hvTightEta,ir,eta);
	fillTH1FperRegion(m_hvTightPhi,ir,phi);
	fillTH1FperRegion(m_hvTightDeltaEta1,ir,deltaEta1);
	fillTH1FperRegion(m_hvTightDeltaPhi2,ir,deltaPhi2);
	fillTH1FperRegion(m_hvTightNOfBLayerHits,ir,numberOfBLayerHits);
	fillTH1FperRegion(m_hvTightNOfSiHits,ir,numberOfPixelHits+numberOfSCTHits);
	fillTH1FperRegion(m_hvTightNOfTRTHits,ir,numberOfTRTHits);
      }
    } else {
      ATH_MSG_WARNING( "Electron selection menu Tight is not defined" );
    }
  }

  // Fill number of electrons histograms
  m_hN->Fill(n_tot);
  m_hTightN->Fill(n_tot_tight);
  for(int i=0;i<NREGION;i++) {
    fillTH1FperRegion(m_hvN,i,n_el[i]);
    fillTH1FperRegion(m_hvTightN,i,n_el_tight[i]);
  }

  return sc;
}
