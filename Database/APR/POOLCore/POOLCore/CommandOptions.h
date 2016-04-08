/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef POOLCORE_COMMANDOPTIONS_H
#define POOLCORE_COMMANDOPTIONS_H

#include <string>
#include <vector>
#include <map>
#include "Exception.h"

namespace pool {

  /**
     @class CommandOptions CommandOptions.h POOLCore/CommandOptions.h

     @brief Utility class for handling input parameters of command line executables.

     Utility class for handling input parameters of command line executables.
     It provides parsing of the command line parameters according to pre-defined options, automated creatiion of help, error generation on misusage. 

  */
  class CommandOptions {

  public:

    // structure defining the supported option parameters
    struct Option {
    
      typedef enum { BOOLEAN,
                     STRING } OptionType;
      // default constructor
      Option();

      //copy constructor
      Option(const Option& anOpt);

      //assignment operator
      Option& operator=(const Option& anOpt);      

      // type of the option (simple boolean or parameter with an associated value) 
      OptionType type;

      // label defining the option on the command line
      std::string flag;

      // property name associated to the flag 
      std::string property;

      // explanation sentence appearing on the help
      std::string helpEntry;

      // default value associated to the property when the option is not set
      std::string defaultValue;

      // flag to declare the option as required
      bool required;

      // method for comparison with an option flag or property
      bool isEqual(const std::string& s) const;
    };

    // marker for the option strings 
    static const char& optionMarker();

    static const Option& helpOption();

    // exception class for the failures in parsing procedure
    class OptionParseException  : public pool::Exception {
    public:
      /// Constructor
      explicit OptionParseException( const std::string& message );
    };

    // constructor - requires an array of Option structures
    explicit CommandOptions(const std::vector<Option>& supportedOptions);

    // activate the parsing of the command line parameters
    bool parse(int argc,char** argv);

    // returns a map of the option set by the user with their associated values
    const std::map<std::string,std::string>& userOptions() const;

    // returns the executable name of the command
    std::string commandName() const;

    // displays the available options defined
    void help() const;

    // displays the options set by the user with their associated values
    void dumpUserOptions() const;

  private:

    // internal method to look up for specific option property or flag
    std::vector<Option>::const_iterator searchOption(const std::string& s) const;

    // array of supported option (described by Option structures)
    std::vector<Option> m_supportedOptions;

    // map op option set by the user with their associated values
    std::map<std::string,std::string> m_options;

    std::vector<std::string> m_requiredOptions;

    // executable name of the command
    std::string m_commandName;
  };
}

#endif
