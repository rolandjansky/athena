/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/IN4MSorterAlg.cxx
 * @author Mike Hance
 * @date Feb, 2011
 * @brief This is a template class, and can't be used directly as an Algorithm.
 *        Derive a non-templated class from this to actually use.
 */


#include "IN4MSorterAlg.h"
#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "egammaEvent/egamma.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
IN4MSorterAlgBase::IN4MSorterAlgBase (const std::string& name,
                                          ISvcLocator* svcloc)
  : AthAlgorithm (name, svcloc),
    m_getter (this)
{
  declareProperty ("OutputKey", m_outputkey,
                   "SG key for output container.");

  declareProperty ("Getter", m_getter,
                   "Getter instance for the input egamma objects.");
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode IN4MSorterAlgBase::initialize()
{
  CHECK( AthAlgorithm::initialize() );
  CHECK( m_getter.retrieve() );
  CHECK( m_getter->configureD3PD<INavigable4Momentum>() );

  return StatusCode::SUCCESS;
}


/**
 * @brief Standard Gaudi execute method.
 */
StatusCode IN4MSorterAlgBase::execute()
{
  CHECK (m_getter->reset() );

  while (const INavigable4Momentum* in4m = m_getter->next<INavigable4Momentum>()) {
    CHECK (this->addElement(in4m));
  }

  CHECK (this->commit());

  return StatusCode::SUCCESS;
}


bool IN4MEtSort::operator()(const INavigable4Momentum* a,
			    const INavigable4Momentum* b){
  volatile double apt = a->pt();
  volatile double bpt = b->pt();
  return bpt < apt;
}

} // namespace D3PD
