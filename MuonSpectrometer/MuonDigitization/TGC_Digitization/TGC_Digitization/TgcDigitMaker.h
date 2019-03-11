/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// TgcDigitMaker.h
//--------------------------------------------------------
// Y. HASEGAWA
// 09 March 2004
//---------------------------------------------------------

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

#include "GaudiKernel/StatusCode.h"
#include "Identifier/Identifier.h"
#include "AthenaKernel/MsgStreamMember.h"

#include "GeoPrimitives/GeoPrimitives.h"

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
class TgcDigitMaker {
  //------ for public
 public:

  TgcDigitMaker(TgcHitIdHelper*                     hitIdHelper, 
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
     TGC digits contains Muon ID only. The bunch crossing ID
     will be included in future, too.
  */
  TgcDigitCollection* executeDigi(const TGCSimHit* hit,
				  const double globalHitTime,
                                  CLHEP::HepRandomEngine* rndmEngine);

  //Declaring the Message method for further use
  MsgStream& msg(const MSG::Level lvl) const ;
  //Declaring the Method providing Verbosity Level
  bool msgLevel(const MSG::Level lvl) const;
  void setMessageLevel(const MSG::Level lvl) const;

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
    N_CROSSTALK_PARAMETER = 4
  };

  /**
     Reads parameters for intrinsic time response from timejitter.dat.
  */
  void  readFileOfTimeJitter();
  /**
     Calculates intrinsic time response according to incident angle of
     a track based on time response parameters
  */
  float timeJitter(const Amg::Vector3D, CLHEP::HepRandomEngine* rndmEngine) const;

  /**
     Determines whether a hit is detected or not.
  */
  bool efficiencyCheck(const int isStrip, CLHEP::HepRandomEngine* rndmEngine) const;
  bool efficiencyCheck(const std::string stationName, const int stationEta, const int stationPhi, const int gasGap, const int isStrip, const double energyDeposit) const;

  uint16_t bcTagging(const double digittime, const int isStrip) const;
  void addDigit(const Identifier id, const uint16_t bctag);

  /** Read share/TGC_Digitization_energyThreshold.dat file */
  void readFileOfEnergyThreshold();
  /** Read share/TGC_Digitization_crossTalk.dat file */
  void readFileOfCrossTalk();
  /** Read share/TGC_Digitization_deadChamber.dat file */
  void readFileOfDeadChamber();
  /** Read share/TGC_Digitization_timeWindowOffset.dat file */
  void readFileOfTimeWindowOffset();
  /** Read share/TGC_Digitization_alignment.dat file */
  void readFileOfAlignment();
  /** Get energy threshold value for each chamber */
  double getEnergyThreshold(const std::string stationName, int stationEta, int stationPhi, int gasGap, int isStrip) const;
  void randomCrossTalk(const Identifier elemId, const int gasGap, const int isStrip, const int channel, 
		       const float posInStrip, const double digitTime, CLHEP::HepRandomEngine* rndmEngine);
  /** Method to check a chamber is dead or active */
  bool isDeadChamber(const std::string stationName, int stationEta, int stationPhi, int gasGap);
  /** Method to get time window offset */
  double getTimeWindowOffset(const std::string stationName, int stationEta, int isStrip) const; 
  /** Get stationName integer from stationName string */
  int getIStationName(const std::string staionName) const; 
  /** Ad hoc implementation of detector position shift */
  void adHocPositionShift(const std::string stationName, int stationEta, int stationPhi, 
			  const Amg::Vector3D direCos, Amg::Vector3D &localPos) const;

  /** Energy threshold value for each chamber */
  double m_energyThreshold[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_GASGAP][N_ISSTRIP];
  /** Cross talk probabilty for each chamber */
  double m_crossTalk[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_GASGAP][N_ISSTRIP][N_CROSSTALK_PARAMETER];
  /** Dead chamber flag for each chamber */
  bool m_isDeadChamber[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_GASGAP];
  /** Time window offset for each chamber */
  double m_timeWindowOffset[N_STATIONNAME][N_STATIONETA][N_ISSTRIP];

  /** Alignment z constants. Translation in the global r direction */ 
  double m_alignmentZ[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  /** Alignment t constants. Translation in the global z direction */ 
  double m_alignmentT[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  /** Alignment s constants. Translation in the global phi direction */ 
  double m_alignmentS[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  /** Alignment ths constants. Rotation around the global phi direction */ 
  double m_alignmentTHS[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];

  std::vector<std::vector<float> > m_vecAngle_Time;

  TgcDigitCollection* m_digits;
  TgcHitIdHelper* m_hitIdHelper;
  unsigned int m_runperiod;
  const MuonGM::MuonDetectorManager* m_mdManager;
  const TgcIdHelper* m_idHelper;
  float m_efficiencyOfWireGangs;
  float m_efficiencyOfStrips;

  /**
     define offsets and widths of time windows for signals from
     wiregangs and strips. The offsets are defined as relative time
     diffference with respect to the time after TOF and cable
     length corrections. Bunch crossing time is specified.
  */
  double m_timeWindowOffsetWire;
  double m_timeWindowOffsetStrip;
  double m_timeWindowWire;
  double m_timeWindowStrip;
  double m_bunchCrossingTime;

  //Declaring private message stream member.
  mutable Athena::MsgStreamMember m_msg;
};

#endif
