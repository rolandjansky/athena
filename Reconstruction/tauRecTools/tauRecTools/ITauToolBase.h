/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITOOLBASE_TAU_H
#define ITOOLBASE_TAU_H

#include "AsgTools/IAsgTool.h"
#include "tauRecTools/TauEventData.h"

/**
 * @brief The base class for all tau tools.
 * 
 * @author Lukasz Janyst
 * @author Justin Griffiths
 * Thanks to Lianyou Shan, Lorenz Hauswald
 */

class ITauToolBase : virtual public asg::IAsgTool
{
 public:

  ASG_TOOL_INTERFACE(ITauToolBase)    

  virtual ~ITauToolBase() {}

  //-----------------------------------------------------------------
  //! Tool initializer
  //-----------------------------------------------------------------
  virtual StatusCode initialize() = 0;

  //-----------------------------------------------------------------
  //! inTrigger - whether the tool is executed at trigger level
  //-----------------------------------------------------------------
  virtual bool inTrigger() const = 0;

  //-----------------------------------------------------------------
  //! Event initializer - called at the beginning of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventInitialize() = 0;

  //-----------------------------------------------------------------
  //! Execute - called for each tau candidate
  //-----------------------------------------------------------------
  virtual StatusCode execute(xAOD::TauJet& pTau) = 0;

  //-----------------------------------------------------------------
  //! Event finalizer - called at the end of each event
  //-----------------------------------------------------------------
  virtual StatusCode eventFinalize() = 0;

  //-----------------------------------------------------------------
  //! Finalizer
  //-----------------------------------------------------------------
  virtual StatusCode finalize() = 0;

  virtual void setTauEventData(TauEventData* data) = 0;

  //make pure
  virtual StatusCode readConfig() = 0;

};

#endif // TOOLBASE_TAU_H
