
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "CaloRec/CaloTopoClusterFromTowerMonitor.h"

#include "CaloGeoHelpers/CaloPhiRange.h"

#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "TH1D.h"
#include "TH2D.h"

#include <vector>

namespace { 
  MsgStream& operator<<(MsgStream& mstr,const SG::ReadHandleKey<xAOD::CaloClusterContainer>& ckey) { mstr << ckey.key();   return mstr; }
  MsgStream& operator<<(MsgStream& mstr,const ServiceHandle<CaloTowerGeometrySvc>&           shdl) { mstr << shdl->name(); return mstr; }
}

CaloTopoClusterFromTowerMonitor::CaloTopoClusterFromTowerMonitor(const std::string& name,ISvcLocator* pSvcLocator)
  : AthHistogramAlgorithm(name,pSvcLocator)
  , m_towerContainerKey("LCWTowerTopoClusterStd")
  , m_towerGeometrySvc("CaloTowerGeometrySvc",name)
  , m_ncBins(100)
  , m_ncMin(-0.5)
  , m_ncMax(m_ncMin+m_ncBins)
  , m_nBins(320)
  , m_nMin(-0.05)
  , m_nMax(m_nMin+m_nBins*20.)
  , m_ptBins(220)
  , m_ptMin(-10.)
  , m_ptMax(100.)
  , m_etaBins(100)
  , m_etaMin(-5.)
  , m_etaMax(5.)
  , m_phiBins(64)
  , m_phiMin(-Gaudi::Units::pi)
  , m_phiMax(Gaudi::Units::pi)
  , m_hsEtaMin(0.)
  , m_hsEtaMax(0.1)
  , m_hsPhiMin(0.)
  , m_hsPhiMax(0.1)
  , m_doGeoAutoBins(true)
  , m_doHotspot(false)
  , h_n((TH1D*)0), h_pt((TH1D*)0), h_eta((TH1D*)0), h_phi((TH1D*)0), h_nc((TH1D*)0), h_samp((TH1D*)0)
  , d_n_eta_phi((TH2D*)0), d_nc_eta_phi((TH2D*)0)
  , d_pt_eta((TH2D*)0), d_nc_eta((TH2D*)0)
  , d_wgt_samp((TH2D*)0), d_ntt_samp((TH2D*)0), d_geo_samp((TH2D*)0)
  , d_maxtowers_samp((TH2D*)0), d_wgttowers_samp((TH2D*)0) 
  , d_maxcells_eta((TH2D*)0), d_allwghts_eta((TH2D*)0)
  , d_deta_eta((TH2D*)0),  d_dphi_eta((TH2D*)0),  d_dphi_deta((TH2D*)0)
  , d_detac_eta((TH2D*)0), d_dphic_eta((TH2D*)0), d_dphic_detac((TH2D*)0), d_detac_samp((TH2D*)0), d_dphic_samp((TH2D*)0)
  , h_nc_hs((TH1D*)0), h_n_hs((TH1D*)0), h_pt_hs((TH1D*)0), h_eta_hs((TH1D*)0), h_phi_hs((TH1D*)0), h_samp_hs((TH1D*)0)
  , d_n_eta_phi_hs((TH2D*)0), d_nc_eta_phi_hs((TH2D*)0)
  , d_deta_eta_hs((TH2D*)0), d_dphi_eta_hs((TH2D*)0), d_dphi_deta_hs((TH2D*)0)
  , d_detac_eta_hs((TH2D*)0), d_dphic_eta_hs((TH2D*)0), d_dphic_detac_hs((TH2D*)0), d_detac_samp_hs((TH2D*)0), d_dphic_samp_hs((TH2D*)0) 
{
  declareProperty("CaloTowerContainerKey", m_towerContainerKey, "Input container key"     );
  declareProperty("CaloTowerGeometrySvc",  m_towerGeometrySvc,  "Tower geometry provider" );
  declareProperty("NTowerCellsBins",       m_ncBins,            "Number of bins in cell-in-tower multiplicity");
  declareProperty("NTowerCellsMin",        m_ncMin,             "Lower limit in cell-in-tower multiplicity");
  declareProperty("NTowerCellsMax",        m_ncMax,             "Upper limit in cell-in-tower multiplicity");
  declareProperty("NTowersBins",           m_nBins,             "Number of bins in tower multiplicity binning");
  declareProperty("NTowersMin",            m_nMin,              "Lower limit in tower multiplicity binning");
  declareProperty("NTowersMax",            m_nMax,              "Upper limit in tower multiplicity binning");          
  declareProperty("PtTowersBins",          m_ptBins,            "Number of bins in tower pT binning");
  declareProperty("PtTowersMin",           m_ptMin,             "Lower limit in tower pT binning");
  declareProperty("PtTowersMax",           m_ptMax,             "Upper limit in tower pT binning");          
  declareProperty("EtaTowersBins",         m_etaBins,           "Number of bins in tower rapidity binning");
  declareProperty("EtaTowersMin",          m_etaMin,            "Lower limit in tower rapidity binning");
  declareProperty("EtaTowersMax",          m_etaMax,            "Upper limit in tower rapidity binning");          
  declareProperty("PhiTowersBins",         m_phiBins,           "Number of bins in tower azimuth binning");
  declareProperty("PhiTowersMin",          m_phiMin,            "Lower limit in tower azimuth binning");
  declareProperty("PhiTowersMax",          m_phiMax,            "Upper limit in tower azimuth binning");
  declareProperty("EtaMinHotspot",         m_hsEtaMin,          "lower limit in tower rapidity for hotspot");
  declareProperty("EtaMaxHotspot",         m_hsEtaMax,          "Upper limit in tower rapidity for hotspot");
  declareProperty("PhiMinHotspot",         m_hsPhiMin,          "lower limit in tower azimuth for hotspot");
  declareProperty("PhiMaxHotspot",         m_hsPhiMax,          "Upper limit in tower azimuth for hotspot");
  declareProperty("DoGeoAutoBins",         m_doGeoAutoBins,     "Flag controls automatic binning for rapidity and azimuth");          
}

