// Hi Emacs ! this is  -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
 *
 * NAME:      TrigCaloCellMakerMT
 * PACKAGE:   Trigger/TrigAlgorithms/TrigCaloRec
 *
 * AUTHOR:    P.A. Delsart
 * CREATED:   August 2006
 *
 *********************************************************************/
#ifndef TRIGCALORECMT_TRIGCALOCELLMAKER_H
#define TRIGCALORECMT_TRIGCALOCELLMAKER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrigCaloRec/IAlgToolEFCalo.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CaloEvent/CaloCellContainer.h"
#include "TrigCaloRec/TrigCaloQuality.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"

class IAlgToolEFCalo;
class ICaloCellMakerTool; 
class TrigCaloQuality;
class ITrigCaloDataAccessSvc;

class TrigCaloCellMakerMT : public AthAlgorithm {

 public:
  
  /**  constructor */
  TrigCaloCellMakerMT(const std::string& name, ISvcLocator* pSvcLocator);

  /** destructor */
  ~TrigCaloCellMakerMT();

  /** HLT method to initialize */
  StatusCode initialize();

  /** HLT method to finalize */
  StatusCode finalize();

  /** HLT method to execute FEX algo on a given TE. 
   input is the Triger Element (TE) from the previous algorithm in
   the configuration chain, the output is the result from the
   TrigCaloCellMakerMT execution. **/
  StatusCode execute();

 private:

  /** Calorimeter second sampling eta granularity. */
  static const double m_detas2;

  /** Calorimeter second sampling phi granularity. */
  static const double m_dphis2;

  /** RoI window size in units of EM calo 2nd sampling cells */
  int m_neta, m_nphi;

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
  //  std::vector<TrigTimer*> m_timer;

  ServiceHandle<ITrigCaloDataAccessSvc> m_dataAccessSvc;

  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_inputRoiKey{ this,
      "RoIs",         // property name
      "rois",                                                  // default value of StoreGate key
      "input RoI collection name"};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_outputCaloRoiKey{ this,
      "OutputCaloRoi",                  // property name
      "RoiForCalo",                                             // default value of StoreGate key
      "output Calo RoI collection name "};

  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_outputIDRoiKey{ this,
      "OutputIDRoi",                  // property name
      "RoiForID",                                             // default value of StoreGate key
      "output Calo RoI collection name "};

  SG::WriteHandleKey<CaloCellContainer> m_outputCellsKey{ this,
      "Cells",                  // property name
      "cells",                                             // default value of StoreGate key
      "output CaloCellContainer "};

  SG::WriteHandleKey<ConstDataVector<CaloCellContainer> > m_outputCDVcellsKey{ this,
      "CVDcells",                  // property name
      "CDVcells",                                             // default value of StoreGate key
      "output ConstDataVector<CaloCellContainer> "};

  SG::WriteHandleKey<TrigCaloQuality> m_outputTrigCaloQualityKey{ this,
      "CaloQuality",                  // property name
      "caloquality",                                             // default value of StoreGate key
      "output TrigCaloQuality"};


  /** Produced Calorimeter Cell Container. */
  CaloCellContainer* m_caloCellContainer;

  
  /** data preparation error */
  //  unsigned int m_conversionError[6];
  TrigCaloQuality * m_trigCaloQuality;

  //  double m_EtaL2, m_PhiL2;
  bool m_fullScanEnabled;

  bool m_trustRoiLimits;

  bool m_createRoiForID;

  double m_etaWidthForID;
  double m_phiWidthForID;

  //  std::vector<uint32_t> m_vec_robs;

  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
  
//public:
//
//  inline CaloCellContainer* GetCellContainer() const {return pCaloCellContainer;}

};
#endif
