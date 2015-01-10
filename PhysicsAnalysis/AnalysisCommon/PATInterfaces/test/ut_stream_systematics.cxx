// Author: Steve Farrell
// Unit test for the streaming feature of the Systematics objects:
// SystematicVariation, SystematicSet, and std::vector<SystematicSet>

#include <iostream>
#include <vector>

#include <TObject.h>
#include <TSystem.h>
#include <TFile.h>

#include <PATInterfaces/SystematicVariation.h>
#include <PATInterfaces/SystematicSet.h>
#include <PATInterfaces/StreamTestWrapper.h>




/// Main unit test function
int main(int /*argc*/, char** /*argv*/)
{
  //std::cout << "running ut_stream_systematics" << std::endl;
  std::string fileDir =
    gSystem->ExpandPathName("/tmp/$USER/ut_stream_systematics/");
  std::string mkdirCommand = "mkdir -p " + fileDir;
  system(mkdirCommand.c_str());
  std::string fileName = fileDir + "sysStreamTest.root";

  // Instantiate a single systematic variation
  CP::SystematicVariation sysVar("sys1", 1);
  // Instantiate a systematic set
  CP::SystematicSet sysSet;
  sysSet.insert(CP::SystematicVariation("sys2", 1));
  sysSet.insert(CP::SystematicVariation("sys3", 2));
  // Instantiate a list of systematic sets
  std::vector<CP::SystematicSet> sysList(3);
  sysList[1].insert(sysSet);
  sysList[2].insert(CP::SystematicVariation("sys4", -1));

  {
    // Open the output file
    TFile f(fileName.c_str(), "recreate");

    // Create the wrapper
    SysStreamTest::StreamTestWrapper sysWrapper(sysVar, sysSet, sysList);
    //std::cout << std::endl << "Dumping systematics before writing" << std::endl;
    //sysWrapper.print();

    // Write the wrapper obj to file
    sysWrapper.Write("sysWrapper");
    f.Close();
  }

  {
    // Now open the file again
    TFile f(fileName.c_str());

    // Grab the sys obj
    SysStreamTest::StreamTestWrapper* sysWrapper = 
      (SysStreamTest::StreamTestWrapper*) f.Get("sysWrapper");
    //std::cout << std::endl << "Dumping systematics after reading" << std::endl;
    //sysWrapper->print();

    // Compare the initial systematics to the written/read ones
    if(sysVar.name() != sysWrapper->sysVar().name()){
      std::cout << "sysVar names don't agree!" << std::endl;
      return 1;
    }
    if(sysSet.name() != sysWrapper->sysSet().name()){
      std::cout << "sysSet names don't agree!" << std::endl;
      return 1;
    }
    if(sysList.size() != sysWrapper->sysList().size()){
      std::cout << "sysList size doesn't agree!" << std::endl;
      return 1;
    }
    for(unsigned int i = 0; i < sysList.size(); ++i){
      if(sysList[i].name() != sysWrapper->sysList()[i].name()){
        std::cout << "sysList name doesn't agree!" << std::endl;
        return 1;
      }
    }

    f.Close();
  }

  return 0;
}
