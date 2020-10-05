/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IINDETETADEPENDENTCUTSSVC_H
#define IINDETETADEPENDENTCUTSSVC_H

#include "GaudiKernel/IInterface.h"

namespace InDet {
  
  static const InterfaceID IID_IInDetEtaDependentCutsSvc("IInDetEtaDependentCutsSvc", 1, 0);

  enum CutName {  
    etaBins, etaWidthBrem, maxdImpactSSSSeeds, maxDoubleHoles,
    maxHoles, maxPixelHoles, maxPrimaryImpact, maxSctHoles, 
    maxShared, maxZImpact, minClusters, minPixelHits, minPT,
    minPTBrem, minSiNotShared, maxHolesGapPattern, maxHolesPattern,
    nWeightedClustersMin, phiWidthBrem, Xi2max, Xi2maxNoAdd,
    numberOfCutCategories
  };
  
  class IInDetEtaDependentCutsSvc : virtual public IInterface {
    
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
  public: 
    
    virtual ~IInDetEtaDependentCutsSvc() {};
    
    static const InterfaceID& interfaceID();
    
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF)=0;
    
    virtual void getValue(const InDet::CutName cutName, std::vector < double >& cut) = 0;
    virtual void getValue(const InDet::CutName cutName, std::vector < int >& cut) = 0;

    
    virtual double  getMaxEta() const                         = 0;
    virtual double  getMinPtAtEta           (const double eta) const = 0;
    virtual double  getMaxZImpactAtEta      (const double eta) const = 0;
    virtual double  getMaxPrimaryImpactAtEta(const double eta) const = 0;
    virtual int     getMinSiHitsAtEta       (const double eta) const = 0;
    virtual int     getMinSiNotSharedAtEta  (const double eta) const = 0;
    virtual int     getMaxSharedAtEta       (const double eta) const = 0;
    virtual int     getMinPixelHitsAtEta    (const double eta) const = 0;
    virtual int     getMaxSiHolesAtEta      (const double eta) const = 0;
    virtual int     getMaxPixelHolesAtEta   (const double eta) const = 0;
    virtual int     getMaxSctHolesAtEta     (const double eta) const = 0;
    virtual int     getMaxDoubleHolesAtEta  (const double eta) const = 0;
      
  };
  
  /////////////////////////////////////////////////////////////////// 
  /// Inline methods: 
  /////////////////////////////////////////////////////////////////// 
  inline const InterfaceID& IInDetEtaDependentCutsSvc::interfaceID() 
  { 
    return IID_IInDetEtaDependentCutsSvc; 
  }

}// end namespace

#endif //> !IINDETETADEPENDENTCUTSSVC_H
