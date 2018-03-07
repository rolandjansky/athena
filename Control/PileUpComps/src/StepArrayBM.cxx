/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<algorithm> /*count_if,max_element*/
#include<cassert>
#include<limits>    /*epsilon*/
#include<numeric>   /*accumulate*/
#include "UtilityFuncs.h"
#include "StepArrayBM.h"

StepArrayBM::StepArrayBM(const std::string& name,ISvcLocator* svc)
  : base_class(name,svc)
  , m_maxBunchCrossingPerOrbit(3564)
  , m_t0Offset(0)
  , m_signalOffset(0)
  , m_intensityPatternProp()
  , m_signalPatternProp()
  , m_ipLength(1)
  , m_spLength(1)
  , m_intensityPattern(new double[m_ipLength])
  , m_signalPattern(new bool[m_spLength])
  , m_largestElementInPattern(1.0)
{
  declareProperty("MaxBunchCrossingPerOrbit", m_maxBunchCrossingPerOrbit, "The number of slots in each LHC beam. Default: 3564.");
  declareProperty("IntensityPattern", m_intensityPatternProp,
                  "An array of floats containing the beam intensity distribution as a function of time in bins of 25ns. ArrayBM normalizes the distribution and uses it as a stencil to determine the relative intensity at each beam xing in the simulated range"
                  );
  declareProperty("SignalPattern", m_signalPatternProp, "An array of booleans to complement the IntensityPattern to indicate where the signal event should happen (i.e. which events to actually simulate). Default behaviour is to do EVERY bunch crossing."
                  );
  m_intensityPattern[0]=1.0;
  m_signalPattern[0]=true;
}

StepArrayBM::~StepArrayBM()
{
  delete [] m_intensityPattern;
  delete [] m_signalPattern;
}

StatusCode StepArrayBM::initialize()
{
  const std::vector<float>& rProp(m_intensityPatternProp.value());
  const std::vector<float>& sProp(m_signalPatternProp.value());
  std::vector<float>::const_iterator pBegin(rProp.begin());
  std::vector<float>::const_iterator pEnd(rProp.end());
  m_ipLength = rProp.size();
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

  //copy normalized intensity pattern from the property, and match signal pattern if requested
  delete [] m_intensityPattern;
  m_intensityPattern = new double[m_ipLength];

  for (unsigned int i(0); i<m_ipLength; ++i)
    {
      if (rProp[i]<0.0)
        {
          ATH_MSG_ERROR("All IntensityPattern elements must be >=0. Please fix element #" << i );
          return StatusCode::FAILURE;
        }
      m_intensityPattern[i] = rProp[i] * inv_maxElement;
    }

  // Will be used to convert values in the m_intensityPattern
  // from having max value 1.0 to having mean value 1.0
  m_largestElementInPattern = (maxElement/denominator);

  //FIXME add a check that entry 0 is zero? In data, BCID=1 is always the first filled bunch.

  //get the signal pattern from the property
  m_spLength = sProp.size();
  delete [] m_signalPattern;
  m_signalPattern = new bool[m_spLength];
  bool isOk=false;
  for (unsigned int i(0); i<m_spLength; ++i)
    {
      m_signalPattern[i] = (sProp[i]>0.0);
      if(m_signalPattern[i]) { isOk=true; }
    }
  if(!isOk)
    {
      ATH_MSG_ERROR("SignalPattern MUST have at least one non-zero value in it.");
      return StatusCode::FAILURE;
    }

  //start the t0Offset and signalOffset at the LAST entry in the arrays, so that the first event is for the FIRST entry in the arrays
  m_t0Offset = m_ipLength-1;
  m_signalOffset = m_spLength-1;

  return StatusCode::SUCCESS;
}

void StepArrayBM::selectT0()
{
  //move to the next xing as dictated by the signalPattern, for each incremember, shift the t0Offset
  do {
    m_signalOffset = (m_signalOffset+1) % m_spLength;
    m_t0Offset = (unsigned int)((m_t0Offset+1) % m_ipLength);
  } while(!m_signalPattern[m_signalOffset]);


  //assert(m_intensityPattern[m_t0Offset % m_ipLength]>0.0); //commented out to allow events with no intensity

  ATH_MSG_DEBUG( "selectT0 offset for this event " << m_t0Offset );
}
