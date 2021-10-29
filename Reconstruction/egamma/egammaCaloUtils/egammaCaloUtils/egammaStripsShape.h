/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef EGAMMACALOUTILS_EGAMMASTRIPSSHAPE_H
#define EGAMMACALOUTILS_EGAMMASTRIPSSHAPE_H

/// @class egammaStripsShape
/// @brief   EM cluster shower shape calculations in 1st ECAL sampling
/// Calculate the width in the strip layer around the eta,phi of
/// the hottest cell in the **middle layer**.
///
/// @author Frederic Derue derue@lpnhe.in2p3.fr
/// @author Christos Anastopoulos
///

class CaloCellContainer;
class CaloDetDescrManager;
class LArEM_ID;

#include "CaloGeoHelpers/CaloSampling.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "GaudiKernel/StatusCode.h"
#include "xAODCaloEvent/CaloCluster.h"

struct egammaStripsShape
{
  struct Info
  {
  public:
    double etamax = 0;
    double phimax = 0;
    double etaseed = 0;
    double phiseed = 0;
    double wstot = -999.;
    double etot = 0;
    double etas3 = 0;
    double deltaEtaTrackShower = -999.;
    double deltaEtaTrackShower7 = -999.;
    double e132 = 0;
    double e1152 = 0;
    double ws3 = -999.;
    double ws3c = -999.;
    double poscs1 = -999.;
    double etaincell = -999.;
    double asymmetrys3 = -999.;
    double f1 = 0;
    double f1core = 0;
    double f2 = 0;
    double widths5 = -999.;
    double esec = 0;
    double val = 0;
    double fside = 0;
    double emaxs1 = 0;
    double emins1 = 0;
    double esec1 = 0;
    int ncetamax = -1;
    int ncetaseed = -1;
    bool success = false;
  };

  /** @brief AlgTool main method */
  static StatusCode execute(const xAOD::CaloCluster& cluster,
                     const CaloDetDescrManager& cmgr,
                     Info& info,
                     bool ExecAllVariables = true) ;
};

#endif
