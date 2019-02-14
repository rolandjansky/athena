/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "EventPrimitives/EventPrimitives.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include <math.h>


namespace Trk
{

  IndexedCrossDistancesSeedFinder::IndexedCrossDistancesSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
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
    
    declareInterface<IVertexSeedFinder>(this);
  }

  IndexedCrossDistancesSeedFinder::~IndexedCrossDistancesSeedFinder() {}

  StatusCode IndexedCrossDistancesSeedFinder::initialize() 
  { 
    StatusCode s = m_mode3dfinder.retrieve();
    if (s.isFailure())
      {
	ATH_MSG_FATAL( "Could not find mode3dfinder tool." );
	return StatusCode::FAILURE;
      }
    s = m_distancefinder.retrieve();
    if (s.isFailure())
      {
	ATH_MSG_FATAL( "Could not find distance finder tool." );
	return StatusCode::FAILURE;
      }
    ATH_MSG_INFO( "Initialize successful" );
    return StatusCode::SUCCESS;
  }

  StatusCode IndexedCrossDistancesSeedFinder::finalize() 
  {
    ATH_MSG_INFO( "Finalize successful" );
    return StatusCode::SUCCESS;
  }

  void IndexedCrossDistancesSeedFinder::setPriVtxPosition( double vx, double vy )
  {
    m_mode3dfinder->setPriVtxPosition( vx, vy ) ;
    return ;
  }


  Amg::Vector3D IndexedCrossDistancesSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const xAOD::Vertex * constraint) {
    

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

  Amg::Vector3D IndexedCrossDistancesSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint) {

    ATH_MSG_DEBUG( " Enter IndexedCrossDistancesSeedFinder  " );

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

    ATH_MSG_DEBUG( " Loop pairs of TrackParameters for modes " );

//    m_trkidx = new std::vector< std::pair<int,int> > ;
    m_trkidx.clear() ;
    int idx_i = 0 ;
    for (std::vector<const Trk::TrackParameters*>::const_iterator i=begin;i!=end-1;i++) 
    {
      idx_i ++ ;
      const Trk::Perigee* MyI=dynamic_cast<const Trk::Perigee*>(*i);
      if (MyI==0) {
	ATH_MSG_WARNING( "Neutrals not supported for seeding. Rejecting this track..." );
	continue;
      }	
      
      int idx_j = idx_i ;   //  1 has been added to idx_i
      for (std::vector<const Trk::TrackParameters*>::const_iterator j=i+1;j!=end;j++) {

        idx_j ++ ;
        const Trk::Perigee* MyJ=dynamic_cast<const Trk::Perigee*>(*j);

        if (MyJ==0) {
	  ATH_MSG_WARNING( "Neutrals not supported for seeding. Rejecting this track..." );
	  continue;
        }		

	
	try {
	  
	  bool result=m_distancefinder->CalculateMinimumDistance(*MyI,*MyJ);
	  if (!result) {ATH_MSG_DEBUG("Problem with distance finder: THIS POINT WILL BE SKIPPED!"); 
          } 
          else 
          {
	    //Get the points which connect the minimum distance between the two tracks
	    PointsAtMinDistance=m_distancefinder->GetPoints();
#ifdef CROSSDISTANCESSEEDFINDER_DEBUG
	    ATH_MSG_DEBUG("Point 1 x: " << PointsAtMinDistance.first.x() << 
			  " y: " << PointsAtMinDistance.first.y() << 
			  " z: " << PointsAtMinDistance.first.z() );
	    ATH_MSG_DEBUG("Point 2 x: " << PointsAtMinDistance.second.x() << 
			  " y: " << PointsAtMinDistance.second.y() << 
			  " z: " << PointsAtMinDistance.second.z() );
	    ATH_MSG_DEBUG("distance is: " << m_distancefinder->GetDistance() );
#endif
	    
	    Amg::Vector3D thepoint((PointsAtMinDistance.first+PointsAtMinDistance.second)/2.);

            if (m_useweights)
            {
              PositionAndWeight thispoint(thepoint,
                                          1./pow(m_trackdistcutoff+m_distancefinder->GetDistance(),m_trackdistexppower));
              
              ATH_MSG_VERBOSE("distance weight: " << 1./pow(m_trackdistcutoff+m_distancefinder->GetDistance(),m_trackdistexppower) );
              
              if (constraint!=0) {
                
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

              if ((useCutOnDistance==false || m_distancefinder->GetDistance()<m_maximumDistanceCut) && thispoint.second > 1e-10)
              {
                CrossingPointsAndWeights.push_back( thispoint );

                m_trkidx.push_back( std::pair<int,int>( idx_i - 1 , idx_j - 1 ) ) ;

                ATH_MSG_VERBOSE( " crossing with track pair : " << idx_i - 1 <<" " 
				 << MyI->parameters()[Trk::d0] <<" "<< idx_j - 1 <<" " 
				 << MyJ->parameters()[Trk::d0] );

              }
            }
            else
            {
	      Amg::Vector3D thispoint(thepoint);
              if ( useCutOnDistance==false || m_distancefinder->GetDistance()<m_maximumDistanceCut )
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

    if (    ( CrossingPoints.size() < 1 && ! m_useweights  ) 
         || ( m_useweights && CrossingPointsAndWeights.size() < 1 ) )
    {
      return Amg::Vector3D(0,0,0);
    }
   
    ATH_MSG_DEBUG(" crossing points prepared : " << CrossingPointsAndWeights.size() ); 

    Amg::Vector3D myresult;

    if (m_useweights)
    {
      myresult=m_mode3dfinder->getMode(CrossingPointsAndWeights);

      m_correXY = m_correZ = -9.9 ;
      m_mode3dfinder->getCorrelationDistance( m_correXY, m_correZ ) ;
    }
    else
    {
      myresult=m_mode3dfinder->getMode(CrossingPoints);
    }
    
    ATH_MSG_DEBUG(" 3D modes found ! " ); 

    return myresult;
    
  }


int IndexedCrossDistancesSeedFinder::getModes1d(   std::vector<float>  & phi, 
                  std::vector<float> & radi, std::vector<float>  & z, std::vector<float> & wght ) const  
{
/**
  std::vector<float> thephi , theradi, thez ;
  int m = m_mode3dfinder->Modes1d( thephi, theradi, thez ) ;

  phi = thephi ;
  radi = theradi ;
  z = thez ;
  return m ;
**/
  return (int)( m_mode3dfinder->Modes1d( phi, radi, z, wght ) ) ;
}


int IndexedCrossDistancesSeedFinder::perigeesAtSeed( 
         std::vector<const Trk::TrackParameters*> * perigees , 
   const std::vector<const Trk::TrackParameters*> & perigeeList ) const
{
  perigees->clear() ;
  std::vector<int> trklist(0) ;

  ATH_MSG_DEBUG(" Enter perigeesAtSeed  " );

  std::vector<int> modes = m_mode3dfinder->AcceptedCrossingPointsIndices() ;
 
  ATH_MSG_DEBUG(" found " << modes.size() <<" modes accepted for perigeesAtSeed " );

  for ( unsigned int c = 0 ; c < modes.size() ; c++ )
  {
//    std::pair<int,int> trk = m_trkidx->at( modes[c] ) ;
    std::pair<int,int> trk = m_trkidx.at( modes[c] ) ;
// there was an +1 operation before m_trkidx was filled
    ATH_MSG_DEBUG(" accepted modes " << modes[c] <<" "<< trk.first <<" "<< trk.second );

    if ( std::find( trklist.begin(), trklist.end(), trk.first ) ==  trklist.end() ) 
      trklist.push_back( trk.first ) ;
    if ( std::find( trklist.begin(), trklist.end(), trk.second ) ==  trklist.end() )
      trklist.push_back( trk.second ) ;
  }

//  m_trkidx.clear() ;
//  if ( m_trkidx ) delete m_trkidx ;

  ATH_MSG_DEBUG(" size of trklist with indicies : " << trklist.size() );

  std::sort( trklist.begin(), trklist.end() ) ;

  for ( unsigned int t = 0 ; t < trklist.size() ; t++ )
  {
    std::vector<const Trk::TrackParameters*>::const_iterator i= perigeeList.begin() + trklist[t] ;

    perigees->push_back( dynamic_cast<const Trk::TrackParameters*>(*i)  ) ;
  }

  return perigees->size() ;
}
 
std::vector<Amg::Vector3D> IndexedCrossDistancesSeedFinder::findMultiSeeds(
    const std::vector<const Trk::Track*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */) 
{

 //implemented to satisfy inheritance but this algorithm only supports one seed at a time
  ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds" );
   return std::vector<Amg::Vector3D>(0);
}

std::vector<Amg::Vector3D> IndexedCrossDistancesSeedFinder::findMultiSeeds( 
   const std::vector<const Trk::TrackParameters*>& /* perigeeList */,const xAOD::Vertex * /* constraint */)
{
  
   //implemented to satisfy inheritance but this algorithm only supports one seed at a time
  ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds" );
   return std::vector<Amg::Vector3D>(0);

}


}
