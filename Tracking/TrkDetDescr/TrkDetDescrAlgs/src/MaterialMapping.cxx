/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialMapping.cxx, (c) ATLAS Detector software
//////////////////////////////////////////////////////////////////

// Gaudi Units
#include "GaudiKernel/SystemOfUnits.h"
//TrkDetDescrAlgs
#include "TrkDetDescrAlgs/MaterialMapping.h"
#include "TrkDetDescrAlgs/LayerMaterialRecord.h"
// Trk
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkDetDescrInterfaces/IMaterialMapper.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/LayerIndex.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkGeometry/EntryLayerProvider.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/MaterialStep.h"
#include "TrkGeometry/MaterialStepCollection.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkGeometry/LayerMaterialMap.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/CompressedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"

#ifdef TRKDETDESCR_MEMUSAGE
#include <unistd.h>
#endif


Trk::MaterialMapping::MaterialMapping(const std::string& name, ISvcLocator* pSvcLocator)
: AthAlgorithm(name,pSvcLocator),
  m_trackingGeometrySvc("AtlasTrackingGeometrySvc",name),
  m_trackingGeometry(0),
  m_mappingVolumeName("Atlas"),
  m_mappingVolume(0),
  m_inputMaterialStepCollection("MaterialStepRecords"),
  m_outputLayerMaterialSetName("AtlasTrackingMaterial"),
  m_etaCutOff(6.0),
  m_associationType(1),
  m_mapMaterial(true),
  m_materialMapper("Trk::MaterialMapper/MappingMaterialMapper"),
  m_materialMappingEvents(0),
  m_maxMaterialMappingEvents(25000),
  m_mapped(0),
  m_unmapped(0),
  m_skippedOutside(0),
  m_mappedEvents(0),
  m_maxMappedEvents(-1),
  m_layerMaterialScreenOutput(0)
{
    // the name of the TrackingGeometry to be retrieved
    declareProperty("TrackingGeometrySvc"         , m_trackingGeometrySvc);
    declareProperty("MappingVolumeName"           , m_mappingVolumeName);
    // general steering
    declareProperty("EtaCutOff"                   , m_etaCutOff);
    // the toolhandle of the MaterialMapper to be used
    declareProperty("MapMaterial"                  , m_mapMaterial);
    declareProperty("MaterialMapper"               , m_materialMapper);
    declareProperty("MaximumMappingEvents"         , m_maxMaterialMappingEvents);
    // Compression section
    declareProperty("CompressMaterial"             , m_compressMaterial);
    declareProperty("CompressMaterialThickness"    , m_compressedMaterialThickness);
    declareProperty("CompressMaterialBinsX0"       , m_compressedMaterialX0Bins);
    declareProperty("CompressMaterialBinsZARho"    , m_compressedMaterialZARhoBins);
    // some job setup
    declareProperty("MaterialAssociationType"      , m_associationType);
    declareProperty("InputMaterialStepCollection"  , m_inputMaterialStepCollection);
    declareProperty("OutputLayerMaterialSetName"   , m_outputLayerMaterialSetName);
    // Maximum of mapped events
    declareProperty("MaximumMappedEvents"          , m_maxMappedEvents);
    // Output screen stuff
    declareProperty("MaterialScreenOutputLevel"    , m_layerMaterialScreenOutput);
    
}

Trk::MaterialMapping::~MaterialMapping()
{}


StatusCode Trk::MaterialMapping::initialize()
{

    ATH_MSG_INFO( "initialize()" );

    if ( (m_trackingGeometrySvc.retrieve()).isFailure() )
        ATH_MSG_WARNING("Could not retrieve TrackingGeometrySvc");

    if ( (m_materialMapper.retrieve()).isFailure() )
        ATH_MSG_WARNING("Could not retrieve MaterialMapper");
        
    return StatusCode::SUCCESS;
}


