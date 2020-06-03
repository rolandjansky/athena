/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
  
  #ifndef CMXCPTob_H
  #define CMXCPTob_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {

  /** TOB data received by the merger modules.

      Used in unpacking and analysis of CP CMX readout data. */
        
    class CMXCPTob  {
    public:

    /** Constructors */
    CMXCPTob();
    CMXCPTob(int crate, int cmx, int cpm, int chip, int loc);
    CMXCPTob(int crate, int cmx, int cpm, int chip, int loc,
             const std::vector<int>& energy,
             const std::vector<int>& isolation,
	     const std::vector<int>& error,
	     const std::vector<unsigned int>& presenceMap,
	     int peak);
  
    virtual ~CMXCPTob();

    /** For multi-slice readout, need to indicate position of triggered BC */
    void setPeak(int peak);
    /** Add data to existing object */
    void addTob(const std::vector<int>& energy,
                const std::vector<int>& isolation,
	        const std::vector<int>& error,
	        const std::vector<unsigned int>& presenceMap);

    /** Data accessors */
    int crate()     const;
    int cmx()       const;
    int cpm()       const;
    int chip()      const;
    int location()  const;
    /** For triggered time slice */
    int energy()    const;
    int isolation() const;
    int error()     const;
    unsigned int presenceMap() const; // filled for neutral format data only
    /** For multi-slice readout */
    const std::vector<int>& energyVec()    const;
    const std::vector<int>& isolationVec() const;
    const std::vector<int>& errorVec()     const;
    const std::vector<unsigned int>& presenceMapVec() const;
    int peak() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_cmx;
    int m_cpm;
    int m_chip;
    int m_location;
    int m_peak;
    std::vector<int> m_energy;
    std::vector<int> m_isolation;
    std::vector<int> m_error;
    std::vector<unsigned int> m_presenceMap;
    
    };

  /** returns crate number */
  inline int CMXCPTob::crate() const{
          return m_crate;
  }

  /** returns CMX number (0/1)==(Left/Right)==(Tau/Em) */      // <<== CHECK
  inline int CMXCPTob::cmx() const{
          return m_cmx;
  }

  /** returns CPM number */
  inline int CMXCPTob::cpm() const{
          return m_cpm;
  }

  /** returns chip number (3 or 4 bits?) */
  inline int CMXCPTob::chip() const{
          return m_chip;
  }

  /** returns location (3 or 2 bits?) */
  inline int CMXCPTob::location() const{
          return m_location;
  }

  /** returns energy for peak sample */
  inline int CMXCPTob::energy() const{
          return m_energy[m_peak];
  }

  /** returns isolation for peak sample */
  inline int CMXCPTob::isolation() const{
          return m_isolation[m_peak];
  }

  /** returns error for peak sample */
  inline int CMXCPTob::error() const{
          return m_error[m_peak];
  }

  /** returns presence map for peak sample */
  inline unsigned int CMXCPTob::presenceMap() const{
          return m_presenceMap[m_peak];
  }

  /** returns energy */
  inline const std::vector<int>& CMXCPTob::energyVec() const{
          return m_energy;
  }

  /** returns isolation */
  inline const std::vector<int>& CMXCPTob::isolationVec() const{
          return m_isolation;
  }

  /** returns error */
  inline const std::vector<int>& CMXCPTob::errorVec() const{
          return m_error;
  }

  /** returns presence maps */
  inline const std::vector<unsigned int>& CMXCPTob::presenceMapVec() const{
          return m_presenceMap;
  }

  /** Set TOB */
  inline void CMXCPTob::addTob(const std::vector<int>& energy,
                               const std::vector<int>& isolation,
                               const std::vector<int>& error,
			       const std::vector<unsigned int>& presenceMap) {
    m_energy = energy;
    m_isolation = isolation;
    m_error = error;
    m_presenceMap = presenceMap;
  }

  /** Specify peak slice  */
  inline void CMXCPTob::setPeak(int peak) {
    m_peak = peak;
  }

  /** returns peak slice number */
  inline int CMXCPTob::peak() const{
          return m_peak;
  }

} // end of namespace

#ifndef CMXCPTob_ClassDEF_H
#include "TrigT1CaloEvent/CMXCPTob_ClassDEF.h"
#endif

#endif