CaloTopoClusterFromTowerMonitor::~CaloTopoClusterFromTowerMonitor()
{ }

StatusCode CaloTopoClusterFromTowerMonitor::initialize()
{
  // initialize read handle key
  ATH_CHECK(m_towerContainerKey.initialize());

  // tower geometry service
  ATH_MSG_INFO( "Allocate tower geometry service:" );
  if ( !m_towerGeometrySvc.isValid() ) { 
    ATH_MSG_ERROR("[reject] - cannot allocate tower geometry service - fatal");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "[accept] - allocated tower geometry provider \042" << m_towerGeometrySvc << "\042");
  }

  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("Tower geometry service is allocated, describes %6zu towers in grid:", m_towerGeometrySvc->towerBins()) );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("[accept] %3zu eta bins in [%5.2f,%5.2f]",m_towerGeometrySvc->etaBins(),m_towerGeometrySvc->etaMin(),m_towerGeometrySvc->etaMax()) );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("[accept] %3zu phi bins in [%5.2f,%5.2f]",m_towerGeometrySvc->phiBins(),m_towerGeometrySvc->phiMin(),m_towerGeometrySvc->phiMax()) );

  std::string cfgStr;  
  if ( m_doGeoAutoBins ) { 
    ATH_MSG_INFO("Eta and phi binning taken from tower geometry provider"); 
    m_etaBins  = m_towerGeometrySvc->etaBins();
    m_etaMin   = m_towerGeometrySvc->etaMin();
    m_etaMax   = m_towerGeometrySvc->etaMax();
    m_phiBins  = m_towerGeometrySvc->phiBins();
    m_phiMin   = m_towerGeometrySvc->phiMin();
    m_phiMax   = m_towerGeometrySvc->phiMax();
    m_hsPhiMax = m_hsPhiMin+m_towerGeometrySvc->phiWidth(); 
    cfgStr = "autoconfig";
  } else {
    ATH_MSG_INFO("Eta and phi binning taken from specific configuration");
    cfgStr = "userconfig"; 
  }

  m_doHotspot = m_hsEtaMin < m_hsEtaMax &&  m_hsPhiMin < m_hsPhiMax;

  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("Eta and phi binning for histograms (total %zu towers)",m_etaBins*m_phiBins) );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("[%s] %3zu eta bins in [%5.2f,%5.2f]",cfgStr.c_str(),m_etaBins,m_etaMin,m_etaMax) );
  ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("[%s] %3zu phi bins in [%5.2f,%5.2f]",cfgStr.c_str(),m_phiBins,m_phiMin,m_phiMax) );

  ATH_CHECK( book() ); 

  // database plots
  std::vector<std::vector<double> > cWghts(m_towerGeometrySvc->towerBins(),std::vector<double>());
  size_t chash(0);
  for ( auto fLnk(m_towerGeometrySvc->begin()); fLnk != m_towerGeometrySvc->end(); ++fLnk, ++chash ) { 
    CaloSampling::CaloSample samp = CaloDetDescrManager::instance()->get_element(chash)->getSampling();
    double cs(static_cast<double>(samp));
    double nt(static_cast<double>(fLnk->size()));
    d_maxtowers_samp->Fill(cs,nt);
    for ( auto elm : *fLnk ) {
      // collect for towers
      size_t tidx(static_cast<size_t>(m_towerGeometrySvc->towerIndex(elm)));
      double weight(m_towerGeometrySvc->cellWeight(elm));
      cWghts[tidx].push_back(weight);
      // plot for smaplings
      d_wgttowers_samp->Fill(cs,weight);
    }
  }

  for ( size_t tidx(0); tidx<cWghts.size(); ++tidx ) { 
    double eta(m_towerGeometrySvc->towerEta(tidx));
    double phi(m_towerGeometrySvc->towerPhi(tidx));
    double nc(1.0*cWghts.at(tidx).size());
    d_maxcells_eta->Fill(eta,nc);
    size_t etaIdx(m_towerGeometrySvc->etaIndexFromTowerIndex(tidx));
    d_maxcells_phi_eta_slice[etaIdx]->Fill(phi,nc);
    for ( auto w : cWghts.at(tidx) ) { d_allwghts_eta->Fill(eta,w); d_allwghts_phi_eta_slice[etaIdx]->Fill(phi,w); }
  }

  if ( msgLvl(MSG::DEBUG) ) { 
    ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("+------------+--------------+") );
    ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("| SamplingId | SamplingName |") );
    ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("+------------+--------------+") );
    const auto& smap = CaloRec::Lookup::samplingNames;
    for ( auto fsamp(smap.begin()); fsamp != smap.end(); ++fsamp ) { 
      ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("|     %02i     |  %11.11s |",fsamp->first,fsamp->second.c_str()) );
    }
    ATH_MSG_DEBUG( CaloRec::Helpers::fmtMsg("+------------+--------------+") );
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloTopoClusterFromTowerMonitor::book()
{
  ////////////////////////////////////////////////////////////
  // composition, multiplicity, kinematics (event by event) //
  ////////////////////////////////////////////////////////////

  int    detaBins(100); int    dphiBins(128);
  double detaMin(-1.0); double dphiMin(-Gaudi::Units::pi/2.);
  double detaMax(1.0);  double dphiMax(Gaudi::Units::pi/2.);

  h_n    = bookAny<TH1D>("TowerMultiplicity",    "N_{tower}",          m_nBins, m_nMin, m_nMax);
  h_pt   = bookAny<TH1D>("TowerPt",              "p_{T}^{tower} [GeV]",m_ptBins,m_ptMin,m_ptMax);
  h_nc   = bookAny<TH1D>("TowerCellMultiplicity","N_{cell}^{tower}",   m_ncBins,m_ncMin,m_ncMax);
  h_eta  = bookForEta<TH1D>("TowerRapidity",     "y_{tower}");
  h_phi  = bookForPhi<TH1D>("TowerAzimuth",      "#phi_{tower} [rad]");

  h_samp = bookForSamplings<TH1D>("TowerCellSamplings","TowerCellSamplings"); setAxisTitle(h_samp,"N_{cells} in sampling","y");
  // relations (event by event)
  d_n_eta_phi  = bookAny<TH2D>("TowerMultiplicityVsEtaVsPhi",    "N_{tower}(y_{tower},#phi_{tower})",       "y_{tower}",m_etaBins,m_etaMin,m_etaMax,m_phiBins,m_phiMin,m_phiMax);
  setAxisTitle(d_n_eta_phi,"#phi_{tower} [rad]","y"); setAxisTitle(d_n_eta_phi,"N_{tower}","z");
  d_nc_eta_phi = bookAny<TH2D>("TowerCellMultiplicityVsEtaBsPhi","N_{cell}^{tower}(y_{tower},#phi_{tower})","y_{tower}",m_etaBins,m_etaMin,m_etaMax,m_phiBins,m_phiMin,m_phiMax); 
  setAxisTitle(d_nc_eta_phi,"#phi_{tower} [rad]","y"); setAxisTitle(d_nc_eta_phi,"N_{cell}^{tower}","z");
  d_pt_eta     = bookAny<TH2D>("TowerPtVsEta",                   "p_{T}^{tower}(y_{tower})",                "y_{tower}",m_etaBins,m_etaMin,m_etaMax,m_ptBins, m_ptMin, m_ptMax );
  setAxisTitle(d_pt_eta,"p_{T}^{tower} [GeV]","y");
  d_nc_eta     = bookAny<TH2D>("TowerCellMultiplicityVsEta",     "N_{cell}^{tower}(y_{tower})",             "y_{tower}",m_etaBins,m_etaMin,m_etaMax,m_ncBins, m_ncMin, m_ncMax );
  setAxisTitle(d_pt_eta,"N_{cell}^{tower} [GeV]","y");
  // distance to nominal
  d_deta_eta  = bookAny<TH2D>("TowerDeltaEtaVsEta",         "(y_{tower}-y_{tower,0})(y_{tower})",      "y_{tower}",      m_etaBins,m_etaMin,m_etaMax,detaBins,detaMin,detaMax);
  setAxisTitle(d_deta_eta,"#Deltay_{tower}","y");
  d_dphi_eta  = bookAny<TH2D>("TowerDeltaPhiVsEta",         "(#phi_{tower}-#phi_{tower,0})(y_{tower})","y_{tower}",      m_etaBins,m_etaMin,m_etaMax,dphiBins,dphiMin,dphiMax);
  setAxisTitle(d_dphi_eta,"#Delta#phi_{tower} [rad]","y");
  d_dphi_deta = bookAny<TH2D>("TowerDeltaPhiVsDeltaEta",    "#Delta#phi_{tower}(#Deltay_{tower})",     "#Deltay_{tower}",detaBins, detaMin, detaMax, dphiBins,dphiMin,dphiMax);
  setAxisTitle(d_dphi_deta,"#Delta#phi_{tower} [rad]","y");
  // cell distance to nominal
  d_detac_eta   = bookAny<TH2D>("CellDeltaEtaVsEta",         "(y_{cell #in tower}#minusy_{tower,0})(y_{tower})",             "y_{tower}",             
				m_etaBins,m_etaMin,m_etaMax,detaBins,detaMin,detaMax);
  setAxisTitle(d_detac_eta,"#Deltay_{cell #in tower}","y");
  d_dphic_eta   = bookAny<TH2D>("CellDeltaPhiVsEta",         "(#phi_{cell #in tower}^{cell}#minus#phi_{tower,0})(y_{tower})","y_{tower}",             
				m_etaBins,m_etaMin,m_etaMax,dphiBins,dphiMin,dphiMax);
  setAxisTitle(d_dphic_eta,"#Delta#phi_{cell #in tower}","y");
  d_dphic_detac = bookAny<TH2D>("CellDeltaPhiVsDeltaEta",    "#Delta#phi_{cell #in tower}^{cell}(#Deltay_{tower}^{cell})",   "#Deltay_{cell #in tower}",
				detaBins,detaMin,detaMax,dphiBins,dphiMin,dphiMax);
  setAxisTitle(d_dphic_detac,"#Delta#phi_{cell #in tower}","y");

  ///////////////////////////
  // features in samplings //
  ///////////////////////////

  int    fwbins(250);
  double fwmin(-0.1);
  double fwmax(4.9);

  int    gwbins(60);
  double gwmin(-0.1);
  double gwmax(1.1);

  int    ctbins(20);
  double ctmin(-0.5);
  double ctmax(19.5);

  // per event
  d_wgt_samp   = bookForSamplings<TH2D>("EvtFullWeightvsSampling",    "[Event] Full weight vs sampling",                  fwbins,fwmin,fwmax);
  setAxisTitle(d_wgt_samp,"w_{geo#plusLCW}","y");
  d_ntt_samp   = bookForSamplings<TH2D>("EvtNTowersPerCellvsSampling","[Event] Number of towers/cell vs sampling",        ctbins,ctmin,ctmax);
  setAxisTitle(d_ntt_samp,"N_{towers}/cell","y");
  d_geo_samp   = bookForSamplings<TH2D>("EvtGeoWeightvsSampling",     "[Event] Geometrical weight vs sampling",           gwbins,gwmin,gwmax);
  setAxisTitle(d_geo_samp,"w_{geo}","y");
  d_detac_samp = bookForSamplings<TH2D>("EvtDetaCellvsSampling",      "[Event] y_{cell #in tower}#minusy_{0}^{tower}",     detaBins,detaMin,detaMax);  
  setAxisTitle(d_detac_samp,"#Deltay_{cell #in tower}","y");
  d_dphic_samp = bookForSamplings<TH2D>("EvtDphiCellvsSampling",      "[Event] #phi_{cell #in tower}#minus#phi_{0}^{tower}",dphiBins,dphiMin,dphiMax);  
  setAxisTitle(d_dphic_samp,"#Delta#phi_{cell #in tower}","y");

  // from database
  d_maxtowers_samp = bookForSamplings<TH2D>("DBNTowersPerCellvsSampling", "[DB] Number of towers/cell vs sampling",ctbins,ctmin,ctmax);
  setAxisTitle(d_maxtowers_samp,"N_{towers}/cell (maximum sharing)","y");
  d_wgttowers_samp = bookForSamplings<TH2D>("DBGeoWeightvsSampling",      "[DB] Geometrical weight vs sampling",   gwbins,gwmin,gwmax);
  setAxisTitle(d_wgttowers_samp,"w_{geo} (from database)","y");

  /////////////////////
  // features in eta //
  /////////////////////

  int    tcbins(100);
  double tcmin(-0.5);
  double tcmax(99.5);

  d_maxcells_eta = bookForEta<TH2D>("DBNTowerCellsvsEta","[DB] Cells in towers vs y_{tower}",   tcbins,tcmin,tcmax); setAxisTitle(d_maxcells_eta,"N_{cells #in tower} (from database)","y");
  d_allwghts_eta = bookForEta<TH2D>("DBGeoWeightvsEta",  "[DB] Geometrical weight vs y_{tower}",gwbins,gwmin,gwmax); setAxisTitle(d_allwghts_eta,"w_{geo} (from database)","y");

  // analyzing the database (in tower eta slices)
  double deta((m_etaMax-m_etaMin)/(1.*m_etaBins)); 
  double eta0(m_etaMin);

  d_maxcells_phi_eta_slice.resize(m_etaBins,(TH2D*)0);
  d_allwghts_phi_eta_slice.resize(m_etaBins,(TH2D*)0);

  std::string hname; std::string htitle;
  for ( int i(0); i<m_etaBins; ++i, eta0+=deta ) { 
    hname  = CaloRec::Helpers::fmtMsg("DBTowerCellsvsPhiEta%02i",i);
    htitle = CaloRec::Helpers::fmtMsg("[DB] Cells in towers vs #phi_{tower} for y #in [%6.3f,%6.3f]",eta0,eta0+deta); 
    d_maxcells_phi_eta_slice[i] = bookForPhi<TH2D>(hname,htitle,tcbins,tcmin,tcmax); setAxisTitle(d_maxcells_phi_eta_slice.at(i),"N_{cells #in tower} (from database)","y");
    hname  = CaloRec::Helpers::fmtMsg("DBGeoWeightvsPhiEta%02i",i);
    htitle = CaloRec::Helpers::fmtMsg("[DB] Geometrical weight vs #phi_{tower} for y #in [%6.3f,%6.3f]",eta0,eta0+deta); 
    d_allwghts_phi_eta_slice[i] = bookForPhi<TH2D>(hname,htitle,gwbins,gwmin,gwmax); setAxisTitle(d_allwghts_phi_eta_slice.at(i),"w_{geo} (from database)","y");
  }

  /////////////////////////
  // debugging y=0,phi=0 //
  /////////////////////////

  if ( m_doHotspot ) { 
    h_n_hs    = bookAny<TH1D>("HSTowerMultiplicity",    "[Hotspot] N_{tower}",       "N_{tower}",          m_nBins, m_nMin, m_nMax);
    h_pt_hs   = bookAny<TH1D>("HSTowerPt",              "[Hotspot] p_{T}^{tower}",   "p_{T}^{tower} [GeV]",m_ptBins,m_ptMin,m_ptMax);
    h_nc_hs   = bookAny<TH1D>("HSTowerCellMultiplicity","[Hotspot] N_{cell}^{tower}","N_{cell}^{tower}",   m_ncBins,m_ncMin,m_ncMax);
    h_eta_hs  = bookForEta<TH1D>("HSTowerEta",          "[Hotspot] y_{tower}");    
    h_phi_hs  = bookForPhi<TH1D>("HSTowerPhi",          "[Hotspot] #phi_{tower}"); 
    
    h_samp_hs = bookForSamplings<TH1D>("HSTowerCellSampling","HSTowerCellSampling");
    
    d_n_eta_phi_hs  = bookAny<TH2D>("HSTowerMultiplicityVsEtaVsPhi",    "[Hotspot] N_{tower}(y_{tower},#phi_{tower})"        "y_{tower}",m_etaBins,m_etaMin,m_etaMax,m_phiBins,m_phiMin,m_phiMax);
    setAxisTitle(d_n_eta_phi_hs,"#phi_{tower} [rad]","y"); setAxisTitle(d_n_eta_phi_hs,"N_{tower}","z");
    d_nc_eta_phi_hs = bookAny<TH2D>("HSTowerCellMultiplicityVsEtaVsPhi","[Hotspot] N_{cell}^{tower}(y_{tower},#phi_{tower})","y_{tower}",m_etaBins,m_etaMin,m_etaMax,m_phiBins,m_phiMin,m_phiMax);
    setAxisTitle(d_nc_eta_phi_hs,"#phi_{tower} [rad]","y"); setAxisTitle(d_nc_eta_phi_hs,"N_{cell}^{tower}","z");
    d_deta_eta_hs   = bookForEta<TH2D>("HSTowerDeltaEtaVsEta",          "[Hotspot] #Deltay(y_{tower})",detaBins,detaMin,detaMax); setAxisTitle(d_deta_eta_hs,"#Deltay_{tower}","y");
    setAxisTitle(d_deta_eta_hs,"#Deltay_{tower}","y");
    d_dphi_eta_hs   = bookForEta<TH2D>("HSTowerDeltaPhiVsEta",          "[Hotspot] #Delta#phi(y_{tower})",dphiBins,dphiMin,dphiMax);
    setAxisTitle(d_dphi_eta_hs,"#Delta#phi_{tower} [rad]","y");
    d_dphi_deta_hs  = bookAny<TH2D>("HSTowerDeltaPhiVsDeltaEta",        "[Hotspot] #Delta#phi_{tower}(#Deltay_{tower})","#Deltay_{tower}",detaBins,detaMin,detaMax,dphiBins,dphiMin,dphiMax);
    setAxisTitle(d_dphi_deta_hs,"#Delta#phi_{tower} [rad]","y");
    d_detac_eta_hs   = bookForEta<TH2D>("HSCellDeltaEtaVsEta",          "[Hotspot] #Deltay_{cell #in tower}(y_{tower})",detaBins,detaMin,detaMax);
    setAxisTitle(d_detac_eta_hs,"#Deltay_{cell #in tower}","y");
    d_dphic_eta_hs   = bookForEta<TH2D>("HSCellDeltaPhiVsEta",          "[Hotspot] #Delta#phi_{cell #in tower}(y_{tower})",dphiBins,dphiMin,dphiMax);
    setAxisTitle(d_dphic_eta_hs,"#Delta#phi_{cell #in tower}","y");
    d_dphic_detac_hs = bookAny<TH2D>("HSCellDeltaPhiVsCellDeltaEta",    "[Hotspot] #Delta#phi_{cell #in tower}(#Deltay_{cell #in tower})","#Deltay_{cell #in tower}",
				      detaBins,detaMin,detaMax,dphiBins,dphiMin,dphiMax);
    setAxisTitle(d_dphic_detac_hs,"#Delta#phi_{cell #in tower}","y");
    d_detac_samp_hs = bookForSamplings<TH2D>("HSCellDeltaEtavsSampling", "[Hotspot] #Deltay_{cell #in tower} in samplings",   detaBins,detaMin,detaMax);
    setAxisTitle(d_detac_samp_hs,"#Deltay_{cell #in tower}","y");
    d_dphic_samp_hs = bookForSamplings<TH2D>("HSCelLDeltaPhivsSampling", "[Hotspot] #Delta#phi_{cell #in tower} in samplings",dphiBins,dphiMin,dphiMax);
    setAxisTitle(d_dphic_samp_hs,"#Delta#phi_{cell #in tower}","y");
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloTopoClusterFromTowerMonitor::execute()
{
  SG::ReadHandle<xAOD::CaloClusterContainer> towerHandle(m_towerContainerKey);
  if ( !towerHandle.isValid() ) { 
    ATH_MSG_ERROR( "cannot allocate cluster container with key <" << m_towerContainerKey << ">" );
    return StatusCode::FAILURE;
  }

  // fill plots
  size_t nhs(0);
  m_cellTags.reset();
  std::vector<double> deta; deta.reserve(1000);
  std::vector<double> dphi; dphi.reserve(deta.capacity()); 
  std::vector<CaloSampling::CaloSample> csam; csam.reserve(deta.capacity());
  h_n->Fill(1.0*towerHandle->size());
  for ( auto ftow(towerHandle->begin()); ftow!=towerHandle->end(); ++ftow ) { 
    const xAOD::CaloCluster* ptow = *ftow;
    // tower kinematics
    double pt(ptow->pt()/Gaudi::Units::GeV);
    double eta(ptow->eta());
    double phi(CaloPhiRange::fix(ptow->phi()));
    double nc(1.0*ptow->size());
    // tower composition and get distances
    this->fillComposition(*ptow,deta,dphi,csam);
    double deltaEta(ptow->eta()-ptow->eta0());
    double deltaPhi(CaloPhiRange::fix(ptow->phi()-ptow->phi0()));
    // inclusive plots
    h_pt->Fill(pt); h_eta->Fill(eta); h_phi->Fill(phi); h_nc->Fill(nc);
    d_n_eta_phi->Fill(eta,phi); d_nc_eta_phi->Fill(eta,phi,nc);
    d_pt_eta->Fill(eta,pt);
    d_nc_eta->Fill(eta,nc);
    d_deta_eta->Fill(eta,deltaEta);
    d_dphi_eta->Fill(eta,deltaPhi);
    d_dphi_deta->Fill(deltaEta,deltaPhi);
    for ( size_t ic(0); ic<deta.size(); ++ic ) { 
      d_detac_eta->Fill(eta,deta.at(ic));
      d_dphic_eta->Fill(eta,dphi.at(ic));
      d_dphic_detac->Fill(deta.at(ic),dphi.at(ic));
      fillSampling<TH2D>(d_detac_samp,csam.at(ic),deta.at(ic));
      fillSampling<TH2D>(d_dphic_samp,csam.at(ic),dphi.at(ic));
      fillSampling<TH1D>(h_samp,csam.at(ic));
    }
    // hot spot
    if ( this->isInHotspot(*ptow) ) {
      ++nhs;
      h_pt_hs->Fill(pt); 
      h_eta_hs->Fill(eta); 
      h_phi_hs->Fill(phi); 
      h_nc_hs->Fill(nc);
      d_n_eta_phi_hs->Fill(eta,phi);    
      d_nc_eta_phi_hs->Fill(eta,phi,nc);
      d_deta_eta_hs->Fill(eta,deltaEta);
      d_dphi_eta_hs->Fill(eta,deltaPhi);
      d_dphi_deta_hs->Fill(deltaEta,deltaPhi);
      for ( size_t ic(0); ic<deta.size(); ++ic ) { 
	d_detac_eta_hs->Fill(eta,deta.at(ic));
	d_dphic_eta_hs->Fill(eta,dphi.at(ic));
	d_dphic_detac_hs->Fill(deta.at(ic),dphi.at(ic));
	fillSampling<TH2D>(d_detac_samp_hs,csam.at(ic),deta.at(ic));
	fillSampling<TH2D>(d_dphic_samp_hs,csam.at(ic),dphi.at(ic));
	fillSampling<TH1D>(h_samp_hs,csam.at(ic));
      }
    }
    // fill in samplings
    for ( auto fCell(ptow->getCellLinks()->begin()); fCell!=ptow->getCellLinks()->end(); ++fCell ) { 
      CaloSampling::CaloSample csamp = (*fCell)->caloDDE()->getSampling();
      fillSampling<TH2D>(d_wgt_samp,csamp,fCell.weight());
      // check for cell which other tower it contributes to and plot geometrical weights (only 1/cell)
      size_t chash(static_cast<size_t>((*fCell)->caloDDE()->calo_hash()));
      if ( !m_cellTags.test(chash) ) { 
	CaloTowerGeometrySvc::elementvector_t lOfTowers = m_towerGeometrySvc->getTowers(chash);
	double ntowers(1.0*lOfTowers.size());
	fillSampling<TH2D>(d_ntt_samp,csamp,ntowers);
	for ( auto elm : lOfTowers ) { fillSampling<TH2D>(d_geo_samp,csamp,m_towerGeometrySvc->cellWeight(elm)); }
	m_cellTags.set(chash);
      }
    }
  }
  // number of towers in hot spot
  h_n_hs->Fill(1.0*nhs);

  return StatusCode::SUCCESS;
}

bool CaloTopoClusterFromTowerMonitor::fillComposition(const xAOD::CaloCluster& ptow,std::vector<double>& deta,std::vector<double>& dphi,std::vector<CaloSampling::CaloSample>& csam) const
{
  deta.clear(); dphi.clear(); csam.clear();
  for ( auto fCell(ptow.getCellLinks()->begin()); fCell != ptow.getCellLinks()->end(); ++fCell ) { 
    deta.push_back((*fCell)->eta()-ptow.eta0()); 
    dphi.push_back(CaloPhiRange::fix((*fCell)->phi()-ptow.phi0()));
    csam.push_back((*fCell)->caloDDE()->getSampling());
  }
  return !deta.empty();
}
