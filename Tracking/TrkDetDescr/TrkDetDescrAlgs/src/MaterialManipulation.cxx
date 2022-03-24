/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialManipulation.cxx, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////

// Gaudi Units
#include "GaudiKernel/SystemOfUnits.h"
//TrkDetDescr Algs, Interfaces, Utils
#include "MaterialManipulation.h"
#include "TrkDetDescrInterfaces/ILayerMaterialManipulator.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkGeometry/ElementTable.h"


Trk::MaterialManipulation::MaterialManipulation(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name,pSvcLocator),
  m_inputLayerMaterialMapName("/GLOBAL/TrackingGeo/Input"),
  m_inputLayerMaterialMap(nullptr), 
  m_outputLayerMaterialMapName("/GLOBAL/TrackingGeo/Output"),
  m_layerMaterialManipulator("")
{
    declareProperty("LayerMaterialManipulator",  m_layerMaterialManipulator);
    // input - output definition
    declareProperty("LayerMaterialMapNameInput",   m_inputLayerMaterialMapName);
    declareProperty("LayerMaterialMapNameOutput",  m_outputLayerMaterialMapName);
    
}

Trk::MaterialManipulation::~MaterialManipulation()
{}

StatusCode Trk::MaterialManipulation::initialize()
{
    ATH_MSG_INFO("initialize()");
    
    if (!m_layerMaterialManipulator.empty() && m_layerMaterialManipulator.retrieve().isFailure()){
        ATH_MSG_FATAL("Could not retrieve LayerMaterial manipulator - although configured. Aborting.");
        return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
}


StatusCode Trk::MaterialManipulation::execute()
{
    ATH_MSG_VERBOSE("MaterialManipulation execute() start");
    
    Trk::LayerMaterialMap* outputLayerMaterialMap = nullptr;
    
    // make sure this is only done once
    if (!m_inputLayerMaterialMap){
        
        // -------------------------------------------------------------------------------
        if (detStore()->retrieve(m_inputLayerMaterialMap, m_inputLayerMaterialMapName).isFailure()){
            ATH_MSG_FATAL( "Could not retrieve LayerMaterialMap wiht name '" << m_inputLayerMaterialMapName << "'. Aborting.");
            return StatusCode::FAILURE;
        } else
            ATH_MSG_DEBUG( "Retrieved LayerMaterialMap wiht name '" << m_inputLayerMaterialMapName << "' for " <<  m_inputLayerMaterialMap->size() << " layers.");
    
       // create the output material map
       outputLayerMaterialMap = new Trk::LayerMaterialMap();    
       // now create the new one and manipulate
       for ( const auto & lmIter : (*m_inputLayerMaterialMap) ){
           // copy the layer material 
           ATH_MSG_VERBOSE("  -> Found map for layer with index " << lmIter.first);
           // use the virtual constructor
           const Trk::LayerMaterialProperties* lmp = nullptr;
           // manipulate it if needed
           if (!m_layerMaterialManipulator.empty()) 
               lmp = m_layerMaterialManipulator->processLayerMaterial(lmIter.first,*(lmIter.second));
           else
               lmp = lmIter.second->clone();
           // and insert into the map
           (*outputLayerMaterialMap)[lmIter.first] = lmp;
       }
       
       // get the element table - and copy it over if there's none
 	   const ElementTable* eTable = m_inputLayerMaterialMap->elementTable();
       if (eTable){
          Trk::SharedObject<const Trk::ElementTable> tElementTable(new Trk::ElementTable(*eTable));
          outputLayerMaterialMap->updateElementTable(tElementTable);
       }
       // finally write it to StoreGate
       if ( (detStore()->record(outputLayerMaterialMap, m_outputLayerMaterialMapName, false)).isFailure()){
           ATH_MSG_ERROR("Writing of LayerMaterialMap with name '" << m_outputLayerMaterialMapName << "' was not successful." );
           delete outputLayerMaterialMap;
       } else 
           ATH_MSG_INFO("Successfully wrote LayerMaterialMap with name '" <<  m_outputLayerMaterialMapName << "' to the detector store.");
        
    }
    return StatusCode::SUCCESS;
    
}


StatusCode Trk::MaterialManipulation::finalize()
{

    ATH_MSG_INFO("========================================================================================= ");
    ATH_MSG_INFO("finalize() starts ...");
    return StatusCode::SUCCESS;
}
