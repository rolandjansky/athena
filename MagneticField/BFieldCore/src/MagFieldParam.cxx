/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BFieldCore/MagFieldParam.h"

MagFieldParam::MagFieldParam(){
  std::cout << "MagFieldParam                                        INFO MagFieldParam SetDefault     " << std::endl;
  setDefault();
}
MagFieldParam::~MagFieldParam(){}


 void MagFieldParam::setDefault()
{
//  Set Default values
    m_binner           = 20.     ;
    m_rmainn           = 105.    ;
    m_zmainn           = 320.    ;
    m_eps0             = 5.      ;
    m_rtori            = 423.0   ;
    m_ztori            = 681.5   ;
    m_rtoro            = 1500.0  ;
    m_ztoro            = 2300.0  ;
    m_TorCurNom        =99999.0  ;
    m_SolCurNom        = 7730.0  ;
    m_TorCur           =99999.0  ;
    m_SolCur           = 7730.0  ;
//
    m_dx               = 9999.   ;
    m_dy               = 9999.   ;
    m_dz               = 9999.   ;
    m_dpsi             = 9999.   ;
    m_dphi             = 9999.   ;
    m_dtheta           = 9999.   ;
//
    m_dxsol            = 9999.   ;
    m_dysol            = 9999.   ;
    m_dzsol            = 9999.   ;
    m_dpsisol          = 9999.   ;
    m_dphisol          = 9999.   ;
    m_dthetasol        = 9999.   ;
//
    m_h8dx1            = 9999.   ;
    m_h8dx2            = 9999.   ;
    m_h8dx3            = 9999.   ;
    m_h8dy1            = 9999.   ;
    m_h8dy2            = 9999.   ;
    m_h8dy3            = 9999.   ;
    m_h8dz1            = 9999.   ;
    m_h8dz2            = 9999.   ;
    m_h8dz3            = 9999.   ;
//
    m_itf              = 9999    ;
    m_InfosChain       = 9999    ;
    noTiltNoShift      = 9999    ;
//
    doScale            = false   ;
    calcul             = false   ;
//
    outFactory         = " init print factory";
}
void MagFieldParam::setParam(const std::string& outFact)
{
    outFactory = outFact;
    return;
}
void MagFieldParam::printParam(std::ostream &outperso)
{
  outperso<< "  " << "\n";
  outperso<<"MagFieldParam                                        INFO : m_binner        "<< m_binner << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_rmainn        "<< m_rmainn << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_zmainn        "<< m_zmainn << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_eps0          "<< m_eps0   << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_rtori         "<< m_rtori  << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_ztori         "<< m_ztori  << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_rtoro         "<< m_rtoro  << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_ztoro         "<< m_ztoro  << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_itf           "<< m_itf    << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dx            "<< m_dx     << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dy            "<< m_dy     << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dz            "<< m_dz     << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dpsi          "<< m_dpsi   << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dphi          "<< m_dphi   << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dtheta        "<< m_dtheta << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dx            "<< m_dxsol  << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dy            "<< m_dysol  << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dz            "<< m_dzsol  << "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dpsi          "<< m_dpsisol<< "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dphi          "<< m_dphisol<< "\n" ;
  outperso<<"MagFieldParam                                        INFO : m_dtheta        "<< m_dthetasol<< "\n";
  outperso<<"MagFieldParam                                        INFO : scale           "<< doScale  << "\n";
  outperso<<"MagFieldParam                                        INFO : Calcul          "<< calcul   << "\n";
  outperso<<"MagFieldParam                                        INFO : noTiltNoShift   "<< noTiltNoShift<< "\n";
//outperso<<"MagFieldParam                                        INFO : outFactory      "<< outFactory << "\n";
  return ;
}
