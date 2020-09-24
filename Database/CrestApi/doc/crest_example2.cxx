/*!
   \file
   \brief Main file

   This file contains the examples for the CREST C++ Client Library.
   Main part of the examples is commented. Please uncomment the code you need.
   Check and correct (if it is necessary) the CREST Server parameters in CrestClient instances.
 */

#include <iostream>
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>

#include <CrestApi/CrestApi.h>
#include <filesystem>
// #include <dir.h>

using namespace Crest;
// # define S_PATH "/crestapi"
// # define S_PORT "8080"
// # define SURL "http://mvg-pc-04.cern.ch:8090"

using namespace std;
std::string SURL = "http://mvg-pc-04.cern.ch:8090";

void print_path(){
  cout << (std::string)SURL << endl;
}


bool createDirTree(const std::string full_path){
    size_t pos = 0;
    bool ret_val = true;

    while(ret_val == true && pos != std::string::npos)
    {
        pos = full_path.find('/', pos + 1);
        // ret_val = createDir(full_path.substr(0, pos));
        ret_val = std::filesystem::create_directory(full_path.substr(0, pos));
    }

    return ret_val;
}


//===================================================
// Tag Method Tests
//===================================================

void testCreateTag(std::string tagname){
  std::cout << std:: endl << "test: createTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  // std::string url = "http://mvg-pc-04.cern.ch:8090";
  // CrestClient myCrestClient = CrestClient(url);

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
    std::cout << std:: endl << "test: createTag (success)" << std::endl;
  }
  catch (const std::runtime_error& e) {
    std::cout << std:: endl << "test: createTag (failed)" << std::endl;
  }
}


// Method to create a tag on the local storage (on disk).

void testCreateTagF(std::string tagname){
  std::cout << std:: endl << "test: createTag" << std::endl;

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
    std::cout << std:: endl << "test: createTag (success)" << std::endl;
  }
  catch (const std::runtime_error& e) {
    std::cout << std:: endl << "test: createTag (failed)" << std::endl;
  }
}

void testListTags(){
  std::cout << std:: endl << "test: createTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: listTags" << std::endl;

  try {
     nlohmann::json tag_list = myCrestClient.listTags();
     std::cout << std:: endl << "test: listTags (result)" << std::endl
       << tag_list.dump(4) << std::endl;
  }
     catch(const std::runtime_error& e){
     std::cout << std:: endl << "test: listTags (failed)" << std::endl;
  }
}


void testFindTag(std::string tagname){
  std::cout << std:: endl << "test: findTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: findTag" << std::endl;
  // string name2 = "test_MvG3";

  try {
     nlohmann::json tag_info = myCrestClient.findTag(tagname);
     std::cout << std:: endl << "test: findTag (result)" << std::endl
       << tag_info.dump(4) << std::endl;
  }
     catch(const std::runtime_error& e){
     std::cout << std:: endl << "test: findTag (failed)" << std::endl;
  }
}


void testRemoveTag(std::string tagname){
  std::cout << std:: endl << "test: createTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: removeTag" << std::endl;
  // std::string tagName = "test_MvG7";

  try {
     myCrestClient.removeTag(tagname);
     std::cout << std:: endl << "test: removeTag (success)" << std::endl;
  }
     catch(const std::runtime_error& e){
     std::cout << std:: endl << "test: removeTag (failed)" << std::endl;
  }
}

void testUpdateTag(std::string tagname){
  std::cout << std:: endl << "test: updateTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: updateTag" << std::endl;

  nlohmann::json js = {{"objectType","json3"}};

  try{
     myCrestClient.updateTag(tagname, js);
     std::cout << std:: endl << "test: updateTag (success)" << std::endl;
  }
     catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: updateTag (failed)" << std::endl;
  }
}

void testUpdateTagSpecification(std::string tagname){
  std::cout << std:: endl << "test: updateTagSpecification" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: updateTagSpecification" << std::endl;

  std::string newObjectType = "json4";

  try{
     myCrestClient.updateTagSpecification(tagname, newObjectType);
     std::cout << std:: endl << "test: updateTagSpecification (success)" << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: updateTagSpecification (failed)" << std::endl;
  }
}

