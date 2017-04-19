/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUICK_ANA__ANA_TOOL_RETRIEVE_H
#define QUICK_ANA__ANA_TOOL_RETRIEVE_H

//        Copyright Iowa State University 2014.
//                  Author: Nils Krumnack
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Please feel free to contact me (nils.erik.krumnack@cern.ch) for bug
// reports, feature suggestions, praise and complaints.



#include <QuickAna/Global.h>

#include <QuickAna/AnaTool.h>

namespace ana
{
  /// \brief The tool class implementing basic object retrieval.

  class AnaToolRetrieve : virtual public IAnaTool, public AnaTool
  {
    //
    // public interface
    //

    ASG_TOOL_CLASS (AnaToolRetrieve, ana::IAnaTool)


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolRetrieve (const std::string& name);


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    AnaToolRetrieve (const std::string& name, const std::string& containerName);


  public:
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) override;

  public:
    virtual AnalysisStep step () const override;

  public:
    virtual unsigned inputTypes () const override;

  public:
    virtual unsigned outputTypes () const override;

  public:
    virtual void
    fillEventDataSource (EventData& event) const override;

  public:
    virtual StatusCode execute (IEventObjects& objects) override;

  public:
    virtual StatusCode
    getInitialConfiguration (InternalConfiguration& conf) override;

  public:
    std::string m_containerName;



    //
    // private interface
    //

    /// \brief the type of object we work on
  private:
    ObjectType m_type;
  };
}

#endif
