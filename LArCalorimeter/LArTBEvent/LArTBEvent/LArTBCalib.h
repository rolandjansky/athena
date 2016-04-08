/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARTBCALIB_H
#define LARTBCALIB_H


#include <vector>
#include <string>

/** @class LArTBCalib LArTBCalib.h "LArTBEvent/LArTBCalib.h"
    
    @ author A. Abdesselam and P. Ghez
*/
class LArTBCalib {
 public:
  
  /** Constructor */
  LArTBCalib(int boardNumber, int nbPattern, int iPattern, int nbTrigger, 
	     int iTrigger, int nbDAC, int iDAC, int nbDelay, int specDelay, 
	     std::vector< int > DACValues, std::vector< std::vector < int > > 
	     delayValues, std::vector< std::vector < int > > patternValues);
  
  /** Destructor */
  virtual ~LArTBCalib();
  
  /* ------------------------------------------------------------  */
  /* ------------------------ G E T T E R ------------------------ */
  /* ------------------------------------------------------------  */  

  /** Get calibratoin board number */
  inline int getBoardNumber() const{ return m_boardNumber;}

  /** Get the number of pattern for calibration line */
  inline int getNbPattern() const{ return m_nbPattern;}

  /** Get the number of DAC */
  inline int getNbDAC() const{ return m_nbDAC;}

  /** Get pattern number for the current event (not available) */
  inline int getIPattern() const{ return m_iPattern;}
  
  /** Get the number of trigger per pattern */
  inline int getNbTrigger() const{ return m_nbTrigger;}

  /** Get the trigger number in one pattern (not available) */
  inline int getITrigger() const{ return m_iTrigger;}

  /** Get DAC number for the current event (not availble ) */
  inline int getIDAC() const{ return m_iDAC;}
  
  /** Get the number of delay */
  inline int getNbDelay() const{ return m_nbDelay;}

  /** Get the delay number for the current event (not available) */
  inline int getSpecDelay() const{ return m_specDelay;}

  /** Get the collection of DAC values */
  inline std::vector < int > getDACValues() const{ return m_DACValues;}

  /** Get the collection of Delay values */
  inline std::vector < std::vector < int > > getDelayValues() const
    { return m_delayValues;}

  /** Get the collection of pattern(calibration lines) values */
  inline std::vector < std::vector < int > > getPatternValues() const
    { return m_patternValues;}
  
  /**  Convertion operator to a std::string  */
  virtual operator std::string() const;

 private:
  LArTBCalib();
  int m_boardNumber;     /**< board number */
  int m_nbPattern;       /**< number of patterns */
  int m_iPattern;        /**< pattern index changes event by event */
  int m_nbTrigger;       /**< number of trigger */
  int m_iTrigger;        /**< curent trigger */
  int m_nbDAC;           /**< number of DAC values */
  int m_iDAC;            /**< dac index changes event by event */
  int m_nbDelay;         /**< number of delays */
  int m_specDelay;       /**< calib has different delay values */
  std::vector < int > m_DACValues;   /**< all DAC values */
  std::vector < std::vector < int > > m_delayValues; /**< all pattern values */
  std::vector < std::vector < int > > m_patternValues;/**< all delay values */

}; 
#endif //LARTBCALIB_H
