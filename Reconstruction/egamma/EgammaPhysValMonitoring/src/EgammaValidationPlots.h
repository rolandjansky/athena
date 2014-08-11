/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_EGAMMAVALIDATIONPLOTS_H
#define EGAMMAPHYSVALMONITORING_EGAMMAVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

class EgammaValidationPlots:public PlotBase {
    public:
      EgammaValidationPlots(PlotBase* pParent, std::string sDir);
      void fill(const xAOD::Electron& electron);
      void fill(const xAOD::Photon& photon);
      
      // Reco only information
      Trk::ParamPlots                 m_oElectronAllPlots;
      Trk::ParamPlots                 m_oElectronTruthPlots;

      Trk::ParamPlots                 m_oPhotonAllPlots;
      Trk::ParamPlots                 m_oPhotonTruthPlots;

};

#endif
