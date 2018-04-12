/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// PlanarModuleStepper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDigTools/PlanarModuleStepper.h"
#include "TrkDigEvent/DigitizationModule.h"
#include "TrkDetDescrUtils/Intersection.h"
#include "TrkSurfaces/Surface.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

// constructor
Trk::PlanarModuleStepper::PlanarModuleStepper(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p)
{
    declareInterface<IModuleStepper>(this);
}

// destructor
Trk::PlanarModuleStepper::~PlanarModuleStepper()
{}


// Athena standard methods
// initialize
StatusCode Trk::PlanarModuleStepper::initialize()
{

    return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::PlanarModuleStepper::finalize()
{
    return StatusCode::SUCCESS;
}


/** calculate the steps caused by this strack */
std::vector<Trk::DigitizationStep> Trk::PlanarModuleStepper::cellSteps(const Trk::DigitizationModule& dmodule,
                                                                       const Amg::Vector3D& startPoint,
                                                                       const Amg::Vector3D& endPoint) const
{  
    // create the return vector
    std::vector<Trk::DigitizationStep> cSteps;
    
    // get the test surfaces for bin intersections
    auto& stepSurfaces = dmodule.stepSurfaces(startPoint,endPoint);
    
    // the track direction
    Amg::Vector3D trackDirection((endPoint-startPoint).unit());
    
    // the intersections through the surfaces, start one is the first valid one
    std::vector<Trk::Intersection> stepIntersections;
    stepIntersections.reserve(stepSurfaces.size()+1);
    
    // run them - and check for the fast exit    
    for (auto& sSurface : stepSurfaces){
        // try it out by intersecting, but do not force the direction 
        Trk::Intersection sIntersection = sSurface->straightLineIntersection(startPoint,trackDirection,true,true);
        if (sIntersection.valid) {
            // now record
            stepIntersections.push_back(sIntersection);
            ATH_MSG_VERBOSE("Boundary Surface intersected with = " << Amg::toString(sIntersection.position) );
        }  
    }
    // last one is also valid - now sort
    stepIntersections.push_back(Trk::Intersection(endPoint,(startPoint-endPoint).mag(),true));
    std::sort(stepIntersections.begin(),stepIntersections.end());
    
    Amg::Vector3D lastPosition = startPoint;    
    // reserve the right amount
    cSteps.reserve(stepIntersections.size());
    for (auto& sIntersection : stepIntersections){
        // create the new cStep 
        cSteps.push_back( dmodule.digitizationStep(lastPosition,sIntersection.position));
        lastPosition = sIntersection.position;
    }
    // return all the steps
    return cSteps;
}                                                                      
                                                                                                                                                

// calculate the steps caused by this track - fast simulation interface
std::vector<Trk::DigitizationStep> Trk::PlanarModuleStepper::cellSteps(const Trk::DigitizationModule& dmodule,
                                                                       const Amg::Vector2D& moduleIntersection,
                                                                       const Amg::Vector3D& trackDirection) const
{

    
    // first, intersect the boundary surfaces
    auto boundarySurfaces =  dmodule.boundarySurfaces();
    // intersect them - fast exit for cases where readout and counter readout are hit
    Amg::Vector3D intersection3D(moduleIntersection.x(),moduleIntersection.y(),0.);
    size_t attempts = 0;
    // the collected intersections
    std::vector< Trk::Intersection > boundaryIntersections; 
    // run them - and check for the fast exit    
    for (auto& bSurface : boundarySurfaces){
        // count as an attempt
        ++attempts;
        // try it out by intersecting, but do not force the direction 
        Trk::Intersection bIntersection = bSurface->straightLineIntersection(intersection3D,trackDirection,false,true);
        if (bIntersection.valid) {
            // now record
            boundaryIntersections.push_back(bIntersection);
            ATH_MSG_VERBOSE("Boundary Surface intersected with = " << Amg::toString(bIntersection.position) );
        }
        // fast break in case of readout/counter surface hit
        if (attempts == 2 && boundaryIntersections.size() == attempts) break; 
        else if (attempts > 2 && boundaryIntersections.size()==3) break;    
    }
    // post-process if we have more than 2 intersections, only first or last can be wrong after resorting 
    if (boundaryIntersections.size() > 2){
        ATH_MSG_VERBOSE("More than 2 Boundary Surfaces intersected, this is an edge case, resolving ... ");
        std::sort(boundaryIntersections.begin(),boundaryIntersections.end());
        if (boundaryIntersections[0].pathLength*boundaryIntersections[1].pathLength < 0.) boundaryIntersections.erase(boundaryIntersections.end());
        else boundaryIntersections.erase(boundaryIntersections.begin());
    }
    // return 
    return cellSteps(dmodule, boundaryIntersections[0].position, boundaryIntersections[1].position);
}
