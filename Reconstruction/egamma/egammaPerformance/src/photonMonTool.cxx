/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//
//      2014-05-21 Author: Remi Lafaye (Annecy) 
//      2015-02-15 Author: Bertrand LAFORGE (LPNHE Paris)
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
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "TH1F.h"
#include "TH2F.h"

photonMonTool::photonMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  :  egammaMonToolBase(type,name,parent),
     m_photonGroup(nullptr),
     m_photonConvGroup(nullptr),
     m_photonUnconvGroup(nullptr),
     m_photonIdGroup(nullptr),
     m_photonRegionGroup(nullptr),
     m_photonLBGroup(nullptr)
{
  // Name of the photon collection
  declareProperty("PhotonContainer", m_PhotonContainer = "PhotonCollection", "Name of the photon collection");

  m_CbTightPhotons = new photonHist(std::string("CbTight"));
  m_CbTightPhotons->m_lumiBlockNumber = 0;
  m_CbTightPhotons->m_nPhotonsInCurrentLB = 0;
  m_CbTightPhotons->m_nPhotonsPerLumiBlock.clear();
  m_CbTightPhotons->m_nPhotonsPerRegion.clear();
  m_CbTightPhotons->m_nPhotons=0;

  m_CbLoosePhotons = new photonHist(std::string("CbLoose"));
  m_CbLoosePhotons->m_lumiBlockNumber = 0;
  m_CbLoosePhotons->m_nPhotonsInCurrentLB = 0;
  m_CbLoosePhotons->m_nPhotonsPerLumiBlock.clear();
  m_CbLoosePhotons->m_nPhotonsPerRegion.clear();
  m_CbLoosePhotons->m_nPhotons=0;

  m_currentLB = -1;
}

photonMonTool::~photonMonTool()
{

  ATH_MSG_DEBUG("photonMonTool::~photonMontTool");
  if (m_CbLoosePhotons) delete m_CbLoosePhotons;
  ATH_MSG_DEBUG("photonMonTool ::: m_CbLoosePhotons deleted");
  if (m_CbTightPhotons) delete m_CbTightPhotons;
  ATH_MSG_DEBUG("photonMonTool ::: m_CbTightPhotons deleted");
}

