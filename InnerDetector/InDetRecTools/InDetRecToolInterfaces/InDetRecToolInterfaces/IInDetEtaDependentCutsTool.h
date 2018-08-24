/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IINDETETADEPENDENTCUTSTOOL_H
#define IINDETETADEPENDENTCUTSTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/Track.h"

namespace InDet {
  
  static const InterfaceID IID_IInDetEtaDependentCutsTool("IInDetEtaDependentCutsTool", 1, 0);

  enum CutName {  
    etaBins, etaWidthBrem, maxdImpactSSSSeeds, maxDoubleHoles,
    maxHoles, maxPixelHoles, maxPrimaryImpact, maxSctHoles, 
    maxShared, maxZImpact, minClusters, minPixelHits, minPT,
    minPTBrem, minSiNotShared, maxHolesGapPattern, maxHolesPattern,
    nWeightedClustersMin, phiWidthBrem, Xi2max, Xi2maxNoAdd
  };
  
  class IInDetEtaDependentCutsTool : virtual public IAlgTool {
    
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
  public: 
    
    virtual ~IInDetEtaDependentCutsTool() {};
    
    static const InterfaceID& interfaceID();
    
    virtual void getValue(InDet::CutName cutName, std::vector < double >& cut) = 0;
    virtual void getValue(InDet::CutName cutName,    std::vector < int >& cut) = 0;

    template <class T>    
    void getValue(InDet::CutName cutName, T& cut, double eta) {}
    
    virtual double  getMaxEta()                          = 0;
    virtual double  getMinPtAtEta           (double eta) = 0;
    virtual double  getMaxZImpactAtEta      (double eta) = 0;
    virtual double  getMaxPrimaryImpactAtEta(double eta) = 0;
    virtual int     getMinSiHitsAtEta       (double eta) = 0;
    virtual int     getMinSiNotSharedAtEta  (double eta) = 0;
    virtual int     getMaxSharedAtEta       (double eta) = 0;
    virtual int     getMinPixelHitsAtEta    (double eta) = 0;
    virtual int     getMaxSiHolesAtEta      (double eta) = 0;
    virtual int     getMaxPixelHolesAtEta   (double eta) = 0;
    virtual int     getMaxSctHolesAtEta     (double eta) = 0;
    virtual int     getMaxDoubleHolesAtEta  (double eta) = 0;
      
  };
  
  /////////////////////////////////////////////////////////////////// 
  /// Inline methods: 
  /////////////////////////////////////////////////////////////////// 
  inline const InterfaceID& IInDetEtaDependentCutsTool::interfaceID() 
  { 
    return IID_IInDetEtaDependentCutsTool; 
  }

}// end namespace

#endif //> !IINDETETADEPENDENTCUTSTOOL_H
