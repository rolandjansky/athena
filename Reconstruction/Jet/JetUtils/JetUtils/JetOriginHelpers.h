// emacs, this file is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUTIL_JETORIGINHELPERS_H
#define JETUTIL_JETORIGINHELPERS_H
////////////////////////////////////////////////
/// \file JetOriginHelpers.h
/// \brief Helpers to calculate corrected 4-vectors w.r.t to a given orign
///
/// A list of functions are defined to calculate the Jet 4-vector
/// w.r.t to a given origin (i.e vertex). 
/// The calculation is done from the the corrected constituents position
///  (or from the cells inside the constituents, not implemented yet).
/// The calculation is done at the constituent scale : it does not take the jet 4-vector
/// into account and so does not depend on the calibration of the jet.
/// 
///
////////////////////////////////////////////////////////////
#include "xAODJet/Jet.h"
#include "xAODTracking/VertexFwd.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

namespace jet {

  /// \brief returns  4-vector of the jet relative to the vertex
  /// \param jet the Jet 
  /// \param vx the Vertex relative to which to compute the returned 4-vector
  /// \return 4-vector corrected w.r.t vx
  ///
  /// The cluster constituents of jet are retrieved, set at the scale they were used to find
  /// jet, corrected according to vx. The 4-vector sum of this corrected cluster is returned
  xAOD::JetFourMom_t clusterOriginCorrection(const xAOD::Jet& jet, const xAOD::Vertex & vx, const xAOD::CaloCluster::State & state);

}

#endif
