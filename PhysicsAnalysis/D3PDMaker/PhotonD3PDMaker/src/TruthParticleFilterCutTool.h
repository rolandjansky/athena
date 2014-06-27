// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file PhotonD3PDMaker/src/TruthParticleFilterTool.h
 * @author Mike Hance <mike.hance@cern.ch>
 * @date August, 2010
 * @brief Filter truth particles for writing to D3PD.
 */


#ifndef PHOTOND3PDMAKER_TRUTHPARTICLEFILTERCUTTOOL_H
#define PHOTOND3PDMAKER_TRUTHPARTICLEFILTERCUTTOOL_H


#include "D3PDMakerUtils/SGKeyResolver.h"
#include "PhotonD3PDMaker/ITruthParticleFilterCutTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>
class McEventCollection;
namespace HepMC {
  class GenEvent;
  class GenVertex;
  class GenParticle;
}


namespace D3PD {


/**
 * @brief Filter truth particles based on a variety of attributes
 *
 */
class TruthParticleFilterCutTool
  : public ITruthParticleFilterCutTool, public AthAlgTool
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TruthParticleFilterCutTool (const std::string& type,
			      const std::string& name,
			      const IInterface* parent);


  /// Standard Gaudi @c queryInterface method.
  virtual StatusCode queryInterface( const InterfaceID& riid,
                                     void** ppvIf );

  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();

  /// Standard Gaudi finalize  method.
  virtual StatusCode finalize();

  /// Run the tool.
  virtual StatusCode execute();

  /// Here's the meat
  virtual bool passesCut( const HepMC::GenParticle* tp );

private:

  /// compare input PDG with m_pdgIDs, below
  bool check_PDG(unsigned int);

  /// PDG ID to keep
  std::vector<int> m_pdgIDs;
  
  /// lower pT cut
  float m_cut_lower_pT;
  
  /// upper pT cut
  float m_cut_upper_pT;
  
  /// lower eta cut
  float m_cut_lower_eta;
  
  /// upper eta cut
  float m_cut_upper_eta;
};


} // namespace D3PD


#endif // not TRUTHPARTICLEFILTERTOOL_H
