/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   DBObjectBuilder.h
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2018-01-16
 * 
 * @brief  Header file for base class for database objects builders
 * 
 */

#ifndef AFP_DBTOOLS_DBOBJECTBUILDER_H
#define AFP_DBTOOLS_DBOBJECTBUILDER_H

#include "DBObjectBuilderBase.h"

#include <boost/property_tree/ptree.hpp>

#include <vector>
#include <string>
#include <list>

namespace AFP
{

  /// @brief Base class for builders producing objects based on database information.
  ///
  /// Classes building database objects should derive from this
  /// template. It should be enough to override only
  /// DBObjectBuilderBase::addBuilderCommand function.
  template<typename OUTPUT_TYPE>
  class DBObjectBuilder : public DBObjectBuilderBase
  {
  public:
    /// Deletes #m_inputData
    virtual ~DBObjectBuilder () {for (CommandBase* comm : m_commands) delete comm;}

    /// The method sets value of the given field of #m_object.
    ///
    /// @param field pointer to class member field which value is to be set
    /// @param value value to be assigned to the field
    template <typename VALUE_TYPE>
    void setFieldValue (VALUE_TYPE OUTPUT_TYPE::* const field, const VALUE_TYPE value)
    {if (field) m_object->*field = value;}

    /// @brief Method creating a database object based on passed input data.
    ///
    /// The method creates a new output database object and fills with
    /// data from the input argument.
    OUTPUT_TYPE build (const boost::property_tree::ptree::value_type& inputData);

    /// @copydoc DBObjectbuilder::generateBuilderCommands
    std::string generateBuilderCommands () override final;

    /// @brief This method adds new command objects to the list depending on argument
    ///
    /// This is the only method that needs to be overridden when
    /// creating a new builder.
    ///
    /// @param name name of the element in database
    ///
    /// @return true if name was recognised, false if not
    virtual bool addBuilderCommand (const std::string& name) = 0;

  protected:
    // --------------- NESTED CLASSES ---------------
    /// Nested class base for storing commands for building objects
    class CommandBase
    {
    public:
      /// Does nothing
      virtual ~CommandBase() {}

      /// Execute method which calls proper method of the DBObjectBuilder in order to create output object.
      virtual void executeSetFieldValue (const boost::property_tree::ptree::value_type& inputData, DBObjectBuilder<OUTPUT_TYPE>* parentObject) const = 0;
    };

    /// Template of nested command class that calls the method setting the object value with proper type.
    template <typename VALUE_TYPE>
    class Command : public CommandBase
    {
    public:
      /// Sets pointer to the output object field to be set
      Command (VALUE_TYPE OUTPUT_TYPE::*field) : m_field (field) {}

      /// Does nothing
      virtual ~Command() {}

      /// @copydoc CommandBase::executeSetfieldvalue
      void executeSetFieldValue (const boost::property_tree::ptree::value_type& inputData, DBObjectBuilder<OUTPUT_TYPE>* parentObject) const override
      {parentObject->setFieldValue<VALUE_TYPE>(m_field, inputData.second.get_value<VALUE_TYPE>());}

    protected:
      /// Pointer to class member to be set by this command.
      VALUE_TYPE OUTPUT_TYPE::*m_field;
    };
    // --------------- END NESTED CLASSES ---------------

    /// List of commands used for creating an object. The order is important.
    std::list<CommandBase*> m_commands;

    /// Pointer to the conditions object that is being set. Used by the DBObjectBuilder::build method.
    OUTPUT_TYPE* m_object;

  };				// end of DBObjectBuilder class declaration


  // ==============================================
  // =============== IMPLEMENTATION ===============
  // ==============================================

  template<typename OUTPUT_TYPE>
  std::string DBObjectBuilder<OUTPUT_TYPE>::generateBuilderCommands ()
  {
    // clear list of commnads
    for (CommandBase* comm : this->m_commands)
      delete comm;
    this->m_commands.clear();


    // fill list with commands
    std::stringstream unknownVariables;
    for (const std::vector<std::string>& element : this->m_specification)
      if (addBuilderCommand(element[0]) == false)
	unknownVariables<<element[0]<<", ";

    std::stringstream warningMessage;
    // if list of unknown variables is not empty generate warning message
    if (!unknownVariables.str().empty())
      warningMessage <<"DBObjectBuilder will skip following variables present in database: "<<unknownVariables.str();

    return warningMessage.str();
  }


  template<typename OUTPUT_TYPE>
  OUTPUT_TYPE DBObjectBuilder<OUTPUT_TYPE>::build (const boost::property_tree::ptree::value_type& inputData)
  {
    OUTPUT_TYPE output;
    m_object = &output;
    typedef std::list<DBObjectBuilder<OUTPUT_TYPE>::CommandBase*> CommandsList_t;

    typename CommandsList_t::const_iterator commandIter = m_commands.begin();
    const typename CommandsList_t::const_iterator commandIterEnd = m_commands.end();
    for (const boost::property_tree::ptree::value_type& data : inputData.second) {
      if (commandIterEnd != commandIter) {
	DBObjectBuilder<OUTPUT_TYPE>::CommandBase* comm = *commandIter;
	++commandIter;
	
	comm->executeSetFieldValue(data, this);
      }
    }

    m_object = nullptr;

    return output;
  }


} // namespace AFP

#endif	//  AFP_DBTOOLS_DBOBJECTBUILDER_H
