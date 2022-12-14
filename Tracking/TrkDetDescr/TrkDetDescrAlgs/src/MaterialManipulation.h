/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialManipulation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRALGS_MATERIALMANIPULATION_H
#define TRKDETDESCRALGS_MATERIALMANIPULATION_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"
//Eigen
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {

    class ILayerMaterialManipulator;
    class LayerMaterialMap;
    

    /** @class MaterialManipulation

        A simple algorithm that reads in the material maps and writes out a new file,
        allowing to manipulate the material maps.

        @author Andreas.Salzburger@cern.ch      
     */


    class MaterialManipulation : public AthAlgorithm {

      public:

        /** Standard Athena-Algorithm Constructor */
        MaterialManipulation(const std::string& name, ISvcLocator* pSvcLocator);
        
        /** Default Destructor */
        ~MaterialManipulation();

        /** standard Athena-Algorithm method */
        virtual StatusCode          initialize() override;
        
        /** standard Athena-Algorithm method */
        virtual StatusCode          execute() override;
        
        /** standard Athena-Algorithm method */
        virtual StatusCode          finalize() override;

      private:
                 
         //!< input material properties
         std::string                           m_inputLayerMaterialMapName;
         const LayerMaterialMap*               m_inputLayerMaterialMap;

         //!< output material properties
         std::string                           m_outputLayerMaterialMapName;

         ToolHandle<ILayerMaterialManipulator> m_layerMaterialManipulator;

    };
}

#endif 
