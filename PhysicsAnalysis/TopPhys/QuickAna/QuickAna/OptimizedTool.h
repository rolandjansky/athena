/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__TOOL_WRAPPER_H
#define QUICK_ANA__TOOL_WRAPPER_H


#include <QuickAna/Global.h>

#include <AsgTools/AsgMessaging.h>
#include <PATInterfaces/SystematicSet.h>
#include <memory>
#include <set>
#include <unordered_map>

namespace CP
{
  class SystematicSet;
}

namespace ana
{

  /// Holds tool and meta-information for the optimized scheduler.
  ///
  class OptimizedTool : public asg::AsgMessaging
  {
    //
    // public interface
    //

    // no object slicing
    OptimizedTool (const OptimizedTool&) = delete;
    OptimizedTool& operator = (const OptimizedTool&) = delete;


    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief initializing constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    OptimizedTool (std::unique_ptr<IAnaTool> val_tool);


    /// \brief standard destructor
    ///
    /// explicit destructor to reduce number of header file includes
    /// \par Guarantee
    ///   no-fail
  public:
    ~OptimizedTool ();


    /// \brief the name of this tool
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    std::string name () const;


    /// \brief the step of this tool
    /// \par Guarantee
    ///   no-fail
  public:
    AnalysisStep step () const;


    /// \brief the list of all systematics this tool can be affected by
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    CP::SystematicSet affectingSystematics () const;


    /// \brief the list of all systematics this tool recommends to use
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    CP::SystematicSet recommendedSystematics () const;


    /// \brief apply this analysis tool to the objects
    /// \param objects the objects to apply this too
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   tool dependent
  public:
    StatusCode execute ();


    /// \brief call IAnaTool::fillEventDataSource
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    void fillEventDataSource (EventData& event) const;


    /// \brief whether we use the tool directly, i.e. whether it has
    ///   an output that is one of our inputs and is executed on an
    ///   earlier step
    /// \par Guarantee
    ///   no-fail
  public:
    bool hasDirectInput (const OptimizedTool& input) const noexcept;


    /// \brief whether the given tool has been registered via \ref
    ///   addOutput as using the output of his tool directly or
    ///   indirectly
    /// \par Guarantee
    ///   no-fail
  public:
    bool hasOutput (const OptimizedTool *that) const noexcept;


    /// \brief register the given tool as using the output of this
    ///   tool as an input
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    void addOutput (OptimizedTool *output);


    /// \brief configure this wrapper for the given target set of
    /// systematics
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
  public:
    StatusCode configureSystematics (OptimizedStore *store);



    //
    // private interface
    //

    /// \brief the tool wrapped
  private:
    std::unique_ptr<IAnaTool> m_tool;

    /// \brief the list of tools we are using
  private:
    std::set<OptimizedTool*> m_inputs;

    /// \brief the list of tools that are using us
  private:
    std::set<OptimizedTool*> m_outputs;

    /// \brief the list of the OptimizedObjects we use
  private:
    std::unordered_map<CP::SystematicSet,std::unique_ptr<OptimizedObjects>> m_objects;
  };
}

#endif
