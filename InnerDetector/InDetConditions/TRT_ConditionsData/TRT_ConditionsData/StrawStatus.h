/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_STRAWSTATUS_H
#define TRTCONDITIONSDATA_STRAWSTATUS_H
namespace TRTCond 
{
  /** @class StrawStatus
      @author Esben Bryndt Klinkby <klinkby@nbi.dk>
      stores straw status and gives access
  */
    class StrawStatus {
    private:
      unsigned int m_status ; //!< the status
    public:
      //enum EStatus { Undefined, Dead, Good } ; //!< the three status states
      enum EStatus { Undefined, Dead, Good, Xenon, Argon, Krypton, EmulateArgon, EmulateKrypton } ; //!< the available status codes
      /** default constructor */
      StrawStatus() : m_status(Undefined) {}
      /** constructor */ 
      StrawStatus(unsigned int mystatus) : m_status(mystatus){};
      /** destructor */
      virtual ~StrawStatus() {}
      /** set status */
      void setstatus(unsigned int i) { m_status = i ; }
      /** get status */
      unsigned int getstatus() const { return m_status ; }
      /** equality op */
      bool operator==(const StrawStatus& rhs) const {
      	return m_status==rhs.m_status; }
      /** print status */
      void print() const { std::cout << "dummy print from StrawStatus.h " << std::endl ; }
    } ;
    
}

CLASS_DEF(TRTCond::StrawStatus,169228333,1) 

#endif
