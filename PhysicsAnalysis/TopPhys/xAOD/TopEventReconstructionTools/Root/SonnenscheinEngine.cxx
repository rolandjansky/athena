/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventReconstructionTools/SonnenscheinEngine.h"

#include "TopEvent/Event.h" //for TLorentzVector <<

#include <iostream>
#include <iomanip>
#include <cmath>

namespace top {

SonnenscheinEngine::SonnenscheinEngine() : m_debug(false) {
}

SonnenscheinEngine::~SonnenscheinEngine() {
}

std::vector<std::pair<TLorentzVector, TLorentzVector> > SonnenscheinEngine::solve(const TLorentzVector& me_t_lp, const TLorentzVector& me_t_b, double mass_t, double mass_wp, const TLorentzVector& me_tbar_lm, const TLorentzVector& me_tbar_bbar, double mass_tbar, double mass_wm, double me_mex, double me_mey) const {
    std::vector<std::pair<TLorentzVector, TLorentzVector> > lvs;

    if (m_debug) {
        std::cout << "Starting SOLVE\n";
        std::cout << "lp     " << me_t_lp << "\n";
        std::cout << "lm     " << me_tbar_lm << "\n";
        std::cout << "b      " << me_t_b << "\n";
        std::cout << "bbar   " << me_tbar_bbar << "\n";
        std::cout << "met_ex " << me_mex << "\n";
        std::cout << "met_ey " << me_mey << "\n";
        std::cout << "mt     " << mass_t << "\n";
        std::cout << "mtbar  " << mass_tbar << "\n";
        std::cout << "mWp    " << mass_wp << "\n";
        std::cout << "mWm    " << mass_wm << "\n";
    }

    const double Elp = me_t_lp.E();
    const double mlp = me_t_lp.M();
    const double plpx = me_t_lp.Px();
    const double plpy = me_t_lp.Py();
    const double plpz = me_t_lp.Pz();
    const TVector3 plp = 1. * me_t_lp.Vect();

    const double Eb = me_t_b.E();
    const double pbx = me_t_b.Px();
    const double pby = me_t_b.Py();
    const double pbz = me_t_b.Pz();
    const double mb = me_t_b.M();
    const TVector3 pb = 1. * me_t_b.Vect();


    const double Elm = me_tbar_lm.E();
    const double mlm = me_tbar_lm.M();
    const double plmx = me_tbar_lm.Px();
    const double plmy = me_tbar_lm.Py();
    const double plmz = me_tbar_lm.Pz();
    const TVector3 plm = me_tbar_lm.Vect();

    const double Ebbar = me_tbar_bbar.E();
    const double pbbarx = me_tbar_bbar.Px();
    const double pbbary = me_tbar_bbar.Py();
    const double pbbarz = me_tbar_bbar.Pz();
    const double mbbar = me_tbar_bbar.M();
    const TVector3 pbbar = me_tbar_bbar.Vect();

    if (m_debug) {
        std::cout << "mex and mey " << me_mex << " " << me_mey << "\n";
        std::cout << "Starting Calculation\n";
        std::cout << "Calculating a\n";
    }

    double a1 = (Eb + Elp)*(mass_wp * mass_wp - mlp * mlp)
                                            - Elp * (mass_t * mass_t - mb * mb - mlp * mlp)
                                            + 2 * Eb * Elp * Elp - 2 * Elp * pb * plp;

    double a2 = 2 * (Eb * plpx - Elp * pbx);
    double a3 = 2 * (Eb * plpy - Elp * pby);
    double a4 = 2 * (Eb * plpz - Elp * pbz);

    if (m_debug) {
        std::cout << "a1  : " << a1 << "\n";
        std::cout << "a2  : " << a2 << "\n";
        std::cout << "a3  : " << a3 << "\n";
        std::cout << "a4  : " << a4 << "\n";

        //double pvx = nu->Px() ;
        //double pvy = nu->Py() ;
        //double pvz = nu->Pz() ;
        //std::cout << a1 + a2 * pvx + a3 * pvy + a4 * pvz << "\n";
    }

    double b1 = (Ebbar + Elm)*(mass_wm * mass_wm - mlm * mlm)
                                            - Elm * (mass_tbar * mass_tbar - mbbar * mbbar - mlm * mlm)
                                            + 2 * Ebbar * Elm * Elm - 2 * Elm * pbbar * plm;

    double b2 = 2 * (Ebbar * plmx - Elm * pbbarx);
    double b3 = 2 * (Ebbar * plmy - Elm * pbbary);
    double b4 = 2 * (Ebbar * plmz - Elm * pbbarz);

    if (m_debug) {
        std::cout << "Calculating b\n";
        std::cout <<  "b1  : " << b1 << "\n";
        std::cout <<  "b2  : " << b2 << "\n";
        std::cout <<  "b3  : " << b3 << "\n";
        std::cout <<  "b4  : " << b4 << "\n";

        //std::cout <<  "Checking b" << "\n";

        //double pvbarx = nu2->Px() ;
        //double pvbary = nu2->Py() ;
        //double pvbarz = nu2->Pz() ;

        //std::cout <<  b1 + b2 * pvbarx + b3 * pvbary + b4 * pvbarz << "\n";

        std::cout << "Calculating c\n";
    }

    double c22 = (mass_wp * mass_wp - mlp * mlp) * (mass_wp * mass_wp - mlp * mlp)
                                            -4 * (Elp * Elp - plpz * plpz) * (a1 / a4) * (a1 / a4)
                                            -4 * (mass_wp * mass_wp - mlp * mlp) * plpz * a1 / a4;

    double c21 =  4 * (mass_wp * mass_wp - mlp * mlp) * (plpx - plpz * a2 / a4) //?
                                            -8 * (Elp * Elp - plpz * plpz) * a1 * a2 / (a4 * a4)
                                            -8 * plpx * plpz * a1 / a4;

    double c20 = -4 * (Elp * Elp - plpx * plpx)
                                            -4 * (Elp * Elp - plpz * plpz)*(a2 / a4)*(a2 / a4)
                                            -8 * plpx * plpz * a2 / a4;

    double c11 =  4 * (mass_wp * mass_wp - mlp * mlp) * (plpy - plpz * a3 / a4)
                                            -8 * (Elp * Elp - plpz * plpz) * a1 * a3 / (a4 * a4)
                                            -8 * plpy * plpz * a1 / a4;

    double c10 = -8 * (Elp * Elp - plpz * plpz) * a2 * a3 / (a4 * a4)
                                            +8 * plpx * plpy
                                            -8 * plpx * plpz * a3 / a4
                                            -8 * plpy * plpz * a2 / a4;

    double c00 = -4 * (Elp * Elp - plpy * plpy)
                                            -4 * (Elp * Elp - plpz * plpz) * (a3 / a4) * (a3 / a4)
                                            -8 * plpy * plpz * a3 / a4;

    /*
    for cross check - the original paper used these definitions.
    similar for dp.
     */
    c22 = a4 * a4 * c22;
    c21 = a4 * a4 * c21;
    c20 = a4 * a4 * c20;
    c11 = a4 * a4 * c11;
    c10 = a4 * a4 * c10;
    c00 = a4 * a4 * c00;

    if (m_debug) {
        std::cout << "c22  : " << c22 << "\n";
        std::cout << "c21  : " << c21 << "\n";
        std::cout << "c20  : " << c20 << "\n";
        std::cout << "c11  : " << c11 << "\n";
        std::cout << "c10  : " << c10 << "\n";
        std::cout << "c00  : " << c00 << "\n";

        //std::cout  "c=0? " <<
        //c22
        //+ c21 * pvx
        //+ c11 * pvy
        //+ c20 * pvx * pvx
        //+ c10 * pvx * pvy
        //+ c00 * pvy * pvy
        //<<"\n";

        std::cout << "Calculating d'\n";
    }

    double dp22 = pow( mass_wm * mass_wm - mlm * mlm, 2)
                                            -4 * (Elm * Elm - plmz * plmz) * (b1 / b4) * (b1 / b4)
                                            -4 * (mass_wm * mass_wm - mlm * mlm) * plmz * b1 / b4;

    double dp21 =  4 * (mass_wm * mass_wm - mlm * mlm) * (plmx - plmz * b2 / b4)
                                            -8 * (Elm * Elm - plmz * plmz) * b1 * b2 / (b4 * b4)
                                            -8 * plmx * plmz * b1 / b4;

    double dp20 = -4 * (Elm * Elm - plmx * plmx)
                                            -4 * (Elm * Elm - plmz * plmz)*(b2 / b4)*(b2 / b4)
                                            -8 * plmx * plmz * b2 / b4;

    double dp11 =  4 * (mass_wm * mass_wm - mlm * mlm) * (plmy - plmz * b3 / b4)
                                            -8 * (Elm * Elm - plmz * plmz) * b1 * b3 / (b4 * b4)
                                            -8 * plmy * plmz * b1 / b4;

    double dp10 = -8 * (Elm * Elm - plmz * plmz) * b2 * b3 / (b4 * b4)
                                            +8 * plmx * plmy
                                            -8 * plmx * plmz * b3 / b4
                                            -8 * plmy * plmz * b2 / b4;

    double dp00 = -4 * (Elm * Elm - plmy * plmy)
                                            -4 * (Elm * Elm - plmz * plmz) * (b3 / b4) * (b3 / b4)
                                            -8 * plmy * plmz * b3 / b4;

    dp22 = b4 * b4 * dp22;
    dp21 = b4 * b4 * dp21;
    dp20 = b4 * b4 * dp20;
    dp11 = b4 * b4 * dp11;
    dp10 = b4 * b4 * dp10;
    dp00 = b4 * b4 * dp00;

    if (m_debug) {
        std::cout <<  "dp22  : " << dp22 << "\n";
        std::cout <<  "dp21  : " << dp21 << "\n";
        std::cout <<  "dp20  : " << dp20 << "\n";
        std::cout <<  "dp11  : " << dp11 << "\n";
        std::cout <<  "dp10  : " << dp10 << "\n";
        std::cout <<  "dp00  : " << dp00 << "\n";

        //in the paper for c and dp c21 appears twice. is this right?
        //std::cout <<  "dp=0? " <<
        //dp22
        //+ dp21 * pvbarx
        //+ dp11 * pvbary
        //+ dp20 * pvbarx * pvbarx
        //+ dp10 * pvbarx * pvbary
        //+ dp00 * pvbary * pvbary
        //<<"\n";

        std::cout << "Calculating d\n";
    }

    const double d22 = dp22
            + me_mex * me_mex * dp20
            + me_mey * me_mey * dp00
            + me_mex * me_mey * dp10
            + me_mex * dp21
            + me_mey * dp11;

    const double d21 = -1 * dp21
            -2 * me_mex * dp20
            - me_mey * dp10;

    const double d20 = dp20;
    const double d11 = -1 * dp11
            - 2 * me_mey * dp00
            - me_mex * dp10;

    const double d10 = dp10;
    const double d00 = dp00;

    if (m_debug) {
        std::cout << "d22  : " << d22 << "\n";
        std::cout << "d21  : " << d21 << "\n";
        std::cout << "d20  : " << d20 << "\n";
        std::cout << "d11  : " << d11 << "\n";
        std::cout << "d10  : " << d10 << "\n";
        std::cout << "d00  : " << d00 << "\n";

        //std::cout << "d " <<
        //d22
        //+d21 * pvx
        //+d11 * pvy
        //+d20 * pvx * pvx
        //+d10 * pvx * pvy
        //+d00 * pvy * pvy
        //<< "\n";

        std::cout << "Calculating h\n";
    }

    const double h4 = c00 * c00 * d22 * d22
            + c11 * d22 * (c11 * d00 - c00 * d11)
            + c00 * c22 * (d11 * d11 - 2 * d00 * d22)
            + c22 * d00 * (c22 * d00 - c11 * d11);

    const double h3 = c00 * d21 * (2 * c00 * d22 - c11 * d11)
                                            + c00 * d11 * (2 * c22 * d10 + c21 * d11)
                                            + c22 * d00 * (2 * c21 * d00 - c11 * d10)
                                            - c00 * d22 * (c11 * d10 + c10 * d11)
                                            -2 * c00 * d00 *(c22 * d21 + c21 * d22)
                                            - d00 * d11 * (c11 * c21 + c10 * c22)
                                            + c11 * d00 * (c11 * d21 + 2 * c10 * d22);

    const double h2 = c00 * c00 * (2 * d22 * d20 + d21 * d21)
                                            - c00 * d21 * (c11 * d10 + c10 * d11)
                                            + c11 * d20 * (c11 * d00 - c00 * d11)
                                            + c00 * d10 * (c22 * d10 - c10 * d22)
                                            + c00 * d11 * (2 * c21 * d10 + c20 * d11)
                                            + (2 * c22 * c20 + c21 * c21) * d00 * d00
                                            - 2 * c00 * d00 * (c22 * d20 + c21 * d21 + c20 * d22)
                                            + c10 * d00 * (2 * c11 * d21 + c10 * d22)
                                            - d00 * d10 * (c11 * c21 + c10 * c22)
                                            - d00 * d11 * (c11 * c20 + c10 * c21);

    /*
     * leave!
     */
    /*
    double h1 = c00 * d21 * (2 * c00 * d20 - c10 * d10)
    - c00 * d20 * (c11 * d10 + c10 * d11)
    + c00 * d10 * (c21 * d10 + 2 * c20 * d11)
    -2 * c00 * d00 * (c21 * d20 + c20 * d21)
    + c10 * d00 * (2 * c11 * d20 + c10 * d21)
    - c20 * d00 * (2 * c21 * d00 - c10 * d11)
    - d00 * d10 * (c11 * c20 + c10 * c21);
     */

    const double h1 = c00 * d21 * (2 * c00 * d20 - c10 * d10)
                                            - c00 * d20 * (c11 * d10 + c10 * d11)
                                            + c00 * d10 * (c21 * d10 + 2 * c20 * d11)
                                            -2 * c00 * d00 * (c21 * d20 + c20 * d21)
                                            + c10 * d00 * (2 * c11 * d20 + c10 * d21)
                                            + c20 * d00 * (2 * c21 * d00 - c10 * d11)
                                            - d00 * d10 * (c11 * c20 + c10 * c21);

    const double h0 = c00 * c00 * d20 * d20
            + c10 * d20 * (c10 * d00 - c00 * d10)
            + c20 * d10 * (c00 * d10 - c10 * d00)
            + c20 * d00 * (c20 * d00 - 2 * c00 * d20);

    if (m_debug) {
        std::cout <<  "h4  : " << h4 << "\n";
        std::cout <<  "h3  : " << h3 << "\n";
        std::cout <<  "h2  : " << h2 << "\n";
        std::cout <<  "h1  : " << h1 << "\n";
        std::cout <<  "h0  : " << h0 << "\n";

        std::cout << "Messy Calculate Over\n";

        //function is h0 pvx ^4 + h1 pvx ^3 + h2 pvx ^2 + h3 pvx + h4

        std::cout << "gnuplot command\n";

        std::stringstream s;
        s << h0/h0 << "*x*x*x*x + " <<
                h1/h0 << "*x*x*x + " <<
                h2/h0<< "*x*x + " <<
                h3/h0 << "*x + "  << h4/h0;

        std::string fn = s.str();

        std::cout << "plot " << fn << "\n";
    }

    std::vector<double> pvxs = solveQuartic(h0, h1, h2, h3, h4);

    if (m_debug) {
        std::cout << "--------------------------\n";
        std::cout << "Kinematic Solutions\n";
    }

    int num = 0;

    for (std::vector<double>::const_iterator it = pvxs.begin(); it != pvxs.end(); ++it, ++num) {
        double pvx = *it;

        double pvbarx = me_mex - *it;

        double c0 = c00;
        double d0 = d00;

        double c1 = c10 * pvx + c11;
        double c2 = c20 * pvx * pvx + c21 * pvx + c22;

        double d1 = d10 * pvx + d11;
        double d2 = d20 * pvx * pvx + d21 * pvx + d22;

        double pvy = (c0 * d2 - c2 * d0) / (c1 * d0 - c0 * d1);
        double pvbary = me_mey - pvy;

        double pvz =  (a1 + a2 * pvx + a3 * pvy) / (-a4);
        double pvbarz = (b1 + b2 * pvbarx + b3 * pvbary) / (-b4);

        double pvE = sqrt(pvx*pvx + pvy*pvy + pvz*pvz);
        double pvbarE = sqrt(pvbarx*pvbarx + pvbary*pvbary + pvbarz*pvbarz);

        TLorentzVector tlv1(pvx,pvy,pvz,pvE);
        TLorentzVector tlv2(pvbarx,pvbary,pvbarz,pvbarE);

        if (m_debug) {
            std::cout << "Solution " << num << "\n";
            std::cout << tlv1 << std::endl;
            std::cout << tlv2 << std::endl;
            std::cout << "----------" << "\n";
        }

        std::pair<TLorentzVector, TLorentzVector> p(tlv1,tlv2);

        lvs.push_back(p);
    }

    if (m_debug)
        std::cout << "Finished Kinematic Solution" << "\n";

    return lvs;
}

std::vector<double> SonnenscheinEngine::solveQuartic(double h0, double h1, double h2, double h3, double h4) const {
    if (m_debug)
        std::cout << "Solve Quartic\n";

    //normalise to coefficient of highest order.
    const double nh1 = h1 / h0;
    const double nh2 = h2 / h0;
    const double nh3 = h3 / h0;
    const double nh4 = h4 / h0;

    //calculate variables
    const double k1 = nh2 - 3 * nh1 * nh1 / 8.;
    const double k2 = nh3 + nh1 * nh1 * nh1 / 8. - nh1 * nh2 / 2.;
    const double k3 = nh4 - 3 * pow(nh1, 4) / 256. + nh1 * nh1 * nh2 / 16. - nh1 * nh3 / 4.;

    if (m_debug) {
        std::cout << "e " << k1 << "\n";
        std::cout << "f " << k2 << "\n";
        std::cout << "g " << k3 << "\n";
    }

    std::vector<double> sols = solveCubic(2. * k1, k1 * k1 -4. * k3, -k2 * k2);

    //using the solutions from above...
    double t1 = -1.;

    for (std::vector<double>::const_iterator it = sols.begin(); it != sols.end(); ++it) {
        if (*it > 0.)
            t1 = sqrt(*it);
    }

    //we just need one positive root to get the other roots...

    double t2 = (k1 + t1 * t1 - k2 / t1) / 2.;

    if (m_debug)
        std::cout << "first quadratic\n";

    std::vector<double> s1 = solveQuadratic(1., t1, t2);

    std::vector<double> pvxs;

    for (std::vector<double>::const_iterator it = s1.begin(); it != s1.end(); ++it) {

        if (m_debug)
            std::cout << "pvx " << *it - nh1 / 4. << "\n";

        pvxs.push_back(*it - nh1 / 4.);
    }

    //now use the other quadratic
    if (m_debug)
        std::cout << "second quadratic\n";

    std::vector<double> s2 = solveQuadratic(1., -t1, k3 / t2);

    for (std::vector<double>::const_iterator it = s2.begin(); it != s2.end(); ++it) {
        if (m_debug)
            std::cout << "pvx " << *it - nh1 / 4. << "\n";

        pvxs.push_back(*it - nh1 / 4.);
    }

    return pvxs;
}

std::vector<double> SonnenscheinEngine::solveCubic(double s1, double s2, double s3) const {
    std::vector<double> sols;

    const double q = (s1 * s1 - 3 * s2) / 9.;
    const double r = (2 * s1 * s1 * s1 - 9 * s1 * s2 + 27 * s3) / 54.;

    if (m_debug) {
        std::cout << "Solve Cubic\n";
        std::cout << "q " << q << "\n";
        std::cout << "r " << r << "\n";
    }

    if (r*r < q*q*q) {
        const double theta = acos(r / sqrt(q*q*q) );
        const double z1_1 = -2 * sqrt(q) * cos(theta / 3.) - s1 / 3.;
        const double z1_2 = -2 * sqrt(q) * cos((theta + 2 * M_PI) / 3.) - s1 / 3.;
        const double z1_3 = -2 * sqrt(q) * cos((theta - 2 * M_PI) / 3.) - s1 / 3.;

        if (m_debug) {
            std::cout << "r^2 < q^3 -> 3 solutions\n";
            std::cout << "z1_1 " << z1_1 << "\n";
            std::cout << "z1_2 " << z1_2 << "\n";
            std::cout << "z1_3 " << z1_3 << "\n";
        }

        sols.push_back(z1_1);
        sols.push_back(z1_2);
        sols.push_back(z1_3);
    } else {
        if (m_debug)
            std::cout << "r^2 >= q^3 -> 1 solution" << "\n";

        const double radicant = -r + sqrt( r * r - q * q * q);
        const double powthrd = pow( fabs(radicant), 1./3.);
        const double a = sign(radicant) * powthrd; //sign??
        //double b = (a!=0.) ? q/a : 0.;
        const double b = q / a;

        const double z1_1 = a + b - s1 /3.;
        //std::cout << "z1_1 " << z1_1 << "\n";

        sols.push_back(z1_1);
    }

    return sols;
}

std::vector<double> SonnenscheinEngine::solveQuadratic(double a, double b, double c) const {
    std::vector<double> sols;

    if (m_debug)
        std::cout << "Solve Quadratic\n";

    const double b2m4ac = b * b - 4. * a * c;

    if (b2m4ac > 0.) {
        const double sol1 = (-b + sqrt(b2m4ac)) / 2. * a;
        const double sol2 = (-b - sqrt(b2m4ac)) / 2. * a;

        if (m_debug) {
            std::cout << "Quadratic sol1 : " << sol1 << "\n";
            std::cout << "Quadratic sol2 : " << sol2 << "\n";
        }

        sols.push_back(sol1);
        sols.push_back(sol2);
    } else if (m_debug)
        std::cout << "Quadratic has no solutions\n";

    return sols;
}

double SonnenscheinEngine::sign(double a) const {
    return (a < 0) ? -1 : (a > 0) ? 1 : 0;
}

}
