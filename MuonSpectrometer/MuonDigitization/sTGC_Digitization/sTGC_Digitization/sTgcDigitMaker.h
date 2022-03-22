/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class sTgcDigitMaker
   @section Class, Methods and Properties

   All functionality of sTGC digitization is implemented to this
   class.
*/

#ifndef STGCDIGITMAKER_H
#define STGCDIGITMAKER_H

#include <memory>
#include <vector>
#include <string>

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/StatusCode.h"
#include "Identifier/Identifier.h"
#include "MuonSimEvent/sTGCSimHit.h"

namespace CLHEP {
  class HepRandomEngine;
  class HepRandom;
}

namespace MuonGM {
  class MuonDetectorManager;
}

class sTgcDigitCollection;
class sTgcHitIdHelper;
class sTgcIdHelper;

//--- class description
class sTgcDigitMaker : public AthMessaging {
  //------ for public
 public:

  sTgcDigitMaker(const sTgcHitIdHelper* hitIdHelper, const MuonGM::MuonDetectorManager * mdManager, bool doEfficiencyCorrection);

  virtual ~sTgcDigitMaker();

  /**
     Initializes sTgcHitIdHelper and sTgcIdHelper, 
     and call the functions to read files containing digitization parameters.
  */
  StatusCode initialize(const int channelTypes);

  /**
     Digitize a given hit, determining the time and charge spread on wires, pads and strips.
  */
  std::unique_ptr<sTgcDigitCollection> executeDigi(const sTGCSimHit* hit, const float globalHitTime, CLHEP::HepRandomEngine* rndmEngine);

  //====== for private
 private:

  int m_channelTypes; // 1 -> strips, 2 -> strips+wires, 3 -> strips/wires/pads
  double m_theta, m_mean;  
  enum NumberOfDimensions {
    N_STATIONNAME = 2,
    OFFSET_STATIONNAME = 0,
    N_STATIONETA = 9,
    OFFSET_STATIONETA = -4,
    N_STATIONPHI = 8,
    OFFSET_STATIONPHI = 1,
    N_MULTIPLET = 2,
    OFFSET_MULTIPLET = 1, 
    N_GASGAP = 4,
    OFFSET_GASGAP = 1,
    N_CHANNELTYPE = 3,
    OFFSET_CHANNELTYPE = 0
  };

  /** Parameters of a gamma probability distribution function, required for 
   *  estimating wire digit's time of arrival.
   *  More detail in the dat file.
   */
  struct GammaParameter {
    double lowEdge; // low side of the interval in ns
    double kParameter;
    double thetaParameter;
  };

  /** Ionization object with distance, position on the hit segment and 
   *  position on the wire.
   */
  struct Ionization {
    double distance{-9.99}; //smallest distance bet the wire and particle trajectory
    Amg::Vector3D posOnSegment{0.,0.,0.}; // Point of closest approach 
    Amg::Vector3D posOnWire{0.,0.,0.}; // Position on the wire
  };

  /**
     Determines whether a hit is detected or not.
  */
  bool efficiencyCheck(const std::string& stationName, const int stationEta, const int stationPhi, const int multiPlet, const int gasGap, const int channelType, const double energyDeposit) const;

  //uint16_t bcTagging(const float digittime, const int channelType) const;
  void addDigit(sTgcDigitCollection* digits, const Identifier id, const uint16_t bctag, const float digittime, int channelType) const;
  void addDigit(sTgcDigitCollection* digits, const Identifier id, const uint16_t bctag, const float digittime, float charge, int channelType) const;

  /** Read share/sTGC_Digitization_energyThreshold.dat file */
  StatusCode readFileOfEnergyThreshold();
  ///** Read share/sTGC_Digitization_crossTalk.dat file */
  //StatusCode readFileOfCrossTalk();
  /** Read share/sTGC_Digitization_deadChamber.dat file */
  StatusCode readFileOfDeadChamber();
  /** Read share/sTGC_Digitization_EffChamber.dat file */
  StatusCode readFileOfEffChamber();
  /** Read share/sTGC_Digitization_timeWindowOffset.dat file */
  StatusCode readFileOfTimeWindowOffset();
  /** Read share/sTGC_Digitization_alignment.dat file */
  //StatusCode readFileOfAlignment();
  /** Read share/sTGC_Digitization_timeArrival.dat */
  StatusCode readFileOfTimeArrival();
  /** Read share/sTGC_Digitization_timeOffsetStrip.dat */
  StatusCode readFileOfTimeOffsetStrip();

