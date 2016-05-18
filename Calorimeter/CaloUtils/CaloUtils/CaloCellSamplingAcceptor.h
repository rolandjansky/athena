/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOUTILS_CALOCELLSAMPLINGACCEPTOR_H
#define CALOUTILS_CALOCELLSAMPLINGACCEPTOR_H

#include "CaloUtils/CaloCellSamplingSelectorBase.h"

template<typename LIST>
class CaloCellSamplingAcceptor 
  : public CaloCellSamplingSelectorBase<LIST>
{

public:

  CaloCellSamplingAcceptor();
  CaloCellSamplingAcceptor(const LIST& samplings);
  virtual ~CaloCellSamplingAcceptor() { };

protected:

  virtual bool checkSampling(const CaloSampling::CaloSample& aSampling) const;

};

template<typename LIST>
CaloCellSamplingAcceptor<LIST>::CaloCellSamplingAcceptor()
  : CaloCellSamplingSelectorBase<LIST>()
{ }

template<typename LIST>
CaloCellSamplingAcceptor<LIST>::CaloCellSamplingAcceptor(const LIST&
							 samplings)
  : CaloCellSamplingSelectorBase<LIST>(samplings)
{ }


template<typename LIST>
bool 
CaloCellSamplingAcceptor<LIST>::checkSampling(const CaloSampling::CaloSample&
					      aSampling) const
{
  return 
    std::find(this->m_listOfSamplings.begin(),this->m_listOfSamplings.end(),aSampling) 
    != this->m_listOfSamplings.end();
}
#endif
