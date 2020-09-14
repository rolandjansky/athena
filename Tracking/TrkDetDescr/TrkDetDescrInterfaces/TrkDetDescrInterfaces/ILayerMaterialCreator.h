/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "CxxUtils/CachedValue.h"

namespace Trk {

  class LayerMaterialProperties;
  class LayerMaterialRecord;
  
  /** @class ILayerMaterialCreator
    
    Interface class for LayerMaterialCreator AlgTools to create LayerMaterial properties
    with different strategies.
    
    @author Andreas.Salzburger@cern.ch
    */
  class ILayerMaterialCreator : virtual public IAlgTool {
    
    public:
      DeclareInterfaceID (ILayerMaterialCreator, 1, 0);

      /**Virtual destructor*/
      virtual ~ILayerMaterialCreator(){}
      
      /** process the material properties */
      virtual LayerMaterialProperties* createLayerMaterial(const LayerMaterialRecord& lmr) const = 0;
      
      /** create layer material properties from layer material properties */
      virtual LayerMaterialProperties* convertLayerMaterial(const LayerMaterialProperties& lmr) const = 0;
      
      /** the name of the created material map */
      const std::string& layerMaterialName() const {
        if (!m_layerMaterialFullName.isValid()) {
          m_layerMaterialFullName.set (m_layerMaterialDirectory+m_layerMaterialName);
        }
        return *m_layerMaterialFullName.ptr();
      }
      
    protected:
      std::string             m_layerMaterialName;
      std::string             m_layerMaterialDirectory;

    private:
      CxxUtils::CachedValue<std::string> m_layerMaterialFullName;
  };


} // end of namespace


#endif // TRKDETDESCRINTERFACES_ILAYERMATERIALCREATOR_H


