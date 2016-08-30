/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONRIOONTRACK_MUONDRIFTCIRCLEERRORSTRATEGY_H
#define MUON_MUONRIOONTRACK_MUONDRIFTCIRCLEERRORSTRATEGY_H

#include <bitset>
#include "GaudiKernel/MsgStream.h"
#include <iostream>

namespace Muon {
typedef std::bitset<23> MuonDriftCircleErrorStrategyInput;

class MuonDriftCircleErrorStrategy {
public:
  enum Strategy {Moore=0, Muonboy=1, Muon=2, UnknownStrategy=0x7}; // FIXME - better names
  enum CreationParameter{ BroadError=3,// Bit 3. Create with broad errors. 
                          ScaledError, // Error scaling is turned on (this is applied after eveyrthing else)
                          FixedError, //!< A fixed error is given to this hit (user defined via jobProperties)
                          ParameterisedErrors, //!< Use parameterised errors
                          StationError,//!< A term is added to account for misaligned
                          ErrorAtPredictedPosition,
                          T0Refit, //!< A special error was applied to account for the T0 refit (user defined via jobProperties)
                          WireSagGeomCorrection, //!< Wire sag was applied, and so will affect errors
                          TofCorrection, //!< Time of flight correction was applied in calibration
                          PropCorrection, //!< Propagation correction was applied in calibration
                          TempCorrection, //!< Temperature correction was applied in calibration
                          MagFieldCorrection, //!< Magnetic field correction was applied in calibration
                          WireSagTimeCorrection, //!< Wire sag correction was applied in calibration
                          SlewCorrection, //!< Slewing correction was applied in calibration
                          BackgroundCorrection, //!< Background correction was applied in calibration
                          Segment, //!< Treating a segment or a track
                          FirstCalibWindowBit //!< First bit recording the calibration configuration
                        };
  static const std::size_t nCalibWindowBits = 4; //!< Allows up to 16 configurations to be recorded

  MuonDriftCircleErrorStrategy(MuonDriftCircleErrorStrategyInput bits):m_bits(bits){} //!< first 3 bits are Strategy, then come CreationParameter, last four are calib window bits
  void setStrategy(Strategy); //!< Select the strategy to be used - only one can be set at a time
  void setParameter(CreationParameter, bool value);
  void setCalibWindow(unsigned long); //!< Only the first nCalibWindowBits bits will be stored!
  
  bool creationParameter(CreationParameter) const;
  unsigned long calibWindow() const; //!< Returns calibration configuration
  Strategy strategy() const;

  const MuonDriftCircleErrorStrategyInput getBits() const { return m_bits; }
  
private:
  MuonDriftCircleErrorStrategyInput m_bits; //!< 0,1,2 are reserved for Strategy, 3-16 are Creation parameters, 17-20 are calib window bits
};
}

inline
void
Muon::MuonDriftCircleErrorStrategy::setStrategy(Muon::MuonDriftCircleErrorStrategy::Strategy st) {
  unsigned int holder = static_cast<unsigned int>(st);
  m_bits[0] = holder&0x1;
  m_bits[1] = holder&0x2;
  m_bits[2] = holder&0x4;
}

inline
void
Muon::MuonDriftCircleErrorStrategy::setParameter(Muon::MuonDriftCircleErrorStrategy::CreationParameter cp, bool value) {
  m_bits[static_cast<unsigned int>(cp)] = value;
}

inline
void
Muon::MuonDriftCircleErrorStrategy::setCalibWindow(unsigned long config) {
  std::bitset<nCalibWindowBits> tmpconfig(config);

  // Copy temporary bitset into m_bits
  // Should do this with std::copy
  for (std::size_t itmp=0,i=FirstCalibWindowBit; itmp<nCalibWindowBits; ++itmp,++i) {
    m_bits[i] = tmpconfig[itmp];
  }

}

inline 
bool
Muon::MuonDriftCircleErrorStrategy::creationParameter(Muon::MuonDriftCircleErrorStrategy::CreationParameter cp) const {
  return m_bits[static_cast<unsigned int>(cp)];
}

inline
unsigned long
Muon::MuonDriftCircleErrorStrategy::calibWindow() const {
  std::bitset<nCalibWindowBits> tmpconfig;

  // Copy relevant part of m_bits into temporary bitset
  // Should do this with std::copy
  for (std::size_t itmp=0,i=FirstCalibWindowBit; itmp<nCalibWindowBits; ++itmp,++i) {
    tmpconfig[itmp] = m_bits[i];
  }

  return tmpconfig.to_ulong();
}

inline 
Muon::MuonDriftCircleErrorStrategy::Strategy
Muon::MuonDriftCircleErrorStrategy::strategy() const {
  switch (m_bits.to_ulong()&0x7){
  case 0:
    return Moore;
  case 1:
    return Muonboy;
  case 2:
    return Muon;
  default:
    return UnknownStrategy;
  }
  return UnknownStrategy;
}

inline std::ostream& operator << ( std::ostream& sl, const Muon::MuonDriftCircleErrorStrategy& mdces)
{ 
  sl << "MuonDriftCircleErrorStrategy with strategy ";
  const Muon::MuonDriftCircleErrorStrategyInput myBits = mdces.getBits();
  switch (mdces.strategy()){
    case 0:
      sl << "Moore";
      break;
    case 1:
      sl << "Muonboy";
      break;
    case 2:
      sl << "Muon";
      break;
    default:
      sl << "Unknown Strategy";
  }
  sl << " and configuration bits " << myBits;
  sl << " including CalibWindow " << mdces.calibWindow() << std::endl;
  return sl;
}

inline MsgStream& operator << ( MsgStream& sl, const Muon::MuonDriftCircleErrorStrategy& mdces)
{ 
  sl << "MuonDriftCircleErrorStrategy with strategy ";
  const Muon::MuonDriftCircleErrorStrategyInput myBits = mdces.getBits();
  switch (mdces.strategy()){
    case 0:
      sl << "Moore";
      break;
    case 1:
      sl << "Muonboy";
      break;
    case 2:
      sl << "Muon";
      break;
    default:
      sl << "Unknown Strategy";
  }
  sl << " and configuration bits " << myBits; 
  sl << " including CalibWindow " << mdces.calibWindow() << std::endl;
  return sl;
}


#endif
