/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// IInputLayerMaterialProvider.h, (c) ATLAS Detector Software
//////////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASDETDESCRINTERFACES_ILAYERMATERIALPROVIDER_H
#define ISF_FATRASDETDESCRINTERFACES_ILAYERMATERIALPROVIDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Trk {
  class LayerMaterialProperties;    
}

namespace iFatras {

  class IInputLayerMaterialProvider : virtual public IAlgTool {
   
    public:
      /** Virtual destructor */	
      virtual ~IInputLayerMaterialProvider() {}
      
      /// Creates the InterfaceID and interfaceID() method
      DeclareInterfaceID(IInputLayerMaterialProvider, 1, 0);
       
      /** Action to work on Layers */
      virtual Trk::LayerMaterialProperties * handleMaterial() = 0;
 
  };

}

#endif // ISF_FATRASDETDESCRINTERFACES_ILAYERMATERIALPROVIDER_H
