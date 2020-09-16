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
    maxShared, maxZImpact, minClusters, minPixelHits, minInPixelHits, minStripHits, minPT,
    IPd0Max, IPz0Max, sigIPd0Max, sigIPz0Max,
    minPTBrem, minSiNotShared, maxHolesGapPattern, maxHolesPattern,
    nWeightedClustersMin, phiWidthBrem, Xi2max, Xi2maxNoAdd
  };
  
  class IInDetEtaDependentCutsSvc : virtual public IInterface {
    
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
  public: 
    
    virtual ~IInDetEtaDependentCutsSvc() {};
    
    static const InterfaceID& interfaceID();
    
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF)=0;
    
    virtual void getValue(InDet::CutName cutName, std::vector < double >& cut) = 0;
    virtual void getValue(InDet::CutName cutName,    std::vector < int >& cut) = 0;

    template <class T>    
    void getValue(InDet::CutName cutName, T& cut, double eta) {}
    
    virtual double  getMaxEta()                          = 0;
    virtual double  getMinPtAtEta           (double eta) = 0;
    virtual double  getMaxZImpactAtEta      (double eta) = 0;
    virtual double  getMaxPrimaryImpactAtEta(double eta) = 0;
    virtual double  getMaxChi2AtEta         (double eta) = 0;
    //
    virtual double  getIPd0MaxAtEta         (double eta) = 0;
    virtual double  getIPz0MaxAtEta         (double eta) = 0;
    virtual double  getSigIPd0MaxAtEta      (double eta) = 0;
    virtual double  getSigIPz0MaxAtEta      (double eta) = 0;
    //x    
    virtual int     getMinSiHitsAtEta       (double eta) = 0;
    virtual int     getMinSiNotSharedAtEta  (double eta) = 0;
    virtual int     getMaxSharedAtEta       (double eta) = 0;
    virtual int     getMinPixelHitsAtEta    (double eta) = 0;
    virtual int     getMaxSiHolesAtEta      (double eta) = 0;
    virtual int     getMaxPixelHolesAtEta   (double eta) = 0;
    virtual int     getMaxSctHolesAtEta     (double eta) = 0;
    virtual int     getMaxDoubleHolesAtEta  (double eta) = 0;
    virtual int     getMinInnermostPixelHitsAtEta(double eta) = 0;
    virtual int     getMinStripHitsAtEta    (double eta) = 0;
      
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
