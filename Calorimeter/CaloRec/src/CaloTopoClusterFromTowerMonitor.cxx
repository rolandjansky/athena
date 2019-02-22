
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "CaloRec/CaloTopoClusterFromTowerMonitor.h"
#include "CaloRec/CaloTopoClusterFromTowerHelpers.h"
#include "CaloGeoHelpers/CaloPhiRange.h"

#include "TH1D.h"
#include "TH2D.h"

namespace { 
  constexpr auto _pi = Gaudi::Units::pi;
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
  , m_ptBins(200)
  , m_ptMin(0.)
  , m_ptMax(100.)
  , m_etaBins(100)
  , m_etaMin(-5.)
  , m_etaMax(5.)
  , m_phiBins(64)
  , m_phiMin(-_pi)
  , m_phiMax(_pi)
  , m_doGeoAutoBins(true)
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
  
  if ( m_doGeoAutoBins ) { 
    ATH_MSG_INFO("Eta and phi binning taken from tower geometry provider"); 
    m_etaBins = m_towerGeometrySvc->etaBins();
    m_etaMin  = m_towerGeometrySvc->etaMin();
    m_etaMax  = m_towerGeometrySvc->etaMax();
    m_phiBins = m_towerGeometrySvc->phiBins();
    m_phiMin  = m_towerGeometrySvc->phiMin();
    m_phiMax  = m_towerGeometrySvc->phiMax();
  } else {
    ATH_MSG_INFO("Eta and phi binning taken from specific configuration"); 
  }

  return book(); 
}

StatusCode CaloTopoClusterFromTowerMonitor::book()
{
  h_n   = (TH1D*)bookGetPointer( TH1D("Multiplicity",    "Multiplicity",     m_nBins,  m_nMin,  m_nMax  )); h_n->Sumw2();
  h_pt  = (TH1D*)bookGetPointer( TH1D("Pt",              "Pt",               m_ptBins, m_ptMin, m_ptMax )); h_pt->Sumw2();
  h_eta = (TH1D*)bookGetPointer( TH1D("Rapidity",        "Rapidity",         m_etaBins,m_etaMin,m_etaMax)); h_eta->Sumw2();
  h_phi = (TH1D*)bookGetPointer( TH1D("Azimuth",         "Azimuth",          m_phiBins,m_phiMin,m_phiMax)); h_phi->Sumw2();
  h_nc  = (TH1D*)bookGetPointer( TH1D("CellMultiplicity","Cell Multiplicity",m_ncBins, m_ncMin, m_ncMax )); h_nc->Sumw2();

  d_n_eta_phi = (TH2D*)bookGetPointer( TH2D("MultiplicityVsEtaVsPhi","Multiplicity in (eta,phi)",m_etaBins,m_etaMin,m_etaMax,m_phiBins,m_phiMin,m_phiMax)); d_n_eta_phi->Sumw2();
  d_pt_eta    = (TH2D*)bookGetPointer( TH2D("PtVsEta",               "PtVsEta",                  m_etaBins,m_etaMin,m_etaMax,m_ptBins, m_ptMin, m_ptMax )); d_pt_eta->Sumw2();
  d_nc_eta    = (TH2D*)bookGetPointer( TH2D("NCVsEta",               "Number cells versus eta",  m_etaBins,m_etaMin,m_etaMax,m_ncBins, m_ncMin, m_ncMax )); d_nc_eta->Sumw2();

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
  table_t centralReg;
  h_n->Fill(1.0*towerHandle->size());
  for ( auto ftow(towerHandle->begin()); ftow!=towerHandle->end(); ++ftow ) { 
    const xAOD::CaloCluster* ptow = *ftow;
    // tower kinematics
    double pt(ptow->pt()/Gaudi::Units::GeV);
    double eta(ptow->eta());
    double phi(CaloPhiRange::fix(ptow->phi()));
    double nc(1.0*ptow->size());
    h_pt->Fill(pt); h_eta->Fill(eta); h_phi->Fill(phi);
    h_nc->Fill(nc);
    d_n_eta_phi->Fill(eta,phi);
    d_pt_eta->Fill(eta,pt);
    d_nc_eta->Fill(eta,nc);
    // inspect towers around eta = 0, phi = 0 (excess multiplicity)
    if ( std::abs(eta) <= m_towerGeometrySvc->etaWidth() && std::abs(phi) <= m_towerGeometrySvc->phiWidth() ) {
      auto towerKey(key_t(m_towerGeometrySvc->towerIndex(eta,phi),ptow));
      if ( centralReg.find(towerKey) == centralReg.end() ) { centralReg[towerKey] = data_t(); }
      for ( auto fCell(ptow->getCellLinks()->begin());fCell!=ptow->getCellLinks()->end();++fCell ) { 
	auto pcell = *fCell; double weight(fCell.weight());
	centralReg[towerKey].push_back(pcell);
      } // cell-in-cluster loop
    } // region selector
  } // cluster loop

  // analyse central region 
  for ( auto flst(centralReg.begin()); flst!=centralReg.end(); ++flst ) {
    ATH_MSG_INFO( CaloRec::Helpers::fmtMsg("Tower %5zu at (%6.3f,%6.3f) has %zu cells associated",
			    (size_t)flst->first.first,flst->first.second->eta(),flst->first.second->phi(),flst->first.second->size()) );
  }
  return StatusCode::SUCCESS;
}
