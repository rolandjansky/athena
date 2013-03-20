// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id:  $
/**
 * @file TrigTauD3PDMaker/TrigL2TauTracksInfoAssTool.h
 * @author E. Ptacek <eptacek@uoregon.edu>
 * @date Feb, 2010
 * @brief Associate in L2 from a TrigTau object to a TauTracksInfo object using TrigReadNavTool.
 */


#ifndef TRIGTAUD3PDMAKER_TRIGL2TAUCLUSTERASSTOOL_H
#define TRIGTAUD3PDMAKER_TRIGL2TAUCLUSTERASSTOOL_H


#include "D3PDMakerInterfaces/ICollectionGetterTool.h"
#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "boost/type_traits/integral_constant.hpp"

#include "TrigParticle/TrigTau.h"
#include "TrigCaloEvent/TrigTauCluster.h"

#define TRIGREADNAVMATCH_FUNC isSamePtr
#include "TrigTauD3PDMaker/TrigReadNavTool.h"
#include "FourMomUtils/P4Helpers.h"
#include "AthenaKernel/errorcheck.h"
#include "boost/type_traits/is_same.hpp"


namespace TrigTauTool{ class TrigReadNavTool; }


namespace D3PD {


/**
 * @brief Associate to a TrigTauCluster object 
 * from a TrigTau object using TrigReadNavTool.
 *
 * Properties:
 *  ChainPrefix: Consider all chains starting with this prefix.
 *  MatchTool:   The trigger object matching tool.
 */
class TrigL2TauClusterAssTool
  : public SingleAssociationTool<TrigTau, TrigTauCluster>,
    public IIncidentListener
{
public:
  typedef SingleAssociationTool<TrigTau, TrigTauCluster> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TrigL2TauClusterAssTool (const std::string& type,
                              const std::string& name,
                              const IInterface* parent);


  /// Standard Gaudi initialize method.
  StatusCode initialize();


  /**
   * @brief Create any needed tuple variables.
   */
  StatusCode book();


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  const TrigTauCluster* get (const TrigTau& p);

  /**
   * @brief Handle Gaudi incidents.
   * @param inc The incident to handle.
   */
  void handle (const Incident& inc);

private:
  /// Property: The trigger object matching tool.
  ToolHandle<TrigTauTool::TrigReadNavTool> m_matchTool;

  /// Property: Incident service.
  ServiceHandle<IIncidentSvc> m_incSvc;

  /// Property: Chain pattern.
  std::string m_chainPattern;

};


} // namespace D3PD

#endif // not TRIGTAUD3PDMAKER_TRIGL2TAUCLUSTERASSTOOL_H

