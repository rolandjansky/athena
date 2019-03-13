// -*- c++ -*-
#ifndef CALOREC_CALOTOPOCLUSTERFROMTOWERMONITOR_H
#define CALOREC_CALOTOPOCLUSTERFROMTOWERMONITOR_H

#include "StoreGate/ReadHandleKey.h"

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthHistogramAlgorithm.h"

#include "xAODCaloEvent/CaloClusterContainer.h"

#include "CaloRec/CaloTowerGeometrySvc.h"
#include "CaloRec/CaloTopoClusterFromTowerHelpers.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCell.h"

#include <vector>
#include <bitset>
#include <map>
#include <cmath>

#include "TH1D.h"
#include "TH2D.h"

///@brief Algorithm to monitor kinematics and composition of towers (and clusters)
///
/// This algorithm fills a few histograms relevant for monitoring tower-related information, As both topo-clusters and towers
/// use the same representation in data, both signal definitions can be monitored - even though most histograms are designed
/// for distributions of tower-specific signal features.
///
/// The algorithm has several process control properties and configured values:
///
/// | Property          |  Default     | Controled feature or value
/// :------------------ | :----------: |:-------------------------------------------------------------------------------------------
/// | @c DoGeoAutoBins  |   @c true    | if @c true the binning of @f$ y @f$ and @f$ \phi @f$ is constructed from the tower geometry database;
/// | ^                 | ^            | else it is user-defined by the corresponding value property
/// | Hotspot analysis is on if @f$ y_{\rm min}^{\rm hotspot} < y_{\rm max}^{\rm hotspot} @f$ and @f$ \phi_{\rm min}^{\rm hotspot} < \phi_{\rm max}^[\rm hotspot} @f$ (on by defaults) ||
/// | @c EtaMinHotspot  |   0.         | defines lower limit @f$ y_{\rm min}^{\rm hotspot} @f$ of rapidity for the hotspot analysis
/// | @c EtaMaxHotspot  |   0.1        | defines upper limit @f$ y_{\rm max}^{\rm hotspot} @f$ of rapidity for the hotspot analysis
/// | @c PhiMinHotspot  |   0.         | defines lower limit @f$ \phi_{\rm min}^{\rm hotspot} @f$ of azimuth for the hotspot analysis
/// | @c PhiMaxHotspot  |   0.1        | defines upper limit @f$ \phi_{\rm max}^{\rm hotspot} @f$ of azimuth for the hotspot analysis  
/// | Histogram binning: (pseudo)-rapidity (@f$ y_{\rm min} \geq y_{\rm max} @f$ returns error) ||
/// | @c EtaTowersBins  |   100        | number of rapidity bins
/// | @c EtaTowersMin   |  -5.         | lower limit of rapidity range @f$ y_{\rm min} @f$
/// | @c EtaTowersMax   |   5.         | upper limit of rapidity range @f$ y_{\rm max} @f$
/// | Histogram binning: azimuth (@f$ \phi_{\rm min \geq \phi_[\rm max} @f$ returns error) ||
/// | @c PhiTowersBins  |  64          | number of azimuth bins
/// | @c PhiTowersMin   | @f$ -\pi @f$ | lower limit of azimuth range @f$ \phi_{\rm min} @f$
/// | @c PhiTowersMax   | @f$ +\pi @f$ | upper limit of azimuth range @f$ \phi_{\rm max} @f$
/// | Histogram binning: transverse momentum (@f$ p_{\rm T,min} < p_{\rm T,max} @f$ returns error) ||
/// | @c PtTowersBins   | 220          | number of transverse momentum bins
/// | @c PtTowersMin    | -10 GeV      | lower limit of transverse momentum range @f$ p_{\rm T,min} @f$
/// | @c PtTowersMax    | 100 GeV      | upper limit of transverse momentum range @f$ p_{\rm T,max} @f$
class CaloTopoClusterFromTowerMonitor : public AthHistogramAlgorithm
{
public:

  /// Default algorithm constructor
  CaloTopoClusterFromTowerMonitor(const std::string& name,ISvcLocator* pSvcLocator);
  /// Base-class destructor
  virtual ~CaloTopoClusterFromTowerMonitor();

  ///@brief Initialization
  ///
  /// This method configures the algorithm, allocates the tower geometry service and books the histograms by 
  /// invoking the @c book() method. In addition, histograms are filled with geometry information retrieved
  /// from the tower geometry service (static information not depending on event variables).   
  virtual StatusCode initialize();
  ///@brief Execution
  ///
  /// This method allocates the input data container and fills all histograms. If configured, it also provides
  ///  
  virtual StatusCode execute();    ///< Execution fills histograms.


private:

