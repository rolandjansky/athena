/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_ATHTRUTHSELECTIONTOOL_H
#define INDETPHYSVALMONITORING_ATHTRUTHSELECTIONTOOL_H

/**
 * @file AthTruthSelectionTool.h
 * header file for truth selection in this package
 * @author shaun roe
 * @date 10 October 2016
 **/

// STL includes
#include <string>
#include "InDetPhysValMonitoring/IAthSelectionTool.h"
#include "xAODTruth/TruthParticle.h" // typedef, can't fwd declare
#include "AthenaBaseComps/AthAlgTool.h"
#include "CutFlow.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"


/// class to apply selection to xAOD::TruthParticles,required by validation
class AthTruthSelectionTool: virtual public IAthSelectionTool, public AthAlgTool {
public:
  AthTruthSelectionTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~AthTruthSelectionTool () {
    /*nop*/
  };
  StatusCode initialize() final;
  StatusCode finalize() final;
  bool accept(const xAOD::IParticle* particle) final;
  void clearCounters() final;
  std::vector<unsigned int> counters() const final;
  std::vector<std::string> names() const final;
  std::string str() const final;

private:
  CutFlow<xAOD::TruthParticle> m_cutFlow;
  // Cut values;
  float m_maxEta;
  float m_maxPt;
  float m_minPt;
  int m_maxBarcode;
  bool m_requireCharged;
  bool m_requireStatus1;
  // max decay radius for secondaries [mm];
  // set to within (Run2) pixel by default
  double m_maxProdVertRadius;
  int m_pdgId;
  bool m_grandparent;
  bool m_poselectronfromgamma;
  std::vector<unsigned int> m_counters;
  
  /* \defgroup Selection on extrapolated particle to cylinder or disk surface
   * @{
   */
  float m_radiusCylinder; ///< for cylinder topology: radius of cylinder
  float m_minZCylinder; ///< for cylinder topology: minimum |z|
  float m_maxZCylinder; ///< for cylinder topology: maximum |z|
  float m_zDisc; ///< for disk topology: Z position of disk
  float m_minRadiusDisc; ///< for disk topology: minimum radius
  float m_maxRadiusDisc; ///< for disk topology: maximum radius

  //cache surfaces
  mutable Trk::CylinderSurface* cylinder;
  mutable Trk::DiscSurface* disc1;
  mutable Trk::DiscSurface* disc2;


  /* @} */

  ///Too handle for truth-track extrapolation
  ToolHandle<Trk::IExtrapolator> m_extrapolator;
};

#endif
