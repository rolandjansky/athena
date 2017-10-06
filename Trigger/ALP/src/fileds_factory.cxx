#include "ALP/FileDataSource.h"

extern "C" hltinterface::DataSource* create_hltmp_datasource(){
  return new ALPNS::FileDataSource();
}

extern "C" void destroy_hltmp_datasource(hltinterface::DataSource* i){
  ALPNS::FileDataSource* k=reinterpret_cast<ALPNS::FileDataSource*>(i);
  delete k;
}
