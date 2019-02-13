/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    AthAlgTool(t,n,p),
    
    m_mode1dfinder("Trk::FsmwMode1dFinder", this),
    m_IPEstimator("Trk::TrackToVertexIPEstimator", this),
    m_disableAllWeights(false),
    m_constraintcutoff(9.),
    m_constrainttemp(1.),
    m_useLogPt(true),
    m_minPt(400.),
    m_usePt(false),
    m_expPt(1.),
    m_cacheWeights(true),
    m_cachedRunNumber(0),
    m_cachedEventNumber(0),
    m_cachedConstraintX(0.),
    m_cachedConstraintY(0.)
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
    declareInterface<IVertexSeedFinder>(this);
  }

  ZScanSeedFinder::~ZScanSeedFinder() {}

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


  Amg::Vector3D ZScanSeedFinder::findSeed(const std::vector<const Trk::Track*> & VectorTrk,const xAOD::Vertex * constraint) {
    
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

  Amg::Vector3D ZScanSeedFinder::findSeed(const std::vector<const Trk::TrackParameters*> & perigeeList,const xAOD::Vertex * constraint) {
  
    // protect against overflow in exponential function
    const double maxExpArg = log(std::numeric_limits<double>::max()/1.1);

    double ZResult=0.;

    // Check for new run/event/constraint
    if (!m_cacheWeights)
    {
      m_weightMap.clear();
    }
    else
    {
      SG::ReadHandle<xAOD::EventInfo> myEventInfo(m_eventInfoKey);
      if ( !myEventInfo.isValid() )
      {
	  ATH_MSG_ERROR("Failed to retrieve event information; clearing cached weights");
	  m_weightMap.clear();
	  m_cachedRunNumber = 0;
	  m_cachedEventNumber = 0;
	  if (constraint != 0)
	  {
	    m_cachedConstraintX = constraint->position().x();
	    m_cachedConstraintY = constraint->position().y();
	  }
	  else
	  {
	    m_cachedConstraintX = std::numeric_limits<double>::max();
	    m_cachedConstraintY = std::numeric_limits<double>::max();
	  }
      }
      else
      {
	  unsigned int ei_RunNumber = myEventInfo->runNumber();
	  unsigned int ei_EventNumber = myEventInfo->eventNumber();
	  if (ei_RunNumber != m_cachedRunNumber || 
	      ei_EventNumber != m_cachedEventNumber ||
	      ( (constraint != 0) && (constraint->position().x() != m_cachedConstraintX) ) ||
	      ( (constraint != 0) && (constraint->position().y() != m_cachedConstraintY) ))
	  {
	    ATH_MSG_DEBUG("Clearing cached weights due to new event and/or constraint");
	    m_weightMap.clear();
	    m_cachedRunNumber = ei_RunNumber;
	    m_cachedEventNumber = ei_EventNumber;
	    if (constraint != 0)
	    {
	      m_cachedConstraintX = constraint->position().x();
	      m_cachedConstraintY = constraint->position().y();
	    }
	    else
	    {
	      m_cachedConstraintX = std::numeric_limits<double>::max();
	      m_cachedConstraintY = std::numeric_limits<double>::max();
	    }
	  }
      }
    }

    //Prepare the vector of points, on which the 3d mode has later to be calculated
    std::vector<DoubleAndWeight> ZPositions;

    // Whether weighting or not, we want an accurate extrapolated z0 values

    for (auto i : perigeeList)
    {
      const Perigee* iTrk = dynamic_cast<const Trk::Perigee*>(i);
      if (iTrk == 0)
      {
	  ATH_MSG_WARNING("Neutrals not supported for seeding. Rejecting track...");
	  continue;
      }

      std::pair<double, double> z0AndWeight;
      if (m_cacheWeights && (m_weightMap.count(*iTrk) != 0) )
      {
	z0AndWeight = m_weightMap[*iTrk];
      }
      else
      {
	const Trk::ImpactParametersAndSigma* ipas = 0;
	if (constraint != 0 && constraint->covariancePosition()(0,0)!=0) ipas = m_IPEstimator->estimate(i, constraint);
	if (ipas != 0 && ipas->sigmad0 > 0)
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
	  if (constraint != 0) ATH_MSG_WARNING("Unable to compute impact parameter significance; setting IPWeight = 1");
	  z0AndWeight.first = iTrk->position()[Trk::z];
	  z0AndWeight.second = 1.;
	}
	if (ipas != 0) delete ipas;
	// apply pT weighting as/if configured
	if (!m_disableAllWeights && ( m_usePt || m_useLogPt) )
	{
	    double Pt = fabs(1./iTrk->parameters()[Trk::qOverP])*sin(iTrk->parameters()[Trk::theta]);
	    if ( m_usePt )
	    {
	      z0AndWeight.second *= std::pow(Pt, m_expPt);
	    }
	    else
	    {
	      z0AndWeight.second *= log(std::max(Pt / m_minPt, 1.001));
	    }
	}
	// cache the result
	m_weightMap[*iTrk] = z0AndWeight;
      }  // end of lookup/compute z0AndWeight
      ATH_MSG_DEBUG("Found position z: " << z0AndWeight.first << " with weight " << z0AndWeight.second);

      if (z0AndWeight.second >= 0.01)
      {
	ZPositions.push_back(z0AndWeight);
      }

    } // end of loop over perigeeList

    if ( ZPositions.size()>0 ) 
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

  std::vector<Amg::Vector3D> ZScanSeedFinder::findMultiSeeds(const std::vector<const Trk::Track*>& /* vectorTrk */,const xAOD::Vertex * /* constraint */) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds");
    return std::vector<Amg::Vector3D>(0);

  }

  std::vector<Amg::Vector3D> ZScanSeedFinder::findMultiSeeds(const std::vector<const Trk::TrackParameters*>& /* perigeeList */,const xAOD::Vertex * /* constraint */) {
 
    //implemented to satisfy inheritance but this algorithm only supports one seed at a time
    ATH_MSG_WARNING("Multi-seeding requested but seed finder not able to operate in that mode, returning no seeds");
    return std::vector<Amg::Vector3D>(0);

  }


  void ZScanSeedFinder::setPriVtxPosition(double /* vx */, double /* vy */) {
    //implemented to satisfy inheritance
  }

  int ZScanSeedFinder::perigeesAtSeed( std::vector<const Trk::TrackParameters*> * /*a */ ,
				       const std::vector<const Trk::TrackParameters*>&  /*b */) const{
      //implemented to satisfy inheritance
    return 0;
  }

  int ZScanSeedFinder::getModes1d(std::vector<float>& /* a */, std::vector<float> &  /*b */, 
				  std::vector<float>& /* c */, std::vector<float>& /* d */) const{
      //implemented to satisfy inheritance  
    return 0;
  }

  void ZScanSeedFinder::getCorrelationDistance( double & /* cXY*/, double & /*cZ */ ){
      //implemented to satisfy inheritance    
  }



}
