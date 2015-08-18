/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TokenExtractor.cpp
 * @brief Utility to read the event references of a POOL collection and send POOL tokens over a Yampl channel
 */

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionService.h"

#include "POOLCore/Exception.h"

#include "yampl/SocketFactory.h"

#include <cstring>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <curl/curl.h>

typedef std::vector<std::string> TokenVector;              // Tokens in the POOL file ordered by OID (position in file) 
typedef std::map<std::string,TokenVector> MapTokensByGuid; // Token lists by POOL File GUID

// ________________ CLI ________________
struct CLI {
  bool help;
  bool useEI;
  bool verbose;
  char* source;
  char* yampl;
};

int parseCLI(int argc, char** argv, CLI& cli)
{
  int opt;
  int option_index(0);

  struct option long_options[] = {
    {"source",    required_argument, 0, 's'}
    , {"yampl",   required_argument, 0, 'y'}
    , {"useEI",   no_argument,       0, 'e'}
    , {"verbose", no_argument,       0, 'v'}
    , {"help",    no_argument,       0, 'h'}
    , {0, 0, 0, 0}
  };

  while((opt=getopt_long(argc,argv,"hves:y:",long_options,&option_index))!=-1) {
    switch(opt) {
    case 'h':
      cli.help = true;
      break;
    case 'e':
      cli.useEI = true;
      break;
    case 'v':
      cli.verbose = true;
      break;
    case 's':
      cli.source = optarg;
      break;
    case 'y':
      cli.yampl = optarg;
      break;
    case '?':
      return 1;
    default:
      std::cerr << "ERROR: Unexpected error while parsing command-line options" << std::endl;
      return 1;
    }
  }

  if(optind<argc) {
    std::cerr << "ERROR: Unexpected non-option command-line arguments: " << std::endl;
    while(optind<argc) std::cerr << '\t' << argv[optind++] << std::endl;
    return 1;
  }

  if(cli.source==0 && !cli.help) {
    std::cerr << "ERROR: -s is a required option!" << std::endl;
    return 1;
  }

  return 0;
}

void printUsage()
{
  std::cout << std::endl << "Usage:" << std::endl;
  std::cout << "\tTokenExtractor [Options] -s|--source <source>" << std::endl << std::endl;
  std::cout << std::endl << "Required parameter:" << std::endl;
  std::cout << "\t-s|--source : Source for reading POOL Tokens. Either an ASCII file with POOL GUID to TAG file mappings, or an URL to Event Index" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "\t-e|--useEI                : Use Event Index for reading POOL Tokens (default is to use TAG files)" << std::endl;
  std::cout << "\t-y|--yampl <channel_name> : The name of Yampl Channel for communicating with Token Clients (default is TokenExtractorChannel)" << std::endl;
  std::cout << "\t-v|--verbose              : Run in verbose mode" << std::endl;
  std::cout << "\t-h|--help                 : Print usage and exit" << std::endl << std::endl;
}

void printConfiguration(const CLI& cli)
{
  std::cout << std::endl << "***** Token Extractor Configuration:" << std::endl;
  std::cout << "* Read tokens from " << (cli.useEI?"Event Index":"TAG") << std::endl;
  if (cli.source)
    std::cout << "* Source: " << cli.source << std::endl;
  std::cout << "* Yampl Channel: " << (cli.yampl?cli.yampl:"TokenExtractorChannel") << std::endl;
  std::cout << "* Verbose: " << (cli.verbose?"Yes":"No") << std::endl;
  std::cout << "**********************" << std::endl << std::endl;
}
// ________________ CLI ________________

// _______________ curl ________________
struct TokenBuf
{
  void* buf;
  size_t size;
};

size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  struct TokenBuf* tokenbuf = (struct TokenBuf*)data;
  tokenbuf->buf = realloc(tokenbuf->buf,tokenbuf->size+nmemb);
  memcpy((void*)((char*)tokenbuf->buf + tokenbuf->size),ptr,nmemb);
  tokenbuf->size = tokenbuf->size + nmemb;

  return (size_t)(size * nmemb);
}
// _______________ curl ________________

