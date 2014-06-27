// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthExplorerAlg.h 502492 2012-05-25 13:19:11Z jmarchan $
/**
 * @file PhotonD3PDMaker/src/TruthExplorerAlg.h
 * @author Mike Hance <mike.hance@cern.ch>
 * @date March, 2011
 * @brief Calculate some truth quantities
 */


#ifndef PHOTOND3PDMAKER_TRUTHEXPLORERALG_H
#define PHOTOND3PDMAKER_TRUTHEXPLORERALG_H


#include "D3PDMakerUtils/UDAlgBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "PhotonAnalysisUtils/IPAUtruthTool.h"
#include "PhotonAnalysisUtils/IPAUcaloIsolationTool.h"
#include <vector>
#include <string>
#include <map>
#include <set>

namespace D3PD {


class ICollectionGetterTool;


/**
 * @brief Calculate isolation cones of topo clusters for photons.  Save as UD.
 */
class TruthExplorerAlg
  : public UDAlgBase
{
public:
  /**
   * @brief Standard Gaudi algorithm constructor.
   * @param name The algorithm name.
   * @param svcloc The service locator.
   */
  TruthExplorerAlg (const std::string& name,
		    ISvcLocator* svcloc);


  /// Standard Gaudi @c initialize method.
  virtual StatusCode initialize();


  /// Standard Gaudi @c execute method.
  virtual StatusCode execute();


private:
  /// Property: Getter for input photons.
  ToolHandle<ICollectionGetterTool> m_photonGetter;

  /// Property: Getter for input electrons.
  ToolHandle<ICollectionGetterTool> m_electronGetter;

  /// Property: Getter for truth particles
  ToolHandle<ICollectionGetterTool> m_truthParticleGetter;

  /// Property: truth tool
  ToolHandle<IPAUtruthTool> m_PAUtruthTool;

  float
    m_partonIsolation15,
    m_partonIsolation20,
    m_partonIsolation25,
    m_partonIsolation30,
    m_partonIsolation35,
    m_partonIsolation40,
    m_partonIsolation45,
    m_partonIsolation60,
    m_partonIsolation15_UE,
    m_partonIsolation20_UE,
    m_partonIsolation25_UE,
    m_partonIsolation30_UE,
    m_partonIsolation35_UE,
    m_partonIsolation40_UE,
    m_partonIsolation45_UE,
    m_partonIsolation60_UE,
    m_particleIsolation20,
    m_particleIsolation30,
    m_particleIsolation40,
    m_particleIsolation20_ED_corrected,
    m_particleIsolation30_ED_corrected,
    m_particleIsolation40_ED_corrected;

  int m_isHardProc, m_isBrem, m_isPromptPhoton;

  int m_isTruthConv;

  float m_XTruthConv,m_YTruthConv,m_ZTruthConv,m_RTruthConv;

  bool m_CalculateTruthCaloIsolation;
  ToolHandle<IPAUcaloIsolationTool> m_PAUTruthCaloIsolationTool;

  int m_RemoveNHardestJets;

  int m_mother_pdgId;

  StatusCode DecorateTruthParticle(const TruthParticle* tp);
  StatusCode DecorateTruthParticleWithZeroes(const TruthParticle* tp);

  std::set<const TruthParticle*> m_DecoratedTruthParticles;

};
  

} // namespace D3PD


#endif
