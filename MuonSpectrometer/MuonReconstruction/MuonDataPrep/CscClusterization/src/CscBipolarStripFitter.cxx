/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CscBipolarStripFitter.cxx
#include "CscBipolarStripFitter.h"

#include "CscCalibTools/ICscCalibTool.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

///////////////////////////////////////////
// K. Nikolopoulos
// V. Chernyatin and M. Schernau
// Oct 2007
//
// CSC Strip fitter using the bipolar waveform of the pulse
//
//

using Muon::CscStripPrepData;
using std::string;

typedef ICscStripFitter::Result     Result;
typedef ICscStripFitter::ChargeList ChargeList;

//**********************************************************************

CscBipolarStripFitter::CscBipolarStripFitter(string type, string aname, const IInterface *parent)
    : AthAlgTool(type, aname, parent),
      m_pmuon_detmgr(0),
      m_phelper(0),
      m_cscCalibTool(""),
      m_n(0),
      m_n2(0),
      m_zmax(0),
      m_bipolarNormalization(0),
      m_tsampling(0),
      m_powcachez(0),
      m_powcachezn(0)
{

    declareInterface<ICscStripFitter>(this);
    declareProperty("chargeError", m_qerr = 5500.0);
    declareProperty("timeError", m_terr = 5.0);
    declareProperty("failChargeError", m_qerr_fail = 5000.0);
    declareProperty("failTimeError", m_terr_fail = 50.0);
    declareProperty("chargeCalibrationError", m_qerrprop = 0.002);
    declareProperty("cscCalibTool", m_cscCalibTool);
}

//**********************************************************************

CscBipolarStripFitter::~CscBipolarStripFitter() {}

//**********************************************************************

