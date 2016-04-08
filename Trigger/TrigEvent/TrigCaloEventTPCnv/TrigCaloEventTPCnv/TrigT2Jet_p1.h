/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2Jet_p1
 *
 * @brief persistent partner for TrigT2Jet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigT2Jet_p1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JET_P1_H
#define TRIGCALOEVENTTPCNV_TRIGT2JET_P1_H

class TrigT2Jet_p1 
{
  friend class TrigT2JetCnv_p1;

 public:
  
  TrigT2Jet_p1() :
  m_e(0.0), m_ehad0(0.0), m_eem0(0.0), m_eta(0.0), m_phi(0.0), m_roiWord(0)
  {}
  virtual ~TrigT2Jet_p1(){}
  
 private:
  
  float m_e;
  float m_ehad0;
  float m_eem0;
  float m_eta;
  float m_phi  ;
  //float m_coneRadius  ;
  long m_roiWord;
};

#endif
