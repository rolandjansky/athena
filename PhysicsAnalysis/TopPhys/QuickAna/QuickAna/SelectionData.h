/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__SELECTION_DATA_H
#define QUICK_ANA__SELECTION_DATA_H

#include <QuickAna/Global.h>

#include <QuickAna/SelectionStep.h>
#include <QuickAna/xAODInclude.h>

namespace ana
{
  /// \brief this class manages all the selection data for a single
  /// correction or selection tool.

  class SelectionData
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    SelectionData ();


    /// \brief add a cut with the given name and selection step
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
    /// \pre !isInitialized()
  public:
    void registerCut (SelectionStep step, const std::string& name,
		      SelectionCut& cut);


    /// \brief the number of cuts configured
    /// \par Guarantee
    ///   no-fail
  public:
    std::size_t size () const;


    /// \brief initialize this object
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   out of memory II
    /// \pre !isInitialized()
    /// \post isInitialized()
  public:
    StatusCode initialize (const InternalConfiguration& configuration,
			   const std::string& workingPoint);


    /// \brief whether initialize() has been called successfully
    /// \par Guarantee
    ///   no-fail
  public:
    bool isInitialized () const;


    /// \brief prepare for the given particle
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   xAOD errors
    /// \pre isInitialized()
  public:
    StatusCode prepare (SG::AuxElement *particle);


    /// \brief whether we can get information by running the
    /// associated selection tool
    ///
    /// this is mostly meant to optimize away selection tools on
    /// objects that have already been rejected
    /// \par Guarantee
    ///   no-fail
    /// \pre isInitialized()
  public:
    bool needSelectionTool ();


    /// \brief get the given variable to the given choice
    /// \par Guarantee
    ///   no-fail
    /// \pre isInitialized()
    /// \pre cut < size()
  public:
    SelectionChoice getCut (std::size_t cut);


    /// \brief set the given variable to the given choice
    /// \par Guarantee
    ///   no-fail
    /// \pre isInitialized()
    /// \pre cut < size()
  public:
    void setCut (std::size_t cut, SelectionChoice choice);


    /// \brief set the decorations on the associated particle
    /// \par Guarantee
    ///   basic
    /// \par Failures
    ///   xAOD errors
    /// \pre isInitialized()
  public:
    void apply ();



    //
    // private interface
    //

    /// \brief the accessor type we use
  private:
    typedef SG::AuxElement::Accessor<SelectType> AccessorType;

    /// \brief the state of this tool
  private:
    enum class State
    {
      /// \brief something happened that BROKE this tool
      BROKEN,

      /// \brief we are initializing the tool
      INITIAL,

      /// \brief we are fully initialized
      RUNNING
    };

    /// \brief the state of this tool
  private:
    State m_state;


    /// \brief the information for each cut
  private:
    struct CutInfo
    {
      /// \brief the selection step this goes into
    public:
      SelectionStep step;

      /// \brief the name of the cut
    public:
      std::string name;

      /// \brief the accessor for this cut
    public:
      AccessorType accessor;

      /// \brief whether we have been selected
    public:
      SelectionChoice selection;

      CutInfo ();
    };

    /// \brief the list of cuts
  private:
    std::vector<CutInfo> m_cuts;


    /// \brief the information for the different selection steps
    struct SelectionInfo
    {
      /// \brief the accessor for this cut
    public:
      AccessorType accessor;

      /// \brief whether we have been selected
    public:
      bool select;

      SelectionInfo ();
    };

    /// \brief the list of selection steps
  private:
    SelectionInfo m_selections [EnumSize<SelectionStep>::dataSize];


    /// \brief the first step we apply this selection to
  private:
    SelectionStep m_firstStep;


    /// \brief the particle we are operating on
  private:
    SG::AuxElement *m_particle;
  };
}

#endif
