/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-10-22 Author: Remi Lafaye (Annecy) 
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


#include "egammaPerformance/forwardElectronMonTool.h"
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
  :  egammaMonToolBase(type,name,parent)
{
  // Name of the electron collection
  declareProperty("ForwardElectronContainer", m_ForwardElectronContainer = "egammaForwardCollection", "Name of the forward electron collection" );
}

forwardElectronMonTool::~forwardElectronMonTool()
{
}

StatusCode forwardElectronMonTool::bookHistograms()
{
  ATH_MSG_DEBUG("forwardElectronMonTool::bookHistograms()");
  int start, end;
  start = ENDCAP;
  end = FORWARD;

  // Create groups
  MonGroup electronGroup(this,"egamma/forwardElectrons",run); // to be re-booked every new run
  
  // Create sub groups
  MonGroup electronIdGroup       (this,"egamma/forwardElectrons/ID",               run); // to be re-booked every new run

  // MAIN PANEL
  bookTH1F(m_hN,          electronGroup, "forwardElectronN",          "Number of LOOSE electrons",40, 0.0, 40.0);
  bookTH1F(m_hEt,         electronGroup, "forwardElectronEt",         "LOOSE electron transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hEtaPhi,     electronGroup, "forwardElectronEtaPhi",     "LOOSE electron #eta,#phi map", 64, -3.2, 3.2, 64, -3.2, 3.2);
  bookTH1F(m_hEta,        electronGroup, "forwardElectronEta",        "LOOSE electron #eta", 64, -3.2, 3.2);
  bookTH1F(m_hPhi,        electronGroup, "forwardElectronPhi",        "LOOSE electron #phi", 64, -3.2, 3.2);
  bookTH1F(m_hTightN,     electronGroup, "forwardElectronTightN",     "Number of TIGHT electrons",40, 0.0, 40.0);
  bookTH1F(m_hTightEt,    electronGroup, "forwardElectronTightEt",    "TIGHT electron transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hTightEtaPhi,electronGroup, "forwardElectronTightEtaPhi","TIGHT electron #eta,#phi map", 64, -3.2, 3.2, 64, -3.2, 3.2);
  bookTH1F(m_hTightEta,   electronGroup, "forwardElectronTightEta",   "TIGHT electron #eta", 64, -3.2, 3.2);
  bookTH1F(m_hTightPhi,   electronGroup, "forwardElectronTightPhi",   "TIGHT electron #phi", 64, -3.2, 3.2);
  bookTH1FperRegion(m_hvEt,electronGroup,"forwardElectronEt", "LOOSE electron transverse energy [MeV]",100, -1000.0, 250000.0,start,end);

  // ID PANEL
  bookTH1FperRegion(m_hvDensity, electronIdGroup, "forwardElectronENG_DENS",      "First Moment of Energy Density ;FistEngDens; Nevents", 200,0.,2.,    start,end);
  bookTH1FperRegion(m_hvFrac,    electronIdGroup, "forwardElectronFRAC_MAX",      "Fraction of most energetic cell ;FacMax;Nevents",       50,0.,1.,    start,end);
  bookTH1FperRegion(m_hvLongitu, electronIdGroup, "forwardElectronLONGITUDINAL",  "Normalized Longitudinal Moment ;Longitudinal;Nevents", 100,0.,1.,    start,end);
  bookTH1FperRegion(m_hvLambda,  electronIdGroup, "forwardElectron2ND_LAMBDA",    "Second Moment of Lambda ;SecondLambda;Nevents",        500,0.,10000.,start,end);
  bookTH1FperRegion(m_hvLateral, electronIdGroup, "forwardElectronLATERAL",       "Normalized Lateral Moment ;Lateral;Nevents",           100,0.,1.,    start,end);
  bookTH1FperRegion(m_hvSecondR, electronIdGroup, "forwardElectron2ND_R",         "Second Moment of R ;SecondR;Nevents",                  500,0.,20000.,start,end);
  bookTH1FperRegion(m_hvCenterL, electronIdGroup, "forwardElectronCENTER_LAMBDA", "distance of shower center from calo front face;CenterLambda;Nevents",500,0,2000,start,end);

  // EXPERT PER REGION PANEL
  bookTH1FperRegion(m_hvN,       electronGroup,"forwardElectronN",  "Number of LOOSE electrons",40, 0.0, 40.0,start,end);
  bookTH1FperRegion(m_hvEta,     electronGroup,"forwardElectronEta","LOOSE electron #eta",64, -3.2, 3.2,start,end);
  bookTH1FperRegion(m_hvPhi,     electronGroup,"forwardElectronPhi","LOOSE electron #phi",64, -3.2, 3.2,start,end);
  bookTH1FperRegion(m_hvTightN,  electronGroup,"forwardElectronTightN",  "Number of TIGHT electrons",40, 0.0, 40.0,start,end);
  bookTH1FperRegion(m_hvTightEt, electronGroup,"forwardElectronTightEt", "TIGHT electron transverse energy [MeV]",100, -1000.0, 250000.0,start,end);
  bookTH1FperRegion(m_hvTightEta,electronGroup,"forwardElectronTightEta","TIGHT electron #eta",64, -3.2, 3.2,start,end);
  bookTH1FperRegion(m_hvTightPhi,electronGroup,"forwardElectronTightPhi","TIGHT electron #phi",64, -3.2, 3.2,start,end);

  return StatusCode::SUCCESS;
}

StatusCode forwardElectronMonTool::fillHistograms()
{
  ATH_MSG_DEBUG("forwardElectronMonTool::fillHistograms()");
  //check whether Lar signalled event bad
  if(hasBadLar()) {
    ATH_MSG_DEBUG("forwardElectronMonTool::hasBadLar()");
    return StatusCode::SUCCESS;
  }
  
  StatusCode sc;

  // Get electron container
  const xAOD::ElectronContainer* electron_container=0;
  sc = m_storeGate->retrieve(electron_container, m_ForwardElectronContainer);
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
    if((*e_iter)->author(xAOD::EgammaParameters::AuthorFwdElectron)==0) continue;

    // Basic kinematics
    float et  = (*e_iter)->pt();
    float eta = (*e_iter)->eta();
    float phi = (*e_iter)->phi();      
    int ir = GetForwardRegion(eta);
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
        
    // Associated cluster details
    const xAOD::CaloCluster *aCluster = (*e_iter)->caloCluster();
    if (aCluster) {
      // Shower shape variable details
      double firstENGdens=-999.;
      double lateral=-999.;
      double secondR=-999.;
      double centerLambda=-999.;
      double fracMax=-999.;
      double longitudinal=-999.;
      double secondLambda=-999.;
      // loop on moments
      if( aCluster->retrieveMoment( xAOD::CaloCluster::FIRST_ENG_DENS,firstENGdens) )
	fillTH1FperRegion(m_hvDensity,ir,firstENGdens);
	firstENGdens = -999;
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
    }

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
