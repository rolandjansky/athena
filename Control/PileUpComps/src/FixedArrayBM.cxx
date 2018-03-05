/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<algorithm> /*count_if,max_element*/
#include<cassert>
#include<limits>    /*epsilon*/
#include<numeric>   /*accumulate*/
#include "UtilityFuncs.h"
#include "FixedArrayBM.h"

FixedArrayBM::FixedArrayBM(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc)
  , m_maxBunchCrossingPerOrbit(3564)
  , m_t0Offset(0)
  , m_allowEmptyT0BunchCrossing(false)
  , m_intensityPatternProp()
  , m_ipLength(1)
  , m_intensityPattern(new double[m_ipLength])
  , m_largestElementInPattern(1.0)
{
  declareProperty("MaxBunchCrossingPerOrbit", m_maxBunchCrossingPerOrbit, "The number of slots in each LHC beam. Default: 3564.");
  declareProperty("T0Offset", m_t0Offset,  "Offset of the T0 w.r.t. our intensity pattern" );
  declareProperty("AllowEmptyT0BunchCrossing", m_allowEmptyT0BunchCrossing,  "Allow the offset of the T0 to sit in an empty bunch crossing." );
  declareProperty("IntensityPattern", m_intensityPatternProp,
                  "An array of floats containing the beam intensity distribution as a function of time in bins of 25ns. FixedArrayBM normalizes the distribution and uses it as a stencil to determine the relative intensity at each beam xing in the simulated range"
                  );
  m_intensityPattern[0]=1.0;
}

FixedArrayBM::~FixedArrayBM()
{
  delete [] m_intensityPattern;
}

StatusCode FixedArrayBM::initialize()
{
  const std::vector<float>& rProp(m_intensityPatternProp.value());
  std::vector<float>::const_iterator pBegin(rProp.begin());
  std::vector<float>::const_iterator pEnd(rProp.end());
  m_ipLength = rProp.size();
  if(m_ipLength<1)
    {
      ATH_MSG_ERROR("IntensityPattern length (" << m_ipLength << ") is less than 1!");
      return StatusCode::FAILURE;
    }
  //Consistency checks
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

  for (unsigned int i(0); i<m_ipLength; ++i)
    {
      if (rProp[i]<0.0)
        {
          ATH_MSG_ERROR("All IntensityPattern elements must be >=0. Please fix element #" << i );
          return StatusCode::FAILURE;
        }
      m_intensityPattern[i] = rProp[i] * inv_maxElement; // this ensures that the elements are all in the range [0,1]
    }

  // Will be used to convert values in the m_intensityPattern
  // from having max value 1.0 to having mean value 1.0
  m_largestElementInPattern = (maxElement/denominator);

  //FIXME add a check that entry 0 is zero? In data, BCID=1 is always the first filled bunch.

  //check m_t0Offset is within allowed values
  if (m_t0Offset>=m_ipLength)
    {
      ATH_MSG_INFO("Requested T0 offset (" << m_t0Offset << ") does not lie within IntensityPattern provided. Resetting offset to " << (m_t0Offset % m_ipLength) << " as this will give the same result.");
      m_t0Offset = m_t0Offset % m_ipLength;
    }
  assert(m_intensityPattern[m_t0Offset % m_ipLength]>0.0); //just in case
  if (m_intensityPattern[m_t0Offset % m_ipLength]>0.0)
    {
      ATH_MSG_DEBUG( "T0 offset set to " << m_t0Offset << " for all events in this run." );
    }
  else
    {
      if (!(m_allowEmptyT0BunchCrossing.value()))
        {
          ATH_MSG_ERROR("Requested T0 offset (" << m_t0Offset << ") coincides with an empty bunch crossing in the IntensityPattern provided. Please re-submit your job and select an offset corresponding to a bunch crossing with non-zero luminosity.");
          return StatusCode::FAILURE;
        }
      else
        {
           ATH_MSG_INFO( "Allowing T0 to coincide with an empty bunch crossing in the IntensityPattern provided. T0 offset set to " << m_t0Offset << " for all events in this run." );
        }
    }
  return StatusCode::SUCCESS;
}

float FixedArrayBM::normFactor(int iXing) const
  {
    unsigned int index = static_cast<unsigned int>((((iXing + static_cast<int>(m_t0Offset)) % static_cast<int>(m_ipLength)) + static_cast<int>(m_ipLength) )  % static_cast<int>(m_ipLength));
    //The array itself has max-value 1.0, but we want it to have mean value 1.0 (for filled bunch crossings), so we multiple by the m_largestElementInPattern.
    ATH_MSG_VERBOSE("normFactor for BCID " << iXing
                    << " (offset " << m_t0Offset
                    << " index " << index
                    << ") is = " <<  m_largestElementInPattern*m_intensityPattern[ index ]);
    return m_largestElementInPattern*m_intensityPattern[ index ];
  }
