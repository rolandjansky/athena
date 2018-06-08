/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//                           CMMCPHits.h  -  description
//                              -------------------
//     begin                : 25 05 2006
//     email                : Alan.Watson@cern.ch
//  ***************************************************************************/
  
  
  #ifndef CMMCPHits_H
  #define CMMCPHits_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {

  /** Summary of CP (EM/tau) hits received by the merger modules.

      Used in unpacking and analysis of CP CMM readout data. */
        
    class CMMCPHits  {
    public:

    enum DataIDs { REMOTE_0 = 15, REMOTE_1, REMOTE_2, LOCAL, TOTAL, MAXID };

    /** Constructors */
    CMMCPHits();
    CMMCPHits(int crate, int dataID);
    CMMCPHits(int crate, int dataID, const std::vector<unsigned int>& hits0,
                                     const std::vector<unsigned int>& hits1,
	                             const std::vector<int>& error0,
				     const std::vector<int>& error1,
				     int peak);
  
    virtual ~CMMCPHits();

    /** For multi-slice readout, need to indicate position of triggered BC */
    void setPeak(int peak);
    /** Add data to existing object */
    void addHits(const std::vector<unsigned int>& hits0,
                 const std::vector<unsigned int>& hits1,
                 const std::vector<int>& error0,
		 const std::vector<int>& error1);

    /** Data accessors */
    int crate()  const;
    int dataID() const;
    /** For triggered time slice */
    unsigned int HitWord0() const;
    unsigned int HitWord1() const;
    int Error0()  const;
    int Error1()  const;
    /** For multi-slice readout */
    const std::vector<unsigned int>& HitsVec0() const;
    const std::vector<unsigned int>& HitsVec1() const;
    const std::vector<int>& ErrorVec0() const;
    const std::vector<int>& ErrorVec1() const;
    int peak() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_dataID;
    int m_peak;
    std::vector<unsigned int> m_hits0;
    std::vector<unsigned int> m_hits1;
    std::vector<int> m_error0;
    std::vector<int> m_error1;
    
    };
  } // end of namespace

#ifndef CMMCPHits_ClassDEF_H
#include "TrigT1CaloEvent/CMMCPHits_ClassDEF.h"
#endif

#endif
