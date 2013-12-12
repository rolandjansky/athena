/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          ZScanSeedFinder.cxx - Description in header file
*********************************************************************/

//#define ZSCANSEEDFINDER

#include "TrkVertexSeedFinderTools/ZScanSeedFinder.h"

#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "TrkVertexSeedFinderUtils/IMode1dFinder.h"

#include "TrkParameters/TrackParameters.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/ParamDefs.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

//#include <math.h>

namespace Trk
{

  ZScanSeedFinder::ZScanSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    
    m_mode1dfinder("Trk::FsmwMode1dFinder"),
    m_disableAllWeights(false),
    m_constraintcutoff(9.),
    m_constrainttemp(9.),
    m_usept(true),
    m_exppt(1.),
    m_zBfieldApprox(0.60407)
  {   
    declareProperty("Mode1dFinder",     m_mode1dfinder);
    declareProperty("disableAllWeights",m_disableAllWeights);
    declareProperty("constrainttemp",m_constrainttemp);
    declareProperty("constraintcutoff",m_constraintcutoff);
    declareProperty("UsePt",m_usept);
    declareProperty("ExpPt",m_exppt);
    declareProperty("zBfieldApprox",m_zBfieldApprox);
    declareInterface<IVertexSeedFinder>(this);
  }

  ZScanSeedFinder::~ZScanSeedFinder() {}

  StatusCode ZScanSeedFinder::initialize() 
  { 
    if ( m_mode1dfinder.retrieve().isFailure() ) {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_mode1dfinder << endreq;
      return StatusCode::FAILURE;
    } else {
      msg(MSG::INFO)<< "Retrieved tool " << m_mode1dfinder << endreq;
    }

    msg(MSG::INFO) << "Initialize successful" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode ZScanSeedFinder::finalize() 
  {
    msg(MSG::INFO) << "Finalize successful" << endreq;
    return StatusCode::SUCCESS;
  }


  Vertex ZScanSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const RecVertex * constraint) {
    

    //create perigees from track list
    std::vector<const TrackParameters*> perigeeList;
    
    std::vector<const Trk::Track*>::const_iterator begin=VectorTrk.begin();
    std::vector<const Trk::Track*>::const_iterator end=VectorTrk.end();
    
    for (std::vector<const Trk::Track*>::const_iterator iter=begin;
	 iter!=end;++iter) {
      if (isnan((*iter)->perigeeParameters()->parameters()[Trk::d0])) {
	continue;
      }  
      perigeeList.push_back((*iter)->perigeeParameters());
    }
   
    //create seed from perigee list
    return findSeed(perigeeList,constraint);
    
  }

