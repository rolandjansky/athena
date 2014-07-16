/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*author Renato Febbraro*/
/*date 3/7/2007*/
/*modified 27/5/2008 changes in PLC methods ==> adding the methods for the new class TilaLaser PLC*/
/*renato.febbraro@cern.ch*/

#ifndef TILELASEROBJECT_H
#define TILELASEROBJECT_H

#include <vector>
#include <time.h>

#include "TileEvent/TileLaserControl.h"
#include "TileEvent/TileLaser.h"
#include "TileEvent/TileLaserDiode.h"
#include "TileEvent/TileLaserPmt.h"
#include "TileEvent/TileLaserPLC.h"

#include "CLIDSvc/CLASS_DEF.h"


/*static const CLID CLID_TileLaser = 2940;*/

 class TileLaserObject{

 public:

  TileLaserObject();
  
/*   static const CLID& classID() {return CLID_TileLaser;} */
/*   virtual const CLID& classID() {return CLID_TileLaser;} */
  ~TileLaserObject() {}
  
  static const unsigned int nbDiodes;
  static const unsigned int nbPmts;

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
  
  int getDiodeADC( const unsigned int i) const;
  double getDiodePedestal(const unsigned int i) const;
  double getDiodeSigmaPedestal(const unsigned int i) const;
  double getAlpha(const unsigned int i) const;
  double getSigmaAlpha(const unsigned int i) const;
  double getPedestalAlpha(const unsigned int i) const;
  double getSigmaPedAlpha(const unsigned int i) const;

  int getPMADC(const unsigned int j) const; 
  int getTDC(const unsigned int j) const;
  double getPMPedestal(const unsigned int j) const;
  double getPMSigmaPedestal(const unsigned int j) const;

  int getBCID() const; 
  void setBCID(const int BCID);  

  int getAlphaPos() const;
  double getLVdiodes() const;
  double getHVpmts() const;
  int getShutter() const;
  int getInterlock() const;
  int getAlarm() const;

  void setLaser(const int Counter,
		const int diodeCurrOrd, 
		const int diodeCurrMeas, 
		const int filtNumber,
		const int SlamaDelay);

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
                const double sigmaPedAlpha);

  void setPmt(const unsigned int pmt,
	      const int pmAdc, 
	      const int tdc, 
	      const double pmPedestal, 
	      const double pmSigmaPedestal);
	      
  void setPLC(const int alphaPos,
	      const double LVdiodes,
	      const double HVpmts,
	      const int shutter,
	      const int interlock,
	      const int alarm);

 private:

  TileLaser::TileLaserControl m_slowCtrl;
  TileLaser::TileLaser m_laserParameter;
  std::vector<TileLaser::TileLaserDiode> m_diodes; 
  std::vector<TileLaser::TileLaserPmt> m_pmts;
  TileLaser::TileLaserPLC m_plc;

  int m_BCID;
};

CLASS_DEF(TileLaserObject, 2940, 0)

inline int TileLaserObject::getCounter() const
{
  return m_laserParameter.getCounter();
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

inline void TileLaserObject::setBCID(const int BCID)
{
  m_BCID=BCID;
}

inline int TileLaserObject::getBCID() const {return m_BCID;}

#endif 
