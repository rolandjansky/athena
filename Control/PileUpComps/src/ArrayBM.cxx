/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<algorithm> /*count_if,max_element*/
#include<cassert>
#include<cmath>     /*ceil,floor*/
#include<numeric>   /*accumulate*/
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandGeneral.h"
#include "UtilityFuncs.h"
#include "ArrayBM.h"

ArrayBM::ArrayBM(const std::string& name,ISvcLocator* svc): 
  AthService(name,svc),
  m_maxBunchCrossingPerOrbit(3564),
  m_t0Offset(0),
  m_intensityPatternProp(),
  m_ipLength(1),
  m_intensityPattern(new double[m_ipLength]),
  m_biRandom(0),
  m_atRndmGenSvc("AtRanluxGenSvc", name),
  m_largestElementInPattern(1.0)
{
  declareProperty("MaxBunchCrossingPerOrbit", m_maxBunchCrossingPerOrbit, "The number of slots in each LHC beam. Default: 3564.");
  declareProperty("IntensityPattern", m_intensityPatternProp,
		  "An array of floats containing the beam intensity distribution as a function of time in bins of 25ns. ArrayBM normalizes the distribution and uses it as a stencil to determine the relative intensity at each beam xing in the simulated range"
		  );
  declareProperty("RandomSvc", m_atRndmGenSvc, "The random number service that will be used.");  
  m_intensityPattern[0]=1.0;
}

ArrayBM::~ArrayBM() { 
  delete [] m_intensityPattern; 
  delete m_biRandom;
}

StatusCode
ArrayBM::initialize() {
  StatusCode sc(m_atRndmGenSvc.retrieve());
  if (sc.isSuccess()) {  
    const std::vector<float>& rProp(m_intensityPatternProp.value());
    std::vector<float>::const_iterator pBegin(rProp.begin());
    std::vector<float>::const_iterator pEnd(rProp.end());
    m_ipLength = rProp.size();
    // Consistency checks
    if (m_ipLength>m_maxBunchCrossingPerOrbit)
      {
	ATH_MSG_ERROR("IntensityPattern length (" << m_ipLength << "), exceeds the maximum number of bunch crossings per orbit (" << m_maxBunchCrossingPerOrbit << ").");
	return StatusCode::FAILURE;
      }

    // Normalise the pattern so that the non-zero elements average to 1.0
    float nonZeroElementCount(static_cast<float>(std::count_if(pBegin, pEnd, IsNonZero)));
    if(nonZeroElementCount<1.0)
      {
	ATH_MSG_ERROR("IntensityPattern has no non-zero elements!");
	return StatusCode::FAILURE;
      }
    float elementSum(static_cast<float>(std::accumulate(pBegin, pEnd,0.0)));
    float denominator(elementSum/nonZeroElementCount);

    // Normalise the pattern so that the highest element value is 1.0
    float maxElement(*(std::max_element(pBegin, pEnd)));
    float inv_maxElement = maxElement != 0 ? 1. / maxElement : 1;
    

    // Copy normalized intensity pattern from the property
    delete [] m_intensityPattern;
    m_intensityPattern = new double[m_ipLength];
    for (unsigned int i=0; i<m_ipLength; ++i) {
      if (rProp[i]<0.0) {
	ATH_MSG_ERROR("All IntensityPattern elements must be >=0. Please fix element #" << i );
	return StatusCode::FAILURE;
      }
      m_intensityPattern[i] = rProp[i] * inv_maxElement; // this ensures that the elements are all in the range [0,1]
    }

    // Will be used to convert values in the m_intensityPattern 
    // from having max value 1.0 to having mean value 1.0
    m_largestElementInPattern = (maxElement/denominator);

    //FIXME add a check that entry 0 is zero? In data, BCID=1 is always the first filled bunch.
    delete m_biRandom;
    //the engine is created if not there already
    m_biRandom = new CLHEP::RandGeneral(*(m_atRndmGenSvc->GetEngine("BEAMINT")),
				 m_intensityPattern,
				 m_ipLength,
				 /*IntType=*/1); //discrete distribution
  } else {
    ATH_MSG_ERROR("Can not retrieve random stream");
  }
  return sc;
}

void 
ArrayBM::selectT0() {
  //m_biRandom->shoot() returns in range [0,1]
  m_t0Offset = static_cast<unsigned int>(floor((m_biRandom->shoot() * m_ipLength)+0.5));
  assert(m_intensityPattern[m_t0Offset % m_ipLength]>0.0); //just in case

  ATH_MSG_DEBUG( "selectT0 offset for this event " << m_t0Offset );
}

StatusCode 
ArrayBM::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IBeamIntensity::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IBeamIntensity*)this;
  }  else  {
    // Interface is not directly available: try out the base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


