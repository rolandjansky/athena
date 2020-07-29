/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETPHYSVALMONITORING_INDETPERFPLOT_ALTNONPRIMTRACKING
#define INDETPHYSVALMONITORING_INDETPERFPLOT_ALTNONPRIMTRACKING
/**
 * @file InDetPerfPlot_AltNonprimTracking.h
 * @author nora pettersson (me!!!)
 **/


// std includes
#include <string>
#include <array>

// local includes
#include "InDetPlotBase.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTruth/TruthParticle.h"
#include "InDetPhysValMonitoring/ResolutionHelper.h"

///class holding Pt plots for Inner Detector RTT Validation and implementing fill methods
class InDetPerfPlot_AltNonprimTracking: public InDetPlotBase {
public:
  InDetPerfPlot_AltNonprimTracking(InDetPlotBase* pParent, const std::string& dirName);
  void fill(const xAOD::TrackParticle& particle);
  void fill(const xAOD::TrackParticle& particle, const float mu, const unsigned int nVtx);

  void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthie);
  void fill(const xAOD::TrackParticle& particle, const xAOD::TruthParticle& truthie, const float mu, const unsigned int nVtx);

  void fillEfficiency(const xAOD::TruthParticle& truth, const xAOD::TrackParticle& track, const bool isGood, const float mu, const unsigned int nVtx);
  void fillFakeRate(const xAOD::TrackParticle& track, const bool isAssociatedTruth, const float mu, const unsigned int nVtx);
  void fillUnlinked(const xAOD::TrackParticle& track, const bool isAssociatedTruth, const float mu, const unsigned int nVtx);

private:

//  IDPVM::ResolutionHelper m_resolutionHelper;
//  IDPVM::ResolutionHelper::methods m_resolutionMethod;

  TEfficiency *m_effANT_eta;
  TEfficiency *m_effSTD_eta;
  TEfficiency *m_effBAT_eta;
  TEfficiency *m_effANT_pt;
  TEfficiency *m_effSTD_pt;
  TEfficiency *m_effBAT_pt;
  TEfficiency *m_effANT_phi;
  TEfficiency *m_effSTD_phi;
  TEfficiency *m_effBAT_phi;
  TEfficiency *m_effANT_d0;
  TEfficiency *m_effSTD_d0;
  TEfficiency *m_effBAT_d0;
  TEfficiency *m_effANT_z0;
  TEfficiency *m_effSTD_z0;
  TEfficiency *m_effBAT_z0;
  TEfficiency *m_effANT_radius;
  TEfficiency *m_effSTD_radius;
  TEfficiency *m_effBAT_radius;
  TEfficiency *m_effANT_mu;
  TEfficiency *m_effSTD_mu;
  TEfficiency *m_effBAT_mu;
  TEfficiency *m_effANT_nvertices;
  TEfficiency *m_effSTD_nvertices;
  TEfficiency *m_effBAT_nvertices;


  TEfficiency *m_fakeANT_eta;
  TEfficiency *m_fakeSTD_eta;
  TEfficiency *m_fakeBAT_eta;
  TEfficiency *m_fakeANT_pt;
  TEfficiency *m_fakeSTD_pt;
  TEfficiency *m_fakeBAT_pt;
  TEfficiency *m_fakeANT_phi;
  TEfficiency *m_fakeSTD_phi;
  TEfficiency *m_fakeBAT_phi;
  TEfficiency *m_fakeANT_d0;
  TEfficiency *m_fakeSTD_d0;
  TEfficiency *m_fakeBAT_d0;
  TEfficiency *m_fakeANT_z0;
  TEfficiency *m_fakeSTD_z0;
  TEfficiency *m_fakeBAT_z0;
  TEfficiency *m_fakeANT_radius;
  TEfficiency *m_fakeSTD_radius;
  TEfficiency *m_fakeBAT_radius;
  TEfficiency *m_fakeANT_mu;
  TEfficiency *m_fakeSTD_mu;
  TEfficiency *m_fakeBAT_mu;
  TEfficiency *m_fakeANT_nvertices;
  TEfficiency *m_fakeSTD_nvertices;
  TEfficiency *m_fakeBAT_nvertices;




  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  // print out final stats on exact zeroes
  void finalizePlots();
};




#endif
