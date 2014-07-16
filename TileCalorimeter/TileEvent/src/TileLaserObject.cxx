/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*author Renato Febbraro*/
/*date 3/7/2007*/
/*modified 27/5/2008 adding methods for PLC*/
/*renato.febbraro@cern.ch*/

#include "TileEvent/TileLaserObject.h"

using namespace TileLaser;

const unsigned int TileLaserObject::nbDiodes=4;
const unsigned int TileLaserObject::nbPmts=2;


TileLaserObject::TileLaserObject() :
  m_slowCtrl(),
  m_laserParameter(),
  m_diodes(nbDiodes,TileLaserDiode()),
  m_pmts(nbPmts,TileLaserPmt()),
  m_plc(),
  m_BCID(0)
{
}

int TileLaserObject::getDiodeADC(const unsigned int i) const
{
  if(i<nbDiodes){
    return m_diodes[i].getDiodeADC();
  } else {
    return 0;
  }
}

double TileLaserObject::getDiodePedestal(const unsigned int i) const
{
  if(i<nbDiodes){
  return m_diodes[i].getDiodePedestal();
  }else{
    return 0.0;
  }
}

double TileLaserObject::getDiodeSigmaPedestal(const unsigned int i) const
{
  if(i<nbDiodes){
  return m_diodes[i].getDiodeSigmaPedestal();
  }else{
    return 0.0;
  }
}


double TileLaserObject::getAlpha(const unsigned int i) const
{
  if(i<nbDiodes){
  return m_diodes[i].getAlpha();
  }else{
    return 0.0;
  }
}

double TileLaserObject::getSigmaAlpha(const unsigned int i) const
{
  if(i<nbDiodes){  
  return m_diodes[i].getSigmaAlpha();
  }else{
    return 0.0;
  }
}

double TileLaserObject::getPedestalAlpha(const unsigned int i) const
{
  if(i<nbDiodes){
  return m_diodes[i].getPedestalAlpha();
  }else{
    return 0.0;
  }
}

double TileLaserObject::getSigmaPedAlpha(const unsigned int i) const
{
  if(i<nbDiodes){
  return m_diodes[i].getSigmaPedAlpha();
  }else{
    return 0.0;
  }
}


int TileLaserObject::getPMADC(const unsigned int j) const
{
  if(j<nbPmts){
  return m_pmts[j].getPMADC();
  }else{
    return 0;
  }
 }

int TileLaserObject::getTDC(const unsigned int j) const
{
  if(j<nbPmts){
  return m_pmts[j].getTDC();
  }else{
    return 0;
  }
 }

double TileLaserObject::getPMPedestal(const unsigned int j) const
{
  if(j<nbPmts){
  return m_pmts[j].getPMPedestal();
  }else{
    return 0.0;
  }
}

double TileLaserObject::getPMSigmaPedestal(const unsigned int j) const
{
  if(j<nbPmts){
  return m_pmts[j].getPMSigmaPedestal();
  }else{
    return 0.0;
  }
}



void TileLaserObject::setLaser(const int Counter,
			       const int diodeCurrOrd, 
			       const int diodeCurrMeas, 
			       const int filtNumber,
			       const int SlamaDelay)
{
  m_laserParameter.setLaser(Counter, diodeCurrOrd, diodeCurrMeas, filtNumber, SlamaDelay);
}

void TileLaserObject::setControl(const double pumpDiodeTemp, 
                                 const int timeLastMeasP, 
                                 const double diodeBoxTemp,
				 const int timeLastMeasD, 
                                 const double gasFlux,
				 const int timeLastMeasF, 
                                 const double humidity, 
				 const int timeLastMeasH, 
                         	 const time_t lastPedMeas,
				 const time_t lastAlphaMeas) 
{
  m_slowCtrl.setControl(pumpDiodeTemp, timeLastMeasP, diodeBoxTemp,timeLastMeasD, gasFlux, timeLastMeasF, humidity, timeLastMeasH, lastPedMeas, lastAlphaMeas);
}


void TileLaserObject::setPLC(const int alphaPos,
			     const double LVdiodes,
			     const double HVpmts,
			     const int shutter,
			     const int interlock,
			     const int alarm)
{

  m_plc.setPLC(alphaPos, LVdiodes, HVpmts, shutter, interlock, alarm);

}

void TileLaserObject::setDiode(const unsigned int diode,
                               const int diodeAdc, 
                               const double diodePedestal, 
                               const double diodeSigmaPedestal,
                               const double alpha,
                               const double sigmaAlpha,
                               const double pedestalAlpha, 
                               const double sigmaPedAlpha)
{
  if(diode<nbDiodes){
    m_diodes[diode].setDiode(diodeAdc, diodePedestal, diodeSigmaPedestal, alpha, sigmaAlpha, pedestalAlpha, sigmaPedAlpha);
  }else{
    //   log<<MSG::ERROR<<"Exit code in method TileLaserObject::setDiode"<<endreq;
    exit(0);
  }
}

void TileLaserObject::setPmt(const unsigned int pmt,
                             const int pmAdc, 
                             const int tdc, 
                             const double pmPedestal, 
                             const double pmSigmaPedestal)
{ 
  if(pmt<nbPmts){
    m_pmts[pmt].setPmt(pmAdc, tdc, pmPedestal, pmSigmaPedestal);
  }else{
    //    log<<MSG::ERROR<<"Exit code in method TileLaserObject::setPmt"<<endreq;
    exit(0);
  }
}




