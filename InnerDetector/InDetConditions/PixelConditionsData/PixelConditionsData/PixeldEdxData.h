/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixeldEdxData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date Octorber, 2019
 * @brief Store pixel dEdx data in PixeldEdxData.
 */


#ifndef PIXELDEDXDATA_H
#define PIXELDEDXDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <string>
#include <vector>
#include <unordered_map>

#include "AthenaKernel/CondCont.h"

class PixeldEdxData {
  public:
    void   setPar(const int i, const double param);
    void   setPosNeg(const bool posneg);
    void   setFunctionType(const std::string &fun);
    void   setBetheBlochType(const std::string &bb);
    void   setMinimumdEdxForMass(const double mindedxMass);

    double dEdxPdf(double dedx, double p, double mass, std::array<double,9> par, int offset) const;
    double fdEdx_zero(double x, std::array<double,9> par) const;

    double getPar(int i, int j) const; 
    void   getP(double dedx, double p, int nGoodPixels, std::array<double,3> vhypo) const;
    void   getFirstNPar(std::array<double,9> par, double p, int nGoodPixels, int np) const;
    double getMass(double dedx, double p, int nGoodPixels) const;
    double getdEdx(double p, double mass, int nGoodPixels) const;
    double getdEdx(double p, double mass, std::array<double,9> par) const;

    // Crystal Ball distribution
    double Func1(double x,double x0,double sig,double alp,double n) const;
    // Asymetric Gaussian distribution
    double Func2(double x,double x0,double sig,double asym) const;
    // Moyal distribution
    double Func3(double x,double Ep,double R) const;

    double dEdx_5p_BG_aleph(double xbg, std::array<double,9>& pp) const;
    double dEdx_5p_aleph(double p,double mass, std::array<double,9>& pp) const;
    double dEdx_5p_BG(double xbg, std::array<double,9>& pp) const;
    double dEdx_5p(double p,double mass, std::array<double,9>& pp) const;
    double dEdx_BG(double xbg, std::array<double,9>& pp) const;
    double dEdx_def(double p,double mass, std::array<double,9>& pp) const;
    double dEdx_3p(double p,double mass, std::array<double,9>& pp) const;

    std::vector<float> getLikelihoods(double dedx2, double p2, int nGoodPixels) const;

  private:
    static constexpr double m_pi = 0.13957;
    static constexpr double m_k  = 0.49368;
    static constexpr double m_p  = 0.93827;

    std::unordered_map<uint32_t,std::vector<double>> m_par;
    bool m_posneg;
    std::string m_fun;
    std::string m_bb;
    double m_mindedxformass;

};

CLASS_DEF( PixeldEdxData , 127567078 , 1 )

CONDCONT_DEF( PixeldEdxData, 127560679 );

#endif
