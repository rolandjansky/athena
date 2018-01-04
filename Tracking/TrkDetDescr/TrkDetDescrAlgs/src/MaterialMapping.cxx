/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialMapping.cxx, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////

// Gaudi Units
#include "GaudiKernel/SystemOfUnits.h"
//TrkDetDescr Algs, Interfaces, Utils
#include "TrkDetDescrAlgs/MaterialMapping.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkDetDescrInterfaces/IMaterialMapper.h"
#include "TrkDetDescrInterfaces/ILayerMaterialCreator.h"
#include "TrkDetDescrInterfaces/ILayerMaterialAnalyser.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkDetDescrUtils/BinUtility.h"
// TrkExtrapolation
#include "TrkExInterfaces/IExtrapolationEngine.h"
// TrkGeometry
#include "TrkGeometry/LayerMaterialRecord.h"
//#include "TrkGeometry/ElementTable.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MaterialStep.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/CompressedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
// TrkEvent
#include "TrkNeutralParameters/NeutralParameters.h"
// Amg
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

#ifdef TRKDETDESCR_MEMUSAGE
#include <unistd.h>
#endif

Trk::MaterialMapping::MaterialMapping(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name,pSvcLocator),
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc",name),
  m_trackingGeometry(0),
  m_checkForEmptyHits(true),
  m_mappingVolumeName("Atlas"),
  m_mappingVolume(0),
  m_inputMaterialStepCollection("MaterialStepRecords"),
  m_etaCutOff(6.0),
  m_etaSide(0),
  m_useLayerThickness(false),
  m_associationType(1),
  m_layerMaterialRecordAnalyser(""),
  m_mapMaterial(true),
  m_materialMapper(""),
  m_mapComposition(true),
  m_minCompositionFraction(0.005),
  m_elementTable(0),
  m_inputEventElementTable("ElementTable"),
  m_accumulatedMaterialXX0(0.),
  m_accumulatedRhoS(0.),
  m_mapped(0),
  m_unmapped(0),
  m_skippedOutside(0),
  m_layerMaterialScreenOutput(0)
#ifdef TRKDETDESCR_MEMUSAGE      
  ,m_memoryLogger()
#endif 
{
    // the name of the TrackingGeometry to be retrieved
    declareProperty("TrackingGeometrySvc"         , m_trackingGeometrySvc);
    declareProperty("MappingVolumeName"           , m_mappingVolumeName);
    // the extrapolation engine
    declareProperty("CheckForEmptyHits"           , m_checkForEmptyHits);
    declareProperty("ExtrapolationEngine"         , m_extrapolationEngine);
    // general steering
    declareProperty("EtaCutOff"                   , m_etaCutOff);
    declareProperty("EtaSide"                     , m_etaSide);
    // for the analysis of the material 
    declareProperty("LayerMaterialRecordAnalyser" , m_layerMaterialRecordAnalyser);
    // for the creation of the material 
    declareProperty("LayerMaterialCreators"       , m_layerMaterialCreators);
    declareProperty("LayerMaterialAnalysers"      , m_layerMaterialAnalysers);
    // the toolhandle of the MaterialMapper to be used
    declareProperty("MapMaterial"                 , m_mapMaterial);
    declareProperty("MaterialMapper"              , m_materialMapper);
	// Composition related parameters
    declareProperty("MapComposition"              , m_mapComposition);	
    declareProperty("MinCompositionFraction"      , m_minCompositionFraction);	
    // Steer the layer thickness
    declareProperty("UseActualLayerThicknesss"    , m_useLayerThickness);
    // some job setup
    declareProperty("MaterialAssociationType"     , m_associationType);
    declareProperty("InputMaterialStepCollection" , m_inputMaterialStepCollection);
    declareProperty("InputElementTable"           , m_inputEventElementTable);
    // Output screen stuff
    declareProperty("MaterialScreenOutputLevel"   , m_layerMaterialScreenOutput);
    
}

Trk::MaterialMapping::~MaterialMapping()
{}


