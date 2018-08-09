/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JetCMXData.h  -  description
 //                              -------------------
 //     begin                : 04 04 2014
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
  #ifndef JETCMXDATA_H
  #define JETCMXDATA_H
  
  #include "CLIDSvc/CLASS_DEF.h"
  #include "TrigT1CaloEvent/JEMTobRoI.h"

  namespace LVL1 {
  
/** The JetCMXData object contains the data transferred from the CPM to one of
    the Jet CMX in the crate.
    This is a transient class, describing backplane data */

    class JetCMXData  {
    public:

    /** Constructors */
    JetCMXData();
    JetCMXData(int crate, int module, 
               const std::vector<unsigned int>& tobWords);
    JetCMXData(int crate, int module, 
               const DataVector<JEMTobRoI>* tobs);

    /** Destructor */
    virtual ~JetCMXData();

    /** Data accessors */
    int crate() const;
    int module() const;
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
    std::vector <unsigned int> m_DataWords;
    
    };
  } // end of namespace

#ifndef JetCMXData_ClassDEF_H
#include "TrigT1CaloEvent/JetCMXData_ClassDEF.h"
#endif

#endif