MapTokensByGuid::const_iterator retrieveTokens(bool useEI
					       , bool verbose
					       , char* source
					       , const std::string& guid
					       , MapTokensByGuid& tokensByGuid
					       , std::string& errorMessage)
{
  MapTokensByGuid::const_iterator retVal(tokensByGuid.end());
  if(useEI) {
    // Read tokens from the Event Index web service using curl
    std::cout << "Reading tokens from the Event Index" << std::endl;

    // Compose the URL
    // NB!!! It is expected that the source looks like this: 'https://aiatlas016.cern.ch/EIHadoop/ES.jsp?query=path:EICache/atlevind/2014_6_31_13_53_7_893&guid='
    //       And we want to turn it into something like that: 'https://aiatlas016.cern.ch/EIHadoop/ES.jsp?query=path:EICache/atlevind/2014_6_31_13_53_7_893&guid=4E5EDBC7-5F6D-E111-B20A-003048F3524E'
    std::string url(source);
    url += guid;

    if(verbose) std::cout << "URL :" << url << std::endl;

    // Initialize curl
    CURL *curl;
    CURLcode res;
    struct TokenBuf tokenBuf;
    tokenBuf.buf = 0;
    tokenBuf.size = 0;

    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // "http://wn181.ific.uv.es:8080/getIndex.jsp?format=txt2&guid=4E5EDBC7-5F6D-E111-B20A-003048F3524E"
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &tokenBuf);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

      // Perform the request, res will get the return code 
      res = curl_easy_perform(curl);
      // Check for errors
      if(res != CURLE_OK) {
	errorMessage.clear();
	errorMessage = "CURL curl_easy_perform() failed! " + std::string(curl_easy_strerror(res));
	std::cerr <<  "ERROR: " << errorMessage << std::endl;
	return retVal;
      }
      else {
	std::cout << "Token reading OK" << std::endl;
      }

      // Always cleanup 
      curl_easy_cleanup(curl);
    }
    else  {
      errorMessage.clear();
      errorMessage = "CURL curl_easy_init() failed!";
      std::cerr << "ERROR: " << errorMessage << std::endl;
      return retVal;
    }

    // As a result of this operation we have a buffer, which contains the tokens separated by \n
    // We need to parse the buffer and store the tokens in a new entry of the tokensByGuid map

    // Add new entry to the tokensByGuid map
    auto itTokensByGuid = tokensByGuid.insert(std::pair<std::string,TokenVector>(guid,TokenVector())).first;
    TokenVector& newTokens = itTokensByGuid->second;

    // Fill it in
    char* charbuf = (char*)tokenBuf.buf;
    size_t startpos(0);
    size_t i;
    for(i=0; i<tokenBuf.size; ++i) {
      if(charbuf[i]=='\n' || charbuf[i]=='\r') {
	// Ignore empty lines
	if(i!=startpos) {
	  std::string token(&(charbuf[startpos]),i-startpos);
	  if(verbose) std::cout << "Read a line from the Event Index: \'" << token << "\'" << std::endl;
	  // Trim the token
	  token.erase(0,token.find_first_not_of(" \t"));
	  size_t spacepos = token.find_last_not_of(" \t");
	  if(spacepos!=std::string::npos) token.erase(spacepos+1);
	  
	  // Error handling
	  // In certain cases (eg wrong URL) instead of the list of tokens, the server
	  // returns a HTML, which contains the word Error 
	  if(token.find("Error")!=std::string::npos
	     || token.find("ERROR")!=std::string::npos) {
	    errorMessage.clear();
	    errorMessage = "URL " + token;
	    std::cerr << "ERROR: " << errorMessage << std::endl;
	    tokensByGuid.erase(itTokensByGuid);
	    return retVal;
	  }

	  if(token.size()) {
	    if(verbose) std::cout << "TOKEN : " << token << std::endl;
	    newTokens.push_back(token);
	  }
	}
	startpos = i+1;
      }
    }

    if(newTokens.empty()) {
      errorMessage.clear();
      errorMessage = "URL No tokens for GUID " + guid;
      tokensByGuid.erase(itTokensByGuid);
      itTokensByGuid = tokensByGuid.end();
    }
    retVal = itTokensByGuid;
  }
  else {
    // Read tokens from the local TAG file
    std::cout << "Reading tokens from TAG file" << std::endl;    
    // First we need to open and ASCII file, which contains POOL GUID to TAG filename mapping
    // The name of this ASCII file is provided by the source argument
    // The expected format: <POOL file GUID>,<TAG file name> pairs, one per line
    std::string tagFileName("");
    std::ifstream ifstr;
    ifstr.open(source,std::ifstream::in);
    if(ifstr.fail()) {
      std::cerr << "Unable to open " << source << " for reading" << std::endl;
      return retVal;
    }
    if(verbose) std::cout << "Source file : " << source << std::endl;
    while(ifstr.good()) {
      std::string line;
      ifstr >> line;
      if(line.empty()) continue;
      size_t comapos = line.find(',');
      if(comapos==std::string::npos || comapos==0 || comapos==line.size()-1
	 || line.find(',',comapos+1)!=std::string::npos) {
	std::cerr << "WARNING: Ignoring the line with wrong format in the input file: " << line << std::endl;
	continue;
      }
      if(line.substr(0,comapos)==guid) {
	// Found the GUID. Get the TAG file name
	tagFileName = line.substr(comapos+1);
	if(verbose) std::cout << "TAG file : " << tagFileName << std::endl; 
	break;
      }
    } // Reading the input ASCII file
    ifstr.close();
    if(tagFileName.empty()) {
      std::cerr << "Unable to find GUID=" << guid << " in the input file " << source << std::endl;
      return retVal;
    }

    MapTokensByGuid::iterator itTokensByGuid;
    try {
      // Open the TAG file and get all of its tokens
      pool::CollectionService   collectionService;
      pool::ICollection *collection = collectionService.handle(tagFileName,"RootCollection","",true);
      pool::ICollectionQuery *collQuery = collection->newQuery();
      collQuery->selectAllTokens();
      pool::ICollectionCursor& cursor = collQuery->execute();
      std::cout << "Token reading OK" << std::endl;

      // Add new entry to the tokensByGuid map
      itTokensByGuid = tokensByGuid.insert(std::pair<std::string,TokenVector>(guid,TokenVector())).first;
      TokenVector& newTokens = itTokensByGuid->second;
      
      while(cursor.next()) {
	const pool::TokenList &tokens = cursor.currentRow().tokenList();
	if(tokens.begin()!=tokens.end()) {
	  // !!! NB !!!
	  // Here we assume that we work with EVGEN files !
	  // This means, for each event there is only one POOL token.
	  // For other types of input files this algorithm needs to be revised
	  // !!! NB !!!
	  pool::TokenList::const_iterator iter = tokens.begin();
	  newTokens.push_back(iter->toString());
	  if(verbose) std::cout << "TOKEN : " << iter->toString() << std::endl; 
	}
      }
      collection->close();
    }
    catch( pool::Exception& poolException ) {
      std::cerr << "pool::Exception caught! " << poolException.what() << std::endl;;
      return retVal;
    }

    retVal = itTokensByGuid;
  }
  return retVal;
}

