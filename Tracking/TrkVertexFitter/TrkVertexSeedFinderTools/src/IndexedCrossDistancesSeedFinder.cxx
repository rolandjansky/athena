/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//Author: Lianyou Shan <lianyou.shan@cern.ch>
/*********************************************************************
          IndexedCrossDistancesSeedFinder.cxx - Description in header file
*********************************************************************/

//#define CROSSDISTANCESSEEDFINDER_DEBUG

#include "TrkVertexSeedFinderTools/IndexedCrossDistancesSeedFinder.h"
#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "TrkVertexSeedFinderUtils/ITrkDistanceFinder.h"
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"

#include "TrkTrack/Track.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkParameters/TrackParameters.h"
#include <cmath>


namespace Trk
{

  IndexedCrossDistancesSeedFinder::IndexedCrossDistancesSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    m_useweights(true),
    m_trackdistcutoff(0.020),
    m_trackdistexppower(2),
    m_constraintcutoff(9.),
    m_constrainttemp(9.),
    m_mode3dfinder("Trk::Mode3dFromFsmw1dFinder", this ),
    m_distancefinder("Trk::SeedNewtonTrkDistanceFinder", this ),
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


  IndexedCrossDistancesSeedFinder::~IndexedCrossDistancesSeedFinder()
  {
  }


  StatusCode IndexedCrossDistancesSeedFinder::initialize() 
  { 
    ATH_CHECK( m_mode3dfinder.retrieve() );
    ATH_CHECK( m_distancefinder.retrieve() );
    ATH_MSG_INFO( "Initialize successful" );
    return StatusCode::SUCCESS;
  }

  StatusCode IndexedCrossDistancesSeedFinder::finalize() 
  {
    ATH_MSG_INFO( "Finalize successful" );
    return StatusCode::SUCCESS;
  }


  Amg::Vector3D
  IndexedCrossDistancesSeedFinder::findSeed(const std::vector<const Trk::Track*> & /*VectorTrk*/,
                                            const xAOD::Vertex * /*constraint*/) const
  {
    ATH_MSG_ERROR ("Need to supply a primary vertex.");
    return Amg::Vector3D(0.,0.,0.);
  }


