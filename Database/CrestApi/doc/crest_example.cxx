/*!
   \file
   \brief Main file

   This file contains the examples for the CREST C++ Client Library.
   Main part of the examples is commented. Pl testCreateGlobalTagMapFsease uncomment the code you need.
   Check and correct (if it is necessary) the CREST Server parameters in CrestClient instances.
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>

#include <CrestApi/CrestApiExt.h>
#include <filesystem>

#include <CrestApi/picosha2.h>


#include "CxxUtils/checker_macros.h"
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace Crest;

using namespace std;
std::string SURL = "http://mvg-pc-04.cern.ch:8090";

void print_path() {
  std::cout << SURL << std::endl;
}

bool createDirTree(const std::string full_path) {
  size_t pos = 0;
  bool ret_val = true;

  while (ret_val == true && pos != std::string::npos) {
    pos = full_path.find('/', pos + 1);
    ret_val = std::filesystem::create_directory(full_path.substr(0, pos));
  }

  return ret_val;
}

//===================================================
// Tag Method Tests
//===================================================

void testCreateTag(std::string tagname) {
  std::cout << std::endl << "test: createTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"description", "none"},
    {"endOfValidity", 0},
    {"insertionTime", "2018-12-06T11:18:35.641+0000"},
    {"lastValidatedTime", 0},
    {"modificationTime", "2018-12-06T11:18:35.641+0000"},
    {"name", tagname},
    {"payloadSpec", "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"},
    {"synchronization", "none"},
    {"timeType", "time"}
  };

  try{
    myCrestClient.createTag(js);
    std::cout << std::endl << "test: createTag (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testCreateTag2() {
  std::cout << std::endl << "test: createTag2" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  // real tag creation example:
  // curl --header "Content-Type: application/json" --request POST --data
  // '{"name":"CaloSwClusterCorrections.00-RUN2-02-11-clcon-dummy","description":"<timeStamp>run-lumi</timeStamp><addrHeader><address_header
  // service_type=\"71\" clid=\"250904980\"
  // /></addrHeader><typeName>CaloRec::ToolConstants</typeName><fullrep/>","timeType":"run-lumi","payloadSpec":"PoolRef:String4k"}'
  // http://crest-01.cern.ch:8090/crestapi/tags

  nlohmann::json js =
  {
    {"name", "CaloSwClusterCorrections.00-RUN2-02-11-clcon-dummy"},
    {"description",
     "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"250904980\" /></addrHeader><typeName>CaloRec::ToolConstants</typeName><fullrep/>"},
    {"timeType", "run-lumi"},
    {"payloadSpec", "PoolRef:String4k"}
  };

  try{
    myCrestClient.createTag(js);
    std::cout << std::endl << "test: createTag2 (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTag2 (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// Method to create a tag on the local storage (on disk).

void testCreateTagF(std::string tagname) {
  std::cout << std::endl << "test: createTagF" << std::endl;

  bool rewrite = true;
  std::string path = "/tmp/cresttest/crest_dump";
  std::filesystem::path dir(path);
  std::filesystem::create_directories(path);
  CrestClient myCrestClient = CrestClient(rewrite, path);

  nlohmann::json js =
  {
    {"description", "none"},
    {"endOfValidity", 0},
    {"insertionTime", "2018-12-06T11:18:35.641+0000"},
    {"lastValidatedTime", 0},
    {"modificationTime", "2018-12-06T11:18:35.641+0000"},
    {"name", tagname},
    {"payloadSpec", "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"},
    {"synchronization", "none"},
    {"timeType", "time"}
  };

  try{
    myCrestClient.createTag(js);
    std::cout << std::endl << "test: createTagF (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagF (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListTags() {
  std::cout << std::endl << "test: listTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    nlohmann::json tag_list = myCrestClient.listTags();
    std::cout << std::endl << "test: listTags (result) =" << std::endl
              << tag_list.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listTags (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListTagsParams() {
  std::cout << std::endl << "test: listTagsParams" << std::endl;
  CrestClientExt myCrestClient = CrestClientExt(SURL);

  try{
    // variants how to call this method:
    // myCrestClient.listTagsParams();
    // nlohmann::json res = myCrestClient.listTagsParams(100,200);

    nlohmann::json res = myCrestClient.listTagsParams(_page = 3, _size = 5);

    std::cout << std::endl << "test: listTagsParams (result) ="
              << "result = " << res.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listTagsParams (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testListTagsParams(std::string name) {
  std::cout << std::endl << "test: listTagsParams" << std::endl;
  CrestClientExt myCrestClient = CrestClientExt(SURL);

  try{
    // variants how to call this method:
    // nlohmann::json res = myCrestClient.listTagsParams();
    // nlohmann::json res = myCrestClient.listTagsParams("",100,200);

    nlohmann::json res1 = myCrestClient.listTagsParams(_page = 3, _size = 5);

    std::cout << "test: listTagsParams (A) = "
              << std::endl << res1.dump(4) << std::endl;

    nlohmann::json res2 = myCrestClient.listTagsParams(_name = name, _page = 0, _size = 5);

    std::cout << "test: listTagsParams (B) = "
              << std::endl << res2.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listTagsParams (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testFindTag(std::string tagname) {
  std::cout << std::endl << "test: findTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    nlohmann::json tag_info = myCrestClient.findTag(tagname);
    std::cout << std::endl << "test: findTag (result) =" << std::endl
              << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testRemoveTag(std::string tagname) {
  std::cout << std::endl << "test: removeTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    myCrestClient.removeTag(tagname);
    std::cout << std::endl << "test: removeTag (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: removeTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testUpdateTag(std::string tagname) {
  std::cout << std::endl << "test: updateTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js = {{"objectType", "json3"},
                       {"description", "test"}};

  try{
    myCrestClient.updateTag(tagname, js);
    std::cout << std::endl << "test: updateTag (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: updateTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testUpdateTagSpecification(std::string tagname) {
  std::cout << std::endl << "test: updateTagSpecification" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  std::string newObjectType = "json4";

  try{
    myCrestClient.updateTagSpecification(tagname, newObjectType);
    std::cout << std::endl << "test: updateTagSpecification (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: updateTagSpecification (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//===================================================
// Global Tag Method Tests
//===================================================

void testCreateGlobalTag(std::string tagname) {
  std::cout << std::endl << "test: createGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"name", tagname},
    {"validity", 0},
    {"description", "test"},
    {"release", "1"},
    {"insertionTime", "2018-12-18T11:32:58.081+0000"},
    {"snapshotTime", "2018-12-18T11:32:57.952+0000"},
    {"scenario", "test"},
    {"workflow", "M"},
    {"type", "t"},
    {"snapshotTimeMilli", "null"},
    {"insertionTimeMilli", "null"}
  };

  try {
    myCrestClient.createGlobalTag(js);
    std::cout << std::endl << "test: createGlobalTag (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createGlobalTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testFindGlobalTag(std::string tagname) {
  std::cout << std::endl << "test: findGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    nlohmann::json tag_info = myCrestClient.findGlobalTag(tagname);
    std::cout << std::endl << "test: findGlobalTag (result) = " << std::endl
              << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findGlobalTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListGlobalTags() {
  std::cout << std::endl << "test: listGlobalTags" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json tag_info9 = myCrestClient.listGlobalTags();
    std::cout << std::endl << "test: listGlobalTags (result) = "
              << tag_info9.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listGlobalTags (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListGlobalTagsAsString() {
  std::cout << std::endl << "test: listGlobalTagsAsString" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json tag_info9 = myCrestClient.listGlobalTagsAsString();
    std::cout << std::endl << "test: listGlobalTagsAsString (result) = "
              << tag_info9.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listGlobalTagsAsString (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListGlobalTagsParams(std::string name, int size, int page) {
  std::cout << std::endl << "test: listGlobalTagsParams with additional parameters" << std::endl;
  CrestClientExt myCrestClient = CrestClientExt(SURL);

  try{
    nlohmann::json list_1 = myCrestClient.listGlobalTagsParams(name, size, page);
    nlohmann::json list_2 = myCrestClient.listGlobalTagsParams(_name = name, _size = size, _page = page);
    nlohmann::json list_3 = myCrestClient.listGlobalTagsParams(_name = "", _size = size, _page = page);

    //
    std::cout << std::endl << "test: listGlobalTagsParams (result A) =" << std::endl;
    std::cout << list_1.dump(4) << std::endl;
    //

    //
    std::cout << std::endl << "test: listGlobalTagsParams (result B) =" << std::endl;
    std::cout << list_2.dump(4) << std::endl;
    //

    std::cout << std::endl << "test: listGlobalTagsParams (result C) =" << std::endl;
    std::cout << list_3.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listGlobalTagsParams with additional parameters (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testRemoveGlobalTag(std::string tagname) {
  std::cout << std::endl << "test: removeGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    myCrestClient.removeGlobalTag(tagname);
    std::cout << std::endl << "test: removeGlobalTag (success)" << std::endl;
  }
  catch (const std::runtime_error& e) {
    std::cout << std::endl << "test: removeGlobalTag (failed)" << std::endl;
  }
}

void testUpdateGlobalTag(std::string tagname) {
  std::cout << std::endl << "test: updateGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"name", tagname},
    {"validity", 0},
    {"description", "test"},
    {"release", "1"},
    {"insertionTime", "2018-12-18T11:32:58.081+0000"},
    {"snapshotTime", "2018-12-18T11:32:57.952+0000"},
    {"scenario", "test2"},
    {"workflow", "M"},
    {"type", "t"},
    {"snapshotTimeMilli", "null"},
    {"insertionTimeMilli", "null"}
  };

  try {
    myCrestClient.updateGlobalTag(tagname, js);
    std::cout << std::endl << "test: updateGlobalTag (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: updateGlobalTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testFindGlobalTagAsString(std::string tagname) {
  std::cout << std::endl << "test: findGlobalTagAsString" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    nlohmann::json tag_info = myCrestClient.findGlobalTagAsString(tagname);
    std::cout << std::endl << "test: findGlobalTagAsString (result) = " << std::endl
              << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findGlobalTagAsString (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//===================================================
// RunLumiInfo Method Tests
//===================================================

void testCreateRunLumiInfo() {
  std::cout << std::endl << "test: createRunLumiInfo" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  std::string str =
    "{\"since\":\"10\",\"run\":\"7777771\",\"lb\":\"62\",\"starttime\":\"10\",\"endtime\":\"200\"}";
  nlohmann::json js = myCrestClient.getJson(str);

  try{
    myCrestClient.createRunLumiInfo(js);
    std::cout << std::endl << "test: createRunLumiInfo (success) = " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createRunLumiInfo (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testListRunLumiInfo() {
  std::cout << std::endl << "test: listRunLumiInfo" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json info16 = myCrestClient.listRunLumiInfo();
    std::cout << std::endl << "test: listRunLumiInfo (success) = " << std::endl;
    std::cout << info16.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listRunLumiInfo (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//===================================================
// Global Tag Map Method Tests
//===================================================

void testFindGlobalTagMap(std::string mapname) {
  std::cout << std::endl << "test: findGlobalTagMap" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json tag_info = myCrestClient.findGlobalTagMap(mapname);
    std::cout << std::endl << "test: findGlobalTagMap (result) = "
              << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findGlobalTagMap (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// global tag and tag have to exist
void testCreateGlobalTagMap(std::string globaltag, std::string tagname) {
  std::cout << std::endl << "test: createGlobalTagMap" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"globalTagName", globaltag},
    {"record", "testing2"},
    {"label", "test2"},
    {"tagName", tagname}
  };

  try{
    myCrestClient.createGlobalTagMap(js);
    std::cout << std::endl << "test: createGlobalTagMap (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createGlobalTagMap (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//===================================================
// Auxillary Method Tests
//===================================================

void testGetJson() {
  std::cout << std::endl << "test: getJson" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  std::string stra =
    "{\"tagName\":\"IndetBeampos-nominal\",\"since\":0,\"insertionTime\":\"2019-02-21T13:52:06.222+0000\",\"payloadHash\":\"de9afd9a5bb526d175265d1e66520ac1e7a81816cc3081925625759583e9107f\"}";
  std::string strb = "test";

  try{
    std::cout << myCrestClient.getJson(stra).dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getJson for string stra (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  try{
    std::cout << myCrestClient.getJson(strb).dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getJson for string strb (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  std::cout << "getJson test ended" << std::endl;
}

//===================================================
// Tag + Payloads Method Tests
//===================================================

// Be carrful tag has to be creted before this test!
void testStorePayload(std::string tagname) {
  std::cout << std::endl << "test: storePayload" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  // payload + tag name (command line example):
  // curl --form file=@./test.txt --form tag="SB_TAG-PYLD" --form since=0
  // http://mvg-test-pc-03.cern.ch:8090/crestapi/payloads/store

  uint64_t since = 0;
  std::string payload = "aaa";


  try{
    myCrestClient.storePayload(tagname, since, payload);
    std::cout << std::endl << "test: storePayload (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storePayload (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// This test for the local file storage method storePayloadDump.

void testStorePayloadDump(std::string tagname) {
  std::cout << std::endl << "test: storePayloadDump" << std::endl;
  bool rewrite = true;
  std::string path = "/tmp/cresttest/crest_dump";
  std::filesystem::create_directories(path);
  CrestClient myCrestClient = CrestClient(rewrite, path);

  uint64_t since = 200;


  std::string str =
    "{\"niovs\": 2,\"format\":\"PYLD_JSON\",\"iovsList\":[{\"since\":800,\"payload\":\"vv1\"},{\"since\":900,\"payload\":\"ww1\"}]}";

  nlohmann::json js = myCrestClient.getJson(str);

  try {
    myCrestClient.storePayloadDump(tagname, since, str);
    std::cout << std::endl << "test: storePayloadDump (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storePayloadDump (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//===================================================
// IOV List Method Tests
//===================================================

void testFindAllIovs(std::string tagname) {
  std::cout << std::endl << "test: findAllIovs" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json tag_info = myCrestClient.findAllIovs(tagname);
    std::cout << std::endl << "test: findAllIovs (result) =" << std::endl;
    std::cout << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findAllIovs (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testFindAllIovsParams(std::string tagname, int size, int page) {
  std::cout << std::endl << "test: findAllIovs with additional parameters" << std::endl;
  CrestClientExt myCrestClient = CrestClientExt(SURL);

  try{
    nlohmann::json iov_list_1 = myCrestClient.findAllIovsParams(tagname, size, page);
    nlohmann::json iov_list_2 = myCrestClient.findAllIovsParams(tagname, _page = size, _size = page);

    std::cout << std::endl << "test: findAllIovs (result A) =" << std::endl;
    std::cout << iov_list_1.dump(4) << std::endl;

    std::cout << std::endl << "test: findAllIovs (result B) =" << std::endl;
    std::cout << iov_list_2.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findAllIovs with additional parameters (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testFindAllIovsFS(std::string tagname) {
  std::cout << std::endl << "test: findAllIovsFS" << std::endl;

  CrestClient myCrestClient = CrestClient(true);

  try{
    nlohmann::json tag_info = myCrestClient.findAllIovsFs(tagname);
    std::cout << std::endl << "test: findAllIovsFS (result) =" << std::endl;
    std::cout << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findAllIovsFS (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// test to check a method with additional parametrs:

void testFindAllIovsFSPlus(std::string tagname, int size, int page) {
  std::cout << std::endl << "test: findAllIovsFSPlus" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  try{
    nlohmann::json tag_info = myCrestClient.findAllIovsFs(tagname, size, page);
    std::cout << std::endl << "test: findAllIovsFSPlus (result) =" << std::endl;
    std::cout << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findAllIovsFSPlus (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testSelectIovsParams(std::string tagname, long since, long until) {
  std::cout << std::endl << "test: selectIovs with additional parameters" << std::endl;
  CrestClientExt myCrestClient = CrestClientExt(SURL);

  try{
    nlohmann::json iov_list_1 = myCrestClient.selectIovs(tagname, since, until);
    nlohmann::json iov_list_2 = myCrestClient.selectIovs(tagname, since, -1);

    std::cout << std::endl << "test: selectIovs (result A) =" << std::endl;
    std::cout << iov_list_1.dump(4) << std::endl;

    std::cout << std::endl << "test: selectIovs (result B) =" << std::endl;
    std::cout << iov_list_2.dump(4) << std::endl;

    std::cout << "result A size = " << iov_list_1.size() << std::endl;
    std::cout << "result B size = " << iov_list_2.size() << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: selectIovs with additional parameters (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testSelectIovsFS(std::string tagname, long since, long until) {
  std::cout << std::endl << "test: selectIovsFS with additional parameters" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  try{
    nlohmann::json iov_list = myCrestClient.selectIovsFS(tagname, since, until);

    std::cout << std::endl << "test: selectIovs =" << std::endl;
    std::cout << iov_list.dump(4) << std::endl;

    std::cout << "result size = " << iov_list.size() << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: selectIovsFS with additional parameters (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// IOV List Methods (end)


//===================================================
// Tag Meta Info Method Tests
//===================================================

void testCreateTagMetaInfo(std::string tagname) {
  std::cout << std::endl << "test: createTagMetainfo" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"tagName", tagname},
    {"description", "desc-01"},
    {"tagInfo", "taginf-01"},
    {"chansize", 0},
    {"colsize", 0}
  };

  try{
    myCrestClient.createTagMetaInfo(js);
    std::cout << std::endl << "test: createTagMetaInfo (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagMetaInfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// Test for the method with 2 arguments:
void testCreateTagMetaInfo2Args(std::string tagname) {
  std::cout << std::endl << "test: createTagMetainfo" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"tagName", tagname},
    {"description", "desc-01"},
    {"tagInfo", "taginf-01"},
    {"chansize", 0},
    {"colsize", 0},
    {"insertionTime", "2019-03-14T13:29:25.286Z"}
  };

  try{
    std::cout << "test A:" << std::endl;
    myCrestClient.createTagMetaInfo(tagname, js);
    std::cout << "test: createTagMetaInfo (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagMetaInfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
  try{
    std::cout << "test B:" << std::endl;
    myCrestClient.createTagMetaInfo("test", js);
    // myCrestClient.createTagMetaInfo(js);
    std::cout << "test: createTagMetaInfo (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagMetaInfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// analogue
void testCreateTagMetaInfo2(std::string tagname) {
  std::cout << std::endl << "test: createTagMetainfo2" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"chansize", 1},
    {"colsize", 0},
    {"description",
     "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"},
    {"insertionTime", "Fri Nov 13 16:31:10 2020"},
    {"tagName", tagname}
  };

  try{
    myCrestClient.createTagMetaInfo(js);
    std::cout << std::endl << "test: createTagMetaInfo (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagMetaInfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// Example with more detailed parametrs:
void testCreateTagMetaInfoDetailed(std::string tagname) {
  std::cout << std::endl << "test: createTagMetainfoDetailed" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json channel = {{"0", "ATLAS_PREFERRED"}};

  std::cout << std::endl << "channel = " << std::endl
            << channel.dump(4) << std::endl;

  nlohmann::json chanList = nlohmann::json::array({channel});

  nlohmann::json tagInfo =
  {
    {"channel_list", chanList},
    {"node_description", "description of the node"},
    {"payload_spec", "payload specification"}
  };

  std::cout << std::endl << "TagInfo = " << std::endl
            << tagInfo.dump(4) << std::endl;

  std::cout << std::endl << "TagInfo = " << std::endl
            << tagInfo.dump() << std::endl;

  nlohmann::json js =
  {
    {"tagName", tagname},
    {"description", "none"},
    {"chansize", 1},
    {"colsize", 6},
    {"tagInfo", tagInfo.dump()},
    {"insertionTime", "2020-12-04"}
  };


  try{
    myCrestClient.createTagMetaInfo(js);
    std::cout << std::endl << "test: createTagMetaInfoDetailed (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagMetaInfoDetailed (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetTagMetaInfo(std::string tagname) {
  std::cout << std::endl << "test: getTagMetaInfo" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json js = myCrestClient.getTagMetaInfo(tagname);
    std::cout << std::endl << "test: getTagMetaInfo (result) =" << std::endl
              << js.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getTagMetaInfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testUpdateTagMetainfo(std::string tagname) {
  std::cout << std::endl << "test: updateTagMetainfo" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"tagName", tagname},
    {"description", "desc-02"}
  };

  try{
    myCrestClient.updateTagMetaInfo(js);
    std::cout << std::endl << "test: updateTagMetaInfo (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: updateTagMetainfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

//===================================================
//  Tag Meta Info Method Tests in IOVDbSvc format
//===================================================

void testCreateTagMetaInfoIOVDbSvc(std::string tagname) {
  std::cout << std::endl << "test: createTagMetainfoIOVDbSvc" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);
  std::string description = "meta tag description";

  nlohmann::json channel1 = {{"0", "ATLAS_PREFERRED0"}};
  nlohmann::json channel2 = {{"1", "ATLAS_PREFERRED1"}};

  nlohmann::json chanList = nlohmann::json::array({channel1, channel2});

  nlohmann::json tagInfo =
  {
    {"channel_list", chanList},
    {"node_description", "description of the node"},
    {"payload_spec",
     "AlgorithmID:UInt32,LBAvInstLumi:Float,LBAvEvtsPerBX:Float,LumiType:UInt32,Valid:UInt32,BunchInstLumi:Blob64k"}
  };

  try{
    myCrestClient.createTagMetaInfoIOVDbSvc(tagname, tagInfo, description);
    std::cout << std::endl << "test: createTagMetaInfoIOVDbSvc (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagMetaInfoIOVDbSvc (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetTagMetaInfoIOVDbSvc(std::string tagname) {
  std::cout << std::endl << "test: getTagMetaInfoIOVDbSvc" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json js68 = myCrestClient.getTagMetaInfoIOVDbSvc(tagname);
    std::cout << std::endl << "test: getTagMetaInfoIOVDbSvc (result) = " << std::endl
              << js68.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getTagMetaInfoIOVDbSvc (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testConvertTagMetaInfo2CREST() {
  std::cout << std::endl << "test: convertTagMetaInfo2CREST" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  std::string tagname = "test_MvG4";

  nlohmann::json channel1 = {{"0", "ATLAS_PREFERRED0"}};
  nlohmann::json channel2 = {{"1", "ATLAS_PREFERRED1"}};

  nlohmann::json chanList = nlohmann::json::array({channel1, channel2});

  nlohmann::json tagInfo =
  {
    {"channel_list", chanList},
    {"node_description", "description of the node"},
    {"payload_spec",
     "AlgorithmID:UInt32,LBAvInstLumi:Float,LBAvEvtsPerBX:Float,LumiType:UInt32,Valid:UInt32,BunchInstLumi:Blob64k"}
  };

  std::cout << std::endl << "tagInfo = " << std::endl
            << tagInfo.dump(4) << std::endl;

  try{
    nlohmann::json res = myCrestClient.convertTagMetaInfo2CREST(tagInfo);
    std::cout << std::endl
              << "Tag Meta Info in CREST format = " << res.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: convertTagMetaInfo2CREST (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

//================================================
//  Monitoring Method Tests in IOVDbSvc format
//================================================

void testListPayloadTagInfo() {
  std::cout << std::endl << "test: listPayloadTagInfo" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    nlohmann::json tag_list = myCrestClient.listPayloadTagInfo();
    std::cout << std::endl << "test: listPayloadTagInfo (result) =" << std::endl
              << tag_list.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listPayloadTagInfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testListPayloadTagInfo(std::string tagname) {
  std::cout << std::endl << "test: listPayloadTagInfo with tag " << tagname << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    nlohmann::json tag_list = myCrestClient.listPayloadTagInfo(tagname);
    std::cout << std::endl << "test: listPayloadTagInfo (result) =" << std::endl
              << tag_list.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: listPayloadTagInfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

//================================================
//  Payload and Blob Method Tests
//================================================


void testGetBlobInStream(std::string hash) {
  std::cout << std::endl << "test: getBlobInStream" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    remove("data.txt");

    std::ofstream out8;          // stream for writing
    out8.open("data.txt"); // open the file to write
    if (out8.is_open()) {
      string tag_info8 = myCrestClient.getBlobInStream(hash, out8);
      std::cout << std::endl << "test: getBlobInStream (result) =" << std::endl;
      std::cout << tag_info8 << std::endl;
      std::cout << std::endl << "test: getBlobInStream" << std::endl;
    }
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getBlobInStream (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetBlob(std::string hash) {
  std::cout << std::endl << "test: getBlob" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    string tag_info7 = myCrestClient.getBlob(hash);
    std::cout << std::endl << "test: getBlob (result) =" << std::endl;
    std::cout << tag_info7 << std::endl;
    std::cout << std::endl << "test: getBlob" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getBlob (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetPayloadAsJson(std::string hash) {
  std::cout << std::endl << "test: getPayloadAsJson" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json info26 = myCrestClient.getPayloadAsJson(hash);
    std::cout << std::endl << "test: getPayloadAsJson (result) =" << std::endl;
    std::cout << info26.dump(4) << std::endl;
    std::cout << std::endl << "test: getPayloadAsJson" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getPayloadAsJson (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetPayloadAsString(std::string hash) {
  std::cout << std::endl << "test: getPayloadAsString" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    std::string info27 = myCrestClient.getPayloadAsString(hash);
    std::cout << std::endl << "test: getPayloadAsString (result) =" << std::endl;
    std::cout << info27 << std::endl;
    std::cout << std::endl << "test: getPayloadAsString" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getPayloadAsString (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetPayloadMetaInfoAsString(std::string hash) {
  std::cout << std::endl << "test: getPayloadMetaInfoAsString" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    std::string info15 = myCrestClient.getPayloadMetaInfoAsString(hash);
    std::cout << std::endl << "test: getPayloadMetaInfoAsString (result) =" << std::endl;
    std::cout << info15 << std::endl;
    std::cout << std::endl << "test: getPayloadMetaInfoAsString" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getPayloadMetaInfoAsString (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetPayloadMetaInfo(std::string hash) {
  std::cout << std::endl << "test: getPayloadMetaInfo" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    nlohmann::json info14 = myCrestClient.getPayloadMetaInfo(hash);
    std::cout << std::endl << "test: getPayloadMetaInfo (result) =" << std::endl;
    std::cout << info14.dump(4) << std::endl;
    std::cout << std::endl << "test: getPayloadMetaInfo" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getPayloadMetaInfo (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

//===================================================
//  Tests for File Storage Methods
//===================================================

// Tag Meta Info:

// Example with detailed parametrs:
void testCreateTagMetaInfoDetailedFs(std::string tagname) {
  std::cout << std::endl << "test: createTagMetainfoDetailedFs" << std::endl;

  CrestClient myCrestClient = CrestClient(true);

  nlohmann::json channel = {{"0", "ATLAS_PREFERRED"}};

  std::cout << std::endl << "channel = " << std::endl
            << channel.dump(4) << std::endl;

  nlohmann::json chanList = nlohmann::json::array({channel});

  nlohmann::json tagInfo =
  {
    {"channel_list", chanList},
    {"node_description", "description of the node"},
    {"payload_spec", "payload specification"}
  };

  std::cout << std::endl << "TagInfo = " << std::endl
            << tagInfo.dump(4) << std::endl;

  std::cout << std::endl << "TagInfo = " << std::endl
            << tagInfo.dump() << std::endl;

  nlohmann::json js =
  {
    {"tagName", tagname},
    {"description", "none"},
    {"chansize", 1},
    {"colsize", 6},
    {"tagInfo", tagInfo.dump()},
    {"insertionTime", "2020-12-04"}
  };


  try{
    myCrestClient.createTagMetaInfo(js);
    std::cout << std::endl << "test: createTagMetaInfoDetailedFs (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagMetaInfoDetailedFs (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetTagMetaInfoFs(std::string tagname) {
  std::cout << std::endl << "test: getTagMetaInfoFs" << std::endl;

  CrestClient myCrestClient = CrestClient(true);

  try{
    nlohmann::json js = myCrestClient.getTagMetaInfo(tagname);
    std::cout << std::endl << "test: getTagMetaInfoFs (result) =" << std::endl
              << js.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getTagMetaInfoFs (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testUpdateTagMetaInfoFs(std::string tagname) {
  std::cout << std::endl << "test: updateTagMetainfoFs" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  nlohmann::json channel = {{"0", "ATLAS_PREFERRED"}};

  std::cout << std::endl << "channel = " << std::endl
            << channel.dump(4) << std::endl;

  nlohmann::json chanList = nlohmann::json::array({channel});

  nlohmann::json tagInfo =
  {
    {"channel_list", chanList},
    {"node_description", "description of the node"},
    {"payload_spec", "payload specification"}
  };

  std::cout << std::endl << "TagInfo = " << std::endl
            << tagInfo.dump(4) << std::endl;

  std::cout << std::endl << "TagInfo = " << std::endl
            << tagInfo.dump() << std::endl;

  nlohmann::json js =
  {
    {"tagName", tagname},
    {"description", "none"},
    {"chansize", 1},
    {"colsize", 6},
    {"tagInfo", tagInfo.dump()},
    {"insertionTime", "2020-12-04"}
  };

  try{
    // myCrestClient.updateTagMetaInfoFs(tagname+"a",js); // test for wrong tag name, uncomment it if you want test this
    // case.
    myCrestClient.updateTagMetaInfoFs(tagname, js);
    std::cout << std::endl << "test: updateTagMetaInfoFs (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: updateTagMetaInfoFs (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// Tag Methods:

void testFindTagFs(std::string tagname) {
  std::cout << std::endl << "test: findTagFs" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  std::cout << std::endl << "test: findTagFs" << std::endl;

  try {
    nlohmann::json tag_info = myCrestClient.findTag(tagname);
    std::cout << std::endl << "test: findTagFs (result) =" << std::endl
              << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findTagFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testCreateTagFs(std::string tagname) {
  std::cout << std::endl << "test: createTagFs" << std::endl;

  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  nlohmann::json js =
  {
    {"description", "none"},
    {"endOfValidity", 0},
    {"insertionTime", "2018-12-06T11:18:35.641+0000"},
    {"lastValidatedTime", 0},
    {"modificationTime", "2018-12-06T11:18:35.641+0000"},
    {"name", tagname},
    {"payloadSpec", "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"},
    {"synchronization", "none"},
    {"timeType", "time"}
  };

  try{
    myCrestClient.createTag(js);
    std::cout << std::endl << "test: createTagFs (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// Payloads methods:

void testStoreBatchPayloadsFs(std::string tagname) {
  std::cout << std::endl << "test: storeBatchPayloads" << std::endl;

  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  uint64_t endtime = 200;
  std::string str = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
  nlohmann::json js = myCrestClient.getJson(str);

  try {
    myCrestClient.storeBatchPayloads(tagname, endtime, js);
    std::cout << std::endl << "test: storeBatchPayloads (success) " << std::endl;
    //myCrestClient.flush();
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storeBatchPayloads (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// Global Tag Methods:

void testCreateGlobalTagFs(std::string tagname) {
  std::cout << std::endl << "test: createGlobalTagFs" << std::endl;

  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  nlohmann::json js =
  {
    {"name", tagname},
    {"validity", 0},
    {"description", "test"},
    {"release", "1"},
    {"insertionTime", "2018-12-18T11:32:58.081+0000"},
    {"snapshotTime", "2018-12-18T11:32:57.952+0000"},
    {"scenario", "test"},
    {"workflow", "M"},
    {"type", "t"},
    {"snapshotTimeMilli", "null"},
    {"insertionTimeMilli", "null"}
  };

  try {
    myCrestClient.createGlobalTagFs(js);
    std::cout << std::endl << "test: createGlobalTagFs (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createGlobalTagFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testFindGlobalTagFs(std::string tagname) {
  std::cout << std::endl << "test: findGlobalTagFs" << std::endl;
  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  try {
    nlohmann::json tag_info = myCrestClient.findGlobalTagFs(tagname);
    std::cout << std::endl << "test: findGlobalTagFs (result) = " << std::endl
              << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findGlobalTagFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// Global Tag Map Methods:

void testCreateGlobalTagMapFs(std::string globaltag, std::string tagname) {
  std::cout << std::endl << "test: createGlobalTagMapFs" << std::endl;
  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  nlohmann::json js =
  {
    {"globalTagName", globaltag},
    {"record", "testing2"},
    {"label", "test2"},
    {"tagName", tagname}
  };

  try{
    myCrestClient.createGlobalTagMapFs(js);
    std::cout << std::endl << "test: createGlobalTagMapFs (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createGlobalTagMapFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testFindGlobalTagMapFs(std::string tagname) {
  std::cout << std::endl << "test: findGlobalTagMapFs" << std::endl;
  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  try {
    nlohmann::json tag_info = myCrestClient.findGlobalTagMapFs(tagname);
    std::cout << std::endl << "test: findGlobalTagMapFs (result) = " << std::endl
              << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: findGlobalTagMapFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// Hash calculation tests

void testGetHash(std::string str) {
  std::cout << std::endl << "test: getHash" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  std::cout << myCrestClient.getHash(str) << std::endl;
}

void hashCalculationTest(std::string str) {
  std::cout << "Hash(" << str << ") = ";
  std::string hash_hex_str = picosha2::hash256_hex_string(str.begin(), str.end());
  std::cout << hash_hex_str << std::endl;
}

// Global Tag tests:

void testCreateGlobalTag(std::string tagname, std::string description) {
  std::cout << std::endl << "test: createGlobalTag (2 parameters)" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    myCrestClient.createGlobalTag(tagname, description);
    std::cout << std::endl << "test: createGlobalTag (success) (2 parameters)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createGlobalTag (failed) (2 parameters)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// global tag and tag have to exist
void testCreateGlobalTagMap(std::string globaltag, std::string tagname,
                            std::string record, std::string label) {
  std::cout << std::endl << "test: createGlobalTagMap (all parameters)" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js =
  {
    {"globalTagName", globaltag},
    {"record", record},
    {"label", label},
    {"tagName", tagname}
  };

  try{
    myCrestClient.createGlobalTagMap(js);
    std::cout << std::endl << "test: createGlobalTagMap (all parameters) (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createGlobalTagMap (all parameters) (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// current time & date test

void testGetDateAndTime() {
  std::cout << std::endl << "test: getDateAndTime" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try{
    std::string time = myCrestClient.getDateAndTime();
    std::cout << std::endl << "now = " << time << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getDateAndTime (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// payload method tests for file starage

void testGetPayloadAsStringFS(std::string hash) {
  std::cout << std::endl << "test: getPayloadAsStringFS" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  try {
    std::string info = myCrestClient.getPayloadAsStringFS(hash);
    std::cout << std::endl << "test: getPayloadAsStringFS (result) =" << std::endl
              << info << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getPayloadAsStringFS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetPayloadAsJsonFS(std::string hash) {
  std::cout << std::endl << "test: getPayloadAsJsonFS" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  try {
    nlohmann::json info = myCrestClient.getPayloadAsJsonFS(hash);
    std::cout << std::endl << "test: getPayloadAsJsonFS (result) =" << std::endl
              << info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getPayloadAsJsonFS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetPayloadMetaInfoAsStringFS(std::string hash) {
  std::cout << std::endl << "test: getPayloadMetaInfoAsStringFS" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  try {
    std::string info = myCrestClient.getPayloadMetaInfoAsStringFS(hash);
    std::cout << std::endl << "test: getPayloadMetaInfoAsStringFS (result) =" << std::endl
              << info << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getPayloadMetaInfoAsStringFS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetPayloadMetaInfoAsJsonFS(std::string hash) {
  std::cout << std::endl << "test: getPayloadMetaInfoAsJsonFS" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  try {
    nlohmann::json info = myCrestClient.getPayloadMetaInfoAsJsonFS(hash);
    std::cout << std::endl << "test: getPayloadMetaInfoAsJsonFS (result) =" << std::endl
              << info.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getPayloadMetaInfoAsJsonFS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetBlobInStreamFS(std::string hash) {
  std::cout << std::endl << "test: getBlobInStreamFs" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  try{
    remove("data.txt");

    std::ofstream out8;          // stream for writing
    out8.open("data.txt"); // open the file to write
    if (out8.is_open()) {
      string tag_info8 = myCrestClient.getBlobInStreamFs(hash, out8);
      std::cout << std::endl << "test: getBlobInStreamFs (result) =" << std::endl;
      std::cout << tag_info8 << std::endl;
      std::cout << "blob is in the file data.txt" << std::endl;
      std::cout << std::endl << "test: getBlobInStreamFs" << std::endl;
    }
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getBlobInStreamFs (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testGetBlobFS(std::string hash) {
  std::cout << std::endl << "test: getBlobFs" << std::endl;
  CrestClient myCrestClient = CrestClient(true);

  try {
    std::string info = myCrestClient.getBlobFs(hash);
    std::cout << std::endl << "test: getBlobFs (result) =" << std::endl
              << info << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getBlobFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testReturnJArray() {
  std::cout << std::endl << "test: returnJArray" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  nlohmann::json js = {
    {"name", "test"}
  };

  try {
    nlohmann::json res = myCrestClient.returnJArray(js);
    std::cout << std::endl << "test: returnJArray (result) =" << std::endl
              << res.dump(4) << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: returnJArray (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//=====================================================
// Tests for storeBatchPayloads methods
// (i.e. IOVs and payloads stored in batch mode)
//=====================================================


// Test for the storeBatchPayloads method. It has two payloads with the predefined payloadHash values ("aaa" and "bbb").

void testStoreBatchPayloads(std::string tagname) {
  std::cout << std::endl << "test: storeBatchPayloads" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  uint64_t endtime = 200;
  std::string str = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
  nlohmann::json js = myCrestClient.getJson(str);

  try {
    myCrestClient.storeBatchPayloads(tagname, endtime, js);
    std::cout << std::endl << "test: storeBatchPayloads (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storeBatchPayloads (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// Test for the storeBatchPayloads method. The payloads are calculated each time.

void testStoreBatchPayloadsB(std::string tagname) {
  std::cout << std::endl << "test: storeBatchPayloads2" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  uint64_t endtime = 200;
  std::string d1 = "\"" + myCrestClient.getDateAndTime() + "_A\"";
  std::string d2 = "\"" + myCrestClient.getDateAndTime() + "_B\"";

  std::string str = "[{\"payloadHash\":" + d1 + ",\"since\":100},{\"payloadHash\":" + d2 + ",\"since\":150}]";
  nlohmann::json js = myCrestClient.getJson(str);

  try {
    myCrestClient.storeBatchPayloads(tagname, endtime, js);
    std::cout << std::endl << "test: storeBatchPayloads2 (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storeBatchPayloads2 (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// This test creates n IOVs/payloads for the tag with name tagname

void testTagAndStoreBatchFsNItems(std::string tagname, int n) {
  std::cout << std::endl << "test: createTagFs" << std::endl;

  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  nlohmann::json js =
  {
    {"description", "none"},
    {"endOfValidity", 0},
    {"insertionTime", "2018-12-06T11:18:35.641+0000"},
    {"lastValidatedTime", 0},
    {"modificationTime", "2018-12-06T11:18:35.641+0000"},
    {"name", tagname},
    {"payloadSpec", "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"},
    {"synchronization", "none"},
    {"timeType", "time"}
  };

  try{
    myCrestClient.createTag(js);
    std::cout << std::endl << "test: storeBatchPayloads for N items (a tag created)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storeBatchPayloads for N items (tag creation failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  int step = 10;
  uint64_t endtime = (n + 1) * step;

  nlohmann::json data = nlohmann::json::array();

  for (int i = 1; i <= n; i++) {
    int since = i * step;
    std::string d = myCrestClient.getDateAndTime() + "_" + std::to_string(i);

    nlohmann::json itemD =
    {
      {"payloadHash", d},
      {"since", since}
    };

    data.push_back(itemD);
  } // end i

  try {
    myCrestClient.storeBatchPayloads(tagname, endtime, data);
    std::cout << std::endl << "test: storeBatchPayloads for N items (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storeBatchPayloads for N items (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// The test to create a tag with the iovs/payloads together.

void testTagAndStoreBatchFs(std::string tagname) {
  std::cout << std::endl << "test: createTagFs" << std::endl;

  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  nlohmann::json js =
  {
    {"description", "none"},
    {"endOfValidity", 0},
    {"insertionTime", "2018-12-06T11:18:35.641+0000"},
    {"lastValidatedTime", 0},
    {"modificationTime", "2018-12-06T11:18:35.641+0000"},
    {"name", tagname},
    {"payloadSpec", "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"},
    {"synchronization", "none"},
    {"timeType", "time"}
  };

  try{
    myCrestClient.createTag(js);
    std::cout << std::endl << "test: createTagFs (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  uint64_t endtime = 200;
  std::string str = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
  nlohmann::json js2 = myCrestClient.getJson(str);

  try {
    myCrestClient.storeBatchPayloads(tagname, endtime, js2);
    std::cout << std::endl << "test: storeBatchPayloads (success) " << std::endl;
    //myCrestClient.flush();
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storeBatchPayloads (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

void testTagAndStoreBatchFsB(std::string tagname) {
  std::cout << std::endl << "test: createTagFs" << std::endl;

  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  nlohmann::json js =
  {
    {"description", "none"},
    {"endOfValidity", 0},
    {"insertionTime", "2018-12-06T11:18:35.641+0000"},
    {"lastValidatedTime", 0},
    {"modificationTime", "2018-12-06T11:18:35.641+0000"},
    {"name", tagname},
    {"payloadSpec", "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"},
    {"synchronization", "none"},
    {"timeType", "time"}
  };

  try{
    myCrestClient.createTag(js);
    std::cout << std::endl << "test: createTagFs (success)" << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: createTagFs (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }

  uint64_t endtime = 200;
  std::string d1 = "\"" + myCrestClient.getDateAndTime() + "_A\"";
  std::string d2 = "\"" + myCrestClient.getDateAndTime() + "_B\"";

  std::string str = "[{\"payloadHash\":" + d1 + ",\"since\":100},{\"payloadHash\":" + d2 + ",\"since\":150}]";
  nlohmann::json js2 = myCrestClient.getJson(str);

  try {
    myCrestClient.storeBatchPayloads(tagname, endtime, js2);
    std::cout << std::endl << "test: storeBatchPayloads A (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storeBatchPayloads A (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }

  d1 = "\"" + myCrestClient.getDateAndTime() + "_C\"";
  d2 = "\"" + myCrestClient.getDateAndTime() + "_D\"";

  str = "[{\"payloadHash\":" + d1 + ",\"since\":200},{\"payloadHash\":" + d2 + ",\"since\":250}]";
  js2 = myCrestClient.getJson(str);

  try {
    myCrestClient.storeBatchPayloads(tagname, endtime, js2);
    std::cout << std::endl << "test: storeBatchPayloads B (success) " << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: storeBatchPayloads B (failed)" << std::endl;
    std::cerr << e.what() << std::endl;
  }
}

// IOV Number Methods:

void testGetSize(std::string tagname) {
  std::cout << std::endl << "test: getSize" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    int info = myCrestClient.getSize(tagname);
    std::cout << std::endl << "test: getSize (result) = "
              << info << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getSize (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetSizeByTag(std::string tagname) {
  std::cout << std::endl << "test: getSizeByTag" << std::endl;
  CrestClient myCrestClient = CrestClient(SURL);

  try {
    int info = myCrestClient.getSizeByTag(tagname);
    std::cout << std::endl << "test: getSizeByTag (result) = "
              << info << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getSizeByTag (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

void testGetSizeFS(std::string tagname) {
  std::cout << std::endl << "test: getSizeFS" << std::endl;

  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  try {
    int info = myCrestClient.getSizeFS(tagname);
    std::cout << std::endl << "test: getSizeFS (result) = "
              << info << std::endl;
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: getSizeFS (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

// Auxillary tests

void test01() {
  map<std::string, std::string> m;
  m["key1"] = "val1";

  map <std::string, std::string> :: iterator it = m.begin();
  cout << "Map Listing 1: " << endl;
  for (int i = 0; it != m.end(); it++, i++) {  // Output
    cout << i << ") key: " << it->first << ", value: " << it->second << endl;
  }

  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);
  nlohmann::json iovs = myCrestClient.findAllIovsFs("MvG_01");
  std::cout << "IOV List = " << std::endl << iovs.dump(4) << std::endl;

  int length = iovs.size();
  std::cout << "IOV number = " << length << std::endl;
  for (int i = 0; i < length; i++) {
    std::cout << "item (" << i << ") = " << std::endl << iovs[i].dump(4) << std::endl;

    auto r = iovs[i].find("payloadHash");
    if (r != iovs[i].end()) {
      std::string hash = iovs[i].value("payloadHash", "NONE");
      std::cout << "hash = " << hash << std::endl;
      std::cout << "iov = " << iovs[i].dump() << std::endl;
      m[hash] = iovs[i].dump();
    }
  }

  it = m.begin();
  cout << "Map Listing 2: " << endl;
  for (int i = 0; it != m.end(); it++, i++) {  // Output
    cout << i << ") key: " << it->first << ", value: " << it->second << endl;
  }
}

// Tag statistics method test:

void testGetTagDataInfo(std::string tagname) {
  bool rewrite = true;
  CrestClient myCrestClient = CrestClient(rewrite);

  try{
    myCrestClient.getTagDataInfo(tagname);
  }
  catch (const std::exception& e) {
    std::cout << std::endl << "test: testGetTagDataInfo (failed)" << std::endl;
    std::cout << e.what() << std::endl;
  }
}

//----------------------

int main(int argc, char* argv[]) {
  if (argc == 2) {
    // print_path();

    SURL = argv[1];
    print_path();

    // TO RUN TEST UNCOMMENT A LINE!
    // (and check parameters)

    testCreateTag("test_MvG3b"); // error !
    // testListTags();
    testFindTag("test_MvG3b");
    // testRemoveTag("test_MvG3b");
    // testUpdateTag("test_MvG3b");
    // testUpdateTagSpecification("test_MvG3b");

    // testCreateGlobalTag("MvG_TEST_01");
    // testFindGlobalTag("MvG_TEST_01");
    // testFindGlobalTagAsString("MvG_TEST_01");
    // testListGlobalTags();
    // testListGlobalTagsAsString();
    // testRemoveGlobalTag("MvG_TEST_01");
    // testUpdateGlobalTag("MvG_TEST_01");

    // testCreateRunLumiInfo(); // it works on crest-01.cern.ch:8080
    // testListRunLumiInfo(); // it works on crest-01.cern.ch:8080

    // testCreateGlobalTagMap("MvG_TEST_01","test_MvG3b"); // global tag and tag have to exist
    // testFindGlobalTagMap("MvG_TEST_01");

    // testStorePayload("test_MvG3b");

    // testCreateTagF("test_MvG3b"); // Create a tag on a file storage
    // testStorePayloadDump("test_MvG3b");
    testStoreBatchPayloads("test_MvG3b");

    // testGetJson();

    // iovs tests (begin)
    testFindAllIovs("test_MvG3b");
    // iovs tests (end)

    testCreateTagMetaInfo("test_MvG3b");
    // testUpdateTagMetainfo("test_MvG3b");
    testGetTagMetaInfo("test_MvG3b");


    // testCreateTagMetaInfoIOVDbSvc("test_MvG3b"); // it works on crest-01.cern.ch:8080
    // testGetTagMetaInfoIOVDbSvc("test_MvG3b"); // problem on crest-01.cern.ch:8080
    // testConvertTagMetaInfo2CREST();

    // testCreateTag("test_MvG3c");
    // testCreateTagMetainfoIOVDbSvc("test_MvG3c"); // it works on crest-01.cern.ch:8080
    // testGetTagMetaInfoIOVDbSvc("test_MvG3c"); // problem on crest-01.cern.ch:8080  // uncomment
    // testGetTagMetaInfo("test_MvG3c");

    // testListTagsParams();
    // testFindAllIovsParams("test_MvG3c", 1,1);
    // testListTags(); // added for test
    // testListPayloadTagInfo();
    // testListPayloadTagInfo("test_MvG3b");
    // testFindAllIovsParams("PixCalib-DATA-RUN2-UPD4-10", 2, 2);

    testCreateTagMetaInfo2("test_MvG3b");
    testCreateTag2();
    // testGetBlobInStream("3e744b9dc39389baf0c5a0660589b8402f3dbb49b89b3e75f2c9355852a3c677");
    // testGetBlobInStream("9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0");
  } else {
    std::cout << "CREST Server path not found" << std::endl;
    std::cout << "Please, run this program with a server path:" << std::endl;
    std::cout << "crest_example http://mvg-pc-04.cern.ch:8090" << std::endl;
  }
  std::cout << "Test ended" << std::endl;
  return 0;
}
