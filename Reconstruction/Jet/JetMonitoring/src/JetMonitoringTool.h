///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETMONITORING_JETMONITORINGTOOL_H
#define JETMONITORING_JETMONITORINGTOOL_H
///////////////////////////////////////////
///
/// \class JetMonitoringTool
/// An athena tool for jet monitoring.
/// 
////////////////////////////////////////////

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ToolHandle.h"

#include "JetMonitoring/JetContainerHistoFiller.h"

class JetMonitoringTool
  : public ManagedMonitorToolBase
{ 
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public: 

  // Copy constructor:

  /// Constructor with parameters:
  JetMonitoringTool( const std::string& type,
                     const std::string& name, 
                     const IInterface* parent );
  
  /// Destructor:
  virtual ~JetMonitoringTool(); 

  // Athena algtool's Hooks
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private: 

  /// Default constructor:
  JetMonitoringTool();

  ToolHandleArray<JetContainerHistoFiller> m_htools;

  /// Directory under which all histos filled by this tool will live
  std::string m_histoDir; 

  /// Default monitoring interval for all children tools 
  int m_interval;
}; 

#endif
