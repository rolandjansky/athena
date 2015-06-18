/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*author Renato Febbraro*/
/*date 3/7/2007*/
/*renato.febbraro@cern.ch*/

#ifndef TILELASERPMT_H
#define TILELASERPMT_H

class TileLaserPmt {

 public:

  TileLaserPmt() 
    : m_PMADC(0)
    , m_TDC(0)
    , m_pmPedestal(0.0)
    , m_pmSigmaPedestal(0.0) { }

  ~TileLaserPmt() { }

  int getPMADC() const               { return m_PMADC;           }
  int getTDC() const                 { return m_TDC;             }
  double getPMPedestal() const       { return m_pmPedestal;      }
  double getPMSigmaPedestal() const  { return m_pmSigmaPedestal; }
  
  void setPmt(const int pmAdc, 
	      const int tdc, 
	      const double pmPedestal, 
	      const double pmSigmaPedestal) {
      m_PMADC = pmAdc;
      m_TDC = tdc;
      m_pmPedestal = pmPedestal;
      m_pmSigmaPedestal = pmSigmaPedestal;
  }

 private:

  int m_PMADC;
  int m_TDC;
  double m_pmPedestal;
  double m_pmSigmaPedestal;
 
};

#endif
