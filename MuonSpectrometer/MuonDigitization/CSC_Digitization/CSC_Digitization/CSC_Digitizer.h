/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_CSC_DIGITIZER_H
#define MUONDIGITIZATION_CSC_DIGITIZER_H

#include <algorithm>
#include <vector>
#include <map>
#include <cassert>

#include "GaudiKernel/StatusCode.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"

#include "MuonIdHelpers/CscIdHelper.h"

#include "MuonSimEvent/CSCSimHit.h"
#include "MuonSimEvent/CscHitIdHelper.h"

#include "GaudiKernel/ServiceHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "CscCalibTools/ICscCalibTool.h"

class IAtRndmGenSvc;

// Author: Ketevi A. Assamagan
// BNL, October 17 2003

// Digitization class for CSC hits

class DigitData {
public:
  double driftTime0;
  double driftTime;
  double stripCharge;
};

class CSC_Digitizer {

public:

  // Constructors

  // default constructor should not be used
  CSC_Digitizer() {};

  // full constructor
  CSC_Digitizer(CscHitIdHelper * cscHitHelper, const MuonGM::MuonDetectorManager* muonMgr, ICscCalibTool* pcalib);

  // Destructor
  ~CSC_Digitizer() {
    if (m_sprob) delete [] m_sprob;
  }
  // Public methods

  // initialize random number generators
  StatusCode initialize();

  // set the digitization time window
  void setWindow (const double t1, const double t2);

  void set (const double bunchTime);

  void setAmplification(const double amplification) {
    m_amplification = amplification;
  }

  void setDebug(int debug) { m_debug = debug; }
    
  void setDriftVelocity(double v0) { m_driftVelocity = v0; }
  //  void setMaxElectron  (int max)   { m_maxElectron = max; }
  void setElectronEnergy(double e) { m_electronEnergy = e; } // eV
  void setNInterFixed() {m_NInterFromEnergyLoss=false;}
  
  // digitize a single hit
  StatusCode digitize_hit(const CSCSimHit * cscHit, std::vector<IdentifierHash> & hashVec,
			  std::map<IdentifierHash, std::pair<double,double> >& data_map,
			  CLHEP::HepRandomEngine* rndmEngine);
  StatusCode digitize_hit (const CSCSimHit * cscHit, 
                           std::vector<IdentifierHash>& hashVec,
                           std::map<IdentifierHash,std::vector<float> >& data_SampleMap,
                           std::map<IdentifierHash,std::vector<float> >& data_SampleMapOddPhase,
                           CLHEP::HepRandomEngine* rndmEngine);
  StatusCode digitize_hit (const CSCSimHit * cscHit, 
                           std::vector<IdentifierHash>& hashVec,
                           std::map<IdentifierHash,std::vector<float> >& data_SampleMap,
                           CLHEP::HepRandomEngine* rndmEngine);

  // input parameters should be as form of cscHelper returned value....
  IdentifierHash getHashId(int eta, int phiSector, int chamberLayer, int chamberType, int wireLayer, int stripId, int maxStrip, int measuresPhi) {

    int etaIndex = (eta==-1) ? 1 : 2;
    
    IdentifierHash hashId = (stripId-1)+maxStrip*(wireLayer-1)+4*maxStrip*(chamberLayer-1)
      +8*maxStrip*(phiSector-1)+64*maxStrip*(etaIndex-1)+m_hashOffset[chamberType][measuresPhi];

    return hashId;
  }

private:
  
  // private methods need to compute the induce charge on the strip
  double qWire         (const int & nElectrons, const double & gammaDist) const;
  double qStripR       (const double x) const;
  double qStripR       (const double x, const std::string stationType) const;
  double qStripPhi     (const double x, const std::string stationType) const;
  double fparamPhi     (const double x, const int N, const double * p) const;   
  double getDriftTime(const MuonGM::CscReadoutElement* descriptor, const Amg::Vector3D& pos) const;

