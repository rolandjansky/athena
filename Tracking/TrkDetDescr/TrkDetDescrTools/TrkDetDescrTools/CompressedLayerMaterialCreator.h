/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompressedLayerMaterialCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_COMPRESSEDLAYERMATERIALCREATOR_H
#define TRKDETDESCRTOOLS_COMPRESSEDLAYERMATERIALCREATOR_H

// Trk
#include "TrkDetDescrInterfaces/ILayerMaterialCreator.h"
#include "TrkGeometry/MaterialProperties.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


namespace Trk {

    class LayerMaterialProperties;
    class LayerMaterialRecord;
    class BinUtility;

    struct IndexedMaterial {
        unsigned short int firstBin;
        unsigned short int secondBin;
        const Trk::MaterialProperties* materialProperties;
    };
    

    /** @class CompressedLayerMaterialCreator

        LayerMaterialProperties creator for CompressedLayerMaterial
    
        The convertLayerMaterial() method converts into CompressedLayerMaterial (if possible).
    

      @author Andreas.Salzburger@cern.ch
     */

    class CompressedLayerMaterialCreator : public AthAlgTool, virtual public ILayerMaterialCreator {

      public:
        /** Constructor */
        CompressedLayerMaterialCreator(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        ~CompressedLayerMaterialCreator();

        /** process the material properties */
        LayerMaterialProperties* createLayerMaterial(const LayerMaterialRecord& lmr) const;
        
        /** create layer material properties from layer material properties - simply clones */
        LayerMaterialProperties* convertLayerMaterial(const LayerMaterialProperties& lmr) const;

    private:
        /** private method that can be called by both create/convertLayerMaterial */
        LayerMaterialProperties* createCompressedLayerMaterial(const MaterialPropertiesMatrix& lmm, const BinUtility& lmbu) const;
        
        double                  m_compressedMaterialThickness;
        unsigned int            m_compressedMaterialX0Bins;
        unsigned int            m_compressedMaterialZARhoBins;

    };
       
}

#endif




