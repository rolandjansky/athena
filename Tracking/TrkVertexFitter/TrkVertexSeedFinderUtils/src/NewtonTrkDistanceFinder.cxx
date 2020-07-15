/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
    NewtonTrkDistanceFinder.cxx - Description in header file
*********************************************************************/

//#define TrkDistance_DEBUG

#include "GaudiKernel/EventContext.h"

#include "TrkVertexSeedFinderUtils/NewtonTrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"

#include "MagFieldElements/AtlasFieldCache.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include <cmath>



namespace {
  inline double getRadiusOfCurvature(const Trk::Perigee & myPerigee,const double Bzfield) {
    return sin(myPerigee.parameters()[Trk::theta])/(Bzfield*myPerigee.parameters()[Trk::qOverP]);
  }
}

namespace Trk
{
  NewtonTrkDistanceFinder::NewtonTrkDistanceFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_precision(1e-8),
    m_maxloopnumber(20)
  {   
    declareProperty("Precision",m_precision);
    declareProperty("MaxLoops",m_maxloopnumber);
    declareInterface<NewtonTrkDistanceFinder>(this);
  }

  NewtonTrkDistanceFinder::~NewtonTrkDistanceFinder() = default;

  StatusCode NewtonTrkDistanceFinder::initialize() 
  { 
    ATH_CHECK( AlgTool::initialize() );
    ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );
    ATH_MSG_DEBUG( "Initialize successful" );
    return StatusCode::SUCCESS;
  }
  StatusCode NewtonTrkDistanceFinder::finalize() 
  {
    ATH_MSG_DEBUG( "Finalize successful" );
    return StatusCode::SUCCESS;
  }

TwoPoints
NewtonTrkDistanceFinder::GetClosestPoints (const PointOnTrack & firsttrack,
                                           const PointOnTrack & secondtrack) const
{
  //Now the direction of momentum at point of closest approach (but only direction, not versus)
  const double a_phi0    = firsttrack.getPerigee().parameters()[Trk::phi0];
  const double a_cosphi0 = -sin(a_phi0);//do i need it?
  const double a_sinphi0 =  cos(a_phi0);//~?
  
  //Now initialize the variable you need to go on
  const double a_x0=firsttrack.getPerigee().associatedSurface().center().x() +
    firsttrack.getPerigee().parameters()[Trk::d0]*a_cosphi0;
  const double a_y0=firsttrack.getPerigee().associatedSurface().center().y() +
    firsttrack.getPerigee().parameters()[Trk::d0]*a_sinphi0;
  const double a_z0=firsttrack.getPerigee().associatedSurface().center().z() +
    firsttrack.getPerigee().parameters()[Trk::z0];

#ifdef TrkDistance_DEBUG
  ATH_MSG_DEBUG( "a_x0 " << a_x0 << " a_y0 " << a_y0 << " a_z0 " << a_z0 );
  ATH_MSG_DEBUG( "m_a_phi0 " << a_phi0 );
#endif

  // Setup magnetic field retrieval
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, Gaudi::Hive::currentContext()};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

  MagField::AtlasFieldCache fieldCache;
  fieldCondObj->getInitializedCache (fieldCache);

  double magnFieldVect[3];
  double posXYZ[3];
  posXYZ[0] = firsttrack.getPerigee().associatedSurface().center().x();
  posXYZ[1] = firsttrack.getPerigee().associatedSurface().center().y();
  posXYZ[2] = firsttrack.getPerigee().associatedSurface().center().z();
  fieldCache.getField(posXYZ,magnFieldVect);

  
  //Magnetic field at (x0,y0,z0)
  const double a_Bz=magnFieldVect[2]*299.792;//B field in Gev/mm
  //EvaluateMagneticField(a_x0,b_y0,b_z0);  

  const double a_Rt = getRadiusOfCurvature(firsttrack.getPerigee(),a_Bz);
  const double a_cotantheta = 1./tan(firsttrack.getPerigee().parameters()[Trk::theta]);
  
#ifdef TrkDistance_DEBUG
  ATH_MSG_DEBUG( "a_Rt" << a_Rt << " a_cotantheta " << a_cotantheta );
  ATH_MSG_DEBUG( "Magnetic field at perigee is " << a_Bz << "GeV/mm " );