StatusCode Trk::MaterialMapping::initialize()
{

    ATH_MSG_INFO("initialize()");

    if ( (m_trackingGeometrySvc.retrieve()).isFailure() )
        ATH_MSG_WARNING("Could not retrieve TrackingGeometrySvc");

    if (m_extrapolationEngine.retrieve().isFailure())
        ATH_MSG_WARNING("Could not retrieve ExtrapolationEngine - needed for emopty hit scaling");

    if (!m_materialMapper.empty() &&  (m_materialMapper.retrieve()).isFailure() )
        ATH_MSG_WARNING("Could not retrieve MaterialMapper");
    
    if ( !m_layerMaterialRecordAnalyser.empty() && m_layerMaterialRecordAnalyser.retrieve().isFailure() )  
        ATH_MSG_WARNING("Could not retrieve LayerMaterialAnalyser");
        
    if ( m_layerMaterialCreators.size() && m_layerMaterialCreators.retrieve().isFailure() )        
        ATH_MSG_WARNING("Could not retrieve any LayerMaterialCreators");

    if ( m_layerMaterialAnalysers.size() && m_layerMaterialAnalysers.retrieve().isFailure() )        
        ATH_MSG_WARNING("Could not retrieve any LayerMaterialAnalysers");

    ATH_CHECK( m_inputMaterialStepCollection.initialize() );
    ATH_CHECK(  m_inputEventElementTable.initialize() );
        
    return StatusCode::SUCCESS;
}


