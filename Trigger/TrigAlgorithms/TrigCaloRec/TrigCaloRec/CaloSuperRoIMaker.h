// emacs: this is -*- c++ -*- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CaloSuperRoIMaker.h
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   Ademar Delgado, tavares@cern.ch
//
// Description: Receive all TE of the event and store them in a superRoi.
//              Important: To be used in any Partial Scan before the 
//           TrigCaloCellMaker
// Updates:
// 
// ********************************************************************

#ifndef TRIGCALOREC_CALOSUPERROIMAKER_H
#define TRIGCALOREC_CALOSUPERROIMAKER_H

#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "TrigSteeringEvent/TrigSuperRoi.h"

class CaloSuperRoIMaker: public HLT::AllTEAlgo {
public:
  // Functions
  CaloSuperRoIMaker(const std::string & name, ISvcLocator* pSvcLocator);
  ~CaloSuperRoIMaker();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int output);
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();

  // Properties:

private:
  // Functions
  bool initializeTimers();     //!< initialize the timers
  void resetTimers();          //!< reset the monitoring timers
  void storeTimers();          //!< store timings in the monitoring variables
  void finalizeAccessories();  //!< finalize everything that's not the output
  bool inputIsValid(const std::vector<std::vector<HLT::TriggerElement*> >& input) const;
  HLT::TriggerElement* setupOutputTe(unsigned int type_out) ;   //!< prepare the output trigger element
  HLT::ErrorCode finalizeOutput(HLT::TriggerElement* outputTE); //!< finalize the output TE and attach superRoI

private:
  
  // Properties:
  bool   m_doFS;
  double m_RoISize;
  std::string  m_superRoIOutputKey;
  std::string  m_superRoIUniqueKey;

protected:
 // Functions
  
 // Properties
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
  MsgStream* m_log;
  
  // output
  TrigSuperRoi* m_superRoi;
  
  // Timers to monitor
  TrigTimer *m_fillSuperRoI_timer;    //!< get the TE roiDescriptor and add it to superRoi   (hltExecute)
  TrigTimer *m_settingOutput_timer;   //!< prepare the output TE                              (setupOutputTe & finalizeOutput)
  
  // Monitored Variables
  float               m_fillSuperRoITime;   //!< see corresponding timer for details
  float               m_settingOutputTime;  //!< see corresponding timer for details
  long int            m_TEnumb;             //!< mumber of TE received
  double              m_eta;
  double              m_phi;

};
#endif
