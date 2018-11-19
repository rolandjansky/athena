/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/*author Renato Febbraro*/
/*date 3/7/2007*/
/*renato.febbraro@cern.ch*/


#ifndef TILELASER_H
#define TILELASER_H


#include <string>


class TileLaser {

 public:

  TileLaser() 
    : m_counter(0)
    , m_diodeCurrOrd(0)
    , m_diodeCurrMeas(0)
    , m_filtNumber(0)
    , m_slamaDelay(0) { }

  ~TileLaser() { }
    
  int getDiodeCurrOrd() const  { return m_diodeCurrOrd;  }
  int getDiodeCurrMeas() const { return m_diodeCurrMeas; }
  int getFiltNumber() const    { return m_filtNumber;    }
  int getCounter() const       { return m_counter;       }
  int getSlamaDelay() const    { return m_slamaDelay;    }

  void setLaser(const int Counter, 
		const int diodeCurrOrd, 
                const int diodeCurrMeas, 
                const int filtNumber,
		const int SlamaDelay) {
      m_counter = Counter;
      m_diodeCurrOrd = diodeCurrOrd;
      m_diodeCurrMeas = diodeCurrMeas;
      m_filtNumber = filtNumber;
      m_slamaDelay = SlamaDelay;
  }
 

  /** Convertion operator to a std::string,
   * can be used in a cast operation : (std::string) TileLaserPmt */
  operator std::string() const;


private:

  int m_counter;
  int m_diodeCurrOrd;
  int m_diodeCurrMeas;
  int m_filtNumber;
  int m_slamaDelay;

};

#endif
