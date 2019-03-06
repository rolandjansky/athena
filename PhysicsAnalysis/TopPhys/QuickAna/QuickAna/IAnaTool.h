/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__I_ANA_TOOL_H
#define QUICK_ANA__I_ANA_TOOL_H


#include <QuickAna/Global.h>

#include <PATInterfaces/ISystematicsTool.h>

namespace ana
{
  /// \brief The interface class for all the tools constituting object
  /// definitions.
  class IAnaTool : public CP::ISystematicsTool
  {
    //
    // public interface
    //

    ASG_TOOL_INTERFACE(IAnaTool)


    /// \brief fill the initial configuration before initialize
    ///
    /// this is mostly used so that retrieval tools can broadcast the
    /// input collections they are using
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool dependent
  public:
    virtual StatusCode
    getInitialConfiguration (InternalConfiguration& conf) = 0;


    /// \brief use the initial configuration before initialize
    ///
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool dependent
  public:
    virtual StatusCode
    useInitialConfiguration (const InternalConfiguration& conf) = 0;


    /// \brief set the object type to work on
    ///
    /// this is called straight after the tool is created and before
    /// it is initialized.  the primary reason for this function is to
    /// allow jet definitions to be used for either jets or fat jets.
    /// \param type the object type we work on
    /// \param workingPoint the name of the working point we are
    ///   working on or the empty string if this is a sole working
    ///   point or working points do not apply
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   unsupported object type
    /// \warn this interface may change when I introduce multiple
    ///   simultaneous working points
  public:
    virtual StatusCode
    setObjectType (ObjectType type, const std::string& workingPoint) = 0;


    /// \brief the processing step we are working on
    /// \par Guarantee
    ///   no-fail
  public:
    virtual AnalysisStep step () const = 0;


    /// \brief a bit-map indicating which object types we use
    /// \par Guarantee
    ///   no-fail
  public:
    virtual unsigned inputTypes () const = 0;


    /// \brief a bit-map indicating which output types we will create
    /// \par Guarantee
    ///   no-fail
  public:
    virtual unsigned outputTypes () const = 0;


    /// \brief fill the source component of the \ref EventData
    ///
    /// This is meant to be implemented by the retrieval tools that
    /// perform the shallow copy to document what they do.  All other
    /// tools can safely ignore this.
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    virtual void
    fillEventDataSource (EventData& event) const = 0;


    /// \brief extract whatever we need from the given configuration object
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool failures
  public:
    virtual StatusCode
    useConfiguration (const InternalConfiguration& configuration) = 0;


    /// \brief apply this analysis tool to the objects
    /// \param objects the objects to apply this too
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool dependent
  public:
    virtual StatusCode execute (IEventObjects& objects) = 0;
  };
}

#endif
