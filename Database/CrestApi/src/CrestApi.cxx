/*!
   \file
   \brief

   This file contains the methods realization for the CREST C++ Client Library.
   See method description in crestapi.h.
 */

#include <CrestApi/CrestApi.h>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace Crest {

  CrestClient::CrestClient(bool rewriteIfExists, const std::string& root_folder) : m_mode(FILESYSTEM_MODE),
    m_root_folder(root_folder), m_isRewrite(rewriteIfExists) {
    if (!std::filesystem::exists(std::filesystem::path(m_root_folder))) {
      std::filesystem::create_directory(std::filesystem::path(m_root_folder));
    }
    m_data_folder = m_root_folder + "/data";
    if (!std::filesystem::exists(std::filesystem::path(m_data_folder))) {
      std::filesystem::create_directory(std::filesystem::path(m_data_folder));
    }
  }

  CrestClient::CrestClient(const std::string& _host, const std::string& _port) : m_host(_host), m_port(_port), m_mode(
      SERVER_MODE) {
  }

  CrestClient::CrestClient(const std::string& url) : m_mode(SERVER_MODE) {
    size_t found = url.find_first_of(":");

    std::string protocol = url.substr(0, found);
    std::string url_new = url.substr(found + 3); //url_new is the url excluding the http part
    size_t found1 = url_new.find_first_of(":");
    size_t found2 = url_new.find_first_of("/");
    if (found1 != std::string::npos && found2 != std::string::npos) {
      m_host = url_new.substr(0, found1);
      m_port = url_new.substr(found1 + 1, found2 - found1 - 1);
    } else if (found1 != std::string::npos) {
      m_host = url_new.substr(0, found1);
      m_port = url_new.substr(found1 + 1);
    } else if (found2 != std::string::npos) {
      m_port = "80";
      m_host = url_new.substr(0, found2);
    } else {
      m_port = "80";
      m_host = url_new;
    }

    std::cout << "host=" << m_host << " port" << m_port << std::endl;
  }

  CrestClient::~CrestClient() {
    flush();
  }

