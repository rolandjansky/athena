/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerRateSlices/TrigEFMissingEtObject.h"

ClassImp(TrigEFMissingEtObject )


TrigEFMissingEtObject::TrigEFMissingEtObject()
  : TrigMissingEtObject(),
    m_Ez(0),
    m_SumE(0),
    m_flag(0),
    m_comp_number(25) {
  m_c_name.resize(m_comp_number);
  m_c_status.resize(m_comp_number);
  m_c_ex.resize(m_comp_number);
  m_c_ey.resize(m_comp_number);
  m_c_ez.resize(m_comp_number);
  m_c_sumEt.resize(m_comp_number);
  m_c_sumE.resize(m_comp_number);
  m_c_calib0.resize(m_comp_number);
  m_c_calib1.resize(m_comp_number);
  m_c_sumOfSigns.resize(m_comp_number);
  m_c_usedChannels.resize(m_comp_number);
}

TrigEFMissingEtObject::TrigEFMissingEtObject(const TrigEFMissingEtObject & other)
  : TrigMissingEtObject(other),
    m_Ez( other.m_Ez ),
    m_SumE( other.m_SumE ),
    m_flag( other.m_flag ),
    m_c_name( other.m_c_name ),
    m_c_status( other.m_c_status ),
    m_c_ex( other.m_c_ex ),
    m_c_ey( other.m_c_ey ),
    m_c_ez( other.m_c_ez ),
    m_c_sumEt( other.m_c_sumEt ),
    m_c_sumE( other.m_c_sumE ),
    m_c_calib0( other.m_c_calib0 ),
    m_c_calib1( other.m_c_calib1 ),
    m_c_sumOfSigns( other.m_c_sumOfSigns ),
    m_c_usedChannels( other.m_c_usedChannels ) {
}

TrigEFMissingEtObject::~TrigEFMissingEtObject() {
}

void TrigEFMissingEtObject::reset() {
  TrigMissingEtObject::reset();
  m_Ez = 0;
  m_SumE = 0;
  m_flag = 0;

  // m_comp_number should be known at this stage!
  m_c_name.clear();         m_c_name.resize(m_comp_number);
  m_c_status.clear();       m_c_status.resize(m_comp_number);
  m_c_ex.clear();           m_c_ex.resize(m_comp_number);
  m_c_ey.clear();           m_c_ey.resize(m_comp_number);
  m_c_ez.clear();           m_c_ez.resize(m_comp_number);
  m_c_sumEt.clear();        m_c_sumEt.resize(m_comp_number);
  m_c_sumE.clear();         m_c_sumE.resize(m_comp_number);
  m_c_calib0.clear();       m_c_calib0.resize(m_comp_number);
  m_c_calib1.clear();       m_c_calib1.resize(m_comp_number);
  m_c_sumOfSigns.clear();   m_c_sumOfSigns.resize(m_comp_number);
  m_c_usedChannels.clear(); m_c_usedChannels.resize(m_comp_number);
}
