// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
// Header file for class SiDetElementRoadMakerData_xk
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiDetElementRoadMakerData_xk_H
#define SiDetElementRoadMakerData_xk_H


#include <vector>
#include <array>

namespace InDet {

 /**
    @class SiDetElementRoadMakerData_xk
  
    InDet::SiDetElementRoadMakerData_xk holds event dependent data
    used by SiDetElementRoadMaker_xk. 
    @author goblirsc@cern.ch
  */

  class SiDetElementRoadMakerData_xk {

  public:
    /// This helper class is used to keep track of 
    /// which detector elements are already being 
    /// considered when forming a search road during
    /// the extension of silicon seeds through the full
    /// Si detectors. 
    class UsedFlag {
    public:
      UsedFlag() : m_used(false) {}
      /// read the flag 
      bool used() const { return m_used; }
      /// toggle the flag
      void setUsed() { m_used=true; }
      /// reset without having to overwrite the object
      void reset(){ m_used=false;} 
    private:
      bool m_used;
    };

    /// trivial constructor - the members of this event 
    /// data struct need client tool information for 
    /// initialisation, hence this is done at client-level. 
    SiDetElementRoadMakerData_xk(){}

    /// Default destructor
    ~SiDetElementRoadMakerData_xk() = default;
    
    /// method to reset the flags stored in the elementUsageTracker
    /// (below) when building a new search road. Does not perform any
    /// allocations, but resets the existing slots. 
    void resetUsageTracker();

    /// Following the pattern established for example in the 
    /// seed-maker event data, we use public members directly
    /// rather than exposing private members with full read-write-access. 
    /// This has the advantage that the members can be intialized by the 
    /// client tools, which provide the tools needed to do so. 

    /// This is a data structure used to track which of our detector elements 
    /// are already on a search road. Nested in the hierarchy of 
    /// detector region - layer - module within layer. 
    /// Dynamic to avoid hard-coding a certain geometry. 
    typedef std::array<std::vector<std::vector<UsedFlag> >,3> ElementUsageTracker;
    ElementUsageTracker elementUsageTracker;

    /// Flag to check if the event data was already initialized by the client tool. 
    bool isInitialized{false}; 
  };

} // end of name space

#endif // SiDetElementRoadMakerData_xk_H
