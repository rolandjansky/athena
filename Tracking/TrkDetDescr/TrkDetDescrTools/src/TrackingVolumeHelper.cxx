/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackingVolumeHelper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTools/TrackingVolumeHelper.h"
#include "TrkDetDescrInterfaces/ILayerArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/EntryLayerProvider.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/GlueVolumesDescriptor.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Gaudi
#include "GaudiKernel/SystemOfUnits.h"


double Trk::TrackingVolumeHelper::s_layerEnvelopeDistance = 2.*Gaudi::Units::mm;
double Trk::TrackingVolumeHelper::s_layerThickness        = 1.*Gaudi::Units::mm;

// constructor
Trk::TrackingVolumeHelper::TrackingVolumeHelper(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),
  TrackingVolumeManipulator(),
  m_layerArrayCreator("Trk::LayerArrayCreator/LayerArrayCreator"),
  m_trackingVolumeArrayCreator("Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator")
{
    declareInterface<ITrackingVolumeHelper>(this);
    // the tools
    declareProperty("LayerArrayCreator",                m_layerArrayCreator);
    declareProperty("TrackingVolumeArrayCreator",       m_trackingVolumeArrayCreator);
}

// destructor
Trk::TrackingVolumeHelper::~TrackingVolumeHelper()
{}


// the interface methods
StatusCode Trk::TrackingVolumeHelper::initialize()
{

    ATH_MSG_INFO( "initialize() " );    

    // Retrieve the layer array creator  ----------------------------------------------------
    if (m_layerArrayCreator.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerArrayCreator );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO( "Retrieved tool " << m_layerArrayCreator );

    // Retrieve the volume array creator  ----------------------------------------------------
    if (m_trackingVolumeArrayCreator.retrieve().isFailure()) {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeArrayCreator );
        return StatusCode::FAILURE;
    } else 
        ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeArrayCreator );

    return StatusCode::SUCCESS;
}    

StatusCode Trk::TrackingVolumeHelper::finalize()
{
    ATH_MSG_INFO( "finalize() successful" );
    return StatusCode::SUCCESS;
}


Trk::TrackingVolume* Trk::TrackingVolumeHelper::trackingVolumeCylinderLayers(
                                                  Amg::Transform3D* trans,
                                                  Trk::CylinderVolumeBounds* bounds,
                                                  Trk::Material& matprop,
                                                  std::vector<double> radii,
                                                  std::vector<double> envelopeCovers,
                                                  const std::string& volumeName,
                                                  int materialBinsRZ, int materialBinsPhi,
                                                  BinningType btype,
                                                  bool redoNavigation) const
{

    // for the output
    unsigned int layers       = radii.size();
    unsigned int createLayers = (layers) ? layers : 1;

    ATH_MSG_VERBOSE( "TrackingVolume with " << createLayers << " cylindrical layer(s) to be built ..." );
    if (layers)
        ATH_MSG_VERBOSE( "Layer radii provided from outside source." );

    Trk::TrackingVolume* cylLayerVolume = 0;
    // bounds must exist   
    if (bounds) {
        // create the layer vector 
        std::vector<const Trk::CylinderLayer*> layerVector;

        for (unsigned int iLay = 0; iLay<createLayers; ++iLay) {
            // the current Layer
            double layerRadius = layers ? radii[iLay] : bounds->mediumRadius();
            ATH_MSG_VERBOSE( " -> CylinderLayer radius determined     : " << layerRadius );
            double layerHalfZ  = bounds->halflengthZ();
            layerHalfZ -= (createLayers<=envelopeCovers.size()) ? fabs(envelopeCovers[iLay]) : s_layerEnvelopeDistance;
            ATH_MSG_VERBOSE( " -> CylinderLayer halflength determined : " << layerHalfZ );
            // create the cylinder layer bounds
            Trk::CylinderBounds* layerBounds = new Trk::CylinderBounds(layerRadius, layerHalfZ);
            // create the Transform in 3D space if needed
            Amg::Transform3D* layerTransform = trans ? new Amg::Transform3D(*trans) : 0;
            // the z position of the cylinder
            double cylinderCenterZ = layerTransform->translation().z();
            // the layer material --- 1-dim / 2-dim
            Trk::BinnedLayerMaterial* layerMaterial = 0;
            if (materialBinsPhi==1) {
                // the BinUtility for the material
                Trk::BinUtility layerBinUtility1DZ(materialBinsRZ,
                                                          cylinderCenterZ-layerHalfZ,
                                                          cylinderCenterZ+layerHalfZ,
                                                          Trk::open,
                                                          Trk::binZ);
                // ---------------------> create the layer material
                layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtility1DZ);
                ATH_MSG_VERBOSE( " -> Preparing the binned material with " << materialBinsRZ << " bins in Z. ");

            } else { // break the phi symmetry
                Trk::BinUtility layerBinUtilityRphiZ(materialBinsPhi,
                                                            -layerRadius*M_PI,
                                                            layerRadius*M_PI,
                                                            Trk::closed,
                                                            Trk::binRPhi);
                layerBinUtilityRphiZ +=   Trk::BinUtility(materialBinsRZ,
                                                                -layerHalfZ,
                                                                layerHalfZ,
                                                                Trk::open,
                                                                Trk::binZ);
                // ---------------------> create the layer material
                layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityRphiZ);

                ATH_MSG_VERBOSE( " -> Preparing the binned material with "
                        << materialBinsPhi << " / " <<  materialBinsRZ << " bins in r*phi / Z. ");

            }

            // create the layer - cylinder layer w/o transform 
            Trk::CylinderLayer* layer = (layerTransform) ? new Trk::CylinderLayer(layerTransform,
                                                                                  layerBounds,
                                                                                  *layerMaterial,
                                                                                  s_layerThickness,
                                                                                  0,0) :
                                                           new Trk::CylinderLayer(layerBounds,
                                                                                  *layerMaterial,
                                                                                  s_layerThickness,
                                                                                  0,0);                                                                                  
                                                                                  
            // memory cleanup
            delete layerMaterial; layerMaterial = 0;
            // push back into the layer vector for Array creation
            layerVector.push_back(layer);
        }

        // create the Layer Array
        Trk::BinnedArray<Trk::Layer>* layerArray = m_layerArrayCreator ?
                m_layerArrayCreator->cylinderLayerArray(layerVector,
                                                        bounds->innerRadius(),
                                                        bounds->outerRadius(),
                                                        btype) : 0;
        cylLayerVolume = new Trk::TrackingVolume(trans, bounds,matprop,layerArray,0,volumeName);
        // register the re-navigation trigger
        if (redoNavigation) cylLayerVolume->forceNavigationCheck();

    } else
        ATH_MSG_WARNING( "No VolumeBounds were given, TrackingVolume creation fails! " );

    return cylLayerVolume;
}


