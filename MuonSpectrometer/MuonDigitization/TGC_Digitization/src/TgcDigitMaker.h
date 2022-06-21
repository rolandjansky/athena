/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
   @class TgcDigitMaker
   @section Class, Methods and Properties

   All functionality of TGC digitization is implemented to this
   class.
*/

#ifndef TGCDIGITMAKER_H
#define TGCDIGITMAKER_H

#include <vector>
#include <string>

#include "AthenaBaseComps/AthMessaging.h"
#include "GaudiKernel/StatusCode.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "MuonCondData/TgcDigitASDposData.h"
#include "MuonCondData/TgcDigitTimeOffsetData.h"
#include "MuonCondData/TgcDigitCrosstalkData.h"

namespace CLHEP {
  class HepRandomEngine;
}

namespace MuonGM {
  class MuonDetectorManager;
}

class TgcDigitCollection;
class TgcHitIdHelper;
class TgcIdHelper;
class TGCSimHit; 

//--- class description
class TgcDigitMaker : public AthMessaging {
 public:
  TgcDigitMaker(const TgcHitIdHelper* hitIdHelper,
		const MuonGM::MuonDetectorManager * mdManager,
		unsigned int                        runperiod);

  virtual ~TgcDigitMaker();

  /**
     Initializes TgcHitIdHelper, TgcIdHelper and random number
     of a stream for the digitization. In this method,
     readFileOfTimeJitter() reads and sets the parameters for
     intrinsic time response of TGC from timejitter.dat.
  */
  StatusCode initialize();

  /**
     A single hit can be digitized in the two directions
     independently:
     R and phi directions. The information is restored by
     MuonGeoModel and accessible from the class of
     TGCReadOutElement. As described the preceding section, R
     direction is digitized based on the wire ganging 
     information in the database. Digits in phi direction are
     calculated based on the formula in which the structure of
     strips in TGC is well expressed for amdb_simrec.P.03. For
     amdb_simrec.Q or later, phi direction is digitized based
     on the parameter in the database as the same manner of
     those in R direction. The method determines the response
     time for digits which is commonly used for signals from
     wire gangs and strips, and signal propagation time along
     wires and strips. In case that response time is outside of
     the time window to be set, that hit is removed. This method
     also removes some hits based on the detection efficiency to
     be set.
  */
  TgcDigitCollection* executeDigi(const TGCSimHit* hit,
                                  const double globalHitTime,
                                  const TgcDigitASDposData* ASDpos,
                                  const TgcDigitTimeOffsetData* TOffset,
                                  const TgcDigitCrosstalkData* Crosstalk,
                                  CLHEP::HepRandomEngine* rndmEngine);

  //====== for private
 private:
  enum NumberOfDimensions {
    N_STATIONNAME = 8,
    OFFSET_STATIONNAME = 41, 
    N_STATIONETA = 11,
    OFFSET_STATIONETA = -5,
    N_STATIONPHI = 48,
    OFFSET_STATIONPHI = 1,
    N_GASGAP = 3,
    OFFSET_GASGAP = 1,
    N_ISSTRIP = 2,
    OFFSET_ISSTRIP = 0,
    N_ABSSTATIONETA = 5,
    OFFSET_ABSSTATIONETA = 1,
    N_STRIPCHANNEL = 32,
    OFFSET_STRIPCHANNEL = 1
  };
  enum TgcStation {
    kOUTER = 0,
    kINNER,
    N_STATION
  };
  enum TgcSensor {
    kWIRE = 0,
    kSTRIP,
    N_SENSOR
  };

  /**
     Reads parameters for intrinsic time response from timejitter.dat.
  */
  StatusCode readFileOfTimeJitter();
  /**
     Calculates intrinsic time response according to incident angle of
     a track based on time response parameters
  */
  float timeJitter(const Amg::Vector3D&, CLHEP::HepRandomEngine* rndmEngine) const;

  /**
     Determines whether a hit is detected or not.
  */
  bool efficiencyCheck(const TgcSensor sensor, CLHEP::HepRandomEngine* rndmEngine) const;
  bool efficiencyCheck(const std::string& stationName, const int stationEta, const int stationPhi, const int gasGap, const TgcSensor sensor, const double energyDeposit) const;

  uint16_t bcTagging(const double digittime, const double window, const double offset) const;
  void addDigit(const Identifier id, const uint16_t bctag, TgcDigitCollection* digits) const;

  /** Read share/TGC_Digitization_energyThreshold.dat file */
  StatusCode readFileOfEnergyThreshold();
  /** Read share/TGC_Digitization_deadChamber.dat file */
  StatusCode readFileOfDeadChamber();
  /** Read share/TGC_Digitization_StripPosition.dat file */
  StatusCode readFileOfStripPosition();

  /** Get energy threshold value for each chamber */
  double getEnergyThreshold(const std::string& stationName, int stationEta, int stationPhi, int gasGap, const TgcSensor sensor) const;
  void randomCrossTalk(const TgcDigitCrosstalkData* crosstalk,
                       const Identifier elemId, const int gasGap, const TgcSensor sensor,
                       const int channel, const float posInStrip, const float digitTime,
                       const float time_offset, CLHEP::HepRandomEngine* rndmEngine, TgcDigitCollection* digits) const;
  /** Method to check a chamber is dead or active */
  bool isDeadChamber(const std::string& stationName, int stationEta, int stationPhi, int gasGap);
  /** Get stationName integer from stationName string */
  int getIStationName(const std::string& staionName) const; 
  /** Method to get position of Strip channel */
  float getStripPosition(const std::string& stationName, int stationEta, int channel) const;
  /** Method to get signal propagation time delay */
  double getSigPropTimeDelay(const float cableDistance) const;
  /** Method to get time difference by cable radius of inner */
  float timeDiffByCableRadiusOfInner(const int iStationName, const int stationPhi, const int channel) const;
  /** Method to get propagation time to the ASD from the sensor */
  float getDistanceToAsdFromSensor(const TgcDigitASDposData* readCdo, const int iStationName, const int stationEta, const int stationPhi, const TgcSensor sensor, const int channel, const float position) const;
  /** Method to get time offset to absorb signal delay */
  float getTimeOffset(const TgcDigitTimeOffsetData* readCdo, const uint16_t station_num, const int station_eta, const TgcSensor sensor) const;
  /** Method to get the channel crosstalk probability */
  float getCrosstalkProbability(const TgcDigitCrosstalkData* readCdo, const uint16_t layer_id, const TgcSensor sensor, const unsigned int index_prob) const;

 private:
  /** Energy threshold value for each chamber */
  double m_energyThreshold[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_GASGAP][N_ISSTRIP]{};
  /** Dead chamber flag for each chamber */
  bool m_isDeadChamber[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_GASGAP]{};

  /** Position of Strip Channel (Longer base or Shorter base) */
  float m_StripPos[N_STATIONNAME][N_ABSSTATIONETA][N_STRIPCHANNEL]{};

  std::vector<std::vector<float> > m_vecAngle_Time;

  const TgcHitIdHelper* m_hitIdHelper;
  unsigned int m_runperiod;
  const MuonGM::MuonDetectorManager* m_mdManager; // cannot use ReadCondHandleKey since no athena component
  const TgcIdHelper* m_idHelper;
  float m_efficiency[N_SENSOR]{};

  /**
     define the time windows for signals from
     wiregangs and strips. The offsets are defined as relative time
     differences with respect to the time after TOF and cable
     length corrections. Bunch crossing time is specified.
  */
  double m_gateTimeWindow[N_STATION][N_SENSOR]{};
  double m_bunchCrossingTime;
};

#endif