StatusCode Trk::MaterialMapping::execute()
{
    ATH_MSG_VERBOSE("MaterialMapping execute() start");

    // ------------------------------- get the trackingGeometry at first place
    if (!m_trackingGeometry) {
        StatusCode retrieveCode = retrieveTrackingGeometry();
        if (retrieveCode.isFailure()){
            ATH_MSG_INFO("Could not retrieve TrackingGeometry. Exiting.");
            return retrieveCode;
        }
    } 
    if (m_trackingGeometry)
      ATH_MSG_VERBOSE("TrackingGeometry sucessfully retrieved");  
    
    SG::ReadHandle<MaterialStepCollection> materialStepCollection(m_inputMaterialStepCollection);

        // --------- prepare the element table ---------------------------------------------------

    SG::ReadHandle<Trk::ElementTable> eTableEvent(m_inputEventElementTable);	  
    
    (*m_elementTable) += (*eTableEvent);  // accummulate the table 
    m_mapComposition = eTableEvent.isValid() ? true : false;
        
        
        // event parameters - associated asteps, and layers hit per event
        int associatedSteps      = 0;
        m_accumulatedMaterialXX0 = 0.;
        m_accumulatedRhoS        = 0.;
        m_layersRecordedPerEvent.clear();
        // clearing the recorded layers per event
        if (materialStepCollection.isValid() && materialStepCollection->size()){

           // get the number of material steps 
           size_t materialSteps = materialStepCollection->size();
           ATH_MSG_DEBUG("[+] Successfully read  "<<  materialSteps << " geantino steps");
           
           // create a direction out of the last material step
           double dirx = (*materialStepCollection)[materialSteps-1]->hitX();
           double diry = (*materialStepCollection)[materialSteps-1]->hitY();
           double dirz = (*materialStepCollection)[materialSteps-1]->hitZ();
           Amg::Vector3D direction = Amg::Vector3D(dirx,diry,dirz).unit();
           
           double eta = direction.eta();
           // skip the event if the eta cut is not met
           if ( fabs(eta) > m_etaCutOff || (m_etaSide && m_etaSide*eta < 0.)  ) {
               ATH_MSG_VERBOSE("[-] Event is outside eta acceptance of " << m_etaCutOff << ". Skipping it.");
               return StatusCode::SUCCESS;
           }
           
           // now propagate through the full detector and collect the layers
           Trk::NeutralCurvilinearParameters ncP(Amg::Vector3D(0.,0.,0.), direction, 0.);
           // create a neutral extrapolation cell
           Trk::ExtrapolationCell<Trk::NeutralParameters> ecc(ncP);
           ecc.navigationCurvilinear = false;
           ecc.addConfigurationMode(Trk::ExtrapolationMode::StopAtBoundary);
           ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectPassive);
           ecc.addConfigurationMode(Trk::ExtrapolationMode::CollectBoundary);
           
           // let's extrapolate through the detector and remember which layers (with material) should have been hit
           std::vector< std::pair<const Trk::Layer*, Amg::Vector3D> > layersAndHits;
           // call the extrapolation engine
           Trk::ExtrapolationCode eCode = m_extrapolationEngine->extrapolate(ecc);
           // end the parameters if there
           if (eCode.isSuccess()){
               // name of passive surfaces found
               size_t nLayersHit = ecc.extrapolationSteps.size();
               ATH_MSG_VERBOSE("[+] Extrapolation to layers did succeed and found " << nLayersHit << " layers.");
               // reserve the size of the vectors
               layersAndHits.reserve(nLayersHit);
               // for screen output
               size_t ilayer = 0;
               // find all the intersected material - remember the last parameters
               const Trk::NeutralParameters* parameters = 0; 
               // loop over the collected information
               for (auto& es : ecc.extrapolationSteps){
                   // continue if we have parameters
                  parameters = es.parameters;
                  if (parameters){
                      const Trk::Surface& pSurface = parameters->associatedSurface();
                      // get the surface with associated layer (that has material)
                      ATH_MSG_VERBOSE("[L] Testing layer with associatedLayer() " << pSurface.associatedLayer() << " and materialLayer() " << pSurface.materialLayer() );
                      // 
                      if ( ( pSurface.associatedLayer() && pSurface.associatedLayer()->layerMaterialProperties() )  || pSurface.materialLayer() ){
                          // material layer 
                          const Trk::Layer* mLayer = pSurface.materialLayer() ? pSurface.materialLayer() : pSurface.associatedLayer();
                          // record that one
                          std::pair<const Trk::Layer*, Amg::Vector3D> layerHitPair(mLayer, parameters->position());
                          ATH_MSG_VERBOSE("[L] Layer " << ++ilayer << " with index " << mLayer->layerIndex().value() << " hit at " << Amg::toString(parameters->position()));
                          layersAndHits.push_back(layerHitPair);
                      }
                      delete parameters;
                  }
                }
                // cleanup of the final hits
                if (ecc.endParameters != parameters) delete ecc.endParameters;
                
                // we have no layers and Hits
                if (!layersAndHits.size()){
                    ATH_MSG_VERBOSE("[!] No Layer was intersected - skipping.");
                    return StatusCode::SUCCESS;
                }
                
                // layers are ordered, hence you can move the starting point along
                size_t currentLayer = 0;
                // loop through hits and find the closest layer, the start point moves outwards as we go 
                for ( const Trk::MaterialStep* step : *materialStepCollection ) {
                   // verbose output    
                   ATH_MSG_VERBOSE("[L] starting from layer " << currentLayer << " from layer collection for this step.");
                   // step length and position
                   double t     = step->steplength();
                   Amg::Vector3D pos(step->hitX(), step->hitY(), step->hitZ());
                   // skip if : 
                   // -- 0) no mapping volume exists
                   // -- 1) outside the mapping volume 
                   // -- 2) outside the eta acceptance
                   if (!m_mappingVolume || !(m_mappingVolume->inside(pos)) || fabs(pos.eta()) > m_etaCutOff ){
                      ++m_skippedOutside;
                      continue;
                   }
                   // now find the closest layer
                   // (a) if the currentLayer is the last layer and the hit is still inside -> assign
                   if (currentLayer < nLayersHit-1) {
                       // search through the layers - this is the reference distance for projection
                       double currentDistance = (pos-layersAndHits[currentLayer].second).mag();
                       ATH_MSG_VERBOSE("- current distance is " << currentDistance << " from " << Amg::toString(pos) << " and " << Amg::toString(layersAndHits[currentLayer].second) );
                       for (size_t testLayer = (currentLayer+1); testLayer < nLayersHit; ++testLayer){
                           // calculate teh distance to the testLayer
                           double testDistance = (pos-layersAndHits[testLayer].second).mag();
                           ATH_MSG_VERBOSE("[L] Testing layer " << testLayer << " from layer collection for this step.");
                           ATH_MSG_VERBOSE("- test    distance is " << testDistance << " from " << Amg::toString(pos) << " and " << Amg::toString(layersAndHits[testLayer].second) );
                           if ( testDistance < currentDistance ){
                               // screen output
                               ATH_MSG_VERBOSE("[L] Skipping over to current layer " << testLayer << " because " << testDistance << " < " << currentDistance); 
                               // the test distance did shrink - update currentLayer
                               currentLayer      = testLayer; 
                               currentDistance   = testDistance;
                           } else {
                               // stick to the layer you have 
                               break;
                           }
                       }
                   }
                   // the currentLayer *should* be correct now 
                   const Trk::Layer* assignedLayer =  layersAndHits[currentLayer].first;
                   Amg::Vector3D assignedPosition  = layersAndHits[currentLayer].second;
                   // associate the hit 
                   // (1) count it 
                   ++associatedSteps;
                   // (2) associate it
                   associateHit(*assignedLayer, pos, assignedPosition, t, step->fullMaterial());
              } // loop over material Steps
            
              // check for the empty hits - they need to be taken into account
              ATH_MSG_VERBOSE("Found " << layersAndHits.size() << " intersected layers - while having " << m_layersRecordedPerEvent.size() << " recorded ones.");
            
              // now - cross-chek if you have additional layers 
              for ( auto& lhp : layersAndHits){
                  // check if you find the layer int he already done record-map : not found - we need to do an empty hit scaling
                  if (m_layersRecordedPerEvent.find(lhp.first) == m_layersRecordedPerEvent.end()){
                      // try to find the layer material record
                      auto clIter = m_layerRecords.find(lhp.first);
                      if (clIter != m_layerRecords.end() ){
                          (*clIter).second.associateEmptyHit(lhp.second);
                          ATH_MSG_VERBOSE("- to layer with index "<< lhp.first->layerIndex().value() << " with empty hit detected.");
                      } else
                          ATH_MSG_WARNING("- no Layer found in the associated map! Should not happen.");
                  }
               }
               
               // check whether the event was good for at least one hit
               if (associatedSteps) {
                   ATH_MSG_VERBOSE("There are associated steps, need to call finalizeEvent() & record to the MaterialMapper.");
                   // finalize the event   --------------------- Layers ---------------------------------------------
                   for (auto& lRecord : m_layerRecords ) {
                       // associated material
                       Trk::AssociatedMaterial* assMatHit = lRecord.second.finalizeEvent((*lRecord.first));
                       // record the full layer hit 
                       if (assMatHit && !m_materialMapper.empty()) m_materialMapper->recordLayerHit(*assMatHit, true);
                       delete assMatHit;
                       // call the material mapper finalize method
                       ATH_MSG_VERBOSE("Calling finalizeEvent on the MaterialMapper ...");
                   } 
               } // the event had at least one associated hit

            } // end of eCode.success : needed for new mapping schema  
         
         } // material steps existed
         

    return StatusCode::SUCCESS;
}