StatusCode Trk::MaterialMapping::execute()
{
    ATH_MSG_VERBOSE( "MaterialMapping execute() start" );

    // ------------------------------- get the trackingGeometry at first place
    if (!m_trackingGeometry) {
        StatusCode retrieveCode = retrieveTrackingGeometry();
        if (retrieveCode.isFailure()){
            ATH_MSG_INFO( "Could not retrieve TrackingGeometry. Exiting." );
            return retrieveCode;
        }
    }

    // get the material step vector
    const MaterialStepCollection* materialSteps = 0;

    // try this
    StatusCode retrieveCode = evtStore()->retrieve(materialSteps,m_inputMaterialStepCollection);
    if (retrieveCode.isFailure()) {

        ATH_MSG_WARNING( "[!] Could not retrieve the step vector!");
        return retrieveCode;

    } else if (m_maxMappedEvents > 0 && m_mappedEvents >= m_maxMappedEvents ) {

        ATH_MSG_VERBOSE( "Maximunm mapped events already reached. Skipping.");
        return StatusCode::SUCCESS;
        
    } else {   // successful retrieval

        // average numbers for the weighting
        int    associatedSteps  = 0 ;
        double av_eta           = 0.;
        double av_phi           = 0.;

        double eta              = 0.;
        double phi              = 0.;

        ATH_MSG_VERBOSE( "[+] Successfully read  " <<  materialSteps->size() << " geantino steps" );

        auto stepIter = materialSteps->begin();
        auto stepIterEnd = materialSteps->end();

        // loop over the hits and associate that to the layers
        for ( ; stepIter != stepIterEnd; ++stepIter) {

            double hitX  = (*stepIter)->hitX();
            double hitY  = (*stepIter)->hitY();
            double hitZ  = (*stepIter)->hitZ();

            double t     = (*stepIter)->steplength();
            double x0    = (*stepIter)->x0();
            double l0    = (*stepIter)->l0();
            double a     = (*stepIter)->A();
            double z     = (*stepIter)->Z();
            double rho   = (*stepIter)->rho();
            
            Amg::Vector3D pos(hitX,hitY,hitZ);
            
            // skip if outside the mapping volume 
            if (m_mappingVolume && !m_mappingVolume->inside(pos)){
                ++m_skippedOutside;
                continue;
            }
            
            // go ahead 
            eta = pos.eta();
            phi = pos.phi();

            // get the lowest TrackingVolume
            const Trk::TrackingVolume* associatedVolume = m_trackingGeometry->lowestTrackingVolume(pos);
            // add associated volume to volumes map
            if (!associatedVolume) 
                return StatusCode::FAILURE;

            // ------ CUT ------ (e.g. dummy/giant steps, non-physical atomic numbers, high eta ...)
            if (pos.mag() > 1. * Gaudi::Units::mm && t > 10e-10 && t < 10000. && a < 252. && a > 0. && fabs(eta) < m_etaCutOff) {
                if (associateHit(*associatedVolume, pos,t,x0,l0,a,z,rho)) {
                    // average over eta, phi, theta
                    av_eta   += eta;
                    av_phi   += phi;
                    ++associatedSteps;
                }
            }
        } // end of loop over steps

        ATH_MSG_VERBOSE(" Loop over Material steps done, now finalizing event ...");

        // check whether the event was good for at least one hit
        if (associatedSteps) {

            // at least one step was associated -> thus the event counts
            ++m_mappedEvents;

            ATH_MSG_VERBOSE(" There are associated steps, need to call finalizeEvent() & record to the MaterialMapper.");
            // accepted hits
            av_eta   /= associatedSteps;
            av_phi   /= associatedSteps;

            // finalize the event   --------------------- Layers ---------------------------------------------
            auto lIter = m_layerRecords.begin();
            for ( ; lIter != m_layerRecords.end(); ++lIter) {

                const Trk::Layer* layer = lIter->first;
                if (!layer) continue;

                Trk::AssociatedMaterial* assMatHit = (lIter->second).finalizeEvent(*layer, m_layerMaterialScreenOutput);
                if (assMatHit)
                    m_materialMapper->recordFullLayerHit(*assMatHit);
                delete assMatHit;
                
                // call the material mapper finalize method
                ATH_MSG_VERBOSE( " Calling finalizeEvent on the MaterialMapper ..." );
                // increase the number
                ++m_materialMappingEvents;
                // material mapper 
                m_materialMapper->finalizeEvent(av_eta,av_phi);

            } 
        } // the event had at least one associated hit
    }

    return StatusCode::SUCCESS;
}


