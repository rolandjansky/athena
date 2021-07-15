/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Simulation data object associated with an MuonRawData object
 Adapated from the InnerDetector by Ketevi A. Assamagan
 ------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************
 An object of this class stores the simulation information associated with 
 a RawData object, in two data members. 

 One is an integer, the "simulation data word", in which is packed 
 information summarizing the digitization. Its interpretation may depend on 
 which of the three subdetectors is concerned, but will typically contain 
 bit-flags for "noise", "lost in readout", etc.

 The other consists of a number (normally only one) of  pair<int, float> 
 specifying the barcode and one additional piece of information depending 
 on the Muon technology: it could deposited energy (charge), hit time, to
 be associated to a particular Muon Digit. - Ketevi A. Assamagan
 Encoding and decoding methods will be supplied outside this class.

 The implementation emphasizes compactness in memory, at the expense of speed,
 because objects of this class will be persistent, but will not be accessed
 in real production running. 
 
// $Id: MuonSimData.h,v 1.4 2004-07-03 09:04:40 ketevi Exp $

------
 Changed by Niels van Eldik <nveldik@nikhef.nl> 2004/06/03

  - Added MuonMCData, which allows the storage of two values per Deposit
    (needed  by the MDT's: radius + local z position)

***************************************************************************/

#ifndef MUONSIMDATA_MuonSimData_H
# define MUONSIMDATA_MuonSimData_H

class MuonMCData {
 public:
  MuonMCData() : m_first(0.), m_second(0.) {};
  MuonMCData(double f, float s) : m_first(f), m_second(s) {};

  void   SetData(double f, float s) { m_first = f; m_second = s ;}
  double firstEntry() const { return m_first; }
  float  secondEntry() const { return m_second; }
  void   addToFirstEntry (const double fEntry) { m_first  += fEntry; }
  void   addToSecondEntry(const float  sEntry) { m_second += sEntry; }
 private:
  double m_first;
  float  m_second;
};

#include <utility>
#include <vector>
#include "GeneratorObjects/HepMcParticleLink.h"
#include "GeoPrimitives/GeoPrimitives.h"

class MuonSimData {
    
public:

    typedef std::pair<HepMcParticleLink, MuonMCData> Deposit; // A particle link, and two 
    // other values relevent for the subsystem
    MuonSimData();
    MuonSimData (const std::vector< Deposit >& deposits, int simDataWord = 0);
    MuonSimData (std::vector< Deposit >&& deposits, int simDataWord = 0);
    int word() const;                       // Get the packed simdata word
    void deposits(std::vector<Deposit>& deposits) const; // Get the Deposits
    const std::vector< Deposit >& getdeposits() const;
    void setPosition(const Amg::Vector3D& pos);
    const Amg::Vector3D globalPosition() const;
    void setTime(const float& time);
    float getTime() const;
    
private:
    int m_word;
    std::vector<Deposit> m_deposits;
    float m_x;
    float m_y;
    float m_z;
    float m_t;
};


inline int MuonSimData::word() const
{
    return m_word & 0x1fffffff;
}

inline const std::vector< MuonSimData::Deposit >& MuonSimData::getdeposits() const
{
  return m_deposits;
}

inline void MuonSimData::deposits(std::vector< MuonSimData::Deposit>& deposits) const
{
    deposits = m_deposits;
    return;
}


inline void MuonSimData::setPosition(const Amg::Vector3D& pos)
{
  m_x = pos.x();
  m_y = pos.y();
  m_z = pos.z();
}

inline const Amg::Vector3D MuonSimData::globalPosition() const
{
  Amg::Vector3D globPos(m_x, m_y, m_z);
  return globPos;
}

inline void MuonSimData::setTime(const float& time)
{
  m_t =time;
}

inline float MuonSimData::getTime() const
{
  float time= m_t;
  return time;
}

#endif // MUONSIMDATA_MuonSimData_H

