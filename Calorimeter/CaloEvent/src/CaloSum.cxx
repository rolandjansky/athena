/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/



#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloBinDescriptor.h"
#include "CaloEvent/CaloSum.h"

#include <vector>
#include <algorithm>

/////////////////////////////////
// Constructors and Destructor //
/////////////////////////////////

CaloSum::CaloSum() :   m_defaultRanges(true)
		   , m_numberOfSums(0)
		   , m_varType(UnknownType)
		   , m_binDescriptor(size_t(0),0.,0.) {}


CaloSum::CaloSum(CaloVarType                      varType,
		 const CaloBinDescriptor<double>& theBins,
		 size_t                           numberOfSums)
  : m_defaultRanges(true)
    , m_numberOfSums(numberOfSums)
    , m_varType(varType)
    , m_binDescriptor(theBins)
{
  this->setUp();
}
//
//CaloSum::CaloSum(CaloVarType varType,
//		 const CaloBinDescriptor<double>& theBins,
//		 const std::vector<CaloRegion>&   theRegions,
//		 size_t                           numberOfSums)
//  : m_defaultRanges(true)
//    , m_numberOfSums(numberOfSums)
//    , m_varType(varType)
//    , m_caloBinDescriptor(theBins)
//{
//  this->setUp();
//}

CaloSum::~CaloSum()
{ }

///////////////////
// Configuration //
///////////////////

bool CaloSum::setRegion(const CaloRegion& theRegion,
			double            etaLow, 
			double            etaHigh)
{
  std::vector<double> lowEtas;  lowEtas.push_back(etaLow);
  std::vector<double> highEtas; highEtas.push_back(etaHigh);
  return this->setRegion(theRegion,lowEtas,highEtas);
}

//
//bool CaloSum::setRegion(const std::vector<CaloSum::CaloRegion>& theRegions,
//			const std::vector<double>& etaLow,
//			const std::vector<double>& etaHigh)
//{
// //check internal state
//  if ( m_defaultRanges ) 
//    { 
//      m_defaultRanges = false;
//      m_regionRanges.clear();
//    }
//  // check input
//  if ( theRegions.size() != etaLow.size() ||
//       theRegions.size() != etaHigh.size() )
//    {
//      return false;
//    }
//
//  unsigned int i = 0;
//  bool bailOut   = false;
//  while ( ! bailOut && i<theRegions.size() )
//    {
//      std::vector<double> lowEtas  = etaLow[i];
//      std::vector<double> highEtas = etaHigh[i];
//      bailOut = ! this->setRegion(theRegions[i],lowEtas,highEtas);
//      i++;
//    }
//
//  return ! bailOut;
//}

bool CaloSum::setRegion(const CaloRegion&          theRegion,
			const std::vector<double>& lowEtas,
			const std::vector<double>& highEtas)
{
  // check matching vectors
  if ( lowEtas.size() != highEtas.size() || lowEtas.empty() )
    {
      return false;
    }
  // check on defaults
  if ( m_defaultRanges ) 
    {
      m_regionRanges.clear();
      m_defaultRanges = false;
    }

  // store eta ranges
  for (unsigned int i=0; i<lowEtas.size(); i++ )
    {
      range_t theRange(lowEtas[i],highEtas[i]);
      (m_regionRanges[theRegion]).push_back(theRange);
    }
  return !m_regionRanges.empty();
}

CaloSum::CaloVarType CaloSum::getVarType() const
{
  return m_varType;
}

CaloSum::CaloRegion CaloSum::getRegion(double theEta) const
{
  range_map_t::const_iterator fRegion = m_regionRanges.begin();
  bool isFound = false;
  while ( ! isFound && fRegion != m_regionRanges.end() )
    {
      range_cont_t::const_iterator fRange = ((*fRegion).second).begin();
      while ( ! isFound && fRange != ((*fRegion).second).end() )
	{
	  double lowEdge  = (*fRange).first;
	  double highEdge = (*fRange).second;
	  isFound = ( lowEdge <= theEta && theEta < highEdge );
	  ++fRange;
	}
      ++fRegion;
    }
  return isFound ? (*(--fRegion)).first : UnknownRegion ;
}

bool CaloSum::getRange(const CaloRegion&    theRegion,
		       std::vector<double>& lowEtas,
		       std::vector<double>& highEtas) const
{
  range_map_t::const_iterator fRegion = m_regionRanges.find(theRegion);
  if ( fRegion != m_regionRanges.end() )
    {
      lowEtas.resize(((*fRegion).second).size());
      highEtas.resize(((*fRegion).second).size());
      for ( unsigned int i=0; i<((*fRegion).second).size(); i++ )
	{
	  lowEtas[i]  = (((*fRegion).second)[i]).first;
	  highEtas[i] = (((*fRegion).second)[i]).second; 
	}
      return true;
    }
  else
    {
      return false;
    }
}
//
//bool CaloSum::getRange(double theEta, 
//		       std::vector<double>& lowEtas,
//		       std::vector<double>& highEtas)
//{
// CaloRegion theRegion = this->getRegion(theEta);
//  return this->getRange(theRegion,lowEtas,highEtas);
//}

