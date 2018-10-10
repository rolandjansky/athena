/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
  
  #ifndef CMXCPHits_H
  #define CMXCPHits_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {

  /** Summary of CP (EM/tau) hits produced by the merger modules.

      Used in unpacking and analysis of CP CMX readout data. */
        
    class CMXCPHits  {
    public:

    enum Sources { REMOTE_0, REMOTE_1, REMOTE_2, LOCAL, TOTAL,
                   TOPO_CHECKSUM, TOPO_OCCUPANCY_MAP, TOPO_OCCUPANCY_COUNTS,
		   MAXSOURCE };

    /** Constructors */
    CMXCPHits();
    CMXCPHits(int crate, int cmx, int source);
    CMXCPHits(int crate, int cmx, int source,
              const std::vector<unsigned int>& hits0,
              const std::vector<unsigned int>& hits1,
	      const std::vector<int>& error0,
	      const std::vector<int>& error1,
	      int peak);
  
    virtual ~CMXCPHits();

    /** For multi-slice readout, need to indicate position of triggered BC */
    void setPeak(int peak);
    /** Add data to existing object */
    void addHits(const std::vector<unsigned int>& hits0,
                 const std::vector<unsigned int>& hits1,
                 const std::vector<int>& error0,
		 const std::vector<int>& error1);

    /** Data accessors */
    int crate()  const;
    int cmx()    const;
    int source() const;
    /** For triggered time slice */
    unsigned int hits0()  const;
    unsigned int hits1()  const;
    int error0()  const;
    int error1()  const;
    /** For multi-slice readout */
    const std::vector<unsigned int>& hitsVec0()  const;
    const std::vector<unsigned int>& hitsVec1()  const;
    const std::vector<int>& errorVec0()  const;
    const std::vector<int>& errorVec1()  const;
    int peak() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_cmx;                                       // <<== Do we need firmware as well?
    int m_source;
    int m_peak;
    std::vector<unsigned int> m_hits0;
    std::vector<unsigned int> m_hits1;
    std::vector<int> m_error0;
    std::vector<int> m_error1;
    
    };

  /** returns crate number */
  inline int CMXCPHits::crate() const{
          return m_crate;
  }

  /** returns CMX number (0/1)==(Left/Right)==(Tau/Em?) */    // <<== CHECK Might have two firmware types
  inline int CMXCPHits::cmx() const{
          return m_cmx;
  }

  /** returns source of data */
  inline int CMXCPHits::source() const{
          return m_source;
  }

  /** returns hits0 for peak sample */
  inline unsigned int CMXCPHits::hits0() const{
          return m_hits0[m_peak];
  }

  /** returns hits1 for peak sample */
  inline unsigned int CMXCPHits::hits1() const{
          return m_hits1[m_peak];
  }

  /** returns error0 for peak sample */
  inline int CMXCPHits::error0() const{
          return m_error0[m_peak];
  }

  /** returns error1 for peak sample */
  inline int CMXCPHits::error1() const{
          return m_error1[m_peak];
  }

  /** returns hits0 */
  inline const std::vector<unsigned int>& CMXCPHits::hitsVec0() const{
          return m_hits0;
  }

  /** returns hits1 */
  inline const std::vector<unsigned int>& CMXCPHits::hitsVec1() const{
          return m_hits1;
  }

  /** returns error0 */
  inline const std::vector<int>& CMXCPHits::errorVec0() const{
          return m_error0;
  }

  /** returns error1 */
  inline const std::vector<int>& CMXCPHits::errorVec1() const{
          return m_error1;
  }

  /** Set hits */
  inline void CMXCPHits::addHits(const std::vector<unsigned int>& hits0,
                                 const std::vector<unsigned int>& hits1,
                                 const std::vector<int>& error0,
                                 const std::vector<int>& error1) {
    m_hits0 = hits0;
    m_hits1 = hits1;
    m_error0 = error0;
    m_error1 = error1;
  }

  /** Specify peak slice  */
  inline void CMXCPHits::setPeak(int peak) {
    m_peak = peak;
  }

  /** returns peak slice number */
  inline int CMXCPHits::peak() const{
          return m_peak;
  }

} // end of namespace

#ifndef CMXCPHits_ClassDEF_H
#include "TrigT1CaloEvent/CMXCPHits_ClassDEF.h"
#endif

#endif
