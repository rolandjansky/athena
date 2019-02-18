/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          TrackDensitySeedFinder.cxx - Description in header file
*********************************************************************/

#include "TrkVertexSeedFinderTools/TrackDensitySeedFinder.h"

#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/ParamDefs.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include <algorithm>

namespace Trk
{

  TrackDensitySeedFinder::TrackDensitySeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
  AthAlgTool(t,n,p)
  {   
    declareInterface<IVertexSeedFinder>(this);
  }

  TrackDensitySeedFinder::~TrackDensitySeedFinder() {}

  StatusCode TrackDensitySeedFinder::initialize() 
  { 
    ATH_CHECK( m_densityEstimator.retrieve() );
    ATH_MSG_DEBUG("Initialize successful");
    return StatusCode::SUCCESS;
  }

  StatusCode TrackDensitySeedFinder::finalize() 
  {
    ATH_MSG_DEBUG("Finalize successful");
    return StatusCode::SUCCESS;
  }

  Amg::Vector3D TrackDensitySeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,
						 const xAOD::Vertex * constraint) 
  {
    
    //create perigees from track list
    std::vector<const TrackParameters*> perigeeList;
    
    std::vector<const Trk::Track*>::const_iterator begin=VectorTrk.begin();
    std::vector<const Trk::Track*>::const_iterator end=VectorTrk.end();
    
    for (std::vector<const Trk::Track*>::const_iterator iter=begin;
	 iter!=end;++iter) 
    {
      perigeeList.push_back((*iter)->perigeeParameters());
    }
   
    //create seed from perigee list
    return findSeed(perigeeList,constraint);  
  }

  Amg::Vector3D TrackDensitySeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,
						 const xAOD::Vertex * constraint)
  {
    double zResult {0.};
    if ( perigeeList.size()>0 ) 
    {
      m_densityEstimator->reset();
      m_densityEstimator->addTracks(perigeeList);
      zResult = m_densityEstimator->globalMaximum();
      ATH_MSG_DEBUG("Highest density Z position found: " << zResult);
    }
    else
    {
      ATH_MSG_DEBUG("No tracks with sufficient weight; return z position = 0");
    }

    if (constraint)
    {
      return Amg::Vector3D(constraint->position().x(), constraint->position().y(), zResult + constraint->position().z());
    }
    else
    {
      return Amg::Vector3D(0.,0.,zResult);
    }  
  }

  std::vector<Amg::Vector3D> TrackDensitySeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>& /* vectorTrk */,
								    const xAOD::Vertex * /* constraint */) 
  {
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds");
    return std::vector<Amg::Vector3D>(0);
  }

  std::vector<Amg::Vector3D> TrackDensitySeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* perigeeList */,
								    const xAOD::Vertex * /* constraint */) 
  {
     //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds");
    return std::vector<Amg::Vector3D>(0);
  }

  void TrackDensitySeedFinder::setPriVtxPosition(double /* vx */, double /* vy */) {
    //implemented to satisfy inheritance
  }

  int TrackDensitySeedFinder::perigeesAtSeed( std::vector<const Trk::TrackParameters*> * /* a */,
					      const std::vector<const Trk::TrackParameters*> & /* b */) const{
    //implemented to satisfy inheritance
    return 0;
  }

  int TrackDensitySeedFinder::getModes1d(std::vector<float> &/* a */, std::vector<float> &/* b */, 
					 std::vector<float> &/* c */, std::vector<float> &/* d */) const{
   //implemented to satisfy inheritance
    return 0;
  }

  void TrackDensitySeedFinder::getCorrelationDistance( double &/* cXY */, double & /* cZ */){
    //implemented to satisfy inheritance
  }
}