int main(int argc, char *argv[])
{
  // __________________ Parse command line _________________________
  CLI cli{false,false,false,0,0};

  if(parseCLI(argc,argv,cli)) {
    printUsage();
    return 1;
  }

  if(cli.help) {
    printUsage();
    return 0;
  }
  // Hint to coverity: cli.source must be non-null here.
  if (!cli.source) std::abort();

  printConfiguration(cli);

  // ______________ Create container for caching the retrieved Tokens _______________
  MapTokensByGuid tokensByGuid;

  // ______________ Construct the Server Socket and start waiting for requests ______________
  std::string socketName(cli.yampl?cli.yampl:"TokenExtractorChannel");
  yampl::ISocketFactory* socketFactory = new yampl::SocketFactory();
  yampl::ISocket* socket = socketFactory->createServerSocket(yampl::Channel(socketName,yampl::LOCAL_PIPE),yampl::MOVE_DATA);

  while(true) {
    char *requestBuffer(0), *responseBuffer(0);
    std::string strEvtTokens("");

    // In case of an error respond to the request with two messages
    //
    // Message 1: Error Code (unsigned): 0 - GLOBAL, 1 - RANGE
    // Message 2: Error string
    unsigned* errorCode = new unsigned(0);
    std::string errorMessage;

    std::cout << "\nWaiting for an incoming request ..." << std::endl;
    ssize_t requestSize = socket->recv(requestBuffer);
    std::cout << "Request received." << std::endl;

    if(requestSize==1) { // Empty request received. Break the loop
      std::cout << "Empty request. Exiting ...." << std::endl;
      break;
    }

    if(requestSize==-1) {
      errorMessage = "Failed to receive a Range over the yampl channel. Ignoring this Range";
      std::cerr << "ERROR: " << errorMessage << std::endl;
      *errorCode = 1;
      socket->send((void*)errorCode,sizeof(unsigned));
      void* errorMess = malloc(errorMessage.size());
      memcpy(errorMess,errorMessage.data(),errorMessage.size());
      socket->send(errorMess,errorMessage.size());
      continue;
    }

    // Decode the request: GUID,evtNumber[,evtNumber]
    std::string strRequest(requestBuffer,requestSize);
    if(cli.verbose) std::cout << "Request: " << strRequest << std::endl;
    size_t comapos = strRequest.find(',');
    if(comapos==std::string::npos) {
      errorMessage = "Wrong format of the Range: " + strRequest;
      std::cerr << "ERROR: " << errorMessage << std::endl;
      *errorCode = 1;
      socket->send((void*)errorCode,sizeof(unsigned));
      void* errorMess = malloc(errorMessage.size());
      memcpy(errorMess,errorMessage.data(),errorMessage.size());
      socket->send(errorMess,errorMessage.size());
      continue;
    }

    std::string guid = strRequest.substr(0,comapos);
    std::cout << "GUID=" << guid << std::endl;
      
    // _____________ Locate GUID in the tokens map. If does not exist, then retrieve from the source __________________
    MapTokensByGuid::const_iterator guidIterator = tokensByGuid.find(guid);
    if(guidIterator==tokensByGuid.end()) {
      guidIterator = retrieveTokens(cli.useEI,cli.verbose,cli.source,guid,tokensByGuid,errorMessage);
      if(guidIterator==tokensByGuid.end()) {
	std::cerr << "ERROR: unable to retrieve event tokens for GUID=" << guid << std::endl;
	socket->send((void*)errorCode,sizeof(unsigned));
	void* errorMess = malloc(errorMessage.size());
	memcpy(errorMess,errorMessage.data(),errorMessage.size());
	socket->send(errorMess,errorMessage.size());
	continue;
      }
    }

    const TokenVector& cachedTokens = guidIterator->second;
    // ________________________ Parse the string with event numbers, get corresponding Tokens 
    // and store the Tokens in a coma-separated string strEvtTokens __________________________
    size_t startpos(comapos+1);
    size_t endpos = strRequest.find(',',startpos);
    unsigned evtPos(0);
    bool all_ok(true);
    while(endpos!=std::string::npos) {
      std::string strEvtPos(strRequest.substr(startpos,endpos-startpos));
      evtPos = std::atoi(strEvtPos.c_str());
      std::cout << "Event number = " << evtPos << std::endl;
      startpos = endpos+1;
      endpos = strRequest.find(',',startpos);
      if(evtPos<=cachedTokens.size() && evtPos>0) {
	if(!strEvtTokens.empty())
	  strEvtTokens += std::string(",");
	strEvtTokens += cachedTokens[evtPos-1];
	if(cli.verbose) std::cout << "Corresponding Token  " << cachedTokens[evtPos-1] << std::endl;
      }
      else {
	errorMessage = "Range contains wrong positional number " + strEvtPos;
	std::cerr << "ERROR:  " << errorMessage << std::endl;
	all_ok = false;
	break;
      }
    }
    if(all_ok) {
      std::string strEvtPos(strRequest.substr(startpos,endpos-startpos));
      evtPos = std::atoi(strEvtPos.c_str());
      std::cout << "Event number = " << evtPos << std::endl;
      if(evtPos<=cachedTokens.size() && evtPos>0) {
	if(!strEvtTokens.empty())
	  strEvtTokens += std::string(",");
	strEvtTokens += cachedTokens[evtPos-1];
	if(cli.verbose) std::cout << "Corresponding Token  " << cachedTokens[evtPos-1] << std::endl;
      }
      else {
	errorMessage = "Range contains wrong positional number " + strEvtPos;
	std::cerr << "ERROR:  " << errorMessage << std::endl;
	all_ok = false;
      }
    }

    if(all_ok) {
      if(cli.verbose) std::cout << "Response message: " << strEvtTokens << std::endl;
      // Construct a response buffer: evtToken[,evtToken] 
      responseBuffer = (char*)malloc(strEvtTokens.size());
      memcpy(responseBuffer,strEvtTokens.data(),strEvtTokens.size());
      socket->send(responseBuffer,strEvtTokens.size());
      std::cout << "Response sent" << std::endl;
    }
    else {
      *errorCode = 1;
      socket->send((void*)errorCode,sizeof(unsigned));
      void* errorMess = malloc(errorMessage.size());
      memcpy(errorMess,errorMessage.data(),errorMessage.size());
      socket->send(errorMess,errorMessage.size());      
      continue; // In order to avoid double delete of the errorCode;
    }
    
    delete errorCode;
  }

  delete socket;
  return 0;  
}
