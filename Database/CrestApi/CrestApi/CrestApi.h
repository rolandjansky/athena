/*
   Copyright (C) 2019-2022 CERN for the benefit of the ATLAS collaboration
 */


/*!
   \file
   \brief Header file for CREST C++ Client Library

   This file contains the CrestClient class.
   (See CrestClientExt.h for CrestClient class extention with additional methods.)
   Here is the method description.
 */


#ifndef CRESTAPI_CRESAPI_H
#define CRESTAPI_CRESAPI_H

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

// We get an unused parameter warning from within the boost parameter code.
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif


#include <string>
#include <map>
#include <list>
#include <iosfwd>
#include <cstdint>
#include "nlohmann/json.hpp"
#include <curl/curl.h>
#include <iostream>


namespace Crest {
// AUXILIARY CLASS to store URL request parameters
  class urlParameters
  {
  private:
    std::string m_str {};
  public:
    void add(const std::string& key, const std::string& value) {
      if (m_str.empty()) {
        m_str = key + "=" + value;
      } else {
        m_str = m_str + "&" + key + "=" + value;
      }
    }

    std::string getParams() {
      return m_str;
    }

    void reset() {
      m_str.clear();
    }
  };



  class CrestClient
  {
  private:
    std::string m_host {};
    std::string m_port {};
    int m_mode {};
    std::string m_root_folder {};
    std::string m_data_folder {};
    std::map<std::string, nlohmann::json> m_data {};
    bool m_isRewrite {};
    std::string m_currentTag {};

    inline static const std::string s_PATH = "/crestapi";
    inline static const std::string s_TAG_PATH = "/tags";
    inline static const std::string s_ADMIN_PATH = "/admin";
    inline static const std::string s_IOV_PATH = "/iovs";
    inline static const std::string s_IOV_SIZE_PATH = "/getSize";
    inline static const std::string s_IOV_SIZE_PATH_FOR_TAG = "/getSizeByTag";
    inline static const std::string s_IOV_SELECT_PATH = "/selectIovs";
    inline static const std::string s_IOV_GROUP_PATH = "/selectGroups";
    inline static const std::string s_IOV_SNAPSHOT_PATH = "/selectSnapshot";
    inline static const std::string s_GLOBALTAG_PATH = "/globaltags";
    inline static const std::string s_GLOBALTAG_MAP_PATH = "/globaltagmaps";
    inline static const std::string s_PAYLOAD_PATH = "/payloads";
    inline static const std::string s_MONITORING_PAYLOAD_PATH = "/monitoring/payloads";
    inline static const std::string s_META_PATH = "/meta";
    inline static const std::string s_DATA_PATH = "/data";
    inline static const std::string s_STOREBATCH_PATH = "/storebatch";
    inline static const std::string s_FOLDER_PATH = "/folders";
    inline static const std::string s_RUNINFO_PATH = "/runinfo";
    inline static const std::string s_RUNINFO_LIST_PATH = "/list";

    // parameters for CREST file storage:

    inline static const std::string s_FS_TAG_PATH = "/tags";
    inline static const std::string s_FS_GLOBALTAG_PATH = "/globaltags";
    inline static const std::string s_FS_DATA_PATH = "/data";

    inline static const std::string s_FS_TAG_FILE = "/tag.json";
    inline static const std::string s_FS_IOV_FILE = "/iovs.json";
    inline static const std::string s_FS_TAGMETAINFO_FILE = "/tagmetainfo.json";

    inline static const std::string s_FS_META_FILE = "/meta.json";
    inline static const std::string s_FS_PALOAD_FILE = "/payload.json";

    inline static const std::string s_FS_GLOBALTAG_FILE = "/globaltag.json";
    inline static const std::string s_FS_MAP_FILE = "/maps.json";

    inline static const std::string s_FS_PATH = "";

