/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IModuleStepper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDIGINTERFACES_IMODULESTEPPER_H
#define TRKDIGINTERFACES_IMODULESTEPPER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkDigEvent/DigitizationCell.h"
//STL
#include <vector>

namespace Trk {

  class DigitizationModule;

  /** Interface ID for IModuleSteppers*/  
  static const InterfaceID IID_IModuleStepper("IModuleStepper", 1, 0);
  
  /** @class IModuleStepper
  
       Interface class IModuleSteppers
    
      @author Andreas.Salzburger@cern.ch
    */
  class IModuleStepper : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~IModuleStepper(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_IModuleStepper; }

      /** calculate the steps caused by this strack - full simulation interface */
      virtual std::vector<DigitizationStep> cellSteps(const DigitizationModule& dmodule,
                                                      const Amg::Vector3D& startPosition,
                                                      const Amg::Vector3D& endPosition) const = 0;


      /** calculate the steps caused by this track - fast simulation interface */
      virtual std::vector<DigitizationStep> cellSteps(const DigitizationModule& dmodule,
                                                      const Amg::Vector2D& intersection,
                                                      const Amg::Vector3D& direction) const = 0;

  };

} // end of namespace

#endif // TRKDIGINTERFACES_IMODULESTEPPER_H
