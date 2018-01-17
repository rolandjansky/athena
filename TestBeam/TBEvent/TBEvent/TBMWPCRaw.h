/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBMWPCRAW_H
#define TBEVENT_TBMWPCRAW_H
//////////////////////////////////////////////////////////////////////
//                                                                  //
// Raw data class for Beam Projection Chambers (MWPC with time      //
// delay readout). Actually each instance describes a MWPC plane.   //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "TBEvent/TBBeamDetector.h"



class TBMWPCRaw : public TBBeamDetector
{
 public:

  typedef unsigned short signal_type;
  typedef unsigned int   source_type;

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBMWPCRaw();  
  TBMWPCRaw(const std::string& thisBPCName);
  ~TBMWPCRaw();

  ///////////////////////
  // Data Manipulation //
  ///////////////////////

  // signal type definition
  enum SignalType {
    tdcLeft=0, tdcRight, tdcDown, tdcUp
  };
  
  // set signal  //////////////////////////////////////////////////////
  //  void setSignal(source_type theSignalType, signal_type theSignal);

  // set/reset overflow for each signal
/*   void setOverflow(source_type theSignalType); */
/*   void setOverflow(source_type theSignalType, bool overflow); */
/*   void resetOverflow(source_type theSignalType); */


  /** Set the collection of the center wire number */
  inline void setCwireno(std::vector<int> cwireno) {
    m_cwireno = cwireno;
  }
  inline void addCwireno(int cwire){
    m_cwireno.push_back(cwire);
  }

  /** Set the collection of the number of wires in a cluster */
  inline void setNwires(std::vector<int> nwires) {
    m_nwires  = nwires;
  }    
  inline void addNwires(int nwire){
    m_nwires.push_back(nwire);
  }

  // set X or Y
  inline void setXchambers(bool isX){m_isX=isX;}  


  // set global overflow
  virtual void setOverflow();
  virtual void setOverflow(bool overflow);
  virtual void resetOverflow();


  // access signal  //////////////////////////////////////////////////


  /** Get the collection of the center wire number of a cluster */
  inline const std::vector<int> & getCwireno()
    const { return m_cwireno; }

  /** Get the collection of the number of wire of a cluster */
  inline const std::vector<int> & getNwires()
    const { return m_nwires; }



  inline bool isX() const {return m_isX;}
  
  // access overflow for each signal type
  //  bool isOverflow(source_type theSignalType) const;

  // data class reset
  void reset();

 private:

  //////////////////
  // Private Data //
  //////////////////

  //  std::vector<signal_type> m_signals;
  std::vector<bool>        m_overflowType;

  // Variables used in 2002 HEC+EMEC TB copied from LArTBeamChambers :
  std::vector<int> m_cwireno;   /**< center wire number */
  std::vector<int> m_nwires;    /**< the number of wires a cluster */



  bool m_isX;

  bool                     m_overflowSetFlag;

  ///////////////////////////////
  // Private Data Manipulation //
  ///////////////////////////////

    //  void setUpStores();

};

CLASS_DEF( TBMWPCRaw , 239046776 , 1 )

#endif