bool Trk::MaterialMapping::associateHit( const Trk::Layer& associatedLayer, 
                                         const Amg::Vector3D& pos,
                                         const Amg::Vector3D& positionOnLayer,
                                         double stepl,
                                         const Trk::Material& mat)
{   
    // get the intersection with the layer
    ++m_mapped;
    // get the layer
    const Trk::Layer* layer = &associatedLayer;
    
    // get the associated volume
    const Trk::TrackingVolume* associatedVolume = m_trackingGeometry->lowestTrackingVolume(pos);
    
    // try to find the layer material record
    auto clIter = m_layerRecords.find(layer);              
    if (clIter != m_layerRecords.end() ){
        // remember that you actually hit this layer
        m_layersRecordedPerEvent[layer] = true;
        // LayerMaterialRecord found, add the hit
        (*clIter).second.associateGeantinoHit(positionOnLayer, stepl, mat);           
        ATH_MSG_VERBOSE("- associate Geantino Information at intersection [r/z] = " << positionOnLayer.perp() << "/"<< positionOnLayer.z() );
        ATH_MSG_VERBOSE("                                      mapping distance = " << (pos-positionOnLayer).mag() );
        ATH_MSG_VERBOSE("- associate Geantino Information ( s, s/x0 , x0 , l0, a, z, rho ) = "
                            << stepl << ", "<< stepl/mat.X0 << ", "<< mat.X0 << ", "<< mat.L0 << ", "<< mat.A << ", "<<  mat.Z << ", "<< mat.rho );
        m_accumulatedMaterialXX0 += stepl/mat.X0;
        m_accumulatedRhoS += stepl*mat.rho;
        ATH_MSG_VERBOSE("- accumulated material information X/x0 = " << m_accumulatedMaterialXX0);
        ATH_MSG_VERBOSE("- accumulated effective densitity rho*s = " << m_accumulatedRhoS);
        ATH_MSG_VERBOSE("- to layer with index "<< layer->layerIndex().value() << " from '"<< associatedVolume->volumeName() << "'.");
        // record the plain information w/o correction & intersection
        if (m_mapMaterial && !m_materialMapper.empty()){
                Trk::AssociatedMaterial am(pos, stepl, mat, 1., associatedVolume, layer);
                m_materialMapper->recordMaterialHit(am, positionOnLayer);
                ATH_MSG_VERBOSE(" - associated material produced as : " << am);
            }
        
    } else if (layer) {
        ATH_MSG_WARNING("- associate hit - the layer with index " << layer->layerIndex().value() << " was not found - should not happen!");
    }
    // return 
    return true;
}


