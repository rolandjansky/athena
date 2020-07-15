/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          FullPerigeeLinearizedTrackFactory.cxx - Description in header file
*********************************************************************/

#include "TrkVertexFitterUtils/FullLinearizedTrackFactory.h"

#include "VxVertex/LinearizedTrack.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "TrkExInterfaces/IExtrapolator.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include "TrkSurfaces/PerigeeSurface.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include <TMath.h>

namespace Trk
{

  FullLinearizedTrackFactory::FullLinearizedTrackFactory(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),m_extrapolator("Trk::Extrapolator", this)
  {  
    declareProperty("Extrapolator",     m_extrapolator);  
    declareInterface<IVertexLinearizedTrackFactory>(this);    
  }

  FullLinearizedTrackFactory::~FullLinearizedTrackFactory() = default;

  StatusCode FullLinearizedTrackFactory::initialize() 
  { 
    
    ATH_CHECK( m_extrapolator.retrieve() );
    ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );

    msg(MSG::INFO)  << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode FullLinearizedTrackFactory::finalize() 
  {
    ATH_MSG_INFO ("Finalize successful");
    return StatusCode::SUCCESS;
  }

  void FullLinearizedTrackFactory::linearize(VxTrackAtVertex & theTrack,const Amg::Vector3D & linPoint) const 
  {
    if (theTrack.initialPerigee())
      theTrack.setLinTrack(linearizedTrack(theTrack.initialPerigee(),linPoint));
    else
      theTrack.setLinTrack(linearizedTrack(theTrack.initialNeutralPerigee(),linPoint));
  } 

  LinearizedTrack * FullLinearizedTrackFactory::linearizedTrack(const TrackParameters *  trackPars,
                                                                 const Amg::Vector3D& linPoint) const {
    if (!trackPars) return nullptr;
    //perigee surface        
    Amg::Vector3D lp =linPoint;
    const PerigeeSurface perigeeSurface(lp);
    
//Remove matherial changes. Trying to understand where the perigee currently is and
//whether we need to add or remove material during extrapolation.
//Obvious case is the extrapolation form the perigee point: add in the direction 
//opposite to momentum; remove along the momentum.

    const Amg::Vector3D gMomentum  = trackPars->momentum();
    const Amg::Vector3D gDirection = trackPars->position() - lp;
    const double extrapolationDirection = gMomentum.dot( gDirection);
    MaterialUpdateMode mode = (extrapolationDirection < 0) ?
                              Trk::addNoise                : // parameters upstream of vertex
                              Trk::removeNoise             ; // parameters downstream of vertex -> go back

    const TrackParameters* parsAtVertex = 
      m_extrapolator->extrapolate(*trackPars, perigeeSurface, Trk::anyDirection, true, Trk::pion, mode);

    if (dynamic_cast<const Trk::Perigee*>(parsAtVertex)==nullptr ||
        parsAtVertex->covariance()==nullptr ) {
      ATH_MSG_INFO ("Could not extrapolate Perigee to vertex pos: x " << lp.x() << " y " <<
                    lp.y() << " z " << lp.z() << ". Normal if outside ID acceptance ");

      if (dynamic_cast<const Trk::Perigee*>(trackPars) && trackPars->covariance()) {
        if (parsAtVertex) delete parsAtVertex; // in case extrapolation made other parameters
        parsAtVertex = trackPars->clone();
      } else {
        delete parsAtVertex; return nullptr;
      }
    }

    if (parsAtVertex && parsAtVertex->covariance() && parsAtVertex->covariance()->determinant()<=0)
    {
      ATH_MSG_DEBUG ("The track covariance matrix det after extrapolation is: " << parsAtVertex->covariance()->determinant() << 
                       " --> Using non extrapolated track parameters");
      delete parsAtVertex;
      parsAtVertex=trackPars->clone();
    }

    // positions
    AmgVector(5) param = parsAtVertex->parameters();  
    Amg::Vector3D expPoint = parsAtVertex->position();
          
    //phi_v and functions  
    double phi_v = param(Trk::phi);
    double sin_phi_v = sin(phi_v);
    double cos_phi_v = cos(phi_v);
    
    //theta and functions  
    double th = param(Trk::theta);
    double sin_th = sin(th);
    double tan_th = tan(th);

    //q over p  
    double q_ov_p = param(Trk::qOverP);
    int sgn_h = (q_ov_p<0.)? -1:1;
    Amg::Vector3D expMomentum(phi_v, th, q_ov_p);

    // magnetic field  

    SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, Gaudi::Hive::currentContext()};
    const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

    MagField::AtlasFieldCache fieldCache;
    fieldCondObj->getInitializedCache (fieldCache);

