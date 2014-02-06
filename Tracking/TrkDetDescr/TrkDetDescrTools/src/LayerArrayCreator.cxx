/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerArrayCreator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTools/LayerArrayCreator.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/NavigationLayer.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"


// constructor
Trk::LayerArrayCreator::LayerArrayCreator(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p)
{
    declareInterface<ILayerArrayCreator>(this);
}

// destructor
Trk::LayerArrayCreator::~LayerArrayCreator()
{}


// the interface methods
StatusCode Trk::LayerArrayCreator::initialize()
{
    ATH_MSG_INFO( "initialize()" );
    return StatusCode::SUCCESS;
}    

StatusCode Trk::LayerArrayCreator::finalize()
{    
    ATH_MSG_INFO( "finalize() successful" );    
    return StatusCode::SUCCESS;
}


Trk::LayerArray* Trk::LayerArrayCreator::cylinderLayerArray(const std::vector<const Trk::CylinderLayer*>& cylLayersInput,
                                                            double rmin, double rmax, Trk::BinningType btype) const
{  
    ATH_MSG_VERBOSE( " build LayerArray with " << cylLayersInput.size() << " cylindrical material layers." );
    ATH_MSG_VERBOSE( "       rmin/rmax provided : " << rmin << " / " << rmax );

    //copy so that you can sort
    std::vector<const Trk::CylinderLayer*> cylLayers(cylLayersInput);

    // sort the vector
    Trk::CylinderLayerSorterR rSorter;
    std::vector<const Trk::CylinderLayer*>::iterator sortBegin = cylLayers.begin();
    std::vector<const Trk::CylinderLayer*>::iterator sortEnd   = cylLayers.end();
    std::sort(sortBegin, sortEnd, rSorter);

    // needed for all cases
    Trk::LayerArray*                    cylinderLayerArray = 0;
    Trk::BinUtility*                    binUtility = 0;
    std::vector< LayerOrderPosition >   layerOrderVector;

    // the iterator
    std::vector<const Trk::CylinderLayer*>::const_iterator layIter = cylLayers.begin();

    switch (btype) {
        
        // equidistant binning - no navigation layers built
        case Trk::equidistant :
        {
            // count the layers
            unsigned int layers = cylLayers.size();
            // loop over layers and put them in
            for ( ; layIter != cylLayers.end(); ++layIter) {
                // get the R
                const Trk::CylinderSurface& layerSurface = (*layIter)->surfaceRepresentation();
                double currentR = layerSurface.bounds().r();
        
                ATH_MSG_VERBOSE( "equidistant : registering cylindrical MaterialLayer at   radius : " << currentR );
        
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Layer>(*layIter),
                                            Amg::Vector3D(currentR, 0.,0.)));
            }        
            // create the binUitlity
            binUtility = new Trk::BinUtility(layers,rmin,rmax,Trk::open, Trk::binR);
            ATH_MSG_VERBOSE( "equidistant : created a BinUtility as " << *binUtility );
            
            // create the BinnedArray
            cylinderLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);     
        } break;
        
        // bi-equidistant binning - take care the layers have to be binned equidistant + same thickness
        case Trk::biequidistant :
        {
            // count the layers
            unsigned int layers = cylLayers.size();
            // take a reference thinkness
            double layerThickness = cylLayers[0]->thickness();
                        
            // the radialstep
            double radialStep = (rmax-layerThickness-rmin)/(layers-1);
        
            // the next step
            Trk::NavigationLayer* navLayer = 0;
            std::vector<const Trk::CylinderLayer*>::const_iterator layEndIter = cylLayers.end();
        
            // loop over layers
            for (; layIter != layEndIter ; ++layIter) {
                // get the dimensions
                const Trk::CylinderSurface& layerSurface = (*layIter)->surfaceRepresentation();
                double currentR = layerSurface.bounds().r();
                double navLayerHalflengthZ = layerSurface.bounds().halflengthZ();

                ATH_MSG_VERBOSE( "bi-equidistant : registering cylindrical MaterialLayer at   radius : " << currentR );
                // push the original layer in
                layerOrderVector.push_back(Trk::LayerOrderPosition(
                                           Trk::SharedObject<const Trk::Layer>(*layIter),
                                           Amg::Vector3D(currentR,0.,0.)));
                                           
                // special treatment for the first one
                if (layIter != layEndIter-1 ) {
                    double navigationR = currentR + 0.5*(radialStep+layerThickness);
                    ATH_MSG_VERBOSE( "bi-equidistant : creating cylindrical NavigationLayer at   radius : " << navigationR );
                    Trk::CylinderSurface* navLayerSurfaceOne =
                            new Trk::CylinderSurface(new Amg::Transform3D(layerSurface.transform()),
                                                     navigationR,
                                                     navLayerHalflengthZ);
                    // the navigation layer
                    navLayer = new Trk::NavigationLayer(navLayerSurfaceOne);
                    // push the navigation layer in
                    layerOrderVector.push_back(Trk::LayerOrderPosition(
                                               Trk::SharedObject<const Trk::Layer>(navLayer),
                                               Amg::Vector3D(navigationR, 0., 0.)));
                }
                                                                                      
            }

            ATH_MSG_VERBOSE( layerOrderVector.size() << " cylindrical Layers (material + navigation) built. " );
            
            // create the binUtility
            binUtility = new Trk::BinUtility(layers, layerThickness, rmin, rmax, Trk::open, Trk::binR);
            ATH_MSG_VERBOSE( "b-equidistant : created a BinUtility as " << *binUtility );
            
            // create the BinnedArray
            cylinderLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);
        
        } break;
        
        // arbitrary binning
        case Trk::arbitrary :
        {
            std::vector<float> boundaries;
            // maz z extension
            double halfLengthZ = 0;
            // initial step
            boundaries.push_back(rmin);
                        
            // loop over the provided layers and put Navigation layers in between
            for ( ; layIter != cylLayers.end(); ++layIter) {
        
                // get the cylinder surface
                const Trk::CylinderSurface& layerSurface = (*layIter)->surfaceRepresentation();
                // and get the halflength
                double currentHalfLengthZ = layerSurface.bounds().halflengthZ();
                halfLengthZ = (currentHalfLengthZ > halfLengthZ) ? currentHalfLengthZ : halfLengthZ;
                double layerRadius = layerSurface.bounds().r();
                double layerThickness = (*layIter)->thickness();
                // navigation layer : previous bin
                boundaries.push_back(layerRadius-0.5*layerThickness);
                double navLayerRadius = 0.5*(boundaries[boundaries.size()-1]+boundaries[boundaries.size()-2]);
                Trk::CylinderSurface* navLayerSurface = new Trk::CylinderSurface(navLayerRadius, halfLengthZ);
                ATH_MSG_VERBOSE( "arbitrary : creating cylindrical NavigationLayer at radius : " << navLayerRadius );
                layerOrderVector.push_back(Trk::LayerOrderPosition(
                                           Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurface)),
                                           Amg::Vector3D(navLayerRadius, 0., 0.)));
                                          
                // material layer : current bin
                ATH_MSG_VERBOSE( "arbitrary : registering cylindrical MaterialLayer at   radius :" << layerRadius );
                boundaries.push_back(layerRadius+0.5*layerThickness);
                layerOrderVector.push_back(Trk::LayerOrderPosition(
                                           Trk::SharedObject<const Trk::Layer>(*layIter),
                                           Amg::Vector3D(layerRadius, 0.,0.)));
            } 
            // close up the array with last bin
            double navLayerRadiusFinal = 0.5*(rmax+boundaries[boundaries.size()-1]);
            Trk::CylinderSurface* navLayerSurfaceFinal = new Trk::CylinderSurface(navLayerRadiusFinal, halfLengthZ);
            boundaries.push_back(rmax);
            ATH_MSG_VERBOSE( "arbitrary : creating cylindrical NavigationLayer at radius : " << navLayerRadiusFinal );
            layerOrderVector.push_back(Trk::LayerOrderPosition(
                                       Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurfaceFinal)),
                                       Amg::Vector3D(navLayerRadiusFinal, 0., 0.)));
        
            ATH_MSG_VERBOSE( layerOrderVector.size() << " cylindrical Layers (material + navigation) built. " );
            // create the BinUtility
            binUtility = new Trk::BinUtility(boundaries, Trk::open, Trk::binR);
            ATH_MSG_VERBOSE( "arbitrary : created a BinUtility as " << *binUtility );
            
            // and the BinnedArray
            cylinderLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);
        
        } break;
        
        // default return 0
        default : { return 0; }
    }

    return cylinderLayerArray;
}


