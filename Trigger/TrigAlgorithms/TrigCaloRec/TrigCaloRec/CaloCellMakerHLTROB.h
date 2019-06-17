// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      CaloCellMakerHLTROB
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    Denis O. Damazio
 * CREATED:   2019
 *
 *********************************************************************/
#ifndef TRIGCALOREC_CALOCELLMAKERHLTROB_H
#define TRIGCALOREC_CALOCELLMAKERHLTROB_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"

class IAlgToolEFCalo;
class ICaloCellMakerTool; 
class TrigCaloQuality;
class ITrigDataAccess;

class CaloCellMakerHLTROB : public AthAlgorithm {

 public:
  
  /**  constructor */
  CaloCellMakerHLTROB(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~CaloCellMakerHLTROB();

  /** HLT method to initialize */
  StatusCode initialize();

  /** HLT method to finalize */
  StatusCode finalize();

  /** HLT method to execute FEX algo on a given TE. 
   input is the Triger Element (TE) from the previous algorithm in
   the configuration chain, the output is the result from the
   CaloCellMakerHLTROB execution. **/
  StatusCode execute( );

 private:

  /** Offline Tool names for CellContainer ordering and checking. */
  std::vector<std::string> m_containerToolNames;

  /** Offline Tool pointers for CellContainer ordering and checking. */
  std::vector<ICaloCellMakerTool*> m_containerTools; // pointers to tools

  /** Need access to prefetch */
  ToolHandle<ITrigDataAccess>  m_data;

  /** Produced Calorimeter Cell Container. */
  CaloCellContainer* m_caloCellContainer;

  // monitored variables 

  /** Cell Container size (for monitoring purpouses). */
  double m_CellContainerSize;

   /** EF Calorimeter tools for calorimeter data unpacking. */
  ToolHandleArray<IAlgToolEFCalo>     m_tcrAlgTools;
  
  /** data preparation error */
  std::vector<uint32_t> m_vec_robs;

  bool m_fullScanEnabled;
  // counter
  unsigned int m_counter;

  
};
#endif
