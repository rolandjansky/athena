/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPAU_ToT_tool.h, (c) ATLAS Detector software 2010
///////////////////////////////////////////////////////////////////

#ifndef PHOTONANALYSISUTILS_ITOT_TOOL_H
#define PHOTONANALYSISUTILS_ITOT_TOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Trk { class TrackParticleBase ; } 

static const InterfaceID IID_IPAU_ToT_tool("IPAU_ToT_tool", 1, 0);
        
class IPAU_ToT_tool : virtual public IAlgTool {

 public:
  /** Virtual destructor */
  virtual ~IPAU_ToT_tool(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { return IID_IPAU_ToT_tool; };

  virtual double getToTvariable(const Trk::TrackParticleBase* ) const = 0;

};

#endif // PHOTONANALYSISUTILS_ITOT_TOOL_H
