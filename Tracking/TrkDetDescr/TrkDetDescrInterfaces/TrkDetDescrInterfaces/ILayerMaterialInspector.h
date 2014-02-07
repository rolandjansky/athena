/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ILayerMaterialInspector.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ILAYERMATERIALINSPECTOR_H
#define TRKDETDESCRINTERFACES_ILAYERMATERIALINSPECTOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// STL
#include <vector>
#include <cstring>

namespace Trk {

  class Surface;
  class Layer;
  class LayerMaterialProperties;
  
  /** Interface ID for ILayerMaterialInspectors*/  
  static const InterfaceID IID_ILayerMaterialInspector("ILayerMaterialInspector", 1, 0);
  
  /** @class ILayerMaterialInspector
    Interface class ILayerMaterialInspectors
    It inherits from IAlgTool. The actual implementation of the AlgTool depends on the SubDetector,
    more detailed information should be found there.
    
    @author Andreas.Salzburger@cern.ch
    */
  class ILayerMaterialInspector : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ILayerMaterialInspector(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ILayerMaterialInspector; }

      /** process the given layer */
      virtual void processLayer(const Layer& lay,
                                const std::string& volume="UnknownVolume") const = 0;
      
      /** process the material properties */
      virtual void processLayerMaterial(const LayerMaterialProperties& lmp, 
                                        const Surface& surf,
                                        int layerIndex = 0,
                                        const Surface* referenceSurface = 0,                                        
                                        const std::string& volume="UnknownVolume") const = 0;
     
      /** validation action */
      virtual void validationAction() const = 0;
  
  };


} // end of namespace


#endif // TRKDETDESCRINTERFACES_ILAYERMATERIALINSPECTOR_H


