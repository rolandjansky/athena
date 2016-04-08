/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITAUTOOLEXECBASE_TAU_H
#define ITAUTOOLEXECBASE_TAU_H

#include <string>

#include "AsgTools/AsgTool.h"

/**
 * @brief The base class for all tau tools that act as algorithms.
 * 
 * @author Justin Griffiths
 */


class ITauToolExecBase : virtual public asg::IAsgTool
{
 public:

  virtual ~ITauToolExecBase() {}

  //-----------------------------------------------------------------
  //! Tool initializer
  //-----------------------------------------------------------------
  virtual StatusCode initialize() = 0;

  //-----------------------------------------------------------------
  //! Execute - called for each tau candidate
  //-----------------------------------------------------------------
  virtual StatusCode execute() = 0;

  //-----------------------------------------------------------------
  //! Finalizer
  //-----------------------------------------------------------------
  virtual StatusCode finalize() = 0;

};

#endif // ITAUTOOLEXECBASE_TAU_H
