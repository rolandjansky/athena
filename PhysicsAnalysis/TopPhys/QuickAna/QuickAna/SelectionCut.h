/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack



#ifndef QUICK_ANA__SELECTION_CUT_H
#define QUICK_ANA__SELECTION_CUT_H

#include <QuickAna/Global.h>

#include <cstddef>

namespace ana
{
  /// \brief a class for setting a single selection cut
  ///
  /// this class is used inside selection (or correction) tools, to
  /// represent the different cuts that the tool performs (with one
  /// object per cut).  to that end the owning tool should call
  /// registerCut() inside initialize and then call setPass/setFail in
  /// execute.
  ///
  /// to make things less ambigious (i.e. if we a selection cut is
  /// true does it mean it passes or it fails?), the method always
  /// include pass or fail in the name.

  class SelectionCut
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
    SelectionCut ();


    /// \brief get the selection choice for the cut
    /// \par Guarantee
    ///   no-fail
    /// \pre cut has been configured
  public:
    SelectionChoice get () const;


    /// \brief get whether we passed
    /// \par Guarantee
    ///   no-fail
  public:
    bool getPass () const;


    /// \brief get whether we failed
    /// \par Guarantee
    ///   no-fail
  public:
    bool getFail () const;


    /// \brief set the selection choice for the cut
    /// \par Guarantee
    ///   no-fail
    /// \pre cut has been configured
  public:
    void set (SelectionChoice choice) const;


    /// \brief set whether the cut passed
    /// \par Guarantee
    ///   no-fail
    /// \pre cut has been configured
  public:
    void setPassedIf (bool pass) const;


    /// \brief set whether the cut failed
    /// \par Guarantee
    ///   no-fail
    /// \pre cut has been configured
  public:
    void setFailedIf (bool fail) const;


    /// \brief configure this cut to point to the given object
    /// \par Guarantee
    ///   no-fail
  public:
    void configure (SelectionData *val_data, std::size_t val_index);



    //
    // private interface
    //

    /// \brief the selection data object
  private:
    SelectionData *m_data;

    /// \brief the index of this cut
  private:
    std::size_t m_index;
  };
}

#endif