//===================================================
// Global Tag Method Tests
//===================================================

void testCreateGlobalTag(std::string tagname){
  std::cout << std:: endl << "test: createGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: createGlobalTag" << std::endl;
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
    std::cout << std:: endl << "test: createGlobalTag (success) " << std::endl;
  }
  catch (const std::runtime_error& e) {
    std::cout << std:: endl << "test: createGlobalTag (failed)" << std::endl;
  }
}


void testFindGlobalTag(std::string tagname){
  std::cout << std:: endl << "test: findGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std::endl << "test: findGlobalTag" << std::endl;
  // string name32 = "MVG_TEST";

  try {
     nlohmann::json tag_info = myCrestClient.findGlobalTag(tagname);
     std::cout << std:: endl << "test: findGlobalTag (result) = " << std::endl
       << tag_info.dump(4) << std::endl;
  }
     catch(const std::runtime_error& e){
     std::cout << std:: endl << "test: findGlobalTag (failed)" << std::endl;
  }
}

void testListGlobalTags(){
  std::cout << std:: endl << "test: listGlobalTags" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: listGlobalTags" << std::endl;

  try{
     nlohmann::json tag_info9 = myCrestClient.listGlobalTags();
     std::cout << std::endl << "test: listGlobalTags (result) = "
               << tag_info9.dump(4) << std::endl;
  }
     catch (const std::runtime_error& e){
      std::cout << std:: endl << "test: listGlobalTags (failed)" << std::endl;
  }
}


void testListGlobalTagsAsString(){
  std::cout << std:: endl << "test: listGlobalTagsAsString" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: listGlobalTagsAsString" << std::endl;

  try{
     nlohmann::json tag_info9 = myCrestClient.listGlobalTagsAsString();
     std::cout << std:: endl << "test: listGlobalTagsAsString (result) = "
               << tag_info9.dump(4) << std::endl;
  }
     catch (const std::runtime_error& e){
      std::cout << std:: endl << "test: listGlobalTagsAsString (failed)" << std::endl;
  }
}


void testRemoveGlobalTag(std::string tagname){
  std::cout << std:: endl << "test: removeGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: removeGlobalTag" << std::endl;
  // std::string tagName6 = "MvG_TEST";

  try{
     myCrestClient.removeGlobalTag(tagname);
     std::cout << std:: endl << "test: removeGlobalTag (success)" << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: removeGlobalTag (failed)" << std::endl;
  }
}


void testUpdateGlobalTag(std::string tagname){
  std::cout << std:: endl << "test: updateGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: updateGlobalTag" << std::endl;

  // std::string name = "MvG_TEST_01";
  nlohmann::json js =
   {
     {"name",tagname},
     {"validity",0},
     {"description","test"},
     {"release","1"},
     {"insertionTime","2018-12-18T11:32:58.081+0000"},
     {"snapshotTime","2018-12-18T11:32:57.952+0000"},
     {"scenario","test2"},
     {"workflow","M"},
     {"type","t"},
     {"snapshotTimeMilli","null"},
     {"insertionTimeMilli","null"}
   };

  try {
     myCrestClient.updateGlobalTag(tagname, js);
     std::cout << std:: endl << "test: updateGlobalTag (success) " << std::endl;
  }
     catch(const std::runtime_error& e){
     std::cout << std:: endl << "test: updateGlobalTag (failed)" << std::endl;
  }
}


void testFindGlobalTagAsString(std::string tagname){
  std::cout << std:: endl << "test: findGlobalTagAsString" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: findGlobalTagAsString" << std::endl;
  // string name31 = "MVG_TEST";

  try {
     nlohmann::json tag_info = myCrestClient.findGlobalTagAsString(tagname);
     std::cout << std:: endl << "test: findGlobalTagAsString (result) = " << std::endl
       << tag_info.dump(4) << std::endl;
  }
  catch(const std::runtime_error& e){
     std::cout << std:: endl << "test: findGlobalTagAsString (failed)" << std::endl;
  }
}

//===================================================
// RunLumiInfo Method Tests
//===================================================