bool Trk::MaterialMapping::associateHit( const Trk::TrackingVolume& associatedVolume, 
                                         const Amg::Vector3D& pos,
                                         double stepl,
                                         double x0,
                                         double l0,
                                         double a,
                                         double z,
                                         double rho)
{

   
   
   // get the intersection with the layer
   Amg::Vector3D dir = pos.normalized();
   const Trk::LayerIntersection lIntersect = associatedVolume.closestMaterialLayer(pos, dir, Trk::mappingMode, true);
   if (lIntersect.sIntersection.valid){
       ++m_mapped;
       // try to find the layer material record
       auto clIter = m_layerRecords.find(lIntersect.layer);
       if (clIter != m_layerRecords.end() ){
            // record the plain information w/o correction & intersection
           if (m_mapMaterial)
               m_materialMapper->recordMaterialHit(Trk::AssociatedMaterial(pos,
                                                                           stepl,
                                                                           x0,
                                                                           l0,
                                                                           a,
                                                                           z,
                                                                           rho,
                                                                           1.,
                                                                           &associatedVolume,
                                                                           lIntersect.layer),
                                                                           lIntersect.sIntersection.intersection);
           // LayerMaterialRecord found, add the hit
           (*clIter).second.associateGeantinoHit(lIntersect.sIntersection.intersection, stepl, x0, l0, a, z, rho, m_layerMaterialScreenOutput);
           ATH_MSG_VERBOSE(" - associate Geantino Information ( s/x0 , x0 , l0, a, z, rho ) = " 
                                << stepl/x0 << ", " << x0 << " , " << l0 << ", " << a << ", " <<  z << ", " << rho );
           ATH_MSG_VERBOSE(" - to layer with index " << lIntersect.layer->layerIndex().value() << " in '" << associatedVolume.volumeName() << "'.");
       } else {
           ATH_MSG_WARNING("No Layer found in the associated map! Should not happen.");
           return false;    
       }
    } else {
        // unmapped - record for validation
        ++m_unmapped;
        if (m_mapMaterial)
               m_materialMapper->recordMaterialHit(Trk::AssociatedMaterial(pos,
                                                                           stepl,
                                                                           x0,
                                                                           l0,
                                                                           a,
                                                                           z,
                                                                           rho,
                                                                           1.,
                                                                           &associatedVolume,
                                                                           0), Amg::Vector3D(0.,0.,0.));
    
        }
    // return 
    return true;
}