StatusCode photonMonTool::bookHistogramsForOnePhotonType(photonHist& myHist)
{

  ATH_MSG_DEBUG("photonMonTool::bookHistogramsForOnePhoton()");

  int start, end;
  start = 0;
  end = ENDCAP;

  // MAIN PANEL

  // N
  std::string hname = std::string("photonN") + myHist.m_nameOfPhotonType;
  std::string hlongname =  std::string("Number of photons") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hN, *m_photonGroup, hname, hlongname, 20, 0.0, 20.0);

  // Et
  hname = std::string("photonEt") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon transverse energy [MeV]") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hEt, *m_photonGroup, hname, hlongname, 100, -1000.0, 250000.0);

  // EtaPhi
  hname = std::string("photonEtaPhiPt2.5GeV") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon #eta,#phi map (candidates with Pt>2.5 GeV)") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH2F(myHist.m_hEtaPhi, *m_photonGroup, hname, hlongname, 64, -3.2, 3.2, 64, -3.2, 3.2);

  // EtaPhi4GeV
  hname = std::string("photonEtaPhiPtgt4GeV") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon #eta,#phi map (candidates with Pt>4 GeV)") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH2F(myHist.m_hEtaPhi4GeV, *m_photonGroup, hname, hlongname, 64, -3.2, 3.2, 64, -3.2, 3.2);

  // EtaPhi20GeV
  hname = std::string("photonEtaPhiPtgt20GeV") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon #eta,#phi map (candidates with Pt>20 GeV)") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH2F(myHist.m_hEtaPhi20GeV, *m_photonGroup, hname, hlongname, 64, -3.2, 3.2, 64, -3.2, 3.2);

  // Eta
  hname = std::string("photonEta") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon #eta") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hEta, *m_photonGroup, hname, hlongname, 64, -3.2, 3.2);

  // Phi
  hname = std::string("photonPhi") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon #phi") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hPhi, *m_photonGroup, hname, hlongname, 64, -3.2, 3.2);

  // EtCone40
  hname = std::string("photonTopoEtCone40") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon Topocluster Isolation Energy") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hTopoEtCone40, *m_photonGroup, hname, hlongname, 64, -10000., 40000.);

  // PtCone20
  hname = std::string("photonPtCone20") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon Track Isolation Energy") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hPtCone20, *m_photonGroup, hname, hlongname, 64, -10000., 40000.);

  // time
  hname = std::string("photonTime") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Time associated with photon cluster [ns]") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hTime, *m_photonGroup, hname, hlongname, 90, -30., 60.);

  // RConv
  hname = std::string("photonRconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("RConv of photon [mm]") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hRConv, *m_photonGroup, hname, hlongname, 100, 0., 800.);

  ////////////////////////////////////
  // REGION PANEL
  ////////////////////////////////////

  // N
  hname = std::string("photonN") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon number ; Nel ; Nevents") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvN, *m_photonRegionGroup, hname, hlongname ,20, 0.0, 20.0,start,end);

  // Eta
  hname = std::string("photonEta") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon #eta distribution ; #eta ; Nevents") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvEta, *m_photonRegionGroup, hname, hlongname,64, -3.2, 3.2,start,end);

  // Phi
  hname = std::string("photonPhi") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon #phi distribution ; #phi ; Nevents") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvPhi, *m_photonRegionGroup, hname, hlongname, 64, -3.2, 3.2,start,end);

  // Et
  hname = std::string("photonEt") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon Et distribution ; Et [MeV] ; Nevents") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvEt, *m_photonRegionGroup, hname, hlongname, 100, -1000.0, 250000.0,start,end);

  // TopoEtCone40
  hname = std::string("photonTopoEtCone40") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon Isolation Energy TopoEtCone40 [MeV] ; Eiso ; Nevents") + std::string (" (") 
    + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvTopoEtCone40, *m_photonRegionGroup, hname, hlongname, 64, -10000., 40000.,start,end);

  // Ptcone20
  hname = std::string("photonPtCone20") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon PtCone20 distribution ; PtCone20 ; Nevents") + std::string (" (")
    + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvPtCone20, *m_photonRegionGroup, hname, hlongname, 64, -10000., 40000.,start,end);

  // Time
  hname = std::string("photonTime") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon time [ns] ; Time [ns] ; Nevents") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvTime, *m_photonRegionGroup, hname, hlongname, 90, -30.0, 60.0,start,end);

  // ConvType
  hname = std::string("photonConvType") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon conv type; Nevents") + std::string (" (")+ myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvConvType, *m_photonRegionGroup, hname, hlongname, 4,0,4,start,end);

  // ConvTrkMatch1
  hname = std::string("photonConvTrkMatch1") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon ConvTrkMatch1; Nevents") + std::string (" (")+ myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvConvTrkMatch1, *m_photonRegionGroup, hname, hlongname, 4,0,4,start,end);

  // ConvTrkMatch2
  hname = std::string("photonConvTrkMatch2") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon ConvTrkMatch2; Nevents") + std::string (" (")+ myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvConvTrkMatch2, *m_photonRegionGroup, hname, hlongname, 4,0,4,start,end);


  //////////////////////////////////
  // UNCONV PANEL
  //////////////////////////////////

  // N
  hname = std::string("photonNUnconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Number of Unconverted photons") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hNUnconv, *m_photonUnconvGroup, hname, hlongname, 20, 0.0, 20.0);

  // Et
  hname = std::string("photonEtUnconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Unconverted photon transverse energy [MeV]") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hEtUnconv, *m_photonUnconvGroup, hname, hlongname, 100, -1000.0, 250000.0);

  // EtaPhi
  hname = std::string("photonEtaPhiUnconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Unconverted photon #eta,#phi map") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH2F(myHist.m_hEtaPhiUnconv, *m_photonUnconvGroup, hname, hlongname, 64, -3.2, 3.2, 64, -3.2, 3.2);

  // Eta
  hname = std::string("photonEtaUnconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Unconverted photon #eta") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hEtaUnconv, *m_photonUnconvGroup, hname, hlongname, 64, -3.2, 3.2);

  // Phi
  hname = std::string("photonPhiUnconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Unconverted photon #phi") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hPhiUnconv, *m_photonUnconvGroup, hname, hlongname, 64, -3.2, 3.2);

  // EtCone40
  hname = std::string("photonTopoEtCone40Unconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Unconverted photon Topocluster Isolation Energy") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hTopoEtCone40Unconv, *m_photonUnconvGroup, hname, hlongname, 64, -10000., 40000.);

  // PtCone20
  hname = std::string("photonPtCone20Unconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Unconverted photon Topocluster Isolation Energy") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hPtCone20Unconv, *m_photonUnconvGroup, hname, hlongname, 64, -10000., 40000.);

  /////////////////////////////////
  // CONV PANEL 
  /////////////////////////////////

  // N
  hname = std::string("photonNConv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Number of Converted photons") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hNConv, *m_photonConvGroup, hname, hlongname, 20, 0.0, 20.0);

  // Et
  hname = std::string("photonEtConv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Converted photon transverse energy [MeV]") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hEtConv, *m_photonConvGroup, hname, hlongname, 100, -1000.0, 250000.0);

  // EtaPhi
  hname = std::string("photonEtaPhiConv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Converted photon #eta,#phi map") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH2F(myHist.m_hEtaPhiConv, *m_photonConvGroup, hname, hlongname, 64, -3.2, 3.2, 64, -3.2, 3.2);

  // Eta
  hname = std::string("photonEtaConv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Converted photon #eta") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hEtaConv, *m_photonConvGroup, hname, hlongname, 64, -3.2, 3.2);

  // Phi
  hname = std::string("photonPhiConv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Converted photon #phi") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hPhiConv, *m_photonConvGroup, hname, hlongname, 64, -3.2, 3.2);

  // EtCone40
  hname = std::string("photonTopoEtCone40Conv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Converted photon Topocluster Isolation Energy") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hTopoEtCone40Conv, *m_photonConvGroup, hname, hlongname, 64, -10000., 40000.);

  // PtCone20
  hname = std::string("photonPtCone20Conv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Converted photon Topocluster Isolation Energy") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hPtCone20Conv, *m_photonConvGroup, hname, hlongname, 64, -10000., 40000.);

  ////////////////////////////////
  // ID PANEL
  ////////////////////////////////

  hname = std::string("photonEhad1") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon energy leakage in 1st hadronic sampling ; Ehad 1; Nevents") + std::string (" (") 
    + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvEhad1, *m_photonIdGroup, hname, hlongname, 50, -1000., 10000.,start,end);

  hname = std::string("photonCoreEM") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon core energy in EM calorimeter ;E [MeV]; Nevents") + std::string (" (")
    + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvCoreEM, *m_photonIdGroup, hname, hlongname, 50, -5000., 250000.,start,end);

  hname = std::string("photonF0") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon fractional energy in PreSampler ; F0 ; Nevents") + std::string (" (") 
    + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvF0, *m_photonIdGroup, hname, hlongname, 50, -0.2,1.0,start,end);

  hname = std::string("photonF1") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon fractional energy in 1st sampling ; F1 ; Nevents") + std::string (" (") 
    + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvF1, *m_photonIdGroup, hname, hlongname, 50, -0.2,1.0,start,end);

  hname = std::string("photonF2") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon fractional energy in 2nd sampling ; F2 ; Nevents") + std::string (" (") 
    + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvF2, *m_photonIdGroup,hname, hlongname, 50, -0.2,1.0,start,end);

  hname = std::string("photonF3") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon fractional energy in 3rd sampling ; F3 ; Nevents") + std::string (" (") 
    + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvF3, *m_photonIdGroup, hname, hlongname, 50, -0.2,1.0,start,end);

  hname = std::string("photonRe233e237") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon uncor. energy fraction in 3x3/3x7 cells in em sampling 2 ;R 3x3/3x7; Nevents") 
    + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvRe233e237, *m_photonIdGroup, hname, hlongname, 50, 0., 2.,start,end);

  hname = std::string("photonRe237e277") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Photon uncor. energy fraction in 3x7/7x7 cells in em sampling 2 ;R 3x7/7x7; Nevents") 
    + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1FperRegion(myHist.m_hvRe237e277, *m_photonIdGroup, hname, hlongname, 50, 0., 2.,start,end);

  // LUMIBLOCK DEPENDANT PANEL

  hname = std::string("LBEvoNPhotons") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Number of Photons vs LB") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hLB_N, *m_photonLBGroup, hname, hlongname, 2000, -0.5, 1999.5);

  hname = std::string("LBEvoNPhotonsUnconv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Number of Unconverted Photons vs LB") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hLB_NUnconv, *m_photonLBGroup, hname, hlongname, 2000, -0.5, 1999.5);

  hname = std::string("LBEvoNPhotonsConv") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Number of Converted Photons vs LB") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hLB_NConv, *m_photonLBGroup, hname, hlongname, 2000, -0.5, 1999.5);

  hname = std::string("LBEvoConvOverN") + myHist.m_nameOfPhotonType;
  hlongname =  std::string("Fraction of converted photons vs LB") + std::string (" (") + myHist.m_nameOfPhotonType + std::string (")");
  bookTH1F(myHist.m_hLB_fConv, *m_photonLBGroup, hname, hlongname,2000, -0.5,1999.5);

  return StatusCode::SUCCESS;
}