  ///** Get energy threshold value for each chamber */
  double getEnergyThreshold(const std::string& stationName, int stationEta, int stationPhi, int multiPlet, int gasGap, int channelType) const;
  //void randomCrossTalk(const Identifier elemId, const int gasGap, const int channelType, const int channel,
  //                     const float posInStrip, const double digitTime);
  /** Method to check a chamber is dead or active */
  bool isDeadChamber(const std::string& stationName, int stationEta, int stationPhi, int multiPlet, int gasGap);
  float getChamberEfficiency(int stationName, int stationEta, int stationPhi, int multiPlet, int gasGap);
  double getTimeWindowOffset(const std::string& stationName, int stationEta, int channelType) const;
  /** Get stationName integer from stationName string */
  int getIStationName(const std::string& staionName) const;
  /** Ad hoc implementation of detector position shift */
  //void adHocPositionShift(const std::string stationName, int stationEta, int stationPhi,
  //                        const Amg::Vector3D direCos, Amg::Vector3D &localPos) const;

  /** Compute the distance between a track segment and a wire. 
   *  Expected distance is between zero and half of wire pitch (i.e. 0.9 mm),
   *  but can be greater if particle passes through the edge of a chamber.
   *  Assumig the hit is near wire k, the sign of the distance returned is:
   *   - negative if particle crosses the wire surface between wire k and wire k-1
   *   + positive if particle crosses the wire surface between wire k and wire k+1
   *  In case of error, the function returns -9.99.
   */
  double distanceToWire(Amg::Vector3D& position, Amg::Vector3D& direction, Identifier id, int wire_number) const;

  /** Determine the points where the distance between two segments is smallest.
   *  Given two segments, e.g. a particle trajectory and a sTGC wire, solve for the 
   *  two points, the point on the trajectory and the point on the wire, where the 
   *  distance between the two segments is the smallest.
   *
   *  Positions returned are in the local coordinate frame of the wire plane.
   *  Returns an object with distance of -9.99 in case of error.
   */
  Ionization pointClosestApproach(const Identifier& id, int wireNumber, Amg::Vector3D& preStepPos, 
                                  Amg::Vector3D& postStepPos) const;

  /** Get digit time offset of a strip depending on its relative position to 
   *  the strip at the centre of the cluster.
   *  It returns 0 ns by default, as well as when it fails or container is empty.
   */
  double getTimeOffsetStrip(int neighbor_index) const;

  /** Find the gamma pdf parameters of a given distance */
  GammaParameter getGammaParameter(double distance) const;
  /** Get the most probable time of arrival */
  double getMostProbableArrivalTime(double distance) const;

  /** Energy threshold value for each chamber */
  double m_energyThreshold[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_MULTIPLET][N_GASGAP][N_CHANNELTYPE]{};
  ///** Cross talk probabilty for each chamber */
  //double m_crossTalk[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_GASGAP][N_CHANNELTYPE][N_CROSSTALK_PARAMETER];
  /** Dead chamber flag for each chamber */
  bool m_isDeadChamber[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_MULTIPLET][N_GASGAP]{};
  float m_ChamberEfficiency[2][4][8][2][4]{};
  /** Time window offset for each chamber */
  double m_timeWindowOffset[N_STATIONNAME][N_STATIONETA][N_CHANNELTYPE]{};

  ///** Alignment z constants. Translation in the global r direction */
  //double m_alignmentZ[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  ///** Alignment t constants. Translation in the global z direction */
  //double m_alignmentT[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  ///** Alignment s constants. Translation in the global phi direction */
  //double m_alignmentS[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  ///** Alignment ths constants. Rotation around the global phi direction */
  //double m_alignmentTHS[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];

  // Parameters of the gamma pdf required for determining digit time
  std::vector<GammaParameter> m_gammaParameter;
  // 4th-order polymonial describing the most probable time as function of the distance of closest approach
  std::vector<double> m_mostProbableArrivalTime;

  // Time offset to add to Strip timing
  std::vector<double> m_timeOffsetStrip;

  const sTgcHitIdHelper* m_hitIdHelper{}; // not owned here
  const MuonGM::MuonDetectorManager* m_mdManager{}; // not owned here
  const sTgcIdHelper* m_idHelper{}; // not owned here
  float m_IntegralTimeOfElectr;
  bool m_doEfficiencyCorrection;
 
  /**
     define offsets and widths of time windows for signals from
     wiregangs and strips. The offsets are defined as relative time
     diffference with respect to the time after TOF and cable
     length corrections. Bunch crossing time is specified.
  */
  bool m_doTimeCorrection;
  // Flag to enable strip time offset 
  bool m_doTimeOffsetStrip;
  //double m_timeWindowWire;
  //double m_timeWindowStrip;
  //double m_bunchCrossingTime;
  //double m_timeJitterElectronicsStrip;
  //double m_timeJitterElectronicsPad;
  double m_GausMean;
  double m_GausSigma;
  double m_CrossTalk;
  double m_StripResolution;
  double m_ChargeSpreadFactor;

};

#endif