  ///@name Data access properties
  ///@{
  SG::ReadHandleKey<xAOD::CaloClusterContainer> m_towerContainerKey; ///< Allocator for input @c xAOD::CaloClusterContainer 
  ServiceHandle<CaloTowerGeometrySvc>           m_towerGeometrySvc;  ///< Allocator for tower geometry services
  ///@}

  ///@name Histogram binning properties
  ///@{
  int m_ncBins;    ///< Number of cells in towers - number of bins
  double m_ncMin;  ///< Number of cells in towers - lower limit of value range
  double m_ncMax;  ///< Number of cells in towers - upper limit of value range
  int m_nBins;     ///< Tower multiplicity - number of bins  
  double m_nMin;   ///< Tower multiplicity - lower limit of value range  
  double m_nMax;   ///< Tower multiplicity - upper limit of value range
  int m_ptBins;    ///< Tower @f$ p_{\rm T} @f$ - number of bins 
  double m_ptMin;  ///< Tower @f$ p_{\rm T} @f$ - lower limit of value range (in GeV)
  double m_ptMax;  ///< Tower @f$ p_{\rm T} @f$ - upper limit of value range (in GeV)
  int m_etaBins;   ///< Tower rapidity - number of bins
  double m_etaMin; ///< Tower rapidity - lower limit of value range
  double m_etaMax; ///< Tower rapidity - upper limit of value range
  int m_phiBins;   ///< Tower azimuth - number of bins
  double m_phiMin; ///< Tower azimuth - lower limit of value range
  double m_phiMax; ///< Tower azimuth - upper limit of value range

  double m_hsEtaMin; double m_hsEtaMax;
  double m_hsPhiMin; double m_hsPhiMax;

  bool m_doGeoAutoBins; 
  bool m_doHotspot;
  ///@}

  TH1D* h_n;
  TH1D* h_pt;
  TH1D* h_eta;
  TH1D* h_phi;
  TH1D* h_nc;
  TH1D* h_samp;

  TH2D* d_n_eta_phi;
  TH2D* d_nc_eta_phi;
  TH2D* d_pt_eta;
  TH2D* d_nc_eta;

  TH2D* d_wgt_samp;
  TH2D* d_ntt_samp;
  TH2D* d_geo_samp;
  TH2D* d_maxtowers_samp;
  TH2D* d_wgttowers_samp; 

  TH2D* d_maxcells_eta;
  TH2D* d_allwghts_eta;

  TH2D* d_deta_eta;
  TH2D* d_dphi_eta;
  TH2D* d_dphi_deta;

  TH2D* d_detac_eta;
  TH2D* d_dphic_eta;
  TH2D* d_dphic_detac;

  TH2D* d_detac_samp;
  TH2D* d_dphic_samp;

  // hot spot
  TH1D* h_nc_hs;
  TH1D* h_n_hs;
  TH1D* h_pt_hs;
  TH1D* h_eta_hs;
  TH1D* h_phi_hs;
  TH1D* h_samp_hs;

  TH2D* d_n_eta_phi_hs;
  TH2D* d_nc_eta_phi_hs;

  TH2D* d_deta_eta_hs;
  TH2D* d_dphi_eta_hs;
  TH2D* d_dphi_deta_hs;

  TH2D* d_detac_eta_hs;
  TH2D* d_dphic_eta_hs;
  TH2D* d_dphic_detac_hs;

  TH2D* d_detac_samp_hs;
  TH2D* d_dphic_samp_hs;

  std::vector<TH2D*> d_maxcells_phi_eta_slice;
  std::vector<TH2D*> d_allwghts_phi_eta_slice;

protected:

  bool isInHotspot(const xAOD::CaloCluster& ptow) const;
  bool fillComposition(const xAOD::CaloCluster& ptow,std::vector<double>& deta,std::vector<double>& dphi,std::vector<CaloSampling::CaloSample>& csam) const;
  bool setAxisTitle(TH1* h,const std::string& title,const std::string& axis="x"); 

  virtual StatusCode book();
  std::bitset<200000> m_cellTags;

  ///////////////////////
  // BookAny Templates //
  ///////////////////////