  Amg::Vector3D
  IndexedCrossDistancesSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & /*perigeeList*/,
                                            const xAOD::Vertex * /*constraint*/) const
  {
    ATH_MSG_ERROR ("Need to supply a primary vertex.");
    return Amg::Vector3D(0.,0.,0.);
  }


  Amg::Vector3D
  IndexedCrossDistancesSeedFinder::findSeed(const double vx,
                                            const double vy,
                                            const std::vector<const Trk::TrackParameters*>& perigeeList,
                                            const xAOD::Vertex* constraint) const
  {
    std::unique_ptr<Trk::IMode3dInfo> info;
    return findSeed (vx, vy, info, perigeeList, constraint);
  }


  Amg::Vector3D
  IndexedCrossDistancesSeedFinder::findSeed(const double vx,
                                            const double vy,
                                            std::unique_ptr<Trk::IMode3dInfo>& info,
                                            const std::vector<const Trk::TrackParameters*>& perigeeList,
                                            const xAOD::Vertex* constraint) const
  {
    ATH_MSG_DEBUG( " Enter IndexedCrossDistancesSeedFinder  " );

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

    ATH_MSG_DEBUG( " Loop pairs of TrackParameters for modes " );

    std::vector< std::pair <int, int> > trkidx ;
    int idx_i = 0 ;
    for (std::vector<const Trk::TrackParameters*>::const_iterator i=begin;i!=end-1;i++) 
    {
      idx_i ++ ;
      const Trk::Perigee* MyI=dynamic_cast<const Trk::Perigee*>(*i);
      if (MyI==nullptr) {
	ATH_MSG_WARNING( "Neutrals not supported for seeding. Rejecting this track..." );
	continue;
      }	
      
      int idx_j = idx_i ;   //  1 has been added to idx_i
      for (std::vector<const Trk::TrackParameters*>::const_iterator j=i+1;j!=end;j++) {

        idx_j ++ ;
        const Trk::Perigee* MyJ=dynamic_cast<const Trk::Perigee*>(*j);

        if (MyJ==nullptr) {
	  ATH_MSG_WARNING( "Neutrals not supported for seeding. Rejecting this track..." );
	  continue;
        }		

	
	try {
	  
          std::optional<ITrkDistanceFinder::TwoPoints> result
            = m_distancefinder->CalculateMinimumDistance(*MyI,*MyJ);
	  if (!result) {ATH_MSG_DEBUG("Problem with distance finder: THIS POINT WILL BE SKIPPED!"); 
          } 
          else 
          {
	    //Get the points which connect the minimum distance between the two tracks
            double distance = Amg::distance (result->first, result->second);
#ifdef CROSSDISTANCESSEEDFINDER_DEBUG
	    ATH_MSG_DEBUG("Point 1 x: " << result->first.x() << 
			  " y: " << result->first.y() << 
			  " z: " << result->first.z() );
	    ATH_MSG_DEBUG("Point 2 x: " << result->second.x() << 
			  " y: " << result->second.y() << 
			  " z: " << result->second.z() );
	    ATH_MSG_DEBUG("distance is: " << distance );
#endif
	    
	    Amg::Vector3D thepoint((result->first+result->second)/2.);

            if (m_useweights)
            {
              PositionAndWeight thispoint(thepoint,
                                          1./pow(m_trackdistcutoff+distance,m_trackdistexppower));
              
              ATH_MSG_VERBOSE("distance weight: " << 1./pow(m_trackdistcutoff+distance,m_trackdistexppower) );
              
              if (constraint!=nullptr) {
                
		Amg::Vector3D DeltaP(thepoint-constraint->position());
		Amg::Vector3D DeltaPConv;
                ATH_MSG_DEBUG("position x: " << DeltaP.x() << "position y: " << DeltaP.y() << "position z: " << DeltaP.z() );
                DeltaPConv[0]=DeltaP.x();
                DeltaPConv[1]=DeltaP.y();
                DeltaPConv[2]=DeltaP.z();


                double chi2=DeltaPConv.transpose()*weightMatrixPositionConstraint*DeltaPConv;

                ATH_MSG_DEBUG(" chi: " << chi2  << 
			      " beam weight " << 1./(1.+exp((chi2-m_constraintcutoff)/m_constrainttemp)) );
                
                thispoint.second=thispoint.second*1./(1.+exp((chi2-m_constraintcutoff)/m_constrainttemp));
                
              }

              if ((!useCutOnDistance || distance<m_maximumDistanceCut) && thispoint.second > 1e-10)
              {
                CrossingPointsAndWeights.push_back( thispoint );

                trkidx.emplace_back( idx_i - 1 , idx_j - 1 );

                ATH_MSG_VERBOSE( " crossing with track pair : " << idx_i - 1 <<" " 
				 << MyI->parameters()[Trk::d0] <<" "<< idx_j - 1 <<" " 
				 << MyJ->parameters()[Trk::d0] );

              }
            }
            else
            {
	      const Amg::Vector3D& thispoint(thepoint);
              if ( !useCutOnDistance || distance<m_maximumDistanceCut )
              {
                CrossingPoints.push_back( thispoint );
              }
            }
          }
        } catch (...) {
          ATH_MSG_ERROR("Something wrong in distance calculation: please report..." );
        }
      }
      //to be understood...
    }

    //Now all points have been collected (N*(N-1)/2) and 
    //the mode has to be calculated

    if (    ( CrossingPoints.empty() && ! m_useweights  ) 
         || ( m_useweights && CrossingPointsAndWeights.empty() ) )
    {
      return Amg::Vector3D(0,0,0);
    }
   
    ATH_MSG_DEBUG(" crossing points prepared : " << CrossingPointsAndWeights.size() ); 

    Amg::Vector3D myresult;

    if (m_useweights)
    {
      myresult=m_mode3dfinder->getMode(vx, vy, CrossingPointsAndWeights, info);
    }
    else
    {
      myresult=m_mode3dfinder->getMode(vx, vy, CrossingPoints, info);
    }

    info->setTrkidx (std::move (trkidx));
    ATH_MSG_DEBUG(" 3D modes found ! " ); 

    return myresult;
    
  }


std::vector<Amg::Vector3D> IndexedCrossDistancesSeedFinder::findMultiSeeds(
    const std::vector<const Trk::Track*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */) const
{

 //implemented to satisfy inheritance but this algorithm only supports one seed at a time
  ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds" );
   return std::vector<Amg::Vector3D>(0);
}

std::vector<Amg::Vector3D> IndexedCrossDistancesSeedFinder::findMultiSeeds( 
   const std::vector<const Trk::TrackParameters*>& /* perigeeList */,const xAOD::Vertex * /* constraint */) const
{
  
   //implemented to satisfy inheritance but this algorithm only supports one seed at a time
  ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds" );
   return std::vector<Amg::Vector3D>(0);

}


}
