/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CPMCMXData.h  -  description
 //                              -------------------
 //     begin                : 04 04 2014
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
  #ifndef CPMCMXDATA_H
  #define CPMCMXDATA_H
  
  #include "CLIDSvc/CLASS_DEF.h"
  #include "TrigT1CaloEvent/CPMTobRoI.h"

  namespace LVL1 {
  
/** The CPMCMXData object contains the data transferred from the CPM to one of
    the CMXes (EM or Tau) in the crate.
    This is a transient class, describing backplane data */

    class CPMCMXData  {
    public:

    /** Constructors */
    CPMCMXData();
    CPMCMXData(int crate, int module, int type, 
               const std::vector<unsigned int>& tobWords);
    CPMCMXData(int crate, int module, int type, 
               const DataVector<CPMTobRoI>* tobs);

    /** Destructor */
    virtual ~CPMCMXData();

    /** Data accessors */
    int crate() const;
    int module() const;
    int type() const;
    unsigned int presenceMap() const;
    bool overflow() const;
    std::vector<unsigned int> DataWords() const;
    std::vector<unsigned int> TOBPresenceBits() const;
    std::vector<unsigned int> TOBWords() const;
    std::vector<unsigned int> TopoTOBs() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_module;
    int m_type;
    std::vector <unsigned int> m_DataWords;
    
    };
  } // end of namespace

#ifndef CPMCMXData_ClassDEF_H
#include "TrigT1CaloEvent/CPMCMXData_ClassDEF.h"
#endif

#endif
