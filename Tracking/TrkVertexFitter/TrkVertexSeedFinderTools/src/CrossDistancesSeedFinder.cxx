/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          CrossDistancesSeedFinder.cxx - Description in header file
*********************************************************************/

//#define CROSSDISTANCESSEEDFINDER_DEBUG

#include "TrkVertexSeedFinderTools/CrossDistancesSeedFinder.h"

#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"

#include "TrkTrack/Track.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include <math.h>


namespace Trk
{

  CrossDistancesSeedFinder::CrossDistancesSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_useweights(true),
    m_trackdistcutoff(0.020),
    m_trackdistexppower(2),
    m_constraintcutoff(9.),
    m_constrainttemp(9.),
    m_mode3dfinder("Trk::Mode3dTo1dFinder", this),
    m_distancefinder("Trk::SeedNewtonTrkDistanceFinder", this),
    m_maximumTracksNoCut(20),
    m_maximumDistanceCut(3.)
  {   
    declareProperty("useweights",m_useweights);
    declareProperty("trackdistcutoff",m_trackdistcutoff);
    declareProperty("trackdistexppower",m_trackdistexppower);
    declareProperty("constrainttemp",m_constrainttemp);
    declareProperty("constraintcutoff",m_constraintcutoff);
    declareProperty("Mode3dFinder",     m_mode3dfinder);
    declareProperty("TrkDistanceFinder",     m_distancefinder);
    declareProperty("maximumTracksNoCut",m_maximumTracksNoCut);
    declareProperty("maximumDistanceCut",m_maximumDistanceCut);
    
    declareInterface<IVertexSeedFinder>(this);
  }

  CrossDistancesSeedFinder::~CrossDistancesSeedFinder() {}

  StatusCode CrossDistancesSeedFinder::initialize() 
  { 
    StatusCode s = m_mode3dfinder.retrieve();
    if (s.isFailure())
      {
	msg(MSG::FATAL)<<"Could not find mode3dfinder tool." << endmsg;
	return StatusCode::FAILURE;
      }
    s = m_distancefinder.retrieve();
    if (s.isFailure())
      {
	msg(MSG::FATAL)<<"Could not find distance finder tool." << endmsg;
	return StatusCode::FAILURE;
      }
    msg(MSG::INFO)  << "Initialize successful" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode CrossDistancesSeedFinder::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
  }


