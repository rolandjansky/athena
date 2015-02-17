/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __D3PD_TrigEMRingerFillerTool_h__
#define __D3PD_TrigEMRingerFillerTool_h__
/*
  D3PD::TrigEMRingerFillerTool.h
*/
#include <vector>
#include <string>
#include "D3PDMakerUtils/BlockFillerTool.h"
#include "TrigCaloEvent/TrigEMCluster.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigCaloEvent/TrigRNNOutputContainer.h"


namespace D3PD {

  class TrigEMRingerFillerTool : public D3PD::BlockFillerTool<TrigEMCluster>,
                                 public IIncidentListener {
  public:
    TrigEMRingerFillerTool(const std::string& type, const std::string& name, 
			    const IInterface* parent);
    ~TrigEMRingerFillerTool() {};
    
    StatusCode book();
    StatusCode fill(const TrigEMCluster& x);

    //These 2 methods below are for handling the store gate incident (clear).
    virtual StatusCode initialize();
    virtual void handle (const Incident& inc);

  private:
    // Properties
    bool m_WriteBasics;
    bool m_WriteHypoVars;
    bool m_WriteDetails;

    // D3PD variables
    std::vector<float> *m_rings;
    std::vector<float> *m_nnOut;
    int *m_emClusIdx;

    /// Property: Incident service.
    ServiceHandle<IIncidentSvc> m_incSvc;
    
    /// For store gate access.
    StoreGateSvc *m_pStoreGate;
    const TrigRNNOutputContainer *m_sgNNOut;
    const std::string m_sgKeyName;

    unsigned m_emClusCount;
  };

}

#endif // __D3PD_TrigEMRingerFillerTool_h__

