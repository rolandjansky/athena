#include <cassert>
#include <iostream>

#include <nlohmann/json.hpp>

#include <CrestApi/CrestApi.h>
#include <filesystem>

using namespace Crest;

int main() {
  std::cout << "CREST Client Libtary test 2\n";

  int retv = 0;

  // CrestApi Library innitialization for local data storage:

  std::string work_dir = "/tmp/crest_dump";
  bool rewrite = true;
  CrestClient testCrestClient = CrestClient(rewrite,work_dir); // CrestClient();

  //==============================================
  // Tag Creation

  std::cout << std:: endl << "test: createTag" << std::endl;

  // Tag description as a JSON:

  nlohmann::json js=
  {
    {"description", "none"},
    {"endOfValidity", 0},
    {"insertionTime", "2018-12-06T11:18:35.641+0000"},
    {"lastValidatedTime", 0},
    {"modificationTime", "2018-12-06T11:18:35.641+0000"},
    {"name", "test_MvG3a"},
    {"payloadSpec", "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"},
    {"synchronization", "none"},
    {"timeType", "time"}
  };

  try{
    testCrestClient.createTag(js);
    std::cout << std:: endl << "test: createTag (success)" << std::endl;
  }
  catch (const std::runtime_error& e) {
    std::cout << std:: endl << "test: createTag (failed)" << std::endl;
    retv = 1;
  }

 
  //==============================================
  // Tag Reading

     std::cout << std:: endl << "test: findTag" << std::endl;
     std::string tagname = "test_MvG3a";

     try {
        nlohmann::json tag_info = testCrestClient.findTag(tagname);
        std::cout << std:: endl << "test: findTag (result)" << std::endl
        << tag_info.dump(4) << std::endl;
     }
     catch(const std::runtime_error& e){
        std::cout << std:: endl << "test: findTag (failed)" << std::endl;
        retv = 1;
     }
   //

  return retv;
}