Trk::TrackingVolume* Trk::TrackingVolumeHelper::trackingVolumeDiscLayers(
                                                  Amg::Transform3D* trans,
                                                  Trk::CylinderVolumeBounds* bounds,
                                                  Trk::Material& matprop,
                                                  std::vector<double> zpos,
                                                  std::vector<double> innerCovers,
                                                  std::vector<double> outerCovers,
                                                  const std::string& volumeName,
                                                  int materialBinsRZ, int materialBinsPhi,
                                                  BinningType btype,
                                                  bool redoNavigation) const
{
    // -------------------------------------
    ATH_MSG_VERBOSE( "TrackingVolume with disc-like layer(s) to be built ..." );

    Trk::TrackingVolume* discLayerVolume = 0;
    // bounds must exist   
    if (bounds){ 

        // helper transform
        Amg::Transform3D helperTransform;
        if ( trans )
            helperTransform = (*trans) ;
        else {
            helperTransform.setIdentity();
        }
        // get the center from it
        Amg::Vector3D centerPosition = helperTransform.translation();

        unsigned int layers = zpos.size();
        unsigned int createLayers = (layers) ? layers : 1;

        double discZpos   = centerPosition.z();
        double volumeZpos = centerPosition.z();

        std::vector<const Trk::DiscLayer*> layerVector;

        for (unsigned int iLay = 0; iLay<createLayers; ++iLay) {
            // the current Layer
            discZpos        = layers ? zpos[iLay] : discZpos;
            double discInnerRadius =  bounds->innerRadius();
            double discOuterRadius =  bounds->outerRadius();
            discInnerRadius += (createLayers<=innerCovers.size()) ? fabs(innerCovers[iLay]) : s_layerEnvelopeDistance;
            discOuterRadius -= (createLayers<=outerCovers.size()) ? fabs(outerCovers[iLay]) : s_layerEnvelopeDistance;

            ATH_MSG_VERBOSE( " -> DiscLayer at z-position determined : " << discZpos );

            // create the disc layer bounds
            Trk::DiscBounds* layerBounds = new Trk::DiscBounds(discInnerRadius, discOuterRadius);
            // create the HepTransform
            Amg::Transform3D* layerTransform = new Amg::Transform3D;
            (*layerTransform) = Amg::Translation3D(0.,0.,discZpos);

            Trk::BinnedLayerMaterial* layerMaterial = 0;
            // bin in Z in any case
            Trk::BinUtility layerBinUtility(materialBinsRZ,
                                                   discInnerRadius,
                                                   discOuterRadius,
                                                   Trk::open,
                                                   Trk::binR );
            // the BinUtility for the material
            if (materialBinsPhi==1) { // 1-dim material
                ATH_MSG_VERBOSE( " -> Preparing the binned material with "
                        << materialBinsRZ << " bins in R. ");
                // ---------------------> create the layer material
                layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtility);

            } else {
                // add the binning in Phi
                layerBinUtility += Trk::BinUtility(materialBinsPhi,
                                                          -M_PI, M_PI,
                                                          Trk::closed,
                                                          Trk::binPhi);
             // ---------------------> create the layer material          
             layerMaterial =new Trk::BinnedLayerMaterial(layerBinUtility);
    
             ATH_MSG_VERBOSE( " -> Preparing the binned material with "
                 << materialBinsPhi << " / " <<  materialBinsRZ << " bins in phi / R. "); 
      
           }
           // create the layer
           Trk::DiscLayer* layer = new Trk::DiscLayer(layerTransform,
                                                      layerBounds,
                                                      *layerMaterial,
                                                      s_layerThickness,
                                                      0,0);
           // push back into the layer vector for Array creation
           layerVector.push_back(layer);
           // memory cleanup
           delete layerMaterial; layerMaterial = 0;

        }
        // create the Layer Array
        Trk::BinnedArray<Trk::Layer>* layerArray = m_layerArrayCreator ?
                m_layerArrayCreator->discLayerArray(layerVector,
                                                    volumeZpos-(bounds->halflengthZ()),
                                                    volumeZpos+(bounds->halflengthZ()),
                                                    btype) : 0;
        // finally create the Volume
        discLayerVolume = new Trk::TrackingVolume(trans, bounds,matprop,layerArray,0,volumeName);
        if (redoNavigation) discLayerVolume->forceNavigationCheck();

    } else {
        ATH_MSG_WARNING( "No VolumeBounds were given, TrackingVolume creation fails! " );
    }
    return discLayerVolume;
}