  bool outsideWindow(double time) const; // default +-50...
  void fillMaps(const IdentifierHash hash, const double driftTime, const double stripCharge,
		std::vector<IdentifierHash>& hashVec,
		std::map<IdentifierHash,std::pair<double,double> >& data_map);
  //  void fillSampleMaps(const IdentifierHash hash, const double driftTime, const double stripCharge,
  //                      std::vector<IdentifierHash>& hashVec,
  //                      std::map<IdentifierHash,std::vector<float> >& data_map); // new interface trying to provide 4 samples instead of q,t
  void fillSampleMaps(const IdentifierHash hash, const double driftTime, const double stripCharge,
                      std::vector<IdentifierHash>& hashVec,
                      std::map<IdentifierHash,std::vector<float> >& data_map, bool phase=0); // new interface trying to provide 4 samples instead of q,t
  
private:
  
  // private data members
  CscHitIdHelper * m_cscHitHelper;
  const   CscIdHelper * m_cscIdHelper;
  const   MuonGM::MuonDetectorManager * m_muonMgr;
  ICscCalibTool* m_pcalib;
  // Calibration tool.
  bool    m_pointingPhiStrips;
  //  double m_FlatDist;
  //  double m_GaussDist;  
  //  double m_PoissonDist;  
  //  double m_GammaDist;
  int    m_maxElectron;
  bool    m_NInterFromEnergyLoss;
  double m_electronEnergy;
  
  double m_Polia;
  double * m_sprob;
  size_t m_hashOffset[2][2];
  double m_timeWindowLowerOffset;
  double m_timeWindowUpperOffset;
  double m_bunchTime;
  double m_amplification;
  double m_driftVelocity;
  int m_debug;
};


// public methods
// 10/2010
// New way of adding charges...
// only care for samples...
// charges are represented by samples...
// and then in the end we need to convert them into one pair of time and charge (short term goal)
// Long term goal, we need to change EDM to carry over sample charges to RDOCollection...
inline void CSC_Digitizer::fillSampleMaps(const IdentifierHash hashId, 
                                          const double driftTime,
                                          const double stripCharge,
                                          std::vector<IdentifierHash>& hashVec,
                                          std::map<IdentifierHash,std::vector<float> >& data_map, bool phase) {
  
  if ( stripCharge == 0.0 ) return;
  
  // fill the CSC specific charge map the conversion hash -. id is done later !
  if ( data_map.find(hashId) != data_map.end() ) {

    float phaseOffset = (phase) ? +25 : 0; 
    std::vector<float> samples = m_pcalib->getSamplesFromBipolarFunc(driftTime+phaseOffset, stripCharge);
    std::vector<float> existingSamples = data_map[hashId]; // assuming that there are four elements...

    if (m_debug) {
      std::cout << "[CSC_Digitizer::fillSampleMaps] ";
      
      for (unsigned int i=0; i< samples.size() ; ++i) 
        std::cout <<  samples[i] << " ";
      std::cout << " + ";
      
      for (unsigned int i=0; i< existingSamples.size() ; ++i) 
        std::cout << existingSamples[i] << " ";
      
      std::cout << "  ==>  ";
    }

    for (unsigned int i=0; i< samples.size(); ++i ) {
      existingSamples[i] += samples[i];
    }

    if (m_debug) {
      for (unsigned int i=0; i< existingSamples.size() ; ++i) 
        std::cout << existingSamples[i] << " ";
      std::cout << std::endl;
    }

    data_map[hashId] = existingSamples;
    
  }
  else {
    std::vector<float> samples = m_pcalib->getSamplesFromBipolarFunc(driftTime, stripCharge);
    if (data_map[hashId].empty()) {
      for (unsigned int i=0; i< samples.size(); ++i ) {
        data_map[hashId].push_back(samples[i]);
      }
    }
    hashVec.push_back(hashId);
  }
}


inline void CSC_Digitizer::setWindow (const double t1, const double t2) {
  m_timeWindowLowerOffset = t1;
  m_timeWindowUpperOffset = t2;
  //    std::cout << "CSC_Digitizer::timeWindowOffset " << t1 << " " << t2 << std::endl;
}