void Trk::MaterialMapping::assignLayerMaterialProperties( const Trk::TrackingVolume& tvol,
                                                          Trk::LayerMaterialMap* propSet)
{

    if (!propSet) return;

    ATH_MSG_INFO("Processing TrackingVolume: "<< tvol.volumeName() );

    // ----------------------------------- loop over confined layers ------------------------------------------
    const Trk::BinnedArray< Trk::Layer >* confinedLayers = tvol.confinedLayers();
    if (confinedLayers) {
        // get the objects in a vector-like format
        const std::vector<const Trk::Layer*>& layers = confinedLayers->arrayObjects();
        ATH_MSG_INFO("--> found : "<< layers.size() << "confined Layers");
        // the iterator over the vector
        // loop over layers
        for (auto& layer : layers) {
            // assign the material and output
            if (layer && (*layer).layerIndex().value() ) {
                ATH_MSG_INFO("  > LayerIndex: "<< (*layer).layerIndex() );
                // set the material!
                if (propSet) {
                    // find the pair
                    auto curIt = propSet->find((*layer).layerIndex());
                    if (curIt != propSet->end()) {
                        ATH_MSG_INFO("LayerMaterial assigned for Layer with index: "<< (*layer).layerIndex() );
                        // set it to the layer
                        (*layer).assignMaterialProperties(*((*curIt).second), 1.);
                    }
                }
            }
        }
    }

    // ----------------------------------- loop over confined volumes -----------------------------
    const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = tvol.confinedVolumes();
    if (confinedVolumes) {
        // get the objects in a vector-like format
        const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes->arrayObjects();
        ATH_MSG_INFO("--> found : "<< volumes.size() << "confined TrackingVolumes");
        // loop over volumes
        for (auto& volume : volumes) {
            // assing the material and output
            if (volume)
                assignLayerMaterialProperties(*volume, propSet); // call itself recursively
        }
    }
}


