/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialConverter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_LAYERMATERIALCONVERTER_H
#define TRKDETDESCRTOOLS_LAYERMATERIALCONVERTER_H

// Trk
#include "TrkDetDescrInterfaces/ILayerMaterialManipulator.h"
#include "TrkDetDescrInterfaces/ILayerMaterialCreator.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

namespace Trk {

    /** @class LayerMaterialConverter

      AlgTool that converts the layer material from one to another type
    
      @author Andreas.Salzburger@cern.ch
     */

    class LayerMaterialConverter :  public AthAlgTool, virtual public ILayerMaterialManipulator {
     
      public:
        /** Constructor */
        LayerMaterialConverter(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~LayerMaterialConverter();
        
        /** AlgTool initialize method */
        StatusCode initialize();
        

        /** process the layer material - after material creation and before loading */
        const LayerMaterialProperties* processLayerMaterial(const LayerIndex& layIndex, const LayerMaterialProperties& lmp) const;

      private:
          
        ToolHandle<ILayerMaterialCreator>   m_layerMaterialCreator;
        
    };

} // end of namespace

#endif // TRKDETDESCRTOOLS_LAYERMATERIALPROVIDER_H