//==================================
//  TAG METHODS
//

  nlohmann::json CrestClient::listTags() {
    std::string method_name = "CrestClient::listTags";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_TAG_PATH;
    std::string getTxt = "GET";

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }


  nlohmann::json CrestClient::listTags(int size, int page) {
    std::string method_name = "CrestClient::listTags";

    checkFsException(method_name);

    int size_default = 1000;
    int page_default = 0;
    std::string size_param = "";
    std::string page_param = "";
    std::string params = "";

    if (size == size_default && page == page_default){
        nlohmann::json respond = listTags();
        return getResources(respond);
    }

    if (size != size_default) {
       size_param = "size=" + std::to_string(size);
       params = params + size_param;
    }
    if (page != page_default) {
       page_param = "page=" + std::to_string(page);
       if (page_param == "") {
          params = params + page_param;
       }
       else{
          params = params + "&" + page_param;
       }
    }

    std::string current_path = s_PATH + s_TAG_PATH + "?" + params;
    std::string getTxt = "GET";

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }


  void CrestClient::removeTag(const std::string& tagName) {
    std::string method_name = "removeTag";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_ADMIN_PATH + s_TAG_PATH + "/" + tagName;
    std::string getTxt = "DELETE";

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, DELETE, js, method_name);

  }

  nlohmann::json CrestClient::findTag(const std::string& tagName) {
    std::string method_name = "findTag";
    if (m_mode == FILESYSTEM_MODE) {
      return findTagFs(tagName);
    }

    std::string current_path = s_PATH + s_TAG_PATH + "/" + tagName;
    std::string getTxt = "GET";

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::createTag(const std::string& name, const std::string& desc, const std::string& timeType) {
    std::string method_name = "CrestClient::createTag";

    nlohmann::json js2 = {{"description", desc}, {"endOfValidity", 0},
                          {"lastValidatedTime", 0}, {"synchronization", "none"}, {"payloadSpec", "json"},
                          {"name", name}, {"timeType", timeType}};
    createTag(js2);
  }

  void CrestClient::createTag(const std::string& name, const std::string& desc, const std::string& timeType,
                              const std::string& payloadSpec) {
    std::string method_name = "CrestClient::createTag";

    nlohmann::json js2 = {{"description", desc}, {"endOfValidity", 0},
                          {"lastValidatedTime", 0}, {"synchronization", "none"}, {"payloadSpec", payloadSpec},
                          {"name", name}, {"timeType", timeType}};

    createTag(js2);
  }

  void CrestClient::createTag(nlohmann::json& js) {
    std::string method_name = "CrestClient::createTag";
    if (m_mode == FILESYSTEM_MODE) {
      createTagDump(js);
      return;
    }

    std::string current_path = s_PATH + s_TAG_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);

  }

  void CrestClient::updateTag(const std::string& tagname, nlohmann::json body) {
    std::string method_name = "CrestClient::updateTag";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_TAG_PATH + "/" + tagname;

    std::string retv;

    retv = performRequest(current_path, PUT, body, method_name);

  }

  void CrestClient::updateTagSpecification(const std::string& tagname, const std::string& objectType) {
    std::string method_name = "CrestClient::updateTagSpecification";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_TAG_PATH + "/" + tagname;

    nlohmann::json body;
    body["objectType"] = objectType;


    std::string retv;
    retv = performRequest(current_path, PUT, body, method_name);

  }

  void CrestClient::createTagDump(nlohmann::json& js) {
    std::string name = js["name"];
    std::string workDir = m_root_folder + "/" + name;
    if (!std::filesystem::exists(std::filesystem::path(workDir))) {
      std::filesystem::create_directory(std::filesystem::path(workDir));
    }
    if (name.compare(m_currentTag) != 0 && m_isRewrite) {
      std::cerr << "new tag:" << m_currentTag << std::endl;
      flush();
    }
    m_currentTag = name;

    if (m_isRewrite) {
      if (std::filesystem::exists(std::filesystem::path(workDir + "/tag.json"))) {
        std::filesystem::remove(std::filesystem::path(workDir + "/tag.json"));
      }
      if (std::filesystem::exists(std::filesystem::path(workDir + "/iovs.json"))) {
        std::filesystem::remove(std::filesystem::path(workDir + "/iovs.json"));
      }
      std::ofstream myfile;
      std::string tagFile = workDir + "/tag.json";
      myfile.open(tagFile.c_str());
      myfile << js;
      myfile.close();
    }
    if (m_data.find(name) == m_data.end()) {
      m_data.insert(std::pair<std::string, nlohmann::json>(name, nlohmann::json(nlohmann::json::value_t::array)));
    }
  }

  void CrestClient::storePayloadDump(const std::string& tag, uint64_t since, const std::string& js) {
    std::ofstream myfile;
    std::string tagFile = m_data_folder + "/" + tag + "." + std::to_string(since) + ".json";
    myfile.open(tagFile);
    myfile << js;
    myfile.close();
    std::map<std::string, nlohmann::json>::iterator it = m_data.find(tag);
    if (it != m_data.end()) {
      std::string link = "file://" + tagFile;
      nlohmann::json iovs = it->second;
      nlohmann::json obj(nlohmann::json::value_t::object);
      obj["tagName"] = tag;
      obj["since"] = since;
      obj["insertionTime"] = 0;
      obj["payloadHash"] = link;
      iovs.push_back(obj);
      m_data[it->first] = iovs;
    }
  }

  void CrestClient::flush() {
    if (m_mode == SERVER_MODE) return;

    std::cerr << "Flush start" << std::endl;
    for (auto& item : m_data) {
      nlohmann::json iov = item.second;
      std::string name = item.first;
      std::cerr << "Flush tag=" << name << std::endl;
      std::string workDir = m_root_folder + "/" + name;
      std::ofstream myfile;
      std::string tagFile = workDir + "/iovs.json";
      myfile.open(tagFile.c_str());
      myfile << iov;
      myfile.close();
    }
    m_data.clear();
  }

