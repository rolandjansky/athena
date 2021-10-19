/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiHit.h
//   Header file for class SiHit
///////////////////////////////////////////////////////////////////
// Class for the Pixel and SCT hits
///////////////////////////////////////////////////////////////////

#ifndef INDETSIMEVENT_SIHIT_H
#define INDETSIMEVENT_SIHIT_H

// Data members classes
#include "CLHEP/Geometry/Point3D.h"
#include "GeneratorObjects/HepMcParticleLink.h"

class SiHit final {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  //   local start position of the energy deposit
  //   local end position of the energy deposit
  //   deposited energy
  //   time of energy deposition
  //   number of track which released this energy
  //
  SiHit(const HepGeom::Point3D<double> &localStartPosition,
        const HepGeom::Point3D<double> &localEndPosition,
        const double energyLoss,
        const double meanTime,
        const int trackNumber,
        const unsigned int id);

  SiHit(const HepGeom::Point3D<double> &localStartPosition,
        const HepGeom::Point3D<double> &localEndPosition,
        const double energyLoss,
        const double meanTime,
        const int trackNumber,
        const int, const int, const int, const int, const int, const int);
  // Constructor with parameters:
  //   local start position of the energy deposit
  //   local end position of the energy deposit
  //   deposited energy
  //   time of energy deposition
  //   link to particle which released this energy
  //
  SiHit(const HepGeom::Point3D<double> &localStartPosition,
        const HepGeom::Point3D<double> &localEndPosition,
        const double energyLoss,
        const double meanTime,
        const HepMcParticleLink &track,
        const unsigned int id);

  SiHit(const HepGeom::Point3D<double> &localStartPosition,
        const HepGeom::Point3D<double> &localEndPosition,
        const double energyLoss,
        const double meanTime,
        const HepMcParticleLink &track,
        const int, const int, const int, const int, const int, const int);
  // needed by athenaRoot
  SiHit();

  // Destructor:
  ~SiHit() = default;
  
  //move assignment defaulted
  SiHit & operator = (SiHit &&) = default;
  //assignment defaulted
  SiHit & operator = (const SiHit &) = default;
  //copy c'tor defaulted
  SiHit(const SiHit &) = default;

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  unsigned int identify() const;

  // local start position of the energy deposit:
  HepGeom::Point3D<double> localStartPosition() const;

  HepGeom::Point3D<double> localEndPosition() const;

  // deposited energy:
  double energyLoss() const;

  // time of energy deposition: FIXME name!
  double meanTime() const;

  // Set the time of energy deposition: FIXME name!
  void setMeanTime(float meanTime);

  // number of track which released this energy:
  int trackNumber() const;

  // link to the particle generating the hit
  const HepMcParticleLink& particleLink() const;

  // Pixel, SCT, or HGTD
  bool isPixel() const;
  bool isSCT() const;
  bool isHGTD() const;
  bool isPLR() const;

  // Barrel or Endcap
  int getBarrelEndcap() const;

  // Layer/Disk
  int getLayerDisk() const;

  // eta module
  int getEtaModule() const;

  // phi module
  int getPhiModule() const;

  // side
  int getSide() const;

  // some print-out:
  void print() const;

  bool operator < (const SiHit& rhs) const
  {return m_ID < rhs.m_ID;}

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////
  // Scale the length, used to go from cm to mm, of whatever we like.
  void ScaleLength(double);

  ///////////////////////////////////////////////////////////////////
  // copy c'tor made public on 23-5-2008 to allow copy of athenahitsvector<sihit>
  ///////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  float m_stX, m_stY, m_stZ;
  float m_enX, m_enY, m_enZ;
  float m_energyLoss; // deposited energy
  float m_meanTime; // time of energy deposition
  HepMcParticleLink m_partLink;
  unsigned int m_ID;
public:
  enum
    { xDep = 0, xPhi = 1, xEta = 2};
};


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline unsigned int SiHit::identify() const
{
  return m_ID;
}

inline double SiHit::energyLoss() const
{
  return (double) m_energyLoss;
}

inline double SiHit::meanTime() const
{
  return (double) m_meanTime;
}

inline void SiHit::setMeanTime(float meanTime)
{
  m_meanTime=meanTime;
}

inline const HepMcParticleLink& SiHit::particleLink() const
{
  return m_partLink;
}



///////////////////////////////////////////////////////////////////
// open functions:
///////////////////////////////////////////////////////////////////

inline float hitTime(const SiHit& hit)
{
  return (float) hit.meanTime();
}

#endif // INDETSIMEVENT_SIHIT_H