size_t CaloSum::getNumberOfBins() const
{
  return m_binDescriptor.getNumberOfBins();
}

size_t CaloSum::getNumberOfSums() const
{
  //  return m_sumStore.size() > 0 
  //   ? ((m_sumStore.begin()).second).size()
  //    : 0;
  return m_numberOfSums;
}

bool CaloSum::getVariableRange(double& lowerLimit, double& upperLimit) const
{
  lowerLimit = m_binDescriptor.lowerBound();
  upperLimit = m_binDescriptor.upperBound();
  return lowerLimit < upperLimit;
}

bool CaloSum::getBinEdges(size_t theIndex, 
			  double& lowerLimit, 
			  double& upperLimit) const
{
  lowerLimit = m_binDescriptor.lowerBinEdge(theIndex);
  upperLimit = m_binDescriptor.upperBinEdge(theIndex);
  return lowerLimit < upperLimit;
}

bool CaloSum::getBinCenter(size_t theIndex, double& binCenter) const
{
  double lowEdge, highEdge;
  if ( this->getBinEdges(theIndex,lowEdge,highEdge) ) 
    {
      binCenter = ( highEdge + lowEdge ) / 2.;
      return true;
    }
  else
    {
      return false;
    }
}

const CaloBinDescriptor<double>& CaloSum::getBinDescriptor() const
{
  return m_binDescriptor;
}

///////////////////////
// Data Accumulation //
///////////////////////

// extract everything from CaloCell
bool CaloSum::add(const CaloCell* theCell, 
		  double theBinnedVariable,
		  double theSummedVariable)
{
  std::vector<double> theSums; theSums.push_back(theSummedVariable);
  return this->add(theCell,theBinnedVariable,theSums);
}

bool CaloSum::add(const CaloCell* theCell,
		  double theBinnedVariable,
		  const std::vector<double>& theSummedVariables)
{
  // get sampling
  CaloSampling::CaloSample theSampling = theCell->caloDDE()->getSampling();
  // add up
  return this->add(theSampling,theCell->eta(),theBinnedVariable,
		   theSummedVariables);
}

// regions defined by eta ranges (explicit indices)
bool CaloSum::add(const CaloSampling::CaloSample& theSampling,
		  double theEta, 
		  double theBinnedVariable,
		  double theSummedVariable)
{
  std::vector<double> theSums; theSums.push_back(theSummedVariable);
  return this->add(theSampling,theEta,theBinnedVariable,theSums);
}

bool CaloSum::add(const CaloSampling::CaloSample& theSampling,
		  double theEta,
		  double theBinnedVariable,
		  const std::vector<double>& theSummedVariables)
{
  // find bin
  size_t theIndex = m_binDescriptor.getIndex(theBinnedVariable);
  if ( m_binDescriptor.outOfRange(theIndex) )
    {
      return false;
    }
  // get key
  key_t theKey = this->getKey(theEta,theSampling);
  // find in map and generate new if not found
  store_iter_t theAllocator = m_sumStore.find(theKey);
  if ( theAllocator == m_sumStore.end() )
    {
      data_t theDataVector; 
      theDataVector.resize(m_numberOfSums,0.);
      bin_t  theBinVector;  
      theBinVector.resize(m_binDescriptor.getNumberOfBins(),
			  theDataVector);
      m_sumStore[theKey] = theBinVector;
      theAllocator       = m_sumStore.find(theKey);
    }
  // sum up
  size_t theSize =
    (((*theAllocator).second)[theIndex]).size() <= theSummedVariables.size()
    ? (((*theAllocator).second)[theIndex]).size()
    : theSummedVariables.size();
  for ( unsigned int i=0; i<theSize; i++ )
    {
      (((*theAllocator).second)[theIndex])[i] += theSummedVariables[i];
    }

  return true;
}

/////////////////
// Data Access //
/////////////////

CaloSum::const_iterator CaloSum::begin() const
{
  return m_sumStore.begin();
}

CaloSum::const_iterator CaloSum::end() const
{
  return m_sumStore.end();
}

CaloSum::CaloRegion 
CaloSum::getRegion(const_iterator& theIterator) const
{
  return ((*theIterator).first).first;
}

