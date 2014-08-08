/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_IBTAGLABELING_H
#define BTAGGING_IBTAGLABELING_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODJet/Jet.h"

class Jet;

namespace Analysis
{

  static const InterfaceID IID_IBTagLabeling("IBTagLabeling", 1, 0);

  class IBTagLabeling : virtual public IAlgTool 
  {
   public:

       /** Virtual destructor */
       virtual ~IBTagLabeling(){};

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IBTagLabeling; };

       virtual StatusCode initialize() = 0;
       virtual StatusCode finalize() = 0;
       virtual StatusCode BTagLabeling_exec(xAOD::Jet& myJet) = 0;
  };

} // End namespace
#endif
