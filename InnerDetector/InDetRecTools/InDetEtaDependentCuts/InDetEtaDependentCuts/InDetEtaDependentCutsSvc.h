/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef INDETETADEPENDENTCUTS_INDETETADEPENDENTCUTSSVC_H
#define INDETETADEPENDENTCUTS_INDETETADEPENDENTCUTSSVC_H

// STL includes
#include <string>
#include <vector>


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
      
      void getValue(const InDet::CutName cutName, std::vector < double >& cut);
      void getValue(const InDet::CutName cutName,    std::vector < int >& cut);
      
      template <class T>
      T getValueAtEta(const std::vector< T > cuts, const double eta) const;
    
      template <class T>    
      void getValue(const InDet::CutName cutName, T& cut, const double eta);
      
      double  getMaxEta() const;
      double  getMinPtAtEta           (const double eta) const;
      double  getMaxZImpactAtEta      (const double eta) const;
      double  getMaxPrimaryImpactAtEta(const double eta) const;
      int     getMinSiHitsAtEta       (const double eta) const;
      int     getMinSiNotSharedAtEta  (const double eta) const;
      int     getMaxSharedAtEta       (const double eta) const;
      int     getMinPixelHitsAtEta    (const double eta) const;
      int     getMaxSiHolesAtEta      (const double eta) const;
      int     getMaxPixelHolesAtEta   (const double eta) const;
      int     getMaxSctHolesAtEta     (const double eta) const;
      int     getMaxDoubleHolesAtEta  (const double eta) const;
      
      
    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
    private: 
      
      int getIndexByEta(const double eta) const;
      
      DoubleArrayProperty   m_etaBins              {this, "etaBins"             , {0.0, 4.0}, "eta bins (highest eta is maxEta)"         };
      DoubleArrayProperty   m_minPT                {this, "minPT"               , {900.0}   , "min pT [MeV]"                             };
      DoubleArrayProperty   m_maxPrimaryImpact     {this, "maxPrimaryImpact"    , {2.0}     , "max Rphi IP (primaries) [mm]"             };
      DoubleArrayProperty   m_maxZImpact           {this, "maxZImpact"          , {200.0}   , "max Z IP [mm]"                            };
      DoubleArrayProperty   m_maxdImpactSSSSeeds   {this, "maxdImpactSSSSeeds"  , {20.0}    , "max impact on seeds SSS [mm]"             };
      DoubleArrayProperty   m_etaWidthBrem         {this, "etaWidthBrem"        , {0.2}     , "eta Width of road for brem (cut for brem)"};
      DoubleArrayProperty   m_phiWidthBrem         {this, "phiWidthBrem"        , {0.3}     , "phi Width of road for brem (cut for brem)"};
      DoubleArrayProperty   m_minPTBrem            {this, "minPTBrem"           , {1000.0}  , "min pT for brem reocvery [MeV]"           };
      DoubleArrayProperty   m_Xi2max               {this, "Xi2max"              , {9.0}     , "Xi2 max"                                  };
      DoubleArrayProperty   m_Xi2maxNoAdd          {this, "Xi2maxNoAdd"         , {25.0}    , "Xi2 max no add"                           };
      IntegerArrayProperty  m_minClusters          {this, "minClusters"         , {9}       , "min number of (Si) clusters (Si hits)"    };
      IntegerArrayProperty  m_minPixelHits         {this, "minPixelHits"        , {1}       , "min number of pixel hits"                 };
      IntegerArrayProperty  m_minSiNotShared       {this, "minSiNotShared"      , {7}       , "min number of NOT shared"                 };
      IntegerArrayProperty  m_maxShared            {this, "maxShared"           , {2}       , "max number of shared"                     };
      IntegerArrayProperty  m_maxHoles             {this, "maxHoles"            , {2}       , "max number of Si holes"                   };
      IntegerArrayProperty  m_maxPixelHoles        {this, "maxPixelHoles"       , {1}       , "max number of Pixel holes"                };
      IntegerArrayProperty  m_maxSctHoles          {this, "maxSctHoles"         , {2}       , "max number of SCT holes"                  };
      IntegerArrayProperty  m_maxDoubleHoles       {this, "maxDoubleHoles"      , {1}       , "max number of double holes"               };
      IntegerArrayProperty  m_maxHolesPattern      {this, "maxHolesPattern"     , {2}       , "max holes in pattern"                     };
      IntegerArrayProperty  m_maxHolesGapPattern   {this, "maxHolesGapPattern"  , {2}       , "max holes gap in pattern"                 };
      IntegerArrayProperty  m_nWeightedClustersMin {this, "nWeightedClustersMin", {6}       , "min number of weigthed clusters"          };
      
      
    }; 

    
}   // end namespace



#endif //> !INDETETADEPENDENTCUTS_INDETETADEPENDENTCUTSSVC_H
