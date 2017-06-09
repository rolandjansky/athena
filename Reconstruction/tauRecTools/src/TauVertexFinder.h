/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUVERTEXFINDER_H
#define TAUREC_TAUVERTEXFINDER_H

#include "tauRecTools/TauRecToolBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODTracking/VertexContainer.h"
#include "JetEDM/TrackVertexAssociation.h"
#include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"

/////////////////////////////////////////////////////////////////////////////

/** 
 * @brief Associate a vertex (origin) to the tau candidate.
 * 
 *  The vertex has to be consistent with the origin of tau jet seed.
 *  The vertex is determined in the following order: TJVA -> first PV in vertex container -> 0
 *  Attention: all downstream tau variables will be calculated to the vertex set here!
 * 
 * @author KG Tan <Kong.Guan.Tan@cern.ch>
 * @author Felix Friedrich <Felix.Friedrich@cern.ch>
 */

class TauVertexFinder : public TauRecToolBase {
public:
  //-------------------------------------------------------------
  //! Constructor and Destructor
  //-------------------------------------------------------------
  TauVertexFinder(const std::string& name);
  ASG_TOOL_CLASS2(TauVertexFinder, TauRecToolBase, ITauToolBase);
  ~TauVertexFinder();

  //-------------------------------------------------------------
  //! Algorithm functions
  //-------------------------------------------------------------
  virtual StatusCode initialize();
  virtual StatusCode eventInitialize();
  virtual StatusCode execute(xAOD::TauJet& pTau);
  virtual StatusCode eventFinalize();
  virtual StatusCode finalize();

  virtual void cleanup(xAOD::TauJet* ) { }
  virtual void print() const { }


private:
  ElementLink<xAOD::VertexContainer>
  getPV_TJVA(const xAOD::TauJet& tauJet,
             const xAOD::VertexContainer& vertices,
             float& maxJVF);

  float getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks, const jet::TrackVertexAssociation* tva) const;
  // for online ATR-15665
  float getJetVertexFraction(const xAOD::Vertex* vertex, const std::vector<const xAOD::TrackParticle*>& tracks) const;      
  //-------------------------------------------------------------
  //! Convenience functions to handle storegate objects
  //-------------------------------------------------------------
  template <class T>
  bool openContainer(T* &container, std::string containerName, bool printFATAL=false);

  template <class T>
  bool retrieveTool(T &tool);

private:
  bool m_printMissingContainerINFO;
  ToolHandle< InDet::IInDetTrackSelectionTool > m_TrackSelectionToolForTJVA;

  // for online
  std::vector<const xAOD::Vertex*> m_matchedVertexOnline;
  //-------------------------------------------------------------
  //! Configureables
  //-------------------------------------------------------------
  bool m_useTJVA;
  std::string m_inputPrimaryVertexContainerName;
  std::string m_assocTracksName;
  std::string m_trackVertexAssocName;    
  // for online ATR-15665
  float m_transDistMax;
  float m_longDistMax;
  float m_maxZ0SinTheta;
};

#endif // not TAUREC_TAUVERTEXFINDER_H
