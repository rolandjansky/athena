//////////////////////////////////////////////////////////////////
// TrapezoidSegmentation.cxx, ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk includes
#include "TrkDigEvent/TrapezoidSegmentation.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
// Amg includes
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

Trk::TrapezoidSegmentation::TrapezoidSegmentation(std::shared_ptr<const Trk::TrapezoidBounds> mBounds,
                                                      size_t numCellsX, size_t numCellsY) :
   m_activeBounds(mBounds),
   m_binUtility(nullptr),
   m_binsX(numCellsX),
   m_binsY(numCellsY)
{
    // first the x dimension if needed
    if (numCellsX > 1) {
         m_binUtility = new Trk::BinUtility(numCellsX, -0.5*(mBounds->minHalflengthX()+mBounds->maxHalflengthX()), 0.5*(mBounds->minHalflengthX()+mBounds->maxHalflengthX()), Trk::open, Trk::binX);
    }
    // use y dimension if needed
    if (numCellsY > 1){
        Trk::BinUtility yBinUtility(numCellsY, -mBounds->halflengthY(), mBounds->halflengthY(), Trk::open, Trk::binY);
        if (m_binUtility)
            (*m_binUtility) += yBinUtility;
        else 
            m_binUtility = new Trk::BinUtility(yBinUtility);
    }           
}       

Trk::TrapezoidSegmentation::~TrapezoidSegmentation()
{
    delete m_binUtility;
}

