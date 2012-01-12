/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauEtMissL2Object.h"
ClassImp(TrigTauEtMissL2Object )


TrigTauEtMissL2Object::TrigTauEtMissL2Object()
  : TrigTauEtMissObject(),
    m_Ez(-1.e+9),
    m_SumE(-1.e+9),
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
    m_c_usedChannels(0)
{}

TrigTauEtMissL2Object::TrigTauEtMissL2Object(const TrigTauEtMissL2Object & other)
  : TrigTauEtMissObject(other),
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
    m_c_usedChannels( other.m_c_usedChannels )
{}

TrigTauEtMissL2Object::~TrigTauEtMissL2Object(){}

void TrigTauEtMissL2Object::reset()
{
  TrigTauEtMissObject::reset();
  m_Ez = -1.e+9;
  m_SumE = -1.e+9;
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
