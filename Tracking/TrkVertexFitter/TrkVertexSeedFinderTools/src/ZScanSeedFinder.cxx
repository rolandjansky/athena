/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          ZScanSeedFinder.cxx - Description in header file
*********************************************************************/

#include "TrkVertexSeedFinderTools/ZScanSeedFinder.h"

#include "TrkVertexSeedFinderUtils/SeedFinderParamDefs.h"
#include "TrkVertexSeedFinderUtils/IMode1dFinder.h"
#include "TrkVertexFitterInterfaces/ITrackToVertexIPEstimator.h"

#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/ParamDefs.h"

//Amg
#include "GeoPrimitives/GeoPrimitives.h"

#include <algorithm>
//#include <math.h>

namespace Trk
{

  ZScanSeedFinder::ZScanSeedFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    base_class(t,n,p),
    
    m_mode1dfinder("Trk::FsmwMode1dFinder", this),
    m_IPEstimator("Trk::TrackToVertexIPEstimator", this),
    m_disableAllWeights(false),
    m_constraintcutoff(9.),
    m_constrainttemp(1.),
    m_useLogPt(true),
    m_minPt(400.),
    m_usePt(false),
    m_expPt(1.),
    m_cacheWeights(true)
  {   
    declareProperty("Mode1dFinder",     m_mode1dfinder);
    declareProperty("IPEstimator", m_IPEstimator);
    declareProperty("disableAllWeights", m_disableAllWeights);
    declareProperty("constrainttemp", m_constrainttemp);
    declareProperty("constraintcutoff", m_constraintcutoff);
    declareProperty("UsePt", m_usePt);
    declareProperty("ExpPt", m_expPt);
    declareProperty("UseLogPt", m_useLogPt);
    declareProperty("MinPt", m_minPt);
    declareProperty("CacheWeights", m_cacheWeights);
  }


  ZScanSeedFinder::~ZScanSeedFinder()
  {
  }


  StatusCode ZScanSeedFinder::initialize() 
  { 
    ATH_CHECK( m_eventInfoKey.initialize() );
    if ( m_mode1dfinder.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_mode1dfinder);
      return StatusCode::FAILURE;
    } else if ( m_IPEstimator.retrieve().isFailure() ) {
      ATH_MSG_FATAL("Failed to retrieve tool " << m_IPEstimator);
    } else {
      ATH_MSG_INFO("Retrieved tools " << m_mode1dfinder << " and " << m_IPEstimator);
    }

    if ( m_usePt && m_useLogPt )
    {
      ATH_MSG_FATAL("At most one of Pt and LogPt weighting may be selected");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Initialize successful");
    return StatusCode::SUCCESS;
  }

  StatusCode ZScanSeedFinder::finalize() 
  {
    ATH_MSG_DEBUG("Finalize successful");
    return StatusCode::SUCCESS;
  }


