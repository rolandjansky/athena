/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// RectangularSegmentation.cxx, ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk includes
#include "TrkDigEvent/RectangularSegmentation.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
// Amg includes
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

Trk::RectangularSegmentation::RectangularSegmentation(std::shared_ptr<const Trk::RectangleBounds> mBounds,
                                                      size_t numCellsX, size_t numCellsY) :
   m_activeBounds(mBounds),
   m_binUtility(nullptr),
   m_binsX(numCellsX),
   m_binsY(numCellsY)
{
    // first the x dimension if needed
    if (numCellsX > 1)
         m_binUtility = new Trk::BinUtility(numCellsX, -mBounds->halflengthX(), mBounds->halflengthX(), Trk::open, Trk::binX); 
    // use y dimension if needed
    if (numCellsY > 1){
        Trk::BinUtility yBinUtility(numCellsY, -mBounds->halflengthY(), mBounds->halflengthY(), Trk::open, Trk::binY);
        if (m_binUtility)
            (*m_binUtility) += yBinUtility;
        else 
            m_binUtility = new Trk::BinUtility(yBinUtility);
    }           
}        

Trk::RectangularSegmentation::~RectangularSegmentation()
{
    delete m_binUtility;
}

void Trk::RectangularSegmentation::createSegmenationSurfaces(std::vector< std::shared_ptr< const Trk::Surface> >& boundarySurfaces,
                                                             std::vector< std::shared_ptr< const Trk::Surface> >& segmentationSurfacesX,
                                                             std::vector< std::shared_ptr< const Trk::Surface> >& segmentationSurfacesY,
                                                             double halfThickness,
                                                             int readoutDirection, 
                                                             double lorentzAngle) const
{
    // may be needed throughout
    double lorentzAngleTan    =  tan(lorentzAngle);
    double lorentzPlaneShiftX = halfThickness*lorentzAngleTan;
    
    // (A) --- top/bottom surfaces -----------------------------------------------------------
    // let's create the top/botten surfaces first - we call them readout / counter readout
    // there are some things to consider 
    // - they share the RectangleBounds only if the lorentzAngle is 0, otherwise only the readout surface has full length bounds like the module 
    Trk::SharedObject<const Trk::SurfaceBounds> moduleBounds(new Trk::RectangleBounds(m_activeBounds->halflengthX(),m_activeBounds->halflengthY()));
    // - they are separated by half a thickness in z
    Amg::Transform3D* readoutPlaneTransform    = new Amg::Transform3D(Amg::Transform3D::Identity());
    Amg::Transform3D* counterPlaneTransform    = new Amg::Transform3D(Amg::Transform3D::Identity());
    // readout and counter readout bounds, the bounds of the readout plane are like the active ones
    Trk::SharedObject<const Trk::SurfaceBounds> readoutPlaneBounds = moduleBounds;
    Trk::SharedObject<const Trk::SurfaceBounds> counterPlaneBounds(nullptr);
    // the transform of the readout plane is always centric
    (*readoutPlaneTransform).translation()     = Amg::Vector3D(0.,0.,readoutDirection*halfThickness);
    // no lorentz angle and everything is straight-forward
    if (lorentzAngle == 0.){
        counterPlaneBounds = moduleBounds;
        (*counterPlaneTransform).translation()     = Amg::Vector3D(0.,0.,-readoutDirection*halfThickness);
    } else {
        // lorentz reduced Bounds 
        double lorentzReducedHalfX = m_activeBounds->halflengthX() - fabs(lorentzPlaneShiftX);
        Trk::SharedObject<const Trk::SurfaceBounds> lorentzReducedBounds(new Trk::RectangleBounds(lorentzReducedHalfX,m_activeBounds->halflengthY()));
        counterPlaneBounds  = lorentzReducedBounds;
        // now we shift the counter plane in position - this depends on lorentz angle
        double counterPlaneShift = -readoutDirection*lorentzPlaneShiftX;
        (*counterPlaneTransform).translation() = Amg::Vector3D(counterPlaneShift,0.,-readoutDirection*halfThickness); 
    }
    // - build the readout & counter readout surfaces
    boundarySurfaces.push_back(std::shared_ptr<const Trk::PlaneSurface>(new Trk::PlaneSurface(readoutPlaneTransform,readoutPlaneBounds)));
    boundarySurfaces.push_back(std::shared_ptr<const Trk::PlaneSurface>(new Trk::PlaneSurface(counterPlaneTransform,counterPlaneBounds)));
    
    // (B) - bin X and lorentz surfaces -----------------------------------------------------------
    // easy stuff first, constant pitch size and 
    double pitchX             =  2.*m_activeBounds->halflengthX()/m_binsX;

    // now, let's create the SharedBounds of all surfaces marking x bins - choice fixes orientation of the matrix
    Trk::SharedObject<const Trk::SurfaceBounds> xBinBounds(new Trk::RectangleBounds(m_activeBounds->halflengthY(),halfThickness));
    // now, let's create the SharedBounds of all surfaces marking lorentz planes
    double lorentzPlaneHalfX  = fabs(halfThickness/cos(lorentzAngle));
    // teh bounds of the lorentz plane
    Trk::SharedObject<const Trk::SurfaceBounds> lorentzPlaneBounds = (lorentzAngle==0.) ? xBinBounds :
             Trk::SharedObject<const Trk::SurfaceBounds>(new Trk::RectangleBounds(m_activeBounds->halflengthY(),lorentzPlaneHalfX));

    // now the rotation matrix for the xBins
    Amg::RotationMatrix3D xBinRotationMatrix;
    xBinRotationMatrix.col(0) = Amg::Vector3D::UnitY();
    xBinRotationMatrix.col(1) = Amg::Vector3D::UnitZ();
    xBinRotationMatrix.col(2) = Amg::Vector3D::UnitX();
    // now the lorentz plane rotation should be the xBin rotation, rotated by the lorentz angle around y
    Amg::RotationMatrix3D lorentzPlaneRotationMatrix =  (lorentzAngle !=0.) ? 
        xBinRotationMatrix * Amg::AngleAxis3D(lorentzAngle, Amg::Vector3D::UnitX()) : xBinRotationMatrix; 

    // reserve, it's always (number of bins-1) as the boundaries are within the boundarySurfaces
    segmentationSurfacesX.reserve(m_binsX);
    for (size_t ibinx = 0; ibinx <= m_binsX; ++ibinx){
           // the current step x position
           double cPosX = -m_activeBounds->halflengthX()+ibinx*pitchX;
           // (i) this is the low/high boundary --- ( ibin == 0/m_binsX )
           if (!ibinx || ibinx == m_binsX){
               // check if it a straight boundary or not: always straight for no lorentz angle, and either the first boundary or the last dependening on lorentz & readout
               bool boundaryStraight = (lorentzAngle == 0. || (!ibinx && readoutDirection*lorentzAngle > 0.) || (ibinx==m_binsX && readoutDirection*lorentzAngle < 0));
               // set the low boundary parameters : position & rotation 
               Amg::Vector3D boundaryXPosition = boundaryStraight  ? Amg::Vector3D(cPosX, 0.,0.) : Amg::Vector3D(cPosX-readoutDirection*lorentzPlaneShiftX, 0., 0.);
               const Amg::RotationMatrix3D& boundaryXRotation = boundaryStraight ? xBinRotationMatrix : lorentzPlaneRotationMatrix;
               // build the rotation from it
               Amg::Transform3D*  boundaryXTransform = new Amg::Transform3D(Amg::getTransformFromRotTransl(boundaryXRotation, boundaryXPosition));
               // the correct bounds for this
               Trk::SharedObject<const Trk::SurfaceBounds> boundaryXBounds = boundaryStraight ? xBinBounds : lorentzPlaneBounds;
               // boundary surfaces 
               boundarySurfaces.push_back(std::shared_ptr<const Trk::PlaneSurface>(new Trk::PlaneSurface(boundaryXTransform,boundaryXBounds)));
           // (ii) this is the in between bins  --- ( 1 <= ibin < m_mbnsX ) 
           } else {
               // shift by the lorentz angle 
               Amg::Vector3D lorentzPlanePosition(cPosX-readoutDirection*lorentzPlaneShiftX, 0., 0.);
               Amg::Transform3D* lorentzPlaneTransform = new Amg::Transform3D(Amg::getTransformFromRotTransl(lorentzPlaneRotationMatrix,lorentzPlanePosition));
               // lorentz plane surfaces 
               segmentationSurfacesX.push_back(std::shared_ptr<const Trk::PlaneSurface>(new Trk::PlaneSurface(lorentzPlaneTransform,lorentzPlaneBounds)));
           }
    }
    
    // (C) - bin Y surfaces - everything is defined -----------------------------------------------------------
    // now the rotation matrix for the yBins - anticyclic
    Amg::RotationMatrix3D yBinRotationMatrix;
    yBinRotationMatrix.col(0) = Amg::Vector3D::UnitX();
    yBinRotationMatrix.col(1) = Amg::Vector3D::UnitZ();
    yBinRotationMatrix.col(2) = Amg::Vector3D(0.,-1.,0.);
    // easy stuff first, constant pitch in Y 
    double pitchY             =  2.*m_activeBounds->halflengthY()/m_binsY;
    // let's create the SharedBounds of all surfaces marking y bins
    Trk::SharedObject<const Trk::SurfaceBounds> yBinBounds(new Trk::RectangleBounds(m_activeBounds->halflengthX(),halfThickness));
    // reserve, it's always (number of bins-1) as the boundaries are within the boundarySurfaces
    segmentationSurfacesY.reserve(m_binsY);
    for (size_t ibiny = 0; ibiny <= m_binsY; ++ibiny){
        // the position of the bin surface
        double binPosY = -m_activeBounds->halflengthY()+ibiny*pitchY;
        Amg::Vector3D binSurfaceCenter(0.,binPosY,0.);
        Amg::Transform3D* binTransform = new Amg::Transform3D(Amg::getTransformFromRotTransl(yBinRotationMatrix,binSurfaceCenter));
        // these are the boundaries
        if (ibiny == 0 || ibiny == m_binsY)
            boundarySurfaces.push_back(std::shared_ptr<Trk::PlaneSurface>(new Trk::PlaneSurface(binTransform,yBinBounds)));
        else // these are the bin boundaries
            segmentationSurfacesY.push_back(std::shared_ptr<Trk::PlaneSurface>(new Trk::PlaneSurface(binTransform,yBinBounds)));
    }  
}