void testCreateRunLumiInfo(){
  std::cout << std:: endl << "test: createRunLumiInfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: createRunLumiInfo" << std::endl;
  std::string str =
  "{\"since\":\"10\",\"run\":\"7777771\",\"lb\":\"62\",\"starttime\":\"10\",\"endtime\":\"200\"}";
  nlohmann::json js = myCrestClient.getJson(str);

  try{
     myCrestClient.createRunLumiInfo(js);
     std::cout << std:: endl << "test: createRunLumiInfo (success) = " << std::endl;
  }
     catch (const std::runtime_error& e){
      std::cout << std:: endl << "test: createRunLumiInfo (failed)" << std::endl;
   }
}


void testListRunLumiInfo(){
  std::cout << std:: endl << "test: listRunLumiInfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: listRunLumiInfo" << std::endl;
  nlohmann::json info16 = myCrestClient.listRunLumiInfo();
  std::cout << std:: endl << "test: listRunLumiInfo (result)" << std::endl;
  std::cout << info16.dump(4) << std::endl;
  std::cout << std:: endl << "test: listRunLumiInfo" << std::endl;
}

//===================================================
// Global Tag Method Tests
//===================================================

void testFindGlobalTagMap(std::string mapname){
  std::cout << std:: endl << "test: findGlobalTagMap" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: findGlobalTagMap" << std::endl;
  // string name30 = "MVG_TEST";

  try{
     nlohmann::json tag_info = myCrestClient.findGlobalTagMap(mapname);
     std::cout << std:: endl << "test: findGlobalTagMap (result) = "
               << tag_info.dump(4) << std::endl;
  }
  catch (const std::runtime_error& e){
      std::cout << std:: endl << "test: findGlobalTagMap (failed)" << std::endl;
  }
}

// global tag and tag have to exist
void testCreateGlobalTagMap(std::string globaltag, std::string tagname){
  std::cout << std:: endl << "test: createGlobalTagMap" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: createGlobalTagMap" << std::endl;

  nlohmann::json js =
   {
     {"globalTagName", globaltag},
     {"record", "testing2"},
     {"label", "test2"},
     {"tagName", tagname}
   };

  try{
     myCrestClient.createGlobalTagMap(js);
     std::cout << std:: endl << "test: createGlobalTagMap (success) " << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: find_createGlobalTagMap (failed)" << std::endl;
  }
}

//===================================================
// Auxillary Method Tests
//===================================================

void testGetJson(){
  std::cout << std:: endl << "test: getJson" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << "getJson test" << std::endl;
  std::string stra =
  "{\"tagName\":\"IndetBeampos-nominal\",\"since\":0,\"insertionTime\":\"2019-02-21T13:52:06.222+0000\",\"payloadHash\":\"de9afd9a5bb526d175265d1e66520ac1e7a81816cc3081925625759583e9107f\"}";
  std::string strb = "test";
  std::cout << myCrestClient.getJson(stra).dump(4) << std::endl;
  std::cout << myCrestClient.getJson(strb).dump(4) << std::endl;
  std::cout << "getJson test" << std::endl;
}


//===================================================
// Tag + Payloads Method Tests
//===================================================

// Be carrful tag has to be creted before this test!
void testStorePayload(std::string tagname){
  std::cout << std:: endl << "test: createTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  // payload + tag name (command line example):
  // curl --form file=@./test.txt --form tag="SB_TAG-PYLD" --form since=0
  // http://mvg-test-pc-03.cern.ch:8090/crestapi/payloads/store

  std::cout << std:: endl << "test: storePayload" << std::endl;

  // std::string tagname = "test_MvG3a";
  uint64_t since = 0;
  std::string payload = "aaa";


  try{
     myCrestClient.storePayload(tagname, since, payload);
     std::cout << std:: endl << "test: storePayload (success)" << std::endl;
  }
  catch (const std::runtime_error& e){
      std::cout << std:: endl << "test: storePayload (failed)" << std::endl;
  }
}

// This test for the local file storage method storePayloadDump.

