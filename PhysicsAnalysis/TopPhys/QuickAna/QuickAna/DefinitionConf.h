/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__DEFINITION_CONF_H
#define QUICK_ANA__DEFINITION_CONF_H

//        Copyright Iowa State University 2015.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.


#include <QuickAna/Global.h>

#include <string>
#include <vector>

namespace ana
{
  /// \brief the configuration information for an individual object
  /// definition
  ///
  /// this is meant as a temporary until Steve's new configuration
  /// mechanism is in place

  struct DefinitionConf
  {
    //
    // public interface
    //

    /// \brief make the tools from this definition
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool creation failures
  public:
    StatusCode makeTools (const DefinitionArgs& args,
			  AnaToolSequence& sequence);


    /// \brief the object type of this definition
    /// \par Guarantee
    ///   no-fail
  public:
    ObjectType defType () const;

    /// \brief set the value of \ref defType
    /// \par Guarantee
    ///   no-fail
  public:
    void setDefType (ObjectType val_defType);


    /// \brief the name of the definition point
    /// \sa wpName
    /// \par Guarantee
    ///   no-fail
  public:
    const std::string& defName () const;

    /// \brief set the value of \ref defName
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    void setDefName (const std::string& val_defName);


    /// \brief the name of the working point, or the empty string if
    /// this is not a working point
    ///
    /// For now this is the same as \ref defName but some point this
    /// should probably be split in two: One specifying the name of
    /// the definition, the other the name of the decoration, so that
    /// the user can use the same working point multiple times with
    /// slightly different tool configurations.
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    std::string wpName () const;


    /// \brief whether this is the first working point for this
    /// definition
    /// \par Guarantee
    ///   no-fail
  public:
    bool firstWP () const;

    /// \brief set the value of \ref firstWP
    /// \par Guarantee
    ///   no-fail
  public:
    void setFirstWP (bool val_firstWP);


    /// \brief whether we have multiple working points for this
    /// definition
    /// \par Guarantee
    ///   no-fail
  public:
    bool multiWP () const;

    /// \brief set the value of \ref multiWP
    /// \par Guarantee
    ///   no-fail
  public:
    void setMultiWP (bool val_multiWP);


    /// \brief make the configuration object for object definitions
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    static DefinitionConf
    makeObjectDefinitionConf (ObjectType type, const std::string& name);


    /// \brief make the configuration object for the IParticle common
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    static DefinitionConf
    makeIParticleCommonConf (ObjectType type,
			     const std::vector<std::string>& workingPoints);


    /// \brief make the configuration object for the kinematic selection
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    static DefinitionConf
    makeKinSelectConf (ObjectType type, const std::string& kinSelect);


    /// \brief make the configuration object for the EventSelect base tool
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    static DefinitionConf
    makeEventSelectBaseConf ();


    /// \brief make the configuration object for the IsData tool
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    static DefinitionConf
    makeIsDataConf ();


    /// \brief make the configuration object for the pileup
    /// reweighting tool
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    static DefinitionConf
    makePileupConf (const std::vector<std::string>& muDataFiles,
		    const std::vector<std::string>& muMcFiles);


    /// \brief make the configuration list from the global
    /// configuration
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    static StatusCode
    makeConfigList (std::vector<DefinitionConf>& confList,
		    const Configuration& config);



    //
    // private interface
    //

    /// \brief the value of \ref defType
  private:
    ObjectType m_defType = OBJECT_JET;

    /// \brief the value of \ref defName
  private:
    std::string m_defName;

    /// \brief the value of \ref firstWP
  private:
    bool m_firstWP = true;

    /// \brief the value of \ref multiWP
  private:
    bool m_multiWP = true;


    /// \brief the different modes of configuration
    ///
    /// Didn't quite know how to call this.  Essentially I want to be
    /// able to create all tools in one go through a DefinitionConf
    /// list.  To do that, I need to have special modes for all the
    /// tools that are not part of an object definition
    ///
    /// For a more sophisticated implementation this could potentially
    /// be extended to use a class hierarchy here, or extend the
    /// factory method from the object definitions.
  private:
    enum class Mode
    {
      /// \brief an actual object definition
      OBJECT_DEFINITION,

      /// \brief the IParticle common definition
	IPARTICLE_COMMON,

      /// \brief the KinSelect definition
      KINSELECT,

      /// \brief the IsData definition
      ISDATA,

      /// \brief the EventSelectBase definition
      EVENTSELECTBASE,

      /// \brief the pileup definition
      PILEUP
    };

    /// \brief the \ref Mode for this object
  private:
    Mode m_mode = Mode::OBJECT_DEFINITION;

    /// \brief the list of working point names for \ref
    /// Mode::IPARTICLE_COMMON
  private:
    std::vector<std::string> m_wpNameList;

    /// \brief the kinematic selection string for \ref Mode::KINSELECT
  private:
    std::string m_kinSelect;

    /// \brief the data file for \ref Mode::PILEUP
  private:
    std::vector<std::string> m_muDataFiles;

    /// \brief the mc file for \ref Mode::PILEUP
  private:
    std::vector<std::string> m_muMcFiles;

    /// \brief make the IParticle-common tools
  private:
    StatusCode makeIParticleCommonTools (DefinitionArgs& args);

    /// \brief make the kinematic selection tool
  private:
    StatusCode makeKinSelectTool (DefinitionArgs& args);

    /// \brief make the IsData tool
  private:
    StatusCode makeIsDataTool (DefinitionArgs& args);

    /// \brief make the IsData tool
  private:
    StatusCode makeEventSelectBaseTool (DefinitionArgs& args);

    /// \brief make the Pileup tool
  private:
    StatusCode makePileupTool (DefinitionArgs& args);
  };
}

#endif