inline void CSC_Digitizer::set (const double bunchTime) {m_bunchTime = bunchTime;}

// Private methods inlined

inline double CSC_Digitizer::qWire (const int & nElectrons, const double& gammaDist) const {
  
  // find the charge on the wire
  
  //  double amplification = 0.58e5;
  double amplification = m_amplification;
  double wireCharge = 0.0;  
  
  for (int i=0; i<nElectrons; i++) {
    double RNDpol = 0.0;
    if (m_Polia > -1.0) {
      RNDpol = gammaDist/(1.0+m_Polia);
    } 
    wireCharge += amplification*RNDpol;
  }
  return wireCharge;
}

// old implementation before Jan 2012:
inline double CSC_Digitizer::qStripR (const double x) const {
  
  // induced charge on the r-strip
  // parametrization based on experimental data
  
  double rStripCharge = 0.0;
  double Par[4] = {0.185, 2.315, 0.48, 5.2};  // obtained from test beam
  double xx = fabs(x)/10.0; // divide by 10 to convert to cm!
  
  rStripCharge = Par[0]*exp( -Par[1]*xx )+ Par[2]*exp( -Par[3]*(xx*xx) );
  
  return rStripCharge;
}


/** Charge sharing profile.
New implementation Jan 2012 from fit to segment hits in run 186049.
@param x distance between strip and hit center in strip widths
@param stationType small or large chamber
@return the charge fraction for this strip
*/
inline double CSC_Digitizer::qStripR (const double x, const std::string stationType) const {
  
  double stripCharge = 0.0;
  double xx = x * x; // distance in strip widths squared
  
  if (stationType == "CSS") { // small chamber
    stripCharge = 0.7001/(1.0 + 1.38*xx + 2.255*xx*xx);
  } else if (stationType == "CSL") { // large chamber
    stripCharge = 0.7/(1.0 + 1.381*xx + 2.248*xx*xx);
  } else {
    std::cout << "ERROR CSC_Digitizer :: not a valid CSC chamber : stationType = " 
	      << stationType << std::endl; 
    stripCharge = 0.0;
  }  
  return stripCharge;
}


inline double CSC_Digitizer::qStripPhi (const double x, const std::string stationType) const {
  
  double phiStripCharge = 0.0;
  const int N = 9;
  
  // parameters obtained from test beam
  double pSmall[N] = {.9092, .0634, -1.051, 8.05, -35.477, 58.883, -46.111, 17.446, -2.5824};
  double pLarge[N] = {.98, -.36, 5.07, -27.81, 72.257, -99.456, 72.778, -26.779, 3.9054};
  double xx = fabs(x)/10.0; // divide by 10 to convert to cm!
  
  if (xx < 1.75) {
    if (stationType == "CSS") {
      phiStripCharge = fparamPhi(xx,N,pSmall);
    } else if (stationType == "CSL") {
      phiStripCharge = fparamPhi(xx,N,pLarge);
    } else {
      std::cout << "ERROR CSC_Digitizer :: not a valid CSC chamber : stationType = " 
                << stationType << std::endl; 
      phiStripCharge = 0.0;
    }
  }
  
  return phiStripCharge;
}

inline double CSC_Digitizer::fparamPhi (const double x, const int N, const double * p) const {
  
  double fparam = 0;
  for (int j=0; j<N; j++) {
    double pow = 1.0;
    if ( j > 0) {
      for (int i=0; i<j; i++)
        pow *= x;
    }
    fparam += ( *(p+j) ) * pow;
  }
  return fparam;
}   

inline 
bool CSC_Digitizer::outsideWindow(double driftTime) const {
  double time = driftTime; //m_bunchTime is included already....updated one..
  //  double time = driftTime+m_bunchTime;
  return time < m_timeWindowLowerOffset || time > m_timeWindowUpperOffset;
}

