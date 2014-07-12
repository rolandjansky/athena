#include "DataQualityUtils/MonitoringFile.h"
#include <iostream>
#include <string>

int main(int argc,char* argv[]){
  if(argc<6){
    std::cout<<"need ResultsDir, inputFile, hanconfig.hcfg ,min10_config.hcfg,min30_config.hcfg"<<std::endl;
    return 5;
  }
  std::string outDir=std::string(argv[1]);
  std::string inFile(argv[2]);
  std::string conf(argv[3]);
  std::string conf10(argv[4]);
  std::string conf30(argv[5]);
  dqutils::MonitoringFile mf(inFile);
  std::cout<<"Running GetHanResults"<<std::endl;
  mf.getHanResults(outDir,inFile,conf,conf10,conf30 );
  return 0;
}
