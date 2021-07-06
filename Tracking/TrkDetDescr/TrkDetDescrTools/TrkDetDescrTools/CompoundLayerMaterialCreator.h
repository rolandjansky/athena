/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CompoundLayerMaterialCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_COMPOUNDLAYERMATERIALCREATOR_H
#define TRKDETDESCRTOOLS_COMPOUNDLAYERMATERIALCREATOR_H

// Trk
#include "TrkDetDescrInterfaces/ILayerMaterialCreator.h"
#include "TrkGeometry/MaterialProperties.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#ifndef TRKDETDESCTOOLS_MINMAX
#define TRKDETDESCTOOLS_MINMAX
#define minValue(current, test) current = test < current ? test : current
#define maxValue(current, test) current = test > current ? test : current 
#define minMaxValue(currentMin, currentMax, test) minValue(currentMin,test); maxValue(currentMax,test);
#endif


namespace Trk {

    class LayerMaterialProperties;
    class LayerMaterialRecord;
    class BinUtility;

    /** @class CompoundLayerMaterialCreator

        LayerMaterialProperties creator for CompoundLayerMaterial (i.e. a composit of materials)
    
        The convertLayerMaterial() method converts into CompoundLayerMaterial (if possible).

      @author Andreas.Salzburger@cern.ch
     */

    class CompoundLayerMaterialCreator : public AthAlgTool, virtual public ILayerMaterialCreator {

      public:
        /** Constructor */
        CompoundLayerMaterialCreator(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        ~CompoundLayerMaterialCreator();

        /** process the material properties */
        LayerMaterialProperties* createLayerMaterial(const LayerMaterialRecord& lmr) const;
        
        /** create layer material properties from layer material properties - simply clones */
        LayerMaterialProperties* convertLayerMaterial(const LayerMaterialProperties& lmr) const;

    private:
        /** private method that can be called by both create/convertLayerMaterial */
        LayerMaterialProperties* createCompoundLayerMaterial(const MaterialPropertiesMatrix& lmm, const BinUtility& lmbu) const;
        
        bool        m_fullCompoundCalculation;

    };
    
}

#endif




