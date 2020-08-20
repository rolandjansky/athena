/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_GXFTRAJECTORY_H
#define TRK_GXFTRAJECTORY_H

#include "TrkGlobalChi2Fitter/GXFTrackState.h"
#include "TrkGeometry/MagneticFieldProperties.h"

namespace Trk {
  class MeasurementBase;
  class MaterialEffectsBase;
  class Layer;
  class CylinderLayer;
  class DiscLayer;
  class MagneticFieldProperties;

/**
   * @brief Internal representation of the track, used in the track fit.

   @author thijs.cornelissen@cern.ch
  */

  class GXFTrajectory {

  public:
    GXFTrajectory();
    GXFTrajectory(GXFTrajectory & rhs);
    ~GXFTrajectory();
    GXFTrajectory & operator=(GXFTrajectory & rhs);

    bool addMeasurementState(GXFTrackState *, int index = -1);
    void addMaterialState(GXFTrackState *, int index = -1);

    void setReferenceParameters(const TrackParameters *);
    void setScatteringAngles(std::vector < std::pair < double, double > >&);
    void setTrackStates(std::vector < GXFTrackState * >&);
    void setBrems(std::vector<double> &);
    void setNumberOfPerigeeParameters(int);
    void setConverged(bool);
    void reset();
    void setPrefit(int);
    void setOutlier(int, bool isoutlier = true);
    void setPrevChi2(double);
    void setChi2(double);
    void setMass(double);

    std::pair<GXFTrackState *, GXFTrackState *> findFirstLastMeasurement(void);
    bool hasKink(void);

    int numberOfScatterers();
    void setNumberOfScatterers(int);
    int numberOfBrems();
    void setNumberOfBrems(int);
    int numberOfUpstreamStates();
    int numberOfUpstreamScatterers();
    int numberOfUpstreamBrems();
    int numberOfPerigeeParameters();
    int numberOfFitParameters();
    int numberOfSiliconHits();
    int numberOfTRTHits();
    int numberOfHits();
    int numberOfPseudoMeasurements();
    int numberOfOutliers();

    std::vector < GXFTrackState * >&trackStates();
    std::vector < std::pair < double, double >>&scatteringAngles();
    std::vector < std::pair < double, double >>&scatteringSigmas();
    std::vector<double> & brems();
    
    const TrackParameters *referenceParameters(bool takeownership = false);
    bool converged();
    int prefit();
    void resetReferenceParameters();
    double chi2() const;
    double prevchi2();
    int nDOF() const;
  
    Amg::VectorX & residuals();
    Amg::VectorX & errors();
    Amg::MatrixX & weightedResidualDerivatives();

    double totalX0();
    double totalEnergyLoss();

    double mass();
    GXFTrackState *caloElossState();

    std::vector < std::pair < const Layer *,
    const Layer *>>&upstreamMaterialLayers();

    void resetCovariances(void);
    std::unique_ptr<const FitQuality> quality(void) const;

    bool m_straightline;
    MagneticFieldProperties m_fieldprop = Trk::FullField;

  private:
    std::vector < GXFTrackState * >m_states;  //!< The vector of track states, i.e. measurements, scatterers, brem points, and holes
    int m_ndof;
    double m_chi2;
    double m_prevchi2;
    int m_nperpars;
    int m_nscatterers;
    int m_ncaloscatterers;
    int m_nbrems;
    int m_nupstreamstates;
    int m_nupstreamscatterers;
    int m_nupstreamcaloscatterers;
    int m_nupstreambrems;
    int m_nhits;
    int m_noutl;
    int m_nsihits;
    int m_ntrthits;
    int m_npseudo;
    int m_nmeasoutl;
    const TrackParameters *m_refpar;
    bool m_converged;
    std::vector < std::pair < double, double >>m_scatteringangles;
    std::vector < std::pair < double, double >>m_scatteringsigmas;
    std::vector<double> m_brems;
    Amg::VectorX m_res;
    Amg::VectorX m_errors;
    Amg::MatrixX m_weightresderiv;
    double m_totx0;
    double m_toteloss;
    double m_mass;
    bool m_ownrefpar;
    int m_prefit;
    GXFTrackState *m_caloelossstate;
    std::vector < std::pair < const Layer *, const Layer *>>m_upstreammat;
  };
}
#endif
