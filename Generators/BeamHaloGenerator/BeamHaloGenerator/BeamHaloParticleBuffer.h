/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BEAMHALOPARTICLEBUFFER_H
#define BEAMHALOPARTICLEBUFFER_H

#include <fstream>
#include <vector>
#include <map>

class BeamHaloParticle;
namespace CLHEP {
  class HepRandomEngine;
}

class BinnedInterval {
 public:
  BinnedInterval(double upperBinEdge,
		 const std::vector<double>& intervalUpperBinEdges);
  BinnedInterval(const BinnedInterval& binnedInterval);
  ~BinnedInterval();
 
  /** Assignment operator */
  BinnedInterval & operator=(const BinnedInterval &rhs) 
    {
      if ( this != &rhs ) // Fix Coverity SELF_ASSIGN
      {
        m_upperBinEdge = rhs.m_upperBinEdge;
        std::vector<double>* tmp = new std::vector<double>(*(rhs.m_intervalUpperBinEdges));
        delete m_intervalUpperBinEdges;
        m_intervalUpperBinEdges = tmp;
      }
      return *this;
    }

  double upperBinEdge() const { return m_upperBinEdge; }
  std::vector<double>* intervalUpperBinEdges() const { return m_intervalUpperBinEdges; }

 private:
  double m_upperBinEdge;
  std::vector<double>* m_intervalUpperBinEdges;
};

class BeamHaloParticleBuffer {
 public:
  BeamHaloParticleBuffer(const std::string& fileName, 
			 CLHEP::HepRandomEngine *engine);
  ~BeamHaloParticleBuffer();
  int openForWriting();
  int openForReading();
  int close();

  /** A member function to append a particle to the binary file. */
  int writeParticle(BeamHaloParticle *particle);

  /** A member function to read a random particle from the binary file. */
  BeamHaloParticle* readRandomParticle(void);
  
 private:

  /** A member function to read a particle from the current position
      in the binary file. */
  BeamHaloParticle* readParticle(void);

  /** File name */
  std::string m_fileName;

  /** A data member to store the output file stream */
  std::ofstream m_ofstream;

  /** A data member to store the input file stream */
  std::ifstream m_ifstream;

  /** A pointer to a random number engine used for randomly selecting
      particles from the binary file. */
  CLHEP::HepRandomEngine* m_engine;

  /** The size of 1 binary data record. */
  int m_recordSize;

  /** The number of particles per interval. */
  unsigned int m_particlesPerInterval;

  /** The number of particles present within the output or input file */
  long m_numberOfParticles;

  /** A map of the upper bin limits and an vector of bin limits within
   ** the given interval.  The file is split into intervals to avoid
   ** floating point precision problems.  Each interval is considered
   ** as a separate histogram.  The lower limit for the interval is
   ** taken from the last bin or 0 for the first bin entry. */
  std::vector<BinnedInterval> m_binnedIntervals;

  /** The upper edge of the bin */
  double m_upperBinEdge;

  /** A vector of bin limits for a given interval. */
  std::vector<double> m_intervalUpperBinEdges;

  /** The upper bin edge of a bin within a given interval. */
  double m_intervalUpperBinEdge;

  /** A flag to select read or write */
  bool m_writeFlag;
};

#endif