    double mField[3];
    fieldCache.getField(expPoint.data(),mField);

    double B_z=mField[2]*299.792;//Magnetic field is returned in kT.
				 //The scaling is a factor of c needed for computing rho.

    // signed radius and rotation variables
    // (if momentum or mag field is absent, the curvature radius is infinite)

    double rho;
    if(mField[2] == 0. || fabs(q_ov_p) <= 1e-15) rho = 1e+15 ;
    else rho =  sin_th / (q_ov_p * B_z);
    
    //  std:: cout<<"calculated rho "<< rho<<std::endl;  
    double X = expPoint(0) - lp.x() + rho*sin_phi_v;
    double Y = expPoint(1) - lp.y() - rho*cos_phi_v;
    double SS = (X * X + Y * Y);
    double S = sqrt(SS);
    
    //calculated parameters at expansion point
    //q_over_p and theta stay constant along trajectory
    AmgVector(5) parAtExpansionPoint; parAtExpansionPoint.setZero();
    parAtExpansionPoint[0] = rho - sgn_h * S;
    
//calculation of phi at expansion point    
    double phiAtEp;
    int sgnY = (Y<0)? -1:1;
    int sgnX = (X<0)? -1:1;  
    double pi = TMath::Pi();//acos(-1.);
    
    if(fabs(X)>fabs(Y)) phiAtEp = sgn_h*sgnX* acos(-sgn_h * Y / S);
    else
    {
     phiAtEp = asin(sgn_h * X / S);    
     if( (sgn_h * sgnY)> 0) phiAtEp =  sgn_h * sgnX * pi - phiAtEp;
    }
    
    parAtExpansionPoint[2] = phiAtEp;   
    parAtExpansionPoint[1] = expPoint(2) - lp.z() + rho*(phi_v - parAtExpansionPoint[2])/tan_th;    
    parAtExpansionPoint[3] = th;
    parAtExpansionPoint[4] = q_ov_p;  
//   std::cout<<"Calculated parameters at expansion point: "<<parAtExpansionPoint<<std::endl;
//   std::cout<<"Difference: "<<predStateParameters-parAtExpansionPoint<<std::endl;
    
    //jacobian elements
    AmgMatrix(5,3) positionJacobian; positionJacobian.setZero();

    //first row    
    positionJacobian(0,0) = -sgn_h * X / S;
    positionJacobian(0,1) = -sgn_h * Y / S;

    //second row    
    positionJacobian(1,0) =  rho * Y / (tan_th * SS); 
    positionJacobian(1,1) = -rho * X / (tan_th * SS);
    positionJacobian(1,2) = 1.;
    
    //third row
    positionJacobian(2,0) = -Y / SS;
    positionJacobian(2,1) =  X / SS;   
//    std::cout<<"My position Jacobian: "<<positionJacobian<<std::endl;
    
    //momentum jacobian and related stuff    
    AmgMatrix(5,3) momentumJacobian; momentumJacobian.setZero();
    double R = X*cos_phi_v + Y * sin_phi_v;
    double Q = X*sin_phi_v - Y * cos_phi_v;
    double d_phi = parAtExpansionPoint[2] - phi_v;
  
    //first row   
    momentumJacobian(0,0) = -sgn_h * rho * R / S ;
    
    double qOvS_red = 1 - sgn_h * Q / S;
    momentumJacobian(0,1) = qOvS_red  * rho / tan_th;
    momentumJacobian(0,2) = - qOvS_red * rho / q_ov_p; 
    
    //second row
    momentumJacobian(1,0) = (1 - rho*Q/SS )*rho/tan_th;
    momentumJacobian(1,1) = (d_phi + rho * R / (SS * tan_th * tan_th) ) * rho;
    momentumJacobian(1,2) = (d_phi - rho * R /SS ) * rho / (q_ov_p*tan_th);
   
    //third row
    momentumJacobian(2,0) =  rho * Q / SS; 
    momentumJacobian(2,1) = -rho * R / (SS*tan_th);
    momentumJacobian(2,2) =  rho * R / (q_ov_p*SS);
    
    //last two rows:
    momentumJacobian(3,1) = 1.;
    momentumJacobian(4,2) = 1.;
//    std::cout<<"My momentum Jacobian "<<momentumJacobian<<std::endl;
    
    AmgVector(5) constantTerm = parAtExpansionPoint - positionJacobian*expPoint - momentumJacobian*expMomentum;
//    std::cout<<"My constant term: "<<constantTerm<<std::endl;
      
    LinearizedTrack* toreturn=new LinearizedTrack(parsAtVertex->parameters(),
                                                  *parsAtVertex->covariance(),
						  lp,
						  positionJacobian,
						  momentumJacobian,
						  expPoint,
						  expMomentum,
						  constantTerm);

