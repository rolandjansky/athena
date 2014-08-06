// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerObjectAssociationTool.h 362406 2011-04-29 12:38:56Z krasznaa $
/**
 * @file TriggerD3PDMaker/TriggerObjectAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Associate to a trigger object using TrigObjectMatching.
 */


#ifndef TRIGGERD3PDMAKER_TRIGGEROBJECTASSOCIATIONTOOL_H
#define TRIGGERD3PDMAKER_TRIGGEROBJECTASSOCIATIONTOOL_H


#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "TrigObjectMatching/DistanceFunctor.h"
#include "EventKernel/INavigable4Momentum.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "boost/type_traits/integral_constant.hpp"
class TrigMatchTool;


namespace D3PD {


/**
 * @brief Rank results of trigger association.
 * @param tool The tool that calls us.
 * @param p0 The source object for the association.
 * @param p1 First object to compare.
 * @param p2 Second object to compare.
 *
 * When we associate to trigger objects, we may have several candidates
 * at the same (eta, phi).  Just picking the first one is bad, because
 * the ordering of these objects can change from run to run.  So we use
 * this to rank among objects with the same (eta, phi).
 *
 * This version takes an @c I4Momentum and ranks by @c pt.
 */
bool lessForTriggerAssociation (IAlgTool* tool,
                                const I4Momentum* p0,
                                const I4Momentum* p1,
                                const I4Momentum* p2);


/**
 * @brief Rank results of trigger association.
 * @param tool The tool that calls us.
 * @param p0 The source object for the association.
 * @param p1 First object to compare.
 * @param p2 Second object to compare.
 *
 * When we associate to trigger objects, we may have several candidates
 * at the same (eta, phi).  Just picking the first one is bad, because
 * the ordering of these objects can change from run to run.  So we use
 * this to rank among objects with the same (eta, phi).
 *
 * This version takes generic pointers, and doesn't do any ranking.
 * It only exists to avoid possible compilation errors.
 */
bool lessForTriggerAssociation (IAlgTool* tool,
                                const void* p0,
                                const void* p1,
                                const void* p2);


/**
 * @brief Associate to a trigger object using TrigObjectMatching.
 *
 * Properties:
 *  ChainPrefix: Consider all chains starting with this prefix.
 *  MatchTool:   The trigger object matching tool.
 */
template <typename FROM_T, typename TO_T>
class TriggerObjectAssociationTool
  : public SingleAssociationTool<FROM_T, TO_T>,
    public IIncidentListener
{
public:
  typedef SingleAssociationTool<FROM_T, TO_T> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TriggerObjectAssociationTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Create any needed tuple variables.
   */
  virtual StatusCode book();


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const TO_T* get (const FROM_T& p);


  /**
   * @brief Handle Gaudi incidents.
   * @param inc The incident to handle.
   */
  virtual void handle (const Incident& inc);

protected:
   /**
    * @brief Set a specific metric to be used in the matching
    * @param metric The special functor that defines the metric
    *
    * If a special metric is not set by the derived tool then the default
    * is used.
    */
   void setMetric( const DistanceFunctor< TO_T, FROM_T >* metric );

private:
   /// Property: The trigger object matching tool.
   ToolHandle<TrigMatchTool> m_matchTool;
   /// Metric optionally set by the derived tool
   const DistanceFunctor< TO_T, FROM_T >* m_metric;

  /// Property: Incident service.
  ServiceHandle<IIncidentSvc> m_incSvc;

  /// Property: Chain pattern.
  std::string m_chainPattern;

  /// Property: Only select objects that pass?
  bool m_onlyPassed;

  /// Property: Maximum DR for match.
  float m_maxDR;

  /// Property: Should we write the DR variable?
  bool m_writeDR;

  /// Variable: object matching DR
  float* m_dr;

  /// Cached list of trigger objects.
  std::vector<const TO_T*> m_cache;

  /// Cache valid flag.
  bool m_valid;
};


} // namespace D3PD



#include "TriggerD3PDMaker/TriggerObjectAssociationTool.icc"



#endif // not TRIGGERD3PDMAKER_TRIGGEROBJECTASSOCIATIONTOOL_H

