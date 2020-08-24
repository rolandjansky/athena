/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include <cmath>


// Would be nice to use something like Amg::distance instead.
// But that rounds slightly differently.
// Do it like this so that results are identical with the pre-MT version.
namespace {
  inline double square(const double tosquare) {
    return std::pow(tosquare,2);
  }
  double dist(const std::pair<Amg::Vector3D,Amg::Vector3D>& pairofpos) {
    Amg::Vector3D diff(pairofpos.first-pairofpos.second);
    return std::sqrt(square(diff.x())+square(diff.y())+square(diff.z()));
  }
}


namespace Trk
{

  CrossDistancesSeedFinder::CrossDistancesSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
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
  }


  CrossDistancesSeedFinder::~CrossDistancesSeedFinder()
  {
  }


  StatusCode CrossDistancesSeedFinder::initialize() 
  { 
    ATH_CHECK( m_mode3dfinder.retrieve() );
    ATH_CHECK( m_distancefinder.retrieve() );
    ATH_MSG_INFO( "Initialize successful"  );
    return StatusCode::SUCCESS;
  }

  StatusCode CrossDistancesSeedFinder::finalize() 
  {
    ATH_MSG_INFO( "Finalize successful"  );
    return StatusCode::SUCCESS;
  }


  Amg::Vector3D CrossDistancesSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const xAOD::Vertex * constraint) const {
    

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

  Amg::Vector3D CrossDistancesSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint) const {

    bool useCutOnDistance=false;
    if (perigeeList.size()>m_maximumTracksNoCut)
    {
      useCutOnDistance=true;
    }
    
    //now implement the code you already had in the standalone code...


    //Calculate and cache the covariance matrix for the constraint
    AmgSymMatrix(3) weightMatrixPositionConstraint;
    weightMatrixPositionConstraint.setIdentity(); //very arbitrary
    if (constraint != nullptr) {
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
      if (MyI==nullptr) {
	ATH_MSG_WARNING( "Neutrals not supported for seeding. Rejecting this track..."  );
	continue;
      }	
      
      for (std::vector<const Trk::TrackParameters*>::const_iterator j=i+1;j!=end;j++) {

      const Trk::Perigee* MyJ=dynamic_cast<const Trk::Perigee*>(*j);
      if (MyJ==nullptr) {
	ATH_MSG_WARNING( "Neutrals not supported for seeding. Rejecting this track..."  );
	continue;
      }		

#ifdef CROSSDISTANCESSEEDFINDER_DEBUG
	
      ATH_MSG_DEBUG( "Track 1 d0: " << MyI->parameters()[Trk::d0] << " Track2 d0: " << MyJ->parameters()[Trk::d0]  );
      ATH_MSG_DEBUG( "Track 1 z0: " << MyI->parameters()[Trk::z0] << "Track2 z0: " << MyJ->parameters()[Trk::z0]  );
#endif
	
	try {

          std::optional<ITrkDistanceFinder::TwoPoints> result
            = m_distancefinder->CalculateMinimumDistance(*MyI,*MyJ);
	  if (!result) { ATH_MSG_DEBUG( "Problem with distance finder: THIS POINT WILL BE SKIPPED!"  );
          } 
          else 
          {
	    //Get the points which connect the minimum distance between the two tracks
            double distance = dist (result.value());
#ifdef CROSSDISTANCESSEEDFINDER_DEBUG
	    ATH_MSG_DEBUG( "Point 1 x: " << result->first.x() << 
                           " y: " << result->first.y() << 
                           " z: " << result->first.z()  );
	    ATH_MSG_DEBUG( "Point 2 x: " << result->second.x() << 
                           " y: " << result->second.y() << 
                           " z: " << result->second.z()  );
	    ATH_MSG_DEBUG( "distance is: " << distance  );
#endif
	    
	    Amg::Vector3D thepoint((result->first+result->second)/2.);

            if (m_useweights)
            {
              PositionAndWeight thispoint(thepoint,
                                          1./pow(m_trackdistcutoff+distance,m_trackdistexppower));
              
              ATH_MSG_DEBUG( "distance weight: " << 1./pow(m_trackdistcutoff+distance,m_trackdistexppower)  );
              
              if (constraint!=nullptr) {
                
		Amg::Vector3D DeltaP(thepoint-constraint->position());
		Amg::Vector3D DeltaPConv;
                ATH_MSG_DEBUG( "position x: " << DeltaP.x() << "position y: " << DeltaP.y() << "position z: " << DeltaP.z()  );
                DeltaPConv[0]=DeltaP.x();
                DeltaPConv[1]=DeltaP.y();
                DeltaPConv[2]=DeltaP.z();


                double chi2=DeltaPConv.transpose()*weightMatrixPositionConstraint*DeltaPConv;

                ATH_MSG_DEBUG( " chi: " << chi2 
                               << " beam weight " << 1./(1.+exp((chi2-m_constraintcutoff)/m_constrainttemp))  );
                
                thispoint.second=thispoint.second*1./(1.+exp((chi2-m_constraintcutoff)/m_constrainttemp));
                
              }
              if ((!useCutOnDistance || distance<m_maximumDistanceCut) && thispoint.second > 1e-10)
              {
                CrossingPointsAndWeights.push_back(thispoint);
              }
            }
            else
            {
	      const Amg::Vector3D& thispoint(thepoint);
              if (!useCutOnDistance || distance<m_maximumDistanceCut)
              {
                CrossingPoints.push_back(thispoint);
              }
            }
          }
        } catch (...) {
          ATH_MSG_ERROR( "Something wrong in distance calculation: please report..."  );
        }
      }
      //to be understood...
    }
    
    //Now all points have been collected (N*(N-1)/2) and 
    //the mode has to be calculated

    if (CrossingPoints.empty() && CrossingPointsAndWeights.empty())
    {
      return Amg::Vector3D(0,0,0);
    }
   
    Amg::Vector3D myresult;

    if (m_useweights)
    {
      myresult=m_mode3dfinder->getMode(0, 0, CrossingPointsAndWeights);
    }
    else
    {
      myresult=m_mode3dfinder->getMode(0, 0, CrossingPoints);
    }
    
#ifdef CROSSDISTANCESSEEDFINDER_DEBUG
    ATH_MSG_INFO( "Resulting mean POINT FOUND:  x: " << myresult.x() << 
                  " y: " << myresult.y() << 
                  " z: " << myresult.z()  );
#endif
    
    
    return myresult;
    
    
    
    
  }

  std::vector<Amg::Vector3D> CrossDistancesSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */) const {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    ATH_MSG_WARNING( "Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds"  );
    return std::vector<Amg::Vector3D>(0);

  }

  std::vector<Amg::Vector3D> CrossDistancesSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* perigeeList */,const xAOD::Vertex * /* constraint */) const {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    ATH_MSG_WARNING( "Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds"  );
    return std::vector<Amg::Vector3D>(0);

  }


}
