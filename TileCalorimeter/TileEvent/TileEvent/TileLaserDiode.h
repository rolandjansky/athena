/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*author Renato Febbraro*/
/*date 3/7/2007*/
/*renato.febbraro@cern.ch*/

#ifndef TILELASERDIODE_H
#define TILELASERDIODE_H

class TileLaserDiode {

 public:

  TileLaserDiode() 
    : m_diodeADC(0)
    , m_diodePedestal(0.0)
    , m_diodeSigmaPedestal(0.0)
    , m_alpha(0.0)
    , m_sigmaAlpha(0.0)
    , m_pedestalAlpha(0.0)
    , m_sigmaPedAlpha(0.0) { }

  ~TileLaserDiode() { }

  int getDiodeADC() const              { return m_diodeADC; }         
  double getDiodePedestal() const      { return m_diodePedestal; }     
  double getDiodeSigmaPedestal() const { return m_diodeSigmaPedestal; }
  double getAlpha() const              { return m_alpha; }
  double getSigmaAlpha() const         { return m_sigmaAlpha; }        
  double getPedestalAlpha() const      { return m_pedestalAlpha; }     
  double getSigmaPedAlpha() const      { return m_sigmaPedAlpha; }        


  void setDiode(const int diodeAdc, 
                const double diodePedestal, 
                const double diodeSigmaPedestal, 
                const double alpha, 
                const double sigmaAlpha,
                const double pedestalAlpha, 
                const double sigmaPedAlpha) {
      m_diodeADC = diodeAdc;
      m_diodePedestal = diodePedestal;
      m_diodeSigmaPedestal = diodeSigmaPedestal;
      m_alpha = alpha;
      m_sigmaAlpha = sigmaAlpha;
      m_pedestalAlpha = pedestalAlpha;
      m_sigmaPedAlpha = sigmaPedAlpha;
  }
    
 private:
  
  int m_diodeADC;
  double m_diodePedestal;
  double m_diodeSigmaPedestal;
  double m_alpha;
  double m_sigmaAlpha;
  double m_pedestalAlpha;
  double m_sigmaPedAlpha;

};

#endif
