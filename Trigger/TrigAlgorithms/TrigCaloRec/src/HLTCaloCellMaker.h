/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/*
 *   */

/**
 *
 *  NAME:     HLTCaloCellMaker.h
 *   PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloCommon
 *
 *    AUTHOR:   Denis Oliveira Damazio
 *
 *     PURPOSE:  New Algorithm to produce CaloCellContainer
 *      **/

#ifndef TRIGCALOREC_HLTCALOCELLMAKER_H
#define TRIGCALOREC_HLTCALOCELLMAKER_H

#include <string>
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloEvent/CaloBCIDAverage.h"
#include "CaloEvent/CaloCellContainerVector.h"
#include "AthContainers/ConstDataVector.h"
#include "CaloEvent/CaloConstCellContainer.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TileConditions/TileEMScale.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigT2CaloCommon/ITrigCaloDataAccessSvc.h"


/**  Test Algorithm to unpack sequence
 *     produces the CaloCellContainer output. */
class HLTCaloCellMaker: public AthReentrantAlgorithm {
     public:
	HLTCaloCellMaker(const std::string & name, ISvcLocator* pSvcLocator);

	virtual StatusCode execute(const EventContext& context ) const override;
	virtual StatusCode initialize() override;
     private:

	SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey { this, "RoIs", "OutputRoIs", "RoIs to read in" };
	SG::ReadHandleKey<CaloBCIDAverage> m_bcidAvgKey { this, "BCIDAvgKey", "CaloBCIDAverage", "" };

	SG::WriteHandleKey<ConstDataVector<CaloCellContainerVector> > m_cellContainerVKey { this, "CellsVName", "CellsVClusters", "Calo cells container" };
	SG::WriteHandleKey<CaloConstCellContainer > m_cellContainerKey { this, "CellsName", "CellsClusters", "Calo cells container" };

        /// FIXME: Temporary (i hope) to get dependency needed by BS converter.
        SG::ReadCondHandleKey<TileEMScale> m_tileEMScaleKey { this, "TileEMSCaleKey", "TileEMScale", "" };

	ServiceHandle<ITrigCaloDataAccessSvc> m_dataAccessSvc
	  { this, "TrigDataAccessMT", "TrigCaloDataAccessSvc/TrigCaloDataAccessSvc", "Data Access for LVL2 Calo Algorithms in MT" };

	ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };

	Gaudi::Property<bool> m_roiMode { this, "roiMode", true, "RoiMode roi->CaloCellCollection" };
	Gaudi::Property<bool> m_monCells { this, "monitorCells", false ,"monitorCells" };
	Gaudi::Property<bool> m_tileCellsInROI { this, "TileCellsInROI", false, "Require tile cells to be within ROI" };


        // just to help with monitoring
        static inline double getCellPt(const CaloCell* aCell){
          if(!aCell) return -999.0;
          return aCell->et();
        }

	// check if tile cell is within the eta boundaries of the ROI, let RegSelCondAlg_Tile handle phi boundaries
	static inline bool tileCellEtaInRoi(const CaloCell* cell, const TrigRoiDescriptor* roi) {
	  if (cell->eta() < roi->etaMinus() || cell->eta() > roi->etaPlus()) return false;
	  return true;
	}

};

#endif

