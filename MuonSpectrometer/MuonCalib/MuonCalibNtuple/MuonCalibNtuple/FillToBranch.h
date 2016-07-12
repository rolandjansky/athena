/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// FillToBranch.h
//   Header file for class FillToBranch
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// nveldik@nikhef.nl
///////////////////////////////////////////////////////////////////

#ifndef MUONCALIB_FILLTOBRANCH_H
#define MUONCALIB_FILLTOBRANCH_H

namespace MuonCalib {
/** @class FillToBranch
    Function object to be used in combination with std::for_each.  The
    constructor gets a pointer to the branch to be filled and in
    addition to that the information linking it to the data.

    Usage:

    std::for_each( data_iterator_begin,data_iterator_end,
                   FillToBranch( branch_pointer, info ) );

   @author Niels.Van.Eldik@cern.ch
*/

  template< class Branch, class Info, class Data >
    class FillToBranch { 
  public: 
//!< constructor get pointer to the branch to be filled and the information from the events 
    FillToBranch( Branch *branch, const Info &info ) : m_branch(branch), m_eventInfo(info) { }
  
    void operator() ( const Data *data ) {
      if( data && m_branch ) m_branch->fillBranch( *data, m_eventInfo );
      // if( data && m_branch ) m_branch->fillBranch( *data, m_index );
    }   //!< operator called by for_each, fills the event + EventInfo into the branch 

  private:
    Branch *m_branch;     //!< pointer to the branch
//    int     m_index;      //!< integer used to retain substructure of the CalibrationNtuple. For example, each MuonCalibSegment keeps track to which MuonCalibPattern it belongs, each XxxCalibHit keeps track to which MuonCalibSegment it belongs 
    Info    m_eventInfo;  //!< copy of the eventinfo
  };

}  //namespace MuonCalib

#endif