Trk::TrackingVolume* Trk::TrackingVolumeHelper::createCylindricalGapVolume(
                                                         double rMin, 
                                                         double rMax, 
                                                         double zMin, 
                                                         double zMax,
                                                         Material& matprop,
                                                         const std::string& volumeName,
                                                         bool cylinderLayer,
                                                         double coverOne,
                                                         double coverTwo,
                                                         int materialBinsRZ, int materialBinsPhi,
                                                         bool redoNavigation) const 
{ 

    ATH_MSG_VERBOSE( "createCylindricalGapVolume() for volume " << volumeName << " with rMin/rMax/zMin/zMax :" );
    ATH_MSG_VERBOSE( "                   " << rMin << " / " << rMax << " / " << zMin << " / " << zMax );
    // helpers
    std::vector<double> helperEnvelopOne(1,coverOne);

    // create the volume for the TrackingVolume, if needed, volumes can exist w/o transform when centered at 0,0,0
    double zPos = 0.5*(zMin+zMax);
    Amg::Transform3D* volumeTransform = ( fabs(zPos) > 10e-3 )  ? new Amg::Transform3D : 0;
    if ( volumeTransform ) 
        (*volumeTransform) = Amg::Translation3D(0.,0.,zPos);
    // create the volume bounds            
    Trk::CylinderVolumeBounds* volumeBounds = new Trk::CylinderVolumeBounds(rMin, rMax,
            0.5*fabs(zMin-zMax));
    // a dummy material provider
    Trk::TrackingVolume* tvol = 0;
    if (!cylinderLayer) {
        std::vector<double> helperLayer(1, zPos);
        std::vector<double> helperEnvelopTwo(1,coverTwo);
        tvol = trackingVolumeDiscLayers(volumeTransform,
                                        volumeBounds,
                                        matprop,
                                        helperLayer,
                                        helperEnvelopOne,
                                        helperEnvelopTwo,
                                        volumeName,
                                        materialBinsRZ, materialBinsPhi,
                                        arbitrary);
    } else {
        std::vector<double> helperLayer(1,volumeBounds->mediumRadius());
        tvol = trackingVolumeCylinderLayers(volumeTransform,
                                            volumeBounds,
                                            matprop,
                                            helperLayer,
                                            helperEnvelopOne,
                                            volumeName,
                                            materialBinsRZ, materialBinsPhi,
                                            arbitrary);
    }
    if (redoNavigation) tvol->forceNavigationCheck();
    // return the tracking volume
    return tvol;
}


