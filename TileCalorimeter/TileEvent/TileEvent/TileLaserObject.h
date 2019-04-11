/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*author Renato Febbraro*/
/*date 3/7/2007*/
/*modified 27/5/2008 changes in PLC methods ==> adding the methods for the new class TilaLaser PLC*/
/*renato.febbraro@cern.ch*/

/*modified 23/08/2014 to accommodate LASERII system*/
/*Marco van Woerden <mvanwoer@cern.ch>*/

#ifndef TILELASEROBJECT_H
#define TILELASEROBJECT_H

#include <vector>
#include <time.h>

#include "TileEvent/TileLaserControl.h"
#include "TileEvent/TileLaser.h"
#include "TileEvent/TileLaserDiode.h"
#include "TileEvent/TileLaserPmt.h"
#include "TileEvent/TileLaserPLC.h"
#include "TileEvent/TileLasCalib.h"

#include "CLIDSvc/CLASS_DEF.h"


/*static const CLID CLID_TileLaser = 2940;*/

class TileLaserObject{
  
public:
  
  TileLaserObject();
  
  /*   static const CLID& classID() {return CLID_TileLaser;} */
  /*   virtual const CLID& classID() {return CLID_TileLaser;} */
  ~TileLaserObject() {}
  
  /** @brief max size of diode and pmt vectors  */
  enum MAX_SIZE {
    nbGains=2,     // LG/HG
    nbPmts=2,      // LASERI/LASERII MONITORING PMTS
    nbDiodes=14,   // LASERII MONITORING DIODES
    nbTypes=4,     // LASERII CALIBRATION TYPES
    firstType=0    // LASERII VALUE OF FIRST CALIBRATION TYPE
  };
  
  int getVersion() const;
  bool isLASERII() const;
  
  int getCounter() const;
  double getDiodeCurrOrd() const;
  double getDiodeCurrMeas() const;
  int getFiltNumber() const;
  
  double getPumpDiodeTemp() const;
  int getTimeLastMeasP() const;
  double getDiodeBoxTemp() const;
  int getTimeLastMeasD() const;
  double getGasFlux() const;
  int getTimeLastMeasF() const;
  double getHumidity() const;
  int getTimeLastMeasH() const;
  time_t getLastPedMeas() const;
  time_t getLastAlphaMeas() const;
  
  double getMean(int chan, int gain, int type) const;
  double getSigma(int chan, int gain, int type) const;
  int getN(int chan, int gain, int type) const;
  int getType(int chan, int gain, int type) const;
  bool isSet(int chan, int gain, int type) const;
  void setCalibType(int type);
  int  getCalibType() const;
  void setCalib(int chan, int type, double sumXinQDC, double sumX2inQDC, int nevts, int gain);
  int getDaqType() const;
  
  int getDiodeADC( const unsigned int i, const unsigned int gain=0 ) const;
  double getDiodePedestal(const unsigned int i, const unsigned int gain=0 ) const;
  double getDiodeSigmaPedestal(const unsigned int i, const unsigned int gain=0 ) const;
  double getAlpha(const unsigned int i, const unsigned int gain=0 ) const;
  double getSigmaAlpha(const unsigned int i, const unsigned int gain=0 ) const;
  double getPedestalAlpha(const unsigned int i, const unsigned int gain=0 ) const;
  double getSigmaPedAlpha(const unsigned int i, const unsigned int gain=0 ) const;
  
  int getPMADC(const unsigned int j, const unsigned int gain=0 ) const;
  int getTDC(const unsigned int j, const unsigned int gain=0 ) const;
  double getPMPedestal(const unsigned int j, const unsigned int gain=0 ) const;
  double getPMSigmaPedestal(const unsigned int j, const unsigned int gain=0 ) const;
  
  int getBCID() const;
  void setBCID(const int BCID);
  
  int getAlphaPos() const;
  double getLVdiodes() const;
  double getHVpmts() const;
  int getShutter() const;
  int getInterlock() const;
  int getAlarm() const;
  bool getQDCTimeout() const;
  bool getTDCTimeout() const;
  
  void setLaser(const int Counter,
                const int diodeCurrOrd,
                const int diodeCurrMeas,
                const int filtNumber,
                const int timingDelay,
                const int version);
  
  void setControl(const double pumpDiodeTemp,
                  const int timeLastMeasP,
                  const double diodeBoxTemp,
                  const int timeLastMeasD,
                  const double gasFlux,
                  const int timeLastMeasG,
                  const double humidity,
                  const int timeLastMeasH,
                  const time_t lastPedMeas,
                  const time_t lastAlphaMeas);
  
  void setDiode(const unsigned int diode,
                const int diodeAdc,
                const double diodePedestal,
                const double diodeSigmaPedestal,
                const double alpha,
                const double sigmaAlpha,
                const double pedestalAlpha,
                const double sigmaPedAlpha,
                const unsigned int gain);
  