#endif

  //Now the direction of momentum at point of closest approach (but only direction, not versus)
  const double b_phi0    =  secondtrack.getPerigee().parameters()[Trk::phi0];
  const double b_cosphi0 = -sin(b_phi0);//do i need it?
  const double b_sinphi0 =  cos(b_phi0);//~?
  
  //Now initialize the variable you need to go on
  const double b_x0=secondtrack.getPerigee().associatedSurface().center().x() +
    secondtrack.getPerigee().parameters()[Trk::d0]*b_cosphi0;
  const double b_y0=secondtrack.getPerigee().associatedSurface().center().y() +
    secondtrack.getPerigee().parameters()[Trk::d0]*b_sinphi0;
  const double b_z0=secondtrack.getPerigee().associatedSurface().center().z() +
    secondtrack.getPerigee().parameters()[Trk::z0];
  
#ifdef TrkDistance_DEBUG
  ATH_MSG_DEBUG( "b_x0 " << b_x0 << " b_y0 " << b_y0 << " b_z0 " << b_z0 );
  ATH_MSG_DEBUG( "b_phi0 " << b_phi0 );
#endif


  posXYZ[0] = secondtrack.getPerigee().associatedSurface().center().x();
  posXYZ[1] = secondtrack.getPerigee().associatedSurface().center().y();
  posXYZ[2] = secondtrack.getPerigee().associatedSurface().center().z();  
  fieldCache.getField(posXYZ,magnFieldVect);
  
  //Magnetic field at (x0,y0,z0)
  const double b_Bz = magnFieldVect[2]*299.792;//B field in Gev/mm - for the moment use a constant field offline
  //use the right value expressed in GeV
  //EvaluateMagneticField(b_x0,b_y0,b_z0);

  const double b_Rt = getRadiusOfCurvature(secondtrack.getPerigee(),b_Bz);
  const double b_cotantheta = 1./tan(secondtrack.getPerigee().parameters()[Trk::theta]);
  
#ifdef TrkDistance_DEBUG
  ATH_MSG_DEBUG( "b_Rt" << b_Rt << " b_cotantheta " << b_cotantheta );
  ATH_MSG_DEBUG( "Magnetic field at perigee is " << b_Bz << " GeV/mm " );
#endif


  //Now prepare some more elaborate pieces for later
  const double ab_Dx0 = a_x0-b_x0-a_Rt*a_cosphi0+b_Rt*b_cosphi0;
  const double ab_Dy0 = a_y0-b_y0-a_Rt*a_sinphi0+b_Rt*b_sinphi0;
  const double ab_Dz0 = a_z0-b_z0+a_Rt*a_cotantheta*a_phi0-b_Rt*b_cotantheta*b_phi0;

#ifdef TrkDistance_DEBUG
  ATH_MSG_DEBUG( "ab_Dx0 " << ab_Dx0 << " ab_Dy0 " << ab_Dy0 << " ab_Dz0 " << ab_Dz0 );
#endif
 

  //Prepare the initial point that can be different from point of closest approach
  //If you don't specify any point the default will be the point of closest approach!!
  //Another subroutine will be implemented if you want to use 
  //a certain seed
  double a_phi = firsttrack.getPhiPoint();//this has to be corrected as soon as you adjust the Trk2dDistanceSeeder...
  double b_phi = secondtrack.getPhiPoint();

  //store cos and sin of phi
  double a_cosphi = -sin(a_phi);
  double a_sinphi =  cos(a_phi);
  double b_cosphi = -sin(b_phi);
  double b_sinphi =  cos(b_phi);
  

#ifdef TrkDistance_DEBUG
  ATH_MSG_DEBUG( "Beginning phi is a_phi: " << a_phi << " b_phi " << b_phi );
  ATH_MSG_DEBUG( "LOOP number 0" );
