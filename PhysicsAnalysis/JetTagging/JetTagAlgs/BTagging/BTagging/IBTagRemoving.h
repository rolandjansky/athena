/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_IBTAGREMOVING_H
#define BTAGGING_IBTAGREMOVING_H

#include "GaudiKernel/IAlgTool.h"

class Jet;

namespace Analysis
{

  static const InterfaceID IID_IBTagRemoving("IBTagRemoving", 1, 0);

  class IBTagRemoving : virtual public IAlgTool 
  {
   public:

       /** Virtual destructor */
       virtual ~IBTagRemoving(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IBTagRemoving; };

       virtual StatusCode initialize() = 0;
       virtual StatusCode finalize() = 0;
       virtual StatusCode BTagRemoving_exec(Jet& myJet) = 0;
  };

} // End namespace
#endif