StatusCode
CscBipolarStripFitter::initialize()
{

    ATH_MSG_DEBUG("Initializing " << name());

    ATH_MSG_DEBUG("Properties for " << name() << ":");
    ATH_MSG_DEBUG("              Charge error: " << m_qerr);
    ATH_MSG_DEBUG("                Time error: " << m_terr);
    ATH_MSG_DEBUG("         Fail charge error: " << m_qerr_fail);
    ATH_MSG_DEBUG("           Fail time error: " << m_terr_fail);
    ATH_MSG_DEBUG("  Charge calibration error: " << m_qerrprop);

    /** CSC calibratin tool for the Condtiions Data base access */
    if (m_cscCalibTool.retrieve().isFailure()) {
        ATH_MSG_ERROR("Can't get handle on CSC calibration tools");
        return StatusCode::RECOVERABLE;
    }
    // Retrieve the detector descriptor.
    if (detStore()->retrieve(m_pmuon_detmgr).isFailure()) {
        ATH_MSG_ERROR(" Cannot retrieve MuonGeoModel ");
        return StatusCode::RECOVERABLE;
    }
    ATH_MSG_DEBUG("Retrieved geometry.");
    m_phelper = m_pmuon_detmgr->cscIdHelper();


    m_n          = m_cscCalibTool->getNumberOfIntegration();   // 12.;
    m_n2         = m_cscCalibTool->getNumberOfIntegration2();  // 11.66;
    m_powcachez  = -9999.;
    m_powcachezn = -9999.;
    m_zmax       = m_n + m_n2 + 2. - sqrt((m_n + 2. + m_n2) * (m_n + 2. + m_n2) - 4. * m_n * (m_n2 + 1));
    m_zmax *= 0.5;
    // m_n+1 - sqrt(m_n+1.0);
    m_bipolarNormalization = FindPow(m_zmax) * (1 - m_zmax / (m_n2 + 1)) * exp(-m_zmax);
    // FindPow(m_zmax)*(1-m_zmax/(m_n+1))*exp(-m_zmax);
    m_tsampling = m_cscCalibTool->getSamplingTime() / 2.;  //   50/2 =   25.;


    return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode
CscBipolarStripFitter::finalize()
{
    ATH_MSG_DEBUG("Finalizing " << name());
    return StatusCode::SUCCESS;
}

//**********************************************************************

Result
CscBipolarStripFitter::fit(const ChargeList &chgs, double period, Identifier &stripId) const
{

    ATH_MSG_DEBUG("Fitting with sample time " << period);
    ATH_MSG_DEBUG(" Number of charges =" << chgs.size() << "  Charges: " << chgs);
    Result res;


    IdentifierHash stripHash;
    if (m_phelper->get_channel_hash(stripId, stripHash)) {
        ATH_MSG_WARNING("Unable to get CSC striphash id "
                        << " the identifier is ");
        stripId.show();
    }
    ATH_MSG_DEBUG("CalibCscStripFitter:: " << stripId << "  " << (unsigned int)stripHash);


    ////// double m_x[4];
    ////// for(unsigned int i=0;i<chgs.size();i++) {
    //////   m_x[i] = chgs[i];
    ////// }
    double initValues[3];
    for (int i = 0; i < 3; ++i) {
        initValues[i] = 0.;
    }
    //  int imax = -1;
    initValues[2] = 8.;  // m_width
    //////  double samplemax = FindInitValues(m_x,initValues,&imax);

    double noise = m_cscCalibTool->stripNoise(stripHash);
    res.status   = 0;
    if (m_qerr > 0)
        res.dcharge = m_qerr;
    else
        res.dcharge = noise;

    res.dtime  = m_terr;
    res.charge = initValues[0];
    res.time   = initValues[1];

    initValues[1] = res.time - 2.5;
    //  int imeas =4;
    //  int meas[4] = {true,true,true,true};
    //  int ipar = 3;
    //  int par[3] = {true,true,false};
    //  double m_chi2;
    double result[3];
    //////  int FitStatus = TheFitter(m_x,noise,initValues,imeas,meas,ipar,par,&m_chi2,result);

    // Add an error proportional to the charge.
    double dqprop = m_qerrprop * res.charge;
    res.dcharge   = sqrt(res.dcharge * res.dcharge + dqprop * dqprop);
    // Display result.
    ATH_MSG_DEBUG("  Status: " << res.status);
    ATH_MSG_DEBUG("  Charge: " << res.charge);
    ATH_MSG_DEBUG("  Charge err: " << res.dcharge);
    ATH_MSG_DEBUG("    Time: " << res.time);
    ATH_MSG_DEBUG("   fit  : " << result[0] << " " << result[1] << " " << result[2]);
    return res;
}

double
CscBipolarStripFitter::FindInitValues(double *x, double *initValues, int *maxsample) const
{
    // find maximum sample imax:
    double    peakingTime = -99.;  // interpolated peaking time in samples
    double    amplitude   = -99.;  // interpolated amplitude
    double    amin, amax;
    int       imax      = 0;
    const int numSample = 4;
    amax                = x[0];
    amin                = x[0];
    for (int j = 1; j < numSample; j++) {
        if (amax < x[j]) {
            amax = x[j];
            imax = j;
        }
        if (amin > x[j]) amin = x[j];
    }

    // calculate peak and amplitude:
    (*maxsample) = imax;
    if ((imax == 0) || (imax == numSample - 1))  // no interpolation possible
    {
        peakingTime = imax;
        amplitude   = amax;
    } else  // do parabolic interpolation
    {
        double a, b, c;  // coeffients of parabola y=a*x*x+b*x+c
        a = 0.5 * (x[imax + 1] + x[imax - 1] - 2.0 * x[imax]);
        b = 0.5 * (x[imax + 1] - x[imax - 1]);
        c = x[imax];

        peakingTime = -0.5 * b / a;
        amplitude   = a * peakingTime * peakingTime + b * peakingTime + c;
        peakingTime += imax;  // it was relative to imax
    }

    initValues[0] = amplitude;
    initValues[1] = peakingTime;
    // - m_zmax*initValues[2]/m_tsampling;
    return x[imax];
}

double
CscBipolarStripFitter::FindPow(double z) const
{
    if (fabs(m_powcachez - z) < 1.e-4) return m_powcachezn;

    // double zpower = z*z*z;
    // zpower *= zpower;
    // zpower *= zpower;
    double zpower = exp(m_n * log(z));
    m_powcachez   = z;
    m_powcachezn  = zpower;
    return zpower;
}

void
CscBipolarStripFitter::InvertMatrix(double matrix[][3], const int dim, int *correspdim) const
{
    // invert 2x2 or 3x3 symmetric matrix
    if (dim == 1) {
        int ii         = correspdim[0];
        matrix[ii][ii] = 1. / matrix[ii][ii];

    } else if (dim == 2) {
        int    ii          = correspdim[0];
        int    jj          = correspdim[1];
        double determinant = -matrix[jj][ii] * matrix[ii][jj] + matrix[ii][ii] * matrix[jj][jj];
        // if(fabs(determinant)<1.e-13)
        // std::cout<<" zero determinant "<<std::endl;
        double i00     = matrix[ii][ii];
        matrix[ii][ii] = matrix[jj][jj] / determinant;
        matrix[jj][jj] = i00 / determinant;
        matrix[ii][jj] = -matrix[ii][jj] / determinant;
        matrix[jj][ii] = matrix[ii][jj];

    } else if (dim == 3) {
        double sm12        = matrix[0][1] * matrix[0][1];
        double sm13        = matrix[0][2] * matrix[0][2];
        double sm23        = matrix[1][2] * matrix[1][2];
        double determinant = matrix[0][0] * matrix[1][1] * matrix[2][2] - matrix[0][0] * sm23 - sm12 * matrix[2][2]
                             + 2. * matrix[0][1] * matrix[0][2] * matrix[1][2] - matrix[1][1] * sm13;

        // if(fabs(determinant)<1.e-13)
        // std::cout << "zero determinant"<<std::endl;
        double i00 = matrix[1][1] * matrix[2][2] - sm23;
        double i11 = matrix[0][0] * matrix[2][2] - sm13;
        double i22 = matrix[0][0] * matrix[1][1] - sm12;

        matrix[1][0] = (matrix[0][2] * matrix[1][2] - matrix[2][2] * matrix[0][1]) / determinant;
        matrix[2][0] = (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]) / determinant;
        matrix[2][1] = (matrix[0][1] * matrix[0][2] - matrix[0][0] * matrix[1][2]) / determinant;
        matrix[0][1] = matrix[1][0];
        matrix[0][2] = matrix[2][0];
        matrix[1][2] = matrix[2][1];
        matrix[0][0] = i00 / determinant;
        matrix[1][1] = i11 / determinant;
        matrix[2][2] = i22 / determinant;

    } else {
        // int ierr;
        // matrix.invert(ierr);
        printf("this is not a 1x1 or 2x2 or 3x3 matrix\n");
    }
}