void Trk::MaterialMapping::assignLayerMaterialProperties( const Trk::TrackingVolume& tvol,
                                                          Trk::LayerMaterialMap* propSet)
{

    if (!propSet) return;

    ATH_MSG_INFO( "Processing TrackingVolume: " << tvol.volumeName() );

    // ----------------------------------- loop over the entry layers ----------------------------
    const Trk::EntryLayerProvider* entryLayerProvider = tvol.entryLayerProvider();
    if (entryLayerProvider) {
        // get the objects in a vector-like format
        const std::vector<const Trk::Layer*>& elayers = entryLayerProvider->layers();
        ATH_MSG_INFO( " --> found : " << elayers.size() << " entry Layers" );
        // iterator
        auto elayerIter = elayers.begin();
        // loop over layers
        for ( ; elayerIter != elayers.end(); ++elayerIter) {
            // check if the layer has material properties
            if (*elayerIter) {
                ATH_MSG_INFO( "   > LayerIndex: " << (**elayerIter).layerIndex() );
                // set the material!
                if (propSet) {
                    // find the pair
                    auto curIt = propSet->find((**elayerIter).layerIndex());
                    if (curIt != propSet->end()) {
                        ATH_MSG_INFO( "LayerMaterial assigned for Layer with index: " << (**elayerIter).layerIndex() );
                        // set it to the layer
                        (**elayerIter).assignMaterialProperties(*((*curIt).second), 1.);

                    }
                }
            }
        }
    }
    // -------------------------------------- END OF ENTRY LAYER SECTION --------------------------------------


    // ----------------------------------- loop over confined layers -----------------------------
    const Trk::BinnedArray< Trk::Layer >* confinedLayers = tvol.confinedLayers();
    if (confinedLayers) {
        // get the objects in a vector-like format
        const std::vector<const Trk::Layer*>& layers = confinedLayers->arrayObjects();
        ATH_MSG_INFO( " --> found : " << layers.size() << " confined Layers" );
        // the iterator over the vector
        auto layerIter = layers.begin();
        // loop over layers
        for ( ; layerIter != layers.end(); ++layerIter) {
            // assign the material and output
            if (*layerIter && (**layerIter).layerIndex().value() ) {
                ATH_MSG_INFO( "   > LayerIndex: " << (**layerIter).layerIndex() );
                // set the material!
                if (propSet) {
                    // find the pair
                    auto curIt = propSet->find((**layerIter).layerIndex());
                    if (curIt != propSet->end()) {
                        ATH_MSG_INFO( "LayerMaterial assigned for Layer with index: " << (**layerIter).layerIndex() );
                        // set it to the layer
                        (**layerIter).assignMaterialProperties(*((*curIt).second), 1.);

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
        ATH_MSG_INFO( " --> found : " << volumes.size() << " confined TrackingVolumes" );
        // the iterator over the vector
        auto volumeIter = volumes.begin();
        // loop over volumes
        for ( ; volumeIter != volumes.end(); ++volumeIter) {
            // assing the material and output
            if (*volumeIter)
                assignLayerMaterialProperties(**volumeIter, propSet); // call itself recursively
        }
    }
}


StatusCode Trk::MaterialMapping::finalize()
{

    ATH_MSG_INFO( "========================================================================================= " );
    ATH_MSG_INFO( "finalize() starts ...");

    // record the final map to be written to POOL
    Trk::LayerMaterialMap* associatedLayerMaterialProperties  = new Trk::LayerMaterialMap();

    ATH_MSG_INFO( m_layerRecords.size() << " LayerMaterialRecords to be finalized for this material mapping run." );

    // loop over the layers and output the stuff --- fill the associatedLayerMaterialProperties
    auto lIter = m_layerRecords.begin();
    for ( ; lIter != m_layerRecords.end(); ++lIter) {
        // Get the key map, the layer & the volume name
        const Trk::Layer*  layer = (lIter->first);
        Trk::LayerIndex layerKey = (layer->layerIndex());
        // get the enclosing tracking volume
        const Trk::TrackingVolume* eVolume = layer->enclosingTrackingVolume();
        // assign the string
        std::string vName = eVolume ? (eVolume->volumeName()) : "Unknown";

        ATH_MSG_INFO( "Finalize MaterialAssociation for Layer " << layerKey.value() << " in " << eVolume->volumeName() );
        // normalize - use m_finalizeRunDebug
        (lIter->second).finalizeRun(m_layerMaterialScreenOutput);

        // the new ones to create
        Trk::LayerMaterialProperties* layerMaterialProperties = 0;
        // get the vector of MaterialProperties
        const Trk::MaterialPropertiesMatrix& recordedLayerMaterialProperties = (lIter->second).associatedLayerMaterial();
        if (recordedLayerMaterialProperties.size()==1 && recordedLayerMaterialProperties[0].size()==1) {
            // create homogeneous
            const Trk::MaterialProperties* singleProperties = (recordedLayerMaterialProperties[0][0]) ?
                    (recordedLayerMaterialProperties[0][0]) : new Trk::MaterialProperties();
            layerMaterialProperties = new Trk::HomogeneousLayerMaterial(*singleProperties,1.);
            delete singleProperties;
        } else if (recordedLayerMaterialProperties.size()) {
            // get the BinUtility
            const Trk::BinUtility& recordLayerMaterialBinUtil = (*(lIter->second).binUtility());
            // create the new BinnedMaterial
            Trk::BinnedLayerMaterial* binnedMaterial = new Trk::BinnedLayerMaterial(recordLayerMaterialBinUtil,
                                                                                    recordedLayerMaterialProperties,
                                                                                    1.);
            // the compress option for 2D material - simple gif algorithm
            if (m_compressMaterial && recordLayerMaterialBinUtil.max(1) > 1) {
                layerMaterialProperties = compressMaterial(*binnedMaterial);
                delete binnedMaterial;
            } else // this is the option for 1D -> don't compress
                layerMaterialProperties = binnedMaterial;
        }
        // fill them into the map that is written to POOL
        (*associatedLayerMaterialProperties)[layerKey] =  layerMaterialProperties;
    }

    ATH_MSG_INFO( " LayerMaterialProperties for " << associatedLayerMaterialProperties->size() << " layers recorded." );

    // write output to detector store
    if ( (detStore()->record(associatedLayerMaterialProperties, m_outputLayerMaterialSetName, false)).isFailure())
        ATH_MSG_ERROR( "Writing to DetectorStore was not successful!" );
    else {
        ATH_MSG_INFO( "LayerMaterialPropertiesMap: " << m_outputLayerMaterialSetName << " written to the DetectorStore!" );
    }

    ATH_MSG_INFO( "========================================================================================= " );
    ATH_MSG_INFO( "    ->  Total mapped hits                  : "  << m_mapped         );
    double unmapped = (m_unmapped+m_mapped) ? double(m_unmapped)/double(m_unmapped+m_mapped) : 0.;
    ATH_MSG_INFO( "    ->  Total (rel.) unmapped hits         : "  << m_unmapped << " (" <<  unmapped << ")" );
    ATH_MSG_INFO("     ->  Skipped (outisde)                  : "  << m_skippedOutside );
    ATH_MSG_INFO( "========================================================================================= " );
    ATH_MSG_INFO( "finalize() successful");

    return StatusCode::SUCCESS;

}


StatusCode Trk::MaterialMapping::retrieveTrackingGeometry()
{

    // Retrieve the TrackingGeometry from the DetectorStore
    if ((detStore()->retrieve(m_trackingGeometry, m_trackingGeometrySvc->trackingGeometryName())).isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve TrackingGeometry from DetectorStore!" );
        return StatusCode::FAILURE;
    }
    // either get a string volume or the highest one

    const Trk::TrackingVolume* trackingVolume = m_trackingGeometry->highestTrackingVolume();
    
    // prepare the mapping volume
    m_mappingVolume = m_trackingGeometry->trackingVolume(m_mappingVolumeName);
    
    registerVolume(*trackingVolume, 0);

    ATH_MSG_INFO( "Map for " << m_layerRecords.size() << " Layers booked & prepared" );

    return StatusCode::SUCCESS;

}


void Trk::MaterialMapping::registerVolume(const Trk::TrackingVolume& tvol, int lvl)
{
    int sublevel = lvl+1;

    for (int indent=0; indent<sublevel; ++indent)
        std::cout << "  ";
    std::cout << "TrackingVolume name: " << tvol.volumeName() << std::endl;

    // all those to be processed
    std::vector<const Trk::Layer*> volumeLayers;
  
    // collect all material layers that have layerMaterial
    const Trk::BinnedArray< Trk::Layer >* confinedLayers = tvol.confinedLayers();
    if (confinedLayers) {
         // this go ahead with the layers
         const std::vector<const Trk::Layer*>& layers = confinedLayers->arrayObjects();
         for (int indent=0; indent<sublevel; ++indent)
             std::cout << "  ";
         std::cout << "- found : " << layers.size() << " confined Layers" << std::endl;
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
         
    
    // collect all entry layers treatment
    const Trk::EntryLayerProvider* entryLayerProvider = tvol.entryLayerProvider();
    if (entryLayerProvider) {
        // -------------------------
        const std::vector<const Trk::Layer*>& entryLayers = entryLayerProvider->layers();
        for (int indent=0; indent<sublevel; ++indent)
            std::cout << "  ";
        std::cout << "- found : " << entryLayers.size() << " entry Layers" << std::endl;
        auto elIter  = entryLayers.begin();
        auto elIterE = entryLayers.end();
        for ( ; elIter != elIterE; ++elIter ) {
           const Amg::Vector3D& sReferencePoint = (*elIter)->surfaceRepresentation().globalReferencePoint(); 
           bool insideMappingVolume = m_mappingVolume ? m_mappingVolume->inside(sReferencePoint) : true;
           if ((*elIter)->layerMaterialProperties() && insideMappingVolume)
               volumeLayers.push_back((*elIter));
        }
   }

   // now create LayerMaterialRecords for all
   auto lIter  = volumeLayers.begin();
   auto lIterE = volumeLayers.end();
   for ( ; lIter != lIterE; ++lIter ){
           // first occurrance, create a new LayerMaterialRecord
           // - get the bin utility for the binned material (if necessary)
           // - get the material first
           const Trk::LayerMaterialProperties* layerMaterialProperties = (*lIter)->layerMaterialProperties();
           // - dynamic cast to the BinnedLayerMaterial
           const Trk::BinnedLayerMaterial* layerBinnedMaterial
           = dynamic_cast<const Trk::BinnedLayerMaterial*>(layerMaterialProperties);
           // get the binned array
           const Trk::BinUtility* layerMaterialBinUtility = (layerBinnedMaterial) ? layerBinnedMaterial->binUtility() : 0;
           // now fill the layer material record
           if (layerMaterialBinUtility){
               // create a new Layer Material record in the map
               Trk::LayerMaterialRecord  lmr((*lIter)->thickness(),
                                             layerMaterialBinUtility,
                                             (Trk::MaterialAssociationType)m_associationType);
              // and fill it into the map
              m_layerRecords[(*lIter)] = lmr;                        
          }
    }

    // step dopwn the navigation tree to reach the confined volumes
    const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = tvol.confinedVolumes();
    if (confinedVolumes) {
        const std::vector<const Trk::TrackingVolume*>& volumes = confinedVolumes->arrayObjects();

        for (int indent=0; indent<sublevel; ++indent)
            std::cout << "  ";
        std::cout << "- found : " << volumes.size() << " confined TrackingVolumes" << std::endl;
        // loop over the confined volumes
        auto volumesIter = volumes.begin();
        for (; volumesIter != volumes.end(); ++volumesIter)
            if (*volumesIter) {
                registerVolume(**volumesIter, sublevel);
            }
    }

}

Trk::CompressedLayerMaterial* Trk::MaterialMapping::compressMaterial(const Trk::BinnedLayerMaterial& binnedMaterial) {
    // get the matrix
    const Trk::MaterialPropertiesMatrix& materialProperties = binnedMaterial.fullMaterial();
    // the vector to be created and reserve the maximum
    Trk::MaterialPropertiesVector materialVector;
    materialVector.reserve(m_compressedMaterialZARhoBins*m_compressedMaterialX0Bins);
    // this method creates a compressed representation of the BinnedLayerMaterial
    const Trk::BinUtility* cbinutil = binnedMaterial.binUtility();
    // nF x nS
    size_t nFirstBins  = cbinutil->max(0)+1;
    size_t nSecondBins = cbinutil->max(1)+1;
    // low, high boundaries
    double x0min        = 10e10;
    double x0max        = 0.;
    double avZArhoMin   = 10e10;
    double avZArhoMax   = 0.;
    // create two maps, the compression map and the index map
    std::vector< std::vector<unsigned short int> > materialBins;
    // (1) FIRST LOOP, get boundaries
    materialBins.reserve(nSecondBins);
    for (size_t isec = 0; isec < nSecondBins; ++isec) {
        std::vector<unsigned short int> firstbins(nFirstBins,0);
        materialBins.push_back(firstbins);
        // loop over the bins
        for (size_t ifir = 0; ifir < nFirstBins; ++ifir) {
            // get the current material properties
            const Trk::MaterialProperties* matProp = materialProperties[isec][ifir];
            if (matProp) {
                double tinX0   = matProp->thicknessInX0();
                double avZArho = matProp->zOverAtimesRho();
                x0min = tinX0 < x0min ? tinX0 : x0min;
                x0max = tinX0 > x0max ? tinX0 : x0max;
                avZArhoMin = avZArho < avZArhoMin ? avZArho : avZArhoMin;
                avZArhoMax = avZArho > avZArhoMax ? avZArho : avZArhoMax;
            }
        }
    }
    // min / max is defined, find step size
    double stepX0    = (x0max-x0min)/m_compressedMaterialX0Bins;
    double stepZArho = (avZArhoMax-avZArhoMin)/m_compressedMaterialZARhoBins;
    // get the material histogram
    std::vector< std::vector< std::vector< Trk::IndexedMaterial> > > materialHistogram;
    materialHistogram.reserve(m_compressedMaterialZARhoBins);
    // prepare the histogram
    for (size_t izarho = 0; izarho < m_compressedMaterialZARhoBins; ++izarho) {
        std::vector< std::vector < Trk::IndexedMaterial > > x0materialbins;
        x0materialbins.reserve(m_compressedMaterialX0Bins);
        for (size_t ix0 = 0; ix0 < m_compressedMaterialX0Bins; ++ix0) {
            std::vector < Trk::IndexedMaterial > materialBin;
            x0materialbins.push_back( materialBin );
        }
        materialHistogram.push_back(x0materialbins);
    }
    // fill the histogram
    for (size_t isec = 0; isec < nSecondBins; ++isec) {
        for (size_t ifir = 0; ifir < nFirstBins; ++ifir) {
            // get the material properties
            const Trk::MaterialProperties* matProp = dynamic_cast<const Trk::MaterialProperties*>(materialProperties[isec][ifir]);
            if (matProp) {
                // calculate the bins of the material histogram
                double tinX0   = matProp->thicknessInX0();
                double avZArho = matProp->zOverAtimesRho();
                int x0bin    =  int(  (tinX0-x0min)/stepX0 );
                int zarhobin =  int(  (avZArho-avZArhoMin)/stepZArho );
                // range protection
                x0bin    = ( (size_t)x0bin >= m_compressedMaterialX0Bins) ? m_compressedMaterialX0Bins-1 : x0bin;
                x0bin    = x0bin < 0 ? 0 : x0bin;
                zarhobin = ( (size_t)zarhobin >= m_compressedMaterialZARhoBins) ? m_compressedMaterialZARhoBins-1 : zarhobin;
                zarhobin = zarhobin < 0 ? 0 : zarhobin;
                // create indexed material
                Trk::IndexedMaterial idxMaterial;
                idxMaterial.materialProperties = matProp;
                idxMaterial.firstBin  = ifir;
                idxMaterial.secondBin = isec;
                // fill into the material histogram
                materialHistogram[zarhobin][x0bin].push_back(idxMaterial);
            }
        }
    }
    // merge the bins and ready
    materialVector.push_back(0);
    // prepare the histogram
    for (size_t izarho = 0; izarho < m_compressedMaterialZARhoBins; ++izarho) {
        for (size_t ix0 = 0; ix0 < m_compressedMaterialX0Bins; ++ix0) {
            // get the indexed material properties
            std::vector< Trk::IndexedMaterial > indexedMaterial = materialHistogram[izarho][ix0];
            if (indexedMaterial.size()) {
                double avT          = 0.; // thickness: by default on one layer it should be the same !
                double tinX0        = 0.;
                double tinL0        = 0.;
                double avA          = 0.;
                double avZ          = 0.;
                double avRho        = 0.;
                std::vector< Trk::IndexedMaterial >::iterator idmIter = indexedMaterial.begin();
                std::vector< Trk::IndexedMaterial >::iterator idmIterEnd = indexedMaterial.end();
                for ( ; idmIter != idmIterEnd; ++idmIter ) {
                    tinX0 += (*idmIter).materialProperties->thicknessInX0();
                    tinL0 += (*idmIter).materialProperties->thicknessInL0();
                    avA   += (*idmIter).materialProperties->averageA();
                    avZ   += (*idmIter).materialProperties->averageZ();
                    avRho += (*idmIter).materialProperties->averageRho();
                }
                double measure = 1./(indexedMaterial.size());
                // average it
                tinX0 *= measure;
                tinL0 *= measure;
                avA   *= measure;
                avZ   *= measure;
                avRho *= measure;
                avT   *= measure;
                // compress to a model thickness [ rho affected ]
                avRho *= avT/m_compressedMaterialThickness;
                materialVector.push_back(new Trk::MaterialProperties(m_compressedMaterialThickness,
                                                                             m_compressedMaterialThickness/tinX0,
                                                                             m_compressedMaterialThickness/tinL0,
                                                                             avA,
                                                                             avZ,
                                                                             avRho));
                // now set the index
                int matindex = int(materialVector.size()-1);
                idmIter = indexedMaterial.begin();
                for ( ; idmIter != idmIterEnd; ++idmIter )
                    materialBins[(*idmIter).secondBin][(*idmIter).firstBin] = matindex;
            }
        }
    }

    // change the 2bin matrix to a 1bin vector (better for persistency)
    std::vector<unsigned short int> materialBinsVector;
    materialBinsVector.reserve( (cbinutil->max(0)+1)*(cbinutil->max(1)+1) );
    std::vector< std::vector<unsigned short int> >::iterator binVecIter    = materialBins.begin();
    std::vector< std::vector<unsigned short int> >::iterator binVecIterEnd = materialBins.end();
    for ( ; binVecIter != binVecIterEnd; ++binVecIter) {
        std::vector<unsigned short int>::iterator binIter    = (*binVecIter).begin();
        std::vector<unsigned short int>::iterator binIterEnd = (*binVecIter).end();
        for ( ; binIter != binIterEnd; ++binIter )
            materialBinsVector.push_back(*binIter);
    }
    // create the compressed material
    return new Trk::CompressedLayerMaterial(*cbinutil,materialVector,materialBinsVector);

}

