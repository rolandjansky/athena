/*   
     Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETACCELERATIONSERVICE_TRIGINDETACCELERATIONSVC_H
#define TRIGINDETACCELERATIONSERVICE_TRIGINDETACCELERATIONSVC_H

#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/IIncidentSvc.h" 

#include "TrigInDetAccelerationService/ITrigInDetAccelerationSvc.h" 

#include "AthenaBaseComps/AthService.h" 

#include "GaudiKernel/ServiceHandle.h" 

#include "StoreGate/StoreGateSvc.h" 

#include "TrigAccelEvent/WorkFactory.h"

#include<vector>
#include<map>
#include<cstring>

/// Service for creating GPU-accelerated Work items for HLT ID algorithms

class TrigInDetAccelerationSvc : public extends<AthService, ITrigInDetAccelerationSvc, IIncidentListener> {

 public:   
  
  TrigInDetAccelerationSvc( const std::string&, ISvcLocator*);
   
  virtual ~TrigInDetAccelerationSvc() override {};
  
  virtual StatusCode initialize() override;   
  virtual StatusCode finalize() override;
  virtual void handle(const Incident&) override;

  virtual bool isReady() const override {
    return m_factoryConfigured;
  }
  
  virtual TrigAccel::Work* createWork(unsigned int, std::shared_ptr<TrigAccel::OffloadBuffer>) const override;
  virtual const std::vector<short>& getLayerInformation(int) const override;

 private:   

  struct PhiEtaHash {

    struct compare {
    public:
      bool operator()(const struct PhiEtaHash& p1, const struct PhiEtaHash& p2) {
	if(p1.m_phiIndex == p2.m_phiIndex) {
	  return p1.m_etaIndex < p2.m_etaIndex;
	}
	else {
	  return p1.m_phiIndex < p2.m_phiIndex;
	}
      }
    };
    
  public:
  PhiEtaHash(short phi, short eta, int hash) : m_phiIndex(phi), m_etaIndex(eta), m_hash(hash) {};
    short m_phiIndex, m_etaIndex;
    int m_hash;
  };

  int m_nDCs;
  std::string m_moduleName;
  void* m_libHandle; //for OffloadFactory
  TrigAccel::WorkFactory* m_pWF;
  ServiceHandle<StoreGateSvc> m_detStore; 
  ServiceHandle<StoreGateSvc> m_evtStore;   

  bool m_factoryConfigured;

  bool exportGeometryInformation(const std::map<std::tuple<short,short,short>, std::vector<PhiEtaHash> >&) const;
  bool extractGeometryInformation(std::map<std::tuple<short,short,short>, std::vector<PhiEtaHash> >&);

  std::vector<short> m_layerInfo[3];
  
};



#endif
