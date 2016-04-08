// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXBEGINRUN_BEGINRUNALG_H
#define ATHEXBEGINRUN_BEGINRUNALG_H 1

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"

// STL includes
#include <string>

class IAlgTool;
class StoreGateSvc;
class IOVDbTestAmdbCorrection;

/**
 * @class BeginRunAlg
 * @brief Algorithm that demonstrates different ways to do begin-of-run initializations
 *
 * It uses a DataHandle, an IOV callback and the beginRun() method.
 * For an introduction to this topic please see
 * https://twiki.cern.ch/twiki/bin/view/Atlas/AthenaBeginRunInitializations
 *
 * @author Frank Winklmeier
 */
class BeginRunAlg : public AthAlgorithm {
public:
  BeginRunAlg( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
  virtual StatusCode beginRun();
  virtual StatusCode endRun();

  /// IOV callback
  virtual StatusCode iovCallback( IOVSVC_CALLBACK_ARGS_P(idx,keys) );
  
private:
  ToolHandle<IAlgTool>          m_myAlgTool;

  /// DataHandle for conditions object access
  const DataHandle<IOVDbTestAmdbCorrection>  m_dataHandle;

  /// StoreGate key of conditions object
  StringProperty                m_key;
};

#endif  // ATHEXBEGINRUN_BEGINRUNALG_H
