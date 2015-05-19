/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// muonTrkTrackThinTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRKTRACKTHINTOOL_H
#define MUONTRKTRACKTHINTOOL_H

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Forward declarations
class StoreGateSvc;
class IThinningSvc;

class muonTrkTrackThinTool : public AthAlgorithm {
  
 public: 
  /** Constructor with parameters */
  muonTrkTrackThinTool( const std::string& name, ISvcLocator* pSvcLocator );
  
  /** Destructor */
  ~muonTrkTrackThinTool();
  
  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
    
 protected:
  int m_All;
  int m_pass;
  int m_trackAll;
  int m_trackpass;
  int m_EventCounter;
  
  //  private:
  /// Containers
  std::string m_trackCollKey;
  IThinningSvc* m_thinningSvc;
    
}; 

#endif
