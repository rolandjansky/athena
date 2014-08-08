/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     PartialCaloCellContMaker.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   Ademar Delgado, tavares@cern.ch
//
// Description: Fill an partial CellContainer with the cells of the 
//           SuperRoI using the new unpacking prepared for L2
//              Important: Need to receive an TrigSuperRoi that should
//           be made by the CaloSuperRoIMaker
// Updates:
// 
// ********************************************************************

#ifndef TRIGCALOREC_PARTIALCALOCELLCONTMAKER_H
#define TRIGCALOREC_PARTIALCALOCELLCONTMAKER_H

#include "TrigCaloRec/IAlgToolEFCalo.h"
//#include "TrigT2CaloCommon/T2GeometryTool.h"
#include "TrigT2CaloCommon/TrigDataAccess.h"
#include "IRegionSelector/IRoiDescriptor.h"
//#include "CaloInterface/ICalorimeterNoiseTool.h"


class PartialCaloCellContMaker : public IAlgToolEFCalo {
 public:
  // Constructor
	PartialCaloCellContMaker(const std::string & type, const std::string & name,
                                    const IInterface* parent);
  // Destructor
  virtual ~PartialCaloCellContMaker();

  // AlgTool stages
  StatusCode initialize();
  StatusCode finalize();
  using IAlgToolEFCalo::execute;
  StatusCode execute(CaloCellContainer &pCaloCellContainer, const IRoiDescriptor &superRoi);

 private:
 // Functions 
  void fillCellContainer(CaloCellContainer &pCaloCellContainer, const IRoiDescriptor &superRoi, 
                                    const DETID detid=TTEM, const int sampling=0);

 // Properties:
//  BooleanProperty m_CheckCellWithinRoI;

//  IT2GeometryTool *m_geometryTool;
  ITrigDataAccess *m_data;
//  ToolHandle<ICalorimeterNoiseTool> m_noiseTool;
	// configurable for noise suppression on LAr cells and TileCal
//  int m_do_LArCells_noise_suppression; //<! 0 (1) -> no (yes)
//  int m_do_TileCells_noise_suppression; //<! 0 (1) -> no (yes)
//  float m_cutvalue;
//  bool m_absEinSigma;
//  bool m_twogaussiannoise;

 //iterators
  LArTT_Selector<LArCellCont>::const_iterator m_iBegin;
  LArTT_Selector<LArCellCont>::const_iterator m_iEnd;
  LArTT_Selector<LArCellCont>::const_iterator m_it;
  TileCellCollection::const_iterator m_itBegin;
  TileCellCollection::const_iterator m_itEnd;
  TileCellCollection::const_iterator m_itt;
 
 protected: 
 // Properties
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */ 
  MsgStream* m_log;
};
#endif
