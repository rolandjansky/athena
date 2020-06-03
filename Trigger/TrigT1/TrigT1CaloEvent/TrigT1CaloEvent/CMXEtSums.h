/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
 
//*                                                                         *
//*                                                                         *
//***************************************************************************/
  
  #ifndef CMXEtSums_H
  #define CMXEtSums_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {

  /** CMXEtSums object stores Et sums from the Energy CMXs.

      Used in unpacking and analysis of CMX readout data. */
      
    class CMXEtSums  {
    public:

    enum Sources { REMOTE_STANDARD = 16,    REMOTE_RESTRICTED,
                   LOCAL_STANDARD,          LOCAL_RESTRICTED,
		   TOTAL_STANDARD,          TOTAL_RESTRICTED,
		   SUM_ET_STANDARD,         SUM_ET_RESTRICTED,
		   MISSING_ET_STANDARD,     MISSING_ET_RESTRICTED,
                   MISSING_ET_SIG_STANDARD, MAX_SOURCE };

    /** Constructors */
    CMXEtSums();
    CMXEtSums(int crate, int source);
    CMXEtSums(int crate, int source,
              const std::vector<unsigned int>& Et, 
	      const std::vector<unsigned int>& Ex,
	      const std::vector<unsigned int>& Ey,
	      const std::vector<int>& EtError,
	      const std::vector<int>& ExError,
	      const std::vector<int>& EyError,
	      int peak);
  
    virtual ~CMXEtSums();

    /** In multi-slice readout must specify which slice is triggered BC */
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
    int source() const;
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
    int m_source;
    int m_peak;
    std::vector <unsigned int> m_Et;
    std::vector <unsigned int> m_Ex;
    std::vector <unsigned int> m_Ey;
    std::vector <int> m_EtError;
    std::vector <int> m_ExError;
    std::vector <int> m_EyError;
    
    };

  /** returns crate number */
  inline int CMXEtSums::crate() const{
	return m_crate;
  }

  /** returns data source */
  inline int CMXEtSums::source() const{
        return m_source;
  }

  /** returns module ET sum for peak sample */
  inline unsigned int CMXEtSums::Et() const{
	return m_Et[m_peak];
  }

  /** returns module Ex sum for peak sample */
  inline unsigned int CMXEtSums::Ex() const{
	return m_Ex[m_peak];
  }

  /** returns module Ey sum for peak sample */
  inline unsigned int CMXEtSums::Ey() const{
	return m_Ey[m_peak];
  }

  /** returns module ET sum error for peak sample */
  inline int CMXEtSums::EtError() const{
	return m_EtError[m_peak];
  }

  /** returns module Ex sum error for peak sample */
  inline int CMXEtSums::ExError() const{
	return m_ExError[m_peak];
  }

  /** returns module Ey sum error for peak sample */
  inline int CMXEtSums::EyError() const{
	return m_EyError[m_peak];
  }

  /** returns module ET sum */
  inline const std::vector<unsigned int>& CMXEtSums::EtVec() const{
	return m_Et;
  }

  /** returns module Ex sum */
  inline const std::vector<unsigned int>& CMXEtSums::ExVec() const{
	return m_Ex;
  }

  /** returns module Ey sum */
  inline const std::vector<unsigned int>& CMXEtSums::EyVec() const{
	return m_Ey;
  }

  /** returns module ET errors */
  inline const std::vector<int>& CMXEtSums::EtErrorVec() const{
	return m_EtError;
  }

  /** returns module Ex errors */
  inline const std::vector<int>& CMXEtSums::ExErrorVec() const{
	return m_ExError;
  }

  /** returns module Ey errors */
  inline const std::vector<int>& CMXEtSums::EyErrorVec() const{
	return m_EyError;
  }

  /** Update ET sum */
  inline void CMXEtSums::addEt(const std::vector<unsigned int>& Et,
                               const std::vector<int>& EtError) {
        m_Et = Et;
        m_EtError = EtError;
  }

  /** Update Ex sum */
  inline void CMXEtSums::addEx(const std::vector<unsigned int>& Ex,
                               const std::vector<int>& ExError) {
        m_Ex = Ex;
        m_ExError = ExError;
  }

  /** Update Ey sum */
  inline void CMXEtSums::addEy(const std::vector<unsigned int>& Ey,
                               const std::vector<int>& EyError) {
        m_Ey = Ey;
        m_EyError = EyError;
  }

  /** Specify peak slice (Same for all vectors) */
  inline void CMXEtSums::setPeak(int peak) {
        m_peak = peak;
  }
                                                                                
  /** returns peak slice number */
  inline int CMXEtSums::peak() const{
        return m_peak;
  }

  } // end of namespace

#ifndef CMXEtSums_ClassDEF_H
#include "TrigT1CaloEvent/CMXEtSums_ClassDEF.h"
#endif

#endif


