/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
/**@class MuidCaloEnergyTool
 AlgTool for estimating the energy loss of a muon and its error.
 The estimation can be based on the parameterization of the energy loss,
 on the measurement of the calorimeter or a combination of the two (the
 hybrid method).

 Please refer to:
 K. Nikolopoulos, D. Fassouliotis, C. Kourkoumelis, and A. Poppleton,
 "Event-by-Event Estimate of Muon Energy Loss in ATLAS",
 IEEE Trans. Nucl. Sci., vol. 54, no. 5, pt. 2, pp. 1792-1796, Oct. 2007.

  @author Konstantinos.Nikolopoulos@cern.ch, Alan.Poppleton@cern.ch
 (c) ATLAS Combined Muon software
*/
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDCALOENERGYTOOLS_MUIDCALOENERGYTOOL_H
#define MUIDCALOENERGYTOOLS_MUIDCALOENERGYTOOL_H

//<<<<<< INCLUDES                                                       >>>>>>

#include <atomic>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuidInterfaces/IMuidCaloEnergy.h"
#include "MuidInterfaces/IMuidCaloEnergyParam.h"
#include "MuidInterfaces/IMuidTrackIsolation.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Rec {

class CaloMeas;
class IMuidCaloEnergyMeas;
class IMuidCaloEnergyParam;
class IMuidTrackIsolation;

class MuidCaloEnergyTool : public AthAlgTool, virtual public IMuidCaloEnergy {

  public:
    MuidCaloEnergyTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuidCaloEnergyTool(void);  // destructor

    StatusCode initialize();
    StatusCode finalize();

    /**IMuidCaloEnergy interface:
       to get the total energyLoss in the calorimeters */
    CaloEnergy* energyLoss(double trackMomentum, double eta, double phi) const;

    /**IMuidCaloEnergy interface:
       TrackStateOnSurface for parameters and energyLoss at the calorimeter mid-surface */
    const Trk::TrackStateOnSurface* trackStateOnSurface(const Trk::TrackParameters& middleParameters,
                                                        const Trk::TrackParameters* innerParameters,
                                                        const Trk::TrackParameters* outerParameters) const;

  private:
    // private methods
    CaloEnergy* measurement(double trackMomentum, double eta, double phi, CaloMeas* caloMeas) const;
    double      muSpecResolParam(double trackMomentum, double eta) const;
    double      paramCorrection(double trackMomentum, double eta, double MopLoss, double MopSigma) const;
    double      landau(double x, double mpv, double sigma, bool norm) const;

    // helpers, managers, tools
    ToolHandle<IMuidCaloEnergyMeas> m_caloMeasTool{
        this,
        "CaloMeasTool",
        "Rec::MuidCaloEnergyMeas/MuidCaloEnergyMeas",
    };
    ToolHandle<IMuidCaloEnergyParam> m_caloParamTool{
        this,
        "CaloParamTool",
        "Rec::MuidCaloEnergyParam/MuidCaloEnergyParam",
    };
    ToolHandle<IMuidTrackIsolation> m_trackIsolationTool{
        this,
        "TrackIsolationTool",
        "Rec::MuidTrackIsolation/MuidTrackIsolation",
    };

    // configurable options
    bool m_cosmics;
    bool m_energyLossMeasurement;
    bool m_forceIsolationFailure;
    bool m_FSRtreatment;
    bool m_MOPparametrization;
    bool m_trackIsolation;

    // thresholds for use of energy measurement
    double m_emEtCut;         // minimum Et in em for FSR treatment
    double m_emF1Cut;         // minimum F1 in em for FSR treatment
    double m_emMinEnergy;     // mininum energy in the EM
    double m_hecMinEnergy;    // minimum energy in the HEC
    int    m_maxNTracksIso;   // max #tracks in the isolation cone
    double m_minFinalEnergy;  // minimum measured final energy
    double m_minMuonPt;       // minimum pt of the muon

    // counters (for finalize)
    mutable std::atomic_int m_countMean;         // number of tracks using mean
    mutable std::atomic_int m_countMeasurement;  // number of tracks using measurement
    mutable std::atomic_int m_countMop;          // number of tracks using mop
};

}  // namespace Rec

#endif  // MUIDCALOENERGYTOOLS_MUIDCALOENERGYTOOL_H
