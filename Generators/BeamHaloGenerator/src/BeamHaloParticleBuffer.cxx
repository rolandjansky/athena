/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/BeamHaloParticleBuffer.h"
#include "BeamHaloGenerator/BeamHaloParticle.h"
#include "CLHEP/Random/RandFlat.h"
#include <iostream>

//------------------------------------------------------------------------------

BinnedInterval::BinnedInterval(double upperBinEdge,
			       const std::vector<double>& intervalUpperBinEdges): m_upperBinEdge(upperBinEdge) {
  m_intervalUpperBinEdges = new std::vector<double>(intervalUpperBinEdges);
}

//------------------------------------------------------------------------------

BinnedInterval::BinnedInterval(const BinnedInterval& binnedInterval) {
  m_upperBinEdge = binnedInterval.m_upperBinEdge;
  m_intervalUpperBinEdges = new std::vector<double>(*(binnedInterval.m_intervalUpperBinEdges));
}

//------------------------------------------------------------------------------

BinnedInterval::~BinnedInterval() {
  if(m_intervalUpperBinEdges) delete m_intervalUpperBinEdges;
}

//------------------------------------------------------------------------------

BeamHaloParticleBuffer::BeamHaloParticleBuffer(const std::string& fileName,
		   CLHEP::HepRandomEngine* engine): m_fileName(fileName),
						    m_ofstream(),
						    m_ifstream(),
						    m_engine(engine),
						    m_numberOfParticles(0), 
						    m_binnedIntervals(),
						    m_upperBinEdge(0.),
						    m_intervalUpperBinEdges(),
						    m_intervalUpperBinEdge(0),
						    m_writeFlag(false) {
  m_recordSize = sizeof(long) + sizeof(double)*8;
  m_particlesPerInterval = 1000;
}

//------------------------------------------------------------------------------

BeamHaloParticleBuffer::~BeamHaloParticleBuffer() {  
}

//------------------------------------------------------------------------------

int BeamHaloParticleBuffer::openForWriting() {
  m_writeFlag = true;
  m_numberOfParticles = 0;
  m_binnedIntervals.clear();
  m_intervalUpperBinEdges.clear();

  m_ofstream.open(m_fileName.c_str(), std::ios::out | std::ios::binary);
  if(!m_ofstream) {
    std::cerr << "Error: could not open output file" << m_fileName << std::endl;
    return 1;
  }

  return 0;  
}

//------------------------------------------------------------------------------

int BeamHaloParticleBuffer::openForReading() {
  if(m_numberOfParticles <= 0) {
    std::cerr << "Error: the number of particles in the input file is not known." << std::endl;
    return 1;
  }

  m_writeFlag = false;

  m_ifstream.open(m_fileName.c_str(), std::ios::in | std::ios::binary);
  if(!m_ifstream) {
    std::cerr << "Error: could not open input file" << m_fileName << std::endl;
    return 1;
  }

  return 0;
}

//------------------------------------------------------------------------------

int BeamHaloParticleBuffer::close() {
  if(m_writeFlag) {
    if(m_ofstream.is_open()) {
      m_ofstream.close();

      // Store the remaining bin edge vector.
      if(m_intervalUpperBinEdges.size() > 0) {
	BinnedInterval binnedInterval(m_upperBinEdge,m_intervalUpperBinEdges);
	m_binnedIntervals.push_back(binnedInterval);
	m_intervalUpperBinEdges.clear();
	m_intervalUpperBinEdge = 0.;
      }
    }
  }
  else {
    if(m_ifstream.is_open()) {
      m_ifstream.close();
    }
  }

  return 0;
}

//------------------------------------------------------------------------------

int BeamHaloParticleBuffer::writeParticle(BeamHaloParticle *particle) {
  if(!m_writeFlag) {
    std::cerr << "Error: the file " << m_fileName << " is currently open for reading." << std::endl;
    return 0;
  }

  long pdgId = 0;
  double px = 0., py = 0., pz = 0., e = 0., x = 0., y = 0., z = 0., t = 0., weight = 0.;

  pdgId = particle->pdgId();
  px = particle->fourVector().px();
  py = particle->fourVector().py();
  pz = particle->fourVector().pz();
  e = particle->fourVector().e();
  x = particle->positionAtScoringPlane().x();
  y = particle->positionAtScoringPlane().y();
  z = particle->positionAtScoringPlane().z();
  t = particle->positionAtScoringPlane().t();
  weight = particle->weight();

  m_ofstream.write((char*)(&pdgId), sizeof(long)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&px), sizeof(double)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&py), sizeof(double)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&pz), sizeof(double)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&e), sizeof(double)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&x), sizeof(double)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&y), sizeof(double)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&z), sizeof(double)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&t), sizeof(double)); if(m_ifstream.bad()) return 0;
  m_ofstream.write((char*)(&weight), sizeof(double)); if(m_ifstream.bad()) return 0;

  m_numberOfParticles++;

  // If 'm_particlesPerInterval' particles have been collected into
  // this interval then push the interval into the map and clear the
  // interval read for more particles.
  if(m_intervalUpperBinEdges.size() >= m_particlesPerInterval) {
    BinnedInterval binnedInterval(m_upperBinEdge,m_intervalUpperBinEdges);
    m_binnedIntervals.push_back(binnedInterval);
    m_intervalUpperBinEdges.clear();
    m_intervalUpperBinEdge = 0.;
  }

  // Append the weight to give the current upper bin edge.  Then push
  // the bin edge back.
  m_intervalUpperBinEdge += weight;
  m_intervalUpperBinEdges.push_back(m_intervalUpperBinEdge);

  // Increment the upper bin edge.  This is a separate number to avoid
  // problems relating to floating point precision which can cause
  //      (total weight) - (weight of particle) = (total weight)
  // when the "total weight" is large and the "weight" is small.  The
  // solution is to factorise the problem into two steps:
  // (1)      "total weight", "weight of bin"
  // (2)      "total weight of bin", "weight of particle"
  m_upperBinEdge += weight;

  return 0;
}

