/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//
//      NAME:    photonMonTool.cxx
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


#include "egammaPerformance/photonMonTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "TH1F.h"
#include "TH2F.h"

photonMonTool::photonMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  :  egammaMonToolBase(type,name,parent)
{
  // Name of the photon collection
  declareProperty("PhotonContainer", m_PhotonContainer = "PhotonCollection", "Name of the photon collection");
}

photonMonTool::~photonMonTool()
{
}

StatusCode photonMonTool::bookHistograms()
{
  ATH_MSG_DEBUG("photonMonTool::bookHistograms()");
  // Create groups
  MonGroup photonGroup(this,"egamma/photons",run);     // to be re-booked every new run

  // Create sub groups
  MonGroup photonTrkGroup      (this,"egamma/photons/Track",            run); // to be re-booked every new run
  MonGroup photonIdGroup       (this,"egamma/photons/ID",               run); // to be re-booked every new run

  // MAIN PANEL
  // Number of photons
  bookTH1F(m_hN,  photonGroup,"photonN", "Number of LOOSE photons",40, 0.0, 40.0);
  bookTH1F(m_hEt, photonGroup,"photonEt", "LOOSE photon transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH1F(m_hTightN, photonGroup,"photonTightN", "Number of TIGHT photons",40, 0.0, 40.0);
  bookTH1F(m_hTightEt,photonGroup,"photonTightEt", "TIGHT photon transverse energy [MeV]",100, -1000.0, 250000.0);
  bookTH2F(m_hEtaPhi, photonGroup, "photonEtaPhi", "LOOSE photon eta,phi map", 64, -3.2, 3.2, 64, -3.2, 3.2);
  bookTH1F(m_hPhi,photonGroup,  "photonPhi",  "LOOSE photon #phi", 64, -3.2, 3.2);
  bookTH1FperRegion(m_hvEt, photonGroup,  "photonEt",   "LOOSE photon transverse energy [MeV]",  100, -1000.0, 250000.0, ENDCAP); // Don't make plots for forward photons

  // TRACK PANEL
  bookTH2F(m_hTightEtaPhi,                   photonGroup, "photonTightEtaPhi",     "TIGHT photon eta,phi map", 64, -3.2, 3.2, 64, -3.2, 3.2);
  bookTH1FperRegion(m_hvTightConvType,       photonTrkGroup, "photonTightConvType",   "TIGHT photon conv type; Nevents",4, 0, 4);
  bookTH1FperRegion(m_hvTightNOfTRTHits    , photonTrkGroup, "photonTightNOfTRTHits", "TIGHT photon number of TRT Hits ;N TRT hits;Nevents", 51,-0.5,50.5);

  // ID PANEL
  bookTH1FperRegion(m_hvEhad1,    photonIdGroup,"photonEhad1", "LOOSE photon energy leakage in 1st sampling of hadronic cal. ;E had1; Nevents", 50, -1000., 10000.);
  bookTH1FperRegion(m_hvCoreEM,   photonIdGroup,"photonCoreEM","LOOSE photon core energy in EM calorimeter ;E [MeV]; Nevents",50, -5000., 250000., ENDCAP);
  bookTH1FperRegion(m_hvF1,       photonIdGroup, "photonF1",         "LOOSE photon fractional energy in 1st sampling;F1; Nevents", 50, -1.0,1.0, ENDCAP);
  bookTH1FperRegion(m_hvF2,       photonIdGroup, "photonF2",         "LOOSE photon fractional energy in 2nd sampling;F2; Nevents", 50, -1.0,1.0, ENDCAP);
  bookTH1FperRegion(m_hvF3,       photonIdGroup, "photonF3",         "LOOSE photon fractional energy in 3rd sampling;F3; Nevents", 50, -1.0,1.0, ENDCAP);
  bookTH1FperRegion(m_hvRe233e237,photonIdGroup, "photonRe233e237",  "LOOSE photon uncor. energy fraction in 3x3/3x7 cells in em sampling 2 ;R 3x3/3x7; Nevents", 50, -4., 4., ENDCAP);
  bookTH1FperRegion(m_hvRe237e277,photonIdGroup, "photonRe237e277",  "LOOSE photon uncor. energy fraction in 3x7/7x7 cells in em sampling 2 ;R 3x7/7x7; Nevents", 50, -4., 4., ENDCAP);

  // EXPERT PER REGION PANEL
  bookTH1FperRegion(m_hvN,       photonGroup,  "photonN",    "Number of LOOSE photons",40, 0.0, 40.0);
  bookTH1FperRegion(m_hvEta,     photonGroup,  "photonEta",  "LOOSE photon #eta", 64, -3.2, 3.2, ENDCAP); // Don't make plots for forward photons
  bookTH1FperRegion(m_hvPhi,     photonGroup,  "photonPhi",  "LOOSE photon #phi", 64, -3.2, 3.2, ENDCAP); // Don't make plots for forward photons
  bookTH1FperRegion(m_hvTightN,  photonGroup,  "photonTightN",    "Number of TIGHT photons",40, 0.0, 40.0);
  bookTH1FperRegion(m_hvTightEt, photonGroup,  "photonTightEt",   "TIGHT photon transverse energy [MeV]",  100, -1000.0, 250000.0, ENDCAP); // Don't make plots for forward photons
  bookTH1FperRegion(m_hvTightEta,photonGroup,  "photonTightEta",  "TIGHT photon #eta", 64, -3.2, 3.2, ENDCAP); // Don't make plots for forward photons
  bookTH1FperRegion(m_hvTightPhi,photonGroup,  "photonTightPhi",  "TIGHT photon #phi", 64, -3.2, 3.2, ENDCAP); // Don't make plots for forward photons

  // Photon track histograms
  bookTH1FperRegion(m_hvNOfTRTHits    ,     photonTrkGroup, "photonNOfTRTHits",   "LOOSE photon number of TRT Hits ;N TRT hits;Nevents", 51,-0.5,50.5);
  bookTH1FperRegion(m_hvConvType,           photonTrkGroup, "photonConvType",     "LOOSE photon conv type; Nevents",4, 0, 4);
  bookTH1FperRegion(m_hvConvTrkMatch1,      photonTrkGroup, "photonConvTrkMatch1","LOOSE photon convTrackMatch deltaPhi1; Nevents",64, -3.2, 3.2);
  bookTH1FperRegion(m_hvConvTrkMatch1,      photonTrkGroup, "photonConvTrkMatch2","LOOSE photon convTrackMatch deltaPhi2; Nevents",64, -3.2, 3.2);
  bookTH1FperRegion(m_hvTightConvTrkMatch1, photonTrkGroup, "photonTightConvTrkMatch1","TIGHT photon convTrackMatch deltaPhi1; Nevents",64, -3.2, 3.2);
  bookTH1FperRegion(m_hvTightConvTrkMatch1, photonTrkGroup, "photonTightConvTrkMatch2","TIGHT photon convTrackMatch deltaPhi2; Nevents",64, -3.2, 3.2);

  return StatusCode::SUCCESS;
}

StatusCode photonMonTool::fillHistograms()
{
  ATH_MSG_DEBUG("photonMonTool::fillHistograms()");
  //check whether Lar signalled event bad
  if(hasBadLar()) {
    ATH_MSG_DEBUG("photonMonTool::hasBadLar()");
    return StatusCode::SUCCESS;
  }
  
  StatusCode sc;

  const xAOD::PhotonContainer* photon_container=0;
  sc = m_storeGate->retrieve(photon_container, m_PhotonContainer);
  if(sc.isFailure() || !photon_container){
    ATH_MSG_VERBOSE( "no photon container found in TDS");
    return sc;
  }

  xAOD::PhotonContainer::const_iterator g_iter = photon_container->begin();
  xAOD::PhotonContainer::const_iterator g_end  = photon_container->end();
  ATH_MSG_DEBUG("Number of photons: "<<photon_container->size());
  
  int n_tot = 0;
  int n_tot_tight = 0;
  std::vector<int> n_ph, n_ph_tight;
  n_ph.resize(NREGION,0);
  n_ph_tight.resize(NREGION,0);
  for (; g_iter!=g_end; g_iter++){
    if(!(*g_iter)) continue;
    if ((*g_iter)->author(xAOD::EgammaParameters::AuthorPhoton)==0) continue;
    float et=(*g_iter)->pt();
    float eta = (*g_iter)->eta();
    float phi = (*g_iter)->phi();      
    
    int ir = GetRegion(eta);
    
    //ATH_MSG_DEBUG("photon et, eta and phi " << et << " " << eta << " " << phi << " region " << ir << " defined " << m_hvEt.size());

    ++n_tot;
    ++(n_ph[ir]);
    if(m_hEt)     m_hEt->Fill(et);
    if(m_hEtaPhi) m_hEtaPhi->Fill(eta,phi);
    if(m_hPhi)    m_hPhi->Fill(phi);

    fillTH1FperRegion(m_hvEt,ir,et);
    fillTH1FperRegion(m_hvEta,ir,eta);
    fillTH1FperRegion(m_hvPhi,ir,phi);

    // Shower shape variable details
    float ehad1 = 0.0;
    float ecore = 0.0;
    float f1    = 0.0;    
    float f3    = 0.0;     
    float e233  = 0.0;
    float e237  = 0.0;
    float e277  = 0.0;
    if( (*g_iter)->showerShapeValue(ehad1, xAOD::EgammaParameters::ehad1) ) {
      fillTH1FperRegion(m_hvEhad1,ir,ehad1);
    }
    if( (*g_iter)->showerShapeValue(ecore, xAOD::EgammaParameters::ecore) ) {
      fillTH1FperRegion(m_hvCoreEM,ir,ecore);    
    }
    if( (*g_iter)->showerShapeValue(f1, xAOD::EgammaParameters::f1) ) {
      fillTH1FperRegion(m_hvF1,ir,f1);    
    }
    if( (*g_iter)->showerShapeValue(f3, xAOD::EgammaParameters::f3) ) {
      fillTH1FperRegion(m_hvF3,ir,f3);    
    }
    if( (*g_iter)->showerShapeValue(e237, xAOD::EgammaParameters::e237) ) {
      float Re233e237 = 0.0;
      if( e237!=0 && (*g_iter)->showerShapeValue(e233, xAOD::EgammaParameters::e233) ) {
	Re233e237 = e233 / e237;
      }
      fillTH1FperRegion(m_hvRe233e237,ir,Re233e237);
      float Re237e277 = 0.0;
      if( e237!=0 && (*g_iter)->showerShapeValue(e277, xAOD::EgammaParameters::e277) ) {
	if(e277!=0) Re237e277 = e237 / e277;
      }
      fillTH1FperRegion(m_hvRe237e277,ir,Re237e277);
    }
    // Associated cluster details
    const xAOD::CaloCluster *aCluster = (*g_iter)->caloCluster();
    if (aCluster) {
      double ec = aCluster->et()*cosh(aCluster->eta());
      float f2 = 0.0;
      if(ec!=0) f2 = aCluster->energyBE(2)/ec;
      fillTH1FperRegion(m_hvF2,ir,f2);
    } else ATH_MSG_WARNING( "Can't get CaloCluster" );
    
    xAOD::EgammaParameters::ConversionType convType = xAOD::EgammaHelpers::conversionType(*g_iter);
    fillTH1FperRegion(m_hvConvType,ir,convType);
    float deltaPhi1 = 0.0;
    if( (*g_iter)->vertexCaloMatchValue(deltaPhi1, xAOD::EgammaParameters::convMatchDeltaPhi1) ) {
      fillTH1FperRegion(m_hvConvTrkMatch1,ir,deltaPhi1);
    }
    float deltaPhi2 = 0.0;
    if( (*g_iter)->vertexCaloMatchValue(deltaPhi2, xAOD::EgammaParameters::convMatchDeltaPhi2) ) {
      fillTH1FperRegion(m_hvConvTrkMatch2,ir,deltaPhi2);
    }

    // TIGHT photons
    bool isTight;
    if((*g_iter)->passSelection(isTight,"Tight")) {
      if(isTight) {
	n_tot_tight++;
	++(n_ph_tight[ir]);
	if(m_hTightEt)     m_hTightEt->Fill(et);
	if(m_hTightEtaPhi) m_hTightEtaPhi->Fill(eta,phi);
	fillTH1FperRegion(m_hvTightEt,ir,et);
	fillTH1FperRegion(m_hvTightEta,ir,eta);
	fillTH1FperRegion(m_hvTightPhi,ir,phi);
	fillTH1FperRegion(m_hvTightConvType,ir,convType);
	fillTH1FperRegion(m_hvTightConvTrkMatch1,ir,deltaPhi1);
	fillTH1FperRegion(m_hvTightConvTrkMatch2,ir,deltaPhi2);
      }
    } else {
      ATH_MSG_WARNING( "Photon selection menu Tight is not defined" );
    }
  }

  // Fill number of photons histograms
  m_hN->Fill(n_tot);
  m_hTightN->Fill(n_tot_tight);
  for(int i=0;i<NREGION;i++) {
    fillTH1FperRegion(m_hvN,i,n_ph[i]);
    fillTH1FperRegion(m_hvTightN,i,n_ph_tight[i]);
  }
  
  return StatusCode::SUCCESS;
}
