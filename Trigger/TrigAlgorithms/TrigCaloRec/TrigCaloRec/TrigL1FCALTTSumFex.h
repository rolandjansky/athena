/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



 /* *******************************************************
 * NAME : TrigL1FCALTTSumFex.h
 * PACKAGE : Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR : Denis Oliveira Damazio
 * CREATED : 2016, September
 *
 *********************************************************/

#ifndef TRIGCALOREC_TRIGL1FCALTTSUMFEX_H
#define TRIGCALOREC_TRIGL1FCALTTSUMFEX_H

#include "TrigInterfaces/FexAlgo.h"

class TrigL1FCALTTSumFex : public HLT::FexAlgo {

public :
        TrigL1FCALTTSumFex( const std::string& name, ISvcLocator* pSvcLocator );
        ~TrigL1FCALTTSumFex();

        HLT::ErrorCode hltInitialize();
        HLT::ErrorCode hltFinalize();
        HLT::ErrorCode hltExecute ( const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
private :
        std::string m_HIEventShapeContainerKey;

};
#endif
