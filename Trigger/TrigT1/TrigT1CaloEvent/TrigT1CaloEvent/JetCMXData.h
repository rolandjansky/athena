
 //***************************************************************************
 //                           JetCMXData.h  -  description
 //                              -------------------
 //     begin                : 04 04 2014
 //     copyright            : (C) 2013 by Alan Watson
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
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
    std::vector<unsigned int> DataWords() const;
    std::vector<unsigned int> TOBLocalCoords() const;
    std::vector<unsigned int> TOBETLarge() const;
    std::vector<unsigned int> TOBETSmall() const;
      
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