    enum CrestMode {
      SERVER_MODE = 1, FILESYSTEM_MODE = 2
    };
    enum Action {
      GET = 0, POST = 1, PUT = 2, DELETE = 3
    };


// ====================================================
// REQUEST METHODS

/**
 * General auxiliary method to make request to the CREST Server. This method is used by other methods realizing the
 * requests with the concrete kinds of data (iovs|payloads|tags…).
 *
 * @param current_path - URL request path
 * @param action - Action (GET|POST|DELETE)
 * @param js - a JSON object which has to be send or changed with this request. If this argument has to be void it has
 * to be set as nullptr.
 *
 */
    std::string performRequest(const std::string& current_path, Action action, nlohmann::json& js);


/**
 * General auxillary method to make request to the CREST Server. This method used to get a Blob as a stream.
 * See method getBlobInStream(std::string& hash, std::ofstream& out).
 * @param current_path - URL request path
 * @param action - Action (GET|POST|DELETE)
 * @param js - a JSON object which has to be send or changed with this request. If this argument has to be void it has
 * to be set as nullptr.
 * @param out - stream to get the result
 *
 */
    std::string performRequest(const std::string& current_path, Action action, nlohmann::json& js, std::ofstream& out);


/**
 * General auxillary method to make request to the CREST Server. This method is used by other methods realizing the
 * requests with the concrete kinds of data (iovs|payloads|tags…).
 *
 * @param current_path - URL request path
 * @param action - Action (GET|POST|DELETE)
 * @param js - a JSON object which has to be send or changed with this request. If this argument has to be void it has
 * to be set as nullptr.
 * @param method_name - the name of method which called this method. This parameter is used in the error messages.
 *
 */
    std::string performRequest(const std::string& current_path, Action action, nlohmann::json& js,
                               const char* method_name);


/**
 * This is an auxillary method to make an request to store a payload.
 * (Old method name - store_payload_request)
 * @param tag - tag name.
 * @param since - since.
 * @param js - payload in the JSON format.
 */
    std::string storePayloadRequest(const std::string& tag, uint64_t since, const std::string& js);

/**
 * This is an auxillary method to make an request to store several payloads in the batch mode.
 * (The old method name - store_batch_payload_request)
 * @param tag - tag name.
 * @param endtime - end time.
 * @param js - payloads in the JSON format.
 */
    std::string storeBatchPayloadRequest(const std::string& tag, uint64_t endtime, const std::string& js);


// ====================================================
// AUXILLARY METHODS

/**
 * Auxillary method to get a resources parameter from new CREST server response.
 *
 * @param js - CREST server response.
 */
    nlohmann::json getResources(nlohmann::json& js);


/**
 * Auxillary method to get the first element from resources of the new CREST server response.
 *
 * @param js - CREST server response.
 */
    nlohmann::json getResFirst(nlohmann::json& js);


/**
 *  Auxillary method to check if the CrestApi library initialized to work with the file system.
 *  It was used to throw an exception if the library method not implemented for file system.
 * @param method_name - method name.
 */
    void checkFsException(const char* method_name);


/**
 *  Auxillary method to check if the JSON object contain an error message.
 * @param js - a JSON object to be checked. Example: <br>
 * <pre>
 *    nlohmann::json js =
 *    {
 *       "message": "Cannot create tag because name already exists: test_MvG3b",
 *       "type": "error"
 *    }
 * </pre>
 * @param method - method name, where was the error.
 */
    int checkErrors(const nlohmann::json& js, const char* method);

/**
 * This method removes all XML/HTML tags from a string.
 * (It is an auxillary method to clear the CREST Server response.)
 * @param xmlBuffer - the text (a std::string ) to be cleared.
 */
    std::string ParseXMLOutput(std::string_view xmlBuffer);

/**
 * This method removes all end of line and carriage return symbols from a string.
 * (It is an auxillary method to clear the CREST Server response.)
 * @param str - the text (a std::string ) to be cleared.
 */
    std::string removeCR(const std::string& str);


/**
 * This auxillary method checks the CURL response.
 * It contais a error checking routine (common for all CURL request methods).
 * @param res - CURL response status,
 * @param response_code - the CURL response code,
 * @param st - the CURL response,
 * @param method_name - the name on a method which calls one of the perform request methods.
 */
    void checkResult(CURLcode res, long response_code, const std::string& st, const char* method_name);
  public:
// ===================================
// CONSTRUCTORS

/**
 * CrestClient constructor for file storage mode. If CrestClient is created with this method the data will be written on
 * the hard disk.
 * @param rewriteIfExist - this parameter sets if the data can be rewritten.
 * @param root_folder - the folder where the data have to be stored.
 */
    CrestClient(bool rewriteIfExists, const std::string& root_folder = "/tmp/crest_dump");


/**
 * CrestClient constructor for Internet mode. If CrestClient is created with this method the data will be sent to the
 * CREST Server.
 * @param _host - host name of the CREST Server.
 * @param _port - port of the CREST Server.
 */
    CrestClient(const std::string& _host, const std::string& _port);

/**
 * CrestClient constructor for Internet mode. If CrestClient is created with this method the data will be sent to the
 * CREST Server.
 * @param url - URL address of the CREST Server (with port).
 * <br> <br>
 * Example:
 * <br>
 * <pre>
 *    std::string url = "http://mvg-test-pc-03.cern.ch:8090";
 *    CrestClient myCrestClient = CrestClient(url);
 * </pre>
 */
    CrestClient(std::string_view url);

