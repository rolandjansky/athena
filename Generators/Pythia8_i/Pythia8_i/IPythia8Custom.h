/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IPYTHIA8CUSTOM_H
#define IPYTHIA8CUSTOM_H

#include "GaudiKernel/IAlgTool.h"


static const InterfaceID IID_IPythia8Custom("IPythia8Custom", 1, 0);

namespace Pythia8{
  class Pythia;
}


class IPythia8Custom: virtual public IAlgTool {
  public:
    /** Algtool infrastructure */
    static const InterfaceID& interfaceID(){ return IID_IPythia8Custom; };
  
    /** Virtual destructor */
    virtual ~IPythia8Custom(){};
  
    /** Update the pythia event */
    virtual StatusCode ModifyPythiaEvent(Pythia8::Pythia& ) const = 0;
    /** Return how much the cross section is modified.
     *  Should only be called once all events have been processed */
    virtual double CrossSectionScaleFactor() const {return 1.;};

    virtual StatusCode InitializePythiaInfo(Pythia8::Pythia& ) const {return StatusCode::SUCCESS;};
  
};

#endif