Trk::TrackingVolume* Trk::TrackingVolumeHelper::createCylindricalTopLevelVolume(
                                                         const std::vector<const TrackingVolume*>& volumes,
                                                         Material& matprop,
                                                         const std::string& volumeName,
                                                         bool glue) const
{
    ATH_MSG_VERBOSE( "Creating SuperVolume '" << volumeName << "' with " << volumes.size() << " sub volumes:" );

    // set the iterator to the volumes
    std::vector<const Trk::TrackingVolume*>::const_iterator firstVolume = volumes.begin(); // set to the first volume
    std::vector<const Trk::TrackingVolume*>::const_iterator lastVolume = volumes.end();
    for ( ; firstVolume != lastVolume; ++firstVolume)
        ATH_MSG_VERBOSE( "       with volume : " << (*firstVolume)->volumeName() );
    // reset the iterator
    firstVolume = volumes.begin();
    --lastVolume; // set to the last volume

    if (firstVolume == lastVolume) {
        ATH_MSG_WARNING( "Only one TrackingVolume given to create Top level volume (min required: 2) - returning 0 " );
        return 0;
    }
    // get the bounds
    const Trk::CylinderVolumeBounds* firstVolumeBounds =
            dynamic_cast<const Trk::CylinderVolumeBounds*>(&((*firstVolume)->volumeBounds()));
    const Trk::CylinderVolumeBounds* lastVolumeBounds =
            dynamic_cast<const Trk::CylinderVolumeBounds*>(&((*lastVolume)->volumeBounds()));
    // check the dynamic cast
    if (!firstVolumeBounds || !lastVolumeBounds){
        ATH_MSG_WARNING( "The VolumeBounds given turn out to be not of type: Trk::CylinderVolumeBounds (required) - returning 0 " );
        return 0;
    }
    // check whether it is a r-binned case or a z-binned case
    bool rCase = fabs(firstVolumeBounds->innerRadius()-lastVolumeBounds->innerRadius()) > 0.1;

    // fill these ones depending on the rCase
    double zMin      = 0.;
    double zMax      = 0.;
    double rMin      = 0.;
    double rMax      = 0.;

    if (rCase) {
        zMin = (*firstVolume)->center().z() - firstVolumeBounds->halflengthZ();
        zMax = (*firstVolume)->center().z() + firstVolumeBounds->halflengthZ();
        rMin = firstVolumeBounds->innerRadius();
        rMax = lastVolumeBounds->outerRadius();
    } else {
        zMin = (*firstVolume)->center().z() - firstVolumeBounds->halflengthZ();
        zMax = (*lastVolume)->center().z() + firstVolumeBounds->halflengthZ();
        rMin = firstVolumeBounds->innerRadius();
        rMax = firstVolumeBounds->outerRadius();
    }

    double zPos = 0.5*(zMin+zMax);
    // create the HEP transform from the stuff known so far
    Amg::Transform3D* topVolumeTransform = new Amg::Transform3D;
    if (fabs(zPos) > 0.1 )
         (*topVolumeTransform) = Amg::Translation3D(0.,0.,zPos);
    else 
        topVolumeTransform->setIdentity();     
    // create the bounds from the information gathered so far
    Trk::CylinderVolumeBounds* topVolumeBounds = new Trk::CylinderVolumeBounds(rMin,rMax,0.5*fabs(zMax-zMin));
    // create the volume array to fill in
    Trk::BinnedArray<Trk::TrackingVolume>* topVolumeSubVolumeArray = (rCase) ? 
            m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(volumes) :
            m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(volumes);
    // now create the TopVolume
    Trk::TrackingVolume* topVolume = new Trk::TrackingVolume(topVolumeTransform,
                                                             topVolumeBounds,
                                                             matprop,
                                                             0, topVolumeSubVolumeArray,
                                                             volumeName);

    // glue either rCase or z-Case
    if (rCase && glue) {
        ATH_MSG_VERBOSE( "       -> Sub volumes are ordered by radii - start glueing" );
        // the -- did the job already that the last one is not glued
        for ( ; firstVolume != lastVolume;  ++firstVolume) {
            Trk::TrackingVolumeManipulator::glueVolumes(**firstVolume, Trk::tubeOuterCover,
                                                        **(firstVolume+1), Trk::tubeInnerCover);
              ATH_MSG_VERBOSE( "             Glue '"<< (*firstVolume)->volumeName()
                  << "' (tubeOuterCover) to '" << (*(firstVolume+1))->volumeName() << "' (tubeInnerCover). " );
            }
    } else if (glue) {
        ATH_MSG_VERBOSE( "       -> Sub volumes are ordered by z values - start glueing" );
        // the -- did the job already that the last one is not glued
        for ( ; firstVolume != lastVolume;  ++firstVolume) {
            Trk::TrackingVolumeManipulator::glueVolumes(**firstVolume, Trk::positiveFaceXY,
                                                        **(firstVolume+1), Trk::negativeFaceXY);
            ATH_MSG_VERBOSE( "             Glue '"<< (*firstVolume)->volumeName()
                    << "' (positiveFaceXY) to '" << (*(firstVolume+1))->volumeName() << "' (negativeFaceXY). " );
        }
    }
    return topVolume;
}