CaloSampling::CaloSample
CaloSum::getSampling(const_iterator& theIterator) const
{
  return ((*theIterator).first).second;
}

bool CaloSum::getSums(const_iterator& theIterator,
		      size_t theIndex,
		      std::vector<double>& theSums) const
{
  // check index range
  if ( m_binDescriptor.outOfRange(theIndex) )
    {
      return false;
    } 
  // check sum vector
  theSums.resize( (((*theIterator).second)[theIndex]).size(), 0. );
  std::copy( (((*theIterator).second)[theIndex]).begin(),
	     (((*theIterator).second)[theIndex]).end(),
	     theSums.begin());
  return !theSums.empty();
}

bool CaloSum::getSums(const_iterator& theIterator,
		      double theBinnedVariable,
		      std::vector<double>& theSums) const
{
  return this->getSums(theIterator,
		       m_binDescriptor.getIndex(theBinnedVariable),
		       theSums);
}

bool CaloSum::getSums(const CaloRegion&     theRegion,
		      const CaloSampling::CaloSample& theSampling,
		      size_t                          theIndex,
		      std::vector<double>&            theSums) const
{
  // check basics
  if ( m_binDescriptor.outOfRange(theIndex) )
    {
      return false;
    }
  // find element
  key_t theKey = this->getKey(theRegion,theSampling);
  store_const_iter_t theAccessor = m_sumStore.find(theKey);
  if ( theAccessor != m_sumStore.end() )
    {
      theSums.resize((((*theAccessor).second)[theIndex]).size(),0.);
      std::copy((((*theAccessor).second)[theIndex]).begin(),
		(((*theAccessor).second)[theIndex]).end(),
		theSums.begin());
      return !theSums.empty();
    }
  return false;
}

bool CaloSum::getSums(const CaloRegion&     theRegion,
		      const CaloSampling::CaloSample& theSampling,
		      double                          theBinnedVariable,
		      std::vector<double>&            theSums) const
{
  return this->getSums(theRegion,
		       theSampling,
		       m_binDescriptor.getIndex(theBinnedVariable),
		       theSums);
}
//
//bool CaloSum::getSums(const CaloSampling::CaloSample& theSampling,
//		      double                          theEta,
//		      size_t                          theIndex,
//		      std::vector<double>&            theSums) const
//{
//  CaloRegion theRegion = this->getRegion(theEta);
//  return this->getSums(theRegion,theSampling,theIndex,theSums);
//}

bool CaloSum::contains(const CaloRegion& theRegion) const
{
  bool found = false;
  store_const_iter_t firstEntry = m_sumStore.begin();
  store_const_iter_t lastEntry  = m_sumStore.end();
  while ( firstEntry != lastEntry && ! found )
    {
      found = ((*firstEntry).first).first == theRegion;
      ++firstEntry;
    } 
  return found;
}

bool CaloSum::contains(const CaloSampling::CaloSample& theSampling) const
{
  bool found = false;
  store_const_iter_t firstEntry = m_sumStore.begin();
  store_const_iter_t lastEntry  = m_sumStore.end();
  while ( firstEntry != lastEntry && ! found )
    {
      found = ((*firstEntry).first).second == theSampling;
      ++firstEntry;
    } 
  return found;
}

bool CaloSum::contains(const CaloRegion&      theRegion,
		       const CaloSampling::CaloSample& theSampling) const
{
  return 
    m_sumStore.find(this->getKey(theRegion,theSampling)) != 
    m_sumStore.end();
}

/////////////////////
// Private Helpers //
/////////////////////

bool CaloSum::setUp()
{
  // pre-defined ranges
  range_t centralRange(-0.8,0.8);
  (m_regionRanges[Central]).push_back(centralRange);
  range_t endCapPos(0.8,2.5);
  range_t endCapNeg(-2.5,-0.8);
  (m_regionRanges[EndCap]).push_back(endCapPos);
  (m_regionRanges[EndCap]).push_back(endCapNeg);
  range_t forwardPos(2.5,5.0);
  range_t forwardNeg(-5.0,-2.5);
  (m_regionRanges[Forward]).push_back(forwardPos);
  (m_regionRanges[Forward]).push_back(forwardNeg);
  return true;
}

CaloSum::key_t CaloSum::getKey(const CaloRegion&      theRegion, 
			       const CaloSampling::CaloSample& theSampling) 
  const
{
  return key_t(theRegion,theSampling);
}

CaloSum::key_t CaloSum::getKey(double theEta,
			       const CaloSampling::CaloSample& theSampling) 
  const
{
  CaloRegion theRegion = this->getRegion(theEta);
  return this->getKey(theRegion,theSampling);
}

