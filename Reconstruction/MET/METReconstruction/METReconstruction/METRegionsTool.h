///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METRegionsTool.cxx 
// Header file for class METRegionsTool 
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METREGIONSTOOL_H
#define METRECONSTRUCTION_METREGIONSTOOL_H 1

// STL includes
#include <string>

// METReconstruction includes
#include "METReconstruction/METRefinerTool.h"

namespace met{

  class METRegionsTool
    : virtual public METRefinerTool
  {
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METRegionsTool,IMETToolBase)

    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    METRegionsTool(const std::string& name);
    ~METRegionsTool();

    // AsgTool Hooks
    StatusCode  initialize();
    StatusCode  finalize();
  
    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
    protected: 
      StatusCode  executeTool(xAOD::MissingET* metTerm, xAOD::MissingETComponentMap* metMap);

    private:
      // Default constructor
      METRegionsTool();

      std::string m_base_met_containerKey;
      std::string m_base_met_mapKey;
      std::string m_base_met_inputKey;

      double m_eta_centralMax;
      double m_eta_endcapMax;
      double m_eta_forwardMax;
  };

}

#endif  //> !METRECONSTRUCTION_METREGIONSTOOL_H
