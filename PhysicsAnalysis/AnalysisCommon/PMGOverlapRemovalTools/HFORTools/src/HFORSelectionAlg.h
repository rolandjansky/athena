/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HFORTOOLS_HFORSELECTIONALG_H
#define HFORTOOLS_HFORSELECTIONALG_H 1

// Framework includes
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Includes of this package
#include "HFORTools/IHFORSelectionTool.h"



class HFORSelectionAlg: public ::AthFilterAlgorithm {
  public:
    /// Constructor
    HFORSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor
    virtual ~HFORSelectionAlg();

    /// Athena initialize method: called once before the event loop starts
    virtual StatusCode  initialize();

    /// Athena execute method: called once for each event in the event loop
    virtual StatusCode  execute();

    /// Athena finalize method: called once after the event loop finishes
    virtual StatusCode  finalize();

  private:
    /// The tool handle to the HFORSelectionTool
    ToolHandle<IHFORSelectionTool> m_hforSelectionTool;

};

#endif //> !HFORTOOLS_HFORSELECTIONALG_H