const Amg::Vector2D Trk::RectangularSegmentation::cellPosition(const DigitizationCell& dCell) const 
{

    // use the bin utility for this job
    double bX = m_binsX ? m_binUtility->binPosition(dCell.first,0.,0) : 0.;
    double bY = m_binsY ? m_binUtility->binPosition(dCell.second,0.,1) : 0.;
    return Amg::Vector2D(bX,bY);
}


/** Get the digitization cell from 3D position, it used the projection to the readout surface to estimate the 2D positon */
const Trk::DigitizationStep Trk::RectangularSegmentation::digitizationStep(const Amg::Vector3D& startStep,
                                                                           const Amg::Vector3D& endStep,
                                                                           double halfThickness,
                                                                           int readoutDirection, 
                                                                           double lorentzAngle) const
{
   
    Amg::Vector3D stepCenter = 0.5*(startStep+endStep);
    // project to parameter surface
    double lorentzDeltaX = -readoutDirection*stepCenter.z()*tan(lorentzAngle);
    // take the full drift length
    double driftInZ = (halfThickness-readoutDirection*stepCenter.z());
    double driftLength  = fabs(driftInZ/cos(lorentzAngle)); 
    // the projected center
    Amg::Vector2D stepCenterProjected(stepCenter.x()+lorentzDeltaX,stepCenter.y());
    // the cell & its center
    Trk::DigitizationCell dCell = cell(stepCenterProjected);
    Amg::Vector2D cellCenter = cellPosition(dCell);
    // we are ready to return what we have
    return Trk::DigitizationStep((endStep-startStep).mag(),driftLength,dCell,startStep,endStep,stepCenterProjected,cellCenter);   
}



                                                    