#endif


  int loopsnumber = 0;
  
  bool isok=false;

  while (!isok) {

#ifdef TrkDistance_DEBUG
    ATH_MSG_DEBUG( "Entered LOOP number: " << loopsnumber );
    ATH_MSG_DEBUG( "actual value of a_phi: " << a_phi << " of b_phi " << b_phi );
#endif
    
    
    //count the loop number
    ++loopsnumber;
    
    
#ifdef TrkDistance_DEBUG
    ATH_MSG_DEBUG( "First point x: " << GetClosestPoints().first.x()  
                   << "y: " << GetClosestPoints().first.y()  
                   << "z: " << GetClosestPoints().first.z() );
    ATH_MSG_DEBUG( << "Second point x: " << GetClosestPoints().second.x()  
                   << "y: " << GetClosestPoints().second.y()  
                   << "z: " << GetClosestPoints().second.z() );
    
    ATH_MSG_DEBUG( "ActualDistance: " << GetDistance() );
    ATH_MSG_DEBUG( "real Dx0 " << ab_Dx0+a_Rt*a_cosphi-b_Rt*b_cosphi );
    ATH_MSG_DEBUG( "real Dy0 " << ab_Dy0+a_Rt*a_sinphi-b_Rt*b_sinphi );
#endif
    
    //I remove the factor two from the formula
    const double d1da_phi =
      (ab_Dx0-b_Rt*b_cosphi)*(-a_Rt*a_sinphi)+
      (ab_Dy0-b_Rt*b_sinphi)*a_cosphi*a_Rt+
      (ab_Dz0-a_Rt*a_cotantheta*a_phi+b_Rt*b_cotantheta*b_phi)*(-a_Rt*a_cotantheta);
    
    
    //same for second deriv respective to phi
    const double d1db_phi =
      (ab_Dx0+a_Rt*a_cosphi)*b_Rt*b_sinphi-//attention!MINUS here
      (ab_Dy0+a_Rt*a_sinphi)*b_cosphi*b_Rt+
      (ab_Dz0-a_Rt*a_cotantheta*a_phi+b_Rt*b_cotantheta*b_phi)*(+b_Rt*b_cotantheta);
    
    //second derivatives (d^2/d^2(a) d^2/d^2(b) d^2/d(a)d(b) )

    const double d2da_phi2 =
      (ab_Dx0-b_Rt*b_cosphi)*(-a_Rt*a_cosphi)+
      (ab_Dy0-b_Rt*b_sinphi)*(-a_Rt*a_sinphi)+
      +a_Rt*a_Rt*(a_cotantheta*a_cotantheta);

    const double d2db_phi2 =
      (ab_Dx0+a_Rt*a_cosphi)*(+b_Rt*b_cosphi)+
      (ab_Dy0+a_Rt*a_sinphi)*(+b_Rt*b_sinphi)+
      +b_Rt*b_Rt*(b_cotantheta*b_cotantheta);


    const double d2da_phib_phi = -a_Rt*b_Rt*(a_sinphi*b_sinphi+a_cosphi*b_cosphi+a_cotantheta*b_cotantheta);

    //Calculate the determinant of the Jacobian

    const double det = d2da_phi2*d2db_phi2-d2da_phib_phi*d2da_phib_phi;


#ifdef TrkDistance_DEBUG
    ATH_MSG_DEBUG( "d1da_phi " << d1da_phi << " d1db_phi " << d1db_phi << " d2da_phi2 " << d2da_phi2 << " d2db_phi2 " << d2db_phi2 
                   << " d2da_phib_phi " << d2da_phib_phi << " det " << det );
#endif
    
    //if the quadratic form is defined negative or is semidefined, throw the event
    //(you are in a maximum or in a saddle point)
    if (det<0) {
      ATH_MSG_DEBUG( "Hessian is negative: saddle point" );
      throw Error::NewtonProblem("Hessian is negative");
    }
    if (det>0&&d2da_phi2<0) {
      ATH_MSG_DEBUG( "Hessian indicates a maximum: derivative will be zero but result incorrect" );
      throw Error::NewtonProblem("Maximum point found");
    }

    //Now apply the Newton formula in more than one dimension
    const double deltaa_phi = -(d2db_phi2*d1da_phi-d2da_phib_phi*d1db_phi)/det;
    const double deltab_phi = -(-d2da_phib_phi*d1da_phi+d2da_phi2*d1db_phi)/det;

#ifdef TrkDistance_DEBUG
    ATH_MSG_DEBUG( "deltaa_phi: " << deltaa_phi );
    ATH_MSG_DEBUG( "deltab_phi: " << deltab_phi );
#endif


    a_phi += deltaa_phi;
    b_phi += deltab_phi;

    //store cos and sin of phi
    a_cosphi = -sin(a_phi);
    a_sinphi =  cos(a_phi);
    b_cosphi = -sin(b_phi);
    b_sinphi =  cos(b_phi);
    
    if (std::sqrt(std::pow(deltaa_phi,2)+std::pow(deltab_phi,2))<m_precision || 
	loopsnumber>m_maxloopnumber) isok=true;
    
  }
	
  if (loopsnumber>m_maxloopnumber) throw Error::NewtonProblem("Could not find minimum distance: max loops number reached"); //now return error, see how to do it...
  

  return TwoPoints(Amg::Vector3D(a_x0+a_Rt*(a_cosphi-a_cosphi0),
				 a_y0+a_Rt*(a_sinphi-a_sinphi0),
				 a_z0-a_Rt*(a_phi-a_phi0)*a_cotantheta),
		   Amg::Vector3D(b_x0+b_Rt*(b_cosphi-b_cosphi0),
				 b_y0+b_Rt*(b_sinphi-b_sinphi0),
				 b_z0-b_Rt*(b_phi-b_phi0)*b_cotantheta));
}


} // namespace Trk
