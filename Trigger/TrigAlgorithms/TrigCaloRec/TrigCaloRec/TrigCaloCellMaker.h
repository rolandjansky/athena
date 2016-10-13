// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigCaloCellMaker
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    P.A. Delsart
 * CREATED:   August 2006
 *
 *********************************************************************/
#ifndef TRIGCALOREC_TRIGCALOCELLMAKER_H
#define TRIGCALOREC_TRIGCALOCELLMAKER_H

#include "TrigInterfaces/FexAlgo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"

class IAlgToolEFCalo;
class ICaloCellMakerTool; 
class TrigCaloQuality;
class ITrigDataAccess;

class TrigCaloCellMaker : public HLT::FexAlgo {

 public:
  
  /**  constructor */
  TrigCaloCellMaker(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~TrigCaloCellMaker();

  /** HLT method to initialize */
  HLT::ErrorCode hltInitialize();

  /** HLT method to finalize */
  HLT::ErrorCode hltFinalize();

  /** HLT method to execute FEX algo on a given TE. 
   input is the Triger Element (TE) from the previous algorithm in
   the configuration chain, the output is the result from the
   TrigCaloCellMaker execution. **/
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, 
			    HLT::TriggerElement* outputTE);

  /** code to pre-fetch ROBs now that we do this in
 *  in this algorithm */
  using HLT::FexAlgo::prepareRobRequests;
  HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE );

 private:

  /** Calorimeter second sampling eta granularity. */
  static const double m_detas2;

  /** Calorimeter second sampling phi granularity. */
  static const double m_dphis2;

  /** RoI window size in units of EM calo 2nd sampling cells */
  int m_neta, m_nphi;

  /** EF Calorimeter tools for calorimeter data unpacking. */
  ToolHandleArray<IAlgToolEFCalo>     m_tcrAlgTools;
  
  /** Prescale factor for persistency **/
  unsigned int m_persistencyPSF;
  std::string  m_persistencyKey;  
  unsigned int m_counter;

  /** Offline Tool names for CellContainer ordering and checking. */
  std::vector<std::string> m_containerToolNames;

  /** Offline Tool pointers for CellContainer ordering and checking. */
  //ToolHandleArray<ICaloCellMakerTool> m_containerTools; // pointers to tools
  std::vector<ICaloCellMakerTool*> m_containerTools; // pointers to tools

  /** Pointers to timers */
  std::vector<TrigTimer*> m_timer;

  /** Need access to prefetch */
  ToolHandle<ITrigDataAccess>  m_data;

  /** Produced Calorimeter Cell Container. */
  CaloCellContainer* m_caloCellContainer;

  // monitored variables 

  /** Cell Container size (for monitoring purpouses). */
  double m_CellContainerSize;
  
  /** data preparation error */
  unsigned int m_conversionError[6];
  TrigCaloQuality * m_trigCaloQuality;

  double m_EtaL2, m_PhiL2;
  bool m_fullScanEnabled;

  bool m_trustRoiLimits;

  bool m_createRoiForID;

  double m_etaWidthForID;
  double m_phiWidthForID;

  std::vector<uint32_t> m_vec_robs;

  
//public:
//
//  inline CaloCellContainer* GetCellContainer() const {return pCaloCellContainer;}

};
#endif
