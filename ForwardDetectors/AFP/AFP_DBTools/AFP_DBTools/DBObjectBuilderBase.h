/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   DBObjectBuilderBase.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2018-01-19
 * 
 * @brief  Header file for DBObjectBuilderBase class
 * 
 */

#include<string>
#include<vector>

namespace AFP
{

  /// @brief Class parsing conditions database specification
  ///
  /// This class contains DBObjectBuilder methods which do not need
  /// templates.
  class DBObjectBuilderBase
  {
  public:
    /// Does nothing
    virtual ~DBObjectBuilderBase() {}

    /// @brief Parses specification string from database and prepare
    /// list of builder methods.
    ///
    /// @param specification a string with specification format is
    /// described in:
    /// https://twiki.cern.ch/twiki/bin/view/AtlasComputing/JsonFormatForCrest
    ///
    /// @return string with warning message - should be used to report
    /// not recognised variables names in the database (the message is
    /// returned instead of being printed because this is not Athena
    /// algorithm)
    std::string setSpecification (const std::string specification);
    
    /// @brief Creates a list of Command objects to be called when
    /// reading database record.
    ///
    /// This method generates a list of builder command objects
    /// DBObjectBuilder::m_commands that will construct output object
    /// in the same order as information in the database is
    /// saved. When object is being build the input data and list of
    /// commands are iterated simultaneously and input data element is
    /// passed as an argument to the corresponding command from the
    /// list.
    ///
    /// The list of commands is generated based on #m_specification.
    ///
    /// @note For each field in the database there should be a
    /// corresponding command object. If the database field is not
    /// processed a command object with nullptr should be created.
    ///
    /// @return string with warning message - should be used to report
    /// not recognised variables names in the database (the message is
    /// returned instead of being printed because this is not Athena
    /// algorithm)
    virtual std::string generateBuilderCommands () = 0;

  protected:
    /// Calls DBObjectBuilderBase::trimSpaces (std::string) for each string in the vector
    void trimSpaces (std::vector<std::string>& toTrim) const;

    /// Removes spaces from the beginning and end of the string
    void trimSpaces (std::string& toTrim) const;

    /// @brief Parses payload specification string and saves output to vector
    ///

    /// Payload specification stream format is described in:
    /// https://twiki.cern.ch/twiki/bin/view/AtlasComputing/JsonFormatForCrest
    /// The function splits the string into pairs name-type and saves
    /// them as a vector of strings with two elements. All pairs are
    /// added to a global output vector.
    void parsePayloadSpec (const std::string& input, std::vector< std::vector<std::string> >& output) const;

    /// @brief vector describing format of the data saved in the database
    ///
    /// A vector of vectors. Each subvector contains exactly two
    /// elements: name and type. The elements are in the same order as
    /// data.
    std::vector< std::vector<std::string> > m_specification;
  };

} // AFP namespace 
