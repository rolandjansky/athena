/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RandomSurfaceBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk include
#include "TrkDetDescrTestTools/RandomSurfaceBuilder.h"
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

// constructor
Trk::RandomSurfaceBuilder::RandomSurfaceBuilder(const std::string& t, const std::string& n, const IInterface* p)
: AthAlgTool(t,n,p),    
  m_rndmSvc("RndmGenSvc", n),
  m_gaussDist(0),
  m_flatDist(0),
  m_numberOfSurfaces(100),
  m_enableCones(false)
{
   
    declareInterface<ISurfaceBuilder>(this);
   
    // the active dimensions
    declareProperty("NumberOfSurfaces", m_numberOfSurfaces);
    declareProperty("WorldDimensions",  m_worldDimensions);
}

// destructor
Trk::RandomSurfaceBuilder::~RandomSurfaceBuilder()
{
    delete m_gaussDist;
    delete m_flatDist;
}


// Athena standard methods
// initialize
StatusCode Trk::RandomSurfaceBuilder::initialize()
{
   
   // Random number service
   if ( m_rndmSvc.retrieve().isFailure() ) {
     ATH_MSG_FATAL( "Could not retrieve " << m_rndmSvc );
     return StatusCode::FAILURE;
   }
   
   // intialize the random number generators
   m_gaussDist = new Rndm::Numbers(&*m_rndmSvc, Rndm::Gauss(0.,1.));
   m_flatDist  = new Rndm::Numbers(&*m_rndmSvc, Rndm::Flat(0.,1.));
   
   return StatusCode::SUCCESS;
}

// finalize
StatusCode Trk::RandomSurfaceBuilder::finalize()
{
    ATH_MSG_INFO( "finalize() successful." );
    return StatusCode::SUCCESS;
}


const std::vector<const Trk::Surface*>* Trk::RandomSurfaceBuilder::surfaces() const
{
    std::vector<const Trk::Surface*>* cSurfaces = new std::vector<const Trk::Surface*>();
    cSurfaces->reserve(m_numberOfSurfaces);
    for ( size_t isf = 0; isf < m_numberOfSurfaces; ++isf ){
        const Trk::Surface* csf = surface();
        if (csf) cSurfaces->push_back(csf);
    }
    // check how many surfaces had been created
    if ( cSurfaces->size() != m_numberOfSurfaces )
        ATH_MSG_WARNING("Number of created surfaces (" << cSurfaces->size() 
                    <<  ") does not correspond to chosen number (" << m_numberOfSurfaces << ")." );
    return cSurfaces;
    
}


const Trk::Surface* Trk::RandomSurfaceBuilder::surface() const
{
    if (m_worldDimensions.size() < 3 ) return 0;
    
    const Trk::Surface* surface = 0;
    int sType = std::floor(m_flatDist->shoot()*6);
        
    // neglect 0 if you don't do cones
    if (!m_enableCones && !sType){
        while (!sType)
         sType = std::floor(m_flatDist->shoot()*6);   
    }
    
    sType = sType == 6 ? 5 : sType;
    // create the transform parameters
    double tx = m_worldDimensions[0]*(2*m_flatDist->shoot()-1);
    double ty = m_worldDimensions[1]*(2*m_flatDist->shoot()-1);
    double tz = m_worldDimensions[2]*(2*m_flatDist->shoot()-1);
    double dx = m_flatDist->shoot();
    double dy = m_flatDist->shoot();
    double dz = m_flatDist->shoot();
    
     std::unique_ptr<Amg::Transform3D> transform( new Amg::Transform3D );
     // the direciton and curvilinear UVT frame
     Amg::Vector3D direction(dx,dy,dz);
     
     Trk::CurvilinearUVT curvUVT(direction.normalized());
     // translation
     Amg::Translation3D translation(tx, ty, tz);
     
     // create the rotation 
     Amg::RotationMatrix3D rotation;     
     rotation.col(0) = curvUVT.curvU();
     rotation.col(1) = curvUVT.curvV();
     rotation.col(2) = curvUVT.curvT();
     // curvilinear surfaces are boundless
     (*transform)  = translation;
     (*transform).rotate(rotation);
     ATH_MSG_VERBOSE("Created transform is : " << Amg::toString(*transform));
     
     // create the surface
     switch (sType) {
         // create a cone surface - cone does not exist for old EDM as Measured
         case 0 :  {   
             surface = new Trk::ConeSurface(transform.release(), m_flatDist->shoot()*0.7*M_PI); 
         } break;
         case 1 : {   
             surface = new Trk::CylinderSurface(transform.release(), 50.+m_worldDimensions[0]*m_flatDist->shoot(), m_worldDimensions[2]);
         } break;
         // create a disc surface
         case 2 : {   
             surface = new Trk::DiscSurface(transform.release(), 0., m_worldDimensions[1]);
         } break;
         // create a perigee surface
         case 3 : {   
             surface = new Trk::PerigeeSurface(Amg::Vector3D(tx,ty,tz));
         } break;
         // create a plane surface
         case 4 : {
             surface = new Trk::PlaneSurface(transform.release(), m_worldDimensions[0], m_worldDimensions[0]);
         } break;
         // create a straight line surface
         case 5 : {
             surface = new Trk::StraightLineSurface(transform.release(), 50., m_worldDimensions[2]);
         } break;
         default : break;    
     }
  return surface;
}
