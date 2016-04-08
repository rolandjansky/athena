/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ISkimmingTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_INTERFACES_IAUGMENTATIONTOOL_H
#define DERIVATIONFRAMEWORK_INTERFACES_IAUGMENTATIONTOOL_H 

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace DerivationFramework {

  static const InterfaceID IID_IAugmentationTool("IAugmentationTool", 1, 0);
   
  /**
   @class IAugmentationTool
       
   @author James.Catmore-at-cern.ch
   */
     
  class IAugmentationTool : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IAugmentationTool(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IAugmentationTool; }
       
       /** Pass the thinning service  */
       virtual StatusCode addBranches() const = 0;  	
  };

} // end of namespace

#endif 