// IOVs

  void CrestClient::createIov(nlohmann::json& js) {
    std::string method_name = "CrestClient::createIov";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);

  }

  nlohmann::json CrestClient::findAllIovs(const std::string& tagname) {
    std::string method_name = "CrestClient::findAllIovs";
    if (m_mode == FILESYSTEM_MODE) {
      return findAllIovsFs(tagname);
    }

    std::string current_path = s_PATH + s_IOV_PATH + "?by=tagname:" + tagname;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }


  nlohmann::json CrestClient::findAllIovs(const std::string& tagname, int size, int page, const std::string& sort, const std::string& dateformat) {
    std::string method_name = "CrestClient::findAllIovs";
    if (m_mode == FILESYSTEM_MODE) {
      return findAllIovsFs(tagname);
    }

    // http request examples:
    // http://crest-01.cern.ch:8080/crestapi/iovs?by=tagname:test_MvG3b&size=1&page=1
    // http://crest-02.cern.ch:8090/crestapi/tags?size=10&page=2&sort=name:DESC

    std::string current_path = s_PATH + s_IOV_PATH + "?by=tagname:" + tagname + "&size=" + std::to_string(size) + "&page=" + std::to_string(page) + "&sort=" + sort + "&dateformat=" + dateformat;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }


  nlohmann::json CrestClient::getSizeByTag(const std::string& tagname) {
    std::string method_name = "CrestClient::getSizeByTag";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH + s_IOV_SIZE_PATH_FOR_TAG + "?tagname=" + tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  int CrestClient::getSize(const std::string& tagname) {
    std::string method_name = "CrestClient::getSize";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH + s_IOV_SIZE_PATH + "?tagname=" + tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    int respond = atoi(retv.c_str());

    return respond;
  }

  nlohmann::json CrestClient::selectIovs(const std::string& tagname) {
    std::string method_name = "CrestClient::selectIovs";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH + s_IOV_SELECT_PATH + "?tagname=" + tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectIovs(const std::string& tagname, long snapshot) {
    std::string method_name = "CrestClient::selectIovs";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH + s_IOV_SELECT_PATH + "?tagname=" + tagname + "&snapshot=" +
                               std::to_string(snapshot); //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectGroups(const std::string& tagname) {
    std::string method_name = "CrestClient::selectGroups";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH + s_IOV_GROUP_PATH + "?tagname=" + tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectGroups(const std::string& tagname, long snapshot) {
    std::string method_name = "CrestClient::selectGroups";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH + s_IOV_GROUP_PATH + "?tagname=" + tagname + "&snapshot=" +
                               std::to_string(snapshot); //return json

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectSnapshot(const std::string& tagname) {
    std::string method_name = "CrestClient::selectSnapshot";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH + s_IOV_SNAPSHOT_PATH + "?tagname=" + tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectSnapshot(const std::string& tagname, long snapshot) {
    std::string method_name = "CrestClient::selectSnapshot";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_IOV_PATH + s_IOV_SNAPSHOT_PATH + "?tagname=" + tagname + "&snapshot=" +
                               std::to_string(snapshot); //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

// GLOBALTAGS

  void CrestClient::updateGlobalTag(const std::string& name, nlohmann::json body) {
    std::string method_name = "CrestClient::updateGlobalTag";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_ADMIN_PATH + s_GLOBALTAG_PATH + "/" + name;

    std::string retv;

    retv = performRequest(current_path, PUT, body, method_name);

  }

  void CrestClient::createGlobalTag(nlohmann::json& js) {
    std::string method_name = "CrestClient::createGlobalTag";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_GLOBALTAG_PATH;
    std::string retv;
    retv = performRequest(current_path, POST, js, method_name);

  }

  void CrestClient::createGlobalTag(const std::string& tag) {
    std::string method_name = "CrestClient::createGlobalTag";

    checkFsException(method_name);

    nlohmann::json js = getJson(tag, method_name);
    createGlobalTag(js);
  }

  std::string CrestClient::findGlobalTagAsString(const std::string& name) {
    std::string method_name = "CrestClient::findGlobalTagAsString";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_GLOBALTAG_PATH + "/" + name;
    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js);

    return retv;
  }

  nlohmann::json CrestClient::findGlobalTag(const std::string& name) {
    std::string method_name = "CrestClient::findGlobalTag";

    checkFsException(method_name);

    std::string retv = findGlobalTagAsString(name);
    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::removeGlobalTag(const std::string& name) {
    std::string method_name = "CrestClient::removeGlobalTag";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_ADMIN_PATH + s_GLOBALTAG_PATH + "/" + name;
    std::string retv;
    nlohmann::json js = nullptr;
    retv = performRequest(current_path, DELETE, js, method_name);

  }

  nlohmann::json CrestClient::listGlobalTags() {
    std::string method_name = "CrestClient::listGlobalTags";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_GLOBALTAG_PATH;
    std::string retv;
    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);
    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  std::string CrestClient::listGlobalTagsAsString() {
    std::string method_name = "CrestClient::listGlobalTagsAsString";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_GLOBALTAG_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }

//=============================
// GLOBALTAGM MAPs

  nlohmann::json CrestClient::findGlobalTagMap(const std::string& name) {
    std::string method_name = "CrestClient::findGlobalTagMap";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_GLOBALTAG_MAP_PATH + "/" + name;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::createGlobalTagMap(nlohmann::json& js) {
    std::string method_name = "CrestClient::createGlobalTagMap";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_GLOBALTAG_MAP_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);

  }

//==============================
//BLOBS & PAYLOADS

  std::string CrestClient::getBlob(const std::string& hash) {
    std::string method_name = "CrestClient::getBlob";
    if (m_mode == FILESYSTEM_MODE) {
      return getBlobFs(hash);
    }

    std::string current_path = s_PATH + s_PAYLOAD_PATH + "/" + hash;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }

  std::string CrestClient::getBlobInStream(const std::string& hash, std::ofstream& out) {
    std::string method_name = "CrestClient::getBlobInStream";
    if (m_mode == FILESYSTEM_MODE) {
      return getBlobInStreamFs(hash, out);
    }

    std::string current_path = s_PATH + s_PAYLOAD_PATH + "/" + hash;

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, out);

    return retv;
  }

// PAYLOADS

  nlohmann::json CrestClient::getPayloadMetaInfo(const std::string& hash) {
    std::string method_name = "CrestClient::getPayloadMetaInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_PAYLOAD_PATH + "/" + hash + s_META_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }


  std::string CrestClient::getPayloadMetaInfoAsString(const std::string& hash) {
    std::string method_name = "CrestClient::getPayloadMetaInfoAsString";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_PAYLOAD_PATH + "/" + hash + s_META_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }


  nlohmann::json CrestClient::listPayloadTagInfo(const std::string& tagname) {
    std::string method_name = "CrestClient::getPayloadTagInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_MONITORING_PAYLOAD_PATH + "?tagname=" + tagname;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::listPayloadTagInfo() {
    std::string method_name = "CrestClient::getPayloadTagInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_MONITORING_PAYLOAD_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::getPayloadAsJson(const std::string& hash) {
    std::string method_name = "CrestClient::getPayloadAsJson";

    checkFsException(method_name);
 
    std::string current_path = s_PATH + s_PAYLOAD_PATH + "/" + hash + "?format=DTO";

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return respond;
  }

  std::string CrestClient::getPayloadAsString(const std::string& hash) {
    std::string method_name = "CrestClient::getPayloadAsString";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_PAYLOAD_PATH + "/" + hash + "?format=BLOB";

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }

  void CrestClient::createPayload(nlohmann::json& js) {
    std::string method_name = "CrestClient::createPayload";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_PAYLOAD_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);
  }

// METHODS to store the PAYLOADS in BATCH mode

  void CrestClient::storeBatchPayloads(const std::string& tag_name, uint64_t endtime, const std::string& iovsetupload) {
    std::string method_name = "CrestClient::storeBatchPayloads";
    nlohmann::json js = getJson(iovsetupload, method_name);
    storeBatchPayloads(tag_name, endtime, js);
  }

  void CrestClient::storeBatchPayloads(const std::string& tag_name, uint64_t endtime, nlohmann::json& js) {
    std::string method_name = "CrestClient::storeBatchPayloads";
    if (m_mode == FILESYSTEM_MODE) { 
      storeBatchPayloadsFs(tag_name, js);
      return;
    }
    if (!js.is_array()) {
      throw std::runtime_error("ERROR in " + method_name + " JSON has wrong type (must be array)");
    }

    nlohmann::json jsObj = {};
    jsObj["datatype"] = "iovs";
    jsObj["format"] = "JSON";
    jsObj["size"] = js.size();
    jsObj["resources"] = js;
    std::string str = jsObj.dump();
    std::string retv = storeBatchPayloadRequest(tag_name, endtime, str);

    nlohmann::json respond = getJson(retv, method_name);
 
  }

//=========================================
// FOLDERS

  nlohmann::json CrestClient::listFolders() {
    std::string method_name = "CrestClient::listFolders";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_FOLDER_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);

  }

  std::string CrestClient::createFolder(nlohmann::json& body) {
    std::string method_name = "CrestClient::createFolder";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_FOLDER_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, body, method_name);

    return retv;
  }

//==========================================
// RUN INFO

  nlohmann::json CrestClient::listRunLumiInfo() {
    std::string method_name = "CrestClient::listRunLumiInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_RUNINFO_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::createRunLumiInfo(nlohmann::json& body) {
    std::string method_name = "CrestClient::createRunLumiInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_RUNINFO_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, body, method_name);
  }

  nlohmann::json CrestClient::findRunLumiInfo(urlParameters params) {
    std::string method_name = "CrestClient::findRunLumiInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_RUNINFO_PATH + s_RUNINFO_LIST_PATH + "?" + params.getParams();

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

// REQUEST METHODS

  void CrestClient::storePayload(const std::string& tag, uint64_t since, const std::string& js) {
    if (m_mode == FILESYSTEM_MODE) {
      storePayloadDump(tag, since, js);
      return;
    }
    storePayloadRequest(tag, since, js);
  }

  struct data {
    char trace_ascii; /* 1 or 0 */
  };


  size_t CurlWrite_CallbackFunc_StdString(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;

    try
    {
      s->append((char*) contents, newLength);
    }
    catch (std::bad_alloc& e)
    {
      //handle memory problem
      return 0;
    }
    return newLength;
  }


  std::string CrestClient::storePayloadRequest(const std::string& tag, uint64_t since, const std::string& js) {
    std::string current_path = "/crestapi/payloads/store";
    CURL* curl;
    CURLcode res;

    struct curl_httppost* formpost = NULL;
    struct curl_httppost* lastptr = NULL;

    curl = curl_easy_init();
    std::string stt;
    struct curl_slist* headers = NULL;
    if (curl) {
      std::ostringstream url;
      std::string s;
      url << "http://" << m_host << ":" << m_port << current_path;

      curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
      headers = curl_slist_append(headers, "Accept: */*");
      headers = curl_slist_append(headers, "Content-Type:  multipart/form-data");
      headers = curl_slist_append(headers, "Expect:");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "tag",
                   CURLFORM_COPYCONTENTS, tag.c_str(),
                   CURLFORM_END);
      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "since",
                   CURLFORM_COPYCONTENTS, std::to_string(since).c_str(),
                   CURLFORM_END);
      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "file",
                   CURLFORM_COPYCONTENTS, js.c_str(),
                   CURLFORM_END);

      curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s); 

      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);

      // data to check the errors in the server response:
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::string method_name = "CrestClient::storePayload";

      /* always cleanup */
      curl_easy_cleanup(curl);
      curl_formfree(formpost);
      curl_slist_free_all(headers);

      curl_global_cleanup();

      // error checking in the server response:
      checkResult( res, response_code, s, method_name);

      return s;
    }

    throw std::runtime_error("ERROR in CrestClient::storePayload | CURL not init");

  }

  std::string CrestClient::performRequest(const std::string& current_path, Action action, nlohmann::json& js,
                                          const std::string& method_name) {
    CURL* curl;
    CURLcode res;

    /* Enable tracing of ascii data */


    /* get a curl handle */
    curl = curl_easy_init();
    std::string stt;
    struct curl_slist* headers = NULL;
    if (curl) {
      std::ostringstream url;
      std::string s;
      url << "http://" << m_host << ":" << m_port << current_path;

      curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
      if (js.is_null()) {
        if (action == DELETE) curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
      } 
      else {
        stt = js.dump();

        if (action == PUT) curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        headers = curl_slist_append(headers, "Accept: */*");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        headers = curl_slist_append(headers, "Expect:");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        /* set where to read from (on Windows you need to use READFUNCTION too) */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, stt.c_str());
      }

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);


      /* Perform the request, res will get the return code */
      res = curl_easy_perform(curl);


      // data to check the errors in the server response:
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::string method_name = "CrestClient::storePayload";

      /* always cleanup */
      curl_easy_cleanup(curl);
      curl_slist_free_all(headers);

      curl_global_cleanup();

      // error checking in the server response:
      checkResult( res, response_code, s, method_name);

      return s;
    }

    throw std::runtime_error("ERROR in " + method_name + " | CURL not init");
  }