void Trk::TrapezoidSegmentation::createSegmenationSurfaces(std::vector< std::shared_ptr< const Trk::Surface> >& boundarySurfaces,
                                                             std::vector< std::shared_ptr< const Trk::Surface> >& segmentationSurfacesX,
                                                             std::vector< std::shared_ptr< const Trk::Surface> >& segmentationSurfacesY,
                                                             double halfThickness,
                                                             int readoutDirection, 
                                                             double) const
{
    // The Lorentz angle is not taken into account for trapezoidal segmentation
    // (A) --- top/bottom surfaces -----------------------------------------------------------
    // let's create the top/botten surfaces first - we call them readout / counter readout
    // there are some things to consider 
    // - they share only the readout surface, then the segmentation surfaces are tilted and cannot be shared on the same module
    Trk::SharedObject<const Trk::SurfaceBounds> moduleBounds(new Trk::TrapezoidBounds(m_activeBounds->minHalflengthX(),m_activeBounds->maxHalflengthX(),m_activeBounds->halflengthY()));
    // - they are separated by half a thickness in z
    Amg::Transform3D* readoutPlaneTransform    = new Amg::Transform3D(Amg::Transform3D::Identity());
    Amg::Transform3D* counterPlaneTransform    = new Amg::Transform3D(Amg::Transform3D::Identity());
    // readout and counter readout bounds, the bounds of the readout plane are like the active ones
    Trk::SharedObject<const Trk::SurfaceBounds> readoutPlaneBounds = moduleBounds;
    Trk::SharedObject<const Trk::SurfaceBounds> counterPlaneBounds(nullptr);
    // the transform of the readout plane is always centric
    (*readoutPlaneTransform).translation()     = Amg::Vector3D(0.,0.,readoutDirection*halfThickness);
    // no lorentz angle and everything is straight-forward
    counterPlaneBounds = moduleBounds;
    (*counterPlaneTransform).translation()     = Amg::Vector3D(0.,0.,-readoutDirection*halfThickness);
    
    // - build the readout & counter readout surfaces
    boundarySurfaces.push_back(std::shared_ptr<const Trk::PlaneSurface>(new Trk::PlaneSurface(readoutPlaneTransform,readoutPlaneBounds)));
    boundarySurfaces.push_back(std::shared_ptr<const Trk::PlaneSurface>(new Trk::PlaneSurface(counterPlaneTransform,counterPlaneBounds)));
    
    // (B) - bin X -----------------------------------------------------------
    // easy stuff first, constant pitch size and 
    double pitchX             =  2.*(m_activeBounds->maxHalflengthX()+m_activeBounds->minHalflengthX())*0.5/m_binsX;

    // now the rotation matrix for the xBins
    Amg::RotationMatrix3D xBinRotationMatrix;
    xBinRotationMatrix.col(0) = Amg::Vector3D::UnitY();
    xBinRotationMatrix.col(1) = Amg::Vector3D::UnitZ();
    xBinRotationMatrix.col(2) = Amg::Vector3D::UnitX();
    
    // reserve, it's always (number of bins-1) as the boundaries are within the boundarySurfaces
    segmentationSurfacesX.reserve(m_binsX);
    for (size_t ibinx = 0; ibinx <= m_binsX; ++ibinx){
      // the current step x position
      double cPosX = -(m_activeBounds->minHalflengthX()+m_activeBounds->maxHalflengthX())*0.5+ibinx*pitchX;
      
      // set position & rotation for all (boundaries and segmentations) --> Then you separate between them
      Amg::Vector3D xPosition = Amg::Vector3D(cPosX, 0.,0.);
      double stereoLocal = asin(sinStereoLocal(Amg::Vector2D(cPosX, 0.)));
      const Amg::RotationMatrix3D xRotation = xBinRotationMatrix*Amg::AngleAxis3D(stereoLocal, Amg::Vector3D::UnitY()); 
      // build the rotation from it
      Amg::Transform3D*  binTransform = new Amg::Transform3D(Amg::getTransformFromRotTransl(xRotation, xPosition));
      // the correct bounds for this
      Trk::RectangleBounds* xBinBounds = new Trk::RectangleBounds(m_activeBounds->halflengthY()/cos(stereoLocal),halfThickness);
      // these are the boundaries
      if (ibinx==0 || ibinx == m_binsX) // (i) this is the low/high boundary --- ( ibin == 0/m_binsX )
	boundarySurfaces.push_back(std::shared_ptr<const Trk::PlaneSurface>(new Trk::PlaneSurface(binTransform,xBinBounds)));
      else // these are the bin boundaries
	segmentationSurfacesX.push_back(std::shared_ptr<const Trk::PlaneSurface>(new Trk::PlaneSurface(binTransform,xBinBounds)));
    }
    
    // (C) - bin Y surfaces - everything is defined -----------------------------------------------------------
    // now the rotation matrix for the yBins - anticyclic
    Amg::RotationMatrix3D yBinRotationMatrix;
    yBinRotationMatrix.col(0) = Amg::Vector3D::UnitX();
    yBinRotationMatrix.col(1) = Amg::Vector3D::UnitZ();
    yBinRotationMatrix.col(2) = Amg::Vector3D(0.,-1.,0.);
    // easy stuff first, constant pitch in Y 
    double pitchY             =  2.*m_activeBounds->halflengthY()/m_binsY;
    // reserve, it's always (number of bins-1) as the boundaries are within the boundarySurfaces
    segmentationSurfacesY.reserve(m_binsY);
    for (size_t ibiny = 0; ibiny <= m_binsY; ++ibiny){
        // the position of the bin surface
        double binPosY = -m_activeBounds->halflengthY()+ibiny*pitchY;
        Amg::Vector3D binSurfaceCenter(0.,binPosY,0.);
	double localPitchX = PitchX(Amg::Vector2D(0., binPosY));
	Trk::RectangleBounds* yBinBounds = new Trk::RectangleBounds(localPitchX*m_binsX*0.5,halfThickness);
        Amg::Transform3D* binTransform = new Amg::Transform3D(Amg::getTransformFromRotTransl(yBinRotationMatrix,binSurfaceCenter));
        // these are the boundaries
        if (ibiny == 0 || ibiny == m_binsY)
            boundarySurfaces.push_back(std::shared_ptr<Trk::PlaneSurface>(new Trk::PlaneSurface(binTransform,yBinBounds)));
        else // these are the bin boundaries
            segmentationSurfacesY.push_back(std::shared_ptr<Trk::PlaneSurface>(new Trk::PlaneSurface(binTransform,yBinBounds)));
    }  
}


const Amg::Vector2D Trk::TrapezoidSegmentation::cellPosition(const DigitizationCell& dCell) const 
{
    // use the bin utility for this job
    double bX = (m_binsX>1) ? m_binUtility->binPosition(projectLocX(Amg::Vector2D(dCell.first, dCell.second)),0.,0) : 0.;
    double bY = (m_binsY>1) ? m_binUtility->binPosition(dCell.second,0.,1) : 0.;
    return Amg::Vector2D(bX,bY);
}


/** Get the digitization cell from 3D position, it used the projection to the readout surface to estimate the 2D positon */
const Trk::DigitizationStep Trk::TrapezoidSegmentation::digitizationStep(const Amg::Vector3D& startStep,
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



                                                    