// -------------------------------------- display output -----------------------------------------------------------
const Trk::TrackingVolume* Trk::TrackingVolumeHelper::resizeCylindricalTrackingVolume(
                                                               const TrackingVolume* tvol,
                                                               double rMin, double rMax,
                                                               double zMin, double zMax) const
{
    // check if bounds for re-shaping exist
    if (tvol) {
        const Trk::VolumeBounds& origBounds = tvol->volumeBounds();
        Trk::CylinderVolumeBounds* newBounds = new Trk::CylinderVolumeBounds(rMin,rMax,0.5*fabs(zMax-zMin));

        ATH_MSG_VERBOSE( "TrackingVolume '" << tvol->volumeName() << "' changes from bounds " );
        ATH_MSG_VERBOSE( "<----- old : " << origBounds );
        ATH_MSG_VERBOSE( "-----> new : " << *newBounds );

        // prepare the new Parameters
        Amg::Transform3D* htrans = 0;
        if (!Trk::s_idTransform.isApprox(tvol->transform())) {
            htrans    = new Amg::Transform3D;
            (*htrans) = Amg::Translation3D(0.,0.,0.5*(zMin+zMax));
        }
        // check out the layers
        const Trk::LayerArray* confinedLayers = tvol->checkoutConfinedLayers();

        // construct the volume again
        TrackingVolume* newTvol = new TrackingVolume(htrans,
                                                    newBounds,
                                                    *tvol,
                                                    confinedLayers,
                                                    0,
                                                    tvol->volumeName());
        // check out the entry layers
        const Trk::EntryLayerProvider* entryLayerProvider = tvol->checkoutEntryLayerProvider();
        if (entryLayerProvider)
            newTvol->registerEntryLayerProvider( entryLayerProvider );
        // checkout the AbstractVolume for the sensitive part
        const Trk::AbstractVolume* sensitiveVolume = tvol->checkoutSensitiveVolume();
        if (sensitiveVolume)
            newTvol->registerSensitiveVolume( sensitiveVolume );
        // translate the color code
        newTvol->registerColorCode( tvol->colorCode() );
        // delete the old one
        delete tvol;
        // return the new one
        return newTvol;
    } else {
        ATH_MSG_WARNING( "No VolumeBounds were given, TrackingVolume is left untouched! " );
    }
    return tvol;
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::glueTrackingVolumes(const Trk::TrackingVolume& firstVol,
                                                    Trk::BoundarySurfaceFace firstFace,
                                                    const Trk::TrackingVolume& secondVol,
                                                    Trk::BoundarySurfaceFace secondFace) const
{
    Trk::TrackingVolumeManipulator::glueVolumes( firstVol, firstFace, secondVol, secondFace );
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::glueTrackingVolumes(const Trk::TrackingVolume& firstVol,
                                                    Trk::BoundarySurfaceFace firstFace,
                                                    const std::vector<const Trk::TrackingVolume*>& secondVolumes,
                                                    Trk::BoundarySurfaceFace secondFace) const
{

    std::vector<const Trk::TrackingVolume*>::const_iterator volIter    = secondVolumes.begin();
    std::vector<const Trk::TrackingVolume*>::const_iterator volIterEnd = secondVolumes.end();

    if (msgLvl(MSG::VERBOSE)) {
        ATH_MSG_VERBOSE( "Glue Volume '" << firstVol.volumeName() << "' to " << secondVolumes.size() << " volume(s): " );
        for ( ; volIter != volIterEnd; ++volIter) {
            ATH_MSG_VERBOSE( "              -> " << (*volIter)->volumeName() );
        }
        // reset the iterator
        volIter = secondVolumes.begin();
    }
    // if only one volume was given in the vector call the standard one-to-one glueing
    if (secondVolumes.size() == 1) {
        // self call
        glueTrackingVolumes(firstVol, firstFace, *(secondVolumes[0]), secondFace);
    } else {

        Trk::BinnedArray<Trk::TrackingVolume>* navArray = 0;
        // create the Array
        if (firstFace == Trk::negativeFaceXY || firstFace == Trk::positiveFaceXY )
            navArray = m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(secondVolumes, true);
        else
            navArray = m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(secondVolumes, true);

        // set the array as the outside array of the firstVol
        if (firstFace != Trk::tubeInnerCover)
            setOutsideTrackingVolumeArray( firstVol, firstFace, navArray );
        else
            setInsideTrackingVolumeArray( firstVol, firstFace, navArray );

        for ( ; volIter != volIterEnd; ++volIter ) {

            // the secondGlueFace
            Trk::BoundarySurfaceFace secondGlueFace = secondFace;

            if (secondFace == Trk::tubeOuterCover) {
                // protection : there may be a cylinder within the tube vector
                const Trk::CylinderVolumeBounds* currentVolBounds = dynamic_cast<const Trk::CylinderVolumeBounds*>(&((*volIter)->volumeBounds()));
                if (currentVolBounds && currentVolBounds->innerRadius() < 10e-3)
                    secondGlueFace = Trk::cylinderCover;
                setOutsideTrackingVolume(**volIter, secondGlueFace, (&(firstVol)));
            } // for all surfaces except the tunbeInnerCover outside of the surface is identical to outside of the volume
            else if (secondGlueFace != Trk::tubeInnerCover)
                setOutsideTrackingVolume(**volIter, secondGlueFace, (&(firstVol)));
            else
                setInsideTrackingVolume(**volIter, secondGlueFace, (&(firstVol)));

        }
    }
}


Trk::TrackingVolume* Trk::TrackingVolumeHelper::glueTrackingVolumeArrays(
                                                    const Trk::TrackingVolume& firstVol,
                                                    Trk::BoundarySurfaceFace firstFace,
                                                    const Trk::TrackingVolume& secondVol,
                                                    Trk::BoundarySurfaceFace secondFace, std::string name) const
{ 
    Trk::TrackingVolume* enclosingVolume = 0;

    const Trk::CylinderVolumeBounds* cyl1 = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(firstVol.volumeBounds()));
    const Trk::CylinderVolumeBounds* cyl2 = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(secondVol.volumeBounds()));

    if (!cyl1 || !cyl2) {
        ATH_MSG_ERROR( "TrackingVolumeHelper::glueTrackingVolumeArrays: input volumes not cylinders, return 0" );
        return enclosingVolume;
    }
    if (cyl1->halfPhiSector()!= M_PI || cyl2->halfPhiSector()!= M_PI ) {
        ATH_MSG_ERROR( "TrackingVolumeHelper::glueTrackingVolumeArrays: not coded for cylinder Phi sectors yet, return 0" );
        return enclosingVolume;
    }

    // if the swap is required
    Trk::BoundarySurfaceFace firstFaceCorr = firstFace;
    Trk::BoundarySurfaceFace secondFaceCorr = secondFace;


    // build volume envelope
    std::vector<const Trk::TrackingVolume*> vols;
    Trk::CylinderVolumeBounds* envBounds =  0;
    Amg::Transform3D* envTransf = 0;
    Trk::BinnedArray<Trk::TrackingVolume>*  subVols = 0;
    vols.push_back(&firstVol);
    vols.push_back(&secondVol);
    std::vector<const Trk::TrackingVolume*> envGlueNegXY;
    std::vector<const Trk::TrackingVolume*> envGluePosXY;
    std::vector<const Trk::TrackingVolume*> envGlueOuter;
    std::vector<const Trk::TrackingVolume*> envGlueInner;

    if (firstFace==Trk::positiveFaceXY) {
        envBounds =  new Trk::CylinderVolumeBounds(cyl1->innerRadius(),
                                                   cyl1->outerRadius(),
                                                   cyl1->halflengthZ()+cyl2->halflengthZ());
        envTransf   = new Amg::Transform3D;
        (*envTransf) = Amg::Translation3D(firstVol.center().x(), 
                                          firstVol.center().y(),
                                          firstVol.center().z()+cyl2->halflengthZ());
        subVols = m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(vols,false);
        envGlueNegXY.push_back(&firstVol);
        envGluePosXY.push_back(&secondVol);
        envGlueOuter = vols;
        envGlueInner = vols;
    } else if (firstFace==Trk::negativeFaceXY) {
        envBounds =  new Trk::CylinderVolumeBounds(cyl1->innerRadius(),
                                                   cyl1->outerRadius(),
                                                   cyl1->halflengthZ()+cyl2->halflengthZ());
        envTransf = new Amg::Transform3D;
        (*envTransf) = Amg::Translation3D(firstVol.center().x(), 
                                          firstVol.center().y(),
                                          firstVol.center().z()-cyl2->halflengthZ());
        envGlueNegXY.push_back(&secondVol);
        envGluePosXY.push_back(&firstVol);
        // revert vols
        vols.clear();
        vols.push_back(&secondVol);
        vols.push_back(&firstVol);
        // --- account for the swapping
        firstFaceCorr = secondFace;
        secondFaceCorr = firstFace;
        //
        subVols = m_trackingVolumeArrayCreator->cylinderVolumesArrayInZ(vols,false);
        envGlueOuter = vols;
        envGlueInner = vols;
    } else if (firstFace==Trk::tubeInnerCover) {
        if (secondFace==Trk::tubeOuterCover){
            envBounds =  new Trk::CylinderVolumeBounds(cyl2->innerRadius(),
                                                       cyl1->outerRadius(),
                                                       cyl1->halflengthZ());
        } else {
            envBounds =  new Trk::CylinderVolumeBounds(cyl1->outerRadius(),
                                                       cyl1->halflengthZ());
        }
        envTransf = firstVol.transform().isApprox(Amg::Transform3D::Identity()) ? 0 : new Amg::Transform3D;
        if (envTransf)
           (*envTransf) = Amg::Translation3D(firstVol.center());
        // revert vols
        vols.clear();
        vols.push_back(&secondVol);
        vols.push_back(&firstVol);
        // account for the swapping
        firstFaceCorr = secondFace;
        secondFaceCorr = firstFace;
        //
        subVols = m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(vols,false);
        envGlueNegXY = vols;
        envGluePosXY = vols;
        envGlueOuter.push_back(&firstVol);
        envGlueInner.push_back(&secondVol);
    } else {
        envBounds =  new Trk::CylinderVolumeBounds(cyl1->innerRadius(),
                                                   cyl2->outerRadius(),
                                                   cyl1->halflengthZ());
        envTransf = firstVol.transform().isApprox(Amg::Transform3D::Identity()) ? 0 : new Amg::Transform3D;
        if (envTransf)
           (*envTransf) = Amg::Translation3D(firstVol.center());
        subVols = m_trackingVolumeArrayCreator->cylinderVolumesArrayInR(vols,false);
        envGlueNegXY = vols;
        envGluePosXY = vols;
        envGlueOuter.push_back(&secondVol);
        envGlueInner.push_back(&firstVol);
        // account for the swapping
        firstFaceCorr = secondFace;
        secondFaceCorr = firstFace;
    }

    // create the enveloping volume
    enclosingVolume  =  new Trk::TrackingVolume(envTransf,
                                                envBounds,
                                                firstVol,
                                                0,subVols,
                                                name);

    // ENVELOPE GLUE DESCRIPTION -----------------------------------------------------------------
    // glue descriptors ---- they jump to the first one
    const Trk::GlueVolumesDescriptor& glueDescr  = enclosingVolume->glueVolumesDescriptor();

    // for the outside volumes, could be done in a loop as well, but will only save 4 lines
    std::vector<const Trk::TrackingVolume*> glueNegXY;
    std::vector<const Trk::TrackingVolume*> gluePosXY;
    std::vector<const Trk::TrackingVolume*> glueInner;
    std::vector<const Trk::TrackingVolume*> glueOuter;
    fillGlueVolumes(vols,envGlueNegXY,Trk::negativeFaceXY,glueNegXY);
    fillGlueVolumes(vols,envGluePosXY,Trk::positiveFaceXY,gluePosXY);
    fillGlueVolumes(vols,envGlueInner,Trk::tubeInnerCover,glueInner);
    fillGlueVolumes(vols,envGlueOuter,Trk::tubeOuterCover,glueOuter);
    // set them to the envelopGlueDescriptor
    glueDescr.registerGlueVolumes(Trk::negativeFaceXY, glueNegXY);
    glueDescr.registerGlueVolumes(Trk::positiveFaceXY, gluePosXY);
    glueDescr.registerGlueVolumes(Trk::tubeInnerCover, glueInner);
    glueDescr.registerGlueVolumes(Trk::tubeOuterCover, glueOuter);
    glueDescr.registerGlueVolumes(Trk::cylinderCover,  glueOuter);

    // INTERNAL GLUEING ---------------------------------------------------------------------------
    glueTrackingVolumes(vols,firstFaceCorr,secondFaceCorr);

    return enclosingVolume;
}


