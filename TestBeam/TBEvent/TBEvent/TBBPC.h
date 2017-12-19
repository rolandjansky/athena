/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBBPC_H
#define TBEVENT_TBBPC_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Reconstructed BPC data                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <string>

#include "AthenaKernel/CLASS_DEF.h"


#include "TBEvent/TBBeamDetector.h"



class TBBPC : public TBBeamDetector
{
 public:
  
  typedef double signal_type;
  
  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////
    
  TBBPC();
  
  TBBPC(const std::string& thisBPCName);

  ~TBBPC();
  
  ///////////////////////
  // Data Manipulation //
  ///////////////////////
    
  // set signals
  void setXPos(signal_type theSignal);
  void setYPos(signal_type theSignal);
  void setXErr(signal_type theSignal);
  void setYErr(signal_type theSignal);

  void setXPulse(signal_type theSignal);
  void setYPulse(signal_type theSignal);

  void setHitNbr(int hitnbr);

  // set/reset overflow
  void setXPosOverflow(bool overflow=true);
  void setYPosOverflow(bool overflow=true);
  void setXPulseOverflow(bool overflow=true);
  void setYPulseOverflow(bool overflow=true);

  //  void resetXPosOverflow();
  //  void resetYPosOverflow();
  //  void resetXPulseOverflow();
  //  void resetYPulseOverflow();

  //  void setXPosOverflow(bool overflow);
  //  void setYPosOverflow(bool overflow);
  //  void setXPulseOverflow(bool overflow);
  //  void setYPulseOverflow(bool overflow);
  
  // set global overflow
  //  virtual void setOverflow();
  //  virtual void setOverflow(bool overflow);
  //  virtual void resetOverflow();

  // access signals
  inline signal_type getXPos() const
    { return (signal_type) m_xPos; }
  inline signal_type getYPos() const
    { return (signal_type) m_yPos; }

  inline signal_type getXErr() const
    { return (signal_type) m_xErr; }
  inline signal_type getYErr() const
    { return (signal_type) m_yErr; }

  inline signal_type getXPulse() const
    { return m_xPulse; }
  inline signal_type getYPulse() const
    { return m_yPulse; }
  inline int getHitNbr() const 
    {return m_hitnumber;}

  // access overflow for each variable
  inline bool isXPosOverflow() const
    { return m_xPosOverflow; }
  inline bool isYPosOverflow() const
    { return m_yPosOverflow; }
  inline bool isXPulseOverflow() const
    { return m_xPulseOverflow; }
  inline bool isYPulseOverflow() const
    { return m_yPulseOverflow; }

  // data class reset
  void reset();

 private:

  //////////////////
  // Private Data //
  //////////////////


  // reconstructed (x,y)
  signal_type m_xPos,m_yPos;
  signal_type m_xErr,m_yErr;

  // associated pulse height sums
  signal_type m_xPulse, m_yPulse;

  // BPC can distinguish between "0" or "1 or more" particle hits
  short m_hitnumber;

  // overflow indicators
  bool m_xPosOverflow,   m_yPosOverflow;
  bool m_xPulseOverflow, m_yPulseOverflow;

  bool m_overflowSetFlag;
};

CLASS_DEF( TBBPC , 228151918 , 1 )
#endif