// REQUEST OLD VERSION

  std::string CrestClient::performRequest(const std::string& current_path, Action action, nlohmann::json& js) {
    std::string method = "Unknown";
    return performRequest(current_path, action, js, method);
  }

// REQUEST OLD VERSION (end)


//==========================
// REQUEST for STREAM

  std::string CrestClient::performRequest(const std::string& current_path, Action action, nlohmann::json& js,
                                          std::ofstream& out) {
    boost::asio::streambuf response;
    boost::asio::io_service io_service;

    try {

      // Get a list of endpoints corresponding to the server name.
      boost::asio::ip::tcp::resolver resolver(io_service);
      boost::asio::ip::tcp::resolver::query query(m_host, m_port, boost::asio::ip::resolver_query_base::flags());
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

      // Try each endpoint until we successfully establish a connection.
      boost::asio::ip::tcp::socket socket(io_service);
      boost::asio::connect(socket, endpoint_iterator);

      std::string retv = "Invalid response\n";

      boost::asio::streambuf request;
      std::ostream request_stream(&request);

      if (js.is_null()) {
        request_stream << action << " " << current_path << " HTTP/1.0\r\n";
        request_stream << "Host: " << m_host << ":" << m_port << " \r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";
      } else {
        std::string s = js.dump();
        request_stream << action << " " << current_path << " HTTP/1.0\r\n";
        request_stream << "Host: " << m_host << ":" << m_port << " \r\n";
        request_stream << "Content-Type: application/json; \r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Content-Length: " << s.length() << "\r\n";
        request_stream << "Connection: close\r\n\r\n";
        request_stream << s;
      }


      //begin request

      // Send the request.
      boost::asio::write(socket, request);

      // Read the response status line. The response streambuf will automatically
      // grow to accommodate the entire line. The growth may be limited by passing
      // a maximum size to the streambuf constructor.

      boost::asio::read_until(socket, response, "\r\n");



      boost::asio::read_until(socket, response, "\r\n");

      // Check that response is OK.
      std::istream response_stream(&response);
      std::string http_version;
      response_stream >> http_version;
      unsigned int status_code;
      response_stream >> status_code;

      std::string status_message;
      std::getline(response_stream, status_message);


      if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        std::cout << "Invalid response" << std::endl;
        return retv; // response;
      }
      if (status_code != 200) {
        std::cout << "Response returned with status code " << status_code << std::endl;

        return retv;// response;
      }

      // Read the response headers, which are terminated by a blank line.
      boost::asio::read_until(socket, response, "\r\n\r\n");

      // Process the response headers.
      std::string header;
      while (std::getline(response_stream, header) && header != "\r"){
        std::cout << header << std::endl;
      }
      std::cout << std::endl;
      //========
      std::ostringstream txt;

      // Write whatever content we already have to output.
      if (response.size() > 0) out << &response;


      // Read until EOF, writing data to output as we go.
      boost::system::error_code error;
      while (boost::asio::read(socket, response,
                               boost::asio::transfer_at_least(1), error))
        out << &response;

      if (error != boost::asio::error::eof) {
        std::cerr << "connection error" << std::endl;
        std::string st0 = "";
        return st0;
      }

      retv = status_code;
      return retv;

      //end request
    }
    catch (boost::system::system_error& e) {
      std::cerr << e.what() << " | " << e.code() << std::endl;
      std::string st0 = "";
      return st0;
    }
    catch (...) {
      std::cerr << "connection error" << std::endl;
      std::string st0 = "";
      return st0;
    }
  }