void
CscBipolarStripFitter::InvertSymmetric4x4(double W[][4]) const
{
    double Determinant = W[0][3] * W[0][3] * W[1][2] * W[1][2] - 2. * W[0][2] * W[0][3] * W[1][2] * W[1][3]
                         + W[0][2] * W[0][2] * W[1][3] * W[1][3] - W[0][3] * W[0][3] * W[1][1] * W[2][2]
                         + 2. * W[0][1] * W[0][3] * W[1][3] * W[2][2] - W[0][0] * W[1][3] * W[1][3] * W[2][2]
                         + 2. * W[0][2] * W[0][3] * W[1][1] * W[2][3] - 2. * W[0][1] * W[0][3] * W[1][2] * W[2][3]
                         - 2. * W[0][1] * W[0][2] * W[1][3] * W[2][3] + 2. * W[0][0] * W[1][2] * W[1][3] * W[2][3]
                         + W[0][1] * W[0][1] * W[2][3] * W[2][3] - W[0][0] * W[1][1] * W[2][3] * W[2][3]
                         - W[0][2] * W[0][2] * W[1][1] * W[3][3] + 2. * W[0][1] * W[0][2] * W[1][2] * W[3][3]
                         - W[0][0] * W[1][2] * W[1][2] * W[3][3] - W[0][1] * W[0][1] * W[2][2] * W[3][3]
                         + W[0][0] * W[1][1] * W[2][2] * W[3][3];

    W[0][1] = W[3][0] * W[3][1] * W[2][2] - W[3][0] * W[2][1] * W[3][2] - W[2][0] * W[3][1] * W[3][2]
              + W[1][0] * W[3][2] * W[3][2] + W[2][0] * W[2][1] * W[3][3] - W[1][0] * W[2][2] * W[3][3];
    W[0][2] = -W[3][0] * W[2][1] * W[3][1] + W[2][0] * W[3][1] * W[3][1] + W[3][0] * W[1][1] * W[3][2]
              - W[1][0] * W[3][1] * W[3][2] - W[2][0] * W[1][1] * W[3][3] + W[1][0] * W[2][1] * W[3][3];
    W[0][3] = W[3][0] * W[2][1] * W[2][1] - W[2][0] * W[2][1] * W[3][1] - W[3][0] * W[1][1] * W[2][2]
              + W[1][0] * W[3][1] * W[2][2] + W[2][0] * W[1][1] * W[3][2] - W[1][0] * W[2][1] * W[3][2];
    W[1][2] = W[3][0] * W[3][0] * W[2][1] - W[2][0] * W[3][0] * W[3][1] - W[1][0] * W[3][0] * W[3][2]
              + W[0][0] * W[3][1] * W[3][2] + W[1][0] * W[2][0] * W[3][3] - W[0][0] * W[2][1] * W[3][3];

    W[1][3] = -W[2][0] * W[3][0] * W[2][1] + W[2][0] * W[2][0] * W[3][1] + W[1][0] * W[3][0] * W[2][2]
              - W[0][0] * W[3][1] * W[2][2] - W[1][0] * W[2][0] * W[3][2] + W[0][0] * W[2][1] * W[3][2];
    W[2][3] = W[2][0] * W[3][0] * W[1][1] - W[1][0] * W[3][0] * W[2][1] - W[1][0] * W[2][0] * W[3][1]
              + W[0][0] * W[2][1] * W[3][1] + W[1][0] * W[1][0] * W[3][2] - W[0][0] * W[1][1] * W[3][2];

    double W00 = -W[3][1] * W[3][1] * W[2][2] + 2. * W[2][1] * W[3][1] * W[3][2] - W[1][1] * W[3][2] * W[3][2]
                 - W[2][1] * W[2][1] * W[3][3] + W[1][1] * W[2][2] * W[3][3];
    double W11 = -W[3][0] * W[3][0] * W[2][2] + 2. * W[2][0] * W[3][0] * W[3][2] - W[0][0] * W[3][2] * W[3][2]
                 - W[2][0] * W[2][0] * W[3][3] + W[0][0] * W[2][2] * W[3][3];
    double W22 = -W[3][0] * W[3][0] * W[1][1] + 2. * W[1][0] * W[3][0] * W[3][1] - W[0][0] * W[3][1] * W[3][1]
                 - W[1][0] * W[1][0] * W[3][3] + W[0][0] * W[1][1] * W[3][3];
    double W33 = -W[2][0] * W[2][0] * W[1][1] + 2. * W[1][0] * W[2][0] * W[2][1] - W[0][0] * W[2][1] * W[2][1]
                 - W[1][0] * W[1][0] * W[2][2] + W[0][0] * W[1][1] * W[2][2];

    for (int i = 0; i < 3; i++) {
        for (int j = 1; j < 4; j++) {
            if (i >= j) continue;
            W[i][j] = W[i][j] / Determinant;
            W[j][i] = W[i][j];
        }
    }
    W[0][0] = W00 / Determinant;
    W[1][1] = W11 / Determinant;
    W[2][2] = W22 / Determinant;
    W[3][3] = W33 / Determinant;
}
void
CscBipolarStripFitter::Derivative(double A[][3], double fp[][1], double p0[][1], int imeas, int *meas) const
{
    // calculate the derivatives and the 0th order approximation
    // around the ADC samplings
    double norm = p0[0][0];
    // double parm[3]={1.,p0[1][0],p0[2][0]};
    for (int i = 0; i < imeas; i++) {
        int    ii             = meas[i];
        double z              = (ii - p0[1][0]) * m_tsampling / p0[2][0];
        double repquant       = 0.;
        double dFdzNormalized = 0.;
        if (z > 0.) {
            repquant = FindPow(z) * exp(-z) / m_bipolarNormalization;
            dFdzNormalized =
                repquant
                * (m_n / z + z / (m_n2 + 1) - (m_n + 1.) / (m_n2 + 1.) - 1.);  // repquant*(m_n/z+z/(m_n+1)-2.);
        }

        A[ii][0] = repquant * (1. - z / (m_n2 + 1.));  // repquant*(1.-z/(m_n+1.));
        // A[ii][0] = bipolar(&z,parm);
        fp[ii][0] = norm * A[ii][0];

        // double normOverZmax = norm/m_bipolarNormalization;
        double commonpart = norm * dFdzNormalized;  //(z,parm);
        A[ii][1]          = commonpart * (-m_tsampling / p0[2][0]);
        A[ii][2]          = commonpart * (-z / p0[2][0]);
    }
    // end of derivative/zeroth order calculations
}

