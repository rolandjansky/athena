/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "GaudiKernel/StatusCode.h"
#include "Identifier/Identifier.h"
#include "AthenaKernel/MsgStreamMember.h"

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
class sTGCSimHit; 

//--- class description
class sTgcDigitMaker {
  //------ for public
 public:

  sTgcDigitMaker(const sTgcHitIdHelper* hitIdHelper, const MuonGM::MuonDetectorManager * mdManager);

  virtual ~sTgcDigitMaker();

  /**
     Initializes sTgcHitIdHelper, sTgcIdHelper and random number
     of a stream for the digitization.  
  */
  StatusCode initialize(CLHEP::HepRandomEngine* m_rndmEngine, const int channelTypes);

  std::unique_ptr<sTgcDigitCollection> executeDigi(const sTGCSimHit* hit, const float globalHitTime);

  //Declaring the Message method for further use
  MsgStream& msg(const MSG::Level lvl) const { return m_msg << lvl ; }
  bool msgLvl(const MSG::Level lvl) const { return m_msg.get().level() <= lvl ; }
  void setMessageLevel(const MSG::Level lvl) const { m_msg.get().setLevel(lvl); return; }
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


  /**
     Reads parameters for intrinsic time response from timejitter.dat.
  */
  void  readFileOfTimeJitter();
  /**
     Calculates intrinsic time response according to incident angle of a track based on time response parameters
  */
  float timeJitter(float inAngle_time) const;

  /**
     Determines whether a hit is detected or not.
  */
  bool efficiencyCheck(const int channelType) const;
  bool efficiencyCheck(const std::string stationName, const int stationEta, const int stationPhi, const int multiPlet, const int gasGap, const int channelType, const double energyDeposit) const;

  //uint16_t bcTagging(const float digittime, const int channelType) const;
  void addDigit(sTgcDigitCollection* digits, const Identifier id, const uint16_t bctag, const float digittime, int channelType) const;
  void addDigit(sTgcDigitCollection* digits, const Identifier id, const uint16_t bctag, const float digittime, float charge, int channelType) const;

  /** Read share/sTGC_Digitization_energyThreshold.dat file */
  void readFileOfEnergyThreshold();
  ///** Read share/sTGC_Digitization_crossTalk.dat file */
  //void readFileOfCrossTalk();
  /** Read share/sTGC_Digitization_deadChamber.dat file */
  void readFileOfDeadChamber();
  /** Read share/sTGC_Digitization_timeWindowOffset.dat file */
  void readFileOfTimeWindowOffset();
  /** Read share/sTGC_Digitization_alignment.dat file */
  //void readFileOfAlignment();
  ///** Get energy threshold value for each chamber */
  double getEnergyThreshold(const std::string stationName, int stationEta, int stationPhi, int multiPlet, int gasGap, int channelType) const;
  //void randomCrossTalk(const Identifier elemId, const int gasGap, const int channelType, const int channel,
  //                     const float posInStrip, const double digitTime);
  /** Method to check a chamber is dead or active */
  bool isDeadChamber(const std::string stationName, int stationEta, int stationPhi, int multiPlet, int gasGap);
  double getTimeWindowOffset(const std::string stationName, int stationEta, int channelType) const;
  /** Get stationName integer from stationName string */
  int getIStationName(const std::string staionName) const;
  /** Ad hoc implementation of detector position shift */
  //void adHocPositionShift(const std::string stationName, int stationEta, int stationPhi,
  //                        const Amg::Vector3D direCos, Amg::Vector3D &localPos) const;

  /** Energy threshold value for each chamber */
  double m_energyThreshold[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_MULTIPLET][N_GASGAP][N_CHANNELTYPE];
  ///** Cross talk probabilty for each chamber */
  //double m_crossTalk[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_GASGAP][N_CHANNELTYPE][N_CROSSTALK_PARAMETER];
  /** Dead chamber flag for each chamber */
  bool m_isDeadChamber[N_STATIONNAME][N_STATIONETA][N_STATIONPHI][N_MULTIPLET][N_GASGAP];
  /** Time window offset for each chamber */
  double m_timeWindowOffset[N_STATIONNAME][N_STATIONETA][N_CHANNELTYPE];

  ///** Alignment z constants. Translation in the global r direction */
  //double m_alignmentZ[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  ///** Alignment t constants. Translation in the global z direction */
  //double m_alignmentT[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  ///** Alignment s constants. Translation in the global phi direction */
  //double m_alignmentS[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];
  ///** Alignment ths constants. Rotation around the global phi direction */
  //double m_alignmentTHS[N_STATIONNAME][N_STATIONETA][N_STATIONPHI];

  std::vector<std::vector<float> > m_vecAngle_Time;

  CLHEP::HepRandomEngine* m_engine{}; // not owned here
  const sTgcHitIdHelper* m_hitIdHelper{}; // not owned here
  const MuonGM::MuonDetectorManager* m_mdManager{}; // not owned here
  const sTgcIdHelper* m_idHelper{}; // not owned here
  float m_efficiencyOfWireGangs;
  float m_efficiencyOfStrips;
  float m_IntegralTimeOfElectr;
 
  /**
     define offsets and widths of time windows for signals from
     wiregangs and strips. The offsets are defined as relative time
     diffference with respect to the time after TOF and cable
     length corrections. Bunch crossing time is specified.
  */
  bool m_doTimeCorrection;
  double m_timeWindowOffsetPad;
  double m_timeWindowOffsetStrip;
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

 protected:
  //Declaring private message stream member.
  mutable Athena::MsgStreamMember m_msg = Athena::MsgStreamMember("sTgcDigitMaker");
  
};

#endif
