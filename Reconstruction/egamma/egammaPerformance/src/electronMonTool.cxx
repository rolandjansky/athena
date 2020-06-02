/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//      2015-01-26 Author: Bertrand LAFORGE (LPNHE Paris)
//
//      2015-05-22 Add monitoring of LH Medium and Tight electrons, cut based Medium and Tight electrons
//                 Add LumiAware distributions
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


#include "electronMonTool.h"

electronMonTool::electronMonTool(const std::string & type, const std::string & name, const IInterface* parent) : 
  egammaMonToolBase(type,name,parent),
  m_electronGroup(nullptr),
  m_electronTrkGroup(nullptr),
  m_electronIdGroup(nullptr),
  m_electronLBGroup(nullptr)
{
  bool WithFullHistList = true;
  bool WithLimitedHistList = false;

  m_LhLooseElectrons = new electronHist(std::string("LhLoose"), WithFullHistList);
  m_LhLooseElectrons->m_lumiBlockNumber = 0;
  m_LhLooseElectrons->m_nElectronsInCurrentLB = 0;
  m_LhLooseElectrons->m_nElectronsPerLumiBlock.clear();
  m_LhLooseElectrons->m_nElectronsPerRegion.clear();
  m_LhLooseElectrons->m_nElectrons=0;

  m_LhMediumElectrons = new electronHist(std::string("LhMedium"), WithFullHistList);
  m_LhMediumElectrons->m_lumiBlockNumber = 0;
  m_LhMediumElectrons->m_nElectronsInCurrentLB = 0;
  m_LhMediumElectrons->m_nElectronsPerLumiBlock.clear();
  m_LhMediumElectrons->m_nElectronsPerRegion.clear();
  m_LhMediumElectrons->m_nElectrons=0;

  m_CbLooseElectrons = new electronHist(std::string("CbLoose"), WithLimitedHistList);
  m_CbLooseElectrons->m_lumiBlockNumber = 0;
  m_CbLooseElectrons->m_nElectronsInCurrentLB = 0;
  m_CbLooseElectrons->m_nElectronsPerLumiBlock.clear();
  m_CbLooseElectrons->m_nElectronsPerRegion.clear();
  m_CbLooseElectrons->m_nElectrons=0;

  m_LhTightElectrons = new electronHist(std::string("LhTight"), WithFullHistList);
  m_LhTightElectrons->m_lumiBlockNumber = 0;
  m_LhTightElectrons->m_nElectronsInCurrentLB = 0;
  m_LhTightElectrons->m_nElectronsPerLumiBlock.clear();
  m_LhTightElectrons->m_nElectronsPerRegion.clear();
  m_LhTightElectrons->m_nElectrons=0;

  m_CbTightElectrons = new electronHist(std::string("CbTight"), WithLimitedHistList);
  m_CbTightElectrons->m_lumiBlockNumber = 0;
  m_CbTightElectrons->m_nElectronsInCurrentLB = 0;
  m_CbTightElectrons->m_nElectronsPerLumiBlock.clear();
  m_CbTightElectrons->m_nElectronsPerRegion.clear();
  m_CbTightElectrons->m_nElectrons=0;

  m_currentLB = -1;

}

electronMonTool::~electronMonTool()
{

  delete m_CbTightElectrons;
  delete m_LhTightElectrons;
  delete m_CbLooseElectrons;
  delete m_LhMediumElectrons;
  delete m_LhLooseElectrons;

}

