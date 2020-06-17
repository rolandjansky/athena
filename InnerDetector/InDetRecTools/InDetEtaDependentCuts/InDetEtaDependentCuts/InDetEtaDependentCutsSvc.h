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
      
      
      int     getMinSiHitsAtEta       (double eta);
      int     getMinSiNotSharedAtEta  (double eta);
      int     getMaxSharedAtEta       (double eta);
      int     getMinPixelHitsAtEta    (double eta);
      int     getMaxSiHolesAtEta      (double eta);
      int     getMaxPixelHolesAtEta   (double eta);
      int     getMaxSctHolesAtEta     (double eta);
      int     getMaxDoubleHolesAtEta  (double eta);
      
      
    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
    private: 
      
      void getIndexByEta(double eta, int& bin);
      
      template <class T>
      StatusCode checkSize(T& cuts);
      
          BooleanProperty m_emulateSplitter{this, "EmulateSplitting", false, "don't split - only emulate the split"};
      
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
