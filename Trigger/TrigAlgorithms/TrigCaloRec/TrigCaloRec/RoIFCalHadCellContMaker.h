/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCALOREC_ROIFCALHADCELLCONTMAKER_H
#define TRIGCALOREC_ROIFCALHADCELLCONTMAKER_H
/********************************************************************

 NAME:     RoIFCalHadCellContMaker
 PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
 
 AUTHORS:  M.Campanelli/C. Santamarina
 CREATED:  Feb 19, 2008

 PURPOSE:  Fill an RoI CellContainer with the cells of the 
           Forward Hadron Calorimeter using the
           new unpacking prepared for L2
********************************************************************/

#include "TrigCaloRec/IAlgToolEFCalo.h"

//#include "TrigT2CaloCommon/T2GeometryTool.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"

#include "CaloInterface/ICalorimeterNoiseTool.h"

#include "IRegionSelector/IRoiDescriptor.h"

class RoIFCalHadCellContMaker : public IAlgToolEFCalo {
	 public:
           // Constructor
	   RoIFCalHadCellContMaker(const std::string & type,
                                    const std::string & name,
                                    const IInterface* parent);
           // Destructor
           virtual ~RoIFCalHadCellContMaker();

           // AlgTool stages
           StatusCode initialize();
           StatusCode finalize();
           using IAlgToolEFCalo::execute;
           StatusCode execute(CaloCellContainer &pCaloCellContainer, 
			      const IRoiDescriptor& roi );

         private:

           //IT2GeometryTool *m_geometryTool;
           ITrigDataAccess *m_data;

           ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
           int m_do_LArCells_noise_suppression; //<! 0 (1) -> no (yes)
           float m_cutvalue;

           LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
           LArTT_Selector<LArCellCont>::const_iterator m_iEnd;

};
#endif