/** private helper method to fill the glue volumes (or the volume itself in) */      
void Trk::TrackingVolumeHelper::fillGlueVolumes(const std::vector< const TrackingVolume*>& topLevelVolumes,
                                                const std::vector< const TrackingVolume*>& envelopeFaceVolumes,
                                                BoundarySurfaceFace glueFace, 
                                                std::vector<const Trk::TrackingVolume*>& glueVols) const
{
    // loop over the topLevel Volumes
    std::vector<const Trk::TrackingVolume*>::const_iterator refVolIter = topLevelVolumes.begin();
    for ( ; refVolIter != topLevelVolumes.end(); ++refVolIter ) {
        // loop over the faceVolumes
        for (unsigned int ienvFace=0; ienvFace< envelopeFaceVolumes.size(); ++ienvFace){
            // check whether this volume was assigned to on this face
            if (envelopeFaceVolumes[ienvFace]==(*refVolIter)) {
                // get the GlueVolumesDescriptor
                const Trk::GlueVolumesDescriptor& glueVolDescriptor = (*refVolIter)->glueVolumesDescriptor();
                // if the size of glue volumes is 0 -> the referenceVolume is at navigation level
                if ( (glueVolDescriptor.glueVolumes(glueFace)).size() == 0) {
                    glueVols.push_back(*refVolIter);
                } else {
                    // fill all the sub-volumes described by the glueVolumeDescriptor
                    for (unsigned int isubNavVol=0; isubNavVol<(glueVolDescriptor.glueVolumes(glueFace)).size(); ++isubNavVol)
                        glueVols.push_back( (glueVolDescriptor.glueVolumes(glueFace))[isubNavVol]  );
                }
            }
        }// loop over envelopeFaceVolumes
    } // loop over reference Volumes
}


