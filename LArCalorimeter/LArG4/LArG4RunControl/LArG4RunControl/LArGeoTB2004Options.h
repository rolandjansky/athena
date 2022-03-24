/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4RUNCONTROL_LARGEOTB2004OPTIONS_H
#define LARG4RUNCONTROL_LARGEOTB2004OPTIONS_H
#include "CLHEP/Units/SystemOfUnits.h"

// ----------------------------------------------------------------------//
//                                                                       //
// This is for the 2004 Combined Endcap Test Beam                        //
//                                                                       //
//-----------------------------------------------------------------------//

class LArGeoTB2004Options {

 public:
  // A constructor to insure that the data starts up with a reasonable set
  // of defaults:

  inline LArGeoTB2004Options() : 
    m_CryoXPosition(0.0),
    m_TableYPosition(0.0),
    m_Run1(false),m_emec(true),m_hec(true),m_fcal(true),m_coldnose(true),m_printstep(false),m_checkprim(false),m_checkother(false)
  {}

  void saveMe();
  void printMe() const;

  // Set methods
  inline void CryoXPosition(double value)			{ m_CryoXPosition    = value; }
  inline void TableYPosition(double value)			{ m_TableYPosition   = value; }
  inline void Run1(bool value)					{ m_Run1   = value; }
  inline void Emec(bool value)					{ m_emec   = value; }
  inline void Hec(bool value)					{ m_hec   = value; }
  inline void Fcal(bool value)					{ m_fcal   = value; }
  inline void Coldnose(bool value)				{ m_coldnose   = value; }
  inline void Printstep(bool value)                             { m_printstep   = value; }
  inline void Checkprim(bool value)                             { m_checkprim   = value; }
  inline void Checkother(bool value)                            { m_checkother   = value; }

  // Get methods
  inline double CryoXPosition(void) const		      { return m_CryoXPosition; }
  inline double TableYPosition(void) const		      { return m_TableYPosition; }
  inline bool isRun1(void) const	      		      { return m_Run1; }
  inline bool isEmec(void) const		      	      { return m_emec; }
  inline bool isHec(void) const		      		      { return m_hec; }
  inline bool isFcal(void) const		       	      { return m_fcal; }
  inline bool isColdnose(void) const		      	      { return m_coldnose; }
  inline bool isPrintstep(void) const                         { return m_printstep; }
  inline bool isCheckprim(void) const                         { return m_checkprim; }
  inline bool isCheckother(void) const                        { return m_checkother; }

 private:
  // X position of cryostat
  double m_CryoXPosition;
  // Y position of beamline table
  double m_TableYPosition;
  // is this a RUN-II ?
  bool m_Run1;
  // construct EMEC ?
  bool m_emec;
  // construct HEC ?
  bool m_hec;
  // construct FCAL ?
  bool m_fcal;
  // construct ColdNose ?
  bool m_coldnose;
  // printing stepping info ?
  bool m_printstep;
  // checking primary particle ?
  bool m_checkprim;
  // other checks ?
  bool m_checkother;
 
};

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(LArGeoTB2004Options, 120300909, 1)
#endif

#endif
