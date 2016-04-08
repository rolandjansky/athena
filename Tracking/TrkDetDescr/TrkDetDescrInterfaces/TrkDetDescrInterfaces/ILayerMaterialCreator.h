/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ILayerMaterialCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ILAYERMATERIALCREATOR_H
#define TRKDETDESCRINTERFACES_ILAYERMATERIALCREATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// STL
#include <vector>
#include <cstring>

namespace Trk {

  class LayerMaterialProperties;
  class LayerMaterialRecord;
  
  /** Interface ID for ILayerMaterialCreators*/  
  static const InterfaceID IID_ILayerMaterialCreator("ILayerMaterialCreator", 1, 0);
  
  /** @class ILayerMaterialCreator
    
    Interface class for LayerMaterialCreator AlgTools to create LayerMaterial properties
    with different strategies.
    
    @author Andreas.Salzburger@cern.ch
    */
  class ILayerMaterialCreator : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ILayerMaterialCreator(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ILayerMaterialCreator; }
      
      /** process the material properties */
      virtual const LayerMaterialProperties* createLayerMaterial(const LayerMaterialRecord& lmr) const = 0;
      
      /** create layer material properties from layer material properties */
      virtual const LayerMaterialProperties* convertLayerMaterial(const LayerMaterialProperties& lmr) const = 0;
      
      /** the name of the created material map */
      const std::string& layerMaterialName() const { m_layerMaterialFullName=(m_layerMaterialDirectory+m_layerMaterialName); return m_layerMaterialFullName; }
      
    protected:
      std::string             m_layerMaterialName;
      std::string             m_layerMaterialDirectory;
      mutable std::string     m_layerMaterialFullName;  
  
  };


} // end of namespace


#endif // TRKDETDESCRINTERFACES_ILAYERMATERIALCREATOR_H


