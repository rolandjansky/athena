/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 *   */

/**
 *
 *   NAME:     HLTCaloCellSumMaker.h
 *   PACKAGE:  Trigger/TrigDataAccess/TrigCaloDataAccessSvc
 *
 *   AUTHOR:   Denis Oliveira Damazio
 *
 *   PURPOSE:  New Algorithm to produce CaloCellContainer
 *   **/

#ifndef TRIGCALODATAACCESSSVC_HLTCALOCELLSUMMAKER_H
#define TRIGCALODATAACCESSSVC_HLTCALOCELLSUMMAKER_H
#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadHandleKey.h"
#include "CaloEvent/CaloCellContainerVector.h"
#include "AthContainers/ConstDataVector.h"


/** Main LVL2 Algorithm. Processes LVL1 information, call FEX
 *  *     IAlgToolCalos and produces the TrigEMCluster output. */
class HLTCaloCellSumMaker: public AthReentrantAlgorithm {
     public:
        HLTCaloCellSumMaker(const std::string & name, ISvcLocator* pSvcLocator);
        ~HLTCaloCellSumMaker();

        StatusCode execute_r(const EventContext& context ) const ;
        StatusCode initialize();
     private:

        SG::ReadHandleKey<ConstDataVector<CaloCellContainerVector> > m_cellContainerVKey;
	SG::ReadHandle<ConstDataVector<CaloCellContainerVector> > m_cellContainerV;
};

#endif
