/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerRateSlices/TrigL2MissingEtObject.h"

ClassImp(TrigL2MissingEtObject )


TrigL2MissingEtObject::TrigL2MissingEtObject()
  : TrigMissingEtObject(),
    m_Ez(0),
    m_SumE(0),
    m_flag(0),
    m_c_name(),
    m_c_status(0),
    m_c_ex(0),
    m_c_ey(0),
    m_c_ez(0),
    m_c_sumEt(0),
    m_c_sumE(0),
    m_c_calib0(0),
    m_c_calib1(0),
    m_c_sumOfSigns(0),
    m_c_usedChannels(0) {
}

TrigL2MissingEtObject::TrigL2MissingEtObject(const TrigL2MissingEtObject & other)
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

TrigL2MissingEtObject::~TrigL2MissingEtObject() {
}

void TrigL2MissingEtObject::reset() {
  TrigMissingEtObject::reset();
  m_Ez = 0;
  m_SumE = 0;
  m_flag = 0;
  m_c_name = "";
  m_c_status = 0;
  m_c_ex = 0;
  m_c_ey = 0;
  m_c_ez = 0;
  m_c_sumEt = 0;
  m_c_sumE = 0;
  m_c_calib0 = 0;
  m_c_calib1 = 0;
  m_c_sumOfSigns = 0;
  m_c_usedChannels = 0;
}