StatusCode electronMonTool::initialize()
{
  ATH_CHECK( egammaMonToolBase::initialize() );
  ATH_CHECK( m_ElectronContainer.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode electronMonTool::bookHistogramsForOneElectronType(electronHist& myHist)
{

  ATH_MSG_DEBUG("electronMonTool::bookHistogramsForOneElectron()");

  int start;
  int end;
  start = 0;
  end = ENDCAP;

  // MAIN PANEL
  std::string hname = std::string("electronN") + myHist.m_nameOfElectronType;
  std::string hlongname =  std::string("Number of electrons") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH1F(myHist.m_hN, *m_electronGroup, hname, hlongname, 20, 0.0, 20.0);

  hname = std::string("electronEt") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Electron transverse energy [MeV]") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH1F(myHist.m_hEt, *m_electronGroup, hname, hlongname, 100, -1000.0, 250000.0);

  hname = std::string("electronEtaPhiPtgt4GeV") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Electron #eta,#phi map (candidates with Pt>4GeV)") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH2F(myHist.m_hEtaPhi4GeV, *m_electronGroup, hname, hlongname, 64, -3.2, 3.2, 64, -3.2, 3.2);

  hname = std::string("electronEta") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Electron #eta") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH1F(myHist.m_hEta, *m_electronGroup, hname, hlongname, 64, -3.2, 3.2);

  hname = std::string("electronPhi") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Electron #phi") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH1F(myHist.m_hPhi, *m_electronGroup, hname, hlongname, 64, -3.2, 3.2);

  hname = std::string("electronTopoEtCone40") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Electron Topocluster Isolation Energy") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH1F(myHist.m_hTopoEtCone40, *m_electronGroup, hname, hlongname, 64, -10000., 40000.);

  hname = std::string("electronPtCone20") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Electron Track Isolation Energy") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH1F(myHist.m_hPtCone20, *m_electronGroup, hname, hlongname, 64, -10000., 40000.);

  hname = std::string("electronTime") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Time associated with electron cluster [ns]") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH1F(myHist.m_hTime, *m_electronGroup, hname, hlongname, 90, -30., 60.);

  // EXPERT PANEL

  hname = std::string("electronEtaPhiPtgt2.5GeV") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Electron #eta,#phi map  (candidates with Pt>2.5GeV)") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH2F(myHist.m_hEtaPhi, *m_electronGroup, hname, hlongname, 64, -3.2, 3.2, 64, -3.2, 3.2);

  hname = std::string("electronEtaPhiPtgt20GeV") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Electron #eta,#phi map (candidates with Pt>20GeV)") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH2F(myHist.m_hEtaPhi20GeV, *m_electronGroup, hname, hlongname, 64, -3.2, 3.2, 64, -3.2, 3.2);

  // PER REGION IN EXPERT PANEL

  if (myHist.m_fullHistoList) {
    
    hname = std::string("electronN") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron number ; Nel ; Nevents") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvN, *m_electronGroup, hname, hlongname ,20, 0.0, 20.0,start,end);

    hname = std::string("electronEta") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron #eta distribution ; #eta ; Nevents") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvEta, *m_electronGroup, hname, hlongname,64, -3.2, 3.2,start,end);

    hname = std::string("electronPhi") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron #phi distribution ; #phi ; Nevents") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvPhi, *m_electronGroup, hname, hlongname, 64, -3.2, 3.2,start,end);

    hname = std::string("electronEt") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron Et distribution ; Et [MeV] ; Nevents") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvEt, *m_electronGroup, hname, hlongname, 100, -1000.0, 250000.0,start,end);

    hname = std::string("electronTopoEtCone40") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron Isolation Energy TopoEtCone40 [MeV] ; Eiso ; Nevents") + std::string (" (") 
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvTopoEtCone40, *m_electronGroup, hname, hlongname, 64, -10000., 40000.,start,end);

    hname = std::string("electronPtCone20") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron PtCone20 distribution ; PtCone20 ; Nevents") + std::string (" (")
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvPtCone20, *m_electronGroup, hname, hlongname, 64, -10000., 40000.,start,end);

    hname = std::string("electronTime") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron time [ns] ; Time [ns] ; Nevents") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvTime, *m_electronGroup, hname, hlongname, 90, -30.0, 60.0,start,end);

  // TRACK PANEL
    hname = std::string("electronNOfBLayersHits") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron number of track B-Layer Hits ; N B-layer hits ; Nevents") + std::string (" (") 
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvNOfBLayerHits, *m_electronTrkGroup, hname, hlongname, 6,-0.5,5.5,start,end);

    hname = std::string("electronNOfSiHits") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron number of track precision Hits ;N Si hits;Nevents") + std::string (" (")
    + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvNOfSiHits, *m_electronTrkGroup,hname, hlongname, 26,-0.5,25.5,start,end);
    
    hname = std::string("electronNOfTRTHits") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron number of TRT Hits ;N TRT hits;Nevents") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvNOfTRTHits, *m_electronTrkGroup, hname, hlongname, 51,-0.5,50.5,start,end);
    
    hname = std::string("electronNOfTRTHighThresholdHits") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron number of High Threshold TRT Hits ;N TRT High Threshold hits;Nevents") + std::string (" (")
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvNOfTRTHighThresholdHits, *m_electronTrkGroup, hname, hlongname, 51,-0.5,50.5,start,end);
    
    hname = std::string("electronDeltaEta1") + myHist.m_nameOfElectronType;
    hlongname =  std::string("electron track match #Delta #eta (1st sampling) ;#Delta #eta;Nevents") 
      + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvDeltaEta1, *m_electronTrkGroup,hname, hlongname, 50,-0.05,0.05,start,end);
    
    hname = std::string("electronDeltaPhi2") + myHist.m_nameOfElectronType;
    hlongname =  std::string("electron track match #Delta #phi (2nd sampling) ;#Delta #phi ; Nevents") 
      + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvDeltaPhi2, *m_electronTrkGroup, hname, hlongname, 50,-0.15,0.1,start,end);
    
    hname = std::string("electronTrackd0") + myHist.m_nameOfElectronType;
    hlongname =  std::string("electron track match d0 ; d0 ; Nevents") 
      + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvd0, *m_electronTrkGroup, hname, hlongname, 100,-5.,5.,start,end);
    
    // ID PANEL
    hname = std::string("electronEhad1") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron energy leakage in 1st hadronic sampling ; Ehad 1; Nevents") + std::string (" (") 
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvEhad1, *m_electronIdGroup, hname, hlongname, 50, -1000., 10000.,start,end);
    
    hname = std::string("electronEoverP") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron match track E over P ; E/p ; Nevents") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvEoverP, *m_electronIdGroup, hname, hlongname, 50,0,5,start,end);
    
    hname = std::string("electronCoreEM") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron core energy in EM calorimeter ;E [MeV]; Nevents") + std::string (" (")
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvCoreEM, *m_electronIdGroup, hname, hlongname, 50, -5000., 250000.,start,end);
    
    hname = std::string("electronF0") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron fractional energy in PreSampler ; F0 ; Nevents") + std::string (" (") 
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvF0, *m_electronIdGroup, hname, hlongname, 50, -0.2,1.0,start,end);
    
    hname = std::string("electronF1") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron fractional energy in 1st sampling ; F1 ; Nevents") + std::string (" (") 
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvF1, *m_electronIdGroup, hname, hlongname, 50, -0.2,1.0,start,end);
    
    hname = std::string("electronF2") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron fractional energy in 2nd sampling ; F2 ; Nevents") + std::string (" (") 
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvF2, *m_electronIdGroup,hname, hlongname, 50, -0.2,1.0,start,end);
    
    hname = std::string("electronF3") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron fractional energy in 3rd sampling ; F3 ; Nevents") + std::string (" (") 
      + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvF3, *m_electronIdGroup, hname, hlongname, 50, -0.2,1.0,start,end);
    
    hname = std::string("electronRe233e237") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron uncor. energy fraction in 3x3/3x7 cells in em sampling 2 ;R 3x3/3x7; Nevents") 
      + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvRe233e237, *m_electronIdGroup, hname, hlongname, 50, 0., 2.,start,end);
    
    hname = std::string("electronRe237e277") + myHist.m_nameOfElectronType;
    hlongname =  std::string("Electron uncor. energy fraction in 3x7/7x7 cells in em sampling 2 ;R 3x7/7x7; Nevents") 
      + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
    bookTH1FperRegion(myHist.m_hvRe237e277, *m_electronIdGroup, hname, hlongname, 50, 0., 2.,start,end);

  }
  // LUMI DEPENDANT PANEL

  hname = std::string("LBEvoNElectrons") + myHist.m_nameOfElectronType;
  hlongname =  std::string("Number of Electrons vs LB") + std::string (" (") + myHist.m_nameOfElectronType + std::string (")");
  bookTH1F(myHist.m_hLB_N, *m_electronLBGroup, hname, hlongname, 2000, -0.5, 1999.5);

  return StatusCode::SUCCESS;

}