// Request method to store payloads in batch mode

  std::string CrestClient::storeBatchPayloadRequest(const std::string& tag, uint64_t endtime, const std::string& js) {
    std::string current_path = "/crestapi/payloads/storebatch";
    std::string mes = "ERROR in CrestClient::storeBatchPayloads";
    CURL* curl;
    CURLcode res;

    struct curl_httppost* formpost = NULL;
    struct curl_httppost* lastptr = NULL;

    // get a curl handle

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    std::string stt;
    std::string myst;
    struct curl_slist* headers = NULL;
    if (curl) {
      std::ostringstream url;
      std::string s;
      url << "http://" << m_host << ":" << m_port << current_path;

      // First set the URL that is about to receive our POST. This URL can
      // just as well be a https:
      // URL if that is what should receive the data.

      curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
      headers = curl_slist_append(headers, "Accept: */*");
      headers = curl_slist_append(headers, "Content-Type:  multipart/form-data");
      headers = curl_slist_append(headers, "Expect:");
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl, CURLOPT_POST, 1L); //

      // Create the form  for new version
      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "tag",
                   CURLFORM_COPYCONTENTS, tag.c_str(),
                   CURLFORM_END);
      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "endtime",
                   CURLFORM_COPYCONTENTS, std::to_string(endtime).c_str(),
                   CURLFORM_END);
      curl_formadd(&formpost,
                   &lastptr,
                   CURLFORM_COPYNAME, "iovsetupload",
                   CURLFORM_COPYCONTENTS, js.c_str(),
                   CURLFORM_END);

 
      curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrite_CallbackFunc_StdString);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);


      // Perform the request, res will get the return code
      res = curl_easy_perform(curl);

      // data to check the errors in the server response:
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::string method_name = "CrestClient::storePayload";

      /* always cleanup */
      curl_easy_cleanup(curl);
      curl_formfree(formpost);
      curl_slist_free_all(headers);

      curl_global_cleanup();

      // error checking in the server response:
      checkResult( res, response_code, s, method_name);

      return s;
    }

    throw std::runtime_error(mes + " | CURL not init");
  }