    //delete Perigee object created by the Extrapolator
    delete parsAtVertex;
    //return new linearized track
    return toreturn;
  }

  LinearizedTrack * FullLinearizedTrackFactory::linearizedTrack(const NeutralParameters *  neutralPars,
                                                                const Amg::Vector3D& linPoint) const
  { 
    if (!neutralPars) return nullptr;
    Amg::Vector3D lp =linPoint;
    PerigeeSurface perigeeSurface(lp);
 
    //no material effects for neutral particles
    /*
    const Amg::Vector3D gMomentum  = neutralPars->momentum();
    const Amg::Vector3D gDirection = neutralPars->position() - lp;
    const double extrapolationDirection = gMomentum.dot( gDirection);
    MaterialUpdateMode mode = (extrapolationDirection < 0) ?
                               Trk::addNoise                : // parameters upstream of vertex
                               Trk::removeNoise             ; // parameters downstream of vertex -> go back
    */
    const NeutralParameters* parsAtVertex =
      m_extrapolator->extrapolate(*neutralPars, perigeeSurface, Trk::anyDirection, true);

    if (dynamic_cast<const Trk::NeutralPerigee*>(parsAtVertex)==nullptr ||
        parsAtVertex->covariance()==nullptr ) {
      ATH_MSG_INFO ("Could not extrapolate Perigee to vertex pos: x " << lp.x() << " y " <<
                    lp.y() << " z " << lp.z() << ". Should not happen. ");

      if (dynamic_cast<const Trk::NeutralPerigee*>(neutralPars) && neutralPars->covariance()) {
        if (parsAtVertex) delete parsAtVertex; // in case extrapolation made other parameters
        parsAtVertex = neutralPars->clone();
      } else {
        delete parsAtVertex; return nullptr;
      }
    }

    // positions, phi, theta
    AmgVector(5) param = parsAtVertex->parameters();
    Amg::Vector3D expPoint = parsAtVertex->position();

    double phi_v = param(Trk::phi);
    double sin_phi_v = sin(phi_v);
    double cos_phi_v = cos(phi_v);
    double th = param(Trk::theta);
    double tan_th = tan(th);
    double q_ov_p = param(Trk::qOverP);

    //momentum
    Amg::Vector3D expMomentum(phi_v, th, q_ov_p);
    double X = expPoint(0) - lp.x();
    double Y = expPoint(1) - lp.y();
   
    AmgVector(5) parAtExpansionPoint; parAtExpansionPoint.setZero();
    parAtExpansionPoint[0] = Y*cos_phi_v-X*sin_phi_v;
   
   //very easy for a neutral track!
   //phi doesn't change...

    double phiAtEp=phi_v;
    parAtExpansionPoint[2] = phiAtEp;   
    parAtExpansionPoint[1] = expPoint[2] - lp.z() - 1./tan_th*(X*cos_phi_v+Y*sin_phi_v);
    parAtExpansionPoint[3] = th;
    parAtExpansionPoint[4] = q_ov_p;  
   
    //jacobian elements
    AmgMatrix(5,3) positionJacobian; positionJacobian.setZero();

    //first row    
    positionJacobian(0,0) = -sin_phi_v;
    positionJacobian(0,1) = +cos_phi_v;
   
    //second raw    
    positionJacobian(1,0) = -cos_phi_v/tan_th;
    positionJacobian(1,1) = -sin_phi_v/tan_th;
    positionJacobian(1,2) = 1.;
   
//    std::cout<<"My position Jacobian: "<<positionJacobian<<std::endl;
    
    //momentum jacobian and related stuff    
    AmgMatrix(5,3) momentumJacobian; momentumJacobian.setZero();
    momentumJacobian(2,0) = 1.;
    momentumJacobian(3,1) = 1.;
    momentumJacobian(4,2) = 1.;
//    std::cout<<"My momentum Jacobian "<<momentumJacobian<<std::endl;
    
    AmgVector(5) constantTerm = parAtExpansionPoint - positionJacobian*expPoint - momentumJacobian*expMomentum;
//    std::cout<<"My constant term: "<<constantTerm<<std::endl;
      
    LinearizedTrack* toreturn=new LinearizedTrack(parsAtVertex->parameters(),
                                                  *parsAtVertex->covariance(),
						  lp,
						  positionJacobian,
						  momentumJacobian,
						  expPoint,
						  expMomentum,
						  constantTerm);

    //delete MeasuredPerigee object created by the Extrapolator
    delete parsAtVertex;
    //return new linearized track
    return toreturn;
  } 
  
  
}//end of namespace definitions