/** Execute the glueing  - the input volumes are all on navigation level */
void Trk::TrackingVolumeHelper::glueTrackingVolumes(const std::vector<const Trk::TrackingVolume*>& glueVols,
                                                    BoundarySurfaceFace firstFace,
                                                    BoundarySurfaceFace secondFace) const
{

    if (glueVols.size()<2) {
        ATH_MSG_VERBOSE( "Nothing to do in glueVolumes() " );
        return;
    }


    ATH_MSG_VERBOSE( " glueTrackingVolumes() called with boundary faces " << int(firstFace) << " and " << int(secondFace) << "." );

    // the iterators through the volumes
    std::vector<const Trk::TrackingVolume*>::const_iterator firstVol  = glueVols.begin();
    std::vector<const Trk::TrackingVolume*>::const_iterator secondVol = firstVol;
    ++secondVol;
    for ( ; secondVol != glueVols.end(); ++firstVol, ++secondVol) {

        if (msgLvl(MSG::VERBOSE))
            ATH_MSG_VERBOSE( "Processing '" << (*firstVol)->volumeName() << "' and '" << (*secondVol)->volumeName() << "'." );

        // get the glue volume descriptors to see that we have all subvolumes
        const Trk::GlueVolumesDescriptor& glueDescr1 = (*firstVol)->glueVolumesDescriptor();
        const Trk::GlueVolumesDescriptor& glueDescr2 = (*secondVol)->glueVolumesDescriptor();

        // glue volumes at navigation level
        std::vector<const Trk::TrackingVolume*> glueVols1;
        std::vector<const Trk::TrackingVolume*> glueVols2;
        glueVols1 = glueDescr1.glueVolumes(firstFace);
        glueVols2 = glueDescr2.glueVolumes(secondFace);

        // trivial cases
        // (glue one to the other)
        if (glueVols1.size()==0 && glueVols2.size()==0) {
            glueTrackingVolumes(**firstVol,firstFace,**secondVol,secondFace);
            continue;
            // (glue one to many)
        } else if (glueVols1.size()==0 && glueVols2.size()>0) {
            glueVols1.push_back(*firstVol);
            // (glue the other one to many)
        } else if (glueVols1.size()>0 && glueVols2.size()==0) {
            glueVols2.push_back(*secondVol);
        }

        // non-trivial case :: array against array
        // in Z : assume 2dim R/Phi
        if (firstFace==Trk::negativeFaceXY || firstFace==positiveFaceXY ) {
            // turn both vectors into R/Phi 2dim binnedArrays; assume equidistant binning in Phi
            Trk::BinnedArray<Trk::TrackingVolume>*  gv1 = m_trackingVolumeArrayCreator->cylinderVolumesArrayInPhiR(glueVols1,true);
            Trk::BinnedArray<Trk::TrackingVolume>*  gv2 = m_trackingVolumeArrayCreator->cylinderVolumesArrayInPhiR(glueVols2,true);
            SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > sgv1(gv1);
            SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > sgv2(gv2);

            // array vs. array in Z
            if (glueVols2.size()>1)
                for (unsigned int i=0; i<glueVols1.size(); i++) setOutsideTrackingVolumeArray( *(glueVols1[i]), firstFace, sgv2 );
            else
                for (unsigned int i=0; i<glueVols1.size(); i++) setOutsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols2[0] );

            if (glueVols1.size()>1)
                for (unsigned int i=0; i<glueVols2.size(); i++) setOutsideTrackingVolumeArray( *(glueVols2[i]), secondFace, sgv1 );
            else
                for (unsigned int i=0; i<glueVols2.size(); i++) setOutsideTrackingVolume( *(glueVols2[i]), secondFace, glueVols1[0] );


        } else {
            // turn both vectors into Z/Phi 2dim binnedArrays; assume equidistant binning in Phi
            Trk::BinnedArray<Trk::TrackingVolume>*  gv1 = m_trackingVolumeArrayCreator->cylinderVolumesArrayInPhiZ(glueVols1,true);
            Trk::BinnedArray<Trk::TrackingVolume>*  gv2 = m_trackingVolumeArrayCreator->cylinderVolumesArrayInPhiZ(glueVols2,true);
            SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > sgv1(gv1);
            SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > sgv2(gv2);

            // the glue cases -----------------------------------------------------------------------------------
            // handle the tube with care !
            // first vol
            for (unsigned int i=0; i<glueVols1.size(); i++) {
                // set the array as the outside array of the firstVol
                if (firstFace != Trk::tubeInnerCover) {
                    if (glueVols2.size()>1)
                        setOutsideTrackingVolumeArray( *(glueVols1[i]), firstFace, sgv2 );
                    else
                        setOutsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols2[0] );
                } else {
                    if (glueVols2.size()>1){
                        setInsideTrackingVolumeArray( *(glueVols1[i]), firstFace, sgv2 );
                        setOutsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols1[i] );
                    } else {
                        setInsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols2[0] );
                        setOutsideTrackingVolume( *(glueVols1[i]), firstFace, glueVols1[i] );
                    }
                }
            }
            // second
            for (unsigned int i=0; i<glueVols2.size(); i++) {
                // set the array as the outside array of the secondVol
                if (secondFace != Trk::tubeInnerCover)
                    setOutsideTrackingVolumeArray( *(glueVols2[i]), secondFace, sgv1 );
                else {
                    setInsideTrackingVolumeArray( *(glueVols2[i]), secondFace, sgv1 );
                    setOutsideTrackingVolume( *(glueVols2[i]), secondFace, glueVols2[i] );
                }
            }
        }
    }                 
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setInsideTrackingVolume(const Trk::TrackingVolume& tvol,
                                                        Trk::BoundarySurfaceFace face,
                                                        const Trk::TrackingVolume* insidevol) const
{
    Trk::TrackingVolumeManipulator::setInsideVolume( tvol, face, insidevol );
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setInsideTrackingVolumeArray(const Trk::TrackingVolume& tvol,
                                                             Trk::BoundarySurfaceFace face,
                                                             Trk::BinnedArray<Trk::TrackingVolume>* insidevolarray) const
{
    Trk::TrackingVolumeManipulator::setInsideVolumeArray(tvol,face,insidevolarray);
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setInsideTrackingVolumeArray(const Trk::TrackingVolume& tvol,
                                                             Trk::BoundarySurfaceFace face,
                                                             Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > insidevolarray) const
{
    Trk::TrackingVolumeManipulator::setInsideVolumeArray(tvol,face,insidevolarray);
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setOutsideTrackingVolume(const Trk::TrackingVolume& tvol,
                                                         Trk::BoundarySurfaceFace face,
                                                         const Trk::TrackingVolume* outsidevol) const
{ 
    ATH_MSG_VERBOSE( "     -> Glue '" << outsidevol->volumeName() << "' at face " << face << " to '" << tvol.volumeName() << "'.");
    Trk::TrackingVolumeManipulator::setOutsideVolume( tvol, face, outsidevol );
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setOutsideTrackingVolumeArray(const Trk::TrackingVolume& tvol,
                                                              Trk::BoundarySurfaceFace face,
                                                              Trk::BinnedArray<Trk::TrackingVolume>* outsidevolarray) const
{ 
    unsigned int numVols = outsidevolarray->arrayObjects().size() ;
    ATH_MSG_VERBOSE( "     -> Glue " << numVols << " volumes at face " << face << " to '" << tvol.volumeName() );
    Trk::TrackingVolumeManipulator::setOutsideVolumeArray( tvol, face, outsidevolarray );
}


/** Simply forward to base class method to enhance friendship relation */
void Trk::TrackingVolumeHelper::setOutsideTrackingVolumeArray(const Trk::TrackingVolume& tvol,
                                                              Trk::BoundarySurfaceFace face,
                                                              Trk::SharedObject<Trk::BinnedArray<Trk::TrackingVolume> > outsidevolarray) const
{ 
    unsigned int numVols = outsidevolarray.getPtr()->arrayObjects().size() ;
    ATH_MSG_VERBOSE( "     -> Glue " << numVols << " volumes at face " << face << " to '" << tvol.volumeName() );
    Trk::TrackingVolumeManipulator::setOutsideVolumeArray( tvol, face, outsidevolarray );
}



