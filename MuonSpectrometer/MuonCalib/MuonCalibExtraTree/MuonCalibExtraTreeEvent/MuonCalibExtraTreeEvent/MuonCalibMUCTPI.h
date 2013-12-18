/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCALIBMUCTPI_H
#define MUONCALIBMUCTPI_H

#include <string>
#include <vector>

namespace MuonCalib{

  /**
     Simplified class designed to store information from MuCTPi processor. It has :
     
     @author Giuseppe Salamanna
  */
  class MuonCalibMUCTPI{
  public:
    MuonCalibMUCTPI();                                                                               //!< default constructor
    MuonCalibMUCTPI( int roiWord, int bcID, int sysID, int subsysID, int sectorID, int thresholdNumber, int thresholdValue, int roINumber,
		     int overlap, int firstCandidate, int sectorOverflow, int padOverflow,
		     float phi, float eta ); //!< constructor setting all trackparameters and position
    ~MuonCalibMUCTPI() {} ;                                                                          //!< destructor
    
    int roiWord()  const   { return m_roiWord; }
    int bcID()     const   { return m_bcID; };
    int sysID()    const   { return m_sysID; };
    int subsysID() const   { return m_subsysID; };
    int sectorID() const   { return m_sectorID; };
    int thresholdNumber() const   { return m_thresholdNumber; };
    int thresholdValue()  const   { return m_thresholdValue; };
    int roINumber() const   { return m_roINumber; };
    int overlap()   const   { return m_overlap; };
    int firstCandidate() const   { return m_firstCandidate; };
    int sectorOverflow() const   { return m_sectorOverflow; };
    int padOverflow() const   { return m_padOverflow; };
    float phi() const   { return m_phi; };
    float eta() const   { return m_eta; };
    
  private:
    int m_roiWord;
    int m_bcID;
    int m_sysID;
    int m_subsysID;
    int m_sectorID;
    int m_thresholdNumber;
    int m_thresholdValue;
    int m_roINumber;
    int m_overlap;
    int m_firstCandidate;
    int m_sectorOverflow;
    int m_padOverflow;
    float m_phi;
    float m_eta;
    
  } ;

}//namespace MuonCalib

#endif 
