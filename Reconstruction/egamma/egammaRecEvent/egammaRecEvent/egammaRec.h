/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMARECEVENT_EGAMMA_H
#define EGAMMARECEVENT_EGAMMA_H
/**
  @class egammaRec
  Represent an egamma object for internal egamma usage
  during reconstruction
*/
// INCLUDE HEADER FILES:

#include "AthLinks/ElementLink.h"

// CaloCluster include
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

// TrackParticle Includes
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

// Vertex Includes
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

#include <array>
#include <vector>
class egammaRec
{
  ///////////////////////////////////////////////////////////////////
  // Public typedefs:
  ///////////////////////////////////////////////////////////////////
public:
  /** @brief Default constructor implemented*/
  egammaRec();
  
  /**
   * Default copy/move ctor/assignment dtor
   */
  egammaRec(const egammaRec&) = default;
  egammaRec(egammaRec&&) = default;
  egammaRec& operator=(const egammaRec&) = default;
  egammaRec& operator=(egammaRec&&) = default;
  ~egammaRec() = default;

  /// @brief  Return the number of xAOD::CaloClusters that define the electron candidate
  size_t getNumberOfClusters() const;

  /// @brief  Pointer to the xAOD::CaloCluster/s that define the electron candidate
  const xAOD::CaloCluster* caloCluster(size_t index = 0) const;

  /// @brief ElementLink to the xAOD::CaloCluster/s that match the electron candidate
  ElementLink<xAOD::CaloClusterContainer> caloClusterElementLink(size_t index = 0) const;

  /// @brief set Pointer to the xAOD::CaloCluster
  void setCaloClusters(const std::vector<ElementLink<xAOD::CaloClusterContainer>>& links);

  /// @brief Return the number xAOD::TrackParticles that match the electron candidate
  size_t getNumberOfTrackParticles() const;

  /// @brief Pointer to the xAOD::TrackParticle/s that match the electron candidate
  const xAOD::TrackParticle* trackParticle(size_t index = 0) const;

  /// @brief ElementLink to the xAOD::TrackParticle/s that match the electron candidate
  ElementLink<xAOD::TrackParticleContainer> trackParticleElementLink(size_t index = 0) const;

  /// @brief Set the ElementLinks to the xAOD::TrackParticle/s that match the electron candidate
  void setTrackParticles(const std::vector<ElementLink<xAOD::TrackParticleContainer>>& links);

  /// @brief Return the number xAOD::Vertex/vertices  that match the photon candidate
  size_t getNumberOfVertices() const;

  /// @brief Pointer to the xAOD::Vertex/es that match the photon candidate
  const xAOD::Vertex* vertex(size_t index = 0) const;

  /// @brief ElementLink to the xAOD::vertex/es that match the electron candidate
  ElementLink<xAOD::VertexContainer> vertexElementLink(size_t index = 0) const;

  /// @brief set Pointer to the xAOD::vertex/vertices that match  the photon candidate
  void setVertices(const std::vector<ElementLink<xAOD::VertexContainer>>& links);

  /// @brief Push back another vertex
  void pushBackVertex(const ElementLink<xAOD::VertexContainer>& vertexElementLink);

  /// @brief Push front another vertex
  void pushFrontVertex(const ElementLink<xAOD::VertexContainer>& vertexElementLink);

  /// @name matching variables for the best match track
  /// @{
  /** @brief deltaEta at pre sampler(0) -> 3rd sampling(3)*/
  std::array<double,4>  deltaEta () const; 
  /** @brief deltaPhi at pre sampler(0) -> 3rd sampling(3)*/
  std::array<double,4>  deltaPhi () const;
  /** @brief deltaPhi for rescaled momentum extrapolation
       from the perigee. */
  std::array<double,4>  deltaPhiRescaled () const;
  /** @brief deltaPhi from Last measurement*/
  double deltaPhiLast () const ;
  
  /** @brief set deltaEta at pre sampler(0) -> 3rd sampling(3)*/
  void setDeltaEta(const std::array<double, 4>& deltaEtas);

  /** @brief set Phi of track extrapolated at pre sampler(0) -> 3rd sampling(3)*/
  void setDeltaPhi(const std::array<double, 4>& deltaPhis);

  /** @brief set deltaPhi at sampling  for rescaled momentum extrapolation
    from the perigee. */
  void setDeltaPhiRescaled(std::array<double, 4>& deltaPhis);

  /** @brief set deltaPhi at sampling 2 from Last track Measurement*/
  void setDeltaPhiLast(double value);
  /// @}

  /// @name matching variables for the best match vertex
  /// @{
  /** @brief deltaEtaVtx  */
  float deltaEtaVtx() const;

  /** @brief deltaPhiVtx */
  float deltaPhiVtx() const;

  /** @brief set deltaEtaVtx */
  void setDeltaEtaVtx(float value);

  /** @brief set deltaPhiVtx */
  void setDeltaPhiVtx(float value);
  /// @}

private:
  std::vector<ElementLink<xAOD::CaloClusterContainer>> m_caloClusters;
  std::vector<ElementLink<xAOD::TrackParticleContainer>> m_trackParticles;
  std::vector<ElementLink<xAOD::VertexContainer>> m_vertices;

  std::array<double, 4> m_deltaEta{};
  std::array<double, 4> m_deltaPhi{};
  std::array<double, 4> m_deltaPhiRescaled{};

  double m_deltaPhiLast;
  float m_deltaEtaVtx;
  float m_deltaPhiVtx;
};
#include"egammaRec.icc"
#endif