  template<class H>
  H* bookAny(const std::string& hname,const std::string& htitle,const std::string& xtitle,int nxbins,double xmin,double xmax) {
    H* hptr = (H*)bookGetPointer( H(hname.c_str(),htitle.c_str(),nxbins,xmin,xmax) );
    if ( hptr == 0 ) { 
      ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Cannot book distribution \042%s\042 with title \042%s\042",hname.c_str(),htitle.c_str()) );
    } else { 
      hptr->Sumw2();
      if ( !xtitle.empty() && xtitle != "" ) { hptr->GetXaxis()->SetTitle(xtitle.c_str()); }
    }
    return hptr;
  }

  template<class H>
  H* bookAny(const std::string& hname,const std::string& htitle,const std::string& xtitle,int nxbins,double xmin,double xmax,int nybins,double ymin,double ymax) {
    H* hptr = (H*)bookGetPointer( H(hname.c_str(),htitle.c_str(),nxbins,xmin,xmax,nybins,ymin,ymax) );
    if ( hptr == 0 ) { 
      ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Cannot book distribution \042%s\042 with title \042%s\042",hname.c_str(),htitle.c_str()) );
    } else { 
      hptr->Sumw2();
      if ( !xtitle.empty() && xtitle != "" ) { hptr->GetXaxis()->SetTitle(xtitle.c_str()); }
    }
    return hptr;
  }

  template<class H>
  H* bookAny(const std::string& hname,const std::string& htitle,const std::string& xtitle,
	     int nxbins,double xmin,double xmax,int nybins,double ymin,double ymax,int nzbins,double zmin,double zmax) {
      H* hptr = (H*)bookGetPointer( H(hname.c_str(),htitle.c_str(),nxbins,xmin,xmax,nybins,ymin,ymax,nzbins,zmin,zmax) );
    if ( hptr == 0 ) { 
      ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Cannot book distribution \042%s\042 with title \042%s\042",hname.c_str(),htitle.c_str()) );
    } else { 
      hptr->Sumw2();
      if ( !xtitle.empty() && xtitle != "" ) { hptr->GetXaxis()->SetTitle(xtitle.c_str()); }
    }
    return hptr;
  }

  template<class H>
      H* bookAny(const std::string& hname,const std::string& htitle,int nxbins,double xmin,double xmax) {
    H* hptr = (H*)bookGetPointer( H(hname.c_str(),htitle.c_str(),nxbins,xmin,xmax) );
    if ( hptr == 0 ) { 
      ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Cannot book distribution \042%s\042 with title \042%s\042",hname.c_str(),htitle.c_str()) );
    } else { 
      hptr->Sumw2();
      hptr->GetXaxis()->SetTitle(hptr->GetTitle());
    }
    return hptr;
  }

  template<class H>
    H* bookAny(const std::string& hname,const std::string& htitle,int nxbins,double xmin,double xmax,int nybins,double ymin,double ymax) { 
      H* hptr = (H*)bookGetPointer( H(hname.c_str(),htitle.c_str(),nxbins,xmin,xmax,nybins,ymin,ymax) );
    if ( hptr == 0 ) { 
      ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Cannot book distribution \042%s\042 with title \042%s\042",hname.c_str(),htitle.c_str()) );
    } else { 
      hptr->Sumw2();
      hptr->GetXaxis()->SetTitle(hptr->GetTitle());
    }
    return hptr;
  }

  template<class H>
  H* bookAny(const std::string& hname,const std::string& htitle,int nxbins,double xmin,double xmax,int nybins,double ymin,double ymax,int nzbins,double zmin,double zmax) {
    H* hptr = (H*)bookGetPointer( H(hname.c_str(),htitle.c_str(),nxbins,xmin,xmax,nybins,ymin,ymax,nzbins,zmin,zmax) );
    if ( hptr == 0 ) { 
      ATH_MSG_WARNING( CaloRec::Helpers::fmtMsg("Cannot book distribution \042%s\042 with title \042%s\042",hname.c_str(),htitle.c_str()) );
    } else { 
      hptr->Sumw2();
      hptr->GetXaxis()->SetTitle(hptr->GetTitle());
    }
    return hptr;
  }

  ////////////////////////
  // Book for Samplings //
  ////////////////////////

