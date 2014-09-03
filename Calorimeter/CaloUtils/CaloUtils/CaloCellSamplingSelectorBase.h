/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCELLSAMPLINGSELECTORBASE_H
#define CALOUTILS_CALOCELLSAMPLINGSELECTORBASE_H

#include "CaloInterface/ICaloCellSelector.h"

#include "CaloGeoHelpers/CaloSampling.h"

#include <algorithm>
#include <iterator>

class CaloCell;

template<typename LIST>
class CaloCellSamplingSelectorBase : virtual public ICaloCellSelector
{
public:

  CaloCellSamplingSelectorBase();
  CaloCellSamplingSelectorBase(const LIST& samplings);

  virtual ~CaloCellSamplingSelectorBase();

  virtual bool accept(const CaloCell* pCell) const;

protected:

  LIST m_listOfSamplings;

  virtual bool 
  checkSampling(const CaloSampling::CaloSample& aSampling) const = 0;

};

template<typename LIST>
CaloCellSamplingSelectorBase<LIST>::CaloCellSamplingSelectorBase()
{ }

template<typename LIST>
CaloCellSamplingSelectorBase<LIST>::
CaloCellSamplingSelectorBase(const LIST& samplings)
{
  std::copy(samplings.begin(),samplings.end(),
	    std::back_insert_iterator<LIST>(m_listOfSamplings));
}

template<typename LIST>
CaloCellSamplingSelectorBase<LIST>::~CaloCellSamplingSelectorBase()
{ }

template<typename LIST>
bool
CaloCellSamplingSelectorBase<LIST>::accept(const CaloCell* pCell) const
{
  CaloSampling::CaloSample theSampling = pCell->caloDDE()->getSampling();
  return theSampling != CaloSampling::Unknown 
    ? this->checkSampling(theSampling)
    : false;
}
#endif
