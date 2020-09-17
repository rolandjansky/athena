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
      declareProperty("IPsigd0Max",              m_sigIPd0Max           =      {999.0}); // error d0Max 
      declareProperty("IPsigz0Max",              m_sigIPz0Max           =      {999.0}); // error z0Max 
      declareProperty("IPd0Max",                 m_IPd0Max              =         {2.}); // mm
      declareProperty("IPz0Max",                 m_IPz0Max              =        {1.5}); // mm
      //
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
                                                               &m_Xi2maxNoAdd         ,
      							       &m_IPd0Max             ,
							       &m_IPz0Max             ,
							       &m_sigIPd0Max          ,
							       &m_sigIPz0Max          };
                                            
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
      ATH_MSG_INFO ("d0Max: " << m_IPd0Max);
      ATH_MSG_INFO ("z0Max: " << m_IPz0Max);
      ATH_MSG_INFO ("sigd0Max: " << m_sigIPd0Max);
      ATH_MSG_INFO ("sigz0Max: " << m_sigIPz0Max);
      ATH_MSG_INFO ("minPTBrem: " << m_minPTBrem);
      ATH_MSG_INFO ("minSiNotShared: " << m_minSiNotShared);
      ATH_MSG_INFO ("nHolesGapMax: " << m_maxHolesGapPattern);
      ATH_MSG_INFO ("nHolesMax: " << m_maxHolesPattern);
      ATH_MSG_INFO ("nWeightedClustersMin: " << m_nWeightedClustersMin);
      ATH_MSG_INFO ("phiWidthBrem: " << m_phiWidthBrem);
      ATH_MSG_INFO ("Xi2max: " << m_Xi2max);
      ATH_MSG_INFO ("Xi2maxNoAdd: " << m_Xi2maxNoAdd);                

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
      m_mapDoubleCuts[InDet::CutName::IPd0Max]            = m_IPd0Max;
      m_mapDoubleCuts[InDet::CutName::IPz0Max]            = m_IPz0Max;
      m_mapDoubleCuts[InDet::CutName::sigIPd0Max]         = m_sigIPd0Max;
      m_mapDoubleCuts[InDet::CutName::sigIPz0Max]         = m_sigIPz0Max;

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

      std::unordered_map< InDet::CutName, std::vector<double> >::iterator it = m_mapDoubleCuts.find(cutName);

      if(it!=m_mapDoubleCuts.end()) cut = it->second;
      else ATH_MSG_ERROR("CutName not recognized. Cuts will remain unchanged.");

    }
    
    void InDetEtaDependentCutsSvc::getValue(InDet::CutName cutName,    std::vector < int >& cut) {

      // resize the cut vector before setting it
      cut.resize(m_noOfEtaBins);

      std::unordered_map< InDet::CutName, std::vector<int> >::iterator it = m_mapIntCuts.find(cutName);

      if(it!=m_mapIntCuts.end()) cut = it->second;
      else ATH_MSG_ERROR("CutName not recognized. Cuts will remain unchanged.");

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
  //
    double InDetEtaDependentCutsSvc::getIPd0MaxAtEta(double eta) {
      return getValueAtEta<double>(m_IPd0Max,eta);
    }  
    double InDetEtaDependentCutsSvc::getIPz0MaxAtEta(double eta) {
      return getValueAtEta<double>(m_IPz0Max,eta);
    }  
    double InDetEtaDependentCutsSvc::getSigIPd0MaxAtEta(double eta) {
      return getValueAtEta<double>(m_sigIPd0Max,eta);
    }  
    double InDetEtaDependentCutsSvc::getSigIPz0MaxAtEta(double eta) {
      return getValueAtEta<double>(m_sigIPz0Max,eta);
    }  
  //
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
