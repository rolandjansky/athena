/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoWTools/VecStream.h"
#include "CoWTools/CoWRecordStats.h"

void CoWTools::VecStream::putResults(const CoWTools::CoWRecordStats &res){
  m_out.clear();
  m_out.resize(7);
  const long long * vals=res.getValueArray();
  m_out.at(0)=vals[0];
  m_out.at(1)=vals[1];
  m_out.at(2)=vals[2];
  m_out.at(3)=vals[3]+vals[4];
  m_out.at(4)=vals[5]+vals[6];
  m_out.at(5)=vals[8];
  m_out.at(6)=vals[10];
}
