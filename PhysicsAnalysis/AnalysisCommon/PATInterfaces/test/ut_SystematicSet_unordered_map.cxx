// Author: Steve Farrell
// Unit test for the hash and unordered_map usage of SystematicSet

#include <iostream>
#include <unordered_map>

#include <PATInterfaces/SystematicVariation.h>
#include <PATInterfaces/SystematicSet.h>

namespace sys
{
  enum Config
  {
    NOMINAL = 0,
    SET1,
    SET2,
    SET3
  };
}

/// Main unit test function
int main(int argc, char** /*argv*/)
{

  // Any argument turns on verbose
  bool verbose = false;
  if(argc > 1) verbose = true;

  if(verbose) std::cout << "Running ut_SystematicSet_unordered_map" << std::endl;

  // Instantiate some SystematicSets

  // Nominal set
  CP::SystematicSet sysNom;

  // var1
  CP::SystematicSet sysSet1 = { CP::SystematicVariation("sys1", 1) };

  // var2
  CP::SystematicSet sysSet2 = { CP::SystematicVariation("sys1", -1),
                                CP::SystematicVariation("sys2", -1) };

  // var3
  CP::SystematicSet sysSet3 = { CP::SystematicVariation("sys1", 2),
                                CP::SystematicVariation("sys2", -1),
                                CP::SystematicVariation("sys3", 0.5) };

  // Dump some information
  if(verbose){
    std::cout << "Before saving" << std::endl;
    std::cout << sys::NOMINAL << " sysNom  " << sysNom.name()  << " " << sysNom.hash()  << std::endl;
    std::cout << sys::SET1    << " sysSet1 " << sysSet1.name() << " " << sysSet1.hash() << std::endl;
    std::cout << sys::SET2    << " sysSet2 " << sysSet2.name() << " " << sysSet2.hash() << std::endl;
    std::cout << sys::SET3    << " sysSet3 " << sysSet3.name() << " " << sysSet3.hash() << std::endl;
  }

  // Add values to a map keyed by SystematicSet
  std::unordered_map<CP::SystematicSet, int> map;
  map[sysNom] = sys::NOMINAL;
  map[sysSet1] = sys::SET1;
  map[sysSet2] = sys::SET2;
  map[sysSet3] = sys::SET3;

  // Retrieve the values from the map by key
  if(verbose){
    std::cout << "After saving" << std::endl;
    std::cout << "sysNom  " << map[sysNom]  << std::endl;
    std::cout << "sysSet1 " << map[sysSet1] << std::endl;
    std::cout << "sysSet2 " << map[sysSet2] << std::endl;
    std::cout << "sysSet3 " << map[sysSet3] << std::endl;
    std::cout << "another nom " << map[CP::SystematicSet()] << std::endl;
  }

  // Test that the retrieval makes sense
  if(map[sysNom] != sys::NOMINAL ||
     map[sysSet1] != sys::SET1 ||
     map[sysSet2] != sys::SET2 ||
     map[sysSet3] != sys::SET3)
  {
    std::cout << "ERROR reading back results from std::unordered_map<SystematicSet,int>" << std::endl;
    std::cout << "key sysNom  " << map[sysNom]  << " should be " << sys::NOMINAL << std::endl;
    std::cout << "key sysSet1 " << map[sysSet1] << " should be " << sys::SET1 << std::endl;
    std::cout << "key sysSet2 " << map[sysSet2] << " should be " << sys::SET2 << std::endl;
    std::cout << "key sysSet3 " << map[sysSet3] << " should be " << sys::SET3 << std::endl;
    std::cout << "There is something wrong with the hash functionality for SystematicSet" << std::endl;
    return 1;
  }

  // Successful test
  return 0;

}
