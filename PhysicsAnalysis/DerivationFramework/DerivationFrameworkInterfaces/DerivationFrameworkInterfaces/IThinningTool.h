/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ISkimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_INTERFACES_ITHINNINGTOOL_H
#define DERIVATIONFRAMEWORK_INTERFACES_ITHINNINGTOOL_H 

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace DerivationFramework {

  static const InterfaceID IID_IThinningTool("IThinningTool", 1, 0);
   
  /**
   @class IThinningTool
       
   @author James.Catmore-at-cern.ch
   */
     
  class IThinningTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IThinningTool(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IThinningTool; }
       
       /** Pass the thinning service  */
       virtual StatusCode doThinning() const = 0;  	
  };

} // end of namespace

#endif 
