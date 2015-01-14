/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/**
 // filename: TrigRoiUpdater.h
 //
 // author: Jiri Masik 
 //
 // Description: signature specific RoI manipulation for the ID reconstruction
 //              uniform attachment of the RoI to ease an analysis
 */
////////////////////////////////////////////////////////////////////////////

#ifndef INDETTRIGROIUPDATER_H
#define INDETTRIGROIUPDATER_H

#include "GaudiKernel/ToolHandle.h"


//!< Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"


namespace PESA

{

  class TrigRoiUpdater : public HLT::FexAlgo {
  public:
    TrigRoiUpdater(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrigRoiUpdater();
    HLT::ErrorCode hltBeginRun();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* input, HLT::TriggerElement* output);
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltEndRun();

  private:
    
    float m_etaHalfWidth;
    float m_phiHalfWidth;

    //monitoring
    float m_inpPhiMinus,m_inpPhiPlus, m_inpPhiSize;
    float m_inpEtaMinus,m_inpEtaPlus, m_inpEtaSize;
    float m_PhiMinus,m_PhiPlus, m_PhiSize;
    float m_EtaMinus,m_EtaPlus, m_EtaSize;
  };

}

#endif
