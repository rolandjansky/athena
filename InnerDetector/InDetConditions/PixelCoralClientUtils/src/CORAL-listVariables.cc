/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/checker_macros.h"
#include "PixelCoralClientUtils/CoralClient.hh"
#include <string>
#include <vector>
#include <cstring>


  class DummyCoralClientListener : public IPixResultsDbListener {
  public:

  protected:
    void newValue(const std::string &var_name, const std::string &conn_name, bool value) {
      std::cout << "INFO [DummyCoralClientListener::newValue<bool>] var_name = " << var_name  << ", conn_name = " << conn_name
		<< " value = " << (value ? "true" : "false") << std::endl;
    }
    void newValue(const std::string &var_name, const std::string &conn_name, unsigned int value) {
      std::cout << "INFO [DummyCoralClientListener::newValue<unsigned int>] var_name = " << var_name  << ", conn_name = " << conn_name
		<< " value = " << value << std::endl;
    }
    void newValue(const std::string &var_name, const std::string &conn_name, float value) {
      std::cout << "INFO [DummyCoralClientListener::newValue<float>] var_name = " << var_name  << ", conn_name = " << conn_name
		<< " value = " << value << std::endl;
    }
    void newValue(const std::string &var_name, const std::string &conn_name, const CAN::AverageResult_t &value) {
      std::cout << "INFO [DummyCoralClientListener::newValue<AverageResult_t>] var_name = " << var_name  << ", conn_name = " << conn_name
		<< " value = " << value.value(CAN::AverageResult_t::kLower99)
		<< " < " << value.value(CAN::AverageResult_t::kMean)  << " +- " << value.value(CAN::AverageResult_t::kRMS)
		<< " < " << value.value(CAN::AverageResult_t::kUpper99)
		<< std::endl;
    }
    void newValue(const std::string &var_name, const std::string &conn_name, const PixelMap_t & value) {
      std::cout << "INFO [DummyCoralClientListener::newValue<PixelMap_t>] var_name = " << var_name  << ", conn_name = " << conn_name<< std::endl;
      std::cout<< " row/column status"<< std::endl;
      for (PixelMap_t::const_iterator it=value.begin(); it != value.end(); ++it)
	std::cout<<" "<<(*it).first.second<<"/"<<(*it).first.first<<" "<<(*it).second<< std::endl;//CLA
    }
  };


int main (int argc, char **argv){

  CAN::SerialNumber_t analysis_id = 0;
  std::vector<std::string> connectivity_name;
  bool error=false;

  for (int arg_i=1; arg_i<argc; arg_i++) {
    if (strcmp(argv[arg_i],"-s")==0 && arg_i+1<argc && argv[arg_i+1][0]!='-') {
      analysis_id = atoi(argv[++arg_i]);
    }
    else if (strcmp(argv[arg_i],"-c")==0 && arg_i+1<argc && argv[arg_i+1][0]!='-') {
      arg_i++;
      do {
		
	connectivity_name.push_back(  argv[arg_i] );

      } while ( ++arg_i<argc && argv[arg_i][0]!='-' );

    }
    else {
      std::cout << "ERROR [" << argv[0] << ":main] unsupported argument : " << argv[arg_i] << "." << std::endl;
      error=true;
      break;
    }
  }
  if (analysis_id<=0 || error) {
    std::cout << "USAGE " << argv[0] << " -s analysis-serial-number [-c connectivity-name ... ]" << std::endl;
    return -1;
  }


  // --------------------------------------------------
  // Main part
  // --------------------------------------------------

  try {

    PixCoralClient coralClient(true);
    DummyCoralClientListener listener;

    coralClient.getAnalysisResultsFromDB(analysis_id,connectivity_name, listener);

  } catch ( std::exception& e ) {
    std::cout << "std::exception caught: " << e.what() << std::endl;
    return -1;
  }

  catch (...) {
    std::cout << "Unknown exception caught!" << std::endl;
    return -1;
  }

  return 0;

}
                                                      



