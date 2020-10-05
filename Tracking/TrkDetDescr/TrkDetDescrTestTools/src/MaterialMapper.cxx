/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialMapper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTestTools/MaterialMapper.h"
#include "TrkDetDescrUtils/MaterialAssociationType.h"
#include "TrkGeometry/AssociatedMaterial.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/MaterialLayer.h"
// Gaudi
#include "GaudiKernel/ITHistSvc.h" 


// constructor
Trk::MaterialMapper::MaterialMapper(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
   m_incidentSvc("IncidentSvc", n),
  m_materialAssociationType(1),
  m_maxMappingEvents(100000),
  m_processedEvents(0),
  m_totalMaterialTree("TotalMaterial"),
  m_validationTree(0),
  m_validationTreeName("MaterialMapper"),
  m_validationTreeDescription("Material Effects Updator information"),
  m_validationTreeFolder("/val/MaterialMapper"),
  m_materialSteps(0),
  m_averageEta{}, m_averagePhi{}, m_mappedPath{}, m_mappedPathInX0{},
  m_mappedPathInL0{}, m_mappedPathRho{}, m_mappedPathZARho{},
  m_unmappedPathInX0{},
  m_volumeValidation(true),
  m_volumeTreePrefix("VolumeMaterial"),
  m_layerValidation(true),
  m_layerTreePrefix("LayerMaterial_"),
  m_surfaceValidation(true),
  m_surfaceTreePrefix("SurfaceMaterial_")
{
    declareInterface<IMaterialMapper>(this);

    // The maximum mapping events to control the output file size
    declareProperty("MaximumMappingEvents",       m_maxMappingEvents);
    // specification for the Validation Tree
    declareProperty("ValidationTreeName",         m_validationTreeName);
    declareProperty("ValidationTreeDescription",  m_validationTreeDescription);
    declareProperty("ValidationTreeFolder",       m_validationTreeFolder);
    // the total material statistics              
    declareProperty("TotalMaterialTreeName",      m_totalMaterialTree);
    // declare the association type               
    declareProperty("MaterialAssociationType",    m_materialAssociationType);
    // the volume validation                      
    declareProperty("TrackingVolumeValidation",   m_volumeValidation);
    declareProperty("TrackingVolumeTreePrefix",   m_volumeTreePrefix);
    // the layer Validation                       
    declareProperty("DetailedLayerValidation",    m_layerValidation);
    declareProperty("DetailedLayerTreePrefix",    m_layerTreePrefix);
    // the surface Validation
    declareProperty("DetailedSurfaceValidation",  m_surfaceValidation);
    declareProperty("DetailedSurfaceTreePrefix",  m_surfaceTreePrefix);

}

// destructor
Trk::MaterialMapper::~MaterialMapper()
{
    // memory cleanup
    if (m_volumeValidation) {
        auto volTreeIter = m_volumeTrees.begin();
        auto endVolTreeIter = m_volumeTrees.end();
        for ( ; volTreeIter != endVolTreeIter; delete (volTreeIter->second), ++volTreeIter) {}
    }

    if (m_layerValidation) {
        // delete the layer hit TTree
        auto layTreeIter  = m_layerTrees.begin();
        auto endLayTreeIter = m_layerTrees.end();
        for ( ; layTreeIter != endLayTreeIter; delete (layTreeIter->second), ++layTreeIter) {}
        // delete the layer per event TTree
        layTreeIter  = m_layerFullTrees.begin();
        endLayTreeIter = m_layerFullTrees.end();
        for ( ; layTreeIter != endLayTreeIter; delete (layTreeIter->second), ++layTreeIter) {}
    }

    if (m_surfaceValidation) {
        // finalize the Hit Tree
        auto surfTreeIter  = m_surfaceTrees.begin();
        auto endSurfTreeIter = m_surfaceTrees.end();
        for ( ; surfTreeIter != endSurfTreeIter; delete (surfTreeIter->second), ++surfTreeIter) {}
    }
}

// Athena standard methods
// initialize
StatusCode Trk::MaterialMapper::initialize()
{

    // Athena/Gaudi framework
    if (m_incidentSvc.retrieve().isFailure()){
        ATH_MSG_WARNING("Could not retrieve " << m_incidentSvc << ". Exiting.");
        return StatusCode::FAILURE;
    }
    // register to the incident service: EndEvent for histogram filling and reset
    m_incidentSvc->addListener( this, IncidentType::EndEvent);
    
    // book the Tree 
    bookValidationTree();
    return StatusCode::SUCCESS;
}     

// finalize
StatusCode Trk::MaterialMapper::finalize()
{
    ATH_MSG_INFO(  "finalize() successful" );
    return StatusCode::SUCCESS;
}

void Trk::MaterialMapper::recordMaterialHit(const AssociatedMaterial& amhit, const Amg::Vector3D& projectedPosition) const
{

    if (m_processedEvents >= m_maxMappingEvents) return;

    // ----------------------------------- Recording Hits section --------------------------------
    const Amg::Vector3D& position = amhit.materialPosition();

    // get the tracking volume
    const Trk::TrackingVolume* tvol = amhit.associatedTrackingVolume();
    const Trk::Layer*           lay = amhit.associatedLayer();
    
    if (!tvol) {
        ATH_MSG_WARNING("Could not associate TrackingVolume to material step!");
        return;
    }
    
    // ----------------------------------- The Material Validation code --------------------------
    const Trk::TrackingVolume* itvol = tvol;
    while (itvol) {
        // get the volume tree object
        ATH_MSG_VERBOSE( "getting volTreeObj for:   " << itvol->volumeName() << "   at:   " << itvol );
        Trk::VolumeTreeObject* volTreeObj   = volumeTreeObject(lay,itvol);
        if (volTreeObj) {
            // add the quantities
            (*volTreeObj).path          += amhit.steplength();
            (*volTreeObj).pathInX0      += amhit.steplengthInX0();
            (*volTreeObj).pathInL0      += amhit.steplengthInL0();
            (*volTreeObj).pathZARho     += (amhit.Z()/amhit.A())*amhit.rho()*amhit.steplength();
        } else
            ATH_MSG_WARNING( "Cannot find/create VolumeTreeObject for volume '" << itvol->volumeName() << "'." );
        itvol = itvol->getMotherVolume();
        if (itvol) ATH_MSG_VERBOSE( "motherVolume found:   " << itvol->volumeName() );
    }

    // -------------------------------- main tree section -----------------------------------------
    // eta / phi
    m_averageEta += position.eta();
    m_averagePhi += position.phi();
    // effectively crossed Z/A*rho
    double steplength = amhit.steplength();
    // fill the variables for spatial information & event characteristics
    if (lay) {
        m_mappedPath            +=  steplength;
        m_mappedPathInX0        +=  amhit.steplengthInX0();
        m_mappedPathInL0        +=  amhit.steplengthInL0();
        m_mappedPathRho         +=  amhit.rho()*steplength;
        m_mappedPathZARho       +=  (amhit.Z()/amhit.A())*amhit.rho()*steplength;
        ATH_MSG_VERBOSE("[ MaterialMapper ] Accumulated Path in Rho (*Z/A) = " << m_mappedPathRho << " ( " <<  m_mappedPathZARho << " )");
    } else 
        m_unmappedPathInX0 +=  amhit.steplengthInX0();
    
    // mapping information
    m_mapped[m_materialSteps]                    = lay ? lay->layerIndex().value() : 0;
    // the position & step information 
    m_materialAccumPathInX0[m_materialSteps]     = m_mappedPathInX0;
    m_materialAccumPathZARho[m_materialSteps]    = m_mappedPathZARho;
    m_materialStepPath[m_materialSteps]          = steplength;
    m_materialStepX0[m_materialSteps]            = amhit.x0();
    m_materialStepL0[m_materialSteps]            = amhit.l0();
    m_materialStepA[m_materialSteps]             = amhit.A();
    m_materialStepZ[m_materialSteps]             = amhit.Z();
    m_materialStepRho[m_materialSteps]           = amhit.rho();
    m_materialStepPositionX[m_materialSteps]     = position.x();
    m_materialStepPositionY[m_materialSteps]     = position.y();
    m_materialStepPositionZ[m_materialSteps]     = position.z();
    m_materialStepPositionR[m_materialSteps]     = position.perp();
    // record the projected position as well         
    m_materialProjPositionX[m_materialSteps]     = projectedPosition.x();
    m_materialProjPositionY[m_materialSteps]     = projectedPosition.y();
    m_materialProjPositionZ[m_materialSteps]     = projectedPosition.z();
    m_materialProjPositionR[m_materialSteps]     = projectedPosition.perp();
    m_materialProjDistance[m_materialSteps]      = (position-projectedPosition).mag();
    // and increase the number of steps
    ++m_materialSteps;
    

    // record the layer hit
    if (m_layerValidation) recordLayerHit(amhit);
}


void Trk::MaterialMapper::finalizeVolumeHits(bool mapped) const
{
    ATH_MSG_VERBOSE( "finalizeEvent() on Volume hits " );   

    auto volTreeIter = mapped ? m_volumeTrees.begin() : m_volumeTreesUnmapped.begin();
    auto endVolTreeIter = mapped ? m_volumeTrees.end() : m_volumeTreesUnmapped.end();

    // do the loop
    for ( ; volTreeIter != endVolTreeIter; ++volTreeIter ) {

        Trk::VolumeTreeObject* volTreeObj = volTreeIter->second;

        if (!volTreeObj) continue;

        if ((*volTreeObj).path > 0.) {
            // spatial mapping
            (*volTreeObj).eta         = m_averageEta;
            (*volTreeObj).phi         = m_averagePhi;
            // fill the volume Tree Object
            (*volTreeObj).tree->Fill();          
        }
        // reset
        (*volTreeObj).eta           = 0.;
        (*volTreeObj).phi           = 0.;
        (*volTreeObj).path          = 0.;
        (*volTreeObj).pathInX0      = 0.;
        (*volTreeObj).pathInL0      = 0.;
        (*volTreeObj).pathZARho     = 0.;

    }
}


void Trk::MaterialMapper::recordLayerHit(const AssociatedMaterial& amhit, bool full) const
{
    
    if (m_processedEvents >= m_maxMappingEvents) return;
    
    const Trk::Layer* lay = amhit.associatedLayer();
    if (m_layerValidation && lay) {
        // try to find the histogram
        Trk::LayerTreeObject* layTreeObj   = layerTreeObject(*lay,full);
        if (layTreeObj
                && (*layTreeObj).layerHits < TRKDETDESCRTOOLS_MAXLAYERHITS
                && amhit.steplengthInX0() > 0.)
        {

            const Amg::Vector3D&  pos = amhit.materialPosition();

            // hit positions
            double posX   = pos.x();
            double posY   = pos.y();
            double posZ   = pos.z();
            double posR   = pos.perp();
            double posEta = pos.eta();

            // fill the hit position --------------------
            (*layTreeObj).hitPositionX[(*layTreeObj).layerHits] = posX;
            (*layTreeObj).hitPositionY[(*layTreeObj).layerHits] = posY;
            (*layTreeObj).hitPositionZ[(*layTreeObj).layerHits] = posZ;
            (*layTreeObj).hitPositionR[(*layTreeObj).layerHits] = posR;
            (*layTreeObj).hitPositionEta[(*layTreeObj).layerHits] = posEta;

            // increase the layer Hits
            ++(*layTreeObj).layerHits;
            ++(*layTreeObj).densedHits;

            // fill the densed Hit position ---------------
            (*layTreeObj).densedHitX         += posX;
            (*layTreeObj).densedHitY         += posY;
            (*layTreeObj).densedHitZ         += posZ;
            (*layTreeObj).densedHitR         += posR;
            (*layTreeObj).densedHitPhi       += pos.phi();
            (*layTreeObj).densedHitTheta     += pos.theta();

            // fill the correction factor ------------------
            (*layTreeObj).correctionFactor   += amhit.correctionFactor();
            (*layTreeObj).path               += amhit.steplength();
            (*layTreeObj).pathInX0           += amhit.steplengthInX0();
            (*layTreeObj).pathInL0           += amhit.steplengthInL0();
            (*layTreeObj).A                  += amhit.A() * amhit.rho() * amhit.steplength();
            (*layTreeObj).Z                  += amhit.Z() * amhit.rho() * amhit.steplength();
            (*layTreeObj).rho                += amhit.rho() * amhit.steplength();

        }
    }
}

void Trk::MaterialMapper::finalizeLayerHits() const
{

    ATH_MSG_VERBOSE( "finalizeEvent() on Layer hits " );

    for (size_t iltm = 0; iltm < 2; ++iltm)     {   
        // loop over all layer Tree objects and fill them
        auto layObjIter  = iltm ? m_layerFullTrees.begin() : m_layerTrees.begin();
        auto endIter     = iltm ? m_layerFullTrees.end() : m_layerTrees.end();
        // do the loop
        for ( ; layObjIter != endIter; ++layObjIter ) {
        
            Trk::LayerTreeObject* layTreeObj = layObjIter->second;
        
            if (!layTreeObj) continue;
        
            // get the number of single hits
            int hits = (*layTreeObj).densedHits;
        
            if ((*layTreeObj).pathInX0 > 0.) {
                if (hits>1) {
                    (*layTreeObj).densedHitX         /= hits;
                    (*layTreeObj).densedHitY         /= hits;
                    (*layTreeObj).densedHitZ         /= hits;
                    (*layTreeObj).densedHitR         /= hits;
                    (*layTreeObj).densedHitPhi       /= hits;
                    (*layTreeObj).densedHitTheta     /= hits;
                    (*layTreeObj).correctionFactor   /= hits;
                    (*layTreeObj).A                  /= (*layTreeObj).rho;
                    (*layTreeObj).Z                  /= (*layTreeObj).rho;
                    (*layTreeObj).rho                /= (*layTreeObj).path;
                }
                // fill the layer Tree Object
                (*layTreeObj).tree->Fill();          
            }
            // reset
            (*layTreeObj).layerHits         = 0;
            (*layTreeObj).path              = 0.;
            (*layTreeObj).pathInX0          = 0.;
            (*layTreeObj).pathInL0          = 0.;
            (*layTreeObj).densedHits        = 0;
            (*layTreeObj).densedHitX        = 0.;
            (*layTreeObj).densedHitY        = 0.;
            (*layTreeObj).densedHitZ        = 0.;
            (*layTreeObj).densedHitR        = 0.;
            (*layTreeObj).densedHitPhi      = 0.;
            (*layTreeObj).densedHitTheta    = 0.;
            (*layTreeObj).correctionFactor  = 0.;
            (*layTreeObj).A                 = 0.;
            (*layTreeObj).Z                 = 0.;
            (*layTreeObj).rho               = 0.;
        }
    }
}


void Trk::MaterialMapper::recordSurfaceHit(const Amg::Vector2D& locpos,
                                           const AssociatedMaterial& amhit) const
{
 
   if (m_processedEvents >= m_maxMappingEvents) return;
 
   const Trk::Layer* lay = amhit.associatedLayer();
   if (m_surfaceValidation && lay) {
        // try to find the histogram
        Trk::SurfaceTreeObject* surfTreeObj = surfaceTreeObject(*lay);
        // now fill
        if (surfTreeObj) {
            // fill it
            (*surfTreeObj).loc1               += locpos[0];
            (*surfTreeObj).loc2               += locpos[1];
            (*surfTreeObj).eta                += amhit.materialPosition().eta();
            (*surfTreeObj).correctionFactor   += amhit.correctionFactor();
            (*surfTreeObj).path               += amhit.steplength();
            (*surfTreeObj).pathInX0           += amhit.steplengthInX0();
            (*surfTreeObj).pathInL0           += amhit.steplengthInL0();
            (*surfTreeObj).A                  += amhit.A()*amhit.steplength()*amhit.rho();
            (*surfTreeObj).Z                  += amhit.Z()*amhit.steplength()*amhit.rho();
            (*surfTreeObj).rho                += amhit.rho()*amhit.steplength();
            // increase the surface hits
            ++(*surfTreeObj).surfaceHits;

        }
    }
}


void Trk::MaterialMapper::finalizeSurfaceHits() const
{

    ATH_MSG_VERBOSE( "finalizeEvent() on Surface hits " );   

    // loop over all layer Tree objects and fill them
    auto surfObjIter  = m_surfaceTrees.begin();
    auto endIter      = m_surfaceTrees.end();
    // do the loop
    for ( ; surfObjIter != endIter; ++surfObjIter ) {
        Trk::SurfaceTreeObject* surfTreeObj = surfObjIter->second;
        // get the number of single hits
        int hits = (*surfTreeObj).surfaceHits;
        // fill only if hits have been there
        if ((*surfTreeObj).pathInX0 > 0.) {
            if (hits>1) {
                (*surfTreeObj).loc1               /= hits;
                (*surfTreeObj).loc2               /= hits;
                (*surfTreeObj).eta                /= hits;
                (*surfTreeObj).correctionFactor   /= hits;
                (*surfTreeObj).A                  /= hits; 
                (*surfTreeObj).Z                  /= hits; 
                (*surfTreeObj).rho                /= hits; 
            }
            // fill the tree
            (*surfTreeObj).tree->Fill();
        }
        // reset
        (*surfTreeObj).loc1               = 0.;
        (*surfTreeObj).loc2               = 0.;
        (*surfTreeObj).eta                = 0.;

        (*surfTreeObj).correctionFactor   = 0.;
        (*surfTreeObj).path               = 0.;
        (*surfTreeObj).pathInX0           = 0.;
        (*surfTreeObj).pathInL0           = 0.;
        (*surfTreeObj).rho                = 0.;

        (*surfTreeObj).surfaceHits        = 0;

    }
}


Trk::VolumeTreeObject* Trk::MaterialMapper::volumeTreeObject(const Trk::Layer* lay, const Trk::TrackingVolume* tvol) const
{
    // try to find the histogram
    auto endIter =  lay ? m_volumeTrees.end() : m_volumeTreesUnmapped.end();
    auto findIter = lay ? m_volumeTrees.end() : m_volumeTreesUnmapped.end();

    Trk::VolumeTreeObject* tvolTreeObj = 0;

    findIter = lay ? m_volumeTrees.find(tvol) : m_volumeTreesUnmapped.find(tvol);

    if (findIter == endIter) {

        TString   tvolName   = tvol->volumeName();
        tvolName.ReplaceAll("::","_");

        TString   treeName   = tvolName;
                  treeName  += "_";
                  treeName  += m_volumeTreePrefix;
        if (!lay) treeName  += "_UNMAPPED";

        TString   treeTitle  = "TrackingVolume : ";
                  treeTitle += tvolName;
        if (!lay) treeTitle += " - UNMAPPED";

        TString treeRegName  = "/val/";
                treeRegName += treeName;

        ATH_MSG_INFO( "No Tree found for " << treeTitle );
        ATH_MSG_INFO( "   -> Booking it now with register name : " << treeRegName.Data() );

        tvolTreeObj = new Trk::VolumeTreeObject(treeName, treeTitle);
        if (lay)
            m_volumeTrees.insert(std::make_pair(tvol,tvolTreeObj));
        else
            m_volumeTreesUnmapped.insert(std::make_pair(tvol,tvolTreeObj));

        // now register the Tree
        ITHistSvc* tHistSvc = 0;
        if (service("THistSvc",tHistSvc).isFailure()) {
            ATH_MSG_ERROR( "initialize() Could not find Hist Service  -> Switching Tree output for this volume off !" );
            delete tvolTreeObj; tvolTreeObj = 0;
        }
        else if (tHistSvc && (tHistSvc->regTree(treeRegName.Data(), (*tvolTreeObj).tree)).isFailure()) {
            ATH_MSG_ERROR( "initialize() Could not register the validation Tree -> Switching Tree output for this volume off !" );
            delete tvolTreeObj; tvolTreeObj = 0;
        }

    } else  // a tree is found
        tvolTreeObj = findIter->second;

    return tvolTreeObj;
}


Trk::LayerTreeObject* Trk::MaterialMapper::layerTreeObject(const Trk::Layer& lay, bool full) const
{
    if (!lay.layerIndex().value()) return 0;

    // try to find the histogram
    auto endIter  = m_layerTrees.end();
    auto findIter = m_layerTrees.end();

    Trk::LayerTreeObject* layTreeObj = 0;

    findIter = full ?  m_layerFullTrees.find(&lay) : m_layerTrees.find(&lay);
    if (findIter == endIter) {
        // check if it is a boundary MaterialLayer
        const Trk::MaterialLayer* mLayer = dynamic_cast<const Trk::MaterialLayer*>(&lay);
        if (mLayer)
            ATH_MSG_INFO("MaterialLayer from BoundarySurface detected.");
        // get the TrackingVolume
        const Trk::TrackingVolume* enclosingVolume = lay.enclosingTrackingVolume();
        TString tvolName = (enclosingVolume) ?  enclosingVolume->volumeName() : "BoundaryLayers";
        tvolName.ReplaceAll("::","_");

        TString treeName   = tvolName;
                treeName  += "_";
                treeName  += m_layerTreePrefix;

        TString layerType = (lay.surfaceRepresentation().type() == Trk::Surface::Cylinder) ? 
                "CylinderLayer_" : "DiscLayer_";
        if (full) treeName += "full_";
                treeName  += layerType;
        if (mLayer) treeName += "boundary_";
                treeName  += lay.layerIndex().value();

        TString treeTitle  = "TrackingVolume :";
                treeTitle += tvolName;

        TString treeRegName  = "/val/";
                treeRegName += treeName;

        ATH_MSG_INFO( "No Tree found for Layer " << lay.layerIndex().value() << " in Volume '" << tvolName << "'.");
        ATH_MSG_INFO( "   -> Booking it now with register name : " << treeRegName.Data() );

        layTreeObj = new Trk::LayerTreeObject(treeName, treeTitle);
        if (full) m_layerFullTrees.insert(std::make_pair(&lay,layTreeObj));
        else m_layerTrees.insert(std::make_pair(&lay,layTreeObj));

        // now register the Tree
        ITHistSvc* tHistSvc = 0;
        if (service("THistSvc",tHistSvc).isFailure()) {
            ATH_MSG_ERROR( "initialize() Could not find Hist Service  -> Switching Tree output for this layer off !" );
            delete layTreeObj; layTreeObj = 0;
        }
        else if (tHistSvc && (tHistSvc->regTree(treeRegName.Data(), (*layTreeObj).tree)).isFailure()) {
            ATH_MSG_ERROR( "initialize() Could not register the validation Tree -> Switching Tree output for this layer off !" );
            delete layTreeObj; layTreeObj = 0;
        }

    } else  // a tree is found
        layTreeObj = findIter->second;

    return layTreeObj;
}


Trk::SurfaceTreeObject* Trk::MaterialMapper::surfaceTreeObject(const Trk::Layer& lay) const
{
    // try to find the histogram
    auto endIter  = m_surfaceTrees.end();
    auto findIter = m_surfaceTrees.end();

    Trk::SurfaceTreeObject* surfTreeObj = 0;

    findIter = m_surfaceTrees.find(&lay);
    if (findIter == endIter) {
        // get the TrackingVolume
        const Trk::TrackingVolume* enclosingVolume = lay.enclosingTrackingVolume();
        TString volumeName = (enclosingVolume) ?  enclosingVolume->volumeName() : "Unknown";

        TString treeName   = m_surfaceTreePrefix;
                treeName  += lay.layerIndex().value();

        TString treeTitle  = "TrackingVolume :";
                treeTitle += volumeName;

        TString treeRegName  = "/val/";
                treeRegName += treeName;

        ATH_MSG_INFO( "No Tree found for Layer " << lay.layerIndex().value()
                << " in Volume '" << volumeName << "'." );
        ATH_MSG_INFO( "   -> Booking it now with register name : " << treeRegName.Data() );

        surfTreeObj = new Trk::SurfaceTreeObject(treeName, treeTitle);
        m_surfaceTrees.insert(std::make_pair(&lay,surfTreeObj));

        // now register the Tree
        ITHistSvc* tHistSvc = 0;
        if (service("THistSvc",tHistSvc).isFailure()) {
            ATH_MSG_INFO( "initialize() Could not find Hist Service  -> Switching Tree output for this surface off !" );
            delete surfTreeObj; surfTreeObj = 0;
        }
        else if (tHistSvc && (tHistSvc->regTree(treeRegName.Data(), (*surfTreeObj).tree)).isFailure()) {
            ATH_MSG_INFO( "initialize() Could not register the validation Tree -> Switching Tree output for this surface off !" );
            delete surfTreeObj; surfTreeObj = 0;
        }

    } else  // a tree is found
        surfTreeObj = findIter->second;
    return surfTreeObj;
}


void Trk::MaterialMapper::handle( const Incident& inc ) {
  // check the incident type
  if ( inc.type() == IncidentType::EndEvent && m_materialSteps){
    // check if the hit collection already contains:
      ATH_MSG_VERBOSE("EndEvent incident caught, finalize histogramns");
      // average Eta / Phi 
      m_averageEta /= m_materialSteps;
      m_averagePhi /= m_materialSteps;
      // (a) the volume validation
      finalizeVolumeHits(true);    // trees with mapped hits
      finalizeVolumeHits(false);   // trees with unmapped hits
      // (b) the layer validation
      finalizeLayerHits();
      // (c) the surface validation (reference material)
      finalizeSurfaceHits();
      // fill the main TTree & reset
      m_validationTree->Fill();
      // initialize the eta/phi
      m_averageEta          = 0.;
      m_averagePhi          = 0.;
      // initialize the rest
      m_materialSteps       = 0; 
      m_mappedPath          = 0.;
      m_mappedPathInX0      = 0.;
      m_mappedPathInL0      = 0.;
      m_mappedPathRho       = 0.;
      m_mappedPathZARho     = 0.; 
      // increate counter
      ++m_processedEvents;       
  }
  return;
}


void Trk::MaterialMapper::bookValidationTree() 
{

    ATH_MSG_INFO( "Booking the Validation Tree ... " );

    // now register the Tree
    ITHistSvc* tHistSvc = 0;

    // (1) Main MaterialMapper TTree
    // ------------- validation section ------------------------------------------
    m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());
    
    // position coordinates of the update
    m_validationTree->Branch("Eta",                    &m_averageEta,            "averageEta/F");
    m_validationTree->Branch("Phi",                    &m_averagePhi,            "averagePhiF");
    m_validationTree->Branch("Path",                   &m_mappedPath,            "path/F");
    m_validationTree->Branch("PathInX0",               &m_mappedPathInX0,        "pathInX0/F");
    m_validationTree->Branch("PathInL0",               &m_mappedPathInL0,        "pathInL0/F");
    m_validationTree->Branch("PathRho",                &m_mappedPathRho,         "pathRho/F");
    m_validationTree->Branch("PathZARho",              &m_mappedPathZARho,       "pathZARho/F");
    m_validationTree->Branch("UnmappedPathInX0",       &m_unmappedPathInX0,      "unmappedPathInX0/F");
    m_validationTree->Branch("MaterialSteps",          &m_materialSteps,         "steps/I");
    m_validationTree->Branch("Mapped",                 m_mapped,                 "mapped[steps]/I");
    m_validationTree->Branch("MaterialAccumPathInX0",  m_materialAccumPathInX0,  "materialAccumPinX0[steps]/F");
    m_validationTree->Branch("MaterialAccumPathZARho", m_materialAccumPathZARho, "materialAccumPZARho[steps]/F");
    m_validationTree->Branch("MaterialStepPath",       m_materialStepPath,       "materialStepPath[steps]/F");
    m_validationTree->Branch("MaterialStepX0",         m_materialStepX0,         "materialStepX0[steps]/F");
    m_validationTree->Branch("MaterialStepL0",         m_materialStepL0,         "materialStepL0[steps]/F");
    m_validationTree->Branch("MaterialStepZ",          m_materialStepZ,          "materialStepZ[steps]/F");
    m_validationTree->Branch("MaterialStepA",          m_materialStepA,          "materialStepA[steps]/F");
    m_validationTree->Branch("MaterialStepRho",        m_materialStepRho,        "materialStepRho[steps]/F");
    m_validationTree->Branch("MaterialStepPositionX",  m_materialStepPositionX , "materialStepX[steps]/F");
    m_validationTree->Branch("MaterialStepPositionY",  m_materialStepPositionY , "materialStepY[steps]/F");
    m_validationTree->Branch("MaterialStepPositionZ",  m_materialStepPositionZ , "materialStepZ[steps]/F");
    m_validationTree->Branch("MaterialStepPositionR",  m_materialStepPositionR , "materialStepR[steps]/F");
    m_validationTree->Branch("MaterialProjPositionX",  m_materialProjPositionX , "materialProjX[steps]/F");
    m_validationTree->Branch("MaterialProjPositionY",  m_materialProjPositionY , "materialProjY[steps]/F");
    m_validationTree->Branch("MaterialProjPositionZ",  m_materialProjPositionZ , "materialProjZ[steps]/F");
    m_validationTree->Branch("MaterialProjPositionR",  m_materialProjPositionR , "materialProjR[steps]/F");
    m_validationTree->Branch("MaterialProjDistance",   m_materialProjDistance  , "materialProjD[steps]/F");
    
    // now register the Tree
    if (service("THistSvc",tHistSvc).isFailure()) {
        ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !" );
        delete m_validationTree; m_validationTree = 0;
        return;
    }
    if ((tHistSvc->regTree(m_validationTreeFolder.c_str(), m_validationTree)).isFailure()) {
        ATH_MSG_ERROR("initialize() Could not register the validation Tree -> Switching ValidationMode Off !" );
        delete m_validationTree; m_validationTree = 0;
        return;
    }
    
    ATH_MSG_INFO( "                            ... successful." );

} 