void testStorePayloadDump(std::string tagname){
  std::cout << std:: endl << "test: createTag" << std::endl;
  bool rewrite = true;
  std::string path = "/tmp/cresttest/crest_dump";
  std::filesystem::create_directories(path);
  CrestClient myCrestClient = CrestClient(rewrite, path);

  std::cout << std:: endl << "test: storePayloadDump" << std::endl;

  uint64_t since = 200;


  std::string str =  "{\"niovs\": 2,\"format\":\"PYLD_JSON\",\"iovsList\":[{\"since\":800,\"payload\":\"vv1\"},{\"since\":900,\"payload\":\"ww1\"}]}";

  nlohmann::json js = myCrestClient.getJson(str);

  try {
     myCrestClient.storePayloadDump(tagname, since, str);
     std::cout << std:: endl << "test: storePayloadDump (success) " << std::endl;
  }
  catch(const std::runtime_error& e){
     std::cout << std:: endl << "test: storePayloadDump (failed)" << std::endl;
  }
}

void testStoreBatchPayloads(std::string tagname){
  std::cout << std:: endl << "test: storeBatchPayloads" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: storeBatchPayloads" << std::endl;

  uint64_t endtime = 200;
  std::string str = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
  nlohmann::json js = myCrestClient.getJson(str);

  try {
      myCrestClient.storeBatchPayloads(tagname, endtime, js);
      std::cout << std:: endl << "test: storeBatchPayloads (success) " << std::endl;
  }
  catch(const std::runtime_error& e){
      std::cout << std:: endl << "test: storeBatchPayloads (failed)" << std::endl;
  }
}



//===================================================
// IOV Method Tests
//===================================================

void testFindAllIovs(std::string tagname){
  std::cout << std:: endl << "test: findAllIovs" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: findAllIovs" << std::endl;

  //std::string tagname = "test_MvG4";
  try{
    nlohmann::json tag_info = myCrestClient.findAllIovs(tagname);
    std::cout << std:: endl << "test: findAllIovs (result)" << std::endl;
    std::cout << tag_info.dump(4) << std::endl;
    std::cout << std:: endl << "test: findAllIovs" << std::endl;
  }
  catch (const std::runtime_error& e) {
    std::cout << std:: endl << "test: findAllIovs (failed)" << std::endl;
  }
}

//===================================================
// Tag Meta Info Method Tests
//===================================================

void testCreateTagMetainfo(std::string tagname){
  std::cout << std:: endl << "test: createTagMetainfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: createTagMetainfo" << std::endl;   

  //std::string tagname = "test_MvG4";
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
     myCrestClient.createTagMetaInfo(tagname, js);
     std::cout << std:: endl << "test: createTagMetaInfo (success)" << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: createTagMetaInfo (failed)" << std::endl;
  }
}


void testGetTagMetaInfo(std::string tagname){
  std::cout << std:: endl << "test: getTagMetaInfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: getTagMetainfo" << std::endl;   

  // std::string tagname61 = "test_MvG3";

  try{
     nlohmann::json js =  myCrestClient.getTagMetaInfo(tagname);
     // std::cout << std:: endl << "AAA" << js << "BBB" << std::endl;
     std::cout << std:: endl << "test: getTagMetaInfo (result)" << std::endl
	       << js.dump(4) << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: getTagMetaInfo (failed)" << std::endl;
  }    
}


void testUpdateTagMetainfo(std::string tagname){
  std::cout << std:: endl << "test: createTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: updateTagMetainfo" << std::endl;   

  // std::string tagname = "test_MvG3";
  nlohmann::json js = 
  {
      // {"tagName", tagname},
      {"description", "desc-02"},
      // {"tagInfo", "taginf-01"},
      // {"chansize", 0},
      // {"colsize", 0},
      // {"insertionTime", "2019-03-14T13:29:25.286Z"}
  };

  try{
     myCrestClient.updateTagMetaInfo(tagname, js);
     std::cout << std:: endl << "test: updateTagMetaInfo (success)" << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: updateTagMetaInfo (failed)" << std::endl;
  }
}


//===================================================
//  Tag Meta Info Method Tests in IOVDbSvc format
//===================================================

