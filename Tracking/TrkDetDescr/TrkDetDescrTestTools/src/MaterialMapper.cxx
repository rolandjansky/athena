/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
// Gaudi
#include "GaudiKernel/ITHistSvc.h" 


// constructor
Trk::MaterialMapper::MaterialMapper(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  m_materialAssociationType(1),
  m_recordDone(false),
  m_maxMappingEvents(100000),
  m_processedEvents(0),
  m_totalMaterialTree("TotalMaterial"),
  m_validationMode(false),
  m_validationTree(0),
  m_validationTreeName("MaterialMapper"),
  m_validationTreeDescription("Material Effects Updator information"),
  m_validationTreeFolder("/val/MaterialMapper"),
  m_materialSteps(0),
  m_navigationValidation(false),
  m_navigationSteps(0),
  m_volumeValidation(true),
  m_volumeTreePrefix("VolumeMaterial"),
  m_layerValidation(true),
  m_layerTreePrefix("LayerMaterial_"),
  m_surfaceValidation(true),
  m_surfaceTreePrefix("SurfaceMaterial_"),
  m_etaTolerance(0.05),
  m_phiTolerance(0.05),
  m_externalEtaPhi(false)
{
    declareInterface<IMaterialMapper>(this);

    // The maximum mapping events to control the output file size
    declareProperty("MaximumMappingEvents",       m_maxMappingEvents);
    // specification for the Validation Tree
    declareProperty("ValidationMode",             m_validationMode);
    declareProperty("ValidationTreeName",         m_validationTreeName);
    declareProperty("ValidationTreeDescription",  m_validationTreeDescription);
    declareProperty("ValidationTreeFolder",       m_validationTreeFolder);
    declareProperty("NavigationValidationMode",   m_navigationValidation);
    // the total material statistics              
    declareProperty("TotalMaterialTreeName",      m_totalMaterialTree);
    // Tolerance parameters & averaging           
    declareProperty("ToleranceEta",               m_etaTolerance);
    declareProperty("TolerancePhi",               m_phiTolerance);
    declareProperty("ExternalEtaPhi",             m_externalEtaPhi);
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
        // finalize the Hit Tree
        auto layTreeIter  = m_layerTrees.begin();
        auto endLayTreeIter = m_layerTrees.end();
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

    // book the Tree 
    if (m_validationMode) bookValidationTree();
    return StatusCode::SUCCESS;
}     

// finalize
StatusCode Trk::MaterialMapper::finalize()
{
    ATH_MSG_INFO(  "finalize() successful" );
    return StatusCode::SUCCESS;
}

void Trk::MaterialMapper::record(const Amg::Vector3D& position, int) const 
{
    
    if (m_processedEvents >= m_maxMappingEvents) return;
    
    // record the navigation
    m_navigationStepPositionX[m_navigationSteps] = position.x();
    m_navigationStepPositionY[m_navigationSteps] = position.y();
    m_navigationStepPositionZ[m_navigationSteps] = position.z();
    m_navigationStepPositionR[m_navigationSteps] = position.perp();
    ++m_navigationSteps;
}

void Trk::MaterialMapper::recordMaterialHit(const AssociatedMaterial& amhit, const Amg::Vector3D& projectedPosition) const
{

    if (m_processedEvents >= m_maxMappingEvents) return;

    // ----------------------------------- The Material Effects Validation code --------------------------
    const Amg::Vector3D& position = amhit.materialPosition();
    
    m_eta += position.eta();
    m_phi += position.phi();
    m_recordDone = true;

    double measureAZ  = 0.;
    
    double pathLength = amhit.steplength();
    double pathInX0   = amhit.steplengthInX0();
    double pathInL0   = amhit.steplengthInL0();
    double rho = amhit.rho();
    double A = amhit.A();
    double Z = amhit.Z();

    // s/X0 and rho are obvious
    double stepRho    = amhit.steplength() * amhit.rho();

    // get the tracking volume
    const Trk::TrackingVolume* tvol = amhit.associatedTrackingVolume();
    const Trk::Layer*           lay = amhit.associatedLayer();
    
    if (!tvol) {
        ATH_MSG_WARNING("Could not associate TrackingVolume to material step!");
        return;
    }
    
    // ----------------------------------- The Material Validation code --------------------------

    // make it dependent on the AssociationType
    if ((Trk::MaterialAssociationType)m_materialAssociationType == Trk::EffectiveNumSteps )
        measureAZ      = 1.;
    else if ((Trk::MaterialAssociationType)m_materialAssociationType == Trk::EffectiveNumAtoms )
        measureAZ       = rho/A*pathLength;
    else if ((Trk::MaterialAssociationType)m_materialAssociationType == Trk::RadiationLength ) 
        measureAZ       = pathInX0;
    else
        measureAZ       = pathLength;

    const Trk::TrackingVolume* itvol = tvol;
    while (itvol) {
        // get the volume tree object
        ATH_MSG_VERBOSE( "getting volTreeObj for:   " << itvol->volumeName() << "   at:   " << itvol );
        Trk::VolumeTreeObject* volTreeObj   = volumeTreeObject(lay,itvol);
        if (volTreeObj) {
            // add the quantities
            (*volTreeObj).path     += pathLength;
            (*volTreeObj).pathInX0 += pathInX0;
            (*volTreeObj).pathInL0 += pathInL0;
            (*volTreeObj).A        += measureAZ * A;
            (*volTreeObj).Z        += measureAZ * Z;
            (*volTreeObj).rho      += stepRho;
            (*volTreeObj).measure  += measureAZ;
        } else
            ATH_MSG_WARNING( "Cannot find/create VolumeTreeObject for volume '" << itvol->volumeName() << "'." );
        itvol = itvol->getMotherVolume();
        if (itvol) ATH_MSG_VERBOSE( "motherVolume found:   " << itvol->volumeName() );
    }

    // ------------- validation section ------------------------------------------
    if (m_validationMode) {
        // fill the variables for spatial information & event characteristics
        if (lay)
            m_mappedPathInX0 +=  pathInX0;
        else m_unmappedPathInX0 +=  pathInX0;
        m_mapped[m_materialSteps] = lay ? lay->layerIndex().value() : 0;
        // the position
        m_materialStepPathInX0[m_materialSteps]    = pathInX0;
        m_materialStepPositionX[m_materialSteps]   = position.x();
        m_materialStepPositionY[m_materialSteps]   = position.y();
        m_materialStepPositionZ[m_materialSteps]   = position.z();
        m_materialStepPositionR[m_materialSteps]   = position.perp();
        // record the projected position as well
        m_materialProjPositionX[m_materialSteps]   = projectedPosition.x();
        m_materialProjPositionY[m_materialSteps]   = projectedPosition.y();
        m_materialProjPositionZ[m_materialSteps]   = projectedPosition.z();
        m_materialProjPositionR[m_materialSteps]   = projectedPosition.perp();
        // and increase the number of steps
        ++m_materialSteps;
    }

    // record the layer hit
    if (m_layerValidation)
        recordLayerHit(amhit);
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

        if ((*volTreeObj).path > 0. && (*volTreeObj).measure > 0.) {
            (*volTreeObj).A           /= (*volTreeObj).measure;
            (*volTreeObj).Z           /= (*volTreeObj).measure;
            (*volTreeObj).rho         /= (*volTreeObj).path;
            // spatial mapping
            (*volTreeObj).eta         = m_eta;
            (*volTreeObj).phi         = m_phi;
            // fill the volume Tree Object
            (*volTreeObj).tree->Fill();          
        }
        // reset
        (*volTreeObj).eta         = 0.;
        (*volTreeObj).phi         = 0.;
        (*volTreeObj).path        = 0.;
        (*volTreeObj).pathInX0    = 0.;
        (*volTreeObj).pathInL0    = 0.;
        (*volTreeObj).A           = 0.;
        (*volTreeObj).Z           = 0.;
        (*volTreeObj).rho         = 0.;
        (*volTreeObj).measure     = 0.;

    }
}


