/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialAnalyser.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTESTTOOLS_LAYERMATERIALRECORDDIAGNOSER_H
#define TRKDETDESCRTESTTOOLS_LAYERMATERIALRECORDDIAGNOSER_H

// Trk
#include "TrkDetDescrInterfaces/ILayerMaterialAnalyser.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#ifndef LAYERMAXBINS
#define LAYERMAXBINS 50000
#endif

class TTree;

namespace Trk {

    class LayerMaterialProperties;
    class LayerMaterialRecord;
    class Layer;

    /** @class LayerMaterialAnalyser

        LayerMaterialProperties creator for CompoundLayerMaterial

      @author Andreas.Salzburger@cern.ch
     */

    class LayerMaterialAnalyser : public AthAlgTool, virtual public ILayerMaterialAnalyser {

      public:
        /** Constructor */
        LayerMaterialAnalyser(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        ~LayerMaterialAnalyser();

        /** AlgTool initialize method */
        StatusCode initialize();

        /** AlgTool finalize method */
        StatusCode finalize();

        /** process the layer  - after material creation and loading */
        StatusCode analyseLayerMaterial(const Layer& lay) const;

        /** process the layer material proerties  - after material creation and before loading */
        StatusCode analyseLayerMaterial(const Layer& lay, const LayerMaterialProperties& lmp) const;
            
        /** process the layer material record  - before material creation */
        StatusCode analyseLayerMaterial(const Layer& lay, const MaterialPropertiesMatrix& lmr) const;
        
        /** process the layer material record  - before material creation */
        StatusCode analyseLayerMaterial(const Layer& lay, const LayerMaterialRecord& lmr) const;
  

    private:
        StatusCode analyse(const Layer& lay, 
                           const MaterialPropertiesMatrix& lmr,
                           const std::vector< std::vector< unsigned int > >* bCounter = 0) const;
        
        std::string m_layerMaterialName;

        TTree*                m_validationTree;            //!< The validation tree
        std::string           m_validationTreeName;        //!< validation tree name - to be accessed by this from root
        std::string           m_validationTreeDescription; //!< validation tree description - second argument in TTree
        std::string           m_validationTreeFolder;      //!< stream/folder to for the TTree to be written out
        
        mutable int                 m_layerIndex;                //!< the layer index given by the TrackingGeometry
        mutable int                 m_layerType;                 //!< the type of the layer 1 - cylinder, 2 - disk
        std::vector<float>* m_layerTranslation;       //!< center of the transform
        std::vector<float>* m_layerRotation;          //!< orientation of the layer         
        mutable float               m_layerDimension0;           //!< dimension 0 : cylinder r, disk r_min
        mutable float               m_layerDimension1;           //!< dimension 1 : cylinder z, disk r_max
        mutable int                 m_layerBins;                 //!< total number of bins - loc0 * loc 1
        mutable int                 m_layerBins0;                //!< total number of bins - loc 0
        mutable int                 m_layerBins1;                //!< total number of bins - loc 0
        std::vector<int>*   m_bin0;        //!< bin 0
        std::vector<int>*   m_bin1;        //!< bin 1
        std::vector<float>* m_thickness;   //!< gathered thickness from material mapping/material properties
        std::vector<float>* m_X0;          //!< gathered X0 from material mapping/material properties
        std::vector<float>* m_L0;          //!< gathered L0 from material mapping/material properties
        std::vector<float>* m_A;           //!< gathered A from material mapping/material properties
        std::vector<float>* m_Z;           //!< gathered Z from material mapping/material properties
        std::vector<float>* m_Rho;         //!< gathered rho from material mapping/material properties
        std::vector<int>*   m_elements;    //!< gathered number of elements from material mapping/material properties
        std::vector<int>*   m_binCounter;  //!< how often was this bin hit / used

    };

}

#endif




