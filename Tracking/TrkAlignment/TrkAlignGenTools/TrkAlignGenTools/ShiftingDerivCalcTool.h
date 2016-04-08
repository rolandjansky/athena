/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_SHIFTINGDERIVCALCTOOL_H
#define TRK_SHIFTINGDERIVCALCTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "TrkAlignInterfaces/IDerivCalcTool.h"


#include <vector>
#include <map>

/** @file ShiftingDerivCalcTool.h
    @class ShiftingDerivCalcTool

    @brief Tool used to calculate total derivatives of residuals w.r.t. 
    alignment parameters by shifting chambers in various directions, 
    refitting tracks, and determining chi2 vs. chamber positions.  This 
    is used to calculate first derivatives.
    
    @author Robert Harrington <roberth@bu.edu>
    @date 1/5/08
*/

class TGraph;

namespace Trk {

  class TrackStateOnSurface;
  class Track;
  class IGlobalTrackFitter;
  //class ITrackFitter;
  class IAlignResidualCalculator;
  class AlignModule;
  class AlignTSOS;
  class AlignTrack;
  class AlignPar;
  class IAlignModuleTool;

  class ShiftingDerivCalcTool : virtual public IDerivCalcTool, public AthAlgTool {

  public:
    ShiftingDerivCalcTool(const std::string& type, const std::string& name,
                           const IInterface* parent);
    virtual ~ShiftingDerivCalcTool();

    StatusCode initialize();
    StatusCode finalize();

    /** sets derivatives of residuals w.r.t. alignment parameters for hits on track.*/
    bool setDerivatives(AlignTrack* track);

    void showStatistics() {}

    /** sets residual covariance matrix */
    bool setResidualCovMatrix(AlignTrack* alignTrack) const;

  protected:

    // protected typedefs
    typedef std::vector<Amg::VectorX> HitDerivative;
    typedef std::map<const TrackStateOnSurface*,HitDerivative*> DerivativeMap;
    typedef DerivativeMap::value_type DerivativePair;
    
    // protected methods
    Amg::VectorX getDerivatives(AlignTrack* alignTrack, 
                                int ipar, const AlignPar* alignPar,
                                Amg::VectorX& derivativeErr, bool& resetIPar,
                                double& actualSecondDerivative);

    void   setChi2VAlignParam(const AlignTrack* alignTrack, 
                              const AlignModule* module,
                              int nshifts=0);
    void   deleteChi2VAlignParam();

    double shiftSize(const AlignPar* alignPar) const;
    bool   setUnshiftedResiduals(AlignTrack* alignTrack);

  private:
    // private methods
    const Trk::Track* bestPerigeeTrack(const Track* track) const;
    bool scanShifts(const AlignTrack* alignTrack, 
    const std::vector<const AlignModule*>& alignModules);

    bool getAllDerivatives(AlignTrack* alignTrack, const AlignModule* alignModule,
                           std::vector<Amg::VectorX>& deriv_vec,
                           std::vector<Amg::VectorX>& derivErr_vec,
                           std::vector<double>& actualsecderiv_vec,
                           bool& resetIPar);

    // private variables
    ToolHandle<IGlobalTrackFitter> m_trackFitterTool;
    ToolHandle<IGlobalTrackFitter> m_SLTrackFitterTool;
    ToolHandle<IGlobalTrackFitter> m_fitter;
    //ToolHandle<ITrackFitter> m_trackFitterTool;
    //ToolHandle<ITrackFitter> m_SLTrackFitterTool;
    //ToolHandle<ITrackFitter> m_fitter;
    
    ToolHandle<IAlignResidualCalculator> m_residualCalculator;
    ToolHandle<IAlignModuleTool> m_alignModuleTool;

    double m_traSize;
    double m_rotSize;

    bool m_runOutlierRemoval; 
    ParticleHypothesis m_particleHypothesis;

    int m_particleNumber;

    DerivativeMap m_derivative_map;
  
    bool   m_doFits;
    int    m_nFits;
    bool   m_doChi2VAlignParamMeasType;
    int    m_nChamberShifts;
    bool   m_doResidualPlots;
    int    m_nIterations;

    Amg::VectorX* m_unshiftedResiduals;
    Amg::VectorX* m_unshiftedResErrors;

    // stores double** for each module that track passes through
    std::vector<double**> m_chi2VAlignParamVec;  //!< track chi2[idof][ichambershift]
    std::vector<double**> m_chi2VAlignParamXVec; //!< chamber shift[idof][ichambershift]

    double** m_tmpChi2VAlignParam;           
    double** m_tmpChi2VAlignParamX;
    double*** m_tmpChi2VAlignParamMeasType;

    // stores double*** for each module that track passes through 
    // (one double** for each TrackState::MeasurementType)
    std::vector<double***> m_chi2VAlignParamVecMeasType;  //!< track chi2[idof][imeastype][ichambershift]

    double  m_unshiftedTrackChi2; 
    double* m_unshiftedTrackChi2MeasType;

    double m_trackAlignParamCut; //!< cut on value of track alignment parameter, determined from fit of chi2 vs. align parameters to a quadratic

    bool m_setMinIterations; //!< fit track with AlignModules shifted up and down in each extreme, find the number of iterations fitter uses to converge.  Set this number for all subsequent track refits. 

    int m_maxIter; //!< reject track if exceed maximum number of iterations

    int m_minIter; //!< set minimum number of iterations for first track fits

    bool m_removeScatteringBeforeRefit; //!< flag to remove scattering before refitting track

    int m_ntracksProcessed;           //!< number tracks processed
    int m_ntracksPassInitScan;        //!< number tracks pass initial scan
    int m_ntracksPassSetUnshiftedRes; //!< number tracks pass setting unshifted residuals
    int m_ntracksPassDerivatives;     //!< number tracks pass setting derivatives
    int m_ntracksPassGetDeriv;        //!< number tracks pass getting derivatives
    int m_ntracksPassGetDerivSecPass; //!< number tracks pass 2nd pass of getting derivatives
    int m_ntracksPassGetDerivLastPass; //!< number tracks pass 2nd pass of getting derivatives
    int m_ntracksFailMaxIter;
    int m_ntracksFailTrackRefit;
    int m_ntracksFailAlignParamCut;
    int m_ntracksFailFinalAttempt;

    bool m_secPass;

  }; // end class


} // end namespace

#endif // TRK_SHIFTINGDERIVCALCTOOL_H