void Trk::MaterialMapper::recordLayerHit(const AssociatedMaterial& amhit) const
{
    
    if (m_processedEvents >= m_maxMappingEvents) return;
    
    const Trk::Layer* lay = amhit.associatedLayer();
    if (m_layerValidation && lay) {
        // try to find the histogram
        Trk::LayerTreeObject* layTreeObj   = layerTreeObject(*lay);
        if (layTreeObj
                && (*layTreeObj).layerHits < TRKDETDESCRTOOLS_MAXLAYERHITS
                && amhit.steplengthInX0() > 0.)
        {

            const Amg::Vector3D&  pos = amhit.materialPosition();

            // hit positions
            double posX = pos.x();
            double posY = pos.y();
            double posZ = pos.z();
            double posR = pos.perp();
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
            (*layTreeObj).correctionFactor   = amhit.correctionFactor();
            (*layTreeObj).pathInX0           = amhit.steplengthInX0();
            (*layTreeObj).pathInL0           = amhit.steplengthInL0();
            (*layTreeObj).rho                = amhit.rho();

        }
    }
}


void Trk::MaterialMapper::recordFullLayerHit(const AssociatedMaterial& amhit) const
{
    if (m_processedEvents >= m_maxMappingEvents) return;

    const Trk::Layer* lay = amhit.associatedLayer();
    if (m_layerValidation && lay){
        // try to find the histogram
        Trk::LayerTreeObject* layTreeObj   = layerTreeObject(*lay);
        if (layTreeObj
                && (*layTreeObj).layerHits < TRKDETDESCRTOOLS_MAXLAYERHITS
                && amhit.steplengthInX0() > 0.)
        {
            const Amg::Vector3D&  pos = amhit.materialPosition();
            // increase the densed hits
            (*layTreeObj).densedHits         = 1;

            (*layTreeObj).densedHitX         = pos.x();
            (*layTreeObj).densedHitY         = pos.y();
            (*layTreeObj).densedHitZ         = pos.z();
            (*layTreeObj).densedHitR         = pos.perp();
            (*layTreeObj).densedHitPhi       = pos.phi();
            (*layTreeObj).densedHitTheta     = pos.theta();

            (*layTreeObj).correctionFactor   = amhit.correctionFactor();
            (*layTreeObj).pathInX0           = amhit.steplengthInX0();
            (*layTreeObj).pathInL0           = amhit.steplengthInL0();
            (*layTreeObj).rho                = amhit.rho();

            // there haven't been any layer hits
            if (!(*layTreeObj).layerHits) {
                // fill it
                (*layTreeObj).hitPositionX[0] = pos.x();
                (*layTreeObj).hitPositionY[0] = pos.y();
                (*layTreeObj).hitPositionZ[0] = pos.z();
                (*layTreeObj).hitPositionR[0] = pos.perp();

                // increase the layer Hits
                (*layTreeObj).layerHits        = 1;

            }
        }
    }
}