// end of REQUEST METHODS

//==============================
// UTILITIES


  nlohmann::json CrestClient::getJson(const std::string& str, const std::string& method) {
    try {
      nlohmann::json js = nlohmann::json::parse(str);
      return js;
    } 
    catch (nlohmann::json::parse_error& e) {
      if (method == ""){
        // method name is undefined

	std::string wh = e.what();
	throw std::runtime_error("ERROR in JSON conversion: " + wh + " | In string: " + str);
      }
      else {
        std::string str2 = ParseXMLOutput(str);  // to remove HTML tags use this function
        std::string str3 = removeCR(str2); // to remove carridge return
	throw std::runtime_error("ERROR in " + method + " | CREST Server response : " + str3);
      }
    }
  }

  nlohmann::json CrestClient::getJson(const std::string& str) {
    return getJson(str,"");
  }


  // The auxillary method to remove XML/HTML tags from a std::string

  std::string CrestClient::ParseXMLOutput(std::string xmlBuffer){
    bool copy = true;
    std::string plainString = "";   
    std::stringstream convertStream;

    // remove all xml tags
    for (long unsigned int i=0; i < xmlBuffer.length(); i++)
    {                   
        convertStream << xmlBuffer[i];

        if(convertStream.str().compare("<") == 0) copy = false;
        else if(convertStream.str().compare(">") == 0) 
        {
            copy = true;
            convertStream.str(std::string());
            continue;
        }

        if(copy) plainString.append(convertStream.str());       

        convertStream.str(std::string());
    }

    return plainString;
  }

  std::string CrestClient::removeCR(const std::string& str) {
     std::string str2 = str;
     std::string needle = "\n";
     size_t pos;
     while ((pos = str2.find(needle)) != str2.npos) {
         str2.replace(pos, 1, "|");
     }
     needle = "\r";
     while ((pos = str2.find(needle)) != str2.npos) {
         str2.replace(pos, 1, "");
     }
     return str2;
  }


  bool CrestClient::isJson(const std::string& str) {
    try {
      nlohmann::json js = nlohmann::json::parse(str);
      return true;
    } 
    catch (nlohmann::json::parse_error& e) {
      return false;
    }
  }

  void CrestClient::checkResult(CURLcode res, const long response_code, const std::string& st, const std::string& method_name){
    std::string mes = "ERROR in " + method_name + " | ";
    std::string s = st;

    // Bad HTTP response:

    if ( res != CURLE_OK ) {
      throw std::runtime_error(mes + std::to_string(response_code));
    }

    // Errors, decoded from JSON CREST Server messages:

    if (isJson(s)) {
      nlohmann::json respond = getJson(s);
      checkErrors(respond, method_name);
    }

    // HTTP response code error interval:

    if (response_code>=400 || response_code==303 ){

      s = ParseXMLOutput(s);  // to remove HTML tags 
      s = removeCR(s);        // to remove end lines and carridge returns 

      throw std::runtime_error(mes + "CREST Server response : " + s);
    }
  }

