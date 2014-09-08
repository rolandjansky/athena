/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   JetTileFilterAlg.h
 * Author: Stephen Cole <stephen.cole@cern.ch>
 *
 * Created on June 16, 2011, 1:30 PM
 */

#ifndef JetTileFilterALG_H
#define	JetTileFilterALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"
#include "GaudiKernel/ToolHandle.h"

class JetTileFilterAlg: public AthAlgorithm {
public:
    JetTileFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~JetTileFilterAlg();
    
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();
    
    
private:
    std::string m_jetsName;
    std::string m_jetsOut;
    
    float m_jetEtaMin;
    float m_jetEtaMax;
    float m_jetPhiMin;
    float m_jetPhiMax;
};

#endif	/* JetTileFilterALG_H */

