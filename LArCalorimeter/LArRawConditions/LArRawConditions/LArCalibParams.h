//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARCALIBPARAMS_H
#define LARRAWCONDITIONS_LARCALIBPARAMS_H

#include "Identifier/HWIdentifier.h"

#include <vector>
#include <map>

class LArOnlineID;
class StatusCode;

/** This class contains the LAr Calibration Board settings
 *  and a set of functions to decoded them. 
 *
 * @author W. Lampl 
 * @version  0-0-1 , 24.5.2004
 
 */

class LArCalibParams {
  
 public: 
  
  LArCalibParams();

  virtual ~LArCalibParams( );
  virtual StatusCode initialize ( );
  
  void set(const HWIdentifier CalibModuleID, const unsigned nTrigger,
	   const  std::vector<unsigned>& Pattern, const std::vector<unsigned>& DAC, const std::vector<unsigned>& Delay);

  //Special set function, assume only one set of parameters valid for all boards 
  void set(const unsigned nTrigger, const std::vector<unsigned>& Pattern, const std::vector<unsigned>& DAC, 
	   const std::vector<unsigned>& Delay);


  unsigned Delay(const unsigned event, const HWIdentifier calibLineID) const;
      
  unsigned DAC(const unsigned event, const HWIdentifier calibLineID) const ;
  
  bool isPulsed(const unsigned event, const HWIdentifier calibLineID) const; 

  unsigned NTrigger(const HWIdentifier calibModuleID) const;

  unsigned getNumberConfig(const HWIdentifier calibModuleID) const;

  //const CalibBoard& getCalibBoardParams(const HWIdentifier calibModuleID) const;

  class CalibBoard {
  public:
    unsigned m_nTrigger;
    std::vector<unsigned> m_Pattern;
    std::vector<unsigned> m_DAC;
    std::vector<unsigned> m_Delay;
    //GetFunctions:
    unsigned Delay (const unsigned event) const;
    unsigned DAC (const unsigned event) const;
    bool isPulsed(const unsigned event, const unsigned short calibLine) const;
  };
  
 private:
  std::map< HWIdentifier, CalibBoard> m_mCalibBoards;
  const LArOnlineID* m_onlineHelper;
  bool m_uniqueConfig; //True, if the same parameters are valid for all boards.

};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArCalibParams, 228761010,1 )


#endif 