StatusCode photonMonTool::bookHistograms()
{
  ATH_MSG_DEBUG("photonMonTool::bookHistograms()");
  // Create groups
  m_photonGroup = new MonGroup(this,"egamma/photons"+m_GroupExtension, run); // to be re-booked every new run
  // Create sub groups
  m_photonUnconvGroup = new MonGroup(this,"egamma/photons"+m_GroupExtension+"/Unconv",  run); // to be re-booked every new run
  m_photonConvGroup = new MonGroup(this,"egamma/photons"+m_GroupExtension+"/Conv",      run); // to be re-booked every new run
  m_photonIdGroup = new MonGroup(this,"egamma/photons"+m_GroupExtension+"/ID",          run); // to be re-booked every new run
  m_photonRegionGroup = new MonGroup(this,"egamma/photons"+m_GroupExtension+"/Region",  run); // to be re-booked every new run
  m_photonLBGroup = new MonGroup(this,"egamma/photons"+m_GroupExtension+"/LBMon",run, ATTRIB_X_VS_LB, "", "merge"); // to be re-booked every new run

  StatusCode sc;
  sc = bookHistogramsForOnePhotonType(*m_CbLoosePhotons);
  if(sc.isFailure()){
    ATH_MSG_VERBOSE("Could not book Histos");
    return StatusCode::FAILURE;
  } 

  sc = bookHistogramsForOnePhotonType(*m_CbTightPhotons);
  if(sc.isFailure()){
    ATH_MSG_VERBOSE("Could not book Histos");
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

StatusCode photonMonTool::fillHistogramsForOnePhoton(xAOD::PhotonContainer::const_iterator g_iter, photonHist& myHist){

  ATH_MSG_DEBUG("photonMonTool::fillHistogramsforOnePhoton()");

  float et=(*g_iter)->pt();
  float eta = (*g_iter)->eta();
  float phi = (*g_iter)->phi();      
  int ir = GetRegion(eta);
    
  //ATH_MSG_DEBUG("photon et, eta and phi " << et << " " << eta << " " << phi << " region " << ir << " defined " << myHist.m_hvEt.size());

  if (et<2500) return StatusCode::SUCCESS;

  ++myHist.m_nPhotonsPerRegion[ir];  
  ++myHist.m_nPhotons;    
  ++myHist.m_nPhotonsInCurrentLB;

  // Get conversion type 
  // Unconverted photons
  //     unconverted = 0 : unconverted photon
  // single track conversion :
  //     singleSi    = 1 : one track only, with Si hits
  //     singleTRT   = 2 : one track only, no Si hits (TRT only)
  // double track conversions  
  //     doubleSi    = 3 : two tracks, both with Si hits
  //     doubleTRT   = 4 : two tracks, none with Si hits (TRT only)
  //     doubleSiTRT = 5 : two tracks, only one with Si hits 
  xAOD::EgammaParameters::ConversionType convType = xAOD::EgammaHelpers::conversionType(*g_iter);
  fillTH1FperRegion(myHist.m_hvConvType,ir,convType);
  bool isUnconverted = (convType==xAOD::EgammaParameters::ConversionType::unconverted ? 1 : 0) ; 

  Float_t RadiusConv = 0.0;
  RadiusConv = xAOD::EgammaHelpers::conversionRadius(*g_iter);
  
  if(myHist.m_hRConv)  myHist.m_hRConv->Fill(RadiusConv);

  // Fill histograms for photons regardless convertions

  if(myHist.m_hEt)     myHist.m_hEt->Fill(et);
  if(myHist.m_hEtaPhi) myHist.m_hEtaPhi->Fill(eta,phi);
  if(myHist.m_hEtaPhi4GeV&&et>4000) myHist.m_hEtaPhi4GeV->Fill(eta,phi);  
  if(myHist.m_hEtaPhi20GeV&&et>20000) myHist.m_hEtaPhi20GeV->Fill(eta,phi);
  if(myHist.m_hEta)    myHist.m_hEta->Fill(eta);
  if(myHist.m_hPhi)    myHist.m_hPhi->Fill(phi);
  
  fillTH1FperRegion(myHist.m_hvEt,ir,et);
  fillTH1FperRegion(myHist.m_hvEta,ir,eta);
  fillTH1FperRegion(myHist.m_hvPhi,ir,phi);
  
  // Shower shape variable details
  float ehad1 = 0.0;
  float ecore = 0.0;
  float f1    = 0.0;    
  float f3    = 0.0;     
  float e233  = 0.0;
  float e237  = 0.0;
  float e277  = 0.0;
  if( (*g_iter)->showerShapeValue(ehad1, xAOD::EgammaParameters::ehad1) ) fillTH1FperRegion(myHist.m_hvEhad1,ir,ehad1);
  if( (*g_iter)->showerShapeValue(ecore, xAOD::EgammaParameters::ecore) ) fillTH1FperRegion(myHist.m_hvCoreEM,ir,ecore);    
  if( (*g_iter)->showerShapeValue(f1, xAOD::EgammaParameters::f1) )       fillTH1FperRegion(myHist.m_hvF1,ir,f1);    
  if( (*g_iter)->showerShapeValue(f3, xAOD::EgammaParameters::f3) )       fillTH1FperRegion(myHist.m_hvF3,ir,f3);    
  if( (*g_iter)->showerShapeValue(e237, xAOD::EgammaParameters::e237) ) {
    float Re233e237 = 0.0;
    if( e237!=0 && (*g_iter)->showerShapeValue(e233, xAOD::EgammaParameters::e233) ) Re233e237 = e233 / e237;
    fillTH1FperRegion(myHist.m_hvRe233e237,ir,Re233e237);
    float Re237e277 = 0.0;
    if( e237!=0 && (*g_iter)->showerShapeValue(e277, xAOD::EgammaParameters::e277) ) {
      if(e277!=0) Re237e277 = e237 / e277;
    }
    fillTH1FperRegion(myHist.m_hvRe237e277,ir,Re237e277);
  }

  // Associated cluster details
  const xAOD::CaloCluster *aCluster = (*g_iter)->caloCluster();
  if (aCluster) {
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

    float time= aCluster->time();
    myHist.m_hTime->Fill(time);

  } else ATH_MSG_WARNING( "Can't get CaloCluster" );
  
  float deltaPhi1 = 0.0;
  if( (*g_iter)->vertexCaloMatchValue(deltaPhi1, xAOD::EgammaParameters::convMatchDeltaPhi1) ) {
    fillTH1FperRegion(myHist.m_hvConvTrkMatch1,ir,deltaPhi1);
  }
  float deltaPhi2 = 0.0;
  if( (*g_iter)->vertexCaloMatchValue(deltaPhi2, xAOD::EgammaParameters::convMatchDeltaPhi2) ) {
    fillTH1FperRegion(myHist.m_hvConvTrkMatch2,ir,deltaPhi2);
  }

  // Isolation Energy 
  float topoetcone40 = 0.0;
  if ( (*g_iter)->isolationValue(topoetcone40,xAOD::Iso::topoetcone40) ) {
    myHist.m_hTopoEtCone40->Fill(topoetcone40);
  }
 
  float ptcone20 = 0.0;
  if ( (*g_iter)->isolationValue(ptcone20,xAOD::Iso::ptcone20) ) {
    myHist.m_hPtCone20->Fill(ptcone20);
  }

  fillTH1FperRegion(myHist.m_hvTopoEtCone40,ir,topoetcone40);
  fillTH1FperRegion(myHist.m_hvPtCone20,ir,ptcone20);

  // Fill Unconverted or Converted specific histograms :

  myHist.m_hLB_N->Fill(m_currentLB);

  if (isUnconverted) {

    myHist.m_hLB_NUnconv->Fill(m_currentLB);

    ++myHist.m_nPhotonsPerRegionUnconv[ir];  
    ++myHist.m_nPhotonsUnconv;    
    ++myHist.m_nPhotonsInCurrentLBUnconv;
    
    if(myHist.m_hEtUnconv)     myHist.m_hEtUnconv->Fill(et);
    if(myHist.m_hEtaPhiUnconv) myHist.m_hEtaPhiUnconv->Fill(eta,phi);
    if(myHist.m_hEtaUnconv)    myHist.m_hEtaUnconv->Fill(eta);
    if(myHist.m_hPhiUnconv)    myHist.m_hPhiUnconv->Fill(phi);
    
    // fillTH1FperRegion(myHist.m_hvEtUnconv,ir,et);
    // fillTH1FperRegion(myHist.m_hvEtaUnconv,ir,eta);
    // fillTH1FperRegion(myHist.m_hvPhiUnconv,ir,phi);
    
    // // Shower shape variable details
    // fillTH1FperRegion(myHist.m_hvEhad1Unconv,ir,ehad1);
    // fillTH1FperRegion(myHist.m_hvCoreEMUnconv,ir,ecore);    
    // fillTH1FperRegion(myHist.m_hvF1Unconv,ir,f1);    
    // fillTH1FperRegion(myHist.m_hvF3Unconv,ir,f3);    
    // fillTH1FperRegion(myHist.m_hvRe233e237Unconv,ir,Re233e237);
    // fillTH1FperRegion(myHist.m_hvRe237e277Unconv,ir,Re237e277);
    
    // // Associated cluster details
    // fillTH1FperRegion(myHist.m_hvF2Unconv,ir,f2);
    // if (myHist.m_hTimeUnconv) myHist.m_hTimeUnconv->Fill(time);
    // fillTH1FperRegion(myHist.m_hvTimeUnconv,ir,time);
    
    // Isolation Energy 
    if (myHist.m_hTopoEtCone40Unconv) myHist.m_hTopoEtCone40Unconv->Fill(topoetcone40);
    if (myHist.m_hPtCone20Unconv) myHist.m_hPtCone20Unconv->Fill(ptcone20);
    fillTH1FperRegion(myHist.m_hvTopoEtCone40Unconv,ir,topoetcone40);
    fillTH1FperRegion(myHist.m_hvPtCone20Unconv,ir,ptcone20);
    
  } else {
    // if Converted photon  

    myHist.m_hLB_NConv->Fill(m_currentLB);

    ++myHist.m_nPhotonsPerRegionConv[ir];  
    ++myHist.m_nPhotonsConv;    
    ++myHist.m_nPhotonsInCurrentLBConv;

    if(myHist.m_hEtConv)     myHist.m_hEtConv->Fill(et);
    if(myHist.m_hEtaPhiConv) myHist.m_hEtaPhiConv->Fill(eta,phi);
    if(myHist.m_hEtaConv)    myHist.m_hEtaConv->Fill(eta);
    if(myHist.m_hPhiConv)    myHist.m_hPhiConv->Fill(phi);

    // fillTH1FperRegion(myHist.m_hvEtConv,ir,et);
    // fillTH1FperRegion(myHist.m_hvEtaConv,ir,eta);
    // fillTH1FperRegion(myHist.m_hvPhiConv,ir,phi);
    
    // // Shower shape variable details
    // fillTH1FperRegion(myHist.m_hvEhad1Conv,ir,ehad1);
    // fillTH1FperRegion(myHist.m_hvCoreEMConv,ir,ecore);    
    // fillTH1FperRegion(myHist.m_hvF1Conv,ir,f1);    
    // fillTH1FperRegion(myHist.m_hvF3Conv,ir,f3);    
    // fillTH1FperRegion(myHist.m_hvRe233e237Conv,ir,Re233e237);
    // fillTH1FperRegion(myHist.m_hvRe237e277Conv,ir,Re237e277);
    
    // // Associated cluster details
    // fillTH1FperRegion(myHist.m_hvF2Conv,ir,f2);
    // if (myHist.m_hTimeConv) myHist.m_hTimeConv->Fill(time);
    // fillTH1FperRegion(myHist.m_hvTimeConv,ir,time);

    // // Conversion Trk match
    fillTH1FperRegion(myHist.m_hvConvTrkMatch1,ir,deltaPhi1);
    fillTH1FperRegion(myHist.m_hvConvTrkMatch2,ir,deltaPhi2);
    
    // Isolation Energy 
    if (myHist.m_hTopoEtCone40Conv) myHist.m_hTopoEtCone40Conv->Fill(topoetcone40);
    if (myHist.m_hPtCone20Conv) myHist.m_hPtCone20Conv->Fill(ptcone20);
    fillTH1FperRegion(myHist.m_hvTopoEtCone40Conv,ir,topoetcone40);
    fillTH1FperRegion(myHist.m_hvPtCone20Conv,ir,ptcone20);
  }
  
  return StatusCode::SUCCESS;
}


StatusCode photonMonTool::fillHistograms() {
  ATH_MSG_DEBUG("photonMonTool::fillHistograms()");
  
  if (!hasGoodTrigger("single photon")) return StatusCode::SUCCESS; 
  
  //check whether Lar signalled event bad
  if(hasBadLar()) {
    ATH_MSG_DEBUG("photonMonTool::hasBadLar()");
    return StatusCode::SUCCESS;
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

    m_CbLoosePhotons->m_nPhotonsPerLumiBlock.push_back(m_CbLoosePhotons->m_nPhotonsInCurrentLB);
    m_CbLoosePhotons->m_nPhotonsPerLumiBlockUnconv.push_back(m_CbLoosePhotons->m_nPhotonsInCurrentLBUnconv);
    m_CbLoosePhotons->m_nPhotonsPerLumiBlockConv.push_back(m_CbLoosePhotons->m_nPhotonsInCurrentLBConv);

    m_CbTightPhotons->m_nPhotonsPerLumiBlock.push_back(m_CbTightPhotons->m_nPhotonsInCurrentLB);
    m_CbTightPhotons->m_nPhotonsPerLumiBlockUnconv.push_back(m_CbTightPhotons->m_nPhotonsInCurrentLBUnconv);
    m_CbTightPhotons->m_nPhotonsPerLumiBlockConv.push_back(m_CbTightPhotons->m_nPhotonsInCurrentLBConv);

    // update content for the last lumi block
    
    // update the by LB variables
 
    double NConvPhotons = m_CbLoosePhotons->m_nPhotonsInCurrentLBConv;
    double NPhotons =  m_CbLoosePhotons->m_nPhotonsInCurrentLB;
    double fractionConv = 0.;
    if (NPhotons>0) fractionConv = NConvPhotons/NPhotons;

    //ATH_MSG_WARNING( "CommentBL fractionConv= " << fractionConv );
    
    m_CbLoosePhotons->m_hLB_fConv->Fill(m_currentLB, fractionConv);

    NConvPhotons = m_CbTightPhotons->m_nPhotonsInCurrentLBConv;
    NPhotons =  m_CbTightPhotons->m_nPhotonsInCurrentLB;
    fractionConv = 0.;
    if (NPhotons>0) fractionConv = NConvPhotons/NPhotons;
    m_CbTightPhotons->m_hLB_fConv->Fill(m_currentLB, fractionConv);

    // Reset counters
    m_CbLoosePhotons->m_nPhotonsInCurrentLB=0;
    m_CbLoosePhotons->m_nPhotonsInCurrentLBUnconv=0;
    m_CbLoosePhotons->m_nPhotonsInCurrentLBConv=0;

    m_CbTightPhotons->m_nPhotonsInCurrentLB=0;
    m_CbTightPhotons->m_nPhotonsInCurrentLBUnconv=0;
    m_CbTightPhotons->m_nPhotonsInCurrentLBConv=0;
  }

  // Get photon container
  const xAOD::PhotonContainer* photon_container=0;
  sc = m_storeGate->retrieve(photon_container, m_PhotonContainer);
  if(sc.isFailure() || !photon_container){
    ATH_MSG_VERBOSE("no photon container found in TDS");
    return StatusCode::FAILURE;
  } 

  // Check that the auxiliary store association was made successfully:
  if( ! photon_container->hasStore() ) {
    ATH_MSG_DEBUG("No auxiliary store got associated to the photon container with key: " << m_PhotonContainer);
    return StatusCode::FAILURE;
  }

  // Loop on photon container

  xAOD::PhotonContainer::const_iterator ph_iter = photon_container->begin();
  xAOD::PhotonContainer::const_iterator ph_end  = photon_container->end();
  ATH_MSG_DEBUG("Number of photons: " << photon_container->size());

  m_CbLoosePhotons->m_nPhotons       = 0;
  m_CbLoosePhotons->m_nPhotonsUnconv = 0;
  m_CbLoosePhotons->m_nPhotonsConv   = 0;

  m_CbTightPhotons->m_nPhotons       = 0;
  m_CbTightPhotons->m_nPhotonsUnconv = 0;
  m_CbTightPhotons->m_nPhotonsConv   = 0;

  m_CbLoosePhotons->m_nPhotonsPerRegion.resize(NREGION,0);
  m_CbLoosePhotons->m_nPhotonsPerRegionUnconv.resize(NREGION,0);
  m_CbLoosePhotons->m_nPhotonsPerRegionConv.resize(NREGION,0);

  m_CbTightPhotons->m_nPhotonsPerRegion.resize(NREGION,0);
  m_CbTightPhotons->m_nPhotonsPerRegionUnconv.resize(NREGION,0);
  m_CbTightPhotons->m_nPhotonsPerRegionConv.resize(NREGION,0);

  for (; ph_iter!=ph_end; ph_iter++){
    if(!(*ph_iter)) continue;
    
    // Necessary because some photons in the container have another author because reconstructed from topocluster
    if (!((*ph_iter)->author(xAOD::EgammaParameters::AuthorPhoton)|| (*ph_iter)->author(xAOD::EgammaParameters::AuthorAmbiguous))) continue;

    //reject photons outside the kinematic acceptance
    if (!((*ph_iter)->pt()> 7000. || fabs((*ph_iter)->eta()) < 2.47 )) continue;

    // Check what photon is being processed
    bool isGood;
    // CbLoose
    if((*ph_iter)->passSelection(isGood,"Loose")) { 
      if(isGood) { 
	StatusCode sc = fillHistogramsForOnePhoton(ph_iter, *m_CbLoosePhotons); 
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("couldn't book histograms");
	  return StatusCode::FAILURE;
	}
      }
    }  else ATH_MSG_WARNING( "Photon selection menu LHMedium is not defined" );

    // Cb Tight
    if((*ph_iter)->passSelection(isGood,"Tight")) {
      if(isGood) {
	StatusCode sc = fillHistogramsForOnePhoton(ph_iter, *m_CbTightPhotons);  
	if (sc.isFailure()) {
	  ATH_MSG_ERROR("couldn't book histograms");
	  return StatusCode::FAILURE;
	}
      }
    }  else ATH_MSG_WARNING( "Photon selection menu LHTight is not defined" );

  }

  // Fill number of photons histograms
  m_CbLoosePhotons->m_hN->Fill(m_CbLoosePhotons->m_nPhotons);
  m_CbLoosePhotons->m_hNUnconv->Fill(m_CbLoosePhotons->m_nPhotonsUnconv);
  m_CbLoosePhotons->m_hNConv->Fill(m_CbLoosePhotons->m_nPhotonsConv);

  m_CbTightPhotons->m_hN->Fill(m_CbTightPhotons->m_nPhotons);
  m_CbTightPhotons->m_hNUnconv->Fill(m_CbTightPhotons->m_nPhotonsUnconv);
  m_CbTightPhotons->m_hNConv->Fill(m_CbTightPhotons->m_nPhotonsConv);

  for(int i=0;i<NREGION;i++) {
    fillTH1FperRegion(m_CbLoosePhotons->m_hvN,i,m_CbLoosePhotons->m_nPhotons);
    fillTH1FperRegion(m_CbTightPhotons->m_hvN,i,m_CbTightPhotons->m_nPhotons);
  }

  return StatusCode::SUCCESS;
}


StatusCode photonMonTool::procHistograms()
{
  return StatusCode::SUCCESS;
}
