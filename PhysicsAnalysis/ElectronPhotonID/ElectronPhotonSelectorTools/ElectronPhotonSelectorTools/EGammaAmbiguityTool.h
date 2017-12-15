/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef __EGammaAmbiguityTool__
#define __EGammaAmbiguityTool__

/**
   @class EGammaAmbiguityTool
   @brief Electron / photon ambiguity resolution. Dual-use version

   @author Bruno Lenzi
   @date   May 2015
*/

// Atlas includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IEGammaAmbiguityTool.h"
#include "xAODEgamma/EgammaContainerFwd.h"

class EGammaAmbiguityTool : public asg::AsgTool,
			    virtual public IEGammaAmbiguityTool
{
  ASG_TOOL_CLASS2(EGammaAmbiguityTool, IEGammaAmbiguityTool, asg::IAsgTool)

public: 
  /** Standard constructor */
  EGammaAmbiguityTool(std::string myname);


  /** Standard destructor */
  virtual ~EGammaAmbiguityTool();


public:
  /** Gaudi Service Interface method implementations */
  virtual StatusCode initialize();

  /** Gaudi Service Interface method implementations */
  virtual StatusCode finalize();

  /** Return value: AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown */

  virtual unsigned int ambiguityResolve(const xAOD::CaloCluster* cluster,
                                        const xAOD::Vertex* vx,
                                        const xAOD::TrackParticle* tp, xAOD::AmbiguityTool::AmbiguityType& type) const;

  /** Return value: AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown 
      Needed because of cliets
      implementation calls method above
   */
  virtual unsigned int ambiguityResolve(const xAOD::CaloCluster* cluster,
                                        const xAOD::Vertex* vx,
                                        const xAOD::TrackParticle* tp) const;


  /** Redo the ambiguity resolution of central electrons and photons and return
    * AuthorElectron, AuthorPhoton, AuthorAmbiguous, AuthorUnknown 
    * or the author of the object if no overlapping object is found **/
  unsigned int ambiguityResolve(const xAOD::Egamma& egamma) const;

  /** Accept or reject egamma object based on ambiguity resolution 
    * (e.g. if object is a photon and ambiguity return value is electron -> reject) 
    **/
  bool accept( const xAOD::Egamma& egamma, bool acceptAmbiguous = true ) const;
  
  /** Return true if track has innermost pixel hit 
    * or next-to-innermost in case innermost is not expected
    * or at least m_MinNoPixHits pixel hits in case next-to-innermost is not expected
    **/
  bool hasInnermostPixelHit(const xAOD::TrackParticle& tp) const;

  /** Return the number of tracks with "innermost pixel hits" (see above) 
    * in the given vertex **/
  size_t nTrkWithInnermostPixelHits(const xAOD::Vertex& vx) const;
  
  /** Return true if the given TrackParticle is part of the vertex **/
  bool isInVertex(const xAOD::TrackParticle&, const xAOD::Vertex&) const;
  
  /** Return true if the vertex passes the requirement on Rconv - RfirstHit **/
  bool passDeltaR_innermost(const xAOD::Vertex& vx) const;

private:

  /** @brief Minimum number of silicon hits to be an electron and not a photon */
  int m_MinNoSiHits;
  /** @brief Minimum number of pixel hits to be an electron and not a photon */
  int m_MinNoPixHits;
  /** @brief Maximum EoverP, more that this is ambiguous */
  float  m_maxEoverPCut;
  /** @brief Minimum Pt, less that this is ambiguous */
  float m_minPtCut;
  /** @brief Maximum value for Rconv - RfirstHit for Si+Si conversions where both tracks have innermost hits **/
  float m_maxDeltaR_innermost;
  /** @brief Electron container name */
  std::string m_electronContainerName;
  /** @brief Photon container name */
  std::string m_photonContainerName;
  
}; // End: class definition

#endif
