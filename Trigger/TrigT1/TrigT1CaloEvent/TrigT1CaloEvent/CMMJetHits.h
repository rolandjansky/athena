/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//                           CMMJetHits.h  -  description
//                              -------------------
//     begin                : 25 05 2006
//     email                : Alan.Watson@cern.ch
//  ***************************************************************************/
  
  
  #ifndef CMMJetHits_H
  #define CMMJetHits_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {

  /** CMMJetHits class stores Jet hit multiplicities received by and read
      out from the Jet CMMs.

      Its main function is in the unpacking and analysis of the Jet CMM
      readout. */
  
    class CMMJetHits  {
    public:
    
    enum DataIDs { REMOTE_MAIN = 16, LOCAL_MAIN, TOTAL_MAIN,
                   REMOTE_FORWARD, LOCAL_FORWARD, TOTAL_FORWARD,
		   ET_MAP, MAXID };

    /** Constructors */
    CMMJetHits();
    CMMJetHits(int crate, int dataID);
    CMMJetHits(int crate, int dataID,
               const std::vector<unsigned int>& hits,
	       const std::vector<int>& errors, int peak);

    /** Destructor */
    virtual ~CMMJetHits();

    /** In multi-slice readout, need to specify which slice corresponds to the
        triggered bunch-crossing */
    void setPeak(int peak);
    /** Add hits to existing object */
    void addHits(const std::vector<unsigned int>& hits,
                 const std::vector<int>& errors);

    /** Data accessors */
    int crate()  const;
    int dataID() const;
    unsigned int Hits() const;
    int Error()  const;
    /** Multi-slice accessors */
    const std::vector<unsigned int>& HitsVec() const;
    const std::vector<int>& ErrorVec() const;
    int peak() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_dataID;
    int m_peak;
    std::vector<unsigned int> m_hits;
    std::vector<int> m_errors;
    
    };
  } // end of namespace

#ifndef CMMJetHits_ClassDEF_H
#include "TrigT1CaloEvent/CMMJetHits_ClassDEF.h"
#endif

#endif
