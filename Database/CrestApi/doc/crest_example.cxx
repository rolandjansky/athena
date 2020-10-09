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

int main()
{


    //==================================================
    // SETUP FOR mvg-test-pc-03 SERVER
    //
    std::string url32 = "http://mvg-test-pc-03.cern.ch:8090";
    CrestClient myCrestClient = CrestClient(url32); // CrestClient();
    //

    //==================================================
    // SETUP FOR crest-undertow SERVER
    //
    std::string url16 = "http://crest-undertow.web.cern.ch";
    CrestClient myCrestClientF = CrestClient(url16); // CrestClient();
    //
    
    //==================================================
    // SETUP FOR mvg-pc-04 SERVER
    //
    std::string url35 = "http://mvg-pc-04.cern.ch:8090";
    CrestClient myCrestClientM = CrestClient(url35); // CrestClient();
    //

    //==================================================
    // SETUP FOR crest-01 SERVER
    //
    std::string url61 = "http://crest-01.cern.ch:8080";
    CrestClient myCrestClientC = CrestClient(url61); // CrestClient();
    //

    //==================================================
    // GET DATA from file
    try{
    std::string path01 = myCrestClient.getDataPath()+"/layout.json";
    std::ifstream i(path01);  
    nlohmann::json j = nlohmann::json::parse(i);

    // parse and serialize JSON
    std::cout << std::setw(4) << j << "\n\n";
    
    std::cout << "test" << std::endl;
    }
    catch(...){
       std::cout << std:: endl << "test:: Error reading JSON data file. " << std::endl;
    }

    //


    //=============================================
    /*
    std::cout << std:: endl << "test: listTags" << std::endl;

    try {
       nlohmann::json tag_list = myCrestClient.listTags();
       std::cout << std:: endl << "test: listTags (result)" << std::endl
		 << tag_list.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: listTags (failed)" << std::endl;
    }

    */ 


    //==============================================
    /*
    std::cout << std:: endl << "test: removeTag" << std::endl;
    std::string tagName = "test_MvG7";

    try {
       myCrestClient.removeTag(tagName);
       std::cout << std:: endl << "test: removeTag (success)" << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: removeTag (failed)" << std::endl;
    }
    */


    //==============================================
    /*
    std::cout << std:: endl << "test: findTag" << std::endl;
    string name2 = "test_MvG3";

    try {
       nlohmann::json tag_info = myCrestClient.findTag(name2);
       std::cout << std:: endl << "test: findTag (result)" << std::endl
		 << tag_info.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: findTag (failed)" << std::endl;
    }
    */


    //==============================================
    /*
    std::cout << std:: endl << "test: createTag" << std::endl;
    

    nlohmann::json js2 = 
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
        myCrestClient.createTag(js2);
        std::cout << std:: endl << "test: createTag (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createTag (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: createIov" << std::endl;

    nlohmann::json js3 = 
    {
      {"tagName","test_MvG3"},
      {"since",100},
      {"insertionTime","2018-12-18T11:32:58.329+0000"},
      {"payloadHash","ABRACADABRA2"},
    };

    try {
        myCrestClient.createIov(js3);
        std::cout << std:: endl << "test: createIov (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createIov (failed)" << std::endl;
    }
  
    */


    //================================================
    //
    std::cout << std:: endl << "test: createGlobalTag" << std::endl;
    nlohmann::json js4 = 
    {
       {"name","MvG_TEST_01"},
       {"validity",0},
       {"description","test"},
       {"release","1"},
       {"insertionTime","2018-12-18T11:32:58.081+0000"},
       {"snapshotTime","2018-12-18T11:32:57.952+0000"},
       {"scenario","test"},
       {"workflow","M"},
       {"type","t"},
       {"snapshotTimeMilli","null"},
       {"insertionTimeMilli","null"}
    };

    try {
       myCrestClient.createGlobalTag(js4);
       std::cout << std:: endl << "test: createGlobalTag (success) " << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: createGlobalTag (failed)" << std::endl;
    }
    //

    //==============================================
    /*
    std::cout << std::endl << "test: findGlobalTag" << std::endl;
    string name32 = "MVG_TEST";

    try {
       nlohmann::json tag_info32 = myCrestClient.findGlobalTag(name32);
       std::cout << std:: endl << "test: findGlobalTag (result) = " << std::endl
		 << tag_info32.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: findGlobalTag (failed)" << std::endl;
    }
    */



    //==============================================
    /*
    std::cout << std:: endl << "test: removeGlobalTag" << std::endl;
    std::string tagName6 = "MvG_TEST";

    try{
       myCrestClient.removeGlobalTag(tagName6);
       std::cout << std:: endl << "test: removeGlobalTag (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: removeGlobalTag (failed)" << std::endl;
    }
    */


    //===============================================
    /*
    std::cout << std:: endl << "test: getBlob" << std::endl;
    string hash7 = "528c87b4f00af72d6fc2405c108dd132bb525efab29b12ee1bb130e81ac70e12";
    string tag_info7 = myCrestClient.getBlob(hash7);
    std::cout << std:: endl << "test: getBlob (result)" << std::endl;
    std::cout << tag_info7 << std::endl;
    std::cout << std:: endl << "test: getBlob" << std::endl;
    */

    //==============================================
    /*
    remove("data.txt");

    std::ofstream out8;          // stream for writing
    out8.open("data.txt"); // open the file to write
    if (out8.is_open())
    {

          string hash8 = "528c87b4f00af72d6fc2405c108dd132bb525efab29b12ee1bb130e81ac70e12";
          string tag_info8 = myCrestClient.getBlobInStream(hash7, out8);
          std::cout << std:: endl << "test: getBlob (result)" << std::endl;
          std::cout << tag_info8 << std::endl;
          std::cout << std:: endl << "test: getBlob" << std::endl;
    }

    */

    //=============================================
    /*
    std::cout << "getJson test" << std::endl;
    std::string str9a = "{\"tagName\":\"IndetBeampos-nominal\",\"since\":0,\"insertionTime\":\"2019-02-21T13:52:06.222+0000\",\"payloadHash\":\"de9afd9a5bb526d175265d1e66520ac1e7a81816cc3081925625759583e9107f\"}";
    std::string str9b = "test";
    std::cout << myCrestClient.getJson(str9a).dump(4) << std::endl;
    std::cout << myCrestClient.getJson(str9b).dump(4) << std::endl;
    std::cout << "getJson test" << std::endl;
    */

    //============================================
    /*
    urlParameters uP;
    std::cout << "urlParameters test" << std::endl;
  
    std::cout << "params = " << uP.getParams() << std::endl;
    uP.add("key1","value1");
    uP.add("key2","value2");
    std::cout << "params = " << uP.getParams() << std::endl;
    uP.reset();
    std::cout << "params = " << uP.getParams() << std::endl;
    std::cout << "urlParameters test" << std::endl;  
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: listGlobalTags" << std::endl;

    try{
       nlohmann::json tag_info9 = myCrestClient.listGlobalTags();
       std::cout << std::endl << "test: listGlobalTags (result) = "  
                 << tag_info9.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: listGlobalTags (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: listGlobalTagsAsString" << std::endl;

    try{
       nlohmann::json tag_info9 = myCrestClient.listGlobalTagsAsString();
       std::cout << std:: endl << "test: listGlobalTagsAsString (result) = "  
                 << tag_info9.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: listGlobalTagsAsString (failed)" << std::endl;
    }
    */

    //===============================================
    //
    std::cout << std:: endl << "test: findAllIovs" << std::endl;;
    std::string tagname11 = "test_MvG4";
    try{
        nlohmann::json tag_info11 = myCrestClient.findAllIovs(tagname11);
        std::cout << std:: endl << "test: findAllIovs (result)" << std::endl;
        std::cout << tag_info11.dump(4) << std::endl;
        std::cout << std:: endl << "test: findAllIovs" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: findAllIovs (failed)" << std::endl;
    }
    //    
    
    //==============================================
    /*
    std::cout << std:: endl << "test: createTag" << std::endl;
    

    nlohmann::json js2 = 
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
        myCrestClient.createTag(js2);
        std::cout << std:: endl << "test: createTag (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createTag (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: getSizeByTag" << std::endl;
    std::string tagname12 = "CALOH1WeightsH1WeightsCone4Topo-CaloH1WeightsCone4Topo-RUN2-02-000";

    try{
       nlohmann::json tag_info12 = myCrestClient.getSizeByTag(tagname12);
       std::cout << std:: endl << "test: getSizeByTag (result) = "  
                 << tag_info12.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getSizeByTag (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: getSize" << std::endl;
    std::string tagname13 = "CALOH1WeightsH1WeightsCone4Topo-CaloH1WeightsCone4Topo-RUN2-02-000";
    try{
    int tag_info13 = myCrestClient.getSize(tagname13);
        std::cout << std:: endl << "test: getSize (result)" << std::endl;
        std::cout << tag_info13 << std::endl;
        std::cout << std:: endl << "test: getSize" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getSize (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: getPayloadMetaInfo" << std::endl;
    std::string hash14 = "528c87b4f00af72d6fc2405c108dd132bb525efab29b12ee1bb130e81ac70e12";
    nlohmann::json info14 = myCrestClient.getPayloadMetaInfo(hash14);
    std::cout << std:: endl << "test: getPayloadMetaInfo (result)" << std::endl;
    std::cout << info14.dump(4) << std::endl;
    std::cout << std:: endl << "test: getPayloadMetaInfo" << std::endl;
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: getPayloadMetaInfoAsString" << std::endl;
    std::string hash15 = "528c87b4f00af72d6fc2405c108dd132bb525efab29b12ee1bb130e81ac70e12";
    std::string info15 = myCrestClient.getPayloadMetaInfoAsString(hash15);
    std::cout << std:: endl << "test: getPayloadMetaInfoAsString (result)" << std::endl;
    std::cout << info15 << std::endl;
    std::cout << std:: endl << "test: getPayloadMetaInfoAsString" << std::endl;
    */


    //===============================================
    /* 
    std::cout << std:: endl << "test: listRunLumiInfo" << std::endl;
    nlohmann::json info16 = myCrestClientF.listRunLumiInfo();
    std::cout << std:: endl << "test: listRunLumiInfo (result)" << std::endl;
    std::cout << info16.dump(4) << std::endl;
    std::cout << std:: endl << "test: listRunLumiInfo" << std::endl;
    */

    //===============================================
    /* 
    std::cout << std:: endl << "test: listFolders" << std::endl;
    nlohmann::json info17 = myCrestClientF.listFolders();
    std::cout << std:: endl << "test: listFolders (result)" << std::endl;
    std::cout << info17.dump(4) << std::endl;
    std::cout << std:: endl << "test: listFolders" << std::endl;
    */


    //===============================================
    // This method DOES NOT WORK
    /*
    std::cout << std:: endl << "test: createFolder" << std::endl;
    nlohmann::json js18a = 
    {
       {"nodeFullpath", "/test"},
       {"schemaName", "test"},
       {"nodeName", "test"},
       {"nodeDescription", "test"},
       {"tagPattern", "test"},
       {"groupRole", "test"}
    };


    nlohmann::json js18 = 
    {
       {"nodeFullpath", "/test"},
       //{"schemaName", "test"},
       {"nodeName", "test"},
       //{"nodeDescription", "test"},
       //{"tagPattern", "test"},
       //{"groupRole", "test"}
    };

    std::string info18 = myCrestClientF.createFolder(js18);
    std::cout << std:: endl << "test: createFolder (result)" << std::endl;
    std::cout << info18 << std::endl;
    std::cout << std:: endl << "test: createFolder" << std::endl;
    */
    


    //===============================================
    /*
    std::cout << std:: endl << "test: createRunLumiInfo" << std::endl;
    std::string str19 = "{\"since\":\"10\",\"run\":\"7777771\",\"lb\":\"62\",\"starttime\":\"10\",\"endtime\":\"200\"}";
    nlohmann::json js19 = myCrestClientF.getJson(str19);

    try{
       myCrestClientF.createRunLumiInfo(js19);
       std::cout << std:: endl << "test: createRunLumiInfo (success) = " << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createRunLumiInfo (failed)" << std::endl;
    }
    */
    
    //===============================================
    /*

    try {
       std::string tagname20 = "CALOH1WeightsH1WeightsCone4Topo-CaloH1WeightsCone4Topo-RUN2-02-000";
       nlohmann::json tag_info20 = myCrestClient.selectIovs(tagname20);
       std::cout << std:: endl << "test: selectIovs (result) = " << std::endl
		 << tag_info20.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: selectIovs (failed)" << std::endl;
    }
    */

    //===============================================
    //TEST IT AGAIN
    //

    try{
       std::string tagname21 = "AFTestTag_01";
       nlohmann::json tag_info21 = myCrestClientF.selectGroups(tagname21);
       std::cout << std:: endl << "test:  selectGroups (result) = "  
                 << tag_info21.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: selectGroups (failed)" << std::endl;
    }
    //

    //===============================================
    // the same method as 21 but with additional snapshot parameter
    /*
    std::cout << std:: endl << "test: selectGroups" << std::endl;
    std::string tagname22 = "AFTestTag_01";
    nlohmann::json tag_info22 = myCrestClientF.selectGroups(tagname22,0);
    std::cout << std:: endl << "test: selectGroups (result)" << std::endl;
    std::cout << tag_info22.dump(4) << std::endl;
    std::cout << std:: endl << "test: selectGroups" << std::endl;
    */

    //===============================================
    /*

    try {";
       std::string tagname23 = "CALOH1WeightsH1WeightsCone4Topo-CaloH1WeightsCone4Topo-RUN2-02-000";
       nlohmann::json tag_info23 = myCrestClient.selectSnapshot(tagname23);
       std::cout << std:: endl << "test: selectSnapshot (result) = " << std::endl
		 << tag_info23.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: selectSnapshot (failed)" << std::endl;
    }
    */

    //===============================================
    /*

    try {;
       std::string tagname24 = "CALOH1WeightsH1WeightsCone4Topo-CaloH1WeightsCone4Topo-RUN2-02-000";
       nlohmann::json tag_info24 = myCrestClient.selectSnapshot(tagname24,0);
       std::cout << std:: endl << "test: selectSnapshot (result) = " << std::endl
		 << tag_info24.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: selectSnapshot (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: listPayloadTagInfo" << std::endl;
    std::string tagname25 = "AFTestTag_01";
    nlohmann::json tag_info25 = myCrestClientF.listPayloadTagInfo(tagname25);
    std::cout << std:: endl << "test: listPayloadTagInfo (result)" << std::endl;
    std::cout << tag_info25.dump(4) << std::endl;
    std::cout << std:: endl << "test: listPayloadTagInfo" << std::endl;
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: getPayloadAsJson" << std::endl;
    std::string hash26 = "528c87b4f00af72d6fc2405c108dd132bb525efab29b12ee1bb130e81ac70e12";
    nlohmann::json info26 = myCrestClient.getPayloadAsJson(hash26);
    std::cout << std:: endl << "test: getPayloadAsJson (result)" << std::endl;
    std::cout << info26.dump(4) << std::endl;
    std::cout << std:: endl << "test: getPayloadAsJson" << std::endl;
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: getPayloadAsString" << std::endl;
    std::string hash27 = "528c87b4f00af72d6fc2405c108dd132bb525efab29b12ee1bb130e81ac70e12";
    std::string info27 = myCrestClient.getPayloadAsString(hash27);
    std::cout << std:: endl << "test: getPayloadAsString (result)" << std::endl;
    std::cout << info27 << std::endl;
    std::cout << std:: endl << "test: getPayloadAsString" << std::endl;
    */


    //===============================================
    /*
    // command line example:
    //curl --header "Content-Type: application/json" --request GET http://crest-undertow.web.cern.ch/crestapi/runinfo?from=2018010101&to=2019050500&format=Time&page=0&size=10
    std::cout << std:: endl << "test: findRunLumiInfo" << std::endl;
    urlParameters params28;
    params28.add("from","2018010101");
    params28.add("to","2019050500");
    params28.add("format","Time");
    nlohmann::json tag_info28 = myCrestClientF.findRunLumiInfo(params28);
    std::cout << std:: endl << "test: findRunLumiInfo (result)" << std::endl;
    std::cout << tag_info28.dump(4) << std::endl;
    std::cout << std:: endl << "test: findRunLumiInfo" << std::endl;
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: createPayload" << std::endl;
    // the API has an old version with objectType parameter.

    nlohmann::json js29 = 
    {   {"hash", "ABRACADABRA3"},
	{"version", "string"},
	{"objectType", "string"},
	{"data", "none"},
        {"streamerInfo", "none"},
        {"insertionTime", "2019-03-14T13:29:25.286Z"}
    };

    // Payload Example: {"hash": "ABRACADABRA","version": "string","objectType": "string","data": "none","streamerInfo": "none","insertionTime": "2019-03-14T13:29:25.286Z"}
    // curl --header "Content-Type: application/json" --request POST --data '{"hash": "ABRACADABRA","version": "string","objectType": "string","data": "none","streamerInfo": "none","insertionTime": "2019-03-14T13:29:25.286Z"}' http://mvg-test-pc-03.cern.ch:8090/crestapi/payloads
    // curl --header "Content-Type: application/json" --request POST --data '{"tagName":"test_MvG5","since":100,"insertionTime":"2018-12-18T11:32:58.329+0000","payloadHash":"ABRACADABRA3"}' http://mvg-test-pc-03.cern.ch:8090/crestapi/iovs 

    std::cout << std:: endl << "test: createPayload" << std::endl;

    try{
       myCrestClient.createPayload(js29);
       std::cout << std:: endl << "test: createPayload (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createPayload (failed)" << std::endl;
    }
    */

    //==============================================
    /*
    std::cout << std:: endl << "test: createIov" << std::endl;

    nlohmann::json js3b = 
    {
      {"tagName","test_MvG3"},
      {"since",100},
      {"insertionTime","2018-12-18T11:32:58.329+0000"},
      {"payloadHash","ABRACADABRA4"},
    };

    try {
       myCrestClient.createIov(js3b);
       std::cout << std:: endl << "test: createIov (success)" << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: createIov (failed)" << std::endl;
    }
    */



    //==============================================
    /*
    std::cout << std:: endl << "test: createGlobalTagMap" << std::endl;

    nlohmann::json js31 = 
    {
      {"globalTagName", "MVG_TEST"},
      {"record", "testing2"},
      {"label", "test2"},
      {"tagName", "test_MvG3"}
    };


    try{
       myCrestClient.createGlobalTagMap(js31);
       std::cout << std:: endl << "test: createGlobalTagMap (success) " << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: find_createGlobalTagMap (failed)" << std::endl;
    }
    */

    //==============================================
    /*
    std::cout << std:: endl << "test: findGlobalTagMap" << std::endl;
    string name30 = "MVG_TEST";

    try{
       
       nlohmann::json tag_info30 = myCrestClient.findGlobalTagMap(name30);
       std::cout << std:: endl << "test: findGlobalTagMap (result) = "  
                 << tag_info30.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: findGlobalTagMap (failed)" << std::endl;
    }
    */


    //==============================================
    /*
    std::cout << std:: endl << "test: findGlobalTagAsString" << std::endl;
    string name31 = "MVG_TEST";

    try {
       nlohmann::json tag_info31 = myCrestClient.findGlobalTagAsString(name31);
       std::cout << std:: endl << "test: findGlobalTagAsString (result) = " << std::endl
		 << tag_info31.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: findGlobalTagAsString (failed)" << std::endl;
    }
    */


    //==============================================
    /*
    std::cout << std:: endl << "test: createTag" << std::endl;
    

    nlohmann::json js2;

    try{
        myCrestClient.createTag(js2);
        std::cout << std:: endl << "test: createTag (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createTag (failed)" << std::endl;
    }
    */

    //==============================================
    /*
    std::cout << std:: endl << "test: updateTag" << std::endl;
    
    std::string tag_name33 = "test_MvG8";

    nlohmann::json js33= {{"objectType","json3"}};

    try{
        myCrestClientF.updateTag(tag_name33, js33);
        std::cout << std:: endl << "test: updateTag (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: updateTag (failed)" << std::endl;
    }
    */

    //==============================================
    /*
    std::cout << std:: endl << "test: updateTagSpecification" << std::endl;
    
    std::string tag_name34 = "test_MvG8";
    std::string newObjectType34 = "json4";

    try{
        myCrestClientF.updateTagSpecification(tag_name34, newObjectType34);
        std::cout << std:: endl << "test: updateTagSpecification (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: updateTagSpecification (failed)" << std::endl;
    }
    */


    //================================================
    /*
    std::cout << std:: endl << "test: updateGlobalTag" << std::endl;

    std::string name36 = "MvG_TEST_01";
    nlohmann::json js36 = 
    {
       {"name","MvG_TEST_01"},
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
       myCrestClient.updateGlobalTag(name36, js36);
       std::cout << std:: endl << "test: updateGlobalTag (success) " << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: updateGlobalTag (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: selectIovs" << std::endl;

    try {
       std::string tagname37 = "AFTestTag_01";
       long snapshot37 = 0;
       nlohmann::json tag_info37 = myCrestClientF.selectIovs(tagname37, snapshot37);
       std::cout << std:: endl << "test: selectIovs (result) = " << std::endl
		 << tag_info37.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: selectIovs (failed)" << std::endl;
    }
    */

    //=============================================
    // Directory Creation for tests
    // /tmp/mvg/crest_dump

    try{

      std::filesystem::create_directories("/tmp/cresttest/crest_dump");
      try{
      }
      catch(...){
          std::cout << std:: endl << "didectory was ready" << std::endl;
      }
    }
    catch(...){
      std::cout << std:: endl << "didectory was ready" << std::endl;
    }



    //==============================================
    //
    std::cout << std:: endl << "test: createTag" << std::endl;
    
    std::string tagname38 = "test_MvG3a";
    std::string description38 = "none-38";
    std::string timeType38 = "time";
    std::string objectType38 = "json38";

    try{
        myCrestClient.createTag(tagname38, description38, timeType38, objectType38);
        std::cout << std:: endl << "test: createTag (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createTag (failed)" << std::endl;
    }
    //


    //================================================
    /*
    std::cout << std:: endl << "test: storeBatchPayloads" << std::endl;

    std::string name39 = "test_MvG4";
    uint64_t endtime = 200;
    std::string js39 =  "{\"niovs\": 2,\"format\": \"PYLD_JSON\",\"iovsList\":[{\"since\":800,\"payload\":\"vvv\"},{\"since\":900,\"payload\":\"www\"}]}";

    try {
       myCrestClientF.storeBatchPayloads(name39, endtime, js39);
       std::cout << std:: endl << "test: storeBatchPayloads (success) " << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: storeBatchPayloads (failed)" << std::endl;
    }
    */

    //================================================
    /*
    std::cout << std:: endl << "test: storeBatchPayloads" << std::endl;

    std::string name40 = "test_MvG4";
    uint64_t endtime40 = 200;
    std::string str40 =  "{\"niovs\": 2,\"format\": \"PYLD_JSON\",\"iovsList\":[{\"since\":800,\"payload\":\"vv1\"},{\"since\":900,\"payload\":\"ww1\"}]}";
    nlohmann::json js40 = myCrestClientF.getJson(str40);

    try {
       myCrestClientF.storeBatchPayloads(name40, endtime40, js40);
       std::cout << std:: endl << "test: storeBatchPayloads (success) " << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: storeBatchPayloads (failed)" << std::endl;
    }
    */


    //================================================
    //

    bool rewrite41 = true;
    std::string path41 = "/tmp/cresttest/crest_dump";
    CrestClient myCrestClientL = CrestClient(rewrite41, path41); // CrestClient(   
    // myCrestClientL.flush(); // uncomment to flush right now

    //


    //==============================================
    //
    std::cout << std:: endl << "test: createTag" << std::endl;
    

    nlohmann::json js42 = 
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
        myCrestClientL.createTag(js42);
        std::cout << std:: endl << "test: createTag (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createTag (failed)" << std::endl;
    }

    //


    //==============================================
    /*
    std::cout << std:: endl << "test: createTag" << std::endl;
    

    nlohmann::json js43 = 
    {
      {"description", "none"},
      {"endOfValidity", 0},
      {"insertionTime", "2018-12-06T11:18:35.641+0000"},
      {"lastValidatedTime", 0},
      {"modificationTime", "2018-12-06T11:18:35.641+0000"},
      {"name", "test_MvG3b"},
      {"objectType", "json"},
      {"synchronization", "none"},
      {"timeType", "time"}
    };

    try{
        myCrestClientL.createTag(js43);
        std::cout << std:: endl << "test: createTag (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createTag (failed)" << std::endl;
    }

    */

    //==============================================
    /*
    // payload + tag name (command line example):
    // curl --form file=@./test.txt --form tag="SB_TAG-PYLD" --form since=0 http://mvg-test-pc-03.cern.ch:8090/crestapi/payloads/store

    std::cout << std:: endl << "test: storePayload" << std::endl;

    std::string tagname44 = "test_MvG3a";
    uint64_t since44 = 0;
    std::string payload44 = "aaa";


    try{
      myCrestClientL.storePayload(tagname44, since44, payload44);
        std::cout << std:: endl << "test: storePayload (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: storePayload (failed)" << std::endl;
    }    

    */


    //==============================================
    /*
    // payload + tag name (command line example):
    // curl --form file=@./test.txt --form tag="SB_TAG-PYLD" --form since=0 http://mvg-test-pc-03.cern.ch:8090/crestapi/payloads/store

    std::cout << std:: endl << "test: storePayload" << std::endl;

    std::string tagname45 = "test_MvG3a";
    uint64_t since45 = 10;
    std::string payload45 = "bbb";


    try{
      myCrestClientL.storePayload(tagname45, since45, payload45);
        std::cout << std:: endl << "test: storePayload (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: storePayload (failed)" << std::endl;
    }    

    */

    //==============================================
    //

    std::cout << std:: endl << "test: getFileString" << std::endl;
    std::string filename46 = "/tmp/cresttest/crest_dump/test_MvG3a/tag.json";
    std::string tag46 = myCrestClientL.getFileString(filename46);
    std::cout << std:: endl << "tag = " << tag46 << std::endl;
    std::cout << std:: endl << "test: getFileString: Ended" << std::endl;
    //

    //==============================================
    //

    std::cout << std:: endl << "test: getFileList" << std::endl;
    std::string path47 = "/tmp/cresttest/crest_dump/";
    myCrestClientL.getFileList(path47);
    //std::cout << std:: endl << "tag = " << tag46 << std::endl;
    std::cout << std:: endl << "test: getFileList: Ended" << std::endl;
    //

    //==============================================
    //

    std::cout << std:: endl << "test: shortListTagsFs" << std::endl;
    std::list<std::string> tag_list = myCrestClientL.shortListTagsFs();
    std::cout << std:: endl << "test: shortListTagsFs: results" << std::endl;
    for (auto v : tag_list)
        std::cout << v << "\n";

    std::cout << std:: endl << "test: shortListTagsFs: Ended" << std::endl;
    //

 
    //==============================================
    /*
    std::cout << std:: endl << "test: findTagFs" << std::endl;
    std::string tagname48 = "test_MvG3a";
    try {
       nlohmann::json tag_info = myCrestClientL.findTagFs(tagname48);
       std::cout << std:: endl << "test: findTagFs (result)" << std::endl
		 << tag_info.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: findTagFs (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: findAllIovsFs" << std::endl;
    std::string tagname49 = "test_MvG3a";
    try{
        nlohmann::json tag_info49 = myCrestClientL.findAllIovsFs(tagname49);
        std::cout << std:: endl << "test: findAllIovsFs (result)" << std::endl;
        std::cout << tag_info49.dump(4) << std::endl;
        std::cout << std:: endl << "test: findAllIovsFs" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: findAllIovsFs (failed)" << std::endl;
    }
    */ 


    //===============================================
    /*
    std::cout << std:: endl << "test: getFileName" << std::endl;
    std::string path50    = "file:///tmp/mvg/crest_dump/data/test_MvG3a.10.json";
    try{
        std::string filename50 = myCrestClientL.getFileName(path50);
        std::cout << std:: endl << "test: getFileName (result)" << std::endl;
        std::cout << filename50 << std::endl;
        std::cout << std:: endl << "test: getFileName" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getFileName (failed)" << std::endl;
    }
    */

    //===============================================
    /*
    std::cout << std:: endl << "test: getBlobFs" << std::endl;
    std::string hash51    = "file:///tmp/mvg/crest_dump/data/test_MvG3a.10.json";
    try{
        std::string blob51 = myCrestClientL.getBlobFs(hash51);
        std::cout << std:: endl << "test: getBlobFs (result)" << std::endl;
        std::cout << blob51 << std::endl;
        std::cout << std:: endl << "test: getBlobFs" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getBlobFs (failed)" << std::endl;
    }
    */


    //===============================================
    /*
    std::cout << std:: endl << "test: streamTest" << std::endl;
    try{
        myCrestClientL.streamTest();
        std::cout << std:: endl << "test: streamTest (result)" << std::endl;
        // std::cout << blob51 << std::endl;
        std::cout << std:: endl << "test: streamTest" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: streamTest (failed)" << std::endl;
    }
    */


     //===============================================
    /*
    std::cout << std:: endl << "test: getBlobInStreamFs" << std::endl;
    std::string hash52 = "file:///tmp/mvg/crest_dump/data/test_MvG3a.10.json";
    std::ofstream fout("./copy.txt");
    try{
        std::string blob52 = myCrestClientL.getBlobInStreamFs(hash51, fout);
        std::cout << std:: endl << "test: getBlobInStreamFs (result)" << std::endl;
        std::cout << blob52 << std::endl;
        std::cout << std:: endl << "test: getBlobInStreamFs" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getBlobInStreamFs (failed)" << std::endl;
    }
    */

    //===============================================
    //
    std::cout << std:: endl << "test: getBlobForFs" << std::endl;
    std::string hash53    = "file:///tmp/cresttest/crest_dump/data/test_MvG3a.800.json";
    try{
        std::string blob53 = myCrestClientL.getBlob(hash53);
        std::cout << std:: endl << "test: getBlob (result)" << std::endl;
        std::cout << blob53 << std::endl;
        std::cout << std:: endl << "test: getBlob" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getBlob (failed)" << std::endl;
    }

    // myCrestClientL.flush();
    //




    //===============================================
    //
    std::cout << std:: endl << "test: findAllIovs for fs" << std::endl;
    std::string tagname54 = "test_MvG3a";
    try{
        nlohmann::json tag_info54 = myCrestClientL.findAllIovs(tagname54);
        std::cout << std:: endl << "test: findAllIovs (result)" << std::endl;
        std::cout << tag_info54.dump(4) << std::endl;
        std::cout << std:: endl << "test: findAllIovs" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: findAllIovs (failed)" << std::endl;
    }
    //

    //==============================================
    //
    std::cout << std:: endl << "test: findTag for fs" << std::endl;
    std::string tagname55 = "test_MvG3a";
    try {
       nlohmann::json tag_info = myCrestClientL.findTag(tagname55);
       std::cout << std:: endl << "test: findTag (result)" << std::endl
		 << tag_info.dump(4) << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: findTag (failed)" << std::endl;
    }
    //

    //================================================
    /*
    std::cout << std:: endl << "test: storeBatchPayloadsFs" << std::endl;

    std::string name56 = "test_MvG3a";
    std::string str56 =  "{\"niovs\": 2,\"format\": \"PYLD_JSON\",\"iovsList\":[{\"since\":800,\"payload\":\"vv1\"},{\"since\":900,\"payload\":\"ww1\"}]}";
    nlohmann::json js56 = myCrestClientL.getJson(str56);

    try {
       myCrestClientL.storeBatchPayloadsFs(name56, js56);
       std::cout << std:: endl << "test: storeBatchPayloadsFs (success) " << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: storeBatchPayloadsFs (failed)" << std::endl;
    }
    // myCrestClientL.flush(); //uncomment this line to flush right now
    */

    //================================================
    /*
    std::cout << std:: endl << "test: storePayloadDump" << std::endl;

    std::string name57 = "test_MvG3a";
    uint64_t since57 = 200;
    std::string str57 =  "{\"niovs\": 2,\"format\": \"PYLD_JSON\",\"iovsList\":[{\"since\":800,\"payload\":\"vv1\"},{\"since\":900,\"payload\":\"ww1\"}]}";
    nlohmann::json js57 = myCrestClientL.getJson(str57);

    try {
      myCrestClientL.storePayloadDump(name57, since57, str57);
       //myCrestClientF.updateGlobalTag(js39);
       std::cout << std:: endl << "test: storePayloadDump (success) " << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: storePayloadDump (failed)" << std::endl;
    }
    // myCrestClientL.flush(); //uncomment this line to flush right now
    */

    //================================================
    //
    std::cout << std:: endl << "test: storeBatchPayloads" << std::endl;

    std::string name58 = "test_MvG3a";
    uint64_t endtime58 = 200;
    std::string str58 =  "{\"niovs\": 2,\"format\": \"PYLD_JSON\",\"iovsList\":[{\"since\":800,\"payload\":\"vv1\"},{\"since\":900,\"payload\":\"ww1\"}]}";
    nlohmann::json js58 = myCrestClientL.getJson(str58);

    try {
      myCrestClientL.storeBatchPayloads(name58, endtime58, js58);
       //myCrestClientF.updateGlobalTag(js39);
       std::cout << std:: endl << "test: storeBatchPayloads (success) " << std::endl;
    }
    catch(const std::runtime_error& e){
       std::cout << std:: endl << "test: storeBatchPayloads (failed)" << std::endl;
    }
    // myCrestClientL.flush(); //uncomment this line to flush right now
    //

    //=================================================
    // Error messages parsing
    /*

    std::string str59a = "{\"type\":\"error\",\"message\":\"Cannot search iovs without a tagname selection.\"}";
    std::string str59b = "{\"message\":\"Iov created in tag test_MvG3 with time 0\",\"action\":\"storePayloadWithIovMultiForm\",\"code\":201,\"id\":\"5829730c1ab90a5de3b510df7ab7125d7d27553e1d9371243b23c2f34d81f048\"}";
    std::cout << std:: endl << "Error Parsing " << std::endl;
    nlohmann::json js59a = myCrestClientL.getJson(str59a);
    nlohmann::json js59b = myCrestClientL.getJson(str59b);

    myCrestClientL.checkErrors(js59a);
    myCrestClientL.checkErrors(js59b);

    */

    //==============================================
    // Create Tag Meta Info
    //
    std::cout << std:: endl << "test: createTagMetainfo" << std::endl;   

    std::string tagname60 = "test_MvG4";
    nlohmann::json js60 = 
    {
      {"tagName", "test_MvG4"},
      {"description", "desc-01"},
      {"tagInfo", "taginf-01"},
      {"chansize", 0},
      {"colsize", 0},
      {"insertionTime", "2019-03-14T13:29:25.286Z"}
    };

    try{
        myCrestClientM.createTagMetaInfo(tagname60, js60);
        std::cout << std:: endl << "test: createTagMetaInfo (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createTagMetaInfo (failed)" << std::endl;
    }
    //


    //==============================================
    // Get Tag Meta Info
    //
    std::cout << std:: endl << "test: getTagMetainfo" << std::endl;   

    std::string tagname61 = "test_MvG3";

    try{
        nlohmann::json js61 =  myCrestClientM.getTagMetaInfo(tagname61);
        // std::cout << std:: endl << "AAA" << js61 << "BBB" << std::endl;
        std::cout << std:: endl << "test: getTagMetaInfo (result)" << std::endl
		 << js61.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getTagMetaInfo (failed)" << std::endl;
    }
    //

    //==============================================
    // Update Tag Meta Info
    //
    std::cout << std:: endl << "test: updateTagMetainfo" << std::endl;   

    std::string tagname62 = "test_MvG3";
    nlohmann::json js62 = 
    {
      // {"tagName", "test_MvG3"},
      {"description", "desc-02"},
      // {"tagInfo", "taginf-01"},
      // {"chansize", 0},
      // {"colsize", 0},
      // {"insertionTime", "2019-03-14T13:29:25.286Z"}
    };

    try{
        myCrestClientM.updateTagMetaInfo(tagname62, js62);
        std::cout << std:: endl << "test: updateTagMetaInfo (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: updateTagMetaInfo (failed)" << std::endl;
    }
    //

    //=================================================
    // Get Resources 
    //

    std::string str63a = "{\"datatype\":\"tagmetas\",\"format\":\"TagMetaSetDto\",\"resources\":[{\"chansize\":0,\"colsize\":0,\"description\":\"desc-01\",\"insertionTime\":\"2019-12-04\",\"tagInfo\":\"taginf-01\",\"tagName\":\"test_MvG3\"}],\"size\":1}";
    std::string str63b = "{\"message\":\"Iov created in tag test_MvG3 with time 0\",\"action\":\"storePayloadWithIovMultiForm\",\"code\":201,\"id\":\"5829730c1ab90a5de3b510df7ab7125d7d27553e1d9371243b23c2f34d81f048\"}";
    std::cout << std:: endl << "Get Resources test:" << std::endl;

    try{
       nlohmann::json js63a = myCrestClientL.getJson(str63a);
       nlohmann::json js63b = myCrestClientL.getJson(str63b);

       nlohmann::json res63a = myCrestClientL.getResources(js63a);
       nlohmann::json res63b = myCrestClientL.getResources(js63b);

       std::cout << std:: endl << "test: getResources A (result):" << std::endl
                 << res63a.dump(4) << std::endl;

       std::cout << std:: endl << "test: getResources A (result):" << std::endl
                 << res63b.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getResources (failed)" << std::endl;
    }
    //

    //=================================================
    // Get the first element from Resources 
    //

    std::string str64a = "{\"datatype\":\"tagmetas\",\"format\":\"TagMetaSetDto\",\"resources\":[{\"chansize\":0,\"colsize\":0,\"description\":\"desc-01\",\"insertionTime\":\"2019-12-04\",\"tagInfo\":\"taginf-01\",\"tagName\":\"test_MvG3\"}],\"size\":1}";
    std::string str64b = "{\"message\":\"Iov created in tag test_MvG3 with time 0\",\"action\":\"storePayloadWithIovMultiForm\",\"code\":201,\"id\":\"5829730c1ab90a5de3b510df7ab7125d7d27553e1d9371243b23c2f34d81f048\"}";
    std::cout << std:: endl << "Get ResFirst test:" << std::endl;

    try{
       nlohmann::json js64a = myCrestClientL.getJson(str64a);
       nlohmann::json js64b = myCrestClientL.getJson(str64b);

       nlohmann::json res64a = myCrestClientL.getResFirst(js64a);
       nlohmann::json res64b = myCrestClientL.getResFirst(js64b);

       std::cout << std:: endl << "test: getResFirst A (result):" << std::endl
                 << res64a.dump(4) << std::endl;

       std::cout << std:: endl << "test: getResFirst A (result):" << std::endl
                 << res64b.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getResFirst (failed)" << std::endl;
    }
    //

    //=================================================
    //  Tag Meta Info Conversion
    /*

    try{

    std::string str65 = "[{\"node_description\": \" string of the folder description \"},{\"channel_list\" : [0, 10, 20] },{\"channel_names\": [\"first\", \"second\", \"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";

    std::cout << std:: endl << "test: Tag Meta Info Conversion:" 
              << "string = " << str65 << std::endl;
    nlohmann::json js65 = myCrestClientL.getJson(str65);
    std::cout << std:: endl << "test: Tag Meta Info Conversion:" 
              << " JSON = "   << js65.dump(4) << std::endl;  

    std::cout << std:: endl << "test: Tag Meta Info Conversion. Elements:" 
              << std::endl;
    std::string key = "node_description";

    int array_length = js65.size();
    for(int i=0; i<array_length; i++){
        nlohmann::json elem = js65[i];
        // std::cout << " elem = " << elem.dump(4) << std::endl;
        try{
	  std::string val = elem[key];
          std::cout << " node_description = " << val << std::endl;
        }
        catch(...){
        }
    }

    }
    catch (...){
    }
    */

    //=================================================
    // getTagMetaInfoElement test
    /*
    try {
       std::string str66 = "[{\"node_description\": \" string of the folder description \"},{\"channel_list\" : [0, 10, 20] },{\"channel_names\": [\"first\", \"second\", \"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";
       nlohmann::json js66 = myCrestClientL.getJson(str66);
       std::string name66 = "node_description";
       std::string result66 = myCrestClientL.getTagMetaInfoElement(name66,js66);
       std::cout << std:: endl << "test: getTagMetaInfoElement test:" 
                 << std::endl
                 << "string = " << "node_description = " << result66 << std::endl;
    }
    catch (...){
    }

    try {
       std::string str66b = "[{\"channel_list\" : [0, 10, 20] },{\"channel_names\": [\"first\", \"second\", \"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";
       nlohmann::json js66b = myCrestClientL.getJson(str66b);
       std::string name66b = "node_description";
       std::string result66b = myCrestClientL.getTagMetaInfoElement(name66b,js66b);
       std::cout << std:: endl << "test: getTagMetaInfoElement test:" 
                 << std::endl
                 << "string = " << "node_description = " << result66b << std::endl;
    }
    catch (...){
    }

    */


    //==============================================
    // Tag Meta Info Converting
    /*
    std::cout << std:: endl << "test: convertTagMetaInfo2CREST" << std::endl;   

    std::string tagname67 = "test_MvG4";

    std::string str67 = "[{\"node_description\":\" string of the folder description \"},{\"channel_list\":[0,10,20]},{\"channel_names\": [\"first\",\"second\",\"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";

    nlohmann::json js67 = myCrestClientM.getJson(str67);

    try{
        nlohmann::json res67 = myCrestClientM.convertTagMetaInfo2CREST(js67);
        std::cout << std:: endl << "test: convertTagMetaInfo2CREST (success)" 
                  << "result = " << res67.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: convertTagMetaInfo2CREST (failed)" << std::endl;
    }
    */

    //==============================================
    // Get Tag Meta Info (2)
    /*
    std::cout << std:: endl << "test: getTagMetainfo" << std::endl;   

    std::string tagname68 = "test_MvG3";

    try{
        nlohmann::json js68 =  myCrestClientM.getTagMetaInfoIOVDbSvc(tagname68);
        std::cout << std:: endl << "test: getTagMetaInfo (result)" << std::endl
		 << js68.dump(4) << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: getTagMetaInfo (failed)" << std::endl;
    }
    */


    //==============================================
    // Create Tag Meta Info (2)
    //
    std::cout << std:: endl << "test: createTagMetainfoIOVDbSvc" << std::endl;   

    // std::string tagname69 = "test_MvG4";
    std::string tagname69 = "test_MvG2";
    std::string str69 = "[{\"node_description\":\" string of the folder description \"},{\"channel_list\":[0,10,20]},{\"channel_names\": [\"first\",\"second\",\"third\"]},{\"payload_specification\":\"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float\"}]";

    nlohmann::json js69 =  myCrestClientM.getJson(str69);

    try{
        // restClientM.createTagMetaInfo(tagname69, js69);
        myCrestClientM.createTagMetaInfoIOVDbSvc(tagname69, js69);
        std::cout << std:: endl << "test: createTagMetaInfoIOVDbSvc (success)" << std::endl;
    }
    catch (const std::runtime_error& e){
        std::cout << std:: endl << "test: createTagMetaInfoIOVDbSvc (failed)" << std::endl;
    }
    //

    return 0;
}


