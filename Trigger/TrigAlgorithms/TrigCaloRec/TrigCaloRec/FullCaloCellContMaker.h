/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOREC_FULLCALOCELLCONTMAKER_H
#define TRIGCALOREC_FULLCALOCELLCONTMAKER_H
/*************************************************************

 NAME:     FullCaloCellContMaker
 PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
 
 AUTHORS:  C. Santamarina
 CREATED:  June 12, 2007

 PURPOSE:  Fill an RoI CellContainer with the cells of the 
           Liquid Argon Electromagnetic Calorimeter using the
           new unpacking prepared for L2
********************************************************************/

#include "TrigCaloRec/IAlgToolEFCalo.h"

//#include "TrigT2CaloCommon/T2GeometryTool.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"

#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "AthenaMonitoring/IMonitorToolBase.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

class FullCaloCellContMaker : public IAlgToolEFCalo {
	 public:
           // Constructor
           FullCaloCellContMaker(const std::string & type,
                                    const std::string & name,
                                    const IInterface* parent);
           // Destructor
           virtual ~FullCaloCellContMaker();

           // AlgTool stages
           StatusCode initialize();
           StatusCode finalize();
           using IAlgToolEFCalo::execute;
           StatusCode execute(CaloCellContainer &pCaloCellContainer);


	   StatusCode addFullLArCells(CaloCellContainer &pCaloCellContainer, DETID detectorID, bool prepare);
	   StatusCode addFullTileCells(CaloCellContainer &pCaloCellContainer, /* int sampling,*/ bool prepare);
	   StatusCode addFullDetCells(CaloCellContainer &pCaloCellContainer);

         private:

           ITrigDataAccess *m_data;

	   ToolHandle<ICalorimeterNoiseTool> m_noiseTool;

	   // properties for constant calibration
	   //double const_cal_em;
	   //double const_cal_had;
	   //bool m_useFullColl;

	   LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
	   LArTT_Selector<LArCellCont>::const_iterator m_iEnd;
	   LArTT_Selector<LArCellCont>::const_iterator m_it;

	   // configurable for noise suppression on LAr cells and TileCal
	   int m_do_LArCells_noise_suppression; //<! 0 (1) -> no (yes)
	   int m_do_TileCells_noise_suppression; //<! 0 (1) -> no (yes)

           float m_cutvalue;

	   TileCellCollection::const_iterator m_itBegin;
	   TileCellCollection::const_iterator m_itEnd;
	   TileCellCollection::const_iterator m_itt;
	   CaloCellContainer::const_iterator m_icBegin;
	   CaloCellContainer::const_iterator m_icEnd;
	   CaloCellContainer::const_iterator m_itc;

	   bool  m_absEinSigma;
	   bool  m_twogaussiannoise;
};
#endif
