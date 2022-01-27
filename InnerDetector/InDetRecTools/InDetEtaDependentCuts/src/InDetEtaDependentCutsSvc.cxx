/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


// InDetEtaDependentCutsSvc includes
#include "InDetEtaDependentCuts/InDetEtaDependentCutsSvc.h"

#include <cmath>
#include <functional>
#include <variant>

namespace InDet {
  
  ////////////////
  // Constructors
  ////////////////
  InDetEtaDependentCutsSvc::InDetEtaDependentCutsSvc(const std::string& name, 
                                                     ISvcLocator* sl):
    AthService(name, sl)
    {}

    ///////////////
    // Destructor
    ///////////////
    InDetEtaDependentCutsSvc::~InDetEtaDependentCutsSvc() {}
    
    
    ///////////////
    // queryInterface
    ///////////////
    StatusCode InDetEtaDependentCutsSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
      if( IID_IInDetEtaDependentCutsSvc == riid ) {
        *ppvIF = dynamic_cast< IInDetEtaDependentCutsSvc* >(this); 
      } else{ 
        return AthService::queryInterface(riid, ppvIF); 
      }
      addRef();
      return StatusCode::SUCCESS; 
    }

    ///////////////
    // Initialize
    ///////////////
    StatusCode 
    InDetEtaDependentCutsSvc::initialize() {
      ATH_MSG_DEBUG ("Initializing " << name() << "...");
      if (m_etaBins.empty()) {
        ATH_MSG_ERROR( "Wrong initialisation of eta bins. Check the eta bin values in " << name() );
        return StatusCode::FAILURE;
      }
      
      // expecting eta bins in ascending order
      if (not std::is_sorted(m_etaBins.value().begin(), m_etaBins.value().end())) {
        ATH_MSG_ERROR( "Wrong initialisation of eta bins in " << name() << ". Values are not sorted!" );
        return StatusCode::FAILURE;
      }          
      
      using setOfCuts = std::variant< std::reference_wrapper<std::vector <double>>, std::reference_wrapper<std::vector <int>> >;
      
      std::vector < setOfCuts> allCuts    { m_etaWidthBrem.value()        , 
                                            m_maxdImpactSSSSeeds.value()  ,
                                            m_maxPrimaryImpact.value()    ,
                                            m_maxZImpact.value()          ,
                                            m_minPT.value()               ,
                                            m_minPTBrem.value()           ,
                                            m_phiWidthBrem.value()        ,
                                            m_Xi2max.value()              ,
                                            m_Xi2maxNoAdd.value()         ,
                                            m_maxDoubleHoles.value()      ,
                                            m_maxHoles.value()            ,
                                            m_maxPixelHoles.value()       ,
                                            m_maxSctHoles.value()         ,
                                            m_maxShared.value()           ,
                                            m_minClusters.value()         ,
                                            m_minPixelHits.value()        ,
                                            m_minSiNotShared.value()      ,
                                            m_maxHolesGapPattern.value()  ,
                                            m_maxHolesPattern.value()     ,
                                            m_nWeightedClustersMin.value(),
                                            m_minInPixelHits.value(),
                                            m_minStripHits.value()};
      
      // checking if the set of cuts makes sense
      size_t noOfEtaBins = m_etaBins.size();
                                            
      for (setOfCuts& cuts : allCuts) {
        auto sCode = std::visit([noOfEtaBins] (auto & testingCuts) -> StatusCode { 
         
          if (testingCuts.get().size() == noOfEtaBins) 
            return StatusCode::SUCCESS;
                                 
          if (testingCuts.get().size() > noOfEtaBins)
            return StatusCode::FAILURE;
          
          if (testingCuts.get().size() < noOfEtaBins)
            testingCuts.get().resize(noOfEtaBins, testingCuts.get().back());
                    
          return StatusCode::SUCCESS;
        } , cuts);
        
        if (sCode.isFailure()) {
          ATH_MSG_ERROR( "No. of cut values bigger than eta bins");
          return sCode;
        }
      }
      
      // printing all the cuts
      ATH_MSG_DEBUG ("--- Dynamic cuts ---");
      ATH_MSG_DEBUG ("Eta bins (size=" << (m_etaBins.size()) << "): " << m_etaBins);
      ATH_MSG_DEBUG ("etaWidthBrem: " << m_etaWidthBrem);
      ATH_MSG_DEBUG ("maxdImpactSSSSeeds: " << m_maxdImpactSSSSeeds);
      ATH_MSG_DEBUG ("maxDoubleHoles: " << m_maxDoubleHoles);
      ATH_MSG_DEBUG ("maxHoles: " << m_maxHoles);
      ATH_MSG_DEBUG ("maxPixelHoles: " << m_maxPixelHoles);
      ATH_MSG_DEBUG ("maxPrimaryImpact: " << m_maxPrimaryImpact);
      ATH_MSG_DEBUG ("maxSctHoles: " << m_maxSctHoles);
      ATH_MSG_DEBUG ("maxShared: " << m_maxShared);
      ATH_MSG_DEBUG ("maxZImpact: " << m_maxZImpact);
      ATH_MSG_DEBUG ("minClusters: " << m_minClusters);
      ATH_MSG_DEBUG ("minInnermostPixelHits: " << m_minInPixelHits);
      ATH_MSG_DEBUG ("minPixelHits: " << m_minPixelHits);
      ATH_MSG_DEBUG ("minStripHits: " << m_minStripHits);
      ATH_MSG_DEBUG ("minPT: " << m_minPT);
      ATH_MSG_DEBUG ("minPTBrem: " << m_minPTBrem);
      ATH_MSG_DEBUG ("minSiNotShared: " << m_minSiNotShared);
      ATH_MSG_DEBUG ("nHolesGapMax: " << m_maxHolesGapPattern);
      ATH_MSG_DEBUG ("nHolesMax: " << m_maxHolesPattern);
      ATH_MSG_DEBUG ("nWeightedClustersMin: " << m_nWeightedClustersMin);
      ATH_MSG_DEBUG ("phiWidthBrem: " << m_phiWidthBrem);
      ATH_MSG_DEBUG ("Xi2max: " << m_Xi2max);
      ATH_MSG_DEBUG ("Xi2maxNoAdd: " << m_Xi2maxNoAdd);

      // Initialize maps for navigation
      m_mapDoubleCuts[InDet::CutName::etaBins]            = m_etaBins;
      m_mapDoubleCuts[InDet::CutName::minPT]              = m_minPT;
      m_mapDoubleCuts[InDet::CutName::maxPrimaryImpact]   = m_maxPrimaryImpact;
      m_mapDoubleCuts[InDet::CutName::maxZImpact]         = m_maxZImpact;
      m_mapDoubleCuts[InDet::CutName::Xi2max]             = m_Xi2max;
      m_mapDoubleCuts[InDet::CutName::Xi2maxNoAdd]        = m_Xi2maxNoAdd;
      m_mapDoubleCuts[InDet::CutName::maxdImpactSSSSeeds] = m_maxdImpactSSSSeeds;
      m_mapDoubleCuts[InDet::CutName::minPTBrem]          = m_minPTBrem;
      m_mapDoubleCuts[InDet::CutName::etaWidthBrem]       = m_etaWidthBrem;
      m_mapDoubleCuts[InDet::CutName::phiWidthBrem]       = m_phiWidthBrem;

      m_mapIntCuts[InDet::CutName::minClusters]           = m_minClusters;
      m_mapIntCuts[InDet::CutName::minSiNotShared]        = m_minSiNotShared;
      m_mapIntCuts[InDet::CutName::maxShared]             = m_maxShared;
      m_mapIntCuts[InDet::CutName::minPixelHits]          = m_minPixelHits;
      m_mapIntCuts[InDet::CutName::maxHoles]              = m_maxHoles;
      m_mapIntCuts[InDet::CutName::maxPixelHoles]         = m_maxPixelHoles;
      m_mapIntCuts[InDet::CutName::maxSctHoles]           = m_maxSctHoles;
      m_mapIntCuts[InDet::CutName::maxDoubleHoles]        = m_maxDoubleHoles;
      m_mapIntCuts[InDet::CutName::maxHolesPattern]       = m_maxHolesPattern;
      m_mapIntCuts[InDet::CutName::maxHolesGapPattern]    = m_maxHolesGapPattern;
      m_mapIntCuts[InDet::CutName::nWeightedClustersMin]  = m_nWeightedClustersMin;
      m_mapIntCuts[InDet::CutName::minInPixelHits]        = m_minInPixelHits;
      m_mapIntCuts[InDet::CutName::minStripHits]          = m_minStripHits;

      return StatusCode::SUCCESS;
    }
    
    ///////////////
    // Finalize
    ///////////////
    StatusCode InDetEtaDependentCutsSvc::finalize() {
      ATH_MSG_INFO ("Finalizing " << name() << "...");
      return StatusCode::SUCCESS;
    }

    int 
    InDetEtaDependentCutsSvc::getIndexByEta(const double eta) const {
      const double absEta = std::abs(eta);
      if (absEta > m_etaBins.value().back()) {
        ATH_MSG_ERROR("Requesting cut value for eta outside expected range!! ");
        return -1;
      }
      const auto pVal =  std::lower_bound(m_etaBins.value().begin(), m_etaBins.value().end(), absEta);
      const int bin = std::distance(m_etaBins.value().begin(), pVal);
      ATH_MSG_DEBUG("Checking (abs(eta)/bin) = (" << absEta << "," << bin << ")");
      return bin;
    }


    void InDetEtaDependentCutsSvc::getValue(const InDet::CutName cutName, std::vector < double >& cuts) {
      // getting the number of eta bins
      size_t noOfEtaBins = m_etaBins.size();
      
      // resize the cuts vector before setting it
      cuts.resize(noOfEtaBins);

      std::unordered_map< InDet::CutName, std::vector<double> >::iterator it = m_mapDoubleCuts.find(cutName);

      if(it!=m_mapDoubleCuts.end()) cuts = it->second;
      else ATH_MSG_ERROR("CutName not recognized. Cuts will remain unchanged.");

    }
    
    void InDetEtaDependentCutsSvc::getValue(const InDet::CutName cutName,    std::vector < int >& cuts) {

      // getting the number of eta bins
      size_t noOfEtaBins = m_etaBins.size();
      
      // resize the cuts vector before setting it
      cuts.resize(noOfEtaBins);

      std::unordered_map< InDet::CutName, std::vector<int> >::iterator it = m_mapIntCuts.find(cutName);

      if(it!=m_mapIntCuts.end()) cuts = it->second;
      else ATH_MSG_ERROR("CutName not recognized. Cuts will remain unchanged.");

    }
  
    
    double  InDetEtaDependentCutsSvc::getMaxEta() const {
      return m_etaBins.value().back();
    }
    
    double InDetEtaDependentCutsSvc::getMinPtAtEta(const double eta) const {
      return getValueAtEta<double>(m_minPT,eta);
    }
    
    double  InDetEtaDependentCutsSvc::getMaxZImpactAtEta      (const double eta) const {
      return getValueAtEta<double>(m_maxZImpact, eta);
    }
    
    double  InDetEtaDependentCutsSvc::getMaxPrimaryImpactAtEta(const double eta) const {
      return getValueAtEta<double>(m_maxPrimaryImpact, eta);
    }

    double  InDetEtaDependentCutsSvc::getMaxChi2AtEta(const double eta) const {
      return getValueAtEta<double>(m_Xi2max, eta);
    }

    int     InDetEtaDependentCutsSvc::getMinSiHitsAtEta       (const double eta) const {
      return getValueAtEta<int>(m_minClusters, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMinPixelHitsAtEta    (const double eta) const {
      return getValueAtEta<int>(m_minPixelHits, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxSiHolesAtEta      (const double eta) const {
      return getValueAtEta<int>(m_maxHoles, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxPixelHolesAtEta   (const double eta) const {
      return getValueAtEta<int>(m_maxPixelHoles, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxSctHolesAtEta     (const double eta) const {
      return getValueAtEta<int>(m_maxSctHoles, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxDoubleHolesAtEta  (const double eta) const {
      return getValueAtEta<int>(m_maxDoubleHoles, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMinSiNotSharedAtEta  (const double eta) const {
      return getValueAtEta<int>(m_minSiNotShared, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxSharedAtEta  (const double eta) const {
      return getValueAtEta<int>(m_maxShared, eta);
    }

    int     InDetEtaDependentCutsSvc::getMinInnermostPixelHitsAtEta  (const double eta) const {
      return getValueAtEta<int>(m_minInPixelHits, eta);
    }

    int     InDetEtaDependentCutsSvc::getMinStripHitsAtEta  (const double eta) const {
      return getValueAtEta<int>(m_minStripHits, eta);
    }
      
}   // end namespace