//================================
// METHODS FOR WORKING WITH THE DATA STORED in THE FILE SYSTEM STORAGE

// UTILITIES

  std::string CrestClient::getFileString(const std::string& path) {
    std::ifstream ifs(path);
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
  }

  void CrestClient::getFileList(const std::string& path) {
    std::filesystem::path p(path);
    for (auto i = std::filesystem::directory_iterator(p); i != std::filesystem::directory_iterator(); i++) {
      std::string file = i->path().filename().string();
      if (file != "data") {
        std::cout << file << std::endl;
      } else continue;
    }
  }

  std::list<std::string> CrestClient::shortListTagsFs() {
    std::list<std::string> tag_list;
    std::filesystem::path p(m_root_folder);
    for (auto i = std::filesystem::directory_iterator(p); i != std::filesystem::directory_iterator(); i++) {
      std::string file = i->path().filename().string();

      if (file != "data") {
        tag_list.push_back(file);
      } else continue;
    }
    return tag_list;
  }

  nlohmann::json CrestClient::findTagFs(const std::string& name) {
    nlohmann::json js = nullptr;
    std::string file_path = m_root_folder + "/" + name + "/tag.json";
    try{
      std::string tag = getFileString(file_path);
      js = nlohmann::json::parse(tag);
    }
    catch (...) {

      throw std::runtime_error("ERROR in CrestClient::findTagFs cannot get the tag information form file storage.");

    }

    return js;
  }

  nlohmann::json CrestClient::findAllIovsFs(const std::string& tagname) {
    nlohmann::json js = nullptr;
    std::string file_path = m_root_folder + "/" + tagname + "/iovs.json";

    try{
      std::string tag = getFileString(file_path);
      js = nlohmann::json::parse(tag);
    }
    catch (...) {

      throw std::runtime_error("ERROR in CrestClient::findAllIovsFs : cannot get the iov information form file storage");
    }

    return js;
  }

// auxillary method to get a file name from hash
  std::string CrestClient::getFileName(const std::string& path) {
    std::string filename = "";
    int size = path.size();
    int pos = path.find(":");
    filename = path.substr(pos + 3, size);
    return filename;
  }

  std::string CrestClient::getBlobFs(const std::string& hash) {
    std::string str = "";
    std::string file_path = getFileName(hash);
    try{
      str = getFileString(file_path);
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::getBlobFs : cannot get the blob information form file storage");
    }

    return str;
  }

  std::string CrestClient::getBlobInStreamFs(const std::string& hash, std::ofstream& out) {
    std::string line;
    std::string str = "";

    std::string file_path = getFileName(hash);
    std::ifstream in(file_path);

    if (in.is_open() && out.is_open()) {
      while (getline(in, line)) {
        out << line << std::endl;
      }
    } else {

      throw std::runtime_error("ERROR in CrestClient::getBlobInStreamFs : cannot get the blob information form file storage");
    }
    in.close();

    return str;
  }

// this is test only, later it will be deleted
  void CrestClient::streamTest() {
    std::string line;

    std::ifstream in("./test-03.txt");
    std::ofstream fout("./copy.txt");
    if (in.is_open() && fout.is_open()) {
      while (getline(in, line)) {
        fout << line << std::endl;
      }
    }
    in.close();
  }

  //==========================================
  // storeBatchPayloadsFs methods

