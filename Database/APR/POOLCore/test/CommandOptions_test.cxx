/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLCore/CommandOptions.h"
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace pooltest{

  void stringTokenize(const std::string& str,
                      std::vector<std::string>& tokens,
                      std::string delimiters=" "){
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);
    while (std::string::npos != pos || std::string::npos != lastPos) {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
  }

  class Command {
  
  public:

    explicit Command(const std::string& command): m_args(){
      stringTokenize(command,m_args);
      m_argv = new const char*[argc()];
      for(int i=0;i<argc();i++){
        m_argv[i] = m_args[i].c_str(); 
      }
    } 

    int argc() const {
      return m_args.size();
    }
  
    const char** argv() const {
      return m_argv;
    }
  private:
    std::vector<std::string > m_args;
    const char** m_argv;
  };

  class TestCommandOptions {
  public:
    TestCommandOptions():m_optionsAndValues(),m_options(){}

    void addOption(const pool::CommandOptions::Option& opt,const std::string& value){
      m_optionsAndValues.push_back(std::make_pair(opt,value));
      m_options.push_back(opt);
    }

    bool testSuccess(const Command& commandLine) const {
      pool::CommandOptions cmd(m_options);
      try{
        cmd.parse(commandLine.argc(),const_cast<char**>(commandLine.argv()));
        std::map<std::string,std::string> userOps = cmd.userOptions();
        for(unsigned int i=0;i<m_optionsAndValues.size();i++){
          std::string prop = m_optionsAndValues[i].first.property;
          std::map<std::string,std::string>::const_iterator iter = userOps.find(prop);
          if(iter==userOps.end()){
            std::stringstream sout;
            sout << "[OVAL] test ERROR: option " << prop << " not registered.";
            throw std::runtime_error( sout.str() );
          }
          std::string val = iter->second;
          if(val!=m_optionsAndValues[i].second){
            std::stringstream sout;
            sout << "[OVAL] test ERROR: expected value <" << m_optionsAndValues[i].second << "> for option <" << prop << ">, found <" << val <<
              ">";
            throw std::runtime_error( sout.str() );
          }
          std::cout << "[OVAL] Found property <" << prop << "> with value <" << val << ">." << std::endl;
        }
      } catch (const pool::CommandOptions::OptionParseException& e){
        std::cout << "[OVAL] test ERROR: " << e.what() << std::endl;
        return false;
      } catch ( std::exception& e ) {
        std::cout << "[OVAL] test ERROR: " << e.what() << std::endl;
        return false;
      }
      return true;
    }    

    bool testFailure(const Command& commandLine) const {
      pool::CommandOptions cmd(m_options);
      try{
        cmd.parse(commandLine.argc(),const_cast<char**>(commandLine.argv()));
        std::cout << "[OVAL] test ERROR: expected exception has not been thrown." << std::endl; 
      } catch (const pool::CommandOptions::OptionParseException& e){
        std::cout << "[OVAL] expected exception: " << e.what() << std::endl;
        return true;
      } catch ( std::exception& e ) {
        std::cout << "[OVAL] test ERROR: " << e.what() << std::endl;
        return false;
      }
      return false;
    }    

  private:        
    std::vector<std::pair<pool::CommandOptions::Option,std::string> > m_optionsAndValues;  
    std::vector<pool::CommandOptions::Option> m_options;
  };


}


int main (int, char**)
{
  std::string optFileValue = "myfile.txt";
  std::string optConfigValue = "myconfig.xml";
  pooltest::TestCommandOptions test;
  pool::CommandOptions::Option optFile;
  optFile.flag = "-f";
  optFile.property = "file";
  optFile.type = pool::CommandOptions::Option::STRING;
  optFile.helpEntry = "the input file name";
  test.addOption(optFile,optFileValue);
  pool::CommandOptions::Option optVerbose;
  optVerbose.flag = "-v";
  optVerbose.property = "verbose";
  optVerbose.type = pool::CommandOptions::Option::BOOLEAN;
  optVerbose.helpEntry = "verbose mode";
  test.addOption(optVerbose,"true");
  pool::CommandOptions::Option optConfig;
  optConfig.flag = "-c";
  optConfig.property = "config";
  optConfig.type = pool::CommandOptions::Option::STRING;
  optConfig.helpEntry = "the configuration file";
  test.addOption(optConfig,optConfigValue);

  // command line with short sintax options
  std::stringstream cmdOut1;
  cmdOut1 << "Command " << optFile.flag << " " << optFileValue << 
    " " << optVerbose.flag <<
    " " << optConfig.flag << " " << optConfigValue << " ";
  std::cout << "[OVAL] command= " << cmdOut1.str() << std::endl;
  pooltest::Command command1(cmdOut1.str()); 
  test.testSuccess(command1);
  std::cout << std::endl;

  // command line with full sintax options
  std::stringstream cmdOut2;
  cmdOut2 << "Command --" << optFile.property << " " << optFileValue << 
    " --" << optVerbose.property <<
    " --" << optConfig.property << " " << optConfigValue << " ";
  std::cout << "[OVAL] command= " << cmdOut2.str() << std::endl;
  pooltest::Command command2(cmdOut2.str()); 
  test.testSuccess(command2);
  std::cout << std::endl;

  // failure for unsupported option
  std::stringstream cmdOut3;
  cmdOut3 << "Command " << optFile.flag << " " << optFileValue << 
    " " << optVerbose.flag <<
    " " << optConfig.flag << " " << optConfigValue << " -s pino ";
  std::cout << "[OVAL] command= " << cmdOut3.str() << std::endl;
  pooltest::Command command3(cmdOut3.str()); 
  test.testFailure(command3);
  std::cout << std::endl;

  // failure for missing parameter value
  std::stringstream cmdOut4;
  cmdOut4 << "Command " << optFile.flag << " " << optVerbose.flag <<
    " " << optConfig.flag << " " << optConfigValue << " ";
  std::cout << "[OVAL] command= " << cmdOut4.str() << std::endl;
  pooltest::Command command4(cmdOut4.str()); 
  test.testFailure(command4);
  std::cout << std::endl;

  // failure for unexpected parameter value
  std::stringstream cmdOut5;
  cmdOut5 << "Command " << optFile.flag << " " << optFileValue << 
    " " << optVerbose.flag << " verbose" << 
    " " << optConfig.flag << " " << optConfigValue << " ";
  std::cout << "[OVAL] command= " << cmdOut5.str() << std::endl;
  pooltest::Command command5(cmdOut5.str()); 
  test.testFailure(command5);
  std::cout << std::endl;
  return 0;
}
