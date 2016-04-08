/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_MUONROICHAINSVC_H
#define TRIGGER_MUONROICHAINSVC_H

#include "TrigMuonCoinHierarchy/IMuonRoiChainSvc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/ToolHandle.h"
#include "TrigMuonCoinHierarchy/IMuonRoiChainFindTool.h"

namespace Trigger {
  class MuonRoiChainSvc : public AthService, virtual public IMuonRoiChainSvc, virtual public IIncidentListener {
  public: 
    /** Constructor */
    MuonRoiChainSvc(const std::string& name, ISvcLocator* svc);
    /** Destructor */
    virtual ~MuonRoiChainSvc();

    /** Provide InterfaceID */
    static const InterfaceID & interfaceID() { return IMuonRoiChainSvc::interfaceID(); };
    /** Query interface */
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIF);
    
    /** Intialize */
    virtual StatusCode initialize(void);
    /** Finalize */
    virtual StatusCode finalize(void);

    /** Get MuonRoiChain from an index of a Muon_ROI in LVL1_ROI::muons_type */
    virtual StatusCode getChainWithRoiIndex(const MuonRoiChain** chain, int roiIndex);

    /** Get MuonRoiChain from a Muon_ROI */
    virtual StatusCode getChain(const MuonRoiChain** chain, const Muon_ROI* muonRoi);
    /** Get MuonRoiChain from a data word of MuCTPI_RDO */
    virtual StatusCode getChain(const MuonRoiChain** chain, uint32_t muctpiRdoDataWord);
    /** Get MuonRoiChain from a TgcCoinData (Sector Logic) */
    virtual StatusCode getChain(const MuonRoiChain** chain, const Muon::TgcCoinData* coin);
    /** Get all MuonRoiChains in an event */
    virtual StatusCode getChains(std::vector<const MuonRoiChain*>* chains);
 
    /** To know a new event by IIncidentListener */
    virtual void handle(const Incident& inc);

  private:
    /** Make MuonRoiChains using IMuonRoiChainFindTool */
    virtual StatusCode execute(void);

    /** Initializing and deleting variables */
    void prepareForNewEvent(void);

    /** IMuonRoiChainFindTool */
    ToolHandle<IMuonRoiChainFindTool> m_findTool;
    /** Data member of MuonRoiChains to hold inside MuonRoiChainSvc */
    std::vector<MuonRoiChain*> m_chainVector;
    /** Flag of DEBUG level */ 
    bool m_debug;
    /** Flag of a new event */
    bool m_newEvent;
  };
  
} // end of namespace Trigger

#endif // TRIGGER_MUONROICHAINSVC_H 
