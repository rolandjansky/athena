/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
  
  #ifndef CMXJetHits_H
  #define CMXJetHits_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {

  /** Summary of JEP (Jet) hits produced by the merger modules.

      Used in unpacking and analysis of JEP CMX readout data. */
        
    class CMXJetHits  {
    public:

    enum Sources {
	REMOTE_MAIN = 0,
	LOCAL_MAIN = 1,
	TOTAL_MAIN = 2,
        REMOTE_FORWARD = 4,
	LOCAL_FORWARD = 5,
	TOTAL_FORWARD = 6,
        TOPO_CHECKSUM = 8,
	TOPO_OCCUPANCY_MAP = 9,
	TOPO_OCCUPANCY_COUNTS = 10,
	MAX_SOURCE = 15
    };

    /** Constructors */
    CMXJetHits();
    CMXJetHits(int crate, int source);
    CMXJetHits(int crate, int source,
               const std::vector<unsigned int>& hits0,
               const std::vector<unsigned int>& hits1,
	       const std::vector<int>& error0,
	       const std::vector<int>& error1,
	       int peak);
  
    virtual ~CMXJetHits();

    /** For multi-slice readout, need to indicate position of triggered BC */
    void setPeak(int peak);
    /** Add data to existing object */
    void addHits(const std::vector<unsigned int>& hits0,
                 const std::vector<unsigned int>& hits1,
                 const std::vector<int>& error0,
		 const std::vector<int>& error1);

    /** Data accessors */
    int crate()  const;
    int source() const;
    /** For triggered time slice */
    unsigned int hits0() const;
    unsigned int hits1() const;
    int error0() const;
    int error1() const;
    /** For multi-slice readout */
    const std::vector<unsigned int>& hitsVec0() const;
    const std::vector<unsigned int>& hitsVec1() const;
    const std::vector<int>& errorVec0() const;
    const std::vector<int>& errorVec1() const;
    int peak() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_source;
    int m_peak;
    std::vector<unsigned int> m_hits0;
    std::vector<unsigned int> m_hits1;
    std::vector<int> m_error0;
    std::vector<int> m_error1;
    
    };

  /** returns crate number */
  inline int CMXJetHits::crate() const{
          return m_crate;
  }

  /** returns source of data */
  inline int CMXJetHits::source() const{
          return m_source;
  }

  /** returns hits0 for peak sample */
  inline unsigned int CMXJetHits::hits0() const{
    return m_peak < (int)m_hits0.size()? m_hits0[m_peak]: 0;
  }

  /** returns hits1 for peak sample */
  inline unsigned int CMXJetHits::hits1() const{
    return m_peak < (int)m_hits1.size()? m_hits1[m_peak]: 0;
  }

  /** returns error0 for peak sample */
  inline int CMXJetHits::error0() const{
    return m_peak < (int)m_error0.size()? m_error0[m_peak]: 0;
  }

  /** returns error1 for peak sample */
  inline int CMXJetHits::error1() const{
    return m_peak < (int)m_error1.size()? m_error1[m_peak]: 0;
  }

  /** returns hits0 */
  inline const std::vector<unsigned int>& CMXJetHits::hitsVec0() const{
          return m_hits0;
  }

  /** returns hits1 */
  inline const std::vector<unsigned int>& CMXJetHits::hitsVec1() const{
          return m_hits1;
  }

  /** returns error0 */
  inline const std::vector<int>& CMXJetHits::errorVec0() const{
          return m_error0;
  }

  /** returns error1 */
  inline const std::vector<int>& CMXJetHits::errorVec1() const{
          return m_error1;
  }

  /** Set hits */
  inline void CMXJetHits::addHits(const std::vector<unsigned int>& hits0,
                                 const std::vector<unsigned int>& hits1,
                                 const std::vector<int>& error0,
                                 const std::vector<int>& error1) {
    m_hits0 = hits0;
    m_hits1 = hits1;
    m_error0 = error0;
    m_error1 = error1;
  }

  /** Specify peak slice  */
  inline void CMXJetHits::setPeak(int peak) {
    m_peak = peak;
  }

  /** returns peak slice number */
  inline int CMXJetHits::peak() const{
    return m_peak;
  }

} // end of namespace

#ifndef CMXJetHits_ClassDEF_H
#include "TrigT1CaloEvent/CMXJetHits_ClassDEF.h"
#endif

#endif
