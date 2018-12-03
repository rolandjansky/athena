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

ArrayBM::ArrayBM(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc)
  , m_maxBunchCrossingPerOrbit(3564)
  , m_t0Offset(0)
  , m_intensityPatternProp()
  , m_ipLength(1)
  , m_intensityPattern(new double[m_ipLength])
  , m_biRandom(nullptr)
  , m_atRndmGenSvc("AtRanluxGenSvc", name)
  , m_largestElementInPattern(1.0)
  , m_signalPattern(nullptr)
{
  declareProperty("MaxBunchCrossingPerOrbit", m_maxBunchCrossingPerOrbit, "The number of slots in each LHC beam. Default: 3564.");
  declareProperty("IntensityPattern", m_intensityPatternProp,
                  "An array of floats containing the beam intensity distribution as a function of time in bins of 25ns. ArrayBM normalizes the distribution and uses it as a stencil to determine the relative intensity at each beam xing in the simulated range"
                  );
  declareProperty("RandomSvc", m_atRndmGenSvc, "The random number service that will be used.");
  declareProperty("EmptyBunchOption", m_emptyBunches=0, "Option for empty bunches.  0: normal treatment.  Positive N: first N BCIDs after filled.  Negative N: any empty BCID is allowed.");
  m_intensityPattern[0]=1.0;
}

ArrayBM::~ArrayBM()
{
  delete [] m_intensityPattern;
  delete m_biRandom;
}

StatusCode ArrayBM::initialize()
{
  ATH_CHECK(m_atRndmGenSvc.retrieve());

  // Need to copy to make modifications for empty bunches
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

  // Initialize the signal pattern if we need one different from the intensity pattern
  if (m_signalPattern) delete [] m_signalPattern;
  if (m_emptyBunches!=0){
    m_signalPattern = new double[m_ipLength];
  }
  // Modification for empty bunches option
  if (m_emptyBunches<0 || std::abs(m_emptyBunches)>static_cast<int>(m_ipLength)){
    // Easy case: Just flip all the bunches
    for (size_t i=0;i<m_ipLength;++i){
      if (rProp[i]>0.) m_signalPattern[i]=0.;
      else m_signalPattern[i]=1.;
    } // Loop over all bunches in the pattern
  } else if (m_emptyBunches>0){
    // Harder case: N BCIDs after filled
    int sinceFilled=0;
    // Set sinceFilled for the beginning of the pattern; assume we will not wrap (otherwise caught above)
    for (size_t i=m_ipLength-m_emptyBunches;i<m_ipLength;++i){
      if (rProp[i]>0) sinceFilled=0;
      else sinceFilled+=1;
    } // Done with the loop over previous BCIDs
    // Now do the loop setting the intensity pattern
    for (size_t i=0;i<m_ipLength;++i){
      if (rProp[i]>0){
        // Filled BCID.  Reset count, don't allow signal.
        sinceFilled=0;
        m_signalPattern[i]=0.;
      } else if (sinceFilled<m_emptyBunches){
        // First N BCIDs.  Increment count, allow signal.
        sinceFilled+=1;
        m_signalPattern[i]=1.;
      } else {
        // Beyond N BCIDs.  Increment count, don't allow signal.
        sinceFilled+=1;
        m_signalPattern[i]=0.;
      }
    } // Done with loop over previous BCIDs
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
  for (unsigned int i=0; i<m_ipLength; ++i)
    {
      if (rProp[i]<0.0)
        {
          ATH_MSG_ERROR("All IntensityPattern elements must be >=0. Please fix element #" << i );
          return StatusCode::FAILURE;
        }
      m_intensityPattern[i] = rProp[i] * inv_maxElement; // this ensures that the elements are all in the range [0,1]
    }
  // If we don't want to have signal in empty crossings, then our signal pattern is just the intensity pattern
  if (m_emptyBunches==0){
    m_signalPattern = m_intensityPattern;
  }

  // Will be used to convert values in the m_intensityPattern
  // from having max value 1.0 to having mean value 1.0
  m_largestElementInPattern = (maxElement/denominator);

  //FIXME add a check that entry 0 is zero? In data, BCID=1 is always the first filled bunch.
  delete m_biRandom;
  //the engine is created if not there already
  m_biRandom = new CLHEP::RandGeneral(*(m_atRndmGenSvc->GetEngine("BEAMINT")),
                                      m_signalPattern,
                                      m_ipLength,
                                      /*IntType=*/1); //discrete distribution
  return StatusCode::SUCCESS;
}

void ArrayBM::selectT0()
{
  //m_biRandom->shoot() returns in range [0,1]
  m_t0Offset = static_cast<unsigned int>(floor((m_biRandom->shoot() * m_ipLength)+0.5));
  assert(m_intensityPattern[m_t0Offset % m_ipLength]>0.0); //just in case
  ATH_MSG_DEBUG( "selectT0 offset for this event " << m_t0Offset );
  return;
}

float ArrayBM::normFactor(int iXing) const
{
  unsigned int index = static_cast<unsigned int>((((iXing + static_cast<int>(m_t0Offset)) % static_cast<int>(m_ipLength)) + static_cast<int>(m_ipLength) )  % static_cast<int>(m_ipLength));
  //The array itself has max-value 1.0, but we want it to have mean value 1.0 (for filled bunch crossings), so we multiple by the m_largestElementInPattern.
  ATH_MSG_VERBOSE("normFactor for BCID " << iXing
                  << " (offset " << m_t0Offset
                  << " index " << index
                  << ") is = " <<  m_largestElementInPattern*m_intensityPattern[ index ]);
  return m_largestElementInPattern*m_intensityPattern[ index ];
}
