/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//                           CMMEtSums.h  -  description
//                              -------------------
//     begin                : 25 05 2006
//     email                : Alan.Watson@cern.ch
//***************************************************************************/
  
//*                                                                         *
//*                                                                         *
//***************************************************************************/
  
  #ifndef CMMEtSums_H
  #define CMMEtSums_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {

  /** CMMEtSums object stores Et sums from the Energy CMMs.

      Used in unpacking and analysis of CMM readout data. */
      
    class CMMEtSums  {
    public:

    enum DataIDs { REMOTE = 16, LOCAL, TOTAL,
                   MISSING_ET_MAP, SUM_ET_MAP, MISSING_ET_SIG_MAP, MAXID };

    /** Constructors */
    CMMEtSums();
    CMMEtSums(int crate, int dataID);
    CMMEtSums(int crate, int dataID,
              const std::vector<unsigned int>& Et, 
	      const std::vector<unsigned int>& Ex,
	      const std::vector<unsigned int>& Ey,
	      const std::vector<int>& EtError,
	      const std::vector<int>& ExError,
	      const std::vector<int>& EyError,
	      int peak);
  
    virtual ~CMMEtSums();

    /** In multi-slice readout must specify which slide is triggered BC */
    void setPeak(int peak);
    /** Add data to existing objects */
    void addEt(const std::vector<unsigned int>& Et,
               const std::vector<int>& EtError);
    void addEx(const std::vector<unsigned int>& Ex,
               const std::vector<int>& ExError);
    void addEy(const std::vector<unsigned int>& Ey,
               const std::vector<int>& EyError);

    /** Data accessors */
    int crate() const;
    int dataID() const;
    /** For triggered time slice */
    unsigned int Et() const;
    unsigned int Ex() const;
    unsigned int Ey() const;
    int EtError() const;
    int ExError() const;
    int EyError() const;
    /** Access multi-slice data */
    const std::vector<unsigned int>& EtVec() const;
    const std::vector<unsigned int>& ExVec() const;
    const std::vector<unsigned int>& EyVec() const;
    const std::vector<int>& EtErrorVec() const;
    const std::vector<int>& ExErrorVec() const;
    const std::vector<int>& EyErrorVec() const;
    int peak() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_dataID;
    int m_peak;
    std::vector <unsigned int> m_Et;
    std::vector <unsigned int> m_Ex;
    std::vector <unsigned int> m_Ey;
    std::vector <int> m_EtError;
    std::vector <int> m_ExError;
    std::vector <int> m_EyError;
    
    };
  } // end of namespace

#ifndef CMMEtSums_ClassDEF_H
#include "TrigT1CaloEvent/CMMEtSums_ClassDEF.h"
#endif

#endif