  template<class H>
    H* bookForSamplings(const std::string& hname,const std::string& htitle) {
    int nsamp((int)CaloRec::Lookup::getSamplingId("MINIFCAL0")); double xmin(-0.5); double xmax(xmin+1.*(nsamp+1));
    H* hptr = bookAny<H>(hname,htitle,"",nsamp+1,xmin,xmax);
    if ( hptr != 0 ) { 
      for ( int isamp(0); isamp < nsamp; ++isamp ) { 
	hptr->GetXaxis()->SetBinLabel(isamp+1,CaloRec::Lookup::getSamplingName((CaloSampling::CaloSample)isamp).c_str()); 
      }
      hptr->GetXaxis()->SetBinLabel(hptr->GetNbinsX(),CaloRec::Lookup::getSamplingName(CaloSampling::Unknown).c_str());
    }
    return hptr;
  }

  template<class H>
  H* bookForSamplings(const std::string& hname,const std::string& htitle,int nybins,double ymin,double ymax) {
    int nsamp((int)CaloRec::Lookup::getSamplingId("MINIFCAL0")); double xmin(-0.5); double xmax(xmin+1.*(nsamp+1));
    H* hptr = bookAny<H>(hname,htitle,"",nsamp+1,xmin,xmax,nybins,ymin,ymax);
    if ( hptr != 0 ) { 
      for ( int isamp(0); isamp < nsamp; ++isamp ) { 
	hptr->GetXaxis()->SetBinLabel(isamp+1,CaloRec::Lookup::getSamplingName((CaloSampling::CaloSample)isamp).c_str()); 
      }
      hptr->GetXaxis()->SetBinLabel(hptr->GetNbinsX(),CaloRec::Lookup::getSamplingName(CaloSampling::Unknown).c_str());
    }
    return hptr;
  }

  ///////////////////////
  // Book for Rapidity //
  ///////////////////////

  template<class H>
  H* bookForEta(const std::string& hname,const std::string& htitle) { return bookAny<H>(hname,htitle,"y_{tower}",m_etaBins,m_etaMin,m_etaMax); }

  template<class H>
  H* bookForEta(const std::string& hname,const std::string& htitle,int nybins,double ymin,double ymax) { 
    return bookAny<H>(hname,htitle,"y_{tower}",m_etaBins,m_etaMin,m_etaMax,nybins,ymin,ymax);
  }

  //////////////////////
  // Book for Azimuth //
  //////////////////////

  template<class H>
  H* bookForPhi(const std::string& hname,const std::string& htitle) { return bookAny<H>(hname,htitle,"#phi_{tower} [rad]",m_phiBins,m_phiMin,m_phiMax); }

  template<class H>
  H* bookForPhi(const std::string& hname,const std::string& htitle,int nybins,double ymin,double ymax) { 
    return bookAny<H>(hname,htitle,"#phi_{tower} [rad]",m_phiBins,m_phiMin,m_phiMax,nybins,ymin,ymax);
  }

  ///////////////////
  // Fill Sampling //
  ///////////////////

  template<class H>
  void fillSampling(H* hptr,CaloSampling::CaloSample csamp) { 
    int isamp(std::min(static_cast<int>(csamp),static_cast<int>(CaloRec::Lookup::getSamplingId("MINIFCAL0"))));
    hptr->Fill(1.*isamp);
  }

  template<class H>
  void fillSampling(H* hptr,CaloSampling::CaloSample csamp,double yval) { 
    int isamp(std::min(static_cast<int>(csamp),static_cast<int>(CaloRec::Lookup::getSamplingId("MINIFCAL0"))));
    hptr->Fill(1.*isamp,yval);
  }

  template<class H>
  void fillSampling(H* hptr,CaloSampling::CaloSample csamp,double yval,double zval) { 
    int isamp(std::min(static_cast<int>(csamp),static_cast<int>(CaloRec::Lookup::getSamplingId("MINIFCAL0"))));
    hptr->Fill(1.*isamp,yval,zval);
  }
}; // CaloTopoClusterFromTowerMonitor

inline bool CaloTopoClusterFromTowerMonitor::isInHotspot(const xAOD::CaloCluster& ptow) const
{ return m_doHotspot && ( ptow.eta() >= m_hsEtaMin && ptow.eta() < m_hsEtaMax ) && ( ptow.phi() >= m_hsPhiMin && ptow.phi() < m_hsPhiMax ); }

inline bool CaloTopoClusterFromTowerMonitor::setAxisTitle(TH1* hptr,const std::string& title,const std::string& axis) {
  if ( axis == "x" ) { hptr->GetXaxis()->SetTitle(title.c_str()); return true; }
  if ( axis == "y" ) { hptr->GetYaxis()->SetTitle(title.c_str()); return true; }
  if ( axis == "z" ) { hptr->GetZaxis()->SetTitle(title.c_str()); return true; }
  return false;
}
#endif