//------------------------------------------------------------------------------

BeamHaloParticle* BeamHaloParticleBuffer::readRandomParticle(void) {
  double generatedWeightSum;
  bool found;
  BeamHaloParticle *beamHaloParticle;

  if(m_writeFlag) {
    std::cerr << "Error: the file " << m_fileName << " is currently open for writing." << std::endl;
    return 0;
  }

  if(!m_engine) { 
    std::cerr << "Error: the RandomEngine pointer is null." << std::endl;
    return 0;
  }

  long particleIndex = 0;

  // Generate a number between 0 and the total weight sum.
  generatedWeightSum = CLHEP::RandFlat::shoot(m_engine)*m_upperBinEdge;

  //std::cout << "Total weight sum = " << m_upperBinEdge << ", generated weight sum = " <<  generatedWeightSum << std::endl;

  // Find the interval which corresponds to this weight sum.
  std::vector<BinnedInterval>::iterator superBinItr = m_binnedIntervals.begin();
  std::vector<BinnedInterval>::iterator superBinItr_end = m_binnedIntervals.end();
  found = false;
  while(superBinItr!=superBinItr_end && !found) {
    //std::cout << "Upper bin edge = " << (*superBinItr).upperBinEdge() << std::endl;

    // This works assuming the elements are correctly ordered.
    if(generatedWeightSum <= (*superBinItr).upperBinEdge()) {
      found = true;
    }
    else {
      ++superBinItr;
      particleIndex += (*superBinItr).intervalUpperBinEdges()->size();
    }
  }
  
  // This should never happen.
  if(!found) {
    std::cerr << "Could not find an interval for the weight sum " << generatedWeightSum << std::endl;
    return 0;
  }

  std::vector<double> *intervalUpperBinEdges = (*superBinItr).intervalUpperBinEdges();

  double intervalWeightSum = intervalUpperBinEdges->back();
  
  
  // Generate a number between 0 and the total weight in this interval
  generatedWeightSum = CLHEP::RandFlat::shoot(m_engine)*intervalWeightSum;

  //std::cout << "Interval weight sum = " << intervalWeightSum << ", generated weight sum = " <<  generatedWeightSum << std::endl;

  // Find the particle within this interval.
  std::vector<double>::iterator intervalItr = intervalUpperBinEdges->begin();
  std::vector<double>::iterator intervalItr_end = intervalUpperBinEdges->end();
  found = false;
  while(intervalItr!=intervalItr_end && !found) {
    //std::cout << "Particle bin edge = " << (*intervalItr) << std::endl;

    // This works assuming the elements are correctly ordered.
    if(generatedWeightSum <= (*intervalItr)) {
      found = true;
    }
    else {
      ++intervalItr;
      ++particleIndex;
    }
  }

  // This should never happen.
  if(!found) {
    std::cerr << "Could not find a particle for the weight sum " << generatedWeightSum << std::endl;
    return 0;
  }  

  if(particleIndex >= m_numberOfParticles) {
    std::cerr << "Particle index " << particleIndex << " is out of range." << std::endl;
    return 0;
  }
  
  // Read this particle from the binary file.
  m_ifstream.seekg(particleIndex*m_recordSize);
  beamHaloParticle = readParticle();
  if(!beamHaloParticle) return 0;
  
  return beamHaloParticle;
}

//------------------------------------------------------------------------------

BeamHaloParticle* BeamHaloParticleBuffer::readParticle(void) {
  if(m_writeFlag) {
    std::cerr << "Error: the file " << m_fileName << " is current open for writing." << std::endl;
    return 0;
  }

  long pdgId = 0;
  double px = 0., py = 0., pz = 0., e = 0., x = 0., y = 0., z = 0., t = 0., weight = 0.;
  
  m_ifstream.read((char*)(&pdgId), sizeof(pdgId)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&px), sizeof(px)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&py), sizeof(py)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&pz), sizeof(pz)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&e), sizeof(e)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&x), sizeof(x)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&y), sizeof(y)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&z), sizeof(z)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&t), sizeof(t)); if(m_ifstream.bad()) return 0;
  m_ifstream.read((char*)(&weight), sizeof(weight)); if(m_ifstream.bad()) return 0;

  HepMC::FourVector fourVector(px,py,pz,e);
  HepMC::FourVector positionAtScoringPlane(x,y,z,t);
  BeamHaloParticle *beamHaloParticle = new BeamHaloParticle(pdgId, fourVector, positionAtScoringPlane, weight);

  return beamHaloParticle;
}

//------------------------------------------------------------------------------
