/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCELLSAMPLINGREJECTOR_H
#define CALOUTILS_CALOCELLSAMPLINGREJECTOR_H

#include "CaloUtils/CaloCellSamplingSelectorBase.h"

template<typename LIST>
class CaloCellSamplingRejector 
  : public CaloCellSamplingSelectorBase<LIST>
{

public:

  CaloCellSamplingRejector();
  CaloCellSamplingRejector(const LIST& samplings);
  virtual ~CaloCellSamplingRejector() { };

protected:

  virtual bool checkSampling(const CaloSampling::CaloSample& aSampling) const;
};

template<typename LIST>
CaloCellSamplingRejector<LIST>::CaloCellSamplingRejector()
  : CaloCellSamplingSelectorBase<LIST>()
{ }

template<typename LIST>
CaloCellSamplingRejector<LIST>::CaloCellSamplingRejector(const LIST&
							 samplings)
  : CaloCellSamplingSelectorBase<LIST>(samplings)
{ }


template<typename LIST>
bool 
CaloCellSamplingRejector<LIST>::checkSampling(const CaloSampling::CaloSample&
					      aSampling) const
{
  return 
    std::find(this->m_listOfSamplings.begin(),this->m_listOfSamplings.end(),aSampling)
    == this->m_listOfSamplings.end();
}
#endif
