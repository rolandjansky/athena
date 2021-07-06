/*
 *   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 *   */

/**
 *
 *   NAME:     HLTCaloCellSumMaker.h
 *   PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 *
 *   AUTHOR:   Denis Oliveira Damazio
 *
 *   PURPOSE:  New Algorithm to produce CaloCellContainer
 *   **/

#ifndef TRIGT2CALOCOMMON_HLTCALOCELLSUMMAKER_H
#define TRIGT2CALOCOMMON_HLTCALOCELLSUMMAKER_H
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

        virtual StatusCode execute(const EventContext& context ) const override;
        virtual StatusCode initialize() override;
     private:

        SG::ReadHandleKey<ConstDataVector<CaloCellContainerVector> > m_cellContainerVKey;
	SG::ReadHandle<ConstDataVector<CaloCellContainerVector> > m_cellContainerV;
        SG::ReadHandleKey<CaloCellContainer > m_cellContainerKey;
	SG::ReadHandle<CaloCellContainerVector > m_cellContainer;
        bool m_roiMode;
};

#endif
