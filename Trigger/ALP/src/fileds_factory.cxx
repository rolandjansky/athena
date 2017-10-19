/*

Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Author: Sami Kama 2017
*/
#include "ALP/FileDataSource.h"

extern "C" hltinterface::DataSource* create_hltmp_datasource(){
  return new ALPNS::FileDataSource();
}

extern "C" void destroy_hltmp_datasource(hltinterface::DataSource* i){
  ALPNS::FileDataSource* k=reinterpret_cast<ALPNS::FileDataSource*>(i);
  delete k;
}