  Vertex ZScanSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const RecVertex * constraint) {
    
    double ZResult=0.;

    if (m_disableAllWeights==false) {
      
      //Prepare the vector of points, on which the 3d mode has later to be calculated
      std::vector<DoubleAndWeight> ZPositions;

      //Calculate and cache the covariance matrix for the constraint
      AmgSymMatrix(3) weightMatrixPositionConstraint;
      weightMatrixPositionConstraint.setIdentity(); //very arbitrary
      if (constraint != 0) {
	weightMatrixPositionConstraint = constraint->covariancePosition().inverse();
      }
      
      //Implement here SeedPointFinder algorithm, acting with the track vector.... 
      const std::vector<const Trk::TrackParameters*>::const_iterator begin=perigeeList.begin();
      const std::vector<const Trk::TrackParameters*>::const_iterator end=perigeeList.end();
      
      for (std::vector<const Trk::TrackParameters*>::const_iterator i=begin;i!=end;i++) {
	
	const Perigee* myMP=dynamic_cast<const Trk::Perigee*>(*i);
	if (myMP==0) {
	  msg(MSG::WARNING) << "Neutrals not supported for seeding. Rejecting this track..." << endreq;
	  continue;
	}


        std::pair<double,double> z0AndD0(myMP->position()[Trk::z],0.);

        if (constraint!=0)
        {
          z0AndD0=estimateZandD0(*myMP,*constraint);
        }

#ifdef ZSCANSEEDFINDER
//	msg(MSG::VERBOSE) << "Track z: " << myMP->position()[Trk::z] << endreq;
	msg(MSG::VERBOSE) << "Track z: " << z0AndD0.first << endreq;
#endif
	
	
	DoubleAndWeight thispoint;
	
	thispoint.first=z0AndD0.first; //OLD myMP->position()[Trk::z];
	if ( m_usept==true) {
	  thispoint.second=std::pow(fabs(1./myMP->parameters()[Trk::qOverP])*sin(myMP->parameters()[Trk::theta]),m_exppt);
	} else {
	  thispoint.second=1.;
	}
	
#ifdef ZSCANSEEDFINDER
	msg(MSG::DEBUG) << "Added position z: " << thispoint.first << " with weight " << thispoint.second;
#endif
	  
	if (constraint!=0 && constraint->covariancePosition()(0,0)!=0) {
	  //TODO: this is WRONG, use the correct one once Giacinto confirms
	  double inverror2d(std::sqrt(std::pow(weightMatrixPositionConstraint(0,0),2)+
				      std::pow(weightMatrixPositionConstraint(1,1),2)));
	  
#ifdef ZSCANSEEDFINDER
	  msg(MSG::VERBOSE) << "Mean error on transverse plane: " << std::sqrt(1./inverror2d) << endreq;
#endif
	  
	  double chi2=inverror2d*z0AndD0.second*z0AndD0.second;//myMP->parameters()[Trk::d0]*myMP->parameters()[Trk::d0];
	  thispoint.second=thispoint.second*1./(1.+exp(-(chi2-m_constraintcutoff)/m_constrainttemp));
	  
#ifdef ZSCANSEEDFINDER
	  msg(MSG::DEBUG) << "Resulting chi2: " << chi2 << " and weight " << 1./(1.+exp(-(chi2-m_constraintcutoff)/m_constrainttemp)) << endreq;
#endif
	  
	}
	
	ZPositions.push_back(thispoint);
      }
      
      if (ZPositions.size()>0) {
	ZResult=m_mode1dfinder->getMode(ZPositions);
      }
    } else {
      
      std::vector<double> ZPositions;
      
      //Implement here SeedPointFinder algorithm, acting with the track vector.... 
      const std::vector<const Trk::TrackParameters*>::const_iterator begin=perigeeList.begin();
      const std::vector<const Trk::TrackParameters*>::const_iterator end=perigeeList.end();
      
      for (std::vector<const Trk::TrackParameters*>::const_iterator i=begin;i!=end;++i) {
	
	const Perigee* myMP=dynamic_cast<const Trk::Perigee*>(*i);
	if (myMP==0) {
	  msg(MSG::WARNING) << "Neutrals not supported for seeding. Rejecting this track..." << endreq;
	  continue;
	}
	
        std::pair<double,double> z0AndD0(myMP->position()[Trk::z],0.);

        if (constraint!=0)
        {
          z0AndD0=estimateZandD0(*myMP,*constraint);
        }

#ifdef ZSCANSEEDFINDER
//	msg(MSG::VERBOSE) << "Track z: " << myMP->position()[Trk::z] << endreq;
	msg(MSG::VERBOSE) << "Track z: " << z0AndD0.first << endreq;
#endif
	
	
//	ZPositions.push_back(myMP->position()[Trk::z]);
	ZPositions.push_back(z0AndD0.first);
	
	if (ZPositions.size()>0) {
	  ZResult=m_mode1dfinder->getMode(ZPositions);
	}
      }
    }
    
    
#ifdef ZSCANSEEDFINDER
    msg(MSG::INFO)<< "Resulting mean POINT FOUND:  x: 0 y: 0 z: " << ZResult << endreq;
#endif

    if (constraint)
    {
      return Vertex(Amg::Vector3D(constraint->position().x(),constraint->position().y(),ZResult));
    }
    return Vertex(Amg::Vector3D(0.,0.,ZResult));
    
  }

