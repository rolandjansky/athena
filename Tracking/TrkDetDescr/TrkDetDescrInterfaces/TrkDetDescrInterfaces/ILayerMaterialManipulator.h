/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ILayerMaterialManipulator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ILAYERMATERIALMANIPULATOR_H
#define TRKDETDESCRINTERFACES_ILAYERMATERIALMANIPULATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// STL
#include <vector>
#include <cstring>

namespace Trk {

  class LayerMaterialProperties;  
  class LayerIndex;
    
  /** Interface ID for ILayerMaterialManipulators*/  
  static const InterfaceID IID_ILayerMaterialManipulator("ILayerMaterialManipulator", 1, 0);
  
  /** @class ILayerMaterialManipulator
    
    Interface class for LayerMaterial manipulation, it creates new LayerMaterial
  
    @author Andreas.Salzburger@cern.ch
    */
  class ILayerMaterialManipulator : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ILayerMaterialManipulator(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ILayerMaterialManipulator; }

      /** process the layer material - after material creation and before loading */
      virtual const LayerMaterialProperties* processLayerMaterial(const LayerIndex& layIndex, const LayerMaterialProperties& lmp) const = 0;
              
  };

} // end of namespace

#endif // TRKDETDESCRINTERFACES_ILAYERMATERIALMANIPULATOR_H