    ~CrestClient();



// ==========================================
// Utilities


/**
 * Auxillary method to convert string in to JSON object.
 * @param str - string (std::string)
 * @return - JSON object as nlohmann::json
 *
 */
    nlohmann::json getJson(const std::string& str); // string to json

/**
 * Auxillary method to convert string in to JSON object.
 * @param str - string (std::string)
 * @param method - method name, wthich calls this method. This parameter is used to throw an error exception.
 * @return - JSON object as nlohmann::json
 *
 */
    nlohmann::json getJson(const std::string& str, const char* method); // string to json

/**
 * Auxillary method to get a file as a string.
 * (The old method name -  get_file_string)
 * @param path - path to the file.
 *
 */
    std::string getFileString(const std::string& path); // method to read file as a string

/**
 * Auxillary method to get a file list in the given directory.
 * (The old method name - get_file_list)
 * @param path - path to the directory.
 *
 */
    void getFileList(const std::string& path);



//=====================================================
// AdminApi
//


/**
 * This method allows to remove a Global Tag form CREST DB.
 * (This method is an analogue of the remove_global_tag method in Python)
 * @param name  - global tag name
 */
    void removeGlobalTag(const std::string& name);


/**
 * This method allows to remove a Tag.
 * (This method is an analogue of the remove_tag method in Python)
 * @param tagName - tag name
 */
    void removeTag(const std::string& tagName);


/**
 * This method allows to update a GlobalTag.Arguments: the name has to uniquely identify a global tag.
 * (This method is an analogue of the update_global_tag method in Python)
 * @param name - global tag name
 * @param body - JSON object. A new global tag in JSON format. Example for body parameter: <br>
 * <pre>
 * json body =
 *    {
 *       {"name","MvG_TEST_01"},
 *       {"validity",0},
 *       {"description","test"},
 *       {"release","1"},
 *       {"insertionTime","2018-12-18T11:32:58.081+0000"},
 *       {"snapshotTime","2018-12-18T11:32:57.952+0000"},
 *       {"scenario","test2"},
 *       {"workflow","M"},
 *       {"type","t"},
 *       {"snapshotTimeMilli","null"},
 *       {"insertionTimeMilli","null"}
 *    };
 *  <br>
 * </pre>
 */
    void updateGlobalTag(const std::string& name, nlohmann::json body);

//=====================================================

//=====================================================
// FoldersApi
//

/**
 * Method to create a folder. It is not yet realized
 * (This method is an analogue of the create_folder method in Python)
 */
    std::string createFolder(nlohmann::json& body);  //method does not work, maybe the parameters were wrong


/** This method returns the folder list on CREST DB.
 * (This method is an analogue of the list_folders method in Python)
 *
 */
    nlohmann::json listFolders();


//=====================================================

//=====================================================
// FsApi
//
//
// Dump a tag into filesystem and retrieve the tar file asynchronously.
// This method allows to request a tar file from the server using a tag specified in input.
// tagname = 'none' # str | tagname: the tag name {none} (default to none)
// snapshot = 0 # int | snapshot: the snapshot time {0} (default to 0)
// str build_tar(string tagname, int snapshot);

//=====================================================

//=====================================================
// GlobaltagmapsApi
//

/**
 * Method to create a global tag map.
 * (This method is an analogue of the create_global_tag_map method in Python)
 * @param gt - global tag map in the JSON format. Example: <br>
 * <pre>
 * json gt =
 *    {
 *      {"globalTagName", "M_TEST"},
 *      {"record", "testing2"},
 *      {"label", "test2"},
 *      {"tagName", "test_MvG3"}
 *    }
 * </pre>
 */
    void createGlobalTagMap(nlohmann::json& gt);

/**
 * Method to create a global tag map.
 * (This method is an analogue of the create_global_tag_map method in Python)
 * @param globaltag - global tag name,
 * @param tagname - tag name,
 * @param record - record,
 * @param label - label,
 */
    void createGlobalTagMap(const std::string& globaltag, const std::string& tagname,
                            const std::string& record, const std::string& label);

/**
 * This method search for mappings using the global tag name.
 * (This method is an analogue of the find_global_tag_map method in Python)
 * @param name - name of a global tag
 */
    nlohmann::json findGlobalTagMap(const std::string& name);


//=====================================================

//=====================================================
// GlobaltagsApi
//

/**
 * Create a global tag in the database. This method allows to insert a global tag.
 * (This method is an analogue of the create_global_tag method in Python)
 * @param js - global tag in the JSON format. Example: <br>
 * <pre>
 * json js =
 *    {
 *       {"name","M_TEST_01"},
 *       {"validity",0},
 *       {"description","test"},
 *       {"release","1"},
 *       {"insertionTime","2018-12-18T11:32:58.081+0000"},
 *       {"snapshotTime","2018-12-18T11:32:57.952+0000"},
 *       {"scenario","test"},
 *       {"workflow","M"},
 *       {"type","t"},
 *       {"snapshotTimeMilli","null"},
 *       {"insertionTimeMilli","null"}
 *    };
 *
 * </pre>
 */
    void createGlobalTag(nlohmann::json& js);

/**
 * Create a global tag in the database. This method allows to insert a global tag.
 * (This method is an analogue of the create_global_tag method in Python)
 * @param tag - global tag as JSON string (std::string).
 */
    void createGlobalTag(const std::string& tag);

/**
 * Create a global tag in the database. This method allows to insert a global tag,
 * it has a reduced parameterset: the global tag name and the global tag description.
 * (This method is an analogue of the create_global_tag method in Python)
 * The method set the following default parameters:
 * <pre>
 *   {
 *       "release": "1",
 *       "scenario": "undefined",
 *       "type": "UPD",
 *       "validity": 0,
 *       "workflow": "undefined"
 *   }
 *
 * </pre>
 * @param tagname - global tag name,
 * @param description - global tag description,
 */
    void createGlobalTag(const std::string& tagname, const std::string& description);


/**
 * Finds a global tag by name. This method will search for a global tag with the given name. Only one global tag should
 * be returned.
 * (This method is an analogue of the find_global_tag_as_string method  in Python)
 * This method returns the global tag as a string.
 * @param name - global tag name
 */
    std::string findGlobalTagAsString(const std::string& name);

/**
 * Finds a global tag by name. This method will search for a global tag with the given name. Only one global tag should
 * be returned.
 * (This method is an analogue of the find_global_tag method in Python)
 * This method returns the global tag as a JSON object.
 * @param name - global tag name
 */
    nlohmann::json findGlobalTag(const std::string& name);


/**
 * This method finds a global tag lists. The result is a JSON object.
 * (This method is an analogue of the list_global_tags method in Python)
 */
    nlohmann::json listGlobalTags();

/**
 * This method returns the global tag list. It is a verion of this method with all parameters.
 * @param name - global tag name pattern,
 * @param size - page size,
 * @param page - page number,
 * @param sort - sorting order.
 */
    nlohmann::json listGlobalTags(const std::string& name, int size, int page, const std::string& sort);


/**
 * This method finds a global tag lists. The rusult is a string.
 * (This method is an analogue of the list_global_tags_as_string method in Python)
 */
    std::string listGlobalTagsAsString();


//=====================================================

// ====================================================
// IovsApi


/**
 * This method creates an iov for a given tag in the CREST DB for the created payload. The tag name is used in the iov
 * creation. This tag has to be defined in the CREST DB before creating iov
 * (This method is an analogue of the create_iov method in Python)
 * @param iov - an iov in the JSON format (JSON object). Example: <br>
 * <pre>
 *    json js3b =
 *    {
 *      {"tagName","test_MvG3"},
 *      {"since",100},
 *      {"insertionTime","2018-12-18T11:32:58.329+0000"},
 *      {"payloadHash","ABRACADABRA4"},
 *    };
 * </pre>
 */
    void createIov(nlohmann::json& iov);


/**
 * This method finds all iovs for a given tag name. The result is a JSON object.
 * (This method is an analogue of the find_all_iovs method in Python)
 * @param tagname - tag name.
 */
    nlohmann::json findAllIovs(const std::string& tagname);

/**
 * This method finds all iovs for a given tag name. The result is a JSON object. It is a version of this method with all
 * parameters.
 * (This method is an analogue of the find_all_iovs method in Python)
 * @param tagname - tag name.
 * @param size - page size.
 * @param page - page number.
 * @param sort - sort pattern (for example "{id.since:ASC}").
 * @param dateformat - format of the input time fields.
 */
    nlohmann::json findAllIovs(const std::string& tagname, int size, int page, const std::string& sort,
                               const std::string& dateformat);


/**
 * This metghod gets the number of iovs for the given tag. This method allows to select the count of iovs in a tag.
 * (This method is an analogue of the get_size method in Python)
 * @param tagname - tag name.
 */
    int getSize(const std::string& tagname);


/**
 * This method gets the number of iovs for tags matching pattern. This method allows to select the count of iovs in a
 * tag.
 * (This method is an analogue of the get_size_by_tag method in Python)
 * The result is a JSON object.
 * @param tagname - tag name.
 */
    nlohmann::json getSizeByTag(const std::string& tagname);


/**
 * Select groups for a given tagname. This method allows to select a list of groups. The result is a JSON object.
 * (This method is an analogue of the select_groups method in Python)
 * @param tagname - tag name.
 */
    nlohmann::json selectGroups(const std::string& tagname);


/**
 * Select groups for a given tagname. This method allows to select a list of groups. The result is a JSON object. The
 * result is a JSON object.
 * (This method is an analogue of the select_groups method in Python)
 * @param tagname - tag name.
 * @param snapshot - snapshot.
 */
    nlohmann::json selectGroups(const std::string& tagname, long snapshot);


/**
 * Select iovs for a given tagname. This method allows to select a list of iovs in a tag for a given snapshot time. The
 * result is a JSON object.
 * (This method is an analogue of the select_iovs method in Python)
 * @param tagname - tag name.
 * @param snapshot - snapshot.
 */
    nlohmann::json selectIovs(const std::string& tagname);


/**
 * Select iovs for a given tagname. This method allows to select a list of iovs in a tag for a given snapshot time. The
 * result is a JSON object.
 * (This method is an analogue of the select_iovs method in Python)
 * @param tagname - tag name.
 * @param snapshot - snapshot.
 */
    nlohmann::json selectIovs(const std::string& tagname, long snapshot);


/**
 * This method allows to select a list of all iovs in a tag.
 * (This method is an analogue of the select_snapshot method in Python)
 * @param tagname - tag name.
 */
    nlohmann::json selectSnapshot(const std::string& tagname);

/**
 * This method allows to select a list of all iovs in a tag, using a given snapshot time.
 * (This method is an analogue of the select_snapshot method in Python)
 * @param tagname - tag name.
 * @param snapshot - snapshot.
 */
    nlohmann::json selectSnapshot(const std::string& tagname, long snapshot);


//=======================================================

//=======================================================
// MonitoringApi
//

/**
 * This method retrieves monitoring information on all payloads as a list of payload tag information.
 * (This method is an analogue of the list_payload_tag_info method in Python)
 */
    nlohmann::json listPayloadTagInfo();

/**
 * This method retrieves monitoring information on payload as a list of payload tag information.
 * (This method is an analogue of the list_payload_tag_info method in Python)
 * @params tagname - tag name.
 */
    nlohmann::json listPayloadTagInfo(const std::string& tagname);

//=======================================================

//=======================================================
// PayloadsApi
//


/**
 * This method creates a payload in the CREST DB.
 * (This method is an analogue of the create_payload method in Python)
 * @param js - payload in the JSON format. Example: <br>
 * <pre>
 * json js =
 *    {  {"hash", "ABRACADABRA"},
 *       {"version", "string"},
 *       {"objectType", "string"},
 *       {"data", "none"},
 *       {"streamerInfo", "none"},
 *       {"insertionTime", "2019-03-14T13:29:25.286Z"}
 *    };
 * </pre>
 */
    void createPayload(nlohmann::json& js);

// Create a Payload in the database.
// This method allows to insert a Payload.
// Arguments: PayloadDto should be provided in the body as a JSON file.
// file = '/path/to/file.txt' # file | The file
// payload = 'payload_example' # str | Json body for payloaddto
// PayloadDto create_payload_multi_form(file, PayloadDto payload);


/**
 * This method finds payload data by hash; the payload object contains the real BLOB. The result returns as a string.
 * (This method is an analogue of the get_blob method in Python)
 * @param hash - hash
 */
    std::string getBlob(const std::string& hash);

/**
 * This method finds payload data by hash; the payload object contains the real BLOB. The result returns in the stream
 *"out" (BE CAREFUL!).
 * (This method is an analogue of the get_blob method in Python)
 * @param hash - hash
 * @param[out] out - output stream to get the result.
 */
    std::string getBlobInStream(const std::string& hash, std::ofstream& out);


/**
 *  This method finds a payload resource associated to the hash. The payload returns as a JSON object.
 * (This method is an analogue of the get_payload method in Python)
 * @param hash - hash.
 */
    nlohmann::json getPayloadAsJson(const std::string& hash);


/**
 *  This method finds a payload resource associated to the hash. The payload returns as a string.
 * (This method is an analogue of the get_payload method in Python)
 * @param hash - hash.
 */
    std::string getPayloadAsString(const std::string& hash);


/**
 *  This method finds a payload resource associated to the hash. This method retrieves metadata of the payload resource.
 * The result is a JSON object.
 * (This method is an analogue of the get_payload_meta_info method in Python)
 * @param hash - hash.
 */
    nlohmann::json getPayloadMetaInfo(const std::string& hash);

/**
 *  This method finds a payload resource associated to the hash. This method retrieves metadata of the payload resource.
 * The result is a string.
 * (This method is an analogue of the get_payload_meta_info method in Python)
 * @param hash - hash.
 */
    std::string getPayloadMetaInfoAsString(const std::string& hash);



/**
 * Create a payload in the database, associated to a given iov since and tag name. This method allows to insert a
 * payload and an IOV.
 * (This method is an analogue of the store_payload method in Python)
 * @param tag - tag name.
 * @param since - since. A parameter to create an iov together with payload.
 * @param js - payload.
 */
    void storePayload(const std::string& tag, uint64_t since, const std::string& js);

// Stores payloads in batch mode

/**
 * This method stores several payloads in batch mode. (The deprecated method.)
 * (This method is an analogue of the store_batch_payloads method in Python)
 * @param tag_name - tag name.
 * @param endtime - end time.
 * @param iovsetupload - iov data as a string.
 * <br> Example how to use these parameters: <br>
 * <pre>
 *    std::string name58 = "test_MvG3b";
 *    uint64_t endtime58 = 200;
 *    std::string str58 = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
 *    myCrestClient.storeBatchPayloads(name58, endtime58, str58);
 * </pre>
 */
    void storeBatchPayloads(const std::string& tag_name, uint64_t endtime, const std::string& iovsetupload);


/**
 * This method stores several payloads in batch mode. (The deprecated method.)
 * (This method is an analogue of the store_batch_payloads method in Python)
 * @param tag_name - tag name.
 * @param endtime - end time.
 * @param iovsetupload - iov data as a JSON object.
 * <br> Example how to use these parameters: <br>
 * <pre>
 *    std::string name58 = "test_MvG3a";
 *    uint64_t endtime58 = 200;
 *    std::string str58 = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
 *    nlohmann::json js58 = myCrestClient.getJson(str58);
 *    myCrestClient.storeBatchPayloads(name58, endtime58, js58)
 * </pre>
 */
    void storeBatchPayloads(const std::string& tag_name, uint64_t endtime, nlohmann::json& js);

/**
 * This method stores several payloads in batch mode.
 * (This method is an analogue of the store_batch_payloads method in Python)
 * @param tag_name - tag name.
 * @param endtime - end time, if endtime = 0, the server does not use this parameter in the internal check.
 * @param iovsetupload - iov data as a string.
 * <br> Example how to use these parameters: <br>
 * <pre>
 *    std::string name58 = "test_MvG3b";
 *    uint64_t endtime58 = 200;
 *    std::string str58 = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
 *    myCrestClient.storeBatchPayloads(name58, endtime58, str58);
 * </pre>
 */
    void storeBatchPayloads(const std::string& tag_name, const std::string& iovsetupload, uint64_t endtime = 0);

/**
 * This method stores several payloads in batch mode.
 * (This method is an analogue of the store_batch_payloads method in Python)
 * @param tag_name - tag name.
 * @param endtime - end time, if endtime = 0, the server does not use this parameter in the internal check.
 * @param iovsetupload - iov data as a JSON object.
 * <br> Example how to use these parameters: <br>
 * <pre>
 *    std::string name58 = "test_MvG3a";
 *    uint64_t endtime58 = 200;
 *    std::string str58 = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
 *    nlohmann::json js58 = myCrestClient.getJson(str58);
 *    myCrestClient.storeBatchPayloads(name58, endtime58, js58)
 * </pre>
 */
    void storeBatchPayloads(const std::string& tag_name, nlohmann::json& js, uint64_t endtime = 0);

//=======================================================

//=======================================================
// RuninfoApi
//


/**
 * This method creates an entry for run information. Run informations go into a separate table.
 * (This method is an analogue of the create_run_lumi_info method in Python)
 * @param body - run information in JSON format. Example: <br>
 * <pre>
 *    std::string str19 =
 *"{\"since\":\"10\",\"run\":\"7777771\",\"lb\":\"62\",\"starttime\":\"10\",\"endtime\":\"200\"}";
 *    json body = json::parse(str19);
 * </pre>
 */
    void createRunLumiInfo(nlohmann::json& body);


/**
 * Finds a run/lumi information lists using parameters. This method allows to perform search.
 * (This method is an analogue of the find_run_lumi_info method in Python)
 * @params params - search parameters. Example: <br>
 * <pre>
 *    urlParameters params28;
 *    params28.add("from","2018010101");
 *    params28.add("to","2019050500");
 *    params28.add("format","Time");
 *    json tag_info28 = myCrestClientF.findRunLumiInfo(params28);
 * </pre>
 */
    nlohmann::json findRunLumiInfo(urlParameters params);


/**
 * This method gets full list of run/lumi information data.
 * (This method is an analogue of the list_run_lumi_info method in Python)
 */
    nlohmann::json listRunLumiInfo();

//=======================================================

//=======================================================
// TagsApi

// CREATE TAG


/**
 * This method creates a tag in the CREST database.
 * (This method is an analogue of the create_tag method in Python)
 * @param js - tag in the JSON format. Example: <br>
 * <pre>
 *    json js =
 *    {
 *      {"description", "none"},
 *      {"endOfValidity", 0},
 *      {"insertionTime", "2018-12-06T11:18:35.641+0000"},
 *      {"lastValidatedTime", 0},
 *      {"modificationTime", "2018-12-06T11:18:35.641+0000"},
 *      {"name", "test_M"},
 *      {"payloadSpec", "stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"},
 *      {"synchronization", "none"},
 *      {"timeType", "time"}
 *    };
 * </pre>
 */
    void createTag(nlohmann::json& js);


/**
 * This method create a tag in the CREST database.
 * (This method is an analogue of the create_tag method in Python)
 * @param name - tag name.
 * @param desc - description.
 * @param timeType - time type.
 */
    void createTag(const std::string& name, const std::string& desc, const std::string& timeType);

/**
 * This method create a tag in the CREST database.
 * (This method is an analogue of the create_tag method in Python)
 * @param name - tag name.
 * @param desc - description.
 * @param timeType - time type..
 * @param payloadSpec - payload specification.
 */
    void createTag(const std::string& name, const std::string& desc, const std::string& timeType,
                   const std::string& payloadSpec);

//std::string uploadPayload(std::string  tag,std::string  since, std::string& js);


/**
 * This method finds a tag by the name. The result is a JSON object
 * (This method is an analogue of the find_tag method in Python)
 * @param name - tag name
 */
    nlohmann::json findTag(const std::string& name);


// TAG LIST methods:

/**
 * This method returns the tag list.
 * (This method is an analogue of the list_tags method in Python)
 */
    nlohmann::json listTags(void);

/**
 * This method returns the tag list. It is a verion of this method with additional parameters.
 * (This method is an analogue of the list_tags method in Python)
 * @param size - page size.
 * @param page - page number.
 */
    nlohmann::json listTags(int size, int page);


/**
 * This method returns the tag list. It is a verion of this method with all parameters.
 * (This method is an analogue of the list_tags method in Python)
 * @param name - tag name pattern,
 * @param size - page size,
 * @param page - page number,
 * @param sort - sorting order.
 */
    nlohmann::json listTags(const std::string& name, int size, int page, const std::string& sort);


// UPDATE TAG

/**
 * This method allows to update tag record in the CREST DB.
 * (This method is an analogue of the update_tag method in Python)
 * @param tagname - tg name.
 * @param body - new parameters in the JSON format. This JSON object contain the parameters which have to be changed
 * only. Example: <br>
 * <pre>
 *    json body =
 *    {
 *      {"description","none"},
 *      {"lastValidatedTime",0},
 *      {"endOfValidity",40}
 *    }
 * </pre>
 */
    void updateTag(const std::string& tagname, nlohmann::json body);


/**
 * This method cahnges object type specification.
 * (The old name of this method is update_tag_specification)
 * @param tagname - tag name.
 * @param objectType - new object type.
 */
    void updateTagSpecification(const std::string& tagname, const std::string& objectType);

// FILE SYSTEM METHODS

//Dump methods

/**
 * Method to store a tag in the local file system. The path to stored data defined by a CrestClient constructor
 *(m_root_folder and m_data_folder variables).
 * (This method is an analogue of the create_tag method in Python)
 * @param js - tag as a JSON object.
 *
 */
    void createTagDump(nlohmann::json& js);

/**
 * Auxillary method to write all the data at the end of a session. The path to stored data defined by a CrestClient
 * constructor (in m_root_folder and m_data_folder variables).
 *
 */
    void flush();

/**
 * Method to store a payload in the local file system. This is an auxillary method. It is used by storePayload method if
 * CrestClient is innitialized in the file storage mode.
 * (The old name of this method is store_payload_dump)
 * @param tag - tag name.
 * @param since - since.
 * @param js - payload as a JSON object.
 *
 */
    void storePayloadDump(const std::string& tag, uint64_t since, const std::string& js);


/**
 * This method returns a tag list stored in the file storage. This is the auxilary method. The result is the tag names
 * only (as a string list).
 * (The old name of this method is short_list_tags_fs)
 *
 */
    std::list<std::string> shortListTagsFs();

/**
 * This method finds a tag by the tag name in the file storage. This is the auxilary method. It is called by findTag
 * method when CrestClient is initialized in the file storage mode.
 * (This method is an analogue of the find_tag method in Python)
 * @param name - tag name.
 */
    nlohmann::json findTagFs(const std::string& name);

/**
 * This method finds all iovs by the tag name in the file storage. This is the auxilary method. It is called by
 * find_all_iovs method when CrestClient is initialized in the file storage mode.
   (This method is an analogue of the find_all_iovs method in Python)
 * @param tagname - tag name.
 */
    nlohmann::json findAllIovsFs(const std::string& tagname);

/**
 * This method finds all iovs for a given tag name. The result is a JSON object. It is a verion of this method with all
 * parameters. This method works with the file data storage.
 * (This method is an analogue of the find_all_iovs method in Python)
 * @param tagname - tag name.
 * @param size - page size.
 * @param page - page number.
 */
    nlohmann::json findAllIovsFs(const std::string& tagname, int size, int page);

/**
 * Auxillary method to extract the file name from iov hash (the hash is a path in the file storage).
 * (The old method name is get_filename)
 * @param path - an iov hash.
 */
    std::string getFileName(const std::string& path);


/**
 * This method finds payload data by hash in the file storage; the payload object contains the real BLOB. The result
 * returns as a string. This is an auxillary method called by getBlob method when CrestClient innitialized in the file
 * storage mode.
 * (This method is an analogue of the get_blob method in Python)
 * @param hash - hash
 */
    std::string getBlobFs(const std::string& hash);

/**
 * This method finds payload data by hash; the payload object contains the real BLOB. The result returns in the stream
 *"out" (BE CARREFULL!). This is an auxillary method called by getBlobInStream method when CrestClient innitialized in
 * the file storage mode.
 * (This method is an analogue of the get_blob method in Python)
 * @param hash - hash
 * @param[out] out - output stream to get the result.
 */
    std::string getBlobInStreamFs(const std::string& hash, std::ofstream& out);


/**
 * This auxillary method stores several payloads in batch mode in the file storage.
 * (This method is an analogue of the store_batch_payloads method in Python)
 * @param tag_name - tag name.
 * @param iovsetupload - iov data as a string.
 * <br> Example how to use these parameters: <br>
 * <pre>
 *    std::string name39 = "test_M";
 *    std::string str39 = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
 *    myCrestClient.storeBatchPayloadsFs(name39, str39);
 * </pre>
 */
    void storeBatchPayloadsFs(const std::string& tag_name, std::string& iovsetupload);

/**
 * This auxillary method stores several payloads in batch mode in the file storage.
 * (This method is an analogue of the store_batch_payloads method in Python)
 * @param tag_name - tag name.
 * @param iovsetupload - iov data as a JSON object.
 * <br> Example how to use these parameters: <br>
 * <pre>
 *    std::string name39 = "test_M";
 *    std::string str39 = "[{\"payloadHash\":\"aaa\",\"since\":100},{\"payloadHash\":\"bbb\",\"since\":150}]";
 *    nlohmann::json js39 = myCrestClient.getJson(str39);
 *    myCrestClient.storeBatchPayloadsFs(name39, js39);
 * </pre>
 */
    void storeBatchPayloadsFs(const std::string& tag_name, nlohmann::json& js);


/**
 * this is test only, later it will be deleted
 * (The old method name is streamtest)
 */
    void streamTest();

/**
 * Auxillary method to get an environment variable.
 *
 * @param varname - a variable name.
 */
    std::string getEnvA(const char* varname);

/**
 * Auxillary method to get an environment variable DAT_PATH.
 *
 */
    std::string getDataPath();

/**
 * This method checks if a string is in the JSON format.
 * (It is an auxillary method to check the CREST Server response format.)
 * @param str - the string to be checked.
 */
    bool isJson(const std::string& str);


// Tag Meta Info Methods

/**
 * This method creates a tag meta info in the CREST database.
 * @param js - tag meta info in the JSON format. Example: <br>
 * <pre>
 *    nlohmann::json js =
 *    {
 *      {"tagName", "test_MvG4"},
 *      {"description", "desc-01"},
 *      {"tagInfo", "taginf-01"},
 *      {"chansize", 0},
 *      {"colsize", 0},
 *      {"insertionTime", "2019-03-14T13:29:25.286Z"}
 *    };
 * </pre>
 */
    void createTagMetaInfo(nlohmann::json& js);

/**
 * This method creates a tag meta info in the CREST database.
 * (The tag meta info JSON contains the tag name, that is why it is better to use
 * the method variant without tagname parameter. This method exists for the
 * compatibility reasons.)
 * @param tagname - tag name,
 * @param js - tag meta info in the JSON format. Example: <br>
 * <pre>
 *    nlohmann::json js =
 *    {
 *      {"tagName", "test_MvG4"},
 *      {"description", "desc-01"},
 *      {"tagInfo", "taginf-01"},
 *      {"chansize", 0},
 *      {"colsize", 0},
 *      {"insertionTime", "2019-03-14T13:29:25.286Z"}
 *    };
 * </pre>
 */
    void createTagMetaInfo(const std::string& tagname, nlohmann::json& js);


/**
 * This method gets a tag meta info from the CREST database.
 * @param tagname - tag name
 */
    nlohmann::json getTagMetaInfo(const std::string& tagname);

/**
 * This method updates a tag meta info in the CREST database.
 * @param js - a JSON object with new parameters (These parameters will be changed in the CREST DB). Example: <br>
 * <pre>
 *    nlohmann::json js =
 *    {
 *      {"tagName", "test_MvG4"},
 *      {"description", "desc-01"},
 *      {"tagInfo", "taginf-01"},
 *      {"chansize", 0},
 *      {"colsize", 0},
 *      {"insertionTime", "2019-03-14T13:29:25.286Z"}
 *    };
 * </pre>
 */
    void updateTagMetaInfo(nlohmann::json& js);


/**
 * This method updates a tag meta info in the CREST database.
 * @param tagname - tag name
 * @param js - a JSON object with new parameters (These parameters will be changed in the CREST DB). Example: <br>
 * <pre>
 *    nlohmann::json js =
 *    {
 *      {"tagName", "test_MvG4"},
 *      {"description", "desc-01"},
 *      {"tagInfo", "taginf-01"},
 *      {"chansize", 0},
 *      {"colsize", 0},
 *      {"insertionTime", "2019-03-14T13:29:25.286Z"}
 *    };
 * </pre>
 */
    void updateTagMetaInfo(const std::string& tagname, nlohmann::json& js);


/**
 * This is an auxillary method, it creates a tag meta info on the file system storage.
 * @param js - tag meta info in the JSON format. Example: <br>
 * <pre>
 *    nlohmann::json js =
 *    {
 *      {"tagName", "test_MvG4"},
 *      {"description", "desc-01"},
 *      {"tagInfo", "taginf-01"},
 *      {"chansize", 0},
 *      {"colsize", 0},
 *      {"insertionTime", "2019-03-14T13:29:25.286Z"}
 *    };
 * </pre>
 */
    void createTagMetaInfoFs(nlohmann::json& js);

/**
 * This is an auxillary  method, it gets a tag meta info from file system storage.
 * @param tagname - tag name
 */
    nlohmann::json getTagMetaInfoFs(const std::string& name);


/**
 * This auxillary method updates a tag meta info in local file storage.
 * @param tagname - tag name
 * @param js - a JSON object with new parameters (These parameters will be changed in the CREST DB). Example: <br>
 * <pre>
 *    nlohmann::json js =
 *    {
 *      {"tagName", "test_MvG4"},
 *      {"description", "desc-01"},
 *      {"tagInfo", "taginf-01"},
 *      {"chansize", 0},
 *      {"colsize", 0},
 *      {"insertionTime", "2019-03-14T13:29:25.286Z"}
 *    };
 * </pre>
 */
    void updateTagMetaInfoFs(const std::string& tagname, nlohmann::json& js);

/**
 * Auxillary method to get an element by name from the Tag Meta Info.
 *
 * @param name - an element name.
 * @param js - the Tag Meta Info in JSON format. Example: <br>
 * <pre>
 *   [{"node_description": " string of the folder description "},
 *    {"channel_list" : [0, 10, 20] },
 *    {"channel_names: ["first", "second", "third"]},
 *    {"payload_specification":"stave: Int32, eta: Int32, mag: Float, base: Float, free: Float"}]
 * </pre>
 */
    std::string getTagMetaInfoElement(const std::string& name, nlohmann::json& js);


/**
 * Auxillary method to convert a Tag Meta Info from the JSON
 * CREST Server format in to IOVDbSvc format.
 *
 * @param js - the Tag Meta Info in CREST Server JSON format.
 */
    nlohmann::json convertTagMetaInfo2IOVDbSvc(nlohmann::json& js);


/**
 * Auxillary method to convert a Tag Meta Info from the JSON IOVDbSvc format
 * in CREST Server format.
 *
 * @param js - the Tag Meta Info in CREST Server JSON format.
 */
    nlohmann::json convertTagMetaInfo2CREST(nlohmann::json& js);


/**
 * Auxillary method to split a string
 *
 * @param str - a string to split.
 * @param delim - a deliminator.
 */
    std::vector<std::string> split(std::string_view str, char delim);

/**
 * This method gets a tag meta info from the CREST database in IOVDbSvc format.
 * @param tagname - tag name
 */
    nlohmann::json getTagMetaInfoIOVDbSvc(const std::string& tagname);


/**
 * This method creates a tag meta info in the CREST database.
 * @param tagname - tag name
 * @param js - tag meta info in the JSON IOVDbSvc format. Example: <br>
 * <pre>
 * {
 *    "channel_list": [
 *        {"0": "ATLAS_PREFERRED0"},
 *        {"1": "ATLAS_PREFERRED1"}
 *     ],
 *    "node_description": "description of the node",
 *    "payload_spec":
 *"AlgorithmID:UInt32,LBAvInstLumi:Float,LBAvEvtsPerBX:Float,LumiType:UInt32,Valid:UInt32,BunchInstLumi:Blob64k"
 * }
 * </pre>
 */
    void createTagMetaInfoIOVDbSvc(const std::string& tagname, nlohmann::json& js);

/**
 * This method creates a tag meta info in the CREST database.
 * @param tagname - tag name
 * @param description - tag meta info description, this parameter is a part of the tag meta info in CREST format
 * @param js - tag meta info in the JSON IOVDbSvc format. Example: <br>
 * <pre>
 * {
 *    "channel_list": [
 *        {"0": "ATLAS_PREFERRED0"},
 *        {"1": "ATLAS_PREFERRED1"}
 *     ],
 *    "node_description": "description of the node",
 *    "payload_spec":
 *"AlgorithmID:UInt32,LBAvInstLumi:Float,LBAvEvtsPerBX:Float,LumiType:UInt32,Valid:UInt32,BunchInstLumi:Blob64k"
 * }
 * </pre>
 */
    void createTagMetaInfoIOVDbSvc(const std::string& tagname, nlohmann::json& js, const std::string& description);

//=====================================================
// Global Tag and Global Tag Map methods for the file storage
//

/**
 * The auxillary method to create a global tag in file storage.
 * (This method is an analogue of the create_global_tag method in Python)
 * @param js - global tag in the JSON format. Example: <br>
 * <pre>
 * json js =
 *    {
 *       {"name","M_TEST_01"},
 *       {"validity",0},
 *       {"description","test"},
 *       {"release","1"},
 *       {"insertionTime","2018-12-18T11:32:58.081+0000"},
 *       {"snapshotTime","2018-12-18T11:32:57.952+0000"},
 *       {"scenario","test"},
 *       {"workflow","M"},
 *       {"type","t"},
 *       {"snapshotTimeMilli","null"},
 *       {"insertionTimeMilli","null"}
 *    };
 *
 * </pre>
 */
    void createGlobalTagFs(nlohmann::json& js);


/**
 * The auxillary method to find a global tag by name stored in file storage.
 * (This method is an analogue of the find_global_tag method in Python)
 * This method returns the global tag as a JSON object.
 * @param name - global tag name
 */
    nlohmann::json findGlobalTagFs(const std::string& name);


/**
 * The auxillary method to create a global tag map in file storage.
 * (This method is an analogue of the create_global_tag_map method in Python)
 * @param gt - global tag map in the JSON format. Example: <br>
 * <pre>
 * json gt =
 *    {
 *      {"globalTagName", "M_TEST"},
 *      {"record", "testing2"},
 *      {"label", "test2"},
 *      {"tagName", "test_MvG3"}
 *    }
 * </pre>
 */
    void createGlobalTagMapFs(nlohmann::json& js);

/**
 * The auxillary method to find a global tag by name for the file storage.
 * This method will search for a global tag with the given name.
 * Only one global tag should be returned.
 * (This method is an analogue of the find_global_tag method in Python)
 * This method returns the global tag as a JSON object.
 * @param name - global tag name
 */
    nlohmann::json findGlobalTagMapFs(const std::string& name);


//=====================================================
// Payloads methods for the file storage
//

/**
 * This auxillary method finds a payload resource associated to the hash
 * in the local file storage. The payload returns as a string.
 * (This method is an analogue of the get_payload method in Python)
 * @param hash - hash.
 */
    std::string getPayloadAsStringFS(const std::string& hash);

/**
 * This auxillary method finds a payload resource associated to the hash
 * in the local file storage. The payload returns as a JSON object.
 * (This method is an analogue of the get_payload method in Python)
 * @param hash - hash.
 */
    nlohmann::json getPayloadAsJsonFS(const std::string& hash);

/**
 * This auxillary method finds a payload resource associated to the hash in
 * the local file storage. This method retrieves metadata of the payload resource.
 * The result is a string.
 * (This method is an analogue of the get_payload_meta_info method in Python)
 * @param hash - hash.
 */
    std::string getPayloadMetaInfoAsStringFS(const std::string& hash);

/**
 * This auxillary method finds a payload resource associated to the hash
 * in the local file storage. This method retrieves metadata of the payload resource.
 * The result is a JSON object.
 * (This method is an analogue of the get_payload_meta_info method in Python)
 * @param hash - hash.
 */
    nlohmann::json getPayloadMetaInfoAsJsonFS(const std::string& hash);



//=====================================================
// IOV number methods for the file storage
//

/**
 * This metghod gets the number of iovs for the given tag. This method allows to select the count of iovs in a tag.
 * This method works with the file storage.
 * This method works with the file storage. (This method is an analogue of the get_size method in Python)
 * @param tagname - tag name.
 */
    int getSizeFS(const std::string& tagname);


/**
 * This method gets the number of iovs for tags matching pattern. This method allows to select the count of iovs in a
 * tag. This method works with the file storage.
 * (This method is an analogue of the get_size_by_tag method in Python)
 * The result is a JSON object.
 * @param tagname - tag name.
 */
    nlohmann::json getSizeByTagFS(const std::string& tagname);


//=====================================================
// select IOV list methods
//


/**
 * This method selects all iovs for a given tag name and for the selected time interval (since-until).
 * The result is a JSON array. It is a verion of this method with all
 * parameters. This method works with the CREST server. If until=-1 it means until is infinity,
 * in this case "until=INF" in the URL request.
 * @param tagname - tag name.
 * @param since - start time.
 * @param until - end time.
 * @param snapshot - snapshot (the default value is 0)
 */
    nlohmann::json selectIovs(const std::string& tagname, long since, long until, long snapshot = 0);


/**
 * This method selects all iovs for a given tag name and for the selected time interval (since-until).
 * The result is a JSON array. It is a verion of this method with all
 * parameters. This method works with the file storage. If until=-1 it means until is infinity,
 * in this case "until=INF" in the URL request.
 * @param tagname - tag name.
 * @param since - start time.
 * @param until - end time.
 */
    nlohmann::json selectIovsFS(const std::string& tagname, long since, long until);


//=====================================================
// Auxillary methods
//

/**
 * The auxillary method to calculate a hash code for a given string.
 * This method uses SHA256 algorithm (PicoSHA2 C++ library)
 * @param str - a string
 */
    std::string getHash(std::string_view str);

/**
 * The auxillary method to get a current data and time.
 * It used to create a payload meta info on the local file storage
 * using the storePayloadDump method.
 */
    std::string getDateAndTime();

/**
 * This auxillary method creates a JSON array from a JSON object.
 * It used to convert a single objects (such as tags and global tags to
 * JSON array in methods which retrive the data from a file starage.
 * @param js - JSON object.
 */
    nlohmann::json returnJArray(nlohmann::json js);

/**
 * This is an auxillary method for checking how many unique IOVs(hashcodes) are in an IOV list.
 * It is used to check how many payload files are in the data storage, and how this number
 * corresponds to the unique hashcode number.
 * It works with the file data storage.
 * @param tagname - tag name.
 */
    void getTagDataInfo(const std::string& tagname);
  };
} // namespace


#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


#endif // not CRESTAPI_CRESAPI_H
