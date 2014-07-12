#include "DataQualityUtils/MonitoringFile.h"
#include <iostream>
#include <string>

int main(int argc,char* argv[]){
  if(argc<3){
    std::cout<<"need listFilename outfilename"<<std::endl;
    return 5;
  }
  std::string listFileName=std::string(argv[1]);
  std::string outFileName(argv[2]);
  dqutils::MonitoringFile mf;
  std::cout<<"Running MergeFiles"<<std::endl;
  mf.mergeFiles( outFileName, listFileName );
  std::cout<<"Running MergeLBintervals"<<std::endl;
  mf.mergeLBintervals( outFileName );

  return 0;
}
