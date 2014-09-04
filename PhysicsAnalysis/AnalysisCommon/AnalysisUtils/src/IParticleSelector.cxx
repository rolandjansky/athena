/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file      AnalysisUtils/src/IParticleSelector.cxx
 * @author    sss, S. Protopopescu
 * @date      Mar, 2010
 * @brief     A @c Selector class with @c IParticleContainer.
 */


#include "AnalysisUtils/IParticleSelector.h"
#include "AthenaKernel/errorcheck.h"


/**
 * @brief Standard Gaudi algorithm constructor.
 * @param name The algorithm name.
 * @param svcloc The service locator.
 */
IParticleSelector::IParticleSelector(const std::string& name,
                                     ISvcLocator* pSvcLocator)
  : SelectorBase<IParticleSelector>(name, pSvcLocator)
{
  declareProperty("Selectors",m_selectorHandles,"The Selectors");

  // Set defaults to do no selection.
  m_ptmin = -1e10;
  m_ptmax =  1e10;
  m_etamin = -1e10;
  m_etamax = 1e10;
  m_absetamin = 0;
  m_absetamax = 1e10;
}


/**
 * @brief Standard Gaudi @c initialize method.
 */
StatusCode IParticleSelector::initialize()
{
  CHECK( SelectorBase<IParticleSelector>::initialize() );
  CHECK( m_selectorHandles.retrieve() );
  return StatusCode::SUCCESS;
}


/**
 * @brief Test to see if a particle passes the selection.
 * @param link Link to the particle.
 */
bool IParticleSelector::accept (const IParticleLink& link)
{
  return accept (*link);
}


/**
 * @brief Test to see if a particle passes the selection.
 * @param part The particle
 */
bool IParticleSelector::accept (const Contained* part)
{
  // Apply basic kinematic selection.
  if(!pt_eta_range(part->pt(),part->eta())) return false;

  // Apply additional selection tools.
  for (size_t i = 0; i < m_selectorHandles.size(); i++) {
    // FIXME: ISelector::accept should take a const pointer!
    if (!m_selectorHandles[i]->getTool()->accept (const_cast<Contained*>(part)))
      return false;
  }

  return true;
}