StatusCode Trk::MaterialMapping::finalize()
{

    ATH_MSG_INFO("========================================================================================= ");
    ATH_MSG_INFO("finalize() starts ...");
    
#ifdef TRKDETDESCR_MEMUSAGE       
    m_memoryLogger.refresh(getpid());
    ATH_MSG_INFO("[ memory usage ] Start building of material maps: " );    
    ATH_MSG_INFO( m_memoryLogger );                     
#endif  
        
    // create a dedicated LayerMaterialMap by layerMaterialCreator;
    std::map< std::string, Trk::LayerMaterialMap* > layerMaterialMaps;
    for (  auto& lmcIter : m_layerMaterialCreators  ){
        ATH_MSG_INFO("-> Creating material map '"<< lmcIter->layerMaterialName() << "' from creator "<<  lmcIter.typeAndName() );
        layerMaterialMaps[lmcIter->layerMaterialName()] = new Trk::LayerMaterialMap(); 
    }
    
    ATH_MSG_INFO( m_layerRecords.size() << " LayerMaterialRecords to be finalized for this material mapping run.");
    
    // loop over the layers and output the stuff --- fill the associatedLayerMaterialProperties
    for ( auto& lIter :  m_layerRecords ) {
        // Get the key map, the layer & the volume name
        const Trk::Layer*  layer = lIter.first;
        Trk::LayerIndex layerKey = layer->layerIndex();
        // get the enclosing tracking volume
        const Trk::TrackingVolume* eVolume = layer->enclosingTrackingVolume();
        // assign the string
        std::string vName = eVolume ? (eVolume->volumeName()) : " BoundaryCollection ";
        ATH_MSG_INFO("Finalize MaterialAssociation for Layer "<< layerKey.value() << " in " << vName );
        // normalize - use m_finalizeRunDebug
        (lIter.second).finalizeRun(m_mapComposition);
        // output the material to the analyser if registered
        if (!m_layerMaterialRecordAnalyser.empty() && m_layerMaterialRecordAnalyser->analyseLayerMaterial(*layer, lIter.second).isFailure() )
            ATH_MSG_WARNING("Could not analyse the LayerMaterialRecord  for layer "<< layerKey.value() );
        // check if we have analysers per creator
        bool analyse = (m_layerMaterialCreators.size() == m_layerMaterialAnalysers.size());
        // and now use the creators to make the maps out of the LayerMaterialRecord
        size_t ilmc = 0;
        for ( auto& lmcIter : m_layerMaterialCreators ){
            // call the creator and register in the according map
#ifdef TRKDETDESCR_MEMUSAGE       
            m_memoryLogger.refresh(getpid());
            ATH_MSG_INFO("[ memory usage ] Before building the map for Layer "<< layerKey.value()  );    
            ATH_MSG_INFO( m_memoryLogger );                     
#endif  
            const Trk::LayerMaterialProperties* lMaterial = lmcIter->createLayerMaterial(lIter.second);
#ifdef TRKDETDESCR_MEMUSAGE       
            m_memoryLogger.refresh(getpid());
            ATH_MSG_INFO("[ memory usage ] After building the map for Layer "<< layerKey.value()  );    
            ATH_MSG_INFO( m_memoryLogger );                     
#endif  
            if (lMaterial)
                ATH_MSG_VERBOSE("LayerMaterial map created as "<< *lMaterial );
            // insert the created map for the given layer
            (*layerMaterialMaps[lmcIter->layerMaterialName()])[layerKey.value()] = lMaterial;
            // analyse the it if configured
            if (analyse && lMaterial && (m_layerMaterialAnalysers[ilmc]->analyseLayerMaterial(*layer, *lMaterial)).isFailure() )
                ATH_MSG_WARNING("Could not analyse created LayerMaterialProperties for layer "<< layerKey.value() );
            ++ilmc;
        }
    }

    ATH_MSG_INFO("Finalize map synchronization and write the maps to the DetectorStore.");
   
    for (auto& ilmIter : layerMaterialMaps ){
      // elementTable handling - if existent
      if (m_mapComposition){
          Trk::SharedObject<const Trk::ElementTable> tElementTable(new Trk::ElementTable(*m_elementTable));
          ilmIter.second->updateElementTable(tElementTable);
          if (ilmIter.second->elementTable()){
              ATH_MSG_INFO("ElementTable for LayerMaterialMap '" << ilmIter.first << "' found and syncrhonized." );
              ATH_MSG_INFO( *(ilmIter.second->elementTable()) );
          }
      }
      // detector store writing        
      if ( (detStore()->record(ilmIter.second, ilmIter.first, false)).isFailure()){
          ATH_MSG_ERROR( "Writing of LayerMaterialMap with name '" << ilmIter.first << "' was not successful." );
          delete ilmIter.second;
      } else ATH_MSG_INFO( "LayerMaterialMap: " << ilmIter.first << " written to the DetectorStore!" );
    }
    delete m_elementTable;

#ifdef TRKDETDESCR_MEMUSAGE       
    m_memoryLogger.refresh(getpid());
    ATH_MSG_INFO( "[ memory usage ] At the end of the material map creation.");    
    ATH_MSG_INFO( m_memoryLogger );                     
#endif  

    ATH_MSG_INFO( "========================================================================================= " );
    ATH_MSG_INFO( "    ->  Total mapped hits                  : "  << m_mapped         );
    double unmapped = (m_unmapped+m_mapped) ? double(m_unmapped)/double(m_unmapped+m_mapped) : 0.;
    ATH_MSG_INFO( "    ->  Total (rel.) unmapped hits         : "  << m_unmapped << " (" <<  unmapped << ")" );
    ATH_MSG_INFO( "    ->  Skipped (outisde)                  : "  << m_skippedOutside );
    ATH_MSG_INFO( "========================================================================================= " );
    ATH_MSG_INFO( "finalize() successful");
    return StatusCode::SUCCESS;
}