/* COMMENTED OUT
  std::pair<double,double> ZScanSeedFinder::estimateZandD0(const Trk::Perigee& myPerigee, const Trk::Vertex& myTransvVertex)
  {
    
    Amg::Vector3D lp=myTransvVertex.position();
    
    Amg::Vector3D predStatePosition = myPerigee.position();
    //extrapolatedPerigee->position();
    Amg::Vector3D expPoint;
    expPoint[0] = predStatePosition.x();
    expPoint[1] = predStatePosition.y();
    expPoint[2] = predStatePosition.z(); 
    
    //phi_v and functions  
    double phi_v = myPerigee.parameters()[Trk::phi];
    double sin_phi_v = sin(phi_v);
    double cos_phi_v = cos(phi_v);
    
    //q over p  
    double q_ov_p = myPerigee.parameters()[Trk::qOverP];
    int sgn_h = (q_ov_p<0.)? -1:1;
    
    //theta and functions  
    double th = myPerigee.parameters()[Trk::theta];
    double sin_th = sin(th);
    double tan_th = tan(th);
    
    //momentum
    Amg::Vector3D expMomentum;
    expMomentum[0] = phi_v;
    expMomentum[1] = th;
    expMomentum[2] = q_ov_p;
    
    double B_z=m_zBfieldApprox;
    
    //signed radius and rotation variables    
    double rho =  sin_th / (q_ov_p * B_z);  
    double X = expPoint[0] - lp.x() + rho*sin_phi_v;
    double Y = expPoint[1] - lp.y() - rho*cos_phi_v;
    double SS = (X * X + Y * Y);
    double S = sqrt(SS);
    
    //calculated parameters at expansion point
    
    //calculation of phi at expansion point    
    double phiAtEp;
    int sgnY = (Y<0)? -1:1;
    int sgnX = (X<0)? -1:1;  
    double m_pi = TMath::Pi();//acos(-1.);
    
    if(fabs(X)>fabs(Y)) phiAtEp = sgn_h*sgnX* acos(-sgn_h * Y / S);
    else
    {
      phiAtEp = asin(sgn_h * X / S);    
      if( (sgn_h * sgnY)> 0) phiAtEp =  sgn_h * sgnX * m_pi - phiAtEp;
    }
    
//    std::cout << " Z: " << expPoint[2] << std::endl;
//    std::cout << " Z (PCA): " << expPoint[2] + rho*(phi_v - phiAtEp)/tan_th << std::endl;

//    std::cout << " D0: " << myPerigee.parameters()[Trk::d0] << std::endl;
//    std::cout << " D0 (PCA): " << rho - sgn_h * S << std::endl;
    
    return std::pair<double,double>(expPoint[2] + rho*(phi_v - phiAtEp)/tan_th, rho - sgn_h * S);

}
*/

  std::pair<double,double> ZScanSeedFinder::estimateZandD0(const Trk::Perigee& myPerigee, const Trk::Vertex& myTransvVertex)
  {
    
    Amg::Vector3D lp=myTransvVertex.position();    
    Amg::Vector3D predStatePosition = myPerigee.position();

    Amg::Vector3D expPoint;
    expPoint[0] = predStatePosition.x();
    expPoint[1] = predStatePosition.y();
    expPoint[2] = predStatePosition.z(); 
    
    //phi_v and functions  
    double phi_v = myPerigee.parameters()[Trk::phi];
    double th = myPerigee.parameters()[Trk::theta];
    double tan_th = tan(th);
//    double sin_th = sin(th);
    double sin_phi_v = sin(phi_v);
    double cos_phi_v = cos(phi_v);
    double q_ov_p = myPerigee.parameters()[Trk::qOverP];

    //momentum
    Amg::Vector3D expMomentum;
    expMomentum[0] = phi_v;
    expMomentum[1] = th;
    expMomentum[2] = q_ov_p;
    
    double X = expPoint[0] - lp.x();
    double Y = expPoint[1] - lp.y();

    return std::pair<double,double>(expPoint[2] - 1./tan_th*(X*cos_phi_v+Y*sin_phi_v),Y*cos_phi_v-X*sin_phi_v);

  }

  std::vector<Vertex> ZScanSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>& /* vectorTrk */,const RecVertex * /* constraint */) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    msg(MSG::WARNING) << "Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds" << endreq;
    return std::vector<Vertex>(0);

  }

  std::vector<Vertex> ZScanSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* perigeeList */,const RecVertex * /* constraint */) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    msg(MSG::WARNING) << "Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds" << endreq;
    return std::vector<Vertex>(0);

  }

}
