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


    class UsedFlag {
    public:
      UsedFlag() : m_used(false) {}

      bool used() const { return m_used; }

      void setUsed() { m_used=true; }
      void reset(){ m_used=false;} 
    private:
      bool m_used;
    };
     /**
     * Constructor
     */
    SiDetElementRoadMakerData_xk(){}

    /**
     * Default destructor
     */
    ~SiDetElementRoadMakerData_xk() = default;
    
    void resetUsageTracker();

    typedef std::array<std::vector<std::vector<UsedFlag> >,3> ElementUsageTracker;

    ElementUsageTracker elementUsageTracker;
    bool isInitialized{false}; 
  };

} // end of name space

#endif // SiDetElementRoadMakerData_xk_H