StatusCode Trk::MaterialMapping::retrieveTrackingGeometry()
{

    // Retrieve the TrackingGeometry from the DetectorStore
    if ((detStore()->retrieve(m_trackingGeometry, m_trackingGeometrySvc->trackingGeometryName())).isFailure()) {
        ATH_MSG_FATAL("Could not retrieve TrackingGeometry from DetectorStore!");
        return StatusCode::FAILURE;
    }
    
    // either get a string volume or the highest one
    const Trk::TrackingVolume* trackingVolume = m_trackingGeometry->highestTrackingVolume();
    
    // prepare the mapping volume
    m_mappingVolume = m_trackingGeometry->trackingVolume(m_mappingVolumeName);
    
    // register the confined layers from the TrackingVolume
    registerVolume(*trackingVolume, 0);
    
    ATH_MSG_INFO("Add "<< m_layerRecords.size() << " confined volume layers to mapping setup.");
    ATH_MSG_INFO("Add "<< m_trackingGeometry->boundaryLayers().size() << " boundary layers to mapping setup.");
    
    // register the layers from boundary surfaces
    auto bLayerIter = m_trackingGeometry->boundaryLayers().begin();
    for (; bLayerIter != m_trackingGeometry->boundaryLayers().end(); ++bLayerIter)
        insertLayerMaterialRecord(*bLayerIter->first);

    ATH_MSG_INFO("Map for "<< m_layerRecords.size() << " layers booked & prepared for mapping procedure");

    return StatusCode::SUCCESS;

}


void Trk::MaterialMapping::registerVolume(const Trk::TrackingVolume& tvol, int lvl)
{
    int sublevel = lvl+1;

    for (int indent=0; indent<sublevel; ++indent)
        std::cout << " ";
    std::cout << "TrackingVolume name: "<< tvol.volumeName() << std::endl;

    // all those to be processed
    std::vector<const Trk::Layer*> volumeLayers;
  
    // collect all material layers that have layerMaterial
    const Trk::BinnedArray< Trk::Layer >* confinedLayers = tvol.confinedLayers();
    if (confinedLayers) {
         // this go ahead with the layers
         const std::vector<const Trk::Layer*>& layers = confinedLayers->arrayObjects();
         for (int indent=0; indent<sublevel; ++indent)
             std::cout << " ";
         std::cout << "- found : "<< layers.size() << "confined Layers"<< std::endl;
         // loop over and fill them
         auto clIter  = layers.begin();
         auto clIterE = layers.end();
         for ( ; clIter != clIterE; ++clIter ) {
            // only take layers with MaterialProperties defined and which are within the mapping volume
            const Amg::Vector3D& sReferencePoint = (*clIter)->surfaceRepresentation().globalReferencePoint(); 
            bool insideMappingVolume = m_mappingVolume ? m_mappingVolume->inside(sReferencePoint) : true;
            if ((*clIter)->layerMaterialProperties() && insideMappingVolume)
                volumeLayers.push_back((*clIter));
        }
    }        
    
   // now create LayerMaterialRecords for all
   for ( auto& lIter : volumeLayers )
           insertLayerMaterialRecord(*lIter);
   
    // step dopwn the navigation tree to reach the confined volumes
    const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = tvol.confinedVolumes();
    if (confinedVolumes) {
        const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes->arrayObjects();

        for (int indent=0; indent<sublevel; ++indent)
            std::cout << " ";
        std::cout << "- found : "<< volumes.size() << "confined TrackingVolumes"<< std::endl;
        // loop over the confined volumes
        auto volumesIter = volumes.begin();
        for (; volumesIter != volumes.end(); ++volumesIter)
            if (*volumesIter) {
                registerVolume(**volumesIter, sublevel);
            }
    }

}

void Trk::MaterialMapping::insertLayerMaterialRecord(const Trk::Layer& lay){
 // first occurrance, create a new LayerMaterialRecord
 // - get the bin utility for the binned material (if necessary)
 // - get the material first
 const Trk::LayerMaterialProperties* layerMaterialProperties = lay.layerMaterialProperties();
 // - dynamic cast to the BinnedLayerMaterial
 const Trk::BinnedLayerMaterial* layerBinnedMaterial
 = dynamic_cast<const Trk::BinnedLayerMaterial*>(layerMaterialProperties);
 // get the binned array
 const Trk::BinUtility* layerMaterialBinUtility = (layerBinnedMaterial) ? layerBinnedMaterial->binUtility() : 0;
 // now fill the layer material record
 if (layerMaterialBinUtility){
     // create a new Layer Material record in the map
     Trk::LayerMaterialRecord  lmr((m_useLayerThickness ? lay.thickness() : 1.),
                                   layerMaterialBinUtility,
                                   (Trk::MaterialAssociationType)m_associationType);
     // and fill it into the map
     m_layerRecords[&lay] = lmr;                        
 }
}