Trk::LayerArray* Trk::LayerArrayCreator::discLayerArray(const std::vector<const Trk::DiscLayer*>& discLayersInput,
                                                        double zmin,
                                                        double zmax,
                                                        Trk::BinningType btype) const
{

    ATH_MSG_VERBOSE( " build LayerArray with " << discLayersInput.size() << " disc-like material layers." );
    ATH_MSG_VERBOSE( "       zmin/zmax provided : " << zmin << " / " << zmax );

    // needed for all cases
    Trk::LayerArray*                    discLayerArray = 0;
    Trk::BinUtility*                    binUtility = 0;
    std::vector< LayerOrderPosition >   layerOrderVector;

    //copy so that you can sort
    std::vector<const Trk::DiscLayer*> discLayers(discLayersInput);

    // sort the vector
    Trk::DiscLayerSorterZ zSorter;
    std::vector<const Trk::DiscLayer*>::iterator sortBegin = discLayers.begin();
    std::vector<const Trk::DiscLayer*>::iterator sortEnd   = discLayers.end();
    std::sort(sortBegin, sortEnd, zSorter);

    // the iterator
    std::vector<const Trk::DiscLayer*>::const_iterator layIter = discLayers.begin();

    switch (btype) {
        
        // equidistant binning
        case Trk::equidistant :
        {
            // count the layers
            size_t layers = discLayers.size();
            // loop over layers and put them in
            for ( ; layIter != discLayers.end(); ++layIter) {
                // get the Z
                const Trk::DiscSurface& layerSurface = (*layIter)->surfaceRepresentation();
                ATH_MSG_VERBOSE( "equidistant : registering disc-like MaterialLayer   at z-Position : " << layerSurface.center().z() );
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Layer>(*layIter),
                                            layerSurface.center()));
            }
            // create the binUitlity
            binUtility = new Trk::BinUtility(layers,zmin,zmax,Trk::open,Trk::binZ);
            ATH_MSG_VERBOSE( "equidistant : created a BinUtility as " << *binUtility );
            
            // create the BinnedArray
            discLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);
        
        } break;
        
        // bi-equidistant binning
        case Trk::biequidistant :
        {
            // count the layers
            unsigned int layers = discLayers.size();
            // take a reference layer thickness
            double layerThickness = discLayers[0]->thickness();
            
            // the radialstep
            double zStep = (zmax-layerThickness-zmin)/(layers-1);
            double minR = 0.;
            double maxR = 0.;
        
            // loop over layers
            for (; layIter != discLayers.end() ; ++layIter) {
        
                // get the dimensions
                const Trk::DiscSurface& layerSurface = (*layIter)->surfaceRepresentation();
                double currentZ = layerSurface.center().z();
                // and get dimensions
                const Trk::DiscBounds* dbounds = dynamic_cast<const Trk::DiscBounds*>(&(layerSurface.bounds()));
                if (dbounds) {
                    minR = dbounds->rMin();
                    maxR = dbounds->rMax();
                } else { // protection
                    minR = 0.;
                    maxR = 100000.;
                }

                // get the material layer first
                ATH_MSG_VERBOSE( "bi-equidistant : registering disc-like MaterialLayer at z-Position : " << currentZ );
        
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Trk::Layer>(*layIter),
                                            layerSurface.center()));
                                            

        
                if (layIter != discLayers.end()-1 ) {
                    double navigationZ = currentZ + 0.5*(layerThickness+zStep);
                    Amg::Transform3D* navLayerTransform = new Amg::Transform3D;
                    (*navLayerTransform) = Amg::Translation3D(0.,0.,navigationZ);
                    Trk::DiscSurface* navLayerSurface = new Trk::DiscSurface(navLayerTransform, minR, maxR);
        
                    ATH_MSG_VERBOSE( "bi-equidistant : creating disc-like NavigationLayer at z-Position : " << navigationZ );
        
                    layerOrderVector.push_back( Trk::LayerOrderPosition(
                                                Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurface)),
                                                Amg::Vector3D(0., 0.,navigationZ)));
                }
        
            }
            // verbose output 
            ATH_MSG_VERBOSE( layerOrderVector.size() << " disc Layers (material + navigation) built. " );

            // create the binUtility
            binUtility = new Trk::BinUtility(layers, layerThickness, zmin, zmax, Trk::open, Trk::binZ);
            ATH_MSG_VERBOSE( "bi-equidistant : created a BinUtility as " << *binUtility );
        
            // create the BinnedArray
            discLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);
        
        } break;
        
        // arbitrary binning
        case Trk::arbitrary :
        {
            std::vector<float>  boundaries;

            // max disc dimension
            double minR = 10e10;
            double maxR = 0.;
        
            // initial boundary
            boundaries.push_back(zmin);
            
            // loop over the provided layers and put NavigationLayers in between
            for ( ; layIter != discLayers.end(); ++layIter) {
                // get the cylinder surface
                const Trk::DiscSurface& layerSurface = (*layIter)->surfaceRepresentation();
                // the radius & position
                double layerPositionZ = layerSurface.center().z();
                double layerThickness = (*layIter)->thickness();
                boundaries.push_back(layerPositionZ-0.5*layerThickness);
                // and get dimensions
                const Trk::DiscBounds* dbounds = dynamic_cast<const Trk::DiscBounds*>(&(layerSurface.bounds()));
                if (dbounds) {
                    double layInnerR = dbounds->rMin();
                    double layOuterR = dbounds->rMax();
                    minR = (layInnerR < minR) ? layInnerR : minR;
                    maxR = (layOuterR > maxR) ? layOuterR : maxR;
                } else { // protection
                    minR = 0.;
                    maxR = 100000.;
                }
                // navigation Layer
                double navLayerPositionZ = 0.5*(boundaries[boundaries.size()-1]+boundaries[boundaries.size()-2]);
                // now fill the layer post slot after navigation layer has been determined
                boundaries.push_back(layerPositionZ+0.5*layerThickness);                                
                // the transform for this
                Amg::Transform3D* navLayerTransform = new Amg::Transform3D;
                (*navLayerTransform) = Amg::Translation3D(0.,0.,navLayerPositionZ);
                Trk::DiscSurface* navLayerSurface = new Trk::DiscSurface(navLayerTransform, minR, maxR);
                ATH_MSG_VERBOSE( "arbitrary : creating disc-like NavigationLayer at z-Position : " << navLayerPositionZ );
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurface)),
                                            Amg::Vector3D(0., 0., navLayerPositionZ)));
                                            
                // the material layer
                ATH_MSG_VERBOSE( "arbitrary : registering disc-like MaterialLayer at z-Position : " << layerPositionZ );
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Trk::Layer>(*layIter),
                                            Amg::Vector3D(0.,0., layerPositionZ)));


            } 
            // final material layer
            double navLayerPositionZFinal = 0.5*(zmax+boundaries[boundaries.size()-1]);
            Amg::Transform3D* navLayerTransformFinal = new Amg::Transform3D;
            (*navLayerTransformFinal) = Amg::Translation3D(0.,0.,navLayerPositionZFinal);
            Trk::DiscSurface* navLayerSurfaceFinal = new Trk::DiscSurface(navLayerTransformFinal, minR, maxR);
            ATH_MSG_VERBOSE( "arbitrary : creating disc-like NavigationLayer at z-Position : " << navLayerPositionZFinal );
            layerOrderVector.push_back( Trk::LayerOrderPosition(
                                        Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurfaceFinal)), 
                                        Amg::Vector3D(0., 0., navLayerPositionZFinal)));
            ATH_MSG_VERBOSE( layerOrderVector.size() << " disc Layers (material + navigation) built. " );
            // register the last bounday
            boundaries.push_back(zmax);
            // create the BinUtility
            binUtility = new Trk::BinUtility(boundaries, Trk::open, Trk::binZ);
            ATH_MSG_VERBOSE( "arbitrary : created a BinUtility as " << *binUtility );
            
            // and the BinnedArray
            discLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);
        
        } break;
        
        // default return 0
        default : { return 0; }
    }

    return discLayerArray;
}


