/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CPMHits.h  -  description
 //                              -------------------
 //     begin                : 25 05 2006
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
  #ifndef CPMHits_H
  #define CPMHits_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {
  
/** The CPMHits object contains the hit data produced by a given Cluster
    Processor Module, i.e. the hit multiplicity sums for all 16 EM+Tau
    thresholds for one module. The sums are packed into 2 words,
    representing the data sent to the 2 CMMs.

    The main function of the CPMHits object is for the unpacking and
    analysis of the hit data from the CPM readout. */

    class CPMHits  {
    public:

    /** Constructors */
    CPMHits();
    CPMHits(int crate, int module);
    CPMHits(int crate, int module, const std::vector<unsigned int>&  EMTauHits0, 
            const std::vector<unsigned int>&  EMTauHits1, int peak);

    /** Destructor */
    virtual ~CPMHits();

    /** In multi-slice readout, need to specify which is peak slice */
    void setPeak(int peak);
    /** Add hits to existing object */
    void addHits(const std::vector<unsigned int>&  hits0, 
                 const std::vector<unsigned int>&  hits1);

    /** Data accessors */
    int crate() const;
    int module() const;
    unsigned int HitWord0() const;
    unsigned int HitWord1() const;

    /** Multi-slice accessors */
    const std::vector<unsigned int>& HitsVec0() const;
    const std::vector<unsigned int>& HitsVec1() const;
    int peak() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_module;
    int m_peak;
    std::vector <unsigned int> m_Hits0;
    std::vector <unsigned int> m_Hits1;
    
    };
  } // end of namespace

#ifndef CPMHits_ClassDEF_H
#include "TrigT1CaloEvent/CPMHits_ClassDEF.h"
#endif

#endif
