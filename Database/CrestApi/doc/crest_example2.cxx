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

using namespace Crest;

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
        ret_val = std::filesystem::create_directory(full_path.substr(0, pos));
    }

    return ret_val;
}


//===================================================
// Tag Method Tests
//===================================================

void testCreateTag(std::string tagname){
  std::cout << std::endl << "test: createTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

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
  catch (const std::exception& e){
     std::cout << std::endl << "test: createTag (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }

}


void testCreateTag2(){
  std::cout << std::endl << "test: createTag2" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  // real tag creation example:
  // curl --header "Content-Type: application/json" --request POST --data '{"name":"CaloSwClusterCorrections.00-RUN2-02-11-clcon-dummy","description":"<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"250904980\" /></addrHeader><typeName>CaloRec::ToolConstants</typeName><fullrep/>","timeType":"run-lumi","payloadSpec":"PoolRef:String4k"}' http://crest-01.cern.ch:8090/crestapi/tags

  nlohmann::json js =
  {
    {"name","CaloSwClusterCorrections.00-RUN2-02-11-clcon-dummy"},
    {"description","<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"250904980\" /></addrHeader><typeName>CaloRec::ToolConstants</typeName><fullrep/>"},
    {"timeType","run-lumi"},
    {"payloadSpec","PoolRef:String4k"}
  };

  try{
    myCrestClient.createTag(js);
    std::cout << std::endl << "test: createTag2 (success)" << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: createTag2 (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }

}

// Method to create a tag on the local storage (on disk).

void testCreateTagF(std::string tagname){
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
  catch (const std::exception& e){
     std::cout << std::endl << "test: createTagF (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

void testListTags(){
  std::cout << std::endl << "test: listTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std::endl << "test: listTags" << std::endl;

  try {
     nlohmann::json tag_list = myCrestClient.listTags();
     std::cout << std::endl << "test: listTags (result)" << std::endl
       << tag_list.dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: listTags (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }

}


void testFindTag(std::string tagname){
  std::cout << std::endl << "test: findTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std::endl << "test: findTag" << std::endl;

  try {
     nlohmann::json tag_info = myCrestClient.findTag(tagname);
     std::cout << std::endl << "test: findTag (result)" << std::endl
       << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: findTag (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}


void testRemoveTag(std::string tagname){
  std::cout << std::endl << "test: removeTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try {
     myCrestClient.removeTag(tagname);
     std::cout << std::endl << "test: removeTag (success)" << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: removeTag (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

void testUpdateTag(std::string tagname){
  std::cout << std::endl << "test: updateTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  nlohmann::json js = {{"objectType","json3"}};

  try{
     myCrestClient.updateTag(tagname, js);
     std::cout << std::endl << "test: updateTag (success)" << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: updateTag (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

void testUpdateTagSpecification(std::string tagname){
  std::cout << std::endl << "test: updateTagSpecification" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::string newObjectType = "json4";

  try{
     myCrestClient.updateTagSpecification(tagname, newObjectType);
     std::cout << std::endl << "test: updateTagSpecification (success)" << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: updateTagSpecification (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

//===================================================
// Global Tag Method Tests
//===================================================

void testCreateGlobalTag(std::string tagname){
  std::cout << std::endl << "test: createGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

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
  catch (const std::exception& e){
     std::cout << std::endl << "test: createGlobalTag (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }

}


void testFindGlobalTag(std::string tagname){
  std::cout << std::endl << "test: findGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try {
     nlohmann::json tag_info = myCrestClient.findGlobalTag(tagname);
     std::cout << std::endl << "test: findGlobalTag (result) = " << std::endl
       << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: findGlobalTag (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

void testListGlobalTags(){
  std::cout << std::endl << "test: listGlobalTags" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try{
     nlohmann::json tag_info9 = myCrestClient.listGlobalTags();
     std::cout << std::endl << "test: listGlobalTags (result) = "
               << tag_info9.dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: listGlobalTags (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}


void testListGlobalTagsAsString(){
  std::cout << std::endl << "test: listGlobalTagsAsString" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try{
     nlohmann::json tag_info9 = myCrestClient.listGlobalTagsAsString();
     std::cout << std::endl << "test: listGlobalTagsAsString (result) = "
               << tag_info9.dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: listGlobalTagsAsString (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}


void testRemoveGlobalTag(std::string tagname){
  std::cout << std::endl << "test: removeGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std::endl << "test: removeGlobalTag" << std::endl;

  try{
     myCrestClient.removeGlobalTag(tagname);
     std::cout << std::endl << "test: removeGlobalTag (success)" << std::endl;
  }
  catch (const std::runtime_error& e){
     std::cout << std::endl << "test: removeGlobalTag (failed)" << std::endl;
  }
}


void testUpdateGlobalTag(std::string tagname){
  std::cout << std::endl << "test: updateGlobalTag" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

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
     std::cout << std::endl << "test: updateGlobalTag (success) " << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: updateGlobalTag (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}


void testFindGlobalTagAsString(std::string tagname){
  std::cout << std::endl << "test: findGlobalTagAsString" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try {
     nlohmann::json tag_info = myCrestClient.findGlobalTagAsString(tagname);
     std::cout << std::endl << "test: findGlobalTagAsString (result) = " << std::endl
       << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: findGlobalTagAsString (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

//===================================================
// RunLumiInfo Method Tests
//===================================================

void testCreateRunLumiInfo(){
  std::cout << std::endl << "test: createRunLumiInfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::string str =
  "{\"since\":\"10\",\"run\":\"7777771\",\"lb\":\"62\",\"starttime\":\"10\",\"endtime\":\"200\"}";
  nlohmann::json js = myCrestClient.getJson(str);

  try{
     myCrestClient.createRunLumiInfo(js);
     std::cout << std::endl << "test: createRunLumiInfo (success) = " << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: createRunLumiInfo (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}


void testListRunLumiInfo(){
  std::cout << std::endl << "test: listRunLumiInfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << std::endl << "test: listRunLumiInfo" << std::endl;

  try{
     nlohmann::json info16 = myCrestClient.listRunLumiInfo();
     std::cout << std::endl << "test: listRunLumiInfo (success) = " << std::endl;
     std::cout << info16.dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: listRunLumiInfo (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

//===================================================
// Global Tag Method Tests
//===================================================

void testFindGlobalTagMap(std::string mapname){
  std::cout << std::endl << "test: findGlobalTagMap" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try{
     nlohmann::json tag_info = myCrestClient.findGlobalTagMap(mapname);
     std::cout << std::endl << "test: findGlobalTagMap (result) = "
               << tag_info.dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: findGlobalTagMap (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

// global tag and tag have to exist
void testCreateGlobalTagMap(std::string globaltag, std::string tagname){
  std::cout << std::endl << "test: createGlobalTagMap" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

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
  catch (const std::exception& e){
     std::cout << std::endl << "test: createGlobalTagMap (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

//===================================================
// Auxillary Method Tests
//===================================================

void testGetJson(){
  std::cout << std::endl << "test: getJson" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  std::cout << "getJson test" << std::endl;
  std::string stra =
  "{\"tagName\":\"IndetBeampos-nominal\",\"since\":0,\"insertionTime\":\"2019-02-21T13:52:06.222+0000\",\"payloadHash\":\"de9afd9a5bb526d175265d1e66520ac1e7a81816cc3081925625759583e9107f\"}";
  std::string strb = "test";

  try{
      std::cout << myCrestClient.getJson(stra).dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: getJson for string stra (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }

  try{
      std::cout << myCrestClient.getJson(strb).dump(4) << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: getJson for string strb (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }

  std::cout << "getJson test ended" << std::endl;
}


//===================================================
// Tag + Payloads Method Tests
//===================================================

// Be carrful tag has to be creted before this test!
void testStorePayload(std::string tagname){
  std::cout << std::endl << "test: storePayload" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  // payload + tag name (command line example):
  // curl --form file=@./test.txt --form tag="SB_TAG-PYLD" --form since=0
  // http://mvg-test-pc-03.cern.ch:8090/crestapi/payloads/store

  uint64_t since = 0;
  std::string payload = "aaa";


  try{
     myCrestClient.storePayload(tagname, since, payload);
     std::cout << std::endl << "test: storePayload (success)" << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: storePayload (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

// This test for the local file storage method storePayloadDump.

void testStorePayloadDump(std::string tagname){
  std::cout << std::endl << "test: storePayloadDump" << std::endl;
  bool rewrite = true;
  std::string path = "/tmp/cresttest/crest_dump";
  std::filesystem::create_directories(path);
  CrestClient myCrestClient = CrestClient(rewrite, path);

  uint64_t since = 200;


  std::string str =  "{\"niovs\": 2,\"format\":\"PYLD_JSON\",\"iovsList\":[{\"since\":800,\"payload\":\"vv1\"},{\"since\":900,\"payload\":\"ww1\"}]}";

  nlohmann::json js = myCrestClient.getJson(str);

  try {
     myCrestClient.storePayloadDump(tagname, since, str);
     std::cout << std::endl << "test: storePayloadDump (success) " << std::endl;
  }
  catch (const std::exception& e){
     std::cout << std::endl << "test: storePayloadDump (failed)" << std::endl;
     std::cout << e.what() << std::endl;
  }
}

void testStoreBatchPayloads(std::string tagname){
  std::cout << std::endl << "test: storeBatchPayloads" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  uint64_t endtime = 200;
  std::string str = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
  nlohmann::json js = myCrestClient.getJson(str);

  try {
      myCrestClient.storeBatchPayloads(tagname, endtime, js);
      std::cout << std::endl << "test: storeBatchPayloads (success) " << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: storeBatchPayloads (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}



//===================================================
// IOV Method Tests
//===================================================

void testFindAllIovs(std::string tagname){
  std::cout << std::endl << "test: findAllIovs" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try{
    nlohmann::json tag_info = myCrestClient.findAllIovs(tagname);
    std::cout << std::endl << "test: findAllIovs (result)" << std::endl;
    std::cout << tag_info.dump(4) << std::endl;
    std::cout << std::endl << "test: findAllIovs" << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: findAllIovs (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}

// new

void testFindAllIovsParams(std::string tagname, int size, int page){
  std::cout << std::endl << "test: findAllIovs with additional parameters" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try{
    nlohmann::json iov_list_1 = myCrestClient.findAllIovsParams(tagname, size, page);
    nlohmann::json iov_list_2 = myCrestClient.findAllIovsParams(tagname, _page=size,_size=page);

    std::cout << std::endl << "test: findAllIovs (result A)" << std::endl;
    std::cout << iov_list_1.dump(4) << std::endl;

    std::cout << std::endl << "test: findAllIovs (result B)" << std::endl;
    std::cout << iov_list_2.dump(4) << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: findAllIovs with additional parameters (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}

//===================================================
// Tag Meta Info Method Tests
//===================================================

void testCreateTagMetaInfo(std::string tagname){
  std::cout << std::endl << "test: createTagMetainfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL); 

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
     std::cout << std::endl << "test: createTagMetaInfo (success)" << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: createTagMetaInfo (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}

// analogue 
void testCreateTagMetaInfo2(std::string tagname){
  std::cout << std::endl << "test: createTagMetainfo2" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL); 

  nlohmann::json js = 
  {
      {"chansize",1},
      {"colsize",0},
      {"description","<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>"},
      {"insertionTime","Fri Nov 13 16:31:10 2020"},
      {"tagName","OflPrefLumi-RUN2-UPD4-10"}
  };

  try{
     myCrestClient.createTagMetaInfo(tagname, js);
     std::cout << std::endl << "test: createTagMetaInfo (success)" << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: createTagMetaInfo (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}


void testGetTagMetaInfo(std::string tagname){
  std::cout << std::endl << "test: getTagMetaInfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try{
     nlohmann::json js =  myCrestClient.getTagMetaInfo(tagname);
     std::cout << std::endl << "test: getTagMetaInfo (result)" << std::endl
	       << js.dump(4) << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: getTagMetaInfo (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }    
}


void testUpdateTagMetainfo(std::string tagname){
  std::cout << std::endl << "test: updateTagMetainfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);  

  nlohmann::json js = 
  {
      {"description", "desc-02"},
  };

  try{
     myCrestClient.updateTagMetaInfo(tagname, js);
     std::cout << std::endl << "test: updateTagMetaInfo (success)" << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: updateTagMetainfo (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}


//===================================================
//  Tag Meta Info Method Tests in IOVDbSvc format
//===================================================

void testCreateTagMetaInfoIOVDbSvc(std::string tagname){
  std::cout << std::endl << "test: createTagMetainfoIOVDbSvc" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);  

  std::string str = "[{\"node_description\":\" string of the folder description \"},{\"channel_list\":[0,10,20]},{\"channel_names\": [\"first\",\"second\",\"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";

  nlohmann::json js =  myCrestClient.getJson(str);

  try{
     myCrestClient.createTagMetaInfoIOVDbSvc(tagname, js);
     std::cout << std::endl << "test: createTagMetaInfoIOVDbSvc (success)" << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: createTagMetaInfoIOVDbSvc (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}


void testGetTagMetaInfoIOVDbSvc(std::string tagname){
  std::cout << std::endl << "test: getTagMetaInfoIOVDbSvc" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);  

  try{
     nlohmann::json js68 =  myCrestClient.getTagMetaInfoIOVDbSvc(tagname);
     std::cout << std::endl << "test: getTagMetaInfoIOVDbSvc (result) = " << std::endl
		 << js68.dump(4) << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: getTagMetaInfoIOVDbSvc (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}


void testConvertTagMetaInfo2CREST(){
  std::cout << std::endl << "test: convertTagMetaInfo2CREST" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL); 

  std::string tagname = "test_MvG4";

  std::string str = "[{\"node_description\":\" string of the folder description \"},{\"channel_list\":[0,10,20]},{\"channel_names\": [\"first\",\"second\",\"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";

  nlohmann::json js = myCrestClient.getJson(str);

  try{
     nlohmann::json res = myCrestClient.convertTagMetaInfo2CREST(js);
     std::cout << std::endl << "test: convertTagMetaInfo2CREST (success) = " 
                  << "result = " << res.dump(4) << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: convertTagMetaInfo2CREST (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}

//----------------------

void testListTagsParams(){
  std::cout << std::endl << "test: listTagsParams" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try{

    // variants how to call this method:
    // myCrestClient.listTagsParams();
    // nlohmann::json res = myCrestClient.listTagsParams(100,200);

    nlohmann::json res = myCrestClient.listTagsParams(_page=3,_size=5);
    
    std::cout << std::endl << "test: listTagsParams (success)" 
                  << "result = " << res.dump(4) << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: listTagsParams (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}

//===================================================
//  Monitoring Method Tests in IOVDbSvc format
//================================================

void testListPayloadTagInfo(){

  std::cout << std::endl << "test: listPayloadTagInfo" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try {
     nlohmann::json tag_list = myCrestClient.listPayloadTagInfo();
     std::cout << std::endl << "test: listPayloadTagInfo (result) =" << std::endl
       << tag_list.dump(4) << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: listPayloadTagInfo (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}

void testListPayloadTagInfo(std::string tagname){

  std::cout << std::endl << "test: listPayloadTagInfo with tag " << tagname << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);

  try {
     nlohmann::json tag_list = myCrestClient.listPayloadTagInfo(tagname);
     std::cout << std::endl << "test: listPayloadTagInfo (result)" << std::endl
       << tag_list.dump(4) << std::endl;
  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: listPayloadTagInfo (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}

//================================================
//  Blob Method Tests
//================================================


void testGetBlobInStream(std::string hash){

  std::cout << std::endl << "test: getBlobInStream" << std::endl;
  CrestClient myCrestClient = CrestClient((std::string)SURL);  

  try{

    remove("data.txt");

    std::ofstream out8;          // stream for writing
    out8.open("data.txt"); // open the file to write
    if (out8.is_open())
    {

          string tag_info8 = myCrestClient.getBlobInStream(hash, out8);
          std::cout << std::endl << "test: getBlobInStream (result)" << std::endl;
          std::cout << tag_info8 << std::endl;
          std::cout << std::endl << "test: getBlobInStream" << std::endl;
    }

  }
  catch (const std::exception& e){
      std::cout << std::endl << "test: getBlobInStream (failed)" << std::endl;
      std::cerr << e.what() << std::endl;
  }
}

//----------------------


int main(int argc, char* argv[])
{
    if (argc == 2){
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
    } 
    else {
        cout << "CREST Server path not found" << endl;
        cout << "Please, run this program with a server path:" << endl;
        cout << "crest_example2 http://mvg-pc-04.cern.ch:8090" << endl;
    }
    cout << "Test ended" << endl;
    return 0;
}
