/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*author Renato Febbraro*/
/*date 3/7/2007*/
/*modified 27/5/2008 transfer PLC methods in a new class*/
/*renato.febbraro@cern.ch*/

#ifndef TILELASERCONTROL_H
#define TILELASERCONTROL_H

#include <time.h>

class TileLaserControl {

  public:
    
    TileLaserControl() 
      : m_pumpDiodeTemp(0.0)
      , m_timeLastMeasP(0)
      , m_diodeBoxTemp(0.0)
      , m_timeLastMeasD(0)
      , m_gasFlux(0.0)
      , m_timeLastMeasF(0)
      , m_humidity(0.0)
      , m_timeLastMeasH(0)
      , m_lastPedMeas(0)
      , m_lastAlphaMeas(0) { }

    ~TileLaserControl() { }
      
    double getPumpDiodeTemp() const  { return m_pumpDiodeTemp; }
    int getTimeLastMeasP() const     { return m_timeLastMeasP; }
    double getDiodeBoxTemp() const   { return m_diodeBoxTemp;  } 
    int getTimeLastMeasD() const     { return m_timeLastMeasD; }
    double getGasFlux() const        { return m_gasFlux;       }      
    int getTimeLastMeasF() const     { return m_timeLastMeasF; }
    double getHumidity() const       { return m_humidity;      }     
    int getTimeLastMeasH() const     { return m_timeLastMeasH; }
    time_t getLastPedMeas() const    { return m_lastPedMeas;   }  
    time_t getLastAlphaMeas() const  { return m_lastAlphaMeas; }

    void setControl(const double pumpDiodeTemp, 
                    const int timeLastMeasP, 
                    const double diodeBoxTemp, 
                    const int timeLastMeasD,
                    const double gasFlux, 
                    const int timeLastMeasF,
                    const double humidity, 
                    const int timeLastMeasH,
                    const time_t lastPedMeas,
                    const time_t lastAlphaMeas ) {
        m_pumpDiodeTemp =  pumpDiodeTemp;
        m_timeLastMeasP = timeLastMeasP;
        m_diodeBoxTemp = diodeBoxTemp;
        m_timeLastMeasD = timeLastMeasD;
        m_gasFlux = gasFlux;
        m_timeLastMeasF = timeLastMeasF;
        m_humidity = humidity;
        m_timeLastMeasH = timeLastMeasH;
        m_lastPedMeas = lastPedMeas;
        m_lastAlphaMeas = lastAlphaMeas;
    }
      
  private:
    
    double m_pumpDiodeTemp;
    int m_timeLastMeasP;
    double m_diodeBoxTemp;
    int m_timeLastMeasD;
    double m_gasFlux;
    int m_timeLastMeasF;
    double m_humidity;
    int m_timeLastMeasH;
    time_t m_lastPedMeas;
    time_t m_lastAlphaMeas;
    
  };

#endif
