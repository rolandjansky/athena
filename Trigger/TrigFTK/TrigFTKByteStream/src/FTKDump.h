// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef FTK_DUMP_EXAMPLE_H
#define FTK_DUMP_EXAMPLE_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"


class MsgStream;

/**
*  @short Example for the usage of FTKDump in C++
*
* @author Bartosz Mindur <bartosz.mindur@cern.ch> AGH-UST Krakow
*
*/
class FTKDump : public AthAlgorithm {

public:
  /// Regular algorithm constructor
  FTKDump( const std::string &name, ISvcLocator *pSvcLocator );
  ~FTKDump();

  /// Function called at the beginning of the job
  virtual StatusCode initialize();
  /// Function called at each event
  virtual StatusCode execute();

private:
  //
}; // class FTKDump

#endif // FTK_DUMP_EXAMPLE_H
