//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBEVENTINFO_H
#define TBEVENT_TBEVENTINFO_H 
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Trigger pattern for an event.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <string>







class TBEventInfo
{
 public:
  

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBEventInfo() { 
    m_ev_number=0;
    m_ev_clock=0;
    m_ev_type=0;
    m_run_num=0;
    m_beam_moment=0;
    m_beam_part="";
    m_cryoX=0;
    m_cryoAngle=0;
    m_tableY=0; };
    
  TBEventInfo(int ev_number,
	      int ev_clock,
	      int ev_type,
	      unsigned int run_num,
	      float beam_moment,
	      std::string beam_part,
	      float cryoX,
	      float cryoAngle,
	      float tableY) :    m_ev_number(ev_number), m_ev_clock(ev_clock),   m_ev_type(ev_type),   m_run_num(run_num),  m_beam_moment(beam_moment),  m_beam_part(beam_part),  m_cryoX(cryoX),  m_cryoAngle(cryoAngle),  m_tableY(tableY) {};

  ~TBEventInfo();
  
  ///////////////////////
  // Data Manipulation //
  ///////////////////////
    
  //    void setBit(int i,bool status){if(i<m_triggers.size()) m_triggers[i]=status;}
  int getEventNum() const {return m_ev_number;}
  int getEventClock() const {return m_ev_clock;}
  int getEventType() const {return m_ev_type;}
  unsigned int getRunNum() const {return m_run_num;}
  float getBeamMomentum() const {return m_beam_moment;}
  std::string getBeamParticle() const {return m_beam_part;}
  float getCryoX() const {return m_cryoX;}
  float getCryoAngle() const {return m_cryoAngle;}
  float getTableY() const {return m_tableY;}

 private:
    
  //////////////////
  // Private Data //
  //////////////////

  int m_ev_number;
  int m_ev_clock;
  int m_ev_type;
  unsigned int m_run_num;
  float m_beam_moment;
  std::string m_beam_part;
  float m_cryoX;
  float m_cryoAngle;
  float m_tableY;
  
};

CLASS_DEF( TBEventInfo , 168973241 , 1 )

#endif