inline double CSC_Digitizer::getDriftTime(const MuonGM::CscReadoutElement* descriptor, const Amg::Vector3D& pos) const {
  // need to be calculated correct - Garfield?
  // assumption on the field lines not good but of for pileup stuff!
  double time = -1000.0;
  // tested with 60, 45, 30 and 60 looks the best 12/7/2009
  double v0   = m_driftVelocity / (1e-6 * 1e9); // mm/microsecond   6 cm/microsecond -> mm/ns
  //  double v0   = 60 / (1e-6 * 1e9); // 6 cm/microsecond -> mm/ns
  //  //  double v0   = 45 / (1e-6 * 1e9); // 6 cm/microsecond -> mm/ns
  float anodeCathodeDist = descriptor->anodeCathodeDistance();
  float roxacellWidth = descriptor->roxacellWidth();
  float beta = atan ( (descriptor->longWidth()-descriptor->shortWidth())
                      /(2.*descriptor->lengthUpToMaxWidth()) );
  float longWidth =  descriptor->longWidth() - 2*roxacellWidth * (1+sin(beta))/cos(beta);
  float yy = pos.y()+longWidth/2; 
  float x0 = pos.x();
  int wire = int (yy/anodeCathodeDist)+1;
  float y0 = (yy-wire*anodeCathodeDist);
  if (std::abs(y0) > anodeCathodeDist/2) y0 = std::abs(y0) - anodeCathodeDist/2;
  float driftDist = sqrt(x0*x0+y0*y0); // Lorentz effect small
  time = driftDist / v0;
  //  std::cout << "driftDist= " << driftDist << " " << time << " " << v0 << " " << x0 << " " << pos.y() << " " << yy << " " << wire << " " << y0 << " "
  //            <<  anodeCathodeDist << " " << roxacellWidth << " " << beta << " " << longWidth << " "
  //            << std::endl;
  return time;
}


// public methods
// old way to add electron charges...
// * represent electron charge with bipolar function
// * add two charges with bipolar functions
// * then extract charge and time out of function...
//
// Then, try to speed up (09/2010)
// but it turns out to be vulernable for pileup simulation...
inline void CSC_Digitizer::fillMaps(const IdentifierHash hashId, 
				    const double driftTime,
				    const double stripCharge,
				    std::vector<IdentifierHash>& hashVec,
				    std::map<IdentifierHash,std::pair<double,double> >& data_map) {
  
  if ( stripCharge == 0.0 ) return;
  
  // fill the CSC specific charge map the conversion hash -. id is done later !
  if ( data_map.find(hashId) != data_map.end() ) {
    
    /*
    double t1 = data_map[hashId].first;
    double t2 = driftTime;
    
    double q1 = data_map[hashId].second;
    double q2 = stripCharge;
    
    double tsum = (t1*q1+t2*q2)/(fabs(q1)+fabs(q2));  // charge weight sum....
    double qsum = 0.995*(q1+q2); // 0.995 is considered because direct sum of two charges is slightly more than bipolar addition.
    // http://indico.cern.ch/getFile.py/access?contribId=9&resId=0&materialId=slides&confId=106902
    
    data_map[hashId].first  =tsum;
    data_map[hashId].second =qsum;
    */
    
    std::pair<double,double> result = m_pcalib->addBipfunc(data_map[hashId].first,
                                                           data_map[hashId].second,
                                                           driftTime,
                                                           stripCharge);
    //  To check out conversion is correct...
    if (m_debug)
      std::cout << "[CSC_Digitizer::fillMaps] (" << data_map[hashId].first << ":" << int(data_map[hashId].second) << ")"
                << "+(" << driftTime << ":" << int(stripCharge) << ")"
                << " ==> " << result.first << ":" << int(result.second)
                << " e-  which was " << int(data_map[hashId].second+stripCharge)
                << std::endl;
      
    data_map[hashId].first =result.first;
    data_map[hashId].second =result.second;
    
      
    
  }
  else {
    data_map[hashId].first = driftTime;
    data_map[hashId].second = stripCharge;
    hashVec.push_back(hashId);
  }
}
  


#endif 