void testCreateTagMetainfoIOVDbSvc(std::string tagname){
  std::cout << std:: endl << "test: createTagMetainfoIOVDbSvc" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: createTagMetainfoIOVDbSvc" << std::endl;   

  // std::string tagname = "test_MvG2";
  std::string str = "[{\"node_description\":\" string of the folder description \"},{\"channel_list\":[0,10,20]},{\"channel_names\": [\"first\",\"second\",\"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";

  nlohmann::json js =  myCrestClient.getJson(str);

  try{
     myCrestClient.createTagMetaInfoIOVDbSvc(tagname, js);
     std::cout << std:: endl << "test: createTagMetaInfoIOVDbSvc (success)" << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: createTagMetaInfoIOVDbSvc (failed)" << std::endl;
  }
}


void testGetTagMetaInfoIOVDbSvc(std::string tagname){
  std::cout << std:: endl << "test: getTagMetaInfoIOVDbSvc" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: getTagMetaInfoIOVDbSvc" << std::endl;   

  // std::string tagname = "test_MvG3";

  try{
     nlohmann::json js68 =  myCrestClient.getTagMetaInfoIOVDbSvc(tagname);
     std::cout << std:: endl << "test: getTagMetaInfo (result)" << std::endl
		 << js68.dump(4) << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: getTagMetaInfo (failed)" << std::endl;
  }
}


void testConvertTagMetaInfo2CREST(){
  std::cout << std:: endl << "test: convertTagMetaInfo2CREST" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std:: endl << "test: convertTagMetaInfo2CREST" << std::endl;   

  std::string tagname = "test_MvG4";

  std::string str = "[{\"node_description\":\" string of the folder description \"},{\"channel_list\":[0,10,20]},{\"channel_names\": [\"first\",\"second\",\"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";

  nlohmann::json js = myCrestClient.getJson(str);

  try{
     nlohmann::json res = myCrestClient.convertTagMetaInfo2CREST(js);
     std::cout << std:: endl << "test: convertTagMetaInfo2CREST (success)" 
                  << "result = " << res.dump(4) << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: convertTagMetaInfo2CREST (failed)" << std::endl;
  }
}

//----------------------

void testTagListParams(){
  std::cout << std:: endl << "test: tagListParams" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try{

    //myCrestClient.listTagsParams();
    
    // nlohmann::json res = myCrestClient.listTagsParams(100,200);
    nlohmann::json res = myCrestClient.listTagsParams(_page=3,_size=5);
    
    //
    std::cout << std:: endl << "test: tagListParams (success)" 
                  << "result = " << res.dump(4) << std::endl;
    //
  }
  catch (const std::runtime_error& e){
     std::cout << std:: endl << "test: tagListParams (failed)" << std::endl;
  }
}


//----------------------

// ---------------------

int main(int argc, char* argv[])
{
    if (argc == 2){
      // print_path();
      // cout << "argv[1] = "<< argv[1] << endl;
      SURL = argv[1];
      print_path();

      testCreateTag("test_MvG3b");
      // testListTags();
      // testFindTag("test_MvG3b");
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
      // testFindAllIovs("test_MvG3b");
      // iovs tests (end)

      testCreateTagMetainfo("test_MvG3b");
      // testUpdateTagMetainfo("test_MvG3b");
      testGetTagMetaInfo("test_MvG3b");


      // testCreateTagMetainfoIOVDbSvc("test_MvG3b"); // it works on crest-01.cern.ch:8080
      // testGetTagMetaInfoIOVDbSvc("test_MvG3b"); // problem on crest-01.cern.ch:8080
      // testConvertTagMetaInfo2CREST();

      //  tmp instructions
      // testCreateTag("test_MvG3c");
      // testCreateTagMetainfoIOVDbSvc("test_MvG3c"); // it works on crest-01.cern.ch:8080
      testGetTagMetaInfoIOVDbSvc("test_MvG3c"); // problem on crest-01.cern.ch:8080  // uncomment
      // testGetTagMetaInfo("test_MvG3c");
      //  tmp instructions

      // new 
      // testTagListParams();
      // testListTags(); // added for test

    } 
    else {
        cout << "CREST Server path not found" << endl;
        cout << "Please, run this program with a server path:" << endl;
        cout << "crest_example2 http://mvg-pc-04.cern.ch:8090" << endl;
    }
    // system("pause");
    cout << "Test ended" << endl;
    return 0;
}