void Trk::MaterialMapper::finalizeLayerHits() const
{

    ATH_MSG_VERBOSE( "finalizeEvent() on Layer hits " );

    // loop over all layer Tree objects and fill them
    auto layObjIter  = m_layerTrees.begin();
    auto endIter     = m_layerTrees.end();
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
                (*layTreeObj).rho                /= (*layTreeObj).pathInX0;
            }
            // fill the layer Tree Object
            (*layTreeObj).tree->Fill();          
        }
        // reset
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
        (*layTreeObj).rho               = 0.;
        (*layTreeObj).layerHits         = 0;
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
            (*surfTreeObj).pathInX0           += amhit.steplengthInX0();
            (*surfTreeObj).pathInL0           += amhit.steplengthInL0();
            (*surfTreeObj).rho                += amhit.rho();
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
                  treeName  += "/";
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


Trk::LayerTreeObject* Trk::MaterialMapper::layerTreeObject(const Trk::Layer& lay) const
{
    if (!lay.layerIndex().value()) return 0;

    // try to find the histogram
    auto endIter  = m_layerTrees.end();
    auto findIter = m_layerTrees.end();

    Trk::LayerTreeObject* layTreeObj = 0;

    findIter = m_layerTrees.find(&lay);
    if (findIter == endIter) {
        // get the TrackingVolume
        const Trk::TrackingVolume* enclosingVolume = lay.enclosingTrackingVolume();
        TString tvolName = (enclosingVolume) ?  enclosingVolume->volumeName() : "Unknown";
        tvolName.ReplaceAll("::","_");

        TString treeName   = tvolName;
                treeName  += "/";
                treeName  += m_layerTreePrefix;

        TString layerType = (lay.surfaceRepresentation().type() == Trk::Surface::Cylinder) ? 
                "CylinderLayer_" : "DiscLayer_";
                
                treeName  += layerType;
                treeName  += lay.layerIndex().value();

        TString treeTitle  = "TrackingVolume :";
                treeTitle += tvolName;

        TString treeRegName  = "/val/";
                treeRegName += treeName;

        ATH_MSG_INFO( "No Tree found for Layer " << lay.layerIndex().value()
                << " in Volume '" << tvolName << "'.");
        ATH_MSG_INFO( "   -> Booking it now with register name : " << treeRegName.Data() );

        layTreeObj = new Trk::LayerTreeObject(treeName, treeTitle);
        m_layerTrees.insert(std::make_pair(&lay,layTreeObj));

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


void Trk::MaterialMapper::finalizeEvent(double eta, double phi) const
{

    if (m_processedEvents >= m_maxMappingEvents) return;

    // avoid double finalize
    if (m_recordDone) {
        m_eta = eta; 
        m_phi = phi;
        ATH_MSG_VERBOSE( " -> finalizeEvent() call with (eta,phi) = " << m_eta << ", " << m_phi << " - and record parameter : " << m_recordDone);
        // (a) the volume validation
        finalizeVolumeHits(true);    // trees with mapped hits
        finalizeVolumeHits(false);   // trees with unmapped hits
        // (b) the layer validation
        finalizeLayerHits();
        // (c) the surface validation (reference material)
        finalizeSurfaceHits();
        // (d) the hit validation
        if (m_validationMode){
            m_averageEta = eta;
            m_averagePhi = phi;
            m_validationTree->Fill();
        }
        // the event counter
        ++m_processedEvents;
    }
    m_recordDone       = false;
    m_eta              = 0.;
    m_phi              = 0.;
    m_materialSteps    = 0 ;
    m_navigationSteps  = 0 ;
    m_mappedPathInX0   = 0.;
    m_unmappedPathInX0 = 0.;
    
}


void Trk::MaterialMapper::bookValidationTree() 
{

    ATH_MSG_INFO( "Booking the Validation Tree ... " );

    // now register the Tree
    ITHistSvc* tHistSvc = 0;

    // (1) Material Effects tree
    if (m_validationMode) {
        // ------------- validation section ------------------------------------------
        m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());

        // position coordinates of the update
        m_validationTree->Branch("AveragedEta",           &m_averageEta,           "averageEta/F");
        m_validationTree->Branch("AveragedPhi",           &m_averagePhi,           "averageEta/F");
        m_validationTree->Branch("MappedPathInX0",        &m_mappedPathInX0,       "mappedPathInX0/F");
        m_validationTree->Branch("UnMappedPathInX0",      &m_unmappedPathInX0,     "unmappedPathInX0/F");
        m_validationTree->Branch("MaterialSteps",         &m_materialSteps,        "steps/I");
        m_validationTree->Branch("Mapped",                m_mapped,                "mapped[steps]/I");
        m_validationTree->Branch("MaterialStepPathInX0",  m_materialStepPathInX0  ,"materialStepPinX0[steps]/F");
        m_validationTree->Branch("MaterialStepPositionX", m_materialStepPositionX ,"materialStepX[steps]/F");
        m_validationTree->Branch("MaterialStepPositionY", m_materialStepPositionY ,"materialStepY[steps]/F");
        m_validationTree->Branch("MaterialStepPositionZ", m_materialStepPositionZ ,"materialStepZ[steps]/F");
        
        if (m_navigationValidation){
            // position coordinates of the navigation steps : used for Tracking Validation
            m_validationTree->Branch("NavigationSteps",         &m_navigationSteps,"nsteps/I");
            m_validationTree->Branch("NavigationStepPositionX", m_navigationStepPositionX ,"navigationStepX[nsteps]/F");
            m_validationTree->Branch("NavigationStepPositionY", m_navigationStepPositionY ,"navigationStepY[nsteps]/F");
            m_validationTree->Branch("NavigationStepPositionZ", m_navigationStepPositionZ ,"navigationStepZ[nsteps]/F");
            m_validationTree->Branch("NavigationStepPositionR", m_navigationStepPositionR ,"navigationStepR[nsteps]/F");
        } else {
            // only used on G4 material steps - thus for Mapping Validation
            m_validationTree->Branch("MaterialStepPositionR", m_materialStepPositionR ,"materialStepR[steps]/F");
            m_validationTree->Branch("MaterialProjPositionX", m_materialProjPositionX ,"materialProjX[steps]/F");
            m_validationTree->Branch("MaterialProjPositionY", m_materialProjPositionY ,"materialProjY[steps]/F");
            m_validationTree->Branch("MaterialProjPositionZ", m_materialProjPositionZ ,"materialProjZ[steps]/F");
            m_validationTree->Branch("MaterialProjPositionR", m_materialProjPositionR ,"materialProjR[steps]/F");
        }
        
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

    }
    ATH_MSG_INFO( "                            ... successful." );

} 


