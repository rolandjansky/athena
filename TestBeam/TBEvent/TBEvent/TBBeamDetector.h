/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBEVENT_TBBEAMDETECTOR_H
#define TBEVENT_TBBEAMDETECTOR_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Principal Base Class for all Testbeam Detectors                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <string>

class TBBeamDetector 
{
 public:

  /////////////////////////////////
  // Constructors and Destructor //
  /////////////////////////////////

  TBBeamDetector() : 
      m_tbDetectorName()
    , m_overflow(false)
    { };

    TBBeamDetector(const std::string& tbBeamDetectorName)
    : m_tbDetectorName(tbBeamDetectorName)
    , m_overflow(false)
      { };

    TBBeamDetector(const std::string& tbBeamDetectorName, bool overflow)
    : m_tbDetectorName(tbBeamDetectorName)
    , m_overflow(overflow)
      { };

  virtual ~TBBeamDetector()
    { }

  ///////////////////////
  // Data Manipulation //
  ///////////////////////

  /////////////
  // Setters //
  ////////////
  virtual void setDetectorName(const std::string& tbBeamDetectorName) 
    { m_tbDetectorName = tbBeamDetectorName; }

  virtual void setOverflow() 
    { m_overflow = true; }

  virtual void setOverflow(bool overflow)
    { m_overflow = overflow; }

  /////////////
  // Getters //
  ////////////

 std::string getDetectorName() const
    { return m_tbDetectorName; }

  bool isOverflow() const
    { return m_overflow; }

  virtual void resetOverflow()
    { m_overflow = false; }

  //////////////////
  // Data Members //
  //////////////////

 private:

  std::string m_tbDetectorName;   /// Detector Name


 protected:
 
  bool m_overflow;

};
#endif
