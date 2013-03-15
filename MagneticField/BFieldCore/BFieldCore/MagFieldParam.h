/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MAGFIELDPARAM_H
#define MAGFIELDPARAM_H
#include "BFieldCore/MagFieldMisc.h"
#include <iostream>


class MagFieldParam {
 public:

  MagFieldParam();
  ~MagFieldParam();

public:
///////////////////////////////////
//Data
//  Options
    float m_binner           ;
    float m_rmainn           ;
    float m_zmainn           ;
    float m_eps0             ;
    float m_rtori            ;
    float m_ztori            ;
    float m_rtoro            ;
    float m_ztoro            ;

//  File to be read
    std::string m_NameOfTheFile;
    std::string m_NameOfTheSource;
    std::string m_NoField;
    std::string m_SolenoidField;
    std::string outFactory;
//
    int    m_itf;
    int    m_InfosChain;
    double m_dx,m_dy,m_dz;
    double m_dpsi   ;
    double m_dphi   ;
    double m_dtheta ;
//      
    double m_dxsol,m_dysol,m_dzsol;
    double m_dpsisol   ;
    double m_dphisol   ;
    double m_dthetasol ;
//      
    double m_h8dx1,m_h8dx2,m_h8dx3;
    double m_h8dy1,m_h8dy2,m_h8dy3;
    double m_h8dz1,m_h8dz2,m_h8dz3;
//
   int noTiltNoShift;
//

   bool doScale,calcul;
   double m_TorCur,m_SolCur;
   double m_TorCurNom,m_SolCurNom;
   double m_TorScale,m_SolScale;
   void setParam(const std::string&);
   void printParam(std::ostream &); 
//
 private:
  void setDefault();
};

#endif