Trk::LayerArray* Trk::LayerArrayCreator::planeLayerArray(const std::vector<const Trk::PlaneLayer*>& planeLayersInput,
                                                         double posmin, double posmax, Trk::BinningType btype, Trk::BinningValue bv) const
{
    ATH_MSG_VERBOSE( " build LayerArray with " << planeLayersInput.size() << " plane-like material layers." );

    // needed for all cases
    Trk::LayerArray*                    planeLayerArray = 0;
    Trk::BinUtility*                    binUtility = 0;
    std::vector< LayerOrderPosition >   layerOrderVector;
    Amg::Vector3D layerCenter(0.,0.,0.);

    //copy so that you can sort
    std::vector<const Trk::PlaneLayer*> planeLayers(planeLayersInput);
    
    std::vector<const Trk::PlaneLayer*>::iterator sortBegin = planeLayers.begin();
    std::vector<const Trk::PlaneLayer*>::iterator sortEnd   = planeLayers.end();
    switch  (bv) {
        case Trk::binX : { std::sort(sortBegin, sortEnd, Trk::PlaneLayerSorterX()); } break;
        case Trk::binY : { std::sort(sortBegin, sortEnd, Trk::PlaneLayerSorterY()); } break;
        case Trk::binZ : { std::sort(sortBegin, sortEnd, Trk::PlaneLayerSorterZ()); } break;
        default : {
            ATH_MSG_WARNING("Plane Layers can only be sorted in x/y/z. Returning 0.");
            return 0;  
        }  
    }

    // the iterator
    std::vector<const Trk::PlaneLayer*>::const_iterator layIter = planeLayers.begin();

    switch (btype) {

        // equidistant binning
        case Trk::equidistant :
        {
            // count the layers
            unsigned int layers = planeLayers.size();
            // loop over layers and put them in
            for ( ; layIter != planeLayers.end(); ++layIter) {
                // get the X
                const Trk::PlaneSurface& layerSurface = (*layIter)->surfaceRepresentation();        
                ATH_MSG_VERBOSE( "equidistant : registering plane-like MaterialLayer   at position : " << layerSurface.center() );
        
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Layer>(*layIter),
                                            layerSurface.center()));
            }
            // create the binUitlity
            binUtility = new Trk::BinUtility(layers,posmin,posmax, Trk::open, bv);
            // create the BinnedArray
            planeLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);
        
        } break;
        
        // bi-equidistant binning
        case Trk::biequidistant :
        {
            // count the layers
            unsigned int layers = planeLayers.size();
            // the x-step
            double posStep = (posmax-posmin)/(layers+1);
        
            double currentPos = posmin + posStep;
            double lastPos    = posmin;
        
            double minHalfX = 0.;
            double maxHalfX = 0.;
            double halfY    = 0.;
        
            double layerThickness = 0.;
        
            // loop over layers
            for ( ; layIter != planeLayers.end() ; ++layIter) {
        
                // get the dimensions
                const Trk::PlaneSurface& layerSurface = (*layIter)->surfaceRepresentation();
                // get dimensions
                const Trk::RectangleBounds* recbounds = dynamic_cast<const Trk::RectangleBounds*>(&(layerSurface.bounds()));
                // try rectangular hypothesis
                if (recbounds) {
                    maxHalfX = recbounds->halflengthX();
                    halfY    = recbounds->halflengthY();
                } else {
                    // try trapezoidal hypothesis
                    const Trk::TrapezoidBounds* trapbounds = dynamic_cast<const Trk::TrapezoidBounds*>(&(layerSurface.bounds()));
                    if (trapbounds) {
                        minHalfX = trapbounds->minHalflengthX();
                        maxHalfX = trapbounds->maxHalflengthX();
                        halfY    = trapbounds->halflengthY();
                    } else {
                        // protection
                        minHalfX = 0.;
                        maxHalfX = 10e10;
                        halfY    = 10e10;
                    }
                }
        
                layerThickness = ((*layIter)->thickness() > layerThickness ) ? (*layIter)->thickness() : layerThickness;
        
                // the navigation Layer
                double navigationPos = 0.5*(currentPos+lastPos);
                double navigationX = (bv == Trk::binX) ? navigationPos : 0.;
                double navigationY = (bv == Trk::binY) ? navigationPos : 0.;
                double navigationZ = (bv == Trk::binZ) ? navigationPos : 0.;
                Amg::Translation3D(navigationX,navigationY,navigationZ);
                
                Trk::PlaneSurface* navLayerSurface = 0;
                Amg::Transform3D* navLayerTransform  = new Amg::Transform3D;
                (*navLayerTransform)  = Amg::Translation3D(navigationX,0.,0.);

                if (fabs(minHalfX)<10e-5) {
                    navLayerSurface = new Trk::PlaneSurface(navLayerTransform,
                                                            maxHalfX,
                                                            halfY);
                } else {
                    navLayerSurface = new Trk::PlaneSurface(navLayerTransform,
                                                            minHalfX,
                                                            maxHalfX,
                                                            halfY);
                }
        
                ATH_MSG_VERBOSE( "bi-equidistant : creating plane-like NavigationLayer at position : " << navigationX );
        
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurface)),
                                            Amg::Vector3D(navigationX, 0.,0.)));
                // restore
                lastPos = currentPos;
                // the material Layer
        
                ATH_MSG_VERBOSE( "bi-equidistant : registering plane-like MaterialLayer at position : " << currentPos );
                layerOrderVector.push_back( Trk::LayerOrderPosition (
                                            Trk::SharedObject<const Trk::Layer>(*layIter),
                                            layerSurface.center()));
        
                // increase the Step
                currentPos += posStep;
            }

            // the final navigation layer
            double navigationPosFinal = 0.5*(currentPos+lastPos);
            double navigationXFinal   = (bv == Trk::binX) ? navigationPosFinal : 0.;
            double navigationYFinal   = (bv == Trk::binY) ? navigationPosFinal : 0.;
            double navigationZFinal   = (bv == Trk::binZ) ? navigationPosFinal : 0.;
            
            Amg::Transform3D* navLayerTransform  = new Amg::Transform3D;
            (*navLayerTransform)  = Amg::Translation3D(navigationXFinal,navigationYFinal,navigationZFinal);

            Trk::PlaneSurface* navLayerSurface = (fabs(minHalfX)<10e-5) ?
                    new Trk::PlaneSurface(navLayerTransform, maxHalfX,halfY) :
                    new Trk::PlaneSurface(navLayerTransform, minHalfX, maxHalfX, halfY);
        
            ATH_MSG_VERBOSE( "bi-equidistant : creating plane-like NavigationLayer at position : " << navLayerSurface->center() );
        
            layerOrderVector.push_back( Trk::LayerOrderPosition(
                                        Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurface)),
                                        navLayerSurface->center() ));
        
            // create the binUtility
            binUtility = new Trk::BinUtility(layers, layerThickness, posmin, posmax, Trk::open, bv);
        
            // create the BinnedArray
            planeLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);
        
        } break;
        
        // arbitrary binning
        case Trk::arbitrary :
        {
        
            std::vector<float> boundaries;
            boundaries.push_back(posmin);
            // max plane dimension
            double minHalfX = 0.;
            double maxHalfX = 0.;
            double halfY    = 0.;
        
            // loop over the layers and register navigation layers in between
            for ( ; layIter != planeLayers.end(); ++layIter) {
        
                // get the cylinder surface
                const Trk::PlaneSurface& layerSurface = (*layIter)->surfaceRepresentation();
                // get dimensions
                const Trk::RectangleBounds* recbounds = dynamic_cast<const Trk::RectangleBounds*>(&(layerSurface.bounds()));
                // try rectangular hypothesis
                if (recbounds) {
                    maxHalfX = recbounds->halflengthX();
                    halfY    = recbounds->halflengthY();
                } else {
                    // try trapezoidal hypothesis
                    const Trk::TrapezoidBounds* trapbounds = dynamic_cast<const Trk::TrapezoidBounds*>(&(layerSurface.bounds()));
                    if (trapbounds) {
                        minHalfX = trapbounds->minHalflengthX();
                        maxHalfX = trapbounds->maxHalflengthX();
                        halfY    = trapbounds->halflengthY();
                    } else {
                        // protection
                        minHalfX = 0.;
                        maxHalfX = 10e10;
                        halfY    = 10e10;
                    }
                }
                // the x position
                // and get dimensions
                layerCenter = layerSurface.center();
                double layerPosition = layerCenter[bv];
                // get the thickness
                double layerThickness = (*layIter)->thickness();
                // register
                boundaries.push_back(layerPosition-0.5*layerThickness);
                double navLayerPositionX = (bv == Trk::binX) ? 0.5*(layerPosition+boundaries[boundaries.size()-1]) : layerCenter.x();
                double navLayerPositionY = (bv == Trk::binY) ? 0.5*(layerPosition+boundaries[boundaries.size()-1]) : layerCenter.y();
                double navLayerPositionZ = (bv == Trk::binZ) ? 0.5*(layerPosition+boundaries[boundaries.size()-1]) : layerCenter.z();
                Amg::Translation3D navLayerPosition(navLayerPositionX,navLayerPositionY,navLayerPositionZ);
                Amg::Transform3D* navLayerTransform = new Amg::Transform3D;
                (*navLayerTransform)  = navLayerPosition;
                // create the navigation plane layer        
                Trk::PlaneSurface* navLayerSurface = (fabs(minHalfX)<10e-5) ?
                        new Trk::PlaneSurface( navLayerTransform, maxHalfX, halfY ) :
                        new Trk::PlaneSurface( navLayerTransform, minHalfX, maxHalfX, halfY );
                ATH_MSG_VERBOSE( "arbitrary : creating plane-like NavigationLayer at position : " << navLayerPositionX );
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurface)),
                                            Amg::Vector3D(navLayerPositionX, navLayerPositionY, navLayerPositionZ)));        
                // register the material layer
                boundaries.push_back(layerPosition+0.5*layerThickness);                            
                // material layer
                layerOrderVector.push_back( Trk::LayerOrderPosition(
                                            Trk::SharedObject<const Trk::Layer>(*layIter),
                                            layerSurface.center()));                                        

            } 
            // last NavigationLayer
            double navLayerPositionXFinal = (bv == Trk::binX) ? 0.5*(posmax+boundaries[boundaries.size()-1]) : layerCenter.x();
            double navLayerPositionYFinal = (bv == Trk::binY) ? 0.5*(posmax+boundaries[boundaries.size()-1]) : layerCenter.y();
            double navLayerPositionZFinal = (bv == Trk::binZ) ? 0.5*(posmax+boundaries[boundaries.size()-1]) : layerCenter.z();
            Amg::Translation3D navLayerPositionFinal(navLayerPositionXFinal,navLayerPositionYFinal,navLayerPositionZFinal);
            Amg::Transform3D* navLayerTransformFinal = new Amg::Transform3D;
            (*navLayerTransformFinal)  = navLayerPositionFinal;
            // create the navigation plane layer        
            Trk::PlaneSurface* navLayerSurfaceFinal = (fabs(minHalfX)<10e-5) ?
                        new Trk::PlaneSurface( navLayerTransformFinal, maxHalfX, halfY ) :
                        new Trk::PlaneSurface( navLayerTransformFinal, minHalfX, maxHalfX, halfY );
            ATH_MSG_VERBOSE( "arbitrary : creating plane-like NavigationLayer at position : " << 0.5*(posmax+boundaries[boundaries.size()-1]) );
            layerOrderVector.push_back( Trk::LayerOrderPosition(
                                        Trk::SharedObject<const Trk::Layer>(new Trk::NavigationLayer(navLayerSurfaceFinal)),
                                        Amg::Vector3D(navLayerPositionXFinal, navLayerPositionYFinal, navLayerPositionZFinal)));
    
            ATH_MSG_VERBOSE( layerOrderVector.size() << " plane Layers (material + navigation) built. " );
        
            // create the BinUtility
            binUtility = new Trk::BinUtility(boundaries, Trk::open, bv);
            // and the BinnedArray
            planeLayerArray = new Trk::BinnedArray1D<Trk::Layer>(layerOrderVector, binUtility);
        
        } break;
        // default return 0
        default : { return 0; }
    }

    return planeLayerArray;
}

