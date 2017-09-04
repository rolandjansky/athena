/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// BTrackVertexMapLogger.h
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Initiate dumps of track-to-vertex maps to log file which are provided
// by BPhysTrackVertexMapTool instances.
//
// The BPhysTrackVertexMapTool instances need to be configured separately
// and handed to this tool.
//
//============================================================================
//
#ifndef DERIVATIONFRAMEWORK_BTrackVertexMapLogger_H
#define DERIVATIONFRAMEWORK_BTrackVertexMapLogger_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "BPhysTools/IBPhysTrackVertexMapTool.h"

namespace DerivationFramework {
  
  class BTrackVertexMapLogger : virtual public AthAlgTool,
    virtual public IAugmentationTool {
  public: 
      BTrackVertexMapLogger(const std::string& t, const std::string& n,
			    const IInterface* p);
      
      virtual StatusCode initialize();
      virtual StatusCode finalize();
      
      virtual StatusCode addBranches() const;
      
  private:
      // job options
      ToolHandleArray<xAOD::IBPhysTrackVertexMapTool> m_ttvmTools;
      bool                                            m_enable;

    }; // class
} // namespace

#endif // DERIVATIONFRAMEWORK_BTrackVertexMapLogger_H
