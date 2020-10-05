/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file
 * @class TrigVxSecondary
 * @brief An EF algorithm for secondary vertexing using offline tools.
 * Please look at the mainpage of this package 
 * @author Andrew John Lowe
 */

#ifndef INDETTRIGVXSECONDARY_VXSECONDARY_H
#define INDETTRIGVXSECONDARY_VXSECONDARY_H

#include "GaudiKernel/ToolHandle.h"

// Trigger specific stuff
#include "TrigInterfaces/FexAlgo.h"
#include "TrigInDetToolInterfaces/ITrigTrackJetFinderTool.h"

#include "VxSecVertex/VxSecVertexInfo.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;  // legacy trigger code

//class VxContainer;
class TLorentzVector;

namespace Trk {
  class Track;
  class VxSecVertexInfo;
  class VxCandidate;
  class RecVertex; 
}

namespace InDet {

  class TrackJetFinder;
  class ISecVertexInJetFinder;
  
  class TrigVxSecondary: public HLT::FexAlgo {

  public:
    TrigVxSecondary(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~TrigVxSecondary();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement*, HLT::TriggerElement* outputTE);    
    HLT::ErrorCode hltFinalize();

  private:

    /** @brief Get primary vertex. */
    HLT::ErrorCode getEFPrmVtxForFit(xAOD::Vertex const*& bestFitPriVertex,
				     HLT::TriggerElement* outputTE,
				     unsigned int& numPrimaryVertices,
				     float& xPrmVtx,float& yPrmVtx,float& zPrmVtx); 
    
    
    /** @brief Get primary vertex for beamspot. */
    HLT::ErrorCode getPrmVtxForFit(xAOD::Vertex const*& vertex,
				   HLT::TriggerElement* outputTE,
				   unsigned int& numPrimaryVertices,
				   float& xPrmVtx,float& yPrmVtx,float& zPrmVtx, int& status); 
    
    /** @brief TrigTrackJetFinder tool. */
    PublicToolHandle<ITrigTrackJetFinderTool> m_trackJetFinderTool
       {this,"TrigTrackJetFinderTool","TrigTrackJetFinderTool",""};

    std::string m_priVtxKey;
    std::string m_secVtxKey;

    unsigned int m_secVtx_numTrkTot;
    unsigned int m_secVtx_twoTrkTot;
    unsigned int m_secVtx_numTrkSV;
    unsigned int m_secVtx_numPV;
    unsigned int m_secVtx_numSV;
    float m_secVtx_mass;
    float m_secVtx_energy;

    /** @brief Reconstructed primary vertex x position. Unused */
    float m_xPrmVtx;

    /** @brief Reconstructed primary vertex y position. Unused */
    float m_yPrmVtx;

    /** @brief Reconstructed primary vertex z position. Used by TrackJetFinder */
    float m_zPrmVtx;

    /** @brief When using beamspot as primary vertex use this flag to respect the beamspot flag */
    bool m_useBeamSpotFlag;

    /** @brief Use beamspot for transverse and z-position from vertexing. */
     bool m_T2PrmVtxAtEF;

    /** @brief Which tracks used in the vertexing algorithm (1=SiTrack,2=IDScan, 11=EFID). */
    int m_algo_T2TrkForVtx;

    /** @brief switch to estimate the track impact parameter sign using the HLT jet direction (1) or the HLT track-jet direction (2) or the LVL1 jet RoI direction (3). */
    unsigned int m_useJetDirection;

    /** @brief Used to pass copy of tracks in TrackCollection to offline tool.
     * Offline tool interface expects std::vector<const Trk::Track*>, not DataVector<Trk::Track>
     */
    TLorentzVector* m_jetDirection;

    /** @brief Contains additional information not in VxCandidate.
     * Contains invariant mass, energy fraction, and number of two-track vertices
     */ 
    const Trk::VxSecVertexInfo* m_secVertexInfo;
    Trk::VxSecVertexInfoContainer* m_secVertexInfoContainer;
    //VxContainer* m_secondaryVxContainer;
    //xAOD::VertexContainer* m_secondaryVxContainer;

    /** @brief Sort vertices true/false switch.
     * Default is true. Set in InDetTrigVxSecondary_LoadTools.py
     */
    bool m_sortSecVxContainer;

    /** @brief Algorithm will loop over and run all offline vertexing tool handles contained in this array.
     * Offline tools set in InDetTrigVxSecondary_LoadTools.py
     */
    ToolHandleArray<InDet::ISecVertexInJetFinder> m_secVertexFinderToolsHandleArray;

    unsigned int m_nVxCandidates;
    unsigned int m_nVxContainers;
    unsigned int m_nVxSecVertexInfo;
    unsigned int m_nVxSecVertexInfoContainers;

  }; // end of TrigVxSecondary
} // end of namespace InDet

#endif
