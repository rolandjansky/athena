/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           EnergyCMXData.h  -  description
 //                              -------------------
 //     begin                : 04 04 2014
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
  #ifndef ENERGYCMXDATA_H
  #define ENERGYCMXDATA_H
  
  #include "CLIDSvc/CLASS_DEF.h"

  namespace LVL1 {
  
/** The EnergyCMXData object contains the data transferred from the JEM to
    the EnergySum CMX in the crate.
    This is a transient class, describing backplane data */

    class EnergyCMXData  {
    public:

    /** Constructors */
    EnergyCMXData();
    EnergyCMXData(int crate, int module,
                  unsigned int ex, unsigned int ey, unsigned int et);

    /** Destructor */
    virtual ~EnergyCMXData();

    /** Data accessors */
    int crate() const;
    int module() const;
    unsigned int Ex() const;
    unsigned int Ey() const;
    unsigned int Et() const;
    std::vector<unsigned int> DataWords() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_module;
    unsigned int m_Ex;
    unsigned int m_Ey;
    unsigned int m_Et;
    std::vector<unsigned int> m_DataWords;
    
    };
  } // end of namespace

#ifndef EnergyCMXData_ClassDEF_H
#include "TrigT1CaloEvent/EnergyCMXData_ClassDEF.h"
#endif

#endif