  Amg::Vector3D CrossDistancesSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const xAOD::Vertex * constraint) {
    

    //create perigees from track list
    std::vector<const TrackParameters*> perigeeList;
    for (std::vector<const Trk::Track*>::const_iterator iter=VectorTrk.begin();
	 iter!=VectorTrk.end();iter++) {
      if (std::isnan((*iter)->perigeeParameters()->parameters()[Trk::d0])) {
	continue;
      }  
      perigeeList.push_back((*iter)->perigeeParameters());
    }

    if (perigeeList.size()<2)
    {
      return Amg::Vector3D(0.,0.,0.);
    }
   
    //create seed from perigee list
    return findSeed(perigeeList,constraint);
    
  }

  Amg::Vector3D CrossDistancesSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint) {

    bool useCutOnDistance=false;
    if (perigeeList.size()>m_maximumTracksNoCut)
    {
      useCutOnDistance=true;
    }
    
    //now implement the code you already had in the standalone code...

    //Variable to temporary store the points at minimum distance between the two tracks
    std::pair<Amg::Vector3D,Amg::Vector3D> PointsAtMinDistance;

    //Calculate and cache the covariance matrix for the constraint
    AmgSymMatrix(3) weightMatrixPositionConstraint;
    weightMatrixPositionConstraint.setIdentity(); //very arbitrary
    if (constraint != 0) {
      weightMatrixPositionConstraint = constraint->covariancePosition().inverse();
    }
    
    //Prepare the vector of points, on which the 3d mode has later to be calculated
    std::vector<PositionAndWeight> CrossingPointsAndWeights;
    std::vector<Amg::Vector3D> CrossingPoints;
    
    //Implement here SeedPointFinder algorithm, acting with the track vector.... 
    const std::vector<const Trk::TrackParameters*>::const_iterator begin=perigeeList.begin();
    const std::vector<const Trk::TrackParameters*>::const_iterator end=perigeeList.end();

    for (std::vector<const Trk::TrackParameters*>::const_iterator i=begin;i!=end-1;i++) {
      
      const Trk::Perigee* MyI=dynamic_cast<const Trk::Perigee*>(*i);
      if (MyI==0) {
	msg(MSG::WARNING) << "Neutrals not supported for seeding. Rejecting this track..." << endmsg;
	continue;
      }	
      
      for (std::vector<const Trk::TrackParameters*>::const_iterator j=i+1;j!=end;j++) {

      const Trk::Perigee* MyJ=dynamic_cast<const Trk::Perigee*>(*j);
      if (MyJ==0) {
	msg(MSG::WARNING) << "Neutrals not supported for seeding. Rejecting this track..." << endmsg;
	continue;
      }		

#ifdef CROSSDISTANCESSEEDFINDER_DEBUG
	
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "Track 1 d0: " << MyI->parameters()[Trk::d0] << " Track2 d0: " << MyJ->parameters()[Trk::d0] << endmsg;
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "Track 1 z0: " << MyI->parameters()[Trk::z0] << "Track2 z0: " << MyJ->parameters()[Trk::z0] << endmsg;
#endif
	
	try {
	  
	  bool result=m_distancefinder->CalculateMinimumDistance(*MyI,*MyJ);
	  if (!result) { if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "Problem with distance finder: THIS POINT WILL BE SKIPPED!" << endmsg; 
          } 
          else 
          {
	    //Get the points which connect the minimum distance between the two tracks
	    PointsAtMinDistance=m_distancefinder->GetPoints();
#ifdef CROSSDISTANCESSEEDFINDER_DEBUG
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "Point 1 x: " << PointsAtMinDistance.first.x() << 
	      " y: " << PointsAtMinDistance.first.y() << 
	      " z: " << PointsAtMinDistance.first.z() << endmsg;
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "Point 2 x: " << PointsAtMinDistance.second.x() << 
	      " y: " << PointsAtMinDistance.second.y() << 
	      " z: " << PointsAtMinDistance.second.z() << endmsg;
	    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "distance is: " << m_distancefinder->GetDistance() << endmsg;
#endif
	    
	    Amg::Vector3D thepoint((PointsAtMinDistance.first+PointsAtMinDistance.second)/2.);

            if (m_useweights)
            {
              PositionAndWeight thispoint(thepoint,
                                          1./pow(m_trackdistcutoff+m_distancefinder->GetDistance(),m_trackdistexppower));
              
              if(msgLvl(MSG::VERBOSE)) msg(MSG::DEBUG) << "distance weight: " << 1./pow(m_trackdistcutoff+m_distancefinder->GetDistance(),m_trackdistexppower) << endmsg;
              
              if (constraint!=0) {
                
		Amg::Vector3D DeltaP(thepoint-constraint->position());
		Amg::Vector3D DeltaPConv;
                if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< "position x: " << DeltaP.x() << "position y: " << DeltaP.y() << "position z: " << DeltaP.z() << endmsg;
                DeltaPConv[0]=DeltaP.x();
                DeltaPConv[1]=DeltaP.y();
                DeltaPConv[2]=DeltaP.z();


                double chi2=DeltaPConv.transpose()*weightMatrixPositionConstraint*DeltaPConv;

                if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)<< " chi: " << chi2 
                                                      << " beam weight " << 1./(1.+exp((chi2-m_constraintcutoff)/m_constrainttemp)) << endmsg;
                
                thispoint.second=thispoint.second*1./(1.+exp((chi2-m_constraintcutoff)/m_constrainttemp));
                
              }
              if ((useCutOnDistance==false || m_distancefinder->GetDistance()<m_maximumDistanceCut) && thispoint.second > 1e-10)
              {
                CrossingPointsAndWeights.push_back(thispoint);
              }
            }
            else
            {
	      Amg::Vector3D thispoint(thepoint);
              if (useCutOnDistance==false || m_distancefinder->GetDistance()<m_maximumDistanceCut)
              {
                CrossingPoints.push_back(thispoint);
              }
            }
          }
        } catch (...) {
          msg(MSG::ERROR) << "Something wrong in distance calculation: please report..." << endmsg;
        }
      }
      //to be understood...
    }
    
    //Now all points have been collected (N*(N-1)/2) and 
    //the mode has to be calculated

    if (CrossingPoints.size()<1 && CrossingPointsAndWeights.size()<1)
    {
      return Amg::Vector3D(0,0,0);
    }
   
    Amg::Vector3D myresult;

    if (m_useweights)
    {
      myresult=m_mode3dfinder->getMode(CrossingPointsAndWeights);
    }
    else
    {
      myresult=m_mode3dfinder->getMode(CrossingPoints);
    }
    
#ifdef CROSSDISTANCESSEEDFINDER_DEBUG
    msg(MSG::INFO) << "Resulting mean POINT FOUND:  x: " << myresult.x() << 
      " y: " << myresult.y() << 
      " z: " << myresult.z() << endmsg;
#endif
    
    
    return myresult;
    
    
    
    
  }

  std::vector<Amg::Vector3D> CrossDistancesSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    msg(MSG::WARNING) << "Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds" << endmsg;
    return std::vector<Amg::Vector3D>(0);

  }

  std::vector<Amg::Vector3D> CrossDistancesSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* perigeeList */,const xAOD::Vertex * /* constraint */) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    msg(MSG::WARNING) << "Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds" << endmsg;
    return std::vector<Amg::Vector3D>(0);

  }

  void CrossDistancesSeedFinder::setPriVtxPosition(double /* vx */, double /* vy */) {
    //implemented to satisfy inheritance
  }

  int CrossDistancesSeedFinder::perigeesAtSeed( std::vector<const Trk::TrackParameters*> * /* a */,
						const std::vector<const Trk::TrackParameters*>  & /* b */) const{
    //implemented to satisfy inheritance
    return 0;
  }

  int CrossDistancesSeedFinder::getModes1d(std::vector<float> &/* a */, std::vector<float> & /* b */, 
					   std::vector<float> & /* c */, std::vector<float> & /* d */) const{
   //implemented to satisfy inheritance
    return 0;
  }

  void CrossDistancesSeedFinder::getCorrelationDistance( double & /* cXY */, double & /* cZ */ ){
    //implemented to satisfy inheritance
  }


}
