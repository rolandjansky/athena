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
    class Layer;

    /** @class CompoundLayerMaterialCreator

        LayerMaterialProperties creator for CompoundLayerMaterial

      @author Andreas.Salzburger@cern.ch
     */

    class CompoundLayerMaterialCreator : public AthAlgTool, virtual public ILayerMaterialCreator {

      public:
        /** Constructor */
        CompoundLayerMaterialCreator(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        ~CompoundLayerMaterialCreator();

        /** AlgTool initialize method */
        StatusCode initialize();

        /** AlgTool finalize method */
        StatusCode finalize();

        /** process the material properties */
        const LayerMaterialProperties* createLayerMaterial(const Layer& layer, const LayerMaterialRecord& lmr) const;

    private:
        bool        m_fullCompoundCalculation;

    };
    
}

#endif




