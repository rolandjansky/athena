/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


// InDetEtaDependentCutsSvc includes
#include "InDetEtaDependentCuts/InDetEtaDependentCutsSvc.h"

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
    StatusCode InDetEtaDependentCutsSvc::initialize() {
      
      ATH_MSG_INFO ("Initializing " << name() << "...");
      
      std::vector < std::vector <double>* > sets_of_cuts_d = { &m_etaWidthBrem.value()        ,
                                                               &m_maxdImpactSSSSeeds.value()  ,
                                                               &m_maxPrimaryImpact.value()    ,
                                                               &m_maxZImpact.value()          ,
                                                               &m_minPT.value()               ,
                                                               &m_minPTBrem.value()           ,
                                                               &m_phiWidthBrem.value()        ,
                                                               &m_Xi2max.value()              ,
                                                               &m_Xi2maxNoAdd.value()         };
                                            
      std::vector < std::vector <int>* > sets_of_cuts_i    = { &m_maxDoubleHoles.value()      ,
                                                               &m_maxHoles.value()            ,
                                                               &m_maxPixelHoles.value()       ,
                                                               &m_maxSctHoles.value()         ,
                                                               &m_maxShared.value()           ,
                                                               &m_minClusters.value()         ,
                                                               &m_minPixelHits.value()        ,
                                                               &m_minSiNotShared.value()      ,
                                                               &m_maxHolesGapPattern.value()  ,
                                                               &m_maxHolesPattern.value()     ,
                                                               &m_nWeightedClustersMin.value()};
      
      // checking if the set of cuts makes sense
      if (checkSize(sets_of_cuts_d).isFailure())
        ATH_MSG_ERROR( "Check the cut values used in " << name() );
      
      if (checkSize(sets_of_cuts_i).isFailure())
        ATH_MSG_ERROR( "Check the cut values used in " << name() );
      
      // printing all the cuts
      ATH_MSG_DEBUG ("--- Dynamic cuts ---");
      ATH_MSG_DEBUG ("Eta bins (size=" << (m_etaBins.size()-1) << "): " << m_etaBins);
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
    
    template <class T>
    StatusCode InDetEtaDependentCutsSvc::checkSize(T& cuts) {
      // getting the number of eta bins
      size_t noOfEtaBins = m_etaBins.size()-1;
      for (auto *cut : cuts) {
        if (cut->size() == noOfEtaBins) continue;
        
        if (cut->size() > noOfEtaBins){
          ATH_MSG_ERROR( "No. of cut values bigger than eta bins");
          return StatusCode::FAILURE;
        }
        
        if (cut->size() < noOfEtaBins){
          ATH_MSG_DEBUG( "No. of cut values smaller than eta bins. Extending size..." );
          cut->resize(noOfEtaBins, cut->back());
        }
      }
      return StatusCode::SUCCESS;
    }

    void InDetEtaDependentCutsSvc::getIndexByEta(double eta, int& bin) {
      // getting the number of eta bins
      size_t noOfEtaBins = m_etaBins.size()-1;
      
      // setting the default value in case the bin is not found
      bin = -1;
      
      // getting the bin corresponding to the given eta value
      double absEta = fabs(eta);
      for (int i = int(noOfEtaBins-1); i >= 0; i--) {
        if (absEta > m_etaBins.value().at(i)) {
          bin = i;
          return;
        }
      }
      
      // in case you didn't return yet, then the bin is not found!
      ATH_MSG_ERROR( "Eta bin not found! Check the eta bin values in " << name() );
      return;
    }


    void InDetEtaDependentCutsSvc::getValue(InDet::CutName cutName, std::vector < double >& cut) {
      // getting the number of eta bins
      size_t noOfEtaBins = m_etaBins.size()-1;
      
      // resize the cut vector before setting it
      cut.resize(noOfEtaBins);
      
      switch (cutName) {
        case InDet::CutName::etaBins:
          cut = m_etaBins;
          break;
        
        case InDet::CutName::minPT:
          cut = m_minPT;
          break;
          
        case InDet::CutName::maxPrimaryImpact:
          cut = m_maxPrimaryImpact;
          break;
          
        case InDet::CutName::maxZImpact:
          cut = m_maxZImpact;
          
          break;
        case InDet::CutName::Xi2max:
          cut = m_Xi2max;
          break;
          
        case InDet::CutName::Xi2maxNoAdd:
          cut = m_Xi2maxNoAdd;
          break;
          
        case InDet::CutName::maxdImpactSSSSeeds:
          cut = m_maxdImpactSSSSeeds;
          break;
        
        case InDet::CutName::minPTBrem:
          cut = m_minPTBrem;
          break;
          
        case InDet::CutName::etaWidthBrem:
          cut = m_etaWidthBrem;
          break;
          
        case InDet::CutName::phiWidthBrem:
          cut = m_phiWidthBrem;
          break;
        
        default:
          ATH_MSG_ERROR("CutName not recognized. Cuts will remain unchanged.");
          break;
      }
    }
    
    void InDetEtaDependentCutsSvc::getValue(InDet::CutName cutName,    std::vector < int >& cut) {

      // getting the number of eta bins
      size_t noOfEtaBins = m_etaBins.size()-1;
      
      // resize the cut vector before setting it
      cut.resize(noOfEtaBins);

      switch (cutName) {
        case InDet::CutName::minClusters:
          cut = m_minClusters;
          break;
        
        case InDet::CutName::minSiNotShared:
          cut = m_minSiNotShared;
          break;
          
        case InDet::CutName::maxShared:
          cut = m_maxShared;
          break;
          
        case InDet::CutName::minPixelHits:
          cut = m_minPixelHits;
          
          break;
        case InDet::CutName::maxHoles:
          cut = m_maxHoles;
          break;
          
        case InDet::CutName::maxPixelHoles:
          cut = m_maxPixelHoles;
          break;
          
        case InDet::CutName::maxSctHoles:
          cut = m_maxSctHoles;
          break;
        
        case InDet::CutName::maxDoubleHoles:
          cut = m_maxDoubleHoles;
          break;
          
        case InDet::CutName::maxHolesPattern:
          cut = m_maxHolesPattern;
          break;
          
        case InDet::CutName::maxHolesGapPattern:
          cut = m_maxHolesGapPattern;
          break;
          
        case InDet::CutName::nWeightedClustersMin:
          cut = m_nWeightedClustersMin;
          break;
        
        default:
          ATH_MSG_ERROR("CutName not recognized. Cuts will remain unchanged.");
          break;
      }
    }
  
    template <class T>
    T InDetEtaDependentCutsSvc::getValueAtEta(std::vector< T > cuts, double eta) {
     int bin;
     getIndexByEta(eta, bin);
     return cuts.at(bin);
    }
    
    template <class T>
    void InDetEtaDependentCutsSvc::getValue(InDet::CutName cutName, T& cut, double eta) {
      std::vector < T > cuts; 
      getValue (cutName, cut);
      cut = getValueAtEta< T >(cuts, eta);
    }
    
    double  InDetEtaDependentCutsSvc::getMaxEta() {
      return m_etaBins.value().back();
    }
    
    double InDetEtaDependentCutsSvc::getMinPtAtEta(double eta) {
      return getValueAtEta<double>(m_minPT,eta);
    }
    
    double  InDetEtaDependentCutsSvc::getMaxZImpactAtEta      (double eta) {
      return getValueAtEta<double>(m_maxZImpact, eta);
    }
    
    double  InDetEtaDependentCutsSvc::getMaxPrimaryImpactAtEta(double eta) {
      return getValueAtEta<double>(m_maxPrimaryImpact, eta);
    }
      
    int     InDetEtaDependentCutsSvc::getMinSiHitsAtEta       (double eta) {
      return getValueAtEta<int>(m_minClusters, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMinPixelHitsAtEta    (double eta) {
      return getValueAtEta<int>(m_minPixelHits, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxSiHolesAtEta      (double eta) {
      return getValueAtEta<int>(m_maxHoles, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxPixelHolesAtEta   (double eta) {
      return getValueAtEta<int>(m_maxPixelHoles, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxSctHolesAtEta     (double eta) {
      return getValueAtEta<int>(m_maxSctHoles, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxDoubleHolesAtEta  (double eta) {
      return getValueAtEta<int>(m_maxDoubleHoles, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMinSiNotSharedAtEta  (double eta) {
      return getValueAtEta<int>(m_minSiNotShared, eta);
    }
    
    int     InDetEtaDependentCutsSvc::getMaxSharedAtEta  (double eta) {
      return getValueAtEta<int>(m_maxShared, eta);
    }
      
}   // end namespace
