/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkDriftCircleMath/DCSLFitter.h"

#include <iostream>

namespace TrkDriftCircleMath {

    bool DCSLFitter::fit(Segment& result, const Line& line, const DCOnTrackVec& dcs, const HitSelection& selection, double) const {
        if (m_debug) std::cout << "New seg: " << std::endl;  //<< seg;

        // reserve enough space for hits
        std::vector<FitData> data;
        data.reserve(100);
        result.dcs().reserve(100);

        unsigned int N = dcs.size();

        if (N < 2) { return false; }

        if (selection.size() != N) {
            std::cout << "DCSLFitter <bad HitSelection>" << std::endl;
            return false;
        } else {
            int used(0);
            for (unsigned int i = 0; i < N; ++i) {
                if (selection[i] == 0) ++used;
            }
            if (used < 2) {
                if (m_debug) std::cout << "TOO FEW HITS SELECTED" << std::endl;
                return false;
            }
        }

        double S(0), Sz(0), Sy(0);
        double Zc(0), Yc(0);
        data.resize(N);
        {
            int ii(0);

            DCOnTrackVec::const_iterator it = dcs.begin();
            DCOnTrackVec::const_iterator it_end = dcs.end();
            for (; it != it_end; ++it) {
                FitData& datum = data[ii];
                datum.y = it->y();
                datum.z = it->x();
                datum.r = std::abs(it->r());
                if (it->dr() > 0.)
                    datum.w = 1. / (it->dr());
                else
                    datum.w = 0.;
                datum.w *= datum.w;
                if (datum.r < 0) {
                    datum.r = 0.;
                    if (m_debug) std::cout << "DCSLFitter ERROR: <Negative r> " << datum.r << std::endl;
                }
                if (m_debug)
                    std::cout << "DC:  (" << datum.y << "," << datum.z << ")  R = " << datum.r << " W " << datum.w << " sel "
                              << selection[ii] << std::endl;

                if (selection[ii] == 0) {
                    S += datum.w;
                    Sz += datum.w * datum.z;
                    Sy += datum.w * datum.y;
                }
                ++ii;
            }
        }
        Zc = Sz / S;
        Yc = Sy / S;

        if (m_debug) std::cout << "Yc " << Yc << " Zc " << Zc << std::endl;
        //
        //    shift hits
        //
        Sy = 0;
        Sz = 0;
        double Szz(0), Syy(0), Szy(0), Syyzz(0);

        for (unsigned int i = 0; i < N; ++i) {
            FitData& datum = data[i];

            datum.y -= Yc;
            datum.z -= Zc;

            if (selection[i]) continue;

            datum.rw = datum.r * datum.w;
            datum.ryw = datum.rw * datum.y;
            datum.rzw = datum.rw * datum.z;

            Szz += datum.z * datum.z * datum.w;
            Syy += datum.y * datum.y * datum.w;
            Szy += datum.y * datum.z * datum.w;
            Syyzz += (datum.y - datum.z) * (datum.y + datum.z) * datum.w;
        }

        if (m_debug) std::cout << " Szz " << Szz << " Syy " << Syy << " Szy " << Szy << " Syyzz " << Syyzz << std::endl;

        unsigned int count{0};
        double R{0}, Ry{0}, Rz{0}, Att{0}, Add{S}, Bt{0}, Bd{0}, Stt{0}, Sd{0};

        double theta = line.phi();
        double cosin = std::cos(theta);
        double sinus = std::sin(theta);

        if (m_debug) std::cout << "cos " << cosin << " sin " << sinus << std::endl;

        // make sure 0 <= theta < PI
        if (sinus < 0.0) {
            if (m_debug) { std::cout << "sinus < 0.0 " << std::endl; }
            sinus = -sinus;
            cosin = -cosin;
        } else if (sinus == 0.0 && cosin < 0.0) {
            if (m_debug) { std::cout << "sinus == 0.0 && cosin < 0.0" << std::endl; }
            cosin = -cosin;
        }
        //
        // calculate shift
        //
        //    double d = -( getZ( pos )-Zc)*sinus+( getY( pos )-Yc)*cosin;
        double dshift = Zc * sinus - Yc * cosin;
        if (m_debug) { std::cout << " dshift " << dshift << std::endl; }
        double d = line.y0() + Zc * sinus - Yc * cosin;

        if (m_debug) {
            std::cout << "____________INITIAL VALUES________________" << count << std::endl;
            std::cout << "Theta " << theta << " d " << d << std::endl;
        }

        while (count < 100) {
            if (m_debug) std::cout << "____________NEW ITERATION________________" << count << std::endl;
            R = 0;
            Ry = 0;
            Rz = 0;

            double chi2(0);

            for (unsigned int i = 0; i < N; ++i) {
                if (selection[i]) continue;

                FitData& datum = data[i];

                double dist = datum.y * cosin - datum.z * sinus;
                if (dist > d) {
                    R -= datum.rw;
                    Ry -= datum.ryw;
                    Rz -= datum.rzw;
                    if (m_debug) {
                        double res = std::abs(dist - d) - datum.r;
                        std::cout << " < - > " << dist - d << " r " << datum.r << " res " << res;
                        chi2 += res * res * datum.w * datum.w;
                    }
                } else {
                    R += datum.rw;
                    Ry += datum.ryw;
                    Rz += datum.rzw;
                    if (m_debug) {
                        double res = std::abs(dist - d) - datum.r;
                        std::cout << " < + > " << dist - d << " r " << datum.r << " res " << res;
                        chi2 += res * res * datum.w;
                    }
                }
                if (m_debug) std::cout << std::endl;
            }
            Att = Syy + cosin * (2 * sinus * Szy - cosin * Syyzz);
            Bt = -Szy + cosin * (sinus * Syyzz + 2 * cosin * Szy + Rz) + sinus * Ry;
            Bd = -S * d + R;
            if (Att == 0) {
                if (m_debug) std::cerr << "===> Error NewtonSLDCFitter ZERO Determinant" << std::endl;
                if (data.capacity() > 100) {
                    data.reserve(100);
                    result.dcs().reserve(100);
                }
                return false;
            }
            theta += Bt / Att;
            if (m_debug) std::cout << " new theta " << theta << " chi2 " << chi2 << std::endl;
            if (theta < 0.0) theta += M_PI;
            if (theta >= M_PI) theta -= M_PI;
            cosin = std::cos(theta);
            sinus = std::sqrt(1 - cosin * cosin);
            d = R / S;
            if (m_debug) {
                double dshift = Zc * sinus - Yc * cosin;
                std::cout << " d " << d << " theta " << theta << " dshift " << dshift << std::endl;
                std::cout << "R " << R << " Ry " << Ry << " Rz " << Rz << std::endl;
                std::cout << "Att " << Att << " Add " << Add << " Bt " << Bt << " Bd " << Bd << std::endl;
                std::cout << "dTheta " << Bt / Att << " dD " << Bd / Add << std::endl;
            }
            if (std::abs(Bt / Att) < 0.001 && std::abs(Bd / Add) < 0.001) {
                Stt = std::sqrt(1 / Att);
                Sd = std::sqrt(1 / Add);
                if (m_debug) {
                    std::cout << "Fit converged after " << count << " iterations " << std::endl;
                    std::cout << "Theta " << theta << " d " << d << std::endl;
                    std::cout << "Errors: theta " << Stt << " d " << Sd << std::endl;
                }
                break;
            }
            ++count;
        }

        if (count >= 100) {
            if (data.capacity() > 100) {
                result.dcs().reserve(100);
                data.reserve(100);
            }
            return false;
        }
        if (m_debug) std::cout << "Calculating chi2" << std::endl;

        double yl{0}, chi2{0};

        double dtheta = Stt;
        double dy0 = Sd;

        if (m_debug) std::cout << "contributions to chi2: " << std::endl;

        result.dcs().clear();
        result.clusters().clear();
        result.emptyTubes().clear();

        unsigned int nhits{0};

        // calculate predicted hit positions from track parameters
        for (unsigned int i = 0; i < N; ++i) {
            FitData& datum = data[i];
            yl = cosin * datum.y - sinus * datum.z - d;
            double dth = -(sinus * datum.y + cosin * datum.z) * Stt;
            double errorResiduals = std::hypot(dth, Sd);
            double residuals = std::abs(yl) - datum.r;
            if (selection[i] == 0) {
                ++nhits;
                chi2 += residuals * residuals * datum.w;
            }
            result.dcs().emplace_back(dcs[i]);
            result.dcs().back().residual(residuals);
            result.dcs().back().errorTrack(errorResiduals);

            if (m_debug) {
                std::cout << " r_track " << yl << " dr " << errorResiduals << " r_rt " << datum.r << " d_rt " << dcs[i].dr() << " res "
                          << residuals << " pull " << residuals * sqrt(datum.w) << " sel " << selection[i] << std::endl;
            }
        }

        result.set(chi2, nhits - 2, dtheta, dy0);
        result.line().set(LocVec2D(Zc - sinus * d, Yc + cosin * d), theta);

        if (m_debug) {
            std::cout << "Fit complete: Chi2 tof " << chi2 / (nhits - 2) << " " << !(chi2 / (nhits - 2) > 5) << std::endl;
            if (chi2 / (nhits - 2) > 5) { std::cout << "_______NOT GOOD " << std::endl; }
            std::cout << "fit done " << result << std::endl;
        }

        if (data.capacity() > 100) {
            result.dcs().reserve(100);
            data.reserve(100);
        }
        return true;
    }
}  // namespace TrkDriftCircleMath
