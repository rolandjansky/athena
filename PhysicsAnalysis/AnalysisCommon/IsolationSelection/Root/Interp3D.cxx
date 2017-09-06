/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "IsolationSelection/Interp3D.h"

#include <iostream>

#include <TH3F.h>

double Interp3D::Interpol3d(double x, double y, double z, std::shared_ptr<TH3F> h) {

    int nx = h->GetNbinsX(), ny = h->GetNbinsY(), nz = h->GetNbinsZ();
    int ibx = h->GetXaxis()->FindBin(x), iby = h->GetYaxis()->FindBin(y), ibz = h->GetZaxis()->FindBin(z);
    int ibx2, iby2, ibz2;
    double z000, z010, z110, z100, z001, z011, z111, z101, xc, yc, zc, xc2, yc2, zc2, u, t, v, r;
    if (ibx > nx) ibx = nx;
    else if (ibx == 0) ibx = 1;
    if (iby > ny) iby = ny;
    else if (iby == 0) iby = 1;
    if (ibz > nz) ibz = nz;
    else if (ibz == 0) ibz = 1;
    xc = h->GetXaxis()->GetBinCenter(ibx);
    yc = h->GetYaxis()->GetBinCenter(iby);
    zc = h->GetZaxis()->GetBinCenter(ibz);
    //
    z111 = h->GetBinContent(ibx, iby, ibz);

    // Test no interp for egamma object in the vicinity of the crack
    bool doX = true;
    bool doY = true;
    if (m_NoInterp.find(h->GetName()) != m_NoInterp.end()) {
        for (auto rangeX : m_NoInterp[h->GetName()].xRange) {
            if (x > rangeX.first && x < rangeX.second) doX = false;
        }
        double ay = fabs(y);
        for (auto rangeY : m_NoInterp[h->GetName()].yRange) {
            if (ay > rangeY.first && ay < rangeY.second) doY = false;
        }
    }
    if (m_debug) std::cout << "Isolation type = " << h->GetName() << " pT = " << x << " pT interp ? " << doX << " eta = " << y << " eta interp ? " << doY << " No interp cut = " << z111 << std::endl;

    if (!doX && !doY) return z111;

    if ((ibx > 1 || (ibx == 1 && x > xc)) && (ibx < nx || (ibx == nx && x < xc))) {
        if (x > xc) {
            ibx2 = ibx + 1;
        } else {
            ibx2 = ibx - 1;
        }
        xc2 = h->GetXaxis()->GetBinCenter(ibx2);
        if ((iby > 1 || (iby == 1 && y > yc)) && (iby < ny || (iby == ny && y < yc))) {
            if (y > yc) {
                iby2 = iby + 1;
            } else {
                iby2 = iby - 1;
            }
            yc2 = h->GetYaxis()->GetBinCenter(iby2);
            if ((ibz > 1 || (ibz == 1 && z > zc)) && (ibz < nz || (ibz == nz && z < zc))) {
                if (z > zc) {
                    ibz2 = ibz + 1;
                } else {
                    ibz2 = ibz - 1;
                }
                zc2 = h->GetZaxis()->GetBinCenter(ibz2);

                //
                if (m_debug) {
                    std::cout << "Normal situation " << x << " " << ibx << " " << ibx2 << " " << y << " " << iby << " " << iby2 << " " << z << " " << ibz << " " << ibz2 << std::endl;
                    std::cout << "Bin centers " << xc << " " << xc2 << " " << yc << " " << yc2 << " " << zc << " " << zc2 << std::endl;
                }
                //
                z000 = h->GetBinContent(ibx2, iby2, ibz2);
                z100 = h->GetBinContent(ibx, iby2, ibz2);
                z010 = h->GetBinContent(ibx2, iby, ibz2);
                z110 = h->GetBinContent(ibx, iby, ibz2);
                z001 = h->GetBinContent(ibx2, iby2, ibz);
                z101 = h->GetBinContent(ibx, iby2, ibz);
                z011 = h->GetBinContent(ibx2, iby, ibz);

                t = (x - xc2) / (xc - xc2);
                u = (y - yc2) / (yc - yc2);
                v = (z - zc2) / (zc - zc2);
                if (!doX) t = 1;
                if (!doY) u = 1;

                if (m_debug) {
                    std::cout << "Cuts " << z000 << " " << z100 << " " << z010 << " " << z110 << " " << z001 << " " << z101 << " " << z011 << " " << z111 << std::endl;
                    std::cout << "interp coeff " << t << " " << u << " " << v << std::endl;
                }

                r = z111 * t * u * v + z001 * (1. - t) * (1. - u) * v + z011 * (1. - t) * u * v + z101 * t * (1. - u) * v + z110 * t * u * (1. - v) + z000 * (1. - t) * (1. - u) * (1. - v) + z010 * (1. - t) * u * (1. - v) + z100 * t * (1. - u) * (1. - v);
            } else {
                z011 = h->GetBinContent(ibx2, iby, ibz);
                z001 = h->GetBinContent(ibx2, iby2, ibz);
                z101 = h->GetBinContent(ibx, iby2, ibz);
                t = (x - xc2) / (xc - xc2);
                u = (y - yc2) / (yc - yc2);
                if (!doX) t = 1;
                if (!doY) u = 1;
                r = z111 * t * u + z011 * (1. - t) * u + z101 * t * (1. - u) + z001 * (1. - t) * (1. - u);
            }
        } else if ((ibz > 1 || (ibz == 1 && z > zc)) && (ibz < nz || (ibz == nz && z < zc))) {
            if (z > zc) {
                ibz2 = ibz + 1;
            } else {
                ibz2 = ibz - 1;
            }

            /*
             std::cout << "No y interpolation situation "
             << x << " " << ibx << " " << ibx2 << " "
             << y << " " << iby << " " << iby2 << " "
             << z << " " << ibz << " " << ibz2 << std::endl;
             */

            z110 = h->GetBinContent(ibx, iby, ibz2);
            z010 = h->GetBinContent(ibx2, iby, ibz2);
            z011 = h->GetBinContent(ibx2, iby, ibz);
            zc2 = h->GetYaxis()->GetBinCenter(ibz2);

            t = (x - xc2) / (xc - xc2);
            if (!doX) t = 1;
            v = (z - zc2) / (zc - zc2);
            r = z111 * t * v + z011 * (1. - t) * v + z110 * t * (1. - v) + z010 * (1. - t) * (1. - v);

            //std::cout << "interpolate for " << t << " " << v << " between " << z110 << " " << z010 << " " << z011 << " " << z111 << std::endl;

        } else {
            z011 = h->GetBinContent(ibx2, iby, ibz);
            t = (x - xc2) / (xc - xc2);
            if (!doX) t = 1;
            r = z111 * t + z011 * (1. - t);
        }
    } else {
        if ((iby > 1 || (iby == 1 && y > yc)) && (iby < ny || (iby == ny && y < yc))) {
            if (y > yc) {
                iby2 = iby + 1;
            } else {
                iby2 = iby - 1;
            }
            yc2 = h->GetYaxis()->GetBinCenter(iby2);
            if ((ibz > 1 || (ibz == 1 && z > zc)) && (ibz < nz || (ibz == nz && z < zc))) {
                if (z > zc) {
                    ibz2 = ibz + 1;
                } else {
                    ibz2 = ibz - 1;
                }
                zc2 = h->GetZaxis()->GetBinCenter(ibz2);
                //
                z100 = h->GetBinContent(ibx, iby2, ibz2);
                z110 = h->GetBinContent(ibx, iby, ibz2);
                z101 = h->GetBinContent(ibx, iby2, ibz);
                u = (y - yc2) / (yc - yc2);
                if (!doY) u = 1;
                v = (z - zc2) / (zc - zc2);
                r = z111 * u * v + z101 * (1. - u) * v + z110 * u * (1. - v) + z100 * (1. - u) * (1. - v);
            } else {
                z101 = h->GetBinContent(ibx, iby2, ibz);
                u = (y - yc2) / (yc - yc2);
                if (!doY) u = 1;
                r = z111 * u + z101 * (1. - u);
            }
        } else if ((ibz > 1 || (ibz == 1 && z > zc)) && (ibz < nz || (ibz == nz && z < zc))) {
            if (z > zc) {
                ibz2 = ibz + 1;
            } else {
                ibz2 = ibz - 1;
            }
            z110 = h->GetBinContent(ibx, iby, ibz2);
            zc2 = h->GetYaxis()->GetBinCenter(ibz2);
            v = (z - zc2) / (zc - zc2);
            r = z111 * v + z110 * (1. - v);
        } else {
            r = z111;
        }
    }

    if (m_debug) std::cout << "Cut " << r << std::endl;

    return r;
}