  Amg::Vector3D ZScanSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const xAOD::Vertex * constraint) const {
    
    //create perigees from track list
    std::vector<const TrackParameters*> perigeeList;
    
    std::vector<const Trk::Track*>::const_iterator begin=VectorTrk.begin();
    std::vector<const Trk::Track*>::const_iterator end=VectorTrk.end();
    
    for (std::vector<const Trk::Track*>::const_iterator iter=begin;
	 iter!=end;++iter) 
    {
      if (isnan((*iter)->perigeeParameters()->parameters()[Trk::d0])) 
      {
	continue;
      }  
      perigeeList.push_back((*iter)->perigeeParameters());
    }
   
    //create seed from perigee list
    return findSeed(perigeeList,constraint);
    
  }

  Amg::Vector3D ZScanSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint) const {


    double ZResult=0.;
    std::vector<Trk::DoubleAndWeight> ZPositions;
    if (m_cacheWeights) {
      ZPositions = getPositionsCached (perigeeList, constraint);
    }
    else {
      ZPositions = getPositionsUncached (perigeeList, constraint);
    }

    if ( !ZPositions.empty() ) 
    {
	ZResult=m_mode1dfinder->getMode(ZPositions);
        ATH_MSG_DEBUG("Resulting mean Z position found: " << ZResult);
    }
    else
    {
      ATH_MSG_DEBUG("No tracks with sufficient weight; return z position = 0");
    }

    if (constraint)
    {
      return Amg::Vector3D(constraint->position().x(),constraint->position().y(),ZResult);
    }
    return Amg::Vector3D(0.,0.,ZResult);
    
  }


  std::vector<Trk::DoubleAndWeight>
  ZScanSeedFinder::getPositionsUncached (const std::vector<const Trk::TrackParameters*> & perigeeList,
                                         const xAOD::Vertex * constraint) const
  {
    std::vector<Trk::DoubleAndWeight> ZPositions;

    for (const Trk::TrackParameters* i : perigeeList)
    {
      const Perigee* iTrk = dynamic_cast<const Trk::Perigee*>(i);
      if (iTrk == nullptr)
      {
	  ATH_MSG_WARNING("Neutrals not supported for seeding. Rejecting track...");
	  continue;
      }

      std::pair<double, double> z0AndWeight =
        estimateWeight (*iTrk, constraint);
      ATH_MSG_DEBUG("Found position z: " << z0AndWeight.first <<
                    " with weight " << z0AndWeight.second);

      if (z0AndWeight.second >= 0.01)
      {
	ZPositions.push_back(z0AndWeight);
      }
    }

    return ZPositions;
  }


  std::vector<Trk::DoubleAndWeight>
  ZScanSeedFinder::getPositionsCached (const std::vector<const Trk::TrackParameters*> & perigeeList,
                                       const xAOD::Vertex * constraint) const
  {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    Cache& cache = *m_cache;

    Amg::Vector2D constraintkey;
    if (constraint) {
      constraintkey.x() = constraint->position().x();
      constraintkey.y() = constraint->position().y();
    }
    else {
      constraintkey.x() = std::numeric_limits<double>::max();
      constraintkey.y() = std::numeric_limits<double>::min();
    }

    std::vector<Trk::DoubleAndWeight> ZPositions;

    std::lock_guard<std::mutex> lock (cache.m_mutex);
    if (ctx.evt() != cache.m_evt) {
      cache.m_weightMap.clear();
    }

    for (const Trk::TrackParameters* i : perigeeList)
    {
      const Perigee* iTrk = dynamic_cast<const Trk::Perigee*>(i);
      if (iTrk == nullptr)
      {
	  ATH_MSG_WARNING("Neutrals not supported for seeding. Rejecting track...");
	  continue;
      }

      Cache::key_t key (*iTrk, constraintkey);
      auto [it, flag] = cache.m_weightMap.try_emplace (key, Cache::value_t());
      if (flag) {
        it->second = estimateWeight (*iTrk, constraint);
        ATH_MSG_DEBUG("Found position z: " << it->second.first <<
                      " with weight " << it->second.second);
      }

      if (it->second.second >= 0.01)
      {
	ZPositions.push_back (it->second);
      }
    }

    return ZPositions;
  }


  /**
   * @brief Estimate z-position and weight for one track.
   */
  std::pair<double, double>
  ZScanSeedFinder::estimateWeight (const Trk::Perigee& iTrk,
                                   const xAOD::Vertex* constraint) const
  {
    // protect against overflow in exponential function
    static const double maxExpArg = log(std::numeric_limits<double>::max()/1.1);

    std::unique_ptr<const Trk::ImpactParametersAndSigma> ipas;
    if (constraint != nullptr && constraint->covariancePosition()(0,0)!=0) {
      ipas = std::unique_ptr<const Trk::ImpactParametersAndSigma> (m_IPEstimator->estimate (&iTrk, constraint));
    }

    std::pair<double, double> z0AndWeight;
    if (ipas != nullptr && ipas->sigmad0 > 0)
    {
      z0AndWeight.first = ipas->IPz0 + constraint->position().z();
      double chi2IP = std::pow(ipas->IPd0/ipas->sigmad0, 2);
      ATH_MSG_VERBOSE("d0 from tool: " << ipas->IPd0 << " error: " << ipas->sigmad0 << " chi2: " << chi2IP);
      if ( !m_disableAllWeights )
      {
        z0AndWeight.second = 1./(1.+exp(std::min((chi2IP-m_constraintcutoff)/m_constrainttemp, maxExpArg)));
      }
      else
      {
        z0AndWeight.second = 1.;
      }
    }
    else
    {
      if (constraint != nullptr) ATH_MSG_WARNING("Unable to compute impact parameter significance; setting IPWeight = 1");
      z0AndWeight.first = iTrk.position()[Trk::z];
      z0AndWeight.second = 1.;
    }

    // apply pT weighting as/if configured
    if (!m_disableAllWeights && ( m_usePt || m_useLogPt) )
    {
      double Pt = fabs(1./iTrk.parameters()[Trk::qOverP])*sin(iTrk.parameters()[Trk::theta]);
      if ( m_usePt )
      {
        z0AndWeight.second *= std::pow(Pt, m_expPt);
      }
      else
      {
        z0AndWeight.second *= log(std::max(Pt / m_minPt, 1.001));
      }
    }

    return z0AndWeight;
  }


  std::vector<Amg::Vector3D> ZScanSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */) const {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds");
    return std::vector<Amg::Vector3D>(0);

  }


  std::vector<Amg::Vector3D> ZScanSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* perigeeList */,const xAOD::Vertex * /* constraint */) const {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds");
    return std::vector<Amg::Vector3D>(0);

  }


}
