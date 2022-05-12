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

#include <fstream>
#include <filesystem>
#include <iostream>

#include <sstream>

#include <CrestApi/picosha2.h>

#include <stdio.h>
#include <time.h>

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

  CrestClient::CrestClient(std::string_view url) : m_mode(SERVER_MODE) {
    size_t found = url.find_first_of(':');

    std::string_view url_new = url.substr(found + 3); //url_new is the url excluding the http part
    size_t found1 = url_new.find_first_of(':');
    size_t found2 = url_new.find_first_of('/');
    std::string_view host;
    std::string_view port;
    if (found1 != std::string::npos && found2 != std::string::npos) {
      host = url_new.substr(0, found1);
      port = url_new.substr(found1 + 1, found2 - found1 - 1);
    } else if (found1 != std::string::npos) {
      host = url_new.substr(0, found1);
      port = url_new.substr(found1 + 1);
    } else if (found2 != std::string::npos) {
      port = "80";
      host = url_new.substr(0, found2);
    } else {
      port = "80";
      host = url_new;
    }
    m_port = std::string(port);
    m_host = std::string(host);
    std::cout << "host=" << m_host << " port" << m_port << std::endl;
  }

  CrestClient::~CrestClient() {
    flush();
  }

//==================================
//  TAG METHODS
//

  nlohmann::json CrestClient::listTags() {
    const char* method_name = "CrestClient::listTags";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_TAG_PATH;

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::listTags(int size, int page) {
    const char* method_name = "CrestClient::listTags";

    checkFsException(method_name);

    int size_default = 1000;
    int page_default = 0;
    std::string size_param = "";
    std::string page_param = "";
    std::string params = "";

    if (size == size_default && page == page_default) {
      nlohmann::json respond = listTags();
      return getResources(respond);
    }

    if (size != size_default) {
      size_param = "size=";
      size_param += std::to_string(size);
      params += size_param;
    }
    if (page != page_default) {
      page_param = "page=";
      page_param += std::to_string(page);
      if (page_param.empty()) {
        params += page_param;
      } else {
        params += '&';
        params += page_param;
      }
    }

    std::string current_path = s_PATH;
    current_path += s_TAG_PATH;
    current_path += '?';
    current_path += params;
    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::listTags(const std::string& name, int size, int page, const std::string& sort) {
    const char* method_name = "CrestClient::listTags";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_TAG_PATH;

    if (!name.empty()) {
      std::string nameString = "?by=name:";
      nameString += name;
      current_path += nameString;
      current_path += "&size=";
      current_path += std::to_string(size);
      current_path += "&page=";
      current_path += std::to_string(page);
      current_path += "&sort=";
      current_path += sort;
    } else {
      current_path += "?size=";
      current_path += std::to_string(size);
      current_path += "&page=";
      current_path += std::to_string(page);
      current_path += "&sort=";
      current_path += sort;
    }

    nlohmann::json js = nullptr;
    std::string retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::removeTag(const std::string& tagName) {
    const char* method_name = "removeTag";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_ADMIN_PATH;
    current_path += s_TAG_PATH;
    current_path += '/';
    current_path += tagName;
    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, DELETE, js, method_name);
  }

  nlohmann::json CrestClient::findTag(const std::string& tagName) {
    const char* method_name = "findTag";

    if (m_mode == FILESYSTEM_MODE) {
      return findTagFs(tagName);
    }

    std::string current_path = s_PATH + s_TAG_PATH + '/' + tagName;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::createTag(const std::string& name, const std::string& desc, const std::string& timeType) {
    //const char* method_name = "CrestClient::createTag";

    nlohmann::json js2 = {{"description", desc}, {"endOfValidity", 0},
                          {"lastValidatedTime", 0}, {"synchronization", "none"}, {"payloadSpec", "json"},
                          {"name", name}, {"timeType", timeType}};
    createTag(js2);
  }

  void CrestClient::createTag(const std::string& name, const std::string& desc, const std::string& timeType,
                              const std::string& payloadSpec) {
    //const char* method_name = "CrestClient::createTag";

    nlohmann::json js2 = {{"description", desc}, {"endOfValidity", 0},
                          {"lastValidatedTime", 0}, {"synchronization", "none"}, {"payloadSpec", payloadSpec},
                          {"name", name}, {"timeType", timeType}};

    createTag(js2);
  }

  void CrestClient::createTag(nlohmann::json& js) {
    const char* method_name = "CrestClient::createTag";

    if (m_mode == FILESYSTEM_MODE) {
      createTagDump(js);
      return;
    }

    std::string current_path = s_PATH;
    current_path += s_TAG_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);
  }

  void CrestClient::updateTag(const std::string& tagname, nlohmann::json body) {
    const char* method_name = "CrestClient::updateTag";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_TAG_PATH;
    current_path += '/';
    current_path += tagname;

    std::string retv;

    retv = performRequest(current_path, PUT, body, method_name);
  }

  void CrestClient::updateTagSpecification(const std::string& tagname, const std::string& objectType) {
    const char* method_name = "CrestClient::updateTagSpecification";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_TAG_PATH;
    current_path += '/';
    current_path += tagname;

    nlohmann::json body;
    body["objectType"] = objectType;


    std::string retv;
    retv = performRequest(current_path, PUT, body, method_name);
  }

  void CrestClient::createTagDump(nlohmann::json& js) {
    std::string name = js["name"];

    std::string rootDir = m_root_folder;
    rootDir += s_FS_TAG_PATH;
    if (!std::filesystem::exists(std::filesystem::path(rootDir))) {
      std::filesystem::create_directory(std::filesystem::path(rootDir));
    }

    std::string workDir = m_root_folder;
    workDir += s_FS_TAG_PATH;
    workDir += '/';
    workDir += name;
    if (!std::filesystem::exists(std::filesystem::path(workDir))) {
      std::filesystem::create_directory(std::filesystem::path(workDir));
    }
    if (name.compare(m_currentTag) != 0 && m_isRewrite) {
      flush();
    }
    m_currentTag = name;
    std::string tagFile = workDir + s_FS_TAG_FILE;
    std::string iovFile = workDir + s_FS_IOV_FILE;

    if (m_isRewrite) {
      if (std::filesystem::exists(std::filesystem::path(tagFile))) {
        std::filesystem::remove(std::filesystem::path(tagFile));
      }
      if (std::filesystem::exists(std::filesystem::path(iovFile))) {
        std::filesystem::remove(std::filesystem::path(iovFile));
      }
      std::ofstream myfile;
      myfile.open(tagFile.c_str());
      myfile << js;
      myfile.close();
    }
    if (m_data.find(name) == m_data.end()) {
      m_data.insert(std::pair<std::string, nlohmann::json>(name, nlohmann::json(nlohmann::json::value_t::array)));
    }
  }

  void CrestClient::flush() {
    if (m_mode == SERVER_MODE) return;

    std::cerr << "Flush start" << std::endl;

    for (auto& item : m_data) {
      nlohmann::json iov = item.second;
      std::string name = item.first;
      std::string workDir = m_root_folder + s_FS_TAG_PATH + '/' + name;
      std::ofstream myfile;
      std::string tagFile = workDir + s_FS_IOV_FILE;
      myfile.open(tagFile.c_str());
      myfile << iov;
      myfile.close();
    }

    m_data.clear();
  }

// IOVs

  void CrestClient::createIov(nlohmann::json& js) {
    const char* method_name = "CrestClient::createIov";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);
  }

  nlohmann::json CrestClient::findAllIovs(const std::string& tagname) {
    const char* method_name = "CrestClient::findAllIovs";

    if (m_mode == FILESYSTEM_MODE) {
      return findAllIovsFs(tagname);
    }

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += "?by=tagname:";
    current_path += tagname;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::findAllIovs(const std::string& tagname, int size, int page, const std::string& sort,
                                          const std::string& dateformat) {
    const char* method_name = "CrestClient::findAllIovs";

    if (m_mode == FILESYSTEM_MODE) {
      return findAllIovsFs(tagname);
    }

    // http request examples:
    // http://crest-01.cern.ch:8080/crestapi/iovs?by=tagname:test_MvG3b&size=1&page=1
    // http://crest-02.cern.ch:8090/crestapi/tags?size=10&page=2&sort=name:DESC

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += "?by=tagname:";
    current_path += tagname;
    current_path += "&size=";
    current_path += std::to_string(size);
    current_path += "&page=";
    current_path += std::to_string(page);
    current_path += "&sort=";
    current_path += sort;
    current_path += "&dateformat=";
    current_path += dateformat;


    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::getSizeByTag(const std::string& tagname) {
    const char* method_name = "CrestClient::getSizeByTag";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_SIZE_PATH_FOR_TAG;
    current_path += "?tagname=";
    current_path += tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);


    nlohmann::json respond = getJson(retv, method_name);
    nlohmann::json res = getResources(respond)[0];

    int result = 0;

    if (respond.find("size") != respond.end()) {
      int sizeR = respond["size"];
      if (sizeR == 0) return result;
    }

    if (res.find("niovs") != res.end()) {
      result = res["niovs"];
    } else {
      throw std::runtime_error("ERROR in CrestClient::getSizeByTag CREST Server JSON response has no \"size\" key.");
    }

    return result;
  }

  int CrestClient::getSize(const std::string& tagname) {
    const char* method_name = "CrestClient::getSize";

    if (m_mode == FILESYSTEM_MODE) {
      return getSizeFS(tagname);
    }

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_SIZE_PATH;
    current_path += "?tagname=";
    current_path += tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;
    int res = 0;

    retv = performRequest(current_path, GET, js, method_name);
    js = getJson(retv);

    if (js.find("size") != js.end()) {
      res = js["size"];
    } else {
      throw std::runtime_error("ERROR in CrestClient::getSize CREST Server JSON response has no \"size\" key.");
    }

    return res;
  }

  nlohmann::json CrestClient::selectIovs(const std::string& tagname) {
    const char* method_name = "CrestClient::selectIovs";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_SELECT_PATH;
    current_path += "?tagname=";
    current_path += tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectIovs(const std::string& tagname, long snapshot) {
    const char* method_name = "CrestClient::selectIovs";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_SELECT_PATH;
    current_path += "?tagname=";
    current_path += tagname;
    current_path += "&snapshot=";
    current_path += std::to_string(snapshot); //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectIovs(const std::string& tagname, long since, long until, long snapshot) {
    const char* method_name = "CrestClient::selectIovs";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_SELECT_PATH;
    current_path += "?tagname=";
    current_path += tagname;
    current_path += "&since=";
    current_path += std::to_string(since);
    current_path += "&until=";
    if (until == -1) {
      current_path += "INF";
    } else {
      current_path += std::to_string(until);
    }
    current_path += "&snapshot=";
    current_path += std::to_string(snapshot);

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectGroups(const std::string& tagname) {
    const char* method_name = "CrestClient::selectGroups";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_GROUP_PATH;
    current_path += "?tagname=";
    current_path += tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectGroups(const std::string& tagname, long snapshot) {
    const char* method_name = "CrestClient::selectGroups";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_GROUP_PATH;
    current_path += "?tagname=";
    current_path += tagname;
    current_path += "&snapshot=";
    current_path += std::to_string(snapshot); //return json

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectSnapshot(const std::string& tagname) {
    const char* method_name = "CrestClient::selectSnapshot";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_SNAPSHOT_PATH;
    current_path += "?tagname=";
    current_path += tagname; //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::selectSnapshot(const std::string& tagname, long snapshot) {
    const char* method_name = "CrestClient::selectSnapshot";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_IOV_PATH;
    current_path += s_IOV_SNAPSHOT_PATH;
    current_path += "?tagname=";
    current_path += tagname;
    current_path += "&snapshot=";
    current_path += std::to_string(snapshot); //return json

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

// GLOBALTAGS

  void CrestClient::updateGlobalTag(const std::string& name, nlohmann::json body) {
    const char* method_name = "CrestClient::updateGlobalTag";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_ADMIN_PATH;
    current_path += s_GLOBALTAG_PATH;
    current_path += '/';
    current_path += name;

    std::string retv;

    retv = performRequest(current_path, PUT, body, method_name);
  }

  void CrestClient::createGlobalTag(nlohmann::json& js) {
    const char* method_name = "CrestClient::createGlobalTag";

    if (m_mode == FILESYSTEM_MODE) {
      createGlobalTagFs(js);
      return;
    }

    std::string current_path = s_PATH;
    current_path += s_GLOBALTAG_PATH;
    std::string retv;
    retv = performRequest(current_path, POST, js, method_name);
  }

  void CrestClient::createGlobalTag(const std::string& tag) {
    const char* method_name = "CrestClient::createGlobalTag";

    nlohmann::json js = getJson(tag, method_name);
    createGlobalTag(js);
  }

  void CrestClient::createGlobalTag(const std::string& tagname, const std::string& description) {
    //const char* method_name = "CrestClient::createGlobalTag";

    nlohmann::json js =
    {
      {"name", tagname},
      {"description", description},
      {"validity", 0},
      {"release", 1},
      {"scenario", "undefined"},
      {"workflow", "undefined"},
      {"type", "UPD"}
    };

    createGlobalTag(js);
  }

  std::string CrestClient::findGlobalTagAsString(const std::string& name) {
    const char* method_name = "CrestClient::findGlobalTagAsString";

    if (m_mode == FILESYSTEM_MODE) {
      nlohmann::json j = findGlobalTagFs(name);
      std::string s = j.dump();
      return s;
    }

    std::string current_path = s_PATH;
    current_path += s_GLOBALTAG_PATH;
    current_path += '/';
    current_path += name;
    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }

  nlohmann::json CrestClient::findGlobalTag(const std::string& name) {
    const char* method_name = "CrestClient::findGlobalTag";

    if (m_mode == FILESYSTEM_MODE) {
      return findGlobalTagFs(name);
    }

    std::string retv = findGlobalTagAsString(name);
    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::removeGlobalTag(const std::string& name) {
    const char* method_name = "CrestClient::removeGlobalTag";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_ADMIN_PATH;
    current_path += s_GLOBALTAG_PATH;
    current_path += '/';
    current_path += name;
    std::string retv;
    nlohmann::json js = nullptr;
    retv = performRequest(current_path, DELETE, js, method_name);
  }

  nlohmann::json CrestClient::listGlobalTags() {
    const char* method_name = "CrestClient::listGlobalTags";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_GLOBALTAG_PATH;
    std::string retv;
    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);
    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  std::string CrestClient::listGlobalTagsAsString() {
    const char* method_name = "CrestClient::listGlobalTagsAsString";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_GLOBALTAG_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }

  nlohmann::json CrestClient::listGlobalTags(const std::string& name, int size, int page, const std::string& sort) {
    const char* method_name = "CrestClient::listGlobalTags";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_GLOBALTAG_PATH;
    if (!name.empty()) {
      std::string nameString = "?by=name:";
      nameString += name;
      current_path += nameString;
      current_path += "&size=";
      current_path += std::to_string(size);
      current_path += "&page=";
      current_path += std::to_string(page);
      current_path += "&sort=";
      current_path += sort;
    } else {
      current_path += "?size=";
      current_path += std::to_string(size);
      current_path += "&page=";
      current_path += std::to_string(page);
      current_path += "&sort=";
      current_path += sort;
    }

    std::string retv;
    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);
    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

//=============================
// GLOBALTAGM MAPs

  nlohmann::json CrestClient::findGlobalTagMap(const std::string& name) {
    const char* method_name = "CrestClient::findGlobalTagMap";

    if (m_mode == FILESYSTEM_MODE) {
      return findGlobalTagMapFs(name);
    }

    std::string current_path = s_PATH;
    current_path += s_GLOBALTAG_MAP_PATH;
    current_path += '/';
    current_path += name;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::createGlobalTagMap(nlohmann::json& js) {
    const char* method_name = "CrestClient::createGlobalTagMap";

    if (m_mode == FILESYSTEM_MODE) {
      createGlobalTagMapFs(js);
      return;
    }

    std::string current_path = s_PATH;
    current_path += s_GLOBALTAG_MAP_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);
  }

  void CrestClient::createGlobalTagMap(const std::string& globaltag, const std::string& tagname,
                                       const std::string& record, const std::string& label) {
    const char* method_name = "CrestClient::createGlobalTagMap";

    nlohmann::json js =
    {
      {"globalTagName", globaltag},
      {"record", record},
      {"label", label},
      {"tagName", tagname}
    };

    if (m_mode == FILESYSTEM_MODE) {
      createGlobalTagMapFs(js);
      return;
    }

    std::string current_path = s_PATH + s_GLOBALTAG_MAP_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);
  }

//==============================
//BLOBS & PAYLOADS

  std::string CrestClient::getBlob(const std::string& hash) {
    const char* method_name = "CrestClient::getBlob";

    if (m_mode == FILESYSTEM_MODE) {
      return getBlobFs(hash);
    }

    std::string current_path = s_PATH;
    current_path += s_PAYLOAD_PATH;
    current_path += '/';
    current_path += hash;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }

  std::string CrestClient::getBlobInStream(const std::string& hash, std::ofstream& out) {
    //const char* method_name = "CrestClient::getBlobInStream";
    if (m_mode == FILESYSTEM_MODE) {
      return getBlobInStreamFs(hash, out);
    }

    std::string current_path = s_PATH;
    current_path += s_PAYLOAD_PATH;
    current_path += '/';
    current_path += hash;

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, out);

    return retv;
  }

// PAYLOADS

  nlohmann::json CrestClient::getPayloadMetaInfo(const std::string& hash) {
    const char* method_name = "CrestClient::getPayloadMetaInfo";

    if (m_mode == FILESYSTEM_MODE) {
      return getPayloadMetaInfoAsJsonFS(hash);
    }

    std::string current_path = s_PATH;
    current_path += s_PAYLOAD_PATH;
    current_path += '/';
    current_path += hash;
    current_path += s_META_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  std::string CrestClient::getPayloadMetaInfoAsString(const std::string& hash) {
    const char* method_name = "CrestClient::getPayloadMetaInfoAsString";

    if (m_mode == FILESYSTEM_MODE) {
      return getPayloadMetaInfoAsStringFS(hash);
    }

    std::string current_path = s_PATH;
    current_path += s_PAYLOAD_PATH;
    current_path += '/';
    current_path += hash;
    current_path += s_META_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }

  nlohmann::json CrestClient::listPayloadTagInfo(const std::string& tagname) {
    const char* method_name = "CrestClient::getPayloadTagInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_MONITORING_PAYLOAD_PATH;
    current_path += "?tagname=";
    current_path += tagname;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::listPayloadTagInfo() {
    const char* method_name = "CrestClient::getPayloadTagInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_MONITORING_PAYLOAD_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  nlohmann::json CrestClient::getPayloadAsJson(const std::string& hash) {
    const char* method_name = "CrestClient::getPayloadAsJson";

    if (m_mode == FILESYSTEM_MODE) {
      return getPayloadAsJsonFS(hash);
    }

    std::string current_path = s_PATH;
    current_path += s_PAYLOAD_PATH;
    current_path += '/';
    current_path += hash;
    current_path += "?format=DTO";

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return respond;
  }

  std::string CrestClient::getPayloadAsString(const std::string& hash) {
    const char* method_name = "CrestClient::getPayloadAsString";

    if (m_mode == FILESYSTEM_MODE) {
      return getPayloadAsStringFS(hash);
    }

    std::string current_path = s_PATH;
    current_path += s_PAYLOAD_PATH;
    current_path += '/';
    current_path += hash;
    current_path += "?format=BLOB";

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    return retv;
  }

  void CrestClient::createPayload(nlohmann::json& js) {
    const char* method_name = "CrestClient::createPayload";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_PAYLOAD_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);
  }

// METHODS to store the PAYLOADS in BATCH mode

  void CrestClient::storeBatchPayloads(const std::string& tag_name, uint64_t endtime, const std::string& iovsetupload) {
    const char* method_name = "CrestClient::storeBatchPayloads";

    nlohmann::json js = getJson(iovsetupload, method_name);
    storeBatchPayloads(tag_name, endtime, js);
  }

  void CrestClient::storeBatchPayloads(const std::string& tag_name, uint64_t endtime, nlohmann::json& js) {
    const char* method_name = "CrestClient::storeBatchPayloads";

    if (m_mode == FILESYSTEM_MODE) {
      storeBatchPayloadsFs(tag_name, js);
      return;
    }
    if (!js.is_array()) {
      throw std::runtime_error("ERROR in " + std::string(method_name) + " JSON has wrong type (must be array)");
    }

    nlohmann::json jsObj = {};
    jsObj["datatype"] = "iovs";
    jsObj["format"] = "IovSetDto";
    jsObj["size"] = js.size();
    jsObj["resources"] = js;
    std::string str = jsObj.dump();
    std::string retv = storeBatchPayloadRequest(tag_name, endtime, str);

    nlohmann::json respond = getJson(retv, method_name);
  }

  void CrestClient::storeBatchPayloads(const std::string& tag_name, const std::string& iovsetupload, uint64_t endtime) {
    const char* method_name = "CrestClient::storeBatchPayloads";

    nlohmann::json js = getJson(iovsetupload, method_name);
    storeBatchPayloads(tag_name, endtime, js);
  }

  void CrestClient::storeBatchPayloads(const std::string& tag_name, nlohmann::json& js, uint64_t endtime) {
    const char* method_name = "CrestClient::storeBatchPayloads";

    if (m_mode == FILESYSTEM_MODE) {
      storeBatchPayloadsFs(tag_name, js);
      return;
    }
    if (!js.is_array()) {
      throw std::runtime_error("ERROR in " + std::string(method_name) + " JSON has wrong type (must be array)");
    }

    nlohmann::json jsObj = {};
    jsObj["datatype"] = "iovs";
    jsObj["format"] = "IovSetDto";
    jsObj["size"] = js.size();
    jsObj["resources"] = js;
    std::string str = jsObj.dump();
    std::string retv = storeBatchPayloadRequest(tag_name, endtime, str);

    nlohmann::json respond = getJson(retv, method_name);
  }

//=========================================
// FOLDERS

  nlohmann::json CrestClient::listFolders() {
    const char* method_name = "CrestClient::listFolders";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_FOLDER_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  std::string CrestClient::createFolder(nlohmann::json& body) {
    const char* method_name = "CrestClient::createFolder";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_FOLDER_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, body, method_name);

    return retv;
  }

//==========================================
// RUN INFO

  nlohmann::json CrestClient::listRunLumiInfo() {
    const char* method_name = "CrestClient::listRunLumiInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_RUNINFO_PATH;

    std::string retv;

    nlohmann::json js = nullptr;

    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::createRunLumiInfo(nlohmann::json& body) {
    const char* method_name = "CrestClient::createRunLumiInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH + s_RUNINFO_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, body, method_name);
  }

  nlohmann::json CrestClient::findRunLumiInfo(urlParameters params) {
    const char* method_name = "CrestClient::findRunLumiInfo";

    checkFsException(method_name);

    std::string current_path = s_PATH;
    current_path += s_RUNINFO_PATH;
    current_path += s_RUNINFO_LIST_PATH;
    current_path += '?';
    current_path += params.getParams();

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
      url << "http://" << m_host << ':' << m_port << current_path;

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
      const char* method_name = "CrestClient::storePayload";

      /* always cleanup */
      curl_easy_cleanup(curl);
      curl_formfree(formpost);
      curl_slist_free_all(headers);

      curl_global_cleanup();

      // error checking in the server response:
      checkResult(res, response_code, s, method_name);

      return s;
    }

    throw std::runtime_error("ERROR in CrestClient::storePayload | CURL not init");
  }

  std::string CrestClient::performRequest(const std::string& current_path, Action action, nlohmann::json& js,
                                          const char* method_name) {
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
      url << "http://" << m_host << ':' << m_port << current_path;

      curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
      if (js.is_null()) {
        if (action == DELETE) curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
      } else {
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

      /* always cleanup */
      curl_easy_cleanup(curl);
      curl_slist_free_all(headers);

      curl_global_cleanup();

      // error checking in the server response:
      checkResult(res, response_code, s, method_name);

      return s;
    }

    throw std::runtime_error(std::string("ERROR in ") + std::string(method_name) + " | CURL not init");
  }

// REQUEST OLD VERSION

  std::string CrestClient::performRequest(const std::string& current_path, Action action, nlohmann::json& js) {
    const char* method = "Unknown";

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
        request_stream << "Host: " << m_host << ':' << m_port << " \r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";
      } else {
        std::string s = js.dump();
        request_stream << action << " " << current_path << " HTTP/1.0\r\n";
        request_stream << "Host: " << m_host << ':' << m_port << " \r\n";
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
      while (std::getline(response_stream, header) && header != "\r") {
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

  //
  std::string CrestClient::storeBatchPayloadRequest(const std::string& tag, uint64_t endtime, const std::string& js) {
    std::string_view current_path = "/crestapi/payloads/storebatch";

    CURL* curl;
    CURLcode res;

    struct curl_httppost* formpost = NULL;
    struct curl_httppost* lastptr = NULL;

    // get a curl handle

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    struct curl_slist* headers = NULL;
    if (curl) {
      std::ostringstream url;
      std::string s;
      url << "http://" << m_host << ':' << m_port << current_path;


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
      if (endtime != 0) {
        curl_formadd(&formpost,
                     &lastptr,
                     CURLFORM_COPYNAME, "endtime",
                     CURLFORM_COPYCONTENTS, std::to_string(endtime).c_str(),
                     CURLFORM_END);
      }
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
      const char* method_name = "CrestClient::storePayload";

      /* always cleanup */
      curl_easy_cleanup(curl);
      curl_formfree(formpost);
      curl_slist_free_all(headers);

      curl_global_cleanup();

      // error checking in the server response:
      checkResult(res, response_code, s, method_name);

      return s;
    }
    std::string mes = "ERROR in CrestClient::storeBatchPayloads";
    throw std::runtime_error(mes + " | CURL not init");
  }

  //

// end of REQUEST METHODS

//==============================
// UTILITIES


  nlohmann::json CrestClient::getJson(const std::string& str, const char* method) {
    try {
      nlohmann::json js = nlohmann::json::parse(str);
      return js;
    }
    catch (nlohmann::json::parse_error& e) {
      if (method == nullptr || *method == '\0') {
        // method name is undefined

        std::string wh = e.what();
        throw std::runtime_error("ERROR in JSON conversion: " + wh + " | In string: " + str);
      } else {
        std::string str2 = ParseXMLOutput(str);  // to remove HTML tags use this function
        std::string str3 = removeCR(str2); // to remove carridge return
        throw std::runtime_error("ERROR in " + std::string(method) + " | CREST Server response : " + str3);
      }
    }
  }

  nlohmann::json CrestClient::getJson(const std::string& str) {
    return getJson(str, "");
  }

  // The auxillary method to remove XML/HTML tags from a std::string

  std::string CrestClient::ParseXMLOutput(std::string_view xmlBuffer) {
    bool copy = true;

    std::string plainString = "";

    // remove all xml tags
    for (long unsigned int i = 0; i < xmlBuffer.length(); i++) {
      char convertc = xmlBuffer[i];

      if (convertc == '<') copy = false;
      else if (convertc == '>') {
        copy = true;
        continue;
      }

      if (copy) plainString += convertc;
    }

    return plainString;
  }

  std::string CrestClient::removeCR(const std::string& str) {
    std::string str2 = str;
    std::replace(str2.begin(), str2.end(), '\n', '|');
    char needle = '\r';
    size_t pos;
    while ((pos = str2.find(needle)) != str2.npos) {
      str2.erase(pos, 1);
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

  void CrestClient::checkResult(CURLcode res, const long response_code, const std::string& st,
                                const char* method_name) {
    // Bad HTTP response:

    if (res != CURLE_OK) {
      std::string mes = "ERROR in ";
      mes += method_name;
      mes += " | ";
      throw std::runtime_error(mes + std::to_string(response_code));
    }

    // Errors, decoded from JSON CREST Server messages:

    if (isJson(st)) {
      nlohmann::json respond = getJson(st);
      checkErrors(respond, method_name);
    }

    // HTTP response code error interval:

    if (response_code >= 400 || response_code == 303) {
      std::string s = ParseXMLOutput(st);  // to remove HTML tags
      s = removeCR(s);        // to remove end lines and carridge returns
      std::string mes = "ERROR in ";
      mes += method_name;
      mes += " | ";
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
    std::string file_path = m_root_folder;
    file_path += '/';
    file_path += s_FS_TAG_PATH;
    file_path += '/';
    file_path += name;
    file_path += s_FS_TAG_FILE;
    try{
      std::string tag = getFileString(file_path);
      js = nlohmann::json::parse(tag);
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::findTagFs cannot get the tag information form file storage.");
    }

    return returnJArray(js);
  }

  nlohmann::json CrestClient::findAllIovsFs(const std::string& tagname) {
    nlohmann::json js = nullptr;
    std::string file_path = m_root_folder;
    file_path += '/';
    file_path += s_FS_TAG_PATH;
    file_path += '/';
    file_path += tagname;
    file_path += s_FS_IOV_FILE;

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
    int size = path.size();
    int pos = path.find(':');

    return path.substr(pos + 3, size);
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

  void CrestClient::storeBatchPayloadsFs(const std::string& tag_name, std::string& iovsetupload) {
    const char* method_name = "CrestClient::storeBatchPayloadsFs";

    nlohmann::json js = getJson(iovsetupload, method_name);
    if (!js.is_array()) {
      throw std::runtime_error("ERROR in CrestClient::storeBatchPayloadsFs: JSON has wrong type (must be array");
    }

    storeBatchPayloadsFs(tag_name, js);
  }

  void CrestClient::storeBatchPayloadsFs(const std::string& tag_name, nlohmann::json& js) {
    const char* method_name = "CrestClient::storeBatchPayloadsFs";

    try {
      for (auto& kvp : js) {
        std::string payload = kvp.value("payloadHash", "");
        int since = kvp.value("since", 0);
        storePayloadDump(tag_name, since, payload);
      }
    } // end of try
    catch (...) {
      throw std::runtime_error("ERROR in " + std::string(method_name) + " cannot store the data in a file");
    } // end of catch
    flush(); // MvG
  }

  // storeBatchPayloadsFs (end)


  std::string CrestClient::getEnvA(const char* varname) {
    std::string respond = "";

    char* pPath;
    const char* c = varname;
    pPath = std::getenv(c);
    if (pPath != NULL) respond = std::string(pPath);
    return respond;
  }

  std::string CrestClient::getDataPath() {
    const char* varName = "DAT_PATH";

    std::string respond = getEnvA(varName);
    respond += "/data/crestapi";
    return respond;
  }

  int CrestClient::checkErrors(const nlohmann::json& js, const char* method) {
    int result = 0;
    auto res = js.find("type");

    // Old format parsing:

    if (res != js.end()) {
      std::string type = js.value("type", " unknown type ");
      if (type == "error" || type == "info") {
        std::string message = js.value("message", " unknown message ");
        throw std::runtime_error("ERROR in " + std::string(method) + " | CREST response: " + message);
      }
    }

    // New format parsing:

    auto r1 = js.find("error");
    auto r2 = js.find("message");

    if (r1 != js.end()) {
      if (r2 != js.end()) {
        std::string message = js.value("message", " unknown message ");
        throw std::runtime_error("ERROR in " + std::string(method) + " | CREST response: " + message);
      }
    }
    return result;
  }

  void CrestClient::checkFsException(const char* method_name) {
    if (m_mode == FILESYSTEM_MODE) {
      throw std::runtime_error("ERROR in " + std::string(
                                 method_name) + " This methods is unsupported for FILESYSTEM mode");
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
      const nlohmann::json& elem = js[i];
      if (auto itr = elem.find(name); itr != elem.end()) {
        return *itr;
      }
    }
    return result;
  }

  nlohmann::json CrestClient::convertTagMetaInfo2IOVDbSvc(nlohmann::json& js) {
    std::string jsName = "tagInfo";
    const char* method_name = "CrestClient::convertTagMetaInfo2IOVDbSvc";
    nlohmann::json res = getJson(js[jsName], method_name);
    return res;
  }

  nlohmann::json CrestClient::convertTagMetaInfo2CREST(nlohmann::json& js) {
    int colsize = 0;

    std::string tagInfo = js.dump();

    nlohmann::json result;
    result["tagInfo"] = std::move(tagInfo);
    result["description"] = "none";

    std::string node_desc = js["node_description"];
    std::string payload_spec = js["payload_spec"];
    nlohmann::json chan_list = js["channel_list"];
    int chan_size = chan_list.size();
    result["chansize"] = chan_size;

    try{
      colsize = split(payload_spec, ',').size();
    }
    catch (...) {
      colsize = 0;
    }

    result["colsize"] = colsize;

    return result;
  }

  std::vector<std::string> CrestClient::split(std::string_view str, char delim) {
    std::vector<std::string> tokens;
    std::size_t prev = 0, pos = 0;
    do {
      pos = str.find(delim, prev);
      if (pos == std::string::npos) pos = str.length();
      std::string_view token = str.substr(prev, pos - prev);
      if (!token.empty()) tokens.emplace_back(token);
      prev = pos + 1;
    } while (pos < str.length() && prev < str.length());
    return tokens;
  }

  //====================================
  // Tag Meta Info Methods

  void CrestClient::createTagMetaInfo(nlohmann::json& js) {
    const char* method_name = "CrestClient::createTagMetaInfo";

    if (m_mode == FILESYSTEM_MODE) {
      createTagMetaInfoFs(js);
      return;
    }

    std::string tagname = "";
    try{
      tagname = static_cast<std::string> (js["tagName"]);
    }
    catch (...) {
      throw std::runtime_error(
              "ERROR in CrestClient::createTagMetaInfo cannot get the tag name from tag meta info JSON.");
    }

    std::string current_path = s_PATH;
    current_path += s_TAG_PATH;
    current_path += '/';
    current_path += tagname;
    current_path += s_META_PATH;

    std::string retv;

    retv = performRequest(current_path, POST, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);
  }

  void CrestClient::createTagMetaInfo(const std::string& tagname, nlohmann::json& js) {
    //const char* method_name = "CrestClient::createTagMetaInfo";
    std::string name = js["tagName"];
    if (tagname != name) {
      throw std::runtime_error("ERROR in CrestClient::createTagMetaInfo tagname in method and in JSON are different.");
    }
    createTagMetaInfo(js);
    return;
  }

  nlohmann::json CrestClient::getTagMetaInfo(const std::string& tagname) {
    const char* method_name = "CrestClient::getTagMetaInfo";

    if (m_mode == FILESYSTEM_MODE) {
      return getTagMetaInfoFs(tagname);
    }

    std::string current_path = s_PATH;
    current_path += s_TAG_PATH;
    current_path += '/';
    current_path += tagname;
    current_path += s_META_PATH;

    std::string retv;

    nlohmann::json js = nullptr;
    retv = performRequest(current_path, GET, js, method_name);

    nlohmann::json respond = getJson(retv, method_name);

    return getResources(respond);
  }

  void CrestClient::updateTagMetaInfo(nlohmann::json& js) {
    const char* method_name = "CrestClient::updateTagMetaInfo";

    std::string tagname = "";
    try{
      tagname = static_cast<std::string> (js["tagName"]);
    }
    catch (...) {
      throw std::runtime_error(
              "ERROR in CrestClient::updateTagMetaInfo cannot get the tag name from tag meta info JSON.");
    }


    if (m_mode == FILESYSTEM_MODE) {
      updateTagMetaInfoFs(tagname, js);
      return;
    }

    std::string current_path = s_PATH + s_TAG_PATH + '/' + tagname + s_META_PATH;

    std::string retv;

    retv = performRequest(current_path, PUT, js, method_name);
  }

  void CrestClient::updateTagMetaInfo(const std::string& tagname, nlohmann::json& js) {
    const char* method_name = "CrestClient::updateTagMetaInfo";

    if (m_mode == FILESYSTEM_MODE) {
      std::string name = "";
      try{
        name = static_cast<std::string> (js["tagName"]);
      }
      catch (...) {
        throw std::runtime_error(
                "ERROR in CrestClient::updateTagMetaInfo cannot get the tag name from tag meta info JSON.");
      }

      if (tagname != name) {
        throw std::runtime_error("ERROR in CrestClient::updateTagMetaInfo tagname in method and in JSON are different.");
      }

      updateTagMetaInfoFs(tagname, js);
      return;
    }

    std::string current_path = s_PATH + s_TAG_PATH + '/' + tagname + s_META_PATH;

    std::string retv;

    retv = performRequest(current_path, PUT, js, method_name);
  }

  //===============================================
  // Tag Meta Info Methods (IOVDbSvc format)

  nlohmann::json CrestClient::getTagMetaInfoIOVDbSvc(const std::string& tagname) {
    const char* method_name = "CrestClient::updateTagMetaInfo";

    checkFsException(method_name);

    nlohmann::json js = getTagMetaInfo(tagname);

    nlohmann::json resource = js.at(0);

    nlohmann::json result = convertTagMetaInfo2IOVDbSvc(resource);

    return result;
  }

  void CrestClient::createTagMetaInfoIOVDbSvc(const std::string& tagname, nlohmann::json& js) {
    const char* method_name = "CrestClient::createTagMetaInfoIOVDbSvc";

    checkFsException(method_name);

    nlohmann::json arg = convertTagMetaInfo2CREST(js);
    arg["tagName"] = tagname;

    createTagMetaInfo(arg);

    return;
  }

  void CrestClient::createTagMetaInfoIOVDbSvc(const std::string& tagname, nlohmann::json& js,
                                              const std::string& description) {
    const char* method_name = "CrestClient::createTagMetaInfoIOVDbSvc";

    checkFsException(method_name);

    nlohmann::json arg = convertTagMetaInfo2CREST(js);
    arg["tagName"] = tagname;
    arg["description"] = description;

    createTagMetaInfo(arg);

    return;
  }

  //=====================================
  // Tag Meta Info methods for FS

  void CrestClient::createTagMetaInfoFs(nlohmann::json& js) {
    std::string name = "";
    try{
      name = static_cast<std::string> (js["tagName"]);
    }
    catch (...) {
      throw std::runtime_error(
              "ERROR in CrestClient::createTagMetaInfoFs cannot get the tag name from tag meta info JSON.");
    }
    std::string workDir = m_root_folder;
    workDir += s_FS_TAG_PATH;
    workDir += '/';
    workDir += name;
    std::string tagMetaFile = workDir;
    tagMetaFile += s_FS_TAGMETAINFO_FILE;

    if (!std::filesystem::exists(std::filesystem::path(workDir))) {
      std::filesystem::create_directory(std::filesystem::path(workDir));
    }

    if (m_isRewrite) {
      if (std::filesystem::exists(std::filesystem::path(tagMetaFile))) {
        std::filesystem::remove(std::filesystem::path(tagMetaFile));
      }

      std::ofstream myfile;

      myfile.open(tagMetaFile.c_str());
      myfile << js;
      myfile.close();
    }

    return;
  }

  nlohmann::json CrestClient::getTagMetaInfoFs(const std::string& name) {
    nlohmann::json js = nullptr;
    std::string file_path = m_root_folder;
    file_path += s_FS_TAG_PATH;
    file_path += '/';
    file_path += name;
    file_path += s_FS_TAGMETAINFO_FILE;
    try{
      std::string tag = getFileString(file_path);
      js = nlohmann::json::parse(tag);
    }
    catch (...) {
      throw std::runtime_error(
              "ERROR in CrestClient::getTagMetaInfoFs cannot get the tag meta information form file storage.");
    }

    return returnJArray(js);
  }

  void CrestClient::updateTagMetaInfoFs(const std::string& tagname, nlohmann::json& js) {
    // Check if JSON contains the correct tag name:
    auto it = js.find("tagName");

    if (it != js.end()) {
      if (js["tagName"] != tagname) {
        throw std::runtime_error("ERROR in CrestClient::updateTagMetaInfoFs: JSON contains wrong tag name.");
      }
    }

    nlohmann::json jstmp;
    try{
      jstmp = getTagMetaInfoFs(tagname);
    }
    catch (const std::exception& e) {
      throw std::runtime_error(
              "ERROR in CrestClient::updateTagMetaInfoFs: the file storage does not contain the tag meta info for the tag " + tagname +
              " .");
    }

    // Write all keys/values from new JSON to old one:
    for (auto it = js.begin(); it != js.end(); it++) {
      jstmp[it.key()] = it.value();
    }

    try{
      createTagMetaInfoFs(jstmp);
    }
    catch (const std::exception& e) {
      throw std::runtime_error(
              "ERROR in CrestClient::updateTagMetaInfoFs: cannot write the tag meta info for the tag " + tagname +
              " on file storage.");
    }

    return;
  }

  // Tag Meta Info methods for FS (end)

  //================================================
  // Global Tag and Global Tag Maps methods:

  void CrestClient::createGlobalTagFs(nlohmann::json& js) {
    std::string name = "";
    try{
      name = static_cast<std::string>(js["name"]);
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::createGlobalTagFs: cannot get the global tag name from JSON.");
    }

    std::string gTagDir = m_root_folder;
    gTagDir += s_FS_GLOBALTAG_PATH;
    std::string workDir = gTagDir;
    workDir += '/';
    workDir += name;
    std::string globalTagFile = workDir;
    globalTagFile += s_FS_GLOBALTAG_FILE;


    if (!std::filesystem::exists(std::filesystem::path(gTagDir))) {
      std::filesystem::create_directory(std::filesystem::path(gTagDir));
    }

    if (!std::filesystem::exists(std::filesystem::path(workDir))) {
      std::filesystem::create_directory(std::filesystem::path(workDir));
    }

    if (m_isRewrite) {
      if (std::filesystem::exists(std::filesystem::path(globalTagFile))) {
        std::filesystem::remove(std::filesystem::path(globalTagFile));
      }

      std::ofstream myfile;

      myfile.open(globalTagFile.c_str());
      myfile << js;
      myfile.close();
    }

    return;
  }

  nlohmann::json CrestClient::findGlobalTagFs(const std::string& name) {
    nlohmann::json js = nullptr;


    std::string workDir = m_root_folder;
    workDir += s_FS_GLOBALTAG_PATH;
    workDir += '/';
    workDir += name;
    std::string file_path = workDir;
    file_path += s_FS_GLOBALTAG_FILE;

    try{
      std::string tag = getFileString(file_path);
      js = nlohmann::json::parse(tag);
    }
    catch (...) {
      throw std::runtime_error(
              "ERROR in CrestClient::getGlobalTagFs: cannot get the global tag " + name + " form the file storage.");
    }

    return returnJArray(js);
  }

  void CrestClient::createGlobalTagMapFs(nlohmann::json& js) {
    // global tag name:
    std::string name = "";
    try{
      name = static_cast<std::string>(js["globalTagName"]);
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::createGlobalTagMapFs: cannot get the global tag name from JSON.");
    }

    // tag name:
    std::string tagname = "";
    try{
      tagname = static_cast<std::string>(js["tagName"]);
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::createGlobalTagMapFs: cannot get the tag name from JSON.");
    }

    std::string fname = "/";
    fname += tagname;
    fname += ".json";


    std::string gTagDir = m_root_folder + s_FS_GLOBALTAG_PATH;
    std::string workDir = gTagDir;
    workDir += '/';
    workDir += name;


    if (!std::filesystem::exists(std::filesystem::path(workDir))) {
      std::filesystem::create_directory(std::filesystem::path(workDir));
    }

    if (!std::filesystem::exists(std::filesystem::path(workDir))) {
      std::filesystem::create_directory(std::filesystem::path(workDir));
    }


    std::string catalogFile = workDir + s_FS_MAP_FILE;

    if (std::filesystem::exists(std::filesystem::path(catalogFile))) {
      // cathalogue file exists:

      std::string array_lst = getFileString(catalogFile);
      nlohmann::json cathalogue = getJson(array_lst, "CrestClient::createGlobalTagMapFs");

      if (std::filesystem::exists(std::filesystem::path(catalogFile))) {
        // the file storage contains the record of the global tag map:
        int m = cathalogue.size();
        for (int i = 0; i < m; i++) {
          const std::string& tn = cathalogue[i]["tagName"];
          if (tn == tagname) {
            cathalogue.erase(i);
          }
        }
      } else {
        // the file storage does not contain the record of the global tag map:
      }
      std::filesystem::remove(std::filesystem::path(catalogFile));
      cathalogue.push_back(js);
      std::ofstream myfile;

      myfile.open(catalogFile.c_str());
      myfile << cathalogue;
      myfile.close();
    } else {
      // cathalogue file does not exist (creation):

      nlohmann::json cathalogue = nlohmann::json::array();
      cathalogue.push_back(js);

      std::ofstream myfile;

      myfile.open(catalogFile.c_str());
      myfile << cathalogue;
      myfile.close();
    }


    return;
  }

  nlohmann::json CrestClient::findGlobalTagMapFs(const std::string& name) {
    nlohmann::json js = nullptr;

    std::string workDir = m_root_folder + s_FS_GLOBALTAG_PATH;
    std::string file_path = workDir;
    file_path += '/';
    file_path += name;
    file_path += s_FS_MAP_FILE;

    try{
      std::string tag = getFileString(file_path);
      js = nlohmann::json::parse(tag);
    }
    catch (...) {
      throw std::runtime_error(
              "ERROR in CrestClient::getGlobalTagMapFs: cannot get the global tag map " + name +
              " form the file storage.");
    }

    return js;
  }

  // Global Tag and Global Tag Maps methods (end)


  //=====================================
  // Store Payload FS methods:


  std::string CrestClient::getHash(std::string_view str) {
    std::string hash_hex_str = picosha2::hash256_hex_string(str.begin(), str.end());
    return hash_hex_str;
  }

  void CrestClient::storePayloadDump(const std::string& tag, uint64_t since, const std::string& js) {
    std::ofstream myfile;



    // payload file:
    std::string hashCode = getHash(js);
    std::string workDir = m_data_folder;
    workDir += '/';
    workDir += hashCode;
    if (!std::filesystem::exists(std::filesystem::path(workDir))) {
      std::filesystem::create_directory(std::filesystem::path(workDir));
    }

    std::string tagFile = workDir + "/payload.json";

    myfile.open(tagFile);
    myfile << js;
    myfile.close();


    /*
       // meta info data example:
       {
        "hash": "9834876dcfb05cb167a5c24953eba58c4ac89b1adf57f28f2f9d09af107ee8f0",
        "insertionTime": "2020-12-08",
        "objectType": "JSON",
        "size": 3,
        "streamerInfo":
           "eyJmaWxlbmFtZSI6Ijk4MzQ4NzZkY2ZiMDVjYjE2N2E1YzI0OTUzZWJhNThjNGFjODliMWFkZjU3ZjI4ZjJmOWQwOWFmMTA3ZWU4ZjAiLCJmb3JtYXQiOiJKU09OIiwiaW5zZXJ0aW9uRGF0ZSI6IlR1ZSBEZWMgMDggMTE6MTg6NTAgR01UIDIwMjAifQ==",
        "version": "default"
       }
     */

    nlohmann::json jsn =
    {
      {"hash", hashCode},
      {"objectType", "JSON"},
      {"version", "default"},
      {"size", js.size()},
      {"streamerInfo", "none"},
      {"insertionTime", getDateAndTime()}
    };


    // payload meta info file:
    std::string metaFile = workDir + "/meta.json";

    myfile.open(metaFile);
    myfile << jsn.dump();
    myfile.close();

    // check if data exists

    if (m_data.find(tag) == m_data.end()) {
      try{
        nlohmann::json jsi = findAllIovsFs(tag);
        m_data.insert(std::pair<std::string, nlohmann::json>(tag, jsi));
      }
      catch (...) {
        try { // tag exists, but there are no IOVs
          nlohmann::json tagJS = findTag(tag);
          nlohmann::json jsFree = nlohmann::json::array({});
          m_data.insert(std::pair<std::string, nlohmann::json>(tag, jsFree));
        }
        catch (...) {
          throw std::runtime_error(
                  "ERROR in CrestClient::getSizeByTag file storage has no a tag with the name \"" + tag + "\".");
        }
      }
    }

    std::map<std::string, nlohmann::json>::iterator it = m_data.find(tag);
    if (it != m_data.end()) {
      std::string link = hashCode;
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

  std::string CrestClient::getDateAndTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];

    localtime_r(&now, &tstruct);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
  }

  //==========================================
  // Payload methods for File Storage

  std::string CrestClient::getPayloadAsStringFS(const std::string& hash) {
    std::string workDir = m_data_folder;
    workDir += '/';
    workDir += hash;
    std::string filePath = workDir + "/payload.json";
    std::string res = "";

    try{
      res = getFileString(filePath);
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::getPayloadAsStringFS cannot get the payload form file storage.");
    }

    return res;
  }

  nlohmann::json CrestClient::getPayloadAsJsonFS(const std::string& hash) {
    std::string res = getPayloadAsStringFS(hash);
    nlohmann::json respond = getJson(res, "getPayloadAsJsonFS");
    return respond;
  }

  std::string CrestClient::getPayloadMetaInfoAsStringFS(const std::string& hash) {
    std::string workDir = m_data_folder;
    workDir += '/';
    workDir += hash;

    std::string filePath = workDir;
    filePath += "/meta.json";

    std::string res = "";

    try{
      res = getFileString(filePath);
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::getPayloadAsStringFS cannot get the payload form file storage.");
    }

    return res;
  }

  nlohmann::json CrestClient::getPayloadMetaInfoAsJsonFS(const std::string& hash) {
    std::string res = getPayloadMetaInfoAsStringFS(hash);
    nlohmann::json respond = getJson(res, "getPayloadAsJsonFS");
    return returnJArray(respond);
  }

  std::string CrestClient::getBlobFs(const std::string& hash) {
    try{
      std::string str = getPayloadAsStringFS(hash);
      return str;
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::getBlobFs : cannot get the blob information form file storage");
    }
  }

  std::string CrestClient::getBlobInStreamFs(const std::string& hash, std::ofstream& out) {
    std::string line;
    std::string str = "";

    std::string workDir = m_data_folder;
    workDir += '/';
    workDir += hash;

    std::string file_path = workDir;
    file_path += "/payload.json";

    std::ifstream in(file_path);

    if (in.is_open() && out.is_open()) {
      while (getline(in, line)) {
        out << line << std::endl;
      }
    } else {
      throw std::runtime_error(
              "ERROR in CrestClient::getBlobInStreamFs : cannot get the blob information form file storage");
    }
    in.close();

    return str;
  }

  nlohmann::json CrestClient::returnJArray(nlohmann::json js) {
    nlohmann::json result = nlohmann::json::array();
    result.push_back(js);
    return result;
  }

  int CrestClient::getSizeFS(const std::string& tagname) {
    int res = 0;

    try {
      nlohmann::json iovs = findAllIovsFs(tagname);
      int length = iovs.size();
      return length;
    }
    catch (...) {
      return res;
    }

    return res;
  }

  //==========================================
  // IOV List Methods for File Storage


  nlohmann::json CrestClient::findAllIovsFs(const std::string& tagname, int size, int page) {
    nlohmann::json js = nlohmann::json::array();
    int dataSize = getSizeFS(tagname);

    if (dataSize == 0) return js; // the data is absent

    // index interval to load the data from JSON array:
    int kmin = size * page;
    int kmax = size * (page + 1);

    // check if the interval is correct:
    if (kmin > dataSize - 1) return js; // out of range

    if (kmax > dataSize - 1) { // this step is not full
      kmax = dataSize;
    }

    nlohmann::json data = findAllIovsFs(tagname);

    for (int i = kmin; i < kmax; i++) {
      js.push_back(data[i]);
    }
    return js;
  }

  nlohmann::json CrestClient::selectIovsFS(const std::string& tagname, long since, long until) {
    nlohmann::json js = nlohmann::json::array();

    try{
      nlohmann::json iovList = findAllIovsFs(tagname);
      int niovs = iovList.size();
      for (int i = 0; i < niovs; i++) {
        if (iovList[i].find("since") != iovList[i].end()) {
          ;
          long currentS = iovList[i]["since"];
          if (currentS >= since && currentS <= until) {
            js.push_back(iovList[i]);
          }
        }
      }
    }
    catch (...) {
      throw std::runtime_error("ERROR in CrestClient::selectIovsFS : cannot get the iov list form file storage");
    }

    return js;
  }

  void CrestClient::getTagDataInfo(const std::string& tagname) {
    std::cout << std::endl << "test: getTagDataInfo" << std::endl;

    try {
      int info = getSizeFS(tagname);
      std::cout << std::endl << "IOV Number (" << tagname << ")       = "
                << info << std::endl;
    }
    catch (const std::exception& e) {
      std::cout << std::endl << "test: getTagDataInfo (failed)" << std::endl;
      std::cout << e.what() << std::endl;
    }

    try {
      nlohmann::json iovList = findAllIovs(tagname);
      int niovs = iovList.size();
      std::list <std::string> hashList;

      for (int i = 0; i < niovs; i++) {
        if (iovList[i].find("payloadHash") != iovList[i].end()) {
          std::string hash = iovList[i]["payloadHash"];
          std :: list < std::string > :: iterator pos;
          pos = find(hashList.begin(), hashList.end(), hash);
          if (pos != hashList.end()) {
          } else {
            hashList.push_back(hash);
          }
        }
      }

      int hashNumber = hashList.size();
      std::cout << "Hash Number (" << tagname << ")      = "
                << hashNumber << std::endl;
    }
    catch (const std::exception& e) {
      std::cout << std::endl << "ERROR getTagDataInfo: cannot get IOV List" << std::endl;
      std::cout << e.what() << std::endl;
    }

    std::string path = m_root_folder + s_FS_DATA_PATH;
    int dirNumber = 0;

    for (auto it : std::filesystem::directory_iterator(path)) {
      dirNumber = dirNumber + 1;
    }
    std::cout << "Directory Number (" << tagname << ") = "
              << dirNumber << std::endl;
  }
} // namespace
