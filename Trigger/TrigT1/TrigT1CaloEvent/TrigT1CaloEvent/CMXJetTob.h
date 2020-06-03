/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
  
  #ifndef CMXJetTob_H
  #define CMXJetTob_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {

  /** TOB data received by the merger modules.

      Used in unpacking and analysis of JEP CMX readout data. */
        
    class CMXJetTob  {
    public:

    /** Constructors */
    CMXJetTob();
    CMXJetTob(int crate, int jem, int frame, int loc);
    CMXJetTob(int crate, int jem, int frame, int loc,
              const std::vector<int>& energyLarge,
              const std::vector<int>& energySmall,
 	      const std::vector<int>& error,
	      const std::vector<unsigned int>& presenceMap,
	      int peak);
  
    virtual ~CMXJetTob();

    /** For multi-slice readout, need to indicate position of triggered BC */
    void setPeak(int peak);
    /** Add data to existing object */
    void addTob(const std::vector<int>& energyLarge,
                const std::vector<int>& energySmall,
	        const std::vector<int>& error,
	        const std::vector<unsigned int>& presenceMap);

    /** Data accessors */
    int crate()     const;
    int jem()       const;
    int frame()     const;
    int location()  const;
    /** For triggered time slice */
    int energyLarge() const;
    int energySmall() const;
    int error()       const;
    unsigned int presenceMap() const; // filled for neutral format data only
    /** For multi-slice readout */
    const std::vector<int>& energyLgVec() const;
    const std::vector<int>& energySmVec() const;
    const std::vector<int>& errorVec()    const;
    const std::vector<unsigned int>& presenceMapVec() const;
    int peak() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_jem;
    int m_frame;
    int m_location;
    int m_peak;
    std::vector<int> m_energyLarge;
    std::vector<int> m_energySmall;
    std::vector<int> m_error;
    std::vector<unsigned int> m_presenceMap;
    
    };

  /** returns crate number */
  inline int CMXJetTob::crate() const{
          return m_crate;
  }

  /** returns JEM number */
  inline int CMXJetTob::jem() const{
          return m_jem;
  }

  /** returns frame number */
  inline int CMXJetTob::frame() const{
          return m_frame;
  }

  /** returns location */
  inline int CMXJetTob::location() const{
          return m_location;
  }

  /** returns energy large window for peak sample */
  inline int CMXJetTob::energyLarge() const{
          return m_energyLarge[m_peak];
  }

  /** returns energy small window for peak sample */
  inline int CMXJetTob::energySmall() const{
          return m_energySmall[m_peak];
  }

  /** returns error for peak sample */
  inline int CMXJetTob::error() const{
          return m_error[m_peak];
  }

  /** returns presence map for peak sample */
  inline unsigned int CMXJetTob::presenceMap() const{
          return m_presenceMap[m_peak];
  }

  /** returns energy large window */
  inline const std::vector<int>& CMXJetTob::energyLgVec() const{
          return m_energyLarge;
  }

  /** returns energy small window */
  inline const std::vector<int>& CMXJetTob::energySmVec() const{
          return m_energySmall;
  }

  /** returns error */
  inline const std::vector<int>& CMXJetTob::errorVec() const{
          return m_error;
  }

  /** returns presence maps */
  inline const std::vector<unsigned int>& CMXJetTob::presenceMapVec() const{
          return m_presenceMap;
  }

  /** Set TOB */
  inline void CMXJetTob::addTob(const std::vector<int>& energyLarge,
                                const std::vector<int>& energySmall,
                                const std::vector<int>& error,
			        const std::vector<unsigned int>& presenceMap) {
    m_energyLarge = energyLarge;
    m_energySmall = energySmall;
    m_error = error;
    m_presenceMap = presenceMap;
  }

  /** Specify peak slice  */
  inline void CMXJetTob::setPeak(int peak) {
    m_peak = peak;
  }

  /** returns peak slice number */
  inline int CMXJetTob::peak() const{
          return m_peak;
  }

} // end of namespace

#ifndef CMXJetTob_ClassDEF_H
#include "TrigT1CaloEvent/CMXJetTob_ClassDEF.h"
#endif

#endif
