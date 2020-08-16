/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCOND_DETSTATUS_H
#define DETDESCRCOND_DETSTATUS_H
// DetStatus.h - class to hold details of a subdetector status
// Richard Hawkings, started 19/12/06

#include<string>

class DetStatus final {
 public:
  DetStatus();
  // constructor with parameters
  DetStatus(const int fullcode, 
	    const float deadfrac, const float deadthrust);

  ~DetStatus() = default;

  // access functions
  int code() const;
  int fullCode() const;
  float deadFrac() const;
  float deadThrust() const;
  std::string colour() const;

 private:
  int m_code;
  float m_deadfrac;
  float m_deadthrust;
};

inline int DetStatus::code() const { return m_code<0 ? m_code : m_code & 3; }
inline int DetStatus::fullCode() const { return m_code; }
inline float DetStatus::deadFrac() const { return m_deadfrac;}
inline float DetStatus::deadThrust() const { return m_deadthrust;}

#endif // DETDESCRCOND_DETSTATUS_H
