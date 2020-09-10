/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CscBipolarStripFitter.h

#ifndef CscBipolarStripFitter_H
#define CscBipolarStripFitter_H
///////////////////////////////////////////
// K. Nikolopoulos
// V. Chernyatin and M. Schernau
// Oct 2007
//
// CSC Strip fitter using the bipolar waveform of the pulse
//
//
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CscClusterization/ICscStripFitter.h"
#include "GaudiKernel/ToolHandle.h"

class ICscCalibTool;
class CscIdHelper;
namespace Muon {
class CscStripPrepData;
}
namespace MuonGM {
class MuonDetectorManager;
}


class CscBipolarStripFitter : virtual public ICscStripFitter, public AthAlgTool {

  public:  // Ctors and dtor.
    // Constructor.
    CscBipolarStripFitter(std::string, std::string, const IInterface *);

    // Destructor.
    ~CscBipolarStripFitter();

  public:  // AlgTool methods
    // Initialization.
    StatusCode initialize();

    // Finalization.
    StatusCode finalize();

  public:  // Interface methods
    // Tell compiler not to hide other fit methods.
    using ICscStripFitter::fit;

    // Fit a list of sample charges using the calibration tool.
    // If that fit fails, the peak channel is used.
    Result fit(const ChargeList &charges, double samplingTime, Identifier &stripId) const;

  private:  // data
    // Pointer to muon geometry manager.
    const MuonGM::MuonDetectorManager *m_pmuon_detmgr;
    const CscIdHelper *                m_phelper;

    // Calibration tool.
    ToolHandle<ICscCalibTool> m_cscCalibTool{
        this,
        "cscCalibTool",
        "",
    };

    double FindInitValues(double *x, double *initValues, int *maxsample) const;
    double FindPow(double z) const;
    void   InvertMatrix(double matrix[][3], const int dim, int *) const;
    void   InvertSymmetric4x4(double W[][4]) const;
    void   Derivative(double A[][3], double fp[][1], double p0[][1], int imeas, int *meas) const;
    int    TheFitter(double *x, const double ex, double *initValues, int imeas, int *meas, int ipar, int *par,
                     double *chi2, double *result) const;

    // Job options.
    double m_qerr;       // Charge error for a successful fit.
    double m_terr;       // Time error for a succcessful fit.
    double m_qerr_fail;  // Charge error for a failed fit.
    double m_terr_fail;  // Time error for a failed fit.
    double m_qerrprop;   // Charge calibration error
    double m_n;
    double m_n2;
    double m_zmax;
    double m_bipolarNormalization;
    double m_tsampling;

    mutable double m_powcachez;
    mutable double m_powcachezn;
};

#endif
