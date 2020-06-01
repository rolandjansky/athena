/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-10-22 Author: Remi Lafaye (Annecy) 
//      2015-03-24 Author: Bertrand LAFORGE (LPNHE Paris)
//
//      NAME:    forwardElectronMonTool.cxx
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


#include "forwardElectronMonTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "TH1F.h"
#include "TH2F.h"

forwardElectronMonTool::forwardElectronMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  :  egammaMonToolBase(type,name,parent),
     m_hN(nullptr),
     m_hEt(nullptr),
     m_hEta(nullptr),
     m_hPhi(nullptr),
     m_hEtaPhi(nullptr),
     m_hTime(nullptr),
     m_hTightN(nullptr),
     m_hTightEt(nullptr),
     m_hTightEta(nullptr),
     m_hTightPhi(nullptr),
     m_hTightEtaPhi(nullptr),
     m_hTightTime(nullptr),
     m_hLB_N(nullptr),
     m_nForwardElectrons(0)
{
  m_lumiBlockNumber = 0;
  m_nForwardElectronsInCurrentLB = 0;
  m_nForwardElectronsPerLumiBlock.clear();

}

forwardElectronMonTool::~forwardElectronMonTool()
{
}

StatusCode forwardElectronMonTool::initialize()
{
  ATH_CHECK( egammaMonToolBase::initialize() );
  ATH_CHECK( m_ForwardElectronContainer.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode forwardElectronMonTool::bookHistograms()
{
  ATH_MSG_DEBUG("forwardElectronMonTool::bookHistograms()");
  int start;
  int end;
  start = ENDCAP;
  end = FORWARD;

  // Create groups
  MonGroup electronGroup(this,"egamma/forwardElectrons",run); // to be re-booked every new run
  
  // Create sub groups
  MonGroup electronIdGroup       (this,"egamma/forwardElectrons/ID",               run); // to be re-booked every new run
  MonGroup electronLBGroup       (this,"egamma/forwardElectrons/LBMON",run, ATTRIB_X_VS_LB, "", "merge"); // to be re-booked every new run

  // MAIN PANEL
  bookTH1F(m_hN,            electronGroup, "forwardElectronN",           "Number of LOOSE electrons",40, 0.0, 40.0);
  bookTH1F(m_hEt,           electronGroup, "forwardElectronEt",          "LOOSE electron transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hEtaPhi,       electronGroup, "forwardElectronEtaPhi",      "LOOSE electron #eta,#phi map (candidates with E>10GeV)", 64, -3.2, 3.2, 64, -3.2, 3.2);
  bookTH1F(m_hEta,          electronGroup, "forwardElectronEta",         "LOOSE electron #eta", 64, -5., 5.);
  bookTH1F(m_hPhi,          electronGroup, "forwardElectronPhi",         "LOOSE electron #phi", 64, -3.2, 3.2);
  //bookTH1F(m_hTopoEtCone40, electronGroup, "forwardElectronTopoEtCone40","LOOSE forward electron Isolation Energy TopoEtCone40", 64, -10000., 40000.);
  bookTH1F(m_hTime,         electronGroup, "forwardElectronTime",        "Time associated with the LOOSE electron cluster [ns]", 90, -30., 60.);

  bookTH1F(m_hTightN,            electronGroup, "forwardElectronTightN",           "Number of TIGHT electrons",40, 0.0, 40.0);
  bookTH1F(m_hTightEt,           electronGroup, "forwardElectronTightEt",          "TIGHT electron transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hTightEtaPhi,       electronGroup, "forwardElectronTightEtaPhi",      "TIGHT electron #eta,#phi map (candidates with E>10GeV)", 64, -5., 5., 64, -3.2, 3.2);
  bookTH1F(m_hTightEta,          electronGroup, "forwardElectronTightEta",         "TIGHT electron #eta", 100, -5., 5.);
  bookTH1F(m_hTightPhi,          electronGroup, "forwardElectronTightPhi",         "TIGHT electron #phi", 64, -3.2, 3.2);
  //bookTH1F(m_hTightTopoEtCone40, electronGroup, "forwardElectronTightTopoEtCone40","TIGHT forward electron Isolation Energy TopoEtCone40", 64, -10000., 40000.);
  bookTH1F(m_hTightTime,         electronGroup, "forwardElectronTightTime",        "Time associated with the TIGHT electron cluster [ns]", 90, -30., 60.);

  // ID PANEL
  bookTH1FperRegion(m_hvDensity, electronIdGroup, "forwardElectronENG_DENS",      "First Moment of Energy Density ;FistEngDens; Nevents", 200,0.,2.,    start,end);
  bookTH1FperRegion(m_hvFrac,    electronIdGroup, "forwardElectronFRAC_MAX",      "Fraction of most energetic cell ;FacMax;Nevents",       50,0.,1.,    start,end);
  bookTH1FperRegion(m_hvLongitu, electronIdGroup, "forwardElectronLONGITUDINAL",  "Normalized Longitudinal Moment ;Longitudinal;Nevents", 100,0.,1.,    start,end);
  bookTH1FperRegion(m_hvLambda,  electronIdGroup, "forwardElectron2ND_LAMBDA",    "Second Moment of Lambda ;SecondLambda;Nevents",        500,0.,10000.,start,end);
  bookTH1FperRegion(m_hvLateral, electronIdGroup, "forwardElectronLATERAL",       "Normalized Lateral Moment ;Lateral;Nevents",           100,0.,1.,    start,end);
  bookTH1FperRegion(m_hvSecondR, electronIdGroup, "forwardElectron2ND_R",         "Second Moment of R ;SecondR;Nevents",                  500,0.,20000.,start,end);
  bookTH1FperRegion(m_hvCenterL, electronIdGroup, "forwardElectronCENTER_LAMBDA", "distance of shower center from calo front face;CenterLambda;Nevents",500,0,2000,start,end);

  bookTH1FperRegion(m_hvTightDensity, electronIdGroup, "forwardElectronTightENG_DENS",      "First Moment of Energy Density ;FistEngDens; Nevents", 200,0.,2.,    start,end);
  bookTH1FperRegion(m_hvTightFrac,    electronIdGroup, "forwardElectronTightFRAC_MAX",      "Fraction of most energetic cell ;FacMax;Nevents",       50,0.,1.,    start,end);
  bookTH1FperRegion(m_hvTightLongitu, electronIdGroup, "forwardElectronTightLONGITUDINAL",  "Normalized Longitudinal Moment ;Longitudinal;Nevents", 100,0.,1.,    start,end);
  bookTH1FperRegion(m_hvTightLambda,  electronIdGroup, "forwardElectronTight2ND_LAMBDA",    "Second Moment of Lambda ;SecondLambda;Nevents",        500,0.,10000.,start,end);
  bookTH1FperRegion(m_hvTightLateral, electronIdGroup, "forwardElectronTightLATERAL",       "Normalized Lateral Moment ;Lateral;Nevents",           100,0.,1.,    start,end);
  bookTH1FperRegion(m_hvTightSecondR, electronIdGroup, "forwardElectronTight2ND_R",         "Second Moment of R ;SecondR;Nevents",                  500,0.,20000.,start,end);
  bookTH1FperRegion(m_hvTightCenterL, electronIdGroup, "forwardElectronTightCENTER_LAMBDA", "distance of shower center from calo front face;CenterLambda;Nevents",500,0,2000,start,end);


  // EXPERT PER REGION PANEL
  bookTH1FperRegion(m_hvN,       electronGroup, "forwardElectronN",  "Number of LOOSE electrons",40, 0.0, 40.0,start,end);
  bookTH1FperRegion(m_hvEt,      electronGroup, "forwardElectronEt", "LOOSE electron transverse energy [MeV]",100, -1000.0, 250000.0,start,end);
  bookTH1FperRegion(m_hvEta,     electronGroup, "forwardElectronEta","LOOSE electron #eta",64, -5., 5.,start,end);
  bookTH1FperRegion(m_hvPhi,     electronGroup, "forwardElectronPhi","LOOSE electron #phi",64, -3.2, 3.2,start,end);
  //bookTH1FperRegion(m_hvTopoEtCone40, electronGroup,"forwardElectronTopoEtCone40", "LOOSE Forward electron Isolation Energy TopoEtCone40 [MeV]", 64, -10000., 40000.,start,end);
  bookTH1FperRegion(m_hvTime,electronGroup,"forwardElectronTime", "LOOSE electron time [ns]",90, -30.0, 60.0,start,end);

  bookTH1FperRegion(m_hvTightN,    electronGroup,"forwardElectronTightN",  "Number of TIGHT electrons",40, 0.0, 40.0,start,end);
  bookTH1FperRegion(m_hvTightEt,   electronGroup,"forwardElectronTightEt", "TIGHT electron transverse energy [MeV]",100, -1000.0, 250000.0,start,end);
  bookTH1FperRegion(m_hvTightEta,  electronGroup,"forwardElectronTightEta","TIGHT electron #eta",100, -5., 5.,start,end);
  bookTH1FperRegion(m_hvTightPhi,  electronGroup,"forwardElectronTightPhi","TIGHT electron #phi",64, -3.2, 3.2,start,end);
  //bookTH1FperRegion(m_hvTightTopoEtCone40, electronGroup,"forwardElectronTightTopoEtCone40", "TIGHT Forward electron Isolation Energy TopoEtCone40 [MeV]", 64, -10000., 40000.,start,end);
  bookTH1FperRegion(m_hvTightTime, electronGroup,"forwardElectronTightTime", "TIGHT electron time [ns]",90, -30.0, 60.0,start,end);

  // LB MONITORING PANEL
  bookTH1F(m_hLB_N, electronLBGroup, "forwardElectronNumbervsLB", "Number of Forward Electrons vs LB", 2000, -0.5, 1999.5);

  return StatusCode::SUCCESS;
}

StatusCode forwardElectronMonTool::fillHistograms()
{
  ATH_MSG_DEBUG("forwardElectronMonTool::fillHistograms()");

  if (!hasGoodTrigger("forward electron")) return StatusCode::SUCCESS; 

  //check whether Lar signalled event bad

  if(hasBadLar()) {
    ATH_MSG_DEBUG("forwardElectronMonTool::hasBadLar()");
    return StatusCode::RECOVERABLE;
  }
  
  //--------------------
  //figure out current LB
  //--------------------
  unsigned int previousLB = m_currentLB;
  m_currentLB = getCurrentLB();

  //deal with the change of LB
  if (m_currentLB>previousLB) {
    // update the by LB variables
    m_nForwardElectronsPerLumiBlock.push_back(m_nForwardElectronsInCurrentLB);
    // Reset counters
    m_nForwardElectronsInCurrentLB=0;
  }

  // Get electron container
  SG::ReadHandle<xAOD::ElectronContainer> electron_container{m_ForwardElectronContainer};
  if(!electron_container.isValid()){
    ATH_MSG_VERBOSE("no electron container found in TDS");
    return StatusCode::FAILURE;
  } 

  xAOD::ElectronContainer::const_iterator e_iter = electron_container->begin();
  xAOD::ElectronContainer::const_iterator e_end  = electron_container->end();

  ATH_MSG_DEBUG("Number of electrons: " << electron_container->size());

  // Check that the auxiliary store association was made successfully:
  if( ! electron_container->hasStore() ) {
    ATH_MSG_DEBUG("No auxiliary store got associated to the electron container with key: " << m_ForwardElectronContainer);
    return StatusCode::FAILURE;
  }

  //

  int n_tot = 0;
  int n_tot_tight = 0;
  std::vector<int> n_el;
  std::vector<int> n_el_tight;
  n_el.resize(NREGION,0);
  n_el_tight.resize(NREGION,0);

  for (; e_iter!=e_end; e_iter++) {
    if(!(*e_iter)) continue;
    if((*e_iter)->author(xAOD::EgammaParameters::AuthorFwdElectron)==0) continue;

    // Basic kinematics
    float et  = (*e_iter)->pt();
    float eta = (*e_iter)->eta();
    float phi = (*e_iter)->phi();      
    int ir = GetForwardRegion(eta);
    //ATH_MSG_DEBUG("electrons et, eta and phi" << et << " " << eta << " " << phi);
  
    if (et<5000) return StatusCode::SUCCESS;

    // Isolation Energy 
    //float topoetcone40 = -999.;
    // Shower shape variable details
    double firstENGdens=-999.;
    double lateral=-999.;
    double secondR=-999.;
    double centerLambda=-999.;
    double fracMax=-999.;
    double longitudinal=-999.;
    double secondLambda=-999.;
    double time= -999.;

    ATH_MSG_DEBUG( "Found one Forward Electron..." );

    //    bool isLoose;
    // if ((*e_iter)->passSelection(isLoose,"FWDLoose")) { // cut based Loose Forward electrons
    //  if (isLoose) {

    ATH_MSG_DEBUG( "this electron is also a Loose Forward Electron..." );
    
    ++n_tot;
    ++(n_el[ir]);
    if(m_hEt)     m_hEt->Fill(et);
    if(m_hEtaPhi&&et>10000.) m_hEtaPhi->Fill(eta,phi);
    if(m_hEta)    m_hEta->Fill(eta);
    if(m_hPhi)    m_hPhi->Fill(phi);
    
    m_hLB_N->Fill(m_currentLB);

    fillTH1FperRegion(m_hvEt,ir,et);
    fillTH1FperRegion(m_hvEta,ir,eta);
    fillTH1FperRegion(m_hvPhi,ir,phi);
    
    // Isolation Energy 
    //if( (*e_iter)->isolationValue(topoetcone40,xAOD::Iso::topoetcone40)) {
    //  if (m_hTopoEtCone40) m_hTopoEtCone40->Fill(topoetcone40);
    //}
    //fillTH1FperRegion(m_hvTopoEtCone40,ir,topoetcone40);
    
    // Associated cluster details
    const xAOD::CaloCluster *aCluster = (*e_iter)->caloCluster();
    
    if (aCluster) {
      // loop on moments
      if( aCluster->retrieveMoment( xAOD::CaloCluster::FIRST_ENG_DENS,firstENGdens) )
	fillTH1FperRegion(m_hvDensity,ir,firstENGdens);
      if( aCluster->retrieveMoment( xAOD::CaloCluster::ENG_FRAC_MAX, fracMax) )
	fillTH1FperRegion(m_hvFrac,ir,fracMax);
      if( aCluster->retrieveMoment( xAOD::CaloCluster::LONGITUDINAL, longitudinal) )
	fillTH1FperRegion(m_hvLongitu,ir,longitudinal);
      if( aCluster->retrieveMoment( xAOD::CaloCluster::SECOND_LAMBDA,secondLambda) )
	fillTH1FperRegion(m_hvLambda,ir,secondLambda);
      if( aCluster->retrieveMoment( xAOD::CaloCluster::LATERAL, lateral) )
	fillTH1FperRegion(m_hvLateral,ir,lateral);
      if( aCluster->retrieveMoment( xAOD::CaloCluster::SECOND_R, secondR) )
	fillTH1FperRegion(m_hvSecondR,ir,secondR);
      if( aCluster->retrieveMoment( xAOD::CaloCluster::CENTER_LAMBDA, centerLambda) )
	fillTH1FperRegion(m_hvCenterL,ir,centerLambda);
      time = aCluster->time();
      if (m_hTime) m_hTime->Fill(time);
      fillTH1FperRegion(m_hvTime,ir,time);
    }
    
    // TIGHT electrons
    bool isTight;
    if((*e_iter)->passSelection(isTight,"Tight")) { // Tight Forward electrons
      if(isTight) {
	ATH_MSG_DEBUG( "this electron is also a Tight Forward Electron" );
	n_tot_tight++;
	++(n_el_tight[ir]);
	if(m_hTightEt)     m_hTightEt->Fill(et);
	if(m_hTightEtaPhi&&et>10000) m_hTightEtaPhi->Fill(eta,phi);
	if(m_hTightEta)    m_hTightEta->Fill(eta);
	if(m_hTightPhi)    m_hTightPhi->Fill(phi);
	if (m_hTightTime)  m_hTightTime->Fill(time);
	//if (m_hTightTopoEtCone40) m_hTightTopoEtCone40->Fill(topoetcone40);
	
	fillTH1FperRegion(m_hvTightEt,ir,et);
	fillTH1FperRegion(m_hvTightEta,ir,eta);
	fillTH1FperRegion(m_hvTightPhi,ir,phi);
	
	fillTH1FperRegion(m_hvTightDensity,ir,firstENGdens);
	fillTH1FperRegion(m_hvTightFrac,ir,fracMax);
	fillTH1FperRegion(m_hvTightLongitu,ir,longitudinal);
	fillTH1FperRegion(m_hvTightLambda,ir,secondLambda);
	fillTH1FperRegion(m_hvTightLateral,ir,lateral);
	fillTH1FperRegion(m_hvTightSecondR,ir,secondR);
	fillTH1FperRegion(m_hvTightCenterL,ir,centerLambda);
	fillTH1FperRegion(m_hvTightTime,ir,time);
	//fillTH1FperRegion(m_hvTightTopoEtCone40,ir,topoetcone40);
      }
    } else {
      ATH_MSG_DEBUG( "Electron selection menu Tight is not defined" );
    }
    
    // Fill number of electrons histograms
    if (m_hN) m_hN->Fill(n_tot);
    if (m_hTightN) m_hTightN->Fill(n_tot_tight);
    for(int i=0;i<NREGION;i++) {
      fillTH1FperRegion(m_hvN,i,n_el[i]);
      fillTH1FperRegion(m_hvTightN,i,n_el_tight[i]);
    }
    //} else {
    //ATH_MSG_DEBUG( "Electron selection menu Loose is not defined" );
    //}
  }
  
  return StatusCode::SUCCESS;
}