void CrestClient::storeBatchPayloadsFs(std::string tag_name, std::string& iovsetupload){
    std::string method_name = "CrestClient::storeBatchPayloadsFs";
    nlohmann::json js = getJson(iovsetupload, method_name);
    if(!js.is_array()){
        throw std::runtime_error("ERROR in CrestClient::storeBatchPayloadsFs: JSON has wrong type (must be array");
    }

   storeBatchPayloadsFs(tag_name, js);

}


  void CrestClient::storeBatchPayloadsFs(std::string tag_name, nlohmann::json& js){
    std::string method_name = "CrestClient::storeBatchPayloadsFs";

    try { 
      for (auto& kvp : js) {
        std::string payload =  kvp.value("payloadHash", "");
        int since =  kvp.value("since", 0);
        storePayload(tag_name, since, payload);
      }
    } // end of try
    catch(...) {
      throw std::runtime_error("ERROR in " + method_name + " cannot store the data in a file");
    } // end of catch

}

  // storeBatchPayloadsFs (end)


  std::string CrestClient::getEnvA(const std::string& varname) {
    std::string respond = "";

    char* pPath;
    const char* c = varname.data();
    pPath = std::getenv(c);
    if (pPath != NULL) respond = std::string(pPath);
    return respond;
  }

  std::string CrestClient::getDataPath() {
    std::string varName = "DAT_PATH";
    std::string respond = getEnvA(varName) + "/data/crestapi";
    return respond;
  }


  int CrestClient::checkErrors(const nlohmann::json& js,const std::string& method) {

    int result = 0;
    auto res = js.find("type");
    if (res != js.end()) {
      std::string type = js.value("type", " unknown type ");
      std::string message = js.value("message", " unknown message ");
      result = 1;
      throw std::runtime_error("ERROR in " + method + " | CREST response: " + message);
    } 
    else {
      result = 0;
    }
    return result;
  }

  void CrestClient::checkFsException(std::string& method_name){
    if (m_mode == FILESYSTEM_MODE) {
      throw std::runtime_error("ERROR in " + method_name + " This methods is unsupported for FILESYSTEM mode");
    }
    return;
  }

  nlohmann::json CrestClient::getResources(nlohmann::json& js) {
    nlohmann::json js2 = getJson("[]");
    nlohmann::json result = js.value("resources", js2);
    return result;
  }

  nlohmann::json CrestClient::getResFirst(nlohmann::json& js) {
    nlohmann::json jNull = getJson("[]");
    nlohmann::json res = js.value("resources", jNull);
    nlohmann::json result;
    try {
      result = res.at(0);
    }
    catch (...) {
      nlohmann::json result0 = nullptr;
      return result0;
    }
    return result;
  }

  std::string CrestClient::getTagMetaInfoElement(const std::string& name, nlohmann::json& js) {
    std::string result = "";
    int array_length = js.size();
    for (int i = 0; i < array_length; i++) {
      nlohmann::json elem = js[i];
      if (elem.find(name) != elem.end()) {
        return elem[name];
      }
    }
    return result;
  }

  nlohmann::json CrestClient::convertTagMetaInfo2IOVDbSvc(nlohmann::json& js) {
    std::string jsName = "tagInfo";
    std::string method_name = "CrestClient::convertTagMetaInfo2IOVDbSvc";
    nlohmann::json res = getJson(js[jsName], method_name);
    return res;
  }

  nlohmann::json CrestClient::convertTagMetaInfo2CREST(nlohmann::json& js) {
    nlohmann::json channel_list = getJson("[]");
    nlohmann::json channel_names = getJson("[]");
    nlohmann::json payload_specification;

    int colsize = 0;
    std::string tagInfo = js.dump();
    std::string description = "";


    nlohmann::json result;
    result["tagInfo"] = tagInfo;


    int array_length = js.size();
    for (int i = 0; i < array_length; i++) {
      nlohmann::json elem = js[i];

      if (elem.find("node_description") != elem.end()) {
        description = elem["node_description"];
        result["description"] = description;
      } else if (elem.find("channel_list") != elem.end()) {
        channel_list = elem["channel_list"];
        int chansize = channel_list.size();
        result["chansize"] = chansize;
      } else if (elem.find("channel_names") != elem.end()) {
        channel_names = elem["channel_names"];
      } else if (elem.find("payload_specification") != elem.end()) {
        payload_specification = elem["payload_specification"];
        try{
          colsize = split(payload_specification, ",").size();
        }
        catch (...) {
          colsize = 0;
        }
        result["colsize"] = colsize;
      }
    }

    return result;
  }

  std::vector<std::string> CrestClient::split(const std::string& str, const std::string& delim) {
    std::vector<std::string> tokens;
    std::size_t prev = 0, pos = 0;
    do {
      pos = str.find(delim, prev);
      if (pos == std::string::npos) pos = str.length();
      std::string token = str.substr(prev, pos - prev);
      if (!token.empty()) tokens.push_back(token);
      prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
  }

// Tag Meta Info Methods

  void CrestClient::createTagMetaInfo(const std::string& tagname, nlohmann::json& js) {
    std::string method_name = "CrestClient::createTagMetaInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_TAG_PATH + "/" + tagname + s_META_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

  }

  nlohmann::json CrestClient::getTagMetaInfo(const std::string& tagname) {
    std::string method_name = "CrestClient::getTagMetaInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_TAG_PATH + "/" + tagname + s_META_PATH;
    std::string getTxt = "GET";

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::updateTagMetaInfo(const std::string& tagname, nlohmann::json& js) {
    std::string method_name = "CrestClient::updateTagMetaInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_TAG_PATH + "/" + tagname + s_META_PATH;

    std::string retv;

    retv = performRequest(current_path, PUT, js, method_name);

  }

// Tag Meta Info Methods (IOVDbSvc format)

  nlohmann::json CrestClient::getTagMetaInfoIOVDbSvc(const std::string& tagname) {
    std::string method_name = "CrestClient::updateTagMetaInfo";

    checkFsException(method_name);

    nlohmann::json js = getTagMetaInfo(tagname);

    nlohmann::json resource = js.at(0); 

    nlohmann::json result = convertTagMetaInfo2IOVDbSvc(resource);

    return result;
  }

  void CrestClient::createTagMetaInfoIOVDbSvc(const std::string& tagname, nlohmann::json& js) {
    std::string method_name = "CrestClient::createTagMetaInfoIOVDbSvc";

    checkFsException(method_name);

    nlohmann::json arg = convertTagMetaInfo2CREST(js);
    arg["tagName"] = tagname;
    createTagMetaInfo(tagname, arg);

    return;
  }


 } // namespace 

