// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file      AnalysisUtils/IParticleSelector.h
 * @author    sss, S. Protopopescu
 * @date      Mar, 2010
 * @brief     A @c Selector class with @c IParticleContainer.
 */


#ifndef ANALYSISUTILS_IPARTICLESELECTOR_H
#define ANALYSISUTILS_IPARTICLESELECTOR_H

#include "AnalysisUtils/SelectorBase.h"
#include "AnalysisUtils/ISelector.h"
#include "NavFourMom/IParticleContainer.h"
#include "ParticleEvent/IParticleLinks.h"
#include "GaudiKernel/ToolHandle.h"


/**
 * @brief A @c Selector class with @c IParticleContainer.
 *
 * The behavior of this class comes mainly from the @c SelectorBase class.
 *
 * This class filters an @c IParticleContainer and produces as output
 * a @c SelectedParticles object (essentially a bitmask of the entries
 * that pass) and/or a @c ParticleLinks<IParticleContainer> object
 * (essentially a @c vector of @c ElementLink's to @c IParticle objects).
 *
 * As input, we need the @c StoreGate key of an @c IParticleContainer.
 * We may also get either a @c SelectedParticles or a
 * @c ParticleLinks<IParticleContainer> object (but not both), representing
 * a selection to apply before doing our own.
 *
 * This class implements basic kinematic selections.  Additional
 * selections may be made my adding @c ISelector tools.
 * The default configuration does no selection.
 *
 * Properties:
 *   InputKey:               SG key of the input @c IParticleContainer object.
 *   InSelectedParticlesKey  SG key for a @c SelectedParticles object.
 *                           If specified, it is applied as a selection to
 *                           the input objects before making our own selection.
 *   InParticleLinksKey      SG key for a @c ParticleLinks<IParticleContainer>
 *                           object.
 *                           If specified, it is applied as a selection to
 *                           the input objects before making our own selection.
 *   OutSelectedParticlesKey SG key for the @c SelectedParticles object
 *                           to make containing the result of the selection.
 *   OutParticleLinksKey     SG key for the @c ParticleLinks<IParticleContainer>
 *                           object to make containing the result
 *                           of the selection.
 *   SelectAll               If true, don't apply the selections below.
 *   PtMin                   Minimum pt for selection.       (default: -1e10)
 *   PtMax                   Maximum pt for selection.       (default:  1e10)
 *   EtaMin                  Minimum eta for selection.      (default: -1e10)
 *   EtaMax                  Maximum eta for selection.      (default:  1e10)
 *   AbsEtaMin               Minimum abs(eta) for selection. (default:  0)
 *   AbsEtaMax:              Maximum abs(eta) for selection. (default:  1e10)
 *   Selectors               List of @c ISelector tools to call for each
 *                           particle.  All must accept it to accept
 *                           the particle for the selection.
 */
class IParticleSelector : public SelectorBase<IParticleSelector>
{
public:
  typedef IParticleContainer Container;
  typedef IParticle          Contained;
  typedef IParticleLinks     Links;
  typedef IParticleLink      Link;

  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  IParticleSelector(const std::string& name, ISvcLocator* pSvcLocator);


  /// Standard Gaudi initialize() method.
  virtual StatusCode initialize();


  /**
   * @brief Test to see if a particle passes the selection.
   * @param link Link to the particle.
   */
  bool accept(const IParticleLink& link);


  /**
   * @brief Test to see if a particle passes the selection.
   * @param part The particle
   */
  bool accept(const Contained* part); 


private:
  /// Property: List of additional selection tools.
  ToolHandleArray< ISelector > m_selectorHandles;
};


#endif // not ANALYSISUTILS_IPARTICLESELECTOR_H
