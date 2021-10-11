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
#include <array>

#include "AthenaKernel/CondCont.h"

class PixeldEdxData {
  public:
    void   setPar(const int i, const double param);
    void   setPosNeg(const bool posneg);
    void   setFunctionType(const std::string &fun);
    void   setBetheBlochType(const std::string &bb);
    void   setMinimumdEdxForMass(const double mindedxMass);

    double dEdxPdf(const double dedx, const double signedP, const double mass, const std::array<double,9> & par, const int offset) const;
    double fdEdxZero(const double x, const std::array<double,9> & par) const;

    double getPar(const int i, const int j) const; 
    std::array<double, 3>   getP(const double dedxArg, const double signedP, const int nGoodPixels) const;
    std::array<double,9>   getFirstNPar( const double p, const int nGoodPixels, const int np) const;
    double getMass(const double dedx, const double signedP, const int nGoodPixels) const;
    double getdEdx(const double p, const double mass, const int nGoodPixels) const;
    double getdEdx(const double p, const double mass, const std::array<double,9> & par) const;

    // Crystal Ball distribution
    static double crystalBall(const double x, const double x0,const double sig,const double alp,const double n) ;
    // Asymetric Gaussian distribution
    static double asymGaus(const double x,const double x0,const double sig,const double asym) ;
    // Moyal distribution
    static double moyal(const double x,const double Ep,const double R) ;

    static double dEdx_5p_BG_aleph(const double xbg, const std::array<double,9>& pp) ;
    static double dEdx_5p_aleph(const double p,double mass, const std::array<double,9>& pp) ;
    static double dEdx_5p_BG(const double xbg, const std::array<double,9>& pp) ;
    static double dEdx_5p(const double p,const double mass, const std::array<double,9>& pp) ;
    static double dEdx_BG(const double xbg, const std::array<double,9>& pp) ;
    static double dEdx_def(const double p,const double mass, const std::array<double,9>& pp) ;
    static double dEdx_3p(const double p, const double mass, const std::array<double,9>& pp) ;

    std::vector<float> getLikelihoods(const double dedx2, const double p2, const int nGoodPixels) const;

  private:
    static constexpr double m_piMass = 0.13957;
    static constexpr double m_kMass  = 0.49368;
    static constexpr double m_pMass  = 0.93827;

    std::unordered_map<uint32_t,std::vector<double>> m_par;
    bool m_posneg;
    std::string m_fun;
    std::string m_bb;
    double m_mindedxformass;

};

CLASS_DEF( PixeldEdxData , 127567078 , 1 )

CONDCONT_DEF( PixeldEdxData, 127560679 );

#endif