StatusCode electronMonTool::bookHistograms()
{
  ATH_MSG_DEBUG("electronMonTool::bookHistograms()");

  // Create groups
  m_electronGroup = new MonGroup(this,"egamma/electrons"+m_GroupExtension,run); // to be re-booked every new run
  // Create sub groups
  m_electronTrkGroup = new MonGroup(this,"egamma/electrons"+m_GroupExtension+"/Track",            run); // to be re-booked every new run
  m_electronIdGroup = new MonGroup(this,"egamma/electrons"+m_GroupExtension+"/ID",               run);  // to be re-booked every new run
  m_electronLBGroup = new MonGroup(this,"egamma/electrons"+m_GroupExtension+"/LBMon", run, ATTRIB_X_VS_LB, "", "merge"); // to be re-booked every new run

  StatusCode sc;
  sc = bookHistogramsForOneElectronType(*m_LhLooseElectrons);
  if(sc.isFailure()){
    ATH_MSG_VERBOSE("Could not book Histos");
    return StatusCode::FAILURE;
  } 

  sc = bookHistogramsForOneElectronType(*m_LhMediumElectrons);
  if(sc.isFailure()){
    ATH_MSG_VERBOSE("Could not book Histos");
    return StatusCode::FAILURE;
  } 

  sc = bookHistogramsForOneElectronType(*m_CbLooseElectrons);
  if(sc.isFailure()){
    ATH_MSG_VERBOSE("Could not book Histos");
    return StatusCode::FAILURE;
  } 

  sc = bookHistogramsForOneElectronType(*m_LhTightElectrons);
  if(sc.isFailure()){
    ATH_MSG_VERBOSE("Could not book Histos");
    return StatusCode::FAILURE;
  } 

  sc = bookHistogramsForOneElectronType(*m_CbTightElectrons);
  if(sc.isFailure()){
    ATH_MSG_VERBOSE("Could not book Histos");
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

StatusCode electronMonTool::fillHistogramsForOneElectron(xAOD::ElectronContainer::const_iterator e_iter, 
							 electronHist& myHist){

    // Basic kinematics
    
    float et  = (*e_iter)->pt();
    float eta = (*e_iter)->eta();
    float phi = (*e_iter)->phi();      
    int ir = GetRegion(eta);

    ATH_MSG_DEBUG("electrons et, eta and phi" << et << " " << eta << " " << phi);

    if (et<2500) return StatusCode::SUCCESS; 
    
    ++myHist.m_nElectronsPerRegion[ir];  
    ++myHist.m_nElectrons;  
  
    myHist.m_nElectronsInCurrentLB++;

    if(myHist.m_hEt)     myHist.m_hEt->Fill(et);
    if(myHist.m_hEtaPhi) myHist.m_hEtaPhi->Fill(eta,phi);
    if(myHist.m_hEtaPhi4GeV&&et>4000) myHist.m_hEtaPhi4GeV->Fill(eta,phi);
    if(myHist.m_hEtaPhi20GeV&&et>20000) myHist.m_hEtaPhi20GeV->Fill(eta,phi);
    if(myHist.m_hEta)    myHist.m_hEta->Fill(eta);
    if(myHist.m_hPhi)    myHist.m_hPhi->Fill(phi);

    myHist.m_hLB_N->Fill(m_currentLB);

    if (myHist.m_fullHistoList) {      
      fillTH1FperRegion(myHist.m_hvEt,ir,et);
      fillTH1FperRegion(myHist.m_hvEta,ir,eta);
      fillTH1FperRegion(myHist.m_hvPhi,ir,phi);
      
      // Cluster track match details
      float deltaEta1 = -999.0;
      if( (*e_iter)->trackCaloMatchValue(deltaEta1, xAOD::EgammaParameters::deltaEta1) ) {
	fillTH1FperRegion(myHist.m_hvDeltaEta1,ir,deltaEta1);
      }
      float deltaPhi2 = -999.0;
      if( (*e_iter)->trackCaloMatchValue(deltaPhi2, xAOD::EgammaParameters::deltaPhi2) ) {
	fillTH1FperRegion(myHist.m_hvDeltaPhi2,ir,deltaPhi2);
      }
    }
  
    // Isolation Energy 
    float topoetcone40;
    if( (*e_iter)->isolationValue(topoetcone40,xAOD::Iso::topoetcone40)) {
      myHist.m_hTopoEtCone40->Fill(topoetcone40);
    }
    float ptcone20;
    if( (*e_iter)->isolationValue(ptcone20,xAOD::Iso::ptcone20)) {
      myHist.m_hPtCone20->Fill(ptcone20);
    }

    if (myHist.m_fullHistoList) {
      fillTH1FperRegion(myHist.m_hvTopoEtCone40,ir,topoetcone40);
      fillTH1FperRegion(myHist.m_hvPtCone20,ir,ptcone20);
    }

    ATH_MSG_DEBUG("isolation topocone40=" << topoetcone40 << " ptcone20=" << ptcone20 );    

    // Shower shape variable details
    float ehad1 = 0.0;
    float ecore = 0.0;
    float f1    = 0.0;    
    float f3    = 0.0;     
    float e233  = 0.0;
    float e237  = 0.0;
    float e277  = 0.0;

    if (myHist.m_fullHistoList) {
      if( (*e_iter)->showerShapeValue(ehad1, xAOD::EgammaParameters::ehad1) ) {
	fillTH1FperRegion(myHist.m_hvEhad1,ir,ehad1);
      }
      if( (*e_iter)->showerShapeValue(ecore, xAOD::EgammaParameters::ecore) ) {
	fillTH1FperRegion(myHist.m_hvCoreEM,ir,ecore);    
      }
      if( (*e_iter)->showerShapeValue(f1, xAOD::EgammaParameters::f1) ) {
	fillTH1FperRegion(myHist.m_hvF1,ir,f1);    
      }
      if( (*e_iter)->showerShapeValue(f3, xAOD::EgammaParameters::f3) ) {
	fillTH1FperRegion(myHist.m_hvF3,ir,f3);    
      }
      if( (*e_iter)->showerShapeValue(e237, xAOD::EgammaParameters::e237) ) {
	float Re233e237 = 0.0;
	if( e237!=0 && (*e_iter)->showerShapeValue(e233, xAOD::EgammaParameters::e233) ) {
	  Re233e237 = e233 / e237;
	}
	fillTH1FperRegion(myHist.m_hvRe233e237,ir,Re233e237);
	float Re237e277 = 0.0;
	if( e237!=0 && (*e_iter)->showerShapeValue(e277, xAOD::EgammaParameters::e277) ) {
	  if(e277!=0) Re237e277 = e237 / e277;
	}
	fillTH1FperRegion(myHist.m_hvRe237e277,ir,Re237e277);
      }
    }

    // Associated track details
    const xAOD::TrackParticle *t = (*e_iter)->trackParticle();
    double trackp = 0; 
    unsigned char numberOfInnermostPixelLayerHits=-1;
    unsigned char numberOfPixelHits=-1;
    unsigned char numberOfSCTHits=-1;
    unsigned char numberOfTRTHits=-1;

    if (myHist.m_fullHistoList) {
      if(t) {
	trackp = t->pt()*cosh(t->eta());
	// retrieve track summary information
	if( t->summaryValue(numberOfInnermostPixelLayerHits,xAOD::numberOfInnermostPixelLayerHits) ) {
	  fillTH1FperRegion(myHist.m_hvNOfBLayerHits,ir,numberOfInnermostPixelLayerHits);
	}
	if( t->summaryValue(numberOfPixelHits,xAOD::numberOfPixelHits) && t->summaryValue(numberOfSCTHits,xAOD::numberOfSCTHits) ) {
	  fillTH1FperRegion(myHist.m_hvNOfSiHits,ir,numberOfPixelHits+numberOfSCTHits);
	}
	if( t->summaryValue(numberOfTRTHits,xAOD::numberOfTRTHits) ) {
	  fillTH1FperRegion(myHist.m_hvNOfTRTHits,ir,numberOfTRTHits);
	}
	if(t->d0()) {
	  fillTH1FperRegion(myHist.m_hvd0,ir,t->d0());
	}
      } else ATH_MSG_DEBUG( "Error attempting to retrieve associated track");
    }
    
    // Associated cluster details
    const xAOD::CaloCluster *aCluster = (*e_iter)->caloCluster();
    if (aCluster) {
      float time= aCluster->time();
      myHist.m_hTime->Fill(time);
      if (myHist.m_fullHistoList) {
	fillTH1FperRegion(myHist.m_hvTime,ir,time);
	float ep = 0;	
	if(trackp !=0) ep = aCluster->e()/trackp;
	fillTH1FperRegion(myHist.m_hvEoverP,ir,ep);
	double ec = aCluster->et()*cosh(aCluster->eta());
	float f0=0.0;
	if (ec!=0) f0 = aCluster->energyBE(0)/ec;
	fillTH1FperRegion(myHist.m_hvF0,ir,f0);    
	float f1=0.0;
	if (ec!=0) f1 = aCluster->energyBE(1)/ec;
	fillTH1FperRegion(myHist.m_hvF1,ir,f1);    
	float f2 = 0.0;
	if(ec!=0) f2 = aCluster->energyBE(2)/ec;
	fillTH1FperRegion(myHist.m_hvF2,ir,f2);    
	float f3=0.0;
	if (ec!=0) f3 = aCluster->energyBE(3)/ec;
	fillTH1FperRegion(myHist.m_hvF3,ir,f3); 
      }   
    } else ATH_MSG_WARNING("Can't get CaloCluster");
    
    return StatusCode::SUCCESS;
}

StatusCode electronMonTool::fillHistograms() {
  ATH_MSG_DEBUG("electronMonTool::fillHistograms()");
  
  if (!hasGoodTrigger("single electron")) return StatusCode::SUCCESS; 
  
  //check whether Lar signalled event bad
  if(hasBadLar()) {
    ATH_MSG_DEBUG("electronMonTool::hasBadLar()");
    return StatusCode::SUCCESS;
  }
  
  //--------------------
  //figure out current LB
  //--------------------
  unsigned int previousLB = m_currentLB;
  m_currentLB = getCurrentLB();

  //deal with the change of LB
  if (m_currentLB>previousLB) {

    // update the by LB variables
    m_LhLooseElectrons->m_nElectronsPerLumiBlock.push_back(m_LhLooseElectrons->m_nElectronsInCurrentLB);
    m_LhMediumElectrons->m_nElectronsPerLumiBlock.push_back(m_LhMediumElectrons->m_nElectronsInCurrentLB);
    m_CbLooseElectrons->m_nElectronsPerLumiBlock.push_back(m_CbLooseElectrons->m_nElectronsInCurrentLB);
    m_LhTightElectrons->m_nElectronsPerLumiBlock.push_back(m_LhTightElectrons->m_nElectronsInCurrentLB);
    m_CbTightElectrons->m_nElectronsPerLumiBlock.push_back(m_CbTightElectrons->m_nElectronsInCurrentLB);

    // Reset counters
    m_LhLooseElectrons->m_nElectronsInCurrentLB=0;
    m_LhMediumElectrons->m_nElectronsInCurrentLB=0;
    m_CbLooseElectrons->m_nElectronsInCurrentLB=0;
    m_LhTightElectrons->m_nElectronsInCurrentLB=0;
    m_CbTightElectrons->m_nElectronsInCurrentLB=0;
  }

  // Get electron container
  SG::ReadHandle<xAOD::ElectronContainer> electron_container{m_ElectronContainer};
  if(!electron_container.isValid()){
    ATH_MSG_VERBOSE("no electron container found in TDS");
    return StatusCode::FAILURE;
  } 

  // Check that the auxiliary store association was made successfully:
  if( ! electron_container->hasStore() ) {
    ATH_MSG_DEBUG("No auxiliary store got associated to the electron container with key: " << m_ElectronContainer);
    return StatusCode::FAILURE;
  }

  // Loop on electron container

  xAOD::ElectronContainer::const_iterator e_iter = electron_container->begin();
  xAOD::ElectronContainer::const_iterator e_end  = electron_container->end();
  ATH_MSG_DEBUG("Number of electrons: " << electron_container->size());

  m_LhLooseElectrons->m_nElectrons = 0;
  m_LhMediumElectrons->m_nElectrons = 0;
  m_CbLooseElectrons->m_nElectrons = 0;
  m_LhTightElectrons->m_nElectrons  = 0;
  m_CbTightElectrons->m_nElectrons  = 0;

  m_LhLooseElectrons->m_nElectronsPerRegion.resize(NREGION,0);
  m_LhMediumElectrons->m_nElectronsPerRegion.resize(NREGION,0);
  m_CbLooseElectrons->m_nElectronsPerRegion.resize(NREGION,0);
  m_LhTightElectrons->m_nElectronsPerRegion.resize(NREGION,0);
  m_CbTightElectrons->m_nElectronsPerRegion.resize(NREGION,0);

  for (; e_iter!=e_end; e_iter++){
    if(!(*e_iter)) continue;
    
    // Formally unnecessary because all electrons in the container have these authors by construction
    if (!((*e_iter)->author(xAOD::EgammaParameters::AuthorElectron)|| (*e_iter)->author(xAOD::EgammaParameters::AuthorAmbiguous))) continue;

    //reject electrons outside the kinematic acceptance
    if (!((*e_iter)->pt()> 7000. || fabs((*e_iter)->eta()) < 2.47 )) continue;

    // Check what electron is being processed
    bool isGood;
    // CbMedium
    if((*e_iter)->passSelection(isGood,"Medium")) {
      if(isGood) {
	StatusCode sc = fillHistogramsForOneElectron(e_iter, *m_CbLooseElectrons);
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("couldn't book histograms");
	  return StatusCode::FAILURE;
	}
      }
    }  else ATH_MSG_WARNING( "Electron selection menu Medium is not defined" );

    // LhLoose
    if((*e_iter)->passSelection(isGood,"LHLoose")) {
      if(isGood) {
	StatusCode sc = fillHistogramsForOneElectron(e_iter, *m_LhLooseElectrons); 
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("couldn't book histograms");
	  return StatusCode::FAILURE;
	}
      }
    }  else ATH_MSG_WARNING( "Electron selection LHLoose is not defined" );

    // LhMedium
    if((*e_iter)->passSelection(isGood,"LHMedium")) {
      if(isGood) {
	StatusCode sc = fillHistogramsForOneElectron(e_iter, *m_LhMediumElectrons); 
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("couldn't book histograms");
	  return StatusCode::FAILURE;
	}
      }
    }  else ATH_MSG_WARNING( "Electron selection LHMedium is not defined" );


    // CbTight
    if((*e_iter)->passSelection(isGood,"Tight")) { 
      if(isGood) { 
	StatusCode sc = fillHistogramsForOneElectron(e_iter, *m_CbTightElectrons); 
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("couldn't book histograms");
	  return StatusCode::FAILURE;
	}
      }
    }  else ATH_MSG_WARNING( "Electron selection menu Tight is not defined" );

    // LhTight
    if((*e_iter)->passSelection(isGood,"LHTight")) {
      if(isGood) {
	StatusCode sc = fillHistogramsForOneElectron(e_iter, *m_LhTightElectrons);  
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("couldn't book histograms");
	  return StatusCode::FAILURE;
	}
      }
    }  else ATH_MSG_WARNING( "Electron selection menu LHTight is not defined" );

  }

  // Fill number of electrons histograms
  m_CbLooseElectrons->m_hN->Fill(m_CbLooseElectrons->m_nElectrons);
  m_LhLooseElectrons->m_hN->Fill(m_LhLooseElectrons->m_nElectrons);
  m_LhMediumElectrons->m_hN->Fill(m_LhMediumElectrons->m_nElectrons);
  m_CbTightElectrons->m_hN->Fill(m_CbTightElectrons->m_nElectrons);
  m_LhTightElectrons->m_hN->Fill(m_LhTightElectrons->m_nElectrons);

  for(int i=0;i<NREGION;i++) {
    fillTH1FperRegion(m_CbLooseElectrons->m_hvN,i,m_CbLooseElectrons->m_nElectrons);
    fillTH1FperRegion(m_LhLooseElectrons->m_hvN,i,m_LhLooseElectrons->m_nElectrons);
    fillTH1FperRegion(m_LhMediumElectrons->m_hvN,i,m_LhMediumElectrons->m_nElectrons);
    fillTH1FperRegion(m_CbTightElectrons->m_hvN,i,m_CbTightElectrons->m_nElectrons);
    fillTH1FperRegion(m_LhTightElectrons->m_hvN,i,m_LhTightElectrons->m_nElectrons);
  }

  return StatusCode::SUCCESS;
}
