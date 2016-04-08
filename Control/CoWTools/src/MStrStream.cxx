/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoWTools/MStrStream.h"
#include "CoWTools/CoWRecordStats.h"

void CoWTools::MStrStream::putResults(const CoWTools::CoWRecordStats &res){
  char buff[2048];
  const long long * vals=res.getValueArray();
  snprintf(buff,2048,"VMem= %8lld, RSS= %8lld, PSS= %8lld, Shared= %8lld, Private= %8lld, Swap=%8lld, Anon= %8lld",
	   vals[0],vals[1],vals[2],vals[3]+vals[4],vals[5]+vals[6],vals[10],vals[8]);
  m_out<<buff<<endreq;
}
