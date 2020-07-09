/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
                                            m_nWeightedClustersMin.value()};
      
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
      ATH_MSG_DEBUG ("minPixelHits: " << m_minPixelHits);
      ATH_MSG_DEBUG ("minPT: " << m_minPT);
      ATH_MSG_DEBUG ("minPTBrem: " << m_minPTBrem);
      ATH_MSG_DEBUG ("minSiNotShared: " << m_minSiNotShared);
      ATH_MSG_DEBUG ("nHolesGapMax: " << m_maxHolesGapPattern);
      ATH_MSG_DEBUG ("nHolesMax: " << m_maxHolesPattern);
      ATH_MSG_DEBUG ("nWeightedClustersMin: " << m_nWeightedClustersMin);
      ATH_MSG_DEBUG ("phiWidthBrem: " << m_phiWidthBrem);
      ATH_MSG_DEBUG ("Xi2max: " << m_Xi2max);
      ATH_MSG_DEBUG ("Xi2maxNoAdd: " << m_Xi2maxNoAdd);   

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
      
      switch (cutName) {
        case InDet::CutName::etaBins:
          cuts = m_etaBins;
          break;
        
        case InDet::CutName::minPT:
          cuts = m_minPT;
          break;
          
        case InDet::CutName::maxPrimaryImpact:
          cuts = m_maxPrimaryImpact;
          break;
          
        case InDet::CutName::maxZImpact:
          cuts = m_maxZImpact;
          
          break;
        case InDet::CutName::Xi2max:
          cuts = m_Xi2max;
          break;
          
        case InDet::CutName::Xi2maxNoAdd:
          cuts = m_Xi2maxNoAdd;
          break;
          
        case InDet::CutName::maxdImpactSSSSeeds:
          cuts = m_maxdImpactSSSSeeds;
          break;
        
        case InDet::CutName::minPTBrem:
          cuts = m_minPTBrem;
          break;
          
        case InDet::CutName::etaWidthBrem:
          cuts = m_etaWidthBrem;
          break;
          
        case InDet::CutName::phiWidthBrem:
          cuts = m_phiWidthBrem;
          break;
        
        default:
          ATH_MSG_ERROR("CutName not recognized. Cuts will remain unchanged.");
          break;
      }
    }
    
    void InDetEtaDependentCutsSvc::getValue(const InDet::CutName cutName,    std::vector < int >& cuts) {

      // getting the number of eta bins
      size_t noOfEtaBins = m_etaBins.size();
      
      // resize the cuts vector before setting it
      cuts.resize(noOfEtaBins);

      switch (cutName) {
        case InDet::CutName::minClusters:
          cuts = m_minClusters;
          break;
        
        case InDet::CutName::minSiNotShared:
          cuts = m_minSiNotShared;
          break;
          
        case InDet::CutName::maxShared:
          cuts = m_maxShared;
          break;
          
        case InDet::CutName::minPixelHits:
          cuts = m_minPixelHits;
          
          break;
        case InDet::CutName::maxHoles:
          cuts = m_maxHoles;
          break;
          
        case InDet::CutName::maxPixelHoles:
          cuts = m_maxPixelHoles;
          break;
          
        case InDet::CutName::maxSctHoles:
          cuts = m_maxSctHoles;
          break;
        
        case InDet::CutName::maxDoubleHoles:
          cuts = m_maxDoubleHoles;
          break;
          
        case InDet::CutName::maxHolesPattern:
          cuts = m_maxHolesPattern;
          break;
          
        case InDet::CutName::maxHolesGapPattern:
          cuts = m_maxHolesGapPattern;
          break;
          
        case InDet::CutName::nWeightedClustersMin:
          cuts = m_nWeightedClustersMin;
          break;
        
        default:
          ATH_MSG_ERROR("CutName not recognized. Cuts will remain unchanged.");
          break;
      }
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
      
}   // end namespace