  void setPmt(const unsigned int pmt,
              const int pmAdc,
              const int tdc,
              const double pmPedestal,
              const double pmSigmaPedestal,
              const unsigned int gain);
  
  void setPLC(const int alphaPos,
              const double LVdiodes,
              const double HVpmts,
              const int shutter,
              const int interlock,
              const int alarm);
  
  void setDaqType(const unsigned int daqtype);
  
  void setTimeouts(const bool qdc, const bool tdc);

  enum daqType {daqPedestal=0x10, daqAlpha, daqLed, daqLinearity, daqLaser}; 

private:
  
  TileLaserControl m_slowCtrl;
  TileLaser m_laserParameter;
  std::vector<TileLaserDiode> m_diodesLG;
  std::vector<TileLaserDiode> m_diodesHG;
  std::vector<TileLaserPmt> m_pmtsLG;
  std::vector<TileLaserPmt> m_pmtsHG;
  std::vector<std::vector<TileLasCalib> > m_lascalib;
  TileLaserPLC m_plc;
  bool m_qdctimeout; 
  bool m_tdctimeout;
  
  int m_daqtype;
  // DAQ TYPES ARE RELEVANT FOR LASERII
  // DAQ TYPE   HEX   DEC
  // Pedestal   0x10  16
  // Alpha      0x11  17
  // Led        0x12  18
  // Linearity  0x13  19
  // Laser      0x14  20
  int m_BCID;
  int m_version;

  int m_calibtype;
  // Calibe types are relevant for laserII
  // Pedestal 0
  // Pedestal 1
  // Led      2
  // Alpha

 public:  
  enum calibType {Pedestal0, Pedestal1, LED, Alpha}; 

};

CLASS_DEF(TileLaserObject, 2940, 0)

inline int TileLaserObject::getCounter() const
{
  return m_laserParameter.getCounter();
}

inline int TileLaserObject::getVersion() const
{
  return m_version;
}

inline bool TileLaserObject::isLASERII() const
{
  return (m_version==2);
}

inline double TileLaserObject::getDiodeCurrOrd() const
{
  return m_laserParameter.getDiodeCurrOrd();
}

inline double TileLaserObject::getDiodeCurrMeas() const
{
  return m_laserParameter.getDiodeCurrMeas();
}

inline int TileLaserObject::getFiltNumber() const
{
  return m_laserParameter.getFiltNumber();
}

inline double TileLaserObject::getPumpDiodeTemp() const
{
  return m_slowCtrl.getPumpDiodeTemp();
}

inline int TileLaserObject::getTimeLastMeasP() const
{
  return m_slowCtrl.getTimeLastMeasP();
}

inline double TileLaserObject::getDiodeBoxTemp() const
{
  return m_slowCtrl.getDiodeBoxTemp();
}

inline int TileLaserObject::getTimeLastMeasD() const
{
  return m_slowCtrl.getTimeLastMeasD();
}

inline double TileLaserObject::getGasFlux() const
{
  return m_slowCtrl.getGasFlux();
}

inline int TileLaserObject::getTimeLastMeasF() const
{
  return m_slowCtrl.getTimeLastMeasF();
}

inline double TileLaserObject::getHumidity() const
{
  return m_slowCtrl.getHumidity();
}

inline int TileLaserObject::getTimeLastMeasH() const
{
  return m_slowCtrl.getTimeLastMeasH();
}

inline time_t TileLaserObject::getLastPedMeas() const
{
  return m_slowCtrl.getLastPedMeas();
}

inline time_t TileLaserObject::getLastAlphaMeas() const
{
  return m_slowCtrl.getLastAlphaMeas();
}

inline int TileLaserObject::getAlphaPos() const
{
  return m_plc.getAlphaPos();
}

inline double TileLaserObject::getLVdiodes() const
{
  return m_plc.getLVdiodes();
}

inline double TileLaserObject::getHVpmts() const
{
  return m_plc.getHVpmts();
}

inline int TileLaserObject::getShutter() const
{
  return m_plc.getShutter();
}

inline int TileLaserObject::getInterlock() const
{
  return m_plc.getInterlock();
}

inline int TileLaserObject::getAlarm() const
{
  return m_plc.getAlarm();
}

inline bool TileLaserObject::getQDCTimeout() const
{
  return m_qdctimeout;
}

inline bool TileLaserObject::getTDCTimeout() const
{
  return m_tdctimeout;
}


inline int TileLaserObject::getDaqType() const
{
  return m_daqtype;
}

inline void TileLaserObject::setBCID(const int BCID)
{
  m_BCID=BCID;
}

inline int TileLaserObject::getBCID() const {return m_BCID;}

#endif 
