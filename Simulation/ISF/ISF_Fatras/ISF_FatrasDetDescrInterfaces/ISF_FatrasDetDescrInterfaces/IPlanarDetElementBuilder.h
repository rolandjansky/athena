/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPlanarDetElementBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
 
#ifndef ISF_DETDESCRINTERFACES_IPLANARDETELEMENTBUILDER_H
#define ISF_DETDESCRINTERFACES_IPLANARDETELEMENTBUILDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// STL
#include <vector>
#include <string>

namespace iFatras {

  class PlanarDetElement;
  
  /** @class IPlanarDetElementBuilder
      
      Interface class IPlanarDetElementBuilder
      It inherits from IAlgTool. 
      
      @author Noemi.Calace@cern.ch
  */
  class IPlanarDetElementBuilder : virtual public IAlgTool {
    
  public:
    /**Virtual destructor*/
    virtual ~IPlanarDetElementBuilder(){}
    
    /// Creates the InterfaceID and interfaceID() method
    DeclareInterfaceID(IPlanarDetElementBuilder, 1, 0);
    
    virtual const std::vector< const PlanarDetElement* >* PlanarDetElements() const = 0; 

    virtual const PlanarDetElement* DetElement(unsigned int, unsigned int) const = 0;
    
    virtual const PlanarDetElement* DetElement() const = 0;

    virtual double GetPhiElements() const = 0;
    
    virtual double GetZElements() const = 0;

  };
  
  
} // end of namespace


#endif // ISF_DETDESCRINTERFACES_IPLANARDETELEMENTBUILDER_H
