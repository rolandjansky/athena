/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
// a class to fit CSC strips in a plane into clusters                      //
//                                                                         //
// BNL March 26 2003  --- Ketevi A. Assamagan                              //
/////////////////////////////////////////////////////////////////////////////

#include "CscFit.h"

#include <cmath>

#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "TF1.h"
#include "TH1F.h"

CscFit::CscFit(double sigma) { m_sigma = sigma; }

/// clustering by a Gaussing fit to the charge distribution
/// 4 different fit are:
/// First find the clusters
/// for each cluster fit to the left and to the right of the peak
/// then fit the whole distribution with one Gaussian
/// Finally fit the whold distribution with 2 Gaussians
/// If the fit does not converge or is "bad" then take the weighted average

void CscFit::cscfit(double* qstr, int& maxStrip, double& thr, double& da, int& ncl, double* sig, double* zpos, double& noise) const {
#ifndef NDEBUG
    MsgStream log(Athena::getMessageSvc(), "CscFit");
#endif

    const int NN = 100;
    int NPT;
    int ii;
    int imax;
    int iquest1, iquest2;
    int ithrL, ithrR, is;  // i0=0;

    bool first = true;

    double zl, zr;
    double q1, q2;
    double /* ddz=0, */ zfcsc1 = -1000.0, zcscl = 0, zcscr = 0;  // dlr=-1000.0;
#ifndef NDEBUG
    double fErr1 = 10000.0;
#endif
    double dzcut = da / 25.0;

    int L = maxStrip;

    ithrL = 1;
    ithrR = 1;

    if (first) {
        // i0    = L/2;
        first = false;
    }
    q2 = 0;
    ncl = 0;
    for (is = 1; is < L; is++) {
        q1 = q2;
        q2 = *(qstr + is);
        if (is == (L - 1)) q2 = 0;
        if (q1 < thr && q2 >= thr) ithrL = is;
        if (q1 < thr || q2 >= thr) continue;
        ithrR = is - 1;

        /// A Gaussin fit to the left of the strip with the maximum charge in the cluster
        imax = icmax(qstr, ithrL, std::min(L - 1, (ithrL + 2)));
        iquest1 = std::max(ithrL - 2, 0);
        iquest2 = imax;
        double al = qstr[imax] / 2.0;
        NPT = 0;
        double sum = 0;
        double norm = 0;
        for (ii = 0; ii < L; ii++)
            if (ii >= iquest1 && ii <= iquest2 && *(qstr + ii) > 1) NPT++;
        double lowerBin = da * iquest1 - da * L / 2;
        double higherBin = da * iquest2 - da * L / 2;
        TH1F* hist = new TH1F("csc", "csc", NPT, lowerBin, higherBin);
        for (ii = 0; ii < L; ii++) {
            if (ii >= iquest1 && ii <= iquest2 && *(qstr + ii) > 1) {
                double y = *(qstr + ii);
                double x = da * ((ii + 0.5) - L / 2);
                sum += y * x;
                norm += y;
                hist->Fill(x, y);
            }
        }
        zl = sum / norm;

        TF1* func = new TF1("f1gauss", f1gauss, lowerBin, higherBin, 4);
        if (NPT >= 4) {
            Double_t par[4];
            Double_t x0 = da * ((imax + 0.5) - L / 2);
            Double_t s0 = da * m_sigma;
            func->SetParameters(qstr[imax], x0, s0, 0.0);
            int status = hist->Fit("f1gauss", "QRI");
            func->GetParameters(&par[0]);
#ifndef NDEBUG
            fErr1 = func->GetParError(1);
            log << MSG::DEBUG << "One Gaussing Left Fit: "
                << "Avg = " << zl << " Mean = " << par[1] << " Error = " << fErr1 << " status = " << status << endmsg;
#endif
            if (status == 0) {
                al = (double)par[0];
                zl = (double)par[1];
            }
        }
        delete func;
        delete hist;

        /// A Gaussin fit to the right of the strip with the maximum charge in the cluster
        imax = icmax(qstr, std::max(0, (ithrR - 2)), ithrR);
        iquest1 = imax;
        iquest2 = std::min(ithrR + 2, L - 1);
        double ar = qstr[imax] / 2.0;
        NPT = 0;
        sum = 0;
        norm = 0;
        for (ii = 0; ii < L; ii++)
            if (ii >= iquest1 && ii <= iquest2 && *(qstr + ii) > 1) NPT++;
        lowerBin = da * iquest1 - da * L / 2;
        higherBin = da * iquest2 - da * L / 2;
        hist = new TH1F("csc", "csc", NPT, lowerBin, higherBin);
        for (ii = 0; ii < L; ii++) {
            if (ii >= iquest1 && ii <= iquest2 && *(qstr + ii) > 1) {
                double y = *(qstr + ii);
                double x = da * ((ii + 0.5) - L / 2);
                sum += y * x;
                norm += y;
                hist->Fill(x, y);
            }
        }

        zr = sum / norm;

        func = new TF1("f1gauss", f1gauss, lowerBin, higherBin, 4);
        if (NPT >= 4) {
            Double_t par[4];
            Double_t x0 = da * ((imax + 0.5) - L / 2);
            Double_t s0 = da * m_sigma;
            func->SetParameters(qstr[imax], x0, s0, 0.0);
            int status = hist->Fit("f1gauss", "QRI");
            func->GetParameters(&par[0]);
#ifndef NDEBUG
            fErr1 = func->GetParError(1);
            log << MSG::DEBUG << "One Gaussing Right Fit: "
                << "Avg = " << zr << " Mean = " << par[1] << " Error = " << fErr1 << " status = " << status << endmsg;
#endif
            if (status == 0) {
                ar = (double)par[0];
                zr = (double)par[1];
            }
        }
        delete func;
        delete hist;

        // ddz = fabs(zr-zl);

        /// One Gaussin fit to the cluster charge distribution
        iquest1 = std::max(ithrL - 2, 0);
        iquest2 = std::min(ithrR + 2, L - 1);
        int jmax = icmax(qstr, iquest1, iquest2) + 1;

        NPT = 0;
        sum = 0;
        norm = 0;
        for (ii = 0; ii < L; ii++)
            if (ii >= iquest1 && ii <= iquest2 && *(qstr + ii) > 1) NPT++;
        lowerBin = da * iquest1 - da * L / 2;
        higherBin = da * iquest2 - da * L / 2;
        hist = new TH1F("csc", "csc", NPT, lowerBin, higherBin);
        for (ii = 0; ii < L; ii++) {
            if (ii >= iquest1 && ii <= iquest2 && *(qstr + ii) > 1) {
                double y = *(qstr + ii);
                double x = da * ((ii + 0.5) - L / 2);
                hist->Fill(x, y);
                sum += y * x;
                norm += y;
            }
        }

        double z0 = sum / norm;
        zfcsc1 = z0;
        double sigMainFit = 0.0;

        bool mainFit = false;
        func = new TF1("f1gauss", f1gauss, lowerBin, higherBin, 4);
        if (NPT >= 4) {
            Double_t par[4];
            Double_t s0 = da * m_sigma;
            sigMainFit = s0;
            func->SetParameters(qstr[jmax], z0, s0, 0.0);
            // func->FixParameter(3,s0);
            int status = hist->Fit("f1gauss", "QRBI");
            func->GetParameters(&par[0]);
            if (status == 0) {
                zfcsc1 = (double)par[1];
                sigMainFit = (double)par[2];
#ifndef NDEBUG
                fErr1 = func->GetParError(1);
#endif
                mainFit = true;
            }
#ifndef NDEBUG
            log << MSG::DEBUG << "One Gaussing Main Fit: "
                << "Avg = " << z0 << " Mean = " << par[1] << " Error = " << fErr1 << " baseline = " << par[3] << " status = " << status
                << endmsg;
#endif
        }
        delete func;
        delete hist;

        /// Two-Gaussin fit to the cluster charge distribution
        iquest1 = std::max(ithrL - 2, 0);
        iquest2 = std::min(ithrR + 2, L - 1);
        NPT = 0;
        sum = 0;
        norm = 0;
        int max_strip = -1;
        double max_charge = -1;
        for (ii = 0; ii < L; ii++)
            if (ii >= iquest1 && ii <= iquest2 && *(qstr + ii) > 1) NPT++;
        lowerBin = da * iquest1 - da * L / 2;
        higherBin = da * iquest2 - da * L / 2;
        hist = new TH1F("csc", "csc", NPT, lowerBin, higherBin);
        for (ii = 0; ii < L; ii++) {
            if (ii >= iquest1 && ii <= iquest2 && *(qstr + ii) > 1) {
                double y = *(qstr + ii);
                double x = da * ((ii + 0.5) - L / 2);
                sum += y * x;
                norm += y;
                if (y > max_charge) {
                    max_charge = y;
                    max_strip = ii;
                }
                hist->Fill(x, y);
            }
        }

        bool doubleGaussianFit = false;
        func = new TF1("f2gauss", f2gauss, lowerBin, higherBin, 7);
        if (NPT >= 7) {
            Double_t par[7];
            double_t s0 = da * m_sigma / 2.0;
            if (mainFit) s0 = sigMainFit / 2.0;
            func->SetParameters(al, zl, s0, ar, zr, s0, 0.0);
            int status = hist->Fit("f2gauss", "QRI");
            func->GetParameters(&par[0]);
            if (status == 0) {
                doubleGaussianFit = true;
                zcscl = (double)par[1];
                zcscr = (double)par[4];
            }
#ifndef NDEBUG
            log << MSG::DEBUG << "Double Gaussing Fit: "
                << "Avg = " << z0 << " Mean One Gaussian = " << zfcsc1 << " Mean1 = " << par[1] << " Error1 = " << func->GetParError(1)
                << " Mean2 = " << par[4] << " Error2 = " << func->GetParError(4) << " baseline = " << par[6] << " status = " << status
                << endmsg;
#endif
        }
        delete func;
        delete hist;

        double ysum = 0;
        double ysum2 = 0;
        int count = 0;
        for (int i = max_strip - 1; i <= max_strip + 1; i++) {
            if (i >= 0 && i < L) {
                ysum += *(qstr + i);
                count++;
                ysum2 += count * count;
            }
        }

        /// the error on the weighted mean!!
        double wErr = da / sqrt(12.0);
        if (count > 1 && ysum > 0) wErr = noise * (da / (ysum)) * sqrt(2.0 * ysum2);

        /// make a selection for the best position finding
        /// the error on the position is the error on the weight mean
        double xp0 = 0, xp1 = 0, xp2 = 0;

        xp0 = zfcsc1;
        xp1 = std::min(zcscl, zcscr);
        xp2 = std::max(zcscl, zcscr);

        double dx0 = fabs(xp0 - z0);
        double dx1 = fabs(xp1 - z0);
        double dx2 = fabs(xp2 - z0);

        bool checkIt = (dx0 < dzcut) || (dx1 < dzcut) || (dx2 < dzcut);

        if (mainFit && doubleGaussianFit && checkIt) {
#ifndef NDEBUG
            log << MSG::DEBUG << "Double Gaussian Fit converged ..." << endmsg;
#endif
            ncl = std::min(NN, ncl + 1);
            if (dx0 < dx1 && dx0 < dx2) {
                *(zpos + ncl - 1) = xp0;
            } else if (dx1 < dx2 && dx1 < dx0) {
                *(zpos + ncl - 1) = xp1;
            } else if (dx2 < dx0 && dx2 < dx1) {
                *(zpos + ncl - 1) = xp2;
            } else {
                *(zpos + ncl - 1) = xp0;
            }
            *(sig + ncl - 1) = wErr;
        } else if (mainFit && (dx0 < dzcut)) {
#ifndef NDEBUG
            log << MSG::DEBUG << "One Gaussian Fit converged ..." << endmsg;
#endif
            ncl = std::min(NN, ncl + 1);
            *(zpos + ncl - 1) = xp0;
            *(sig + ncl - 1) = wErr;
        } else {
            if (ysum > 0) {
                ncl = std::min(NN, ncl + 1);
#ifndef NDEBUG
                log << MSG::DEBUG << "Fit was not meaningful: using weighted average" << endmsg;
#endif
                *(zpos + ncl - 1) = z0;
                *(sig + ncl - 1) = wErr;
            }
        }
    }
}

/// a method to find a the index of the strip with the highest charge
int CscFit::icmax(const double* qstr, const int& i1, const int& i2) {
    int i, ic;
    double a;

    a = *(qstr + i1);
    ic = i1;
    for (i = i1 + 1; i <= i2; i++) {
        if (*(qstr + i) < a) continue;
        a = *(qstr + i);
        ic = i;
    }
    return (ic);
}

/// one Gaussing fitting method
Double_t f1gauss(const Double_t* x, const Double_t* par) {
    Double_t arg = 0;
    arg = (x[0] - par[1]) / par[2];
    Double_t fitval = par[3] + par[0] * std::exp(-0.5 * arg * arg);
    return fitval;
}

/// two-Gaussian fitting method
Double_t f2gauss(const Double_t* x, const Double_t* par) {
    Double_t arg1 = 0;
    Double_t arg2 = 0;
    if (par[2] != 0) arg1 = (x[0] - par[1]) / par[2];
    if (par[5] != 0) arg2 = (x[0] - par[4]) / par[5];
    Double_t fitval = par[0] * std::exp(-0.5 * arg1 * arg1) + par[3] * std::exp(-0.5 * arg2 * arg2) + par[6];
    return fitval;
}