int
CscBipolarStripFitter::TheFitter(double *x, const double ex, double *initValues, int imeas, int *meas, int ipar,
                                 int *par, double *chi2, double *result) const
{
    // maximum iterations
    const int maxIter = 7;
    // tolerances
    double fitTolerance0 = 0.1;
    double fitTolerance1 = 0.01;
    // CLHEP::HepMatrix p0(3,1,0); // the matrix of the initial fit parameters
    double p0[3][1];
    for (int j = 0; j < 3; j++) p0[j][0] = initValues[j];

    // CLHEP::HepMatrix m(4,1,0); // the matrix of ADC measurements (samples: 0,1,2,3)
    double m[4][1];
    // CLHEP::HepMatrix W(4,4,0); // the error matrix of the ADC measurements
    double W[4][4];
    for (int i = 0; i < 4; i++) {
        m[i][0] = x[i];
        W[i][i] = ex * ex;
    }
    // covariances
    W[0][1] = 0.03 * ex * ex;
    W[0][2] = -0.411 * ex * ex;
    W[0][3] = -0.188 * ex * ex;
    W[1][2] = 0.0275 * ex * ex;
    W[1][3] = -0.4303 * ex * ex;
    W[2][3] = 0. * ex * ex;
    W[1][0] = W[0][1];
    W[2][0] = W[0][2];
    W[3][0] = W[0][3];
    W[2][1] = W[1][2];
    W[3][1] = W[1][3];
    W[3][2] = W[2][3];

    // WW.invert(ierr);
    InvertSymmetric4x4(W);

    // Taylor expansion of the bipolar pulse model around the
    // samplings : F(x) = F(p0) + A *(p-p0) + higher.order
    // CLHEP::HepMatrix fp(4,1,0); // the matrix of 0th order approximation
    double fp[4][1];
    // CLHEP::HepMatrix A(4,3,0);  // the matrix of derivatives
    double A[4][3];
    for (int i = 0; i < 4; ++i) {
        fp[i][0] = 0.;
        for (int j = 0; j < 3; ++j) {
            A[i][j] = 0.;
        }
    }
    // remarks :
    // if the pulse peaks in the last sampling fit with a constant shaping time
    // if the pulse peaks in the first sampling fit without using the last sampling
    // (too large contribution to the chi2
    int    counter             = 0;
    bool   converged           = false;
    double amplitudeChangeOld  = 0.;
    bool   diverganceCandidate = false;
    // CLHEP::HepMatrix weight(3,3,1); // weight matrix allocated once
    // the non-fitted parts are taken care appropriately
    // at least if the fitting parameters or measurements
    // don't change during the fitting procedure
    double weight[3][3];

    // CLHEP::HepMatrix residFactor(3,4,0); // residFactor allocated once
    double residFactor[3][4];

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (j < 3) {
                weight[i][j] = 0.;
            }
            residFactor[i][j] = 0.;
        }
    }
    weight[0][0] = 1.;
    weight[1][1] = 1.;
    weight[2][2] = 1.;

    while (!converged && counter < maxIter)  // fit loop
    {
        Derivative(A, fp, p0, imeas, meas);  // calculate the matrix of derivatives and 0th order approximation
        // matrix multiplication
        // the weight matrix is symmetric
        // weight= A.T()*W*A;//.assign(A.T()*W*A);

        double helpmatrix[4][3];
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 3; ++j) {
                helpmatrix[i][j] = 0.;
            }
        }

        for (int i = 0; i < imeas; i++) {
            int ii = meas[i];
            for (int j = 0; j < ipar; j++) {
                int jj = par[j];
                for (int k = 0; k < imeas; k++) {
                    int kk = meas[k];
                    helpmatrix[ii][jj] += W[ii][kk] * A[kk][jj];
                }
            }
        }
        for (int i = 0; i < ipar; i++) {
            int ii = par[i];
            for (int j = i; j < ipar; j++) {
                int jj         = par[j];
                weight[ii][jj] = 0.;
                for (int k = 0; k < imeas; k++) {
                    int kk = meas[k];
                    weight[ii][jj] += A[kk][ii] * helpmatrix[kk][jj];  // A[kk][ii]*A[kk][jj];
                }
                // weight[ii][jj]*=W[0][0];
                weight[jj][ii] = weight[ii][jj];
            }
        }
        // weight.invert(ierr); // inversion of weight matrix
        // hand-made inversion of 2x2 or 3x3 symmetric matrix
        InvertMatrix(weight, ipar, par);

        // calculate W*(A.T()*W)
        // residFactor = weight*(A.T()*W);
        double helpmatrix2[3][4];
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 4; ++j) {
                helpmatrix2[i][j] = 0.;
            }
        }

        for (int i = 0; i < ipar; i++) {
            int ii = par[i];
            for (int j = 0; j < imeas; j++) {
                int jj = meas[j];
                for (int k = 0; k < imeas; k++) {
                    int kk = meas[k];
                    helpmatrix2[ii][jj] += A[kk][ii] * W[kk][jj];
                }
            }
        }

        for (int i = 0; i < ipar; i++) {
            int ii = par[i];
            for (int j = 0; j < imeas; j++) {
                int jj              = meas[j];
                residFactor[ii][jj] = 0.;
                for (int k = 0; k < ipar; k++) {
                    int kk = par[k];
                    residFactor[ii][jj] += weight[ii][kk] * helpmatrix2[kk][jj];
                }
                // residFactor[ii][jj]*=W[0][0];
            }
        }

        double paramDiff[3];
        for (int i = 0; i < 3; ++i) {
            paramDiff[i] = 0.;
        }

        for (int i = 0; i < ipar; i++) {
            int ii = par[i];
            // estimation of new parameters
            // paramDiff[i][0] += (weight*(A.T()*W)*(m-fp))[i][0];
            for (int j = 0; j < imeas; j++) {
                int jj = meas[j];
                paramDiff[ii] += residFactor[ii][jj] * (m[jj][0] - fp[jj][0]);
            }
            p0[ii][0] += paramDiff[ii];
        }
        std::cout << "##### " << p0[0][0] << " " << p0[1][0] << " " << p0[2][0] << std::endl;
        // if the parameters are not physical, keep them sensible
        // if peaking time less than -0.5
        double peakingTime = p0[1][0] + m_zmax * p0[2][0] / m_tsampling;
        if (peakingTime < -0.5 || peakingTime > 3.) p0[1][0] = initValues[1];

        if (p0[0][0] < 0.) p0[0][0] = initValues[0];
        double amplitudeChangeNew = fabs(paramDiff[0]);
        if (fabs(paramDiff[0]) < fitTolerance0 && fabs(paramDiff[1]) < fitTolerance1) {
            converged = true;
            // calculate chi2
            // (m-fp).T()*W*(m-fp)
            double residual[4];
            for (int i = 0; i < 4; ++i) {
                residual[i] = 0.;
            }

            for (int i = 0; i < imeas; i++) {
                int ii      = meas[i];
                residual[i] = m[ii][0] - fp[ii][0];
            }

            double tmpChi2 = 0.;
            double helpmatrixchi2[4][1];
            for (int i = 0; i < 4; ++i) {
                helpmatrixchi2[i][0] = 0.;
            }
            for (int i = 0; i < imeas; i++) {
                int ii = meas[i];
                for (int k = 0; k < imeas; k++) {
                    int kk = meas[k];
                    helpmatrixchi2[ii][0] += W[ii][kk] * residual[kk];
                }
            }
            for (int k = 0; k < imeas; k++) {
                int kk = meas[k];
                tmpChi2 += residual[kk] * helpmatrixchi2[kk][0];
                // std::cout << residual[kk] << " ";//*W[kk][kk]*residual[kk]<< " ";
            }
            //	  std::cout<<std::endl;
            (*chi2) = tmpChi2;
        } else if (counter > 4 && (amplitudeChangeNew > 4. * amplitudeChangeOld)) {
            if (diverganceCandidate) {
                // diverging fit
                // return parabola interpolation
                printf("%3.2f %3.2f %3.2f %3.2f\n ", x[0], x[1], x[2], x[3]);
                printf("Diverging fit\n");
                return 4;
            } else
                diverganceCandidate = true;
        }
        if (p0[0][0] < 0.) {
            // negative amplitude
            // fit diverged
            // return parabola
            return 4;
        }
        // if after a couple of iterations the amplitude is low
        // reduce the tolerances (or the maximum iterations)
        // low amplitude pulses tend to oscillate and exhaust all iterations
        if (p0[0][0] < 20.) {
            fitTolerance0 = 0.1;
            fitTolerance1 = 0.05;
        }
        amplitudeChangeOld = amplitudeChangeNew;
        counter++;
    }
    /*
    std::cout << " Error matrix "<<std::endl;
    for(int i=0;i<3;i++)
      {
        for(int j=0;j<3;j++)
          std::cout << weight[i][j]<<"\t";
        std::cout<<std::endl;
      }
    */


    result[0] = p0[0][0];
    result[1] = m_zmax * p0[2][0] / m_tsampling + p0[1][0];
    result[2] = p0[2][0];

    if (counter == maxIter) return 3;
    return 0;
}

//**********************************************************************
