/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETETADEPENDENTCUTS_INDETETADEPENDENTCUTSSVC_H
#define INDETETADEPENDENTCUTS_INDETETADEPENDENTCUTSSVC_H

// STL includes
#include <string>
#include <map>

// FrameWork includes
#include "AthenaBaseComps/AthService.h"

// InDetRecInterfaces includes
#include "InDetRecToolInterfaces/IInDetEtaDependentCutsSvc.h"

#include "GaudiKernel/ISvcLocator.h"

namespace InDet {
  class InDetEtaDependentCutsSvc: virtual public IInDetEtaDependentCutsSvc,
                                  public AthService {

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public:
      
      /// Constructor with parameters: 
      InDetEtaDependentCutsSvc(const std::string& name, ISvcLocator* sl); 
      
      /// Destructor: 
      virtual ~InDetEtaDependentCutsSvc(); 
      
      virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF); 
      
      StatusCode  initialize();
      StatusCode  finalize();
      
      void getValue(InDet::CutName cutName, std::vector < double >& cut) ;
      void getValue(InDet::CutName cutName,    std::vector < int >& cut) ;
      
      template <class T>
      T getValueAtEta(std::vector< T > cuts, double eta);
    
      template <class T>    
      void getValue(InDet::CutName cutName, T& cut, double eta);
      
      double  getMaxEta();
      double  getMinPtAtEta           (double eta);
      double  getMaxZImpactAtEta      (double eta);
      double  getMaxPrimaryImpactAtEta(double eta);
      double  getMaxChi2AtEta         (double eta);
      double  getSigIPd0MaxAtEta      (double eta);
      double  getSigIPz0MaxAtEta      (double eta);
      
      int     getMinSiHitsAtEta       (double eta);
      int     getMinSiNotSharedAtEta  (double eta);
      int     getMaxSharedAtEta       (double eta);
      int     getMinPixelHitsAtEta    (double eta);
      int     getMaxSiHolesAtEta      (double eta);
      int     getMaxPixelHolesAtEta   (double eta);
      int     getMaxSctHolesAtEta     (double eta);
      int     getMaxDoubleHolesAtEta  (double eta);

      int     getMinInnermostPixelHitsAtEta (double eta);
      int     getMinStripHitsAtEta     (double eta);

      
    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
    private: 
      
      void getIndexByEta(double eta, int& bin);
      
      template <class T>
      StatusCode checkSize(T& cuts);
      
      unsigned int         m_noOfEtaBins          ; 
      double               m_maxEta               ; // max eta value
      std::vector<double>  m_etaBins              ; // eta bins (highest eta is maxEta) 
      std::vector<double>  m_etaWidthBrem         ; // cut for brem
      std::vector<double>  m_maxdImpactSSSSeeds   ; // max impact on seeds SSS
      std::vector<double>  m_maxPrimaryImpact     ; // max Rphi IP (primaries) 
      std::vector<double>  m_maxZImpact           ; // max Z IP
      std::vector<double>  m_minPT                ; // min pT
      std::vector<double>  m_minPTBrem            ; // min pT for brem reocvery
      std::vector<double>  m_phiWidthBrem         ; // phi Width of road for brem (cut for brem)
      std::vector<double>  m_Xi2max               ; // Xi2 max
      std::vector<double>  m_Xi2maxNoAdd          ; // Xi2 max no add
      std::vector<double>  m_sigIPd0Max           ; // max sigma IP d0
      std::vector<double>  m_sigIPz0Max           ; // max sigma IP z0

      std::vector<int>     m_maxDoubleHoles       ; // max number of double holes
      std::vector<int>     m_maxHoles             ; // max number of Si holes 
      std::vector<int>     m_maxPixelHoles        ; // max number of Pixel holes
      std::vector<int>     m_maxSctHoles          ; // max number of SCT holes
      std::vector<int>     m_maxShared            ; // max number of shared
      std::vector<int>     m_minClusters          ; // min number of (Si) clusters (Si hits)
      std::vector<int>     m_minPixelHits         ; // min number of pixel hits 
      std::vector<int>     m_minSiNotShared       ; // min number of NOT shared 
      std::vector<int>     m_maxHolesGapPattern   ; // max holes gap in pattern
      std::vector<int>     m_maxHolesPattern      ; // max holes in pattern
      std::vector<int>     m_nWeightedClustersMin ; // nWeightedClustersMin

      std::vector<int>     m_minInPixelHits       ; // min number of pixel hits in the innermost layer
      std::vector<int>     m_minStripHits         ; // min number of strip hits
      
      std::unordered_map< InDet::CutName, std::vector<double> > m_mapDoubleCuts; // double min/max stored in a single object
      std::unordered_map< InDet::CutName, std::vector<int> >    m_mapIntCuts; // int min/max stored in a single object


    }; 

    
}   // end namespace



#endif //> !INDETETADEPENDENTCUTS_INDETETADEPENDENTCUTSSVC_H
