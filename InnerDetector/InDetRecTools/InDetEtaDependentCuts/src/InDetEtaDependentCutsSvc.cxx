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
    AthService(name, sl),
    m_noOfEtaBins(1),
    m_maxEta(2.7)
    {
      //
      // Property declaration
      //
      
      declareProperty("etaBins" ,                m_etaBins              =   {0.0, 4.0}); // eta bins 
      declareProperty("minPT",                   m_minPT                =      {900.0}); // min pT
      declareProperty("minClusters",             m_minClusters          =          {9}); // min number of (Si) clusters (Si hits)
      declareProperty("minPixelHits",            m_minPixelHits         =          {1}); // min number of pixel hits 
      declareProperty("minSiNotShared",          m_minSiNotShared       =          {7}); // min number of NOT shared 
      declareProperty("maxShared",               m_maxShared            =          {2}); // max number of shared
      declareProperty("maxHoles",                m_maxHoles             =          {2}); // max number of Si holes 
      declareProperty("maxPixelHoles",           m_maxPixelHoles        =          {1}); // max number of Pixel holes
      declareProperty("maxSctHoles",             m_maxSctHoles          =          {2}); // max number of SCT holes
      declareProperty("maxDoubleHoles",          m_maxDoubleHoles       =          {1}); // max number of double holes
      declareProperty("maxPrimaryImpact",        m_maxPrimaryImpact     =        {2.0}); // max Rphi IP (primaries) 
      declareProperty("maxZImpact",              m_maxZImpact           =      {200.0}); // max Z IP
      declareProperty("maxHolesPattern",         m_maxHolesPattern      =          {2}); // max holes in pattern
      declareProperty("maxHolesGapPattern",      m_maxHolesGapPattern   =          {2}); // max holes gap in pattern
      declareProperty("Xi2max",                  m_Xi2max               =        {9.0}); // Xi2 max
      declareProperty("Xi2maxNoAdd",             m_Xi2maxNoAdd          =       {25.0}); // Xi2 max no add
      declareProperty("nWeightedClustersMin",    m_nWeightedClustersMin =          {6}); // nWeightedClustersMin
      declareProperty("maxdImpactSSSSeeds",      m_maxdImpactSSSSeeds   =       {20.0}); // max impact on seeds SSS
      declareProperty("minPTBrem",               m_minPTBrem            =     {1000.0}); // min pT for brem reocvery
      declareProperty("phiWidthBrem",            m_phiWidthBrem         =        {0.3}); // phi Width of road for brem (cut for brem)
      declareProperty("etaWidthBrem",            m_etaWidthBrem         =        {0.2}); // cut for brem
      declareProperty("minInnermostPixelHits",   m_minInPixelHits       =          {0}); // min number of pixel hits in innermost layer
      declareProperty("minStripHits",            m_minStripHits         =          {0}); // min number of strip hits
    
    }

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
      
      // defining the max value in eta
      m_maxEta = m_etaBins.back();
      
      // getting the number of eta bins
      m_noOfEtaBins = m_etaBins.size()-1;
      
      std::vector < std::vector <double>* > sets_of_cuts_d = { &m_etaWidthBrem        ,
                                                               &m_maxdImpactSSSSeeds  ,
                                                               &m_maxPrimaryImpact    ,
                                                               &m_maxZImpact          ,
                                                               &m_minPT               ,
                                                               &m_minPTBrem           ,
                                                               &m_phiWidthBrem        ,
                                                               &m_Xi2max              ,
                                                               &m_Xi2maxNoAdd         };
                                            
      std::vector < std::vector <int>* > sets_of_cuts_i    = { &m_maxDoubleHoles      ,
                                                               &m_maxHoles            ,
                                                               &m_maxPixelHoles       ,
                                                               &m_maxSctHoles         ,
                                                               &m_maxShared           ,
                                                               &m_minClusters         ,
                                                               &m_minPixelHits        ,
                                                               &m_minSiNotShared      ,
                                                               &m_maxHolesGapPattern  ,
                                                               &m_maxHolesPattern     ,
                                                               &m_nWeightedClustersMin,
                                                               &m_minInPixelHits,
                                                               &m_minStripHits};
      
      // checking if the set of cuts makes sense
      if (checkSize(sets_of_cuts_d).isFailure())
        ATH_MSG_ERROR( "Check the cut values used in " << name() );
      
      if (checkSize(sets_of_cuts_i).isFailure())
        ATH_MSG_ERROR( "Check the cut values used in " << name() );
      
      // printing all the cuts
      ATH_MSG_INFO ("--- Dynamic cuts ---");
      ATH_MSG_INFO ("Eta bins (size=" << m_noOfEtaBins << "): " << m_etaBins);
      ATH_MSG_INFO ("etaWidthBrem: " << m_etaWidthBrem);
      ATH_MSG_INFO ("maxdImpactSSSSeeds: " << m_maxdImpactSSSSeeds);
      ATH_MSG_INFO ("maxDoubleHoles: " << m_maxDoubleHoles);
      ATH_MSG_INFO ("maxHoles: " << m_maxHoles);
      ATH_MSG_INFO ("maxPixelHoles: " << m_maxPixelHoles);
      ATH_MSG_INFO ("maxPrimaryImpact: " << m_maxPrimaryImpact);
      ATH_MSG_INFO ("maxSctHoles: " << m_maxSctHoles);
      ATH_MSG_INFO ("maxShared: " << m_maxShared);
      ATH_MSG_INFO ("maxZImpact: " << m_maxZImpact);
      ATH_MSG_INFO ("minClusters: " << m_minClusters);
      ATH_MSG_INFO ("minInnermostPixelHits: " << m_minInPixelHits);
      ATH_MSG_INFO ("minPixelHits: " << m_minPixelHits);
      ATH_MSG_INFO ("minStripHits: " << m_minStripHits);
      ATH_MSG_INFO ("minPT: " << m_minPT);
      ATH_MSG_INFO ("minPTBrem: " << m_minPTBrem);
      ATH_MSG_INFO ("minSiNotShared: " << m_minSiNotShared);
      ATH_MSG_INFO ("nHolesGapMax: " << m_maxHolesGapPattern);
      ATH_MSG_INFO ("nHolesMax: " << m_maxHolesPattern);
      ATH_MSG_INFO ("nWeightedClustersMin: " << m_nWeightedClustersMin);
      ATH_MSG_INFO ("phiWidthBrem: " << m_phiWidthBrem);
      ATH_MSG_INFO ("Xi2max: " << m_Xi2max);
      ATH_MSG_INFO ("Xi2maxNoAdd: " << m_Xi2maxNoAdd);                

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
      for (auto *cut : cuts) {
        if (cut->size() == m_noOfEtaBins) continue;
        
        if (cut->size() > m_noOfEtaBins){
          ATH_MSG_ERROR( "No. of cut values bigger than eta bins");
          return StatusCode::FAILURE;
        }
        
        if (cut->size() < m_noOfEtaBins){
          ATH_MSG_DEBUG( "No. of cut values smaller than eta bins. Extending size..." );
          cut->resize(m_noOfEtaBins, cut->back());
        }
      }
      return StatusCode::SUCCESS;
    }

    void InDetEtaDependentCutsSvc::getIndexByEta(double eta, int& bin) {
      
      // setting the default value in case the bin is not found
      bin = -1;
      
      // getting the bin corresponding to the given eta value
      double absEta = fabs(eta);
      for (int i = int(m_noOfEtaBins-1); i >= 0; i--) {
        if (absEta > m_etaBins.at(i)) {
          bin = i;
          return;
        }
      }
      
      // in case you didn't return yet, then the bin is not found!
      ATH_MSG_ERROR( "Eta bin not found! Check the eta bin values in " << name() );
      return;
    }


    void InDetEtaDependentCutsSvc::getValue(InDet::CutName cutName, std::vector < double >& cut) {
      
      // resize the cut vector before setting it
      cut.resize(m_noOfEtaBins);
      
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

      // resize the cut vector before setting it
      cut.resize(m_noOfEtaBins);

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
        
        case InDet::CutName::minInPixelHits:
          cut = m_minInPixelHits;
          break;

        case InDet::CutName::minStripHits:
          cut = m_minStripHits;
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
      return m_maxEta;
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

  double  InDetEtaDependentCutsSvc::getMaxChi2AtEta(double eta) {
      return getValueAtEta<double>(m_Xi2max, eta);
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

    int     InDetEtaDependentCutsSvc::getMinInnermostPixelHitsAtEta    (double eta) {
      return getValueAtEta<int>(m_minInPixelHits, eta);
    }

    int     InDetEtaDependentCutsSvc::getMinStripHitsAtEta    (double eta) {
      return getValueAtEta<int>(m_minStripHits, eta);
    }
      
}   // end namespace
