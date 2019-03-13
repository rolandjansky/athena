/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOREC_ROITILECELLCONTMAKER_H
#define TRIGCALOREC_ROITILECELLCONTMAKER_H
/********************************************************************

 NAME:     RoITileCellContMaker
 PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
 
 AUTHORS:  C. Santamarina
 CREATED:  June 12, 2007

 PURPOSE:  Fill an RoI CellContainer with the cells of the Tile
           Calorimeter using the new unpacking prepared for L2
********************************************************************/

#include "TrigCaloRec/IAlgToolEFCalo.h"

//#include "TrigT2CaloCommon/T2GeometryTool.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"

#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "IRegionSelector/IRoiDescriptor.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

class RoITileCellContMaker : public IAlgToolEFCalo {
	 public:
           // Constructor
	   RoITileCellContMaker(const std::string & type,
                                    const std::string & name,
                                    const IInterface* parent);
           // Destructor
           virtual ~RoITileCellContMaker();

           // AlgTool stages
           StatusCode initialize();
           StatusCode finalize();
           using IAlgToolEFCalo::execute;
           StatusCode execute(CaloCellContainer &pCaloCellContainer, 
			      const IRoiDescriptor& roi );

         private:
           BooleanProperty m_CheckCellWithinRoI;

           //IT2GeometryTool *m_geometryTool;
           ITrigDataAccess *m_data;
           ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
           int m_do_TileCells_noise_suppression; //<! 0 (1) -> no (yes)
           float m_cutvalue;
           bool m_absEinSigma;
           bool m_twogaussiannoise;

           TileCellCollection::const_iterator m_iBegin;
           TileCellCollection::const_iterator m_iEnd;

};
#endif
