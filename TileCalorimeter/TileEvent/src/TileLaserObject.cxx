/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*author Renato Febbraro*/
/*date 3/7/2007*/
/*modified 27/5/2008 adding methods for PLC*/
/*renato.febbraro@cern.ch*/

/*modified 23/08/2014 to accommodate LASERII system*/
/*Marco van Woerden <mvanwoer@cern.ch>*/

#include "TileEvent/TileLaserObject.h"


TileLaserObject::TileLaserObject()
  : m_slowCtrl()
  , m_laserParameter()
  , m_diodesLG(nbDiodes,TileLaserDiode())
  , m_diodesHG(nbDiodes,TileLaserDiode())
  , m_pmtsLG(nbPmts,TileLaserPmt())
  , m_pmtsHG(nbPmts,TileLaserPmt())
  , m_lascalib(nbTypes,std::vector<TileLasCalib>(16))
  , m_plc()
  , m_qdctimeout(false)
  , m_tdctimeout(false)
  , m_daqtype(0)
  , m_BCID(0)
  , m_version(0)
  , m_calibtype(-1)
{
/*  for(int type=0;type<nbTypes;++type){
    m_lascalib.push_back(std::vector<TileLasCalib>());
    for(int chan=0;chan<nbDiodes+nbPmts;++chan){
      m_lascalib[type].push_back(TileLasCalib());
    } // FOR
  } // FOR*/
} // TileLaserObject::TileLaserObject

double TileLaserObject::getMean(int chan, int gain, int type) const{
  if(type-firstType<0 || type-firstType>=nbTypes) return -1.;
  if(chan<0 || chan>=nbDiodes+nbPmts) return -1.;
  if(gain!=0 && gain!=1) return -1.;
  if(m_lascalib[type][chan].getType() == type) return m_lascalib[type][chan].getMean(gain);
  return -99.;
} // getMean

double TileLaserObject::getSigma(int chan, int gain, int type) const{
  if(type-firstType<0 || type-firstType>=nbTypes) return -1.;
  if(chan<0 || chan>=nbDiodes+nbPmts) return -1.;
  if(gain!=0 && gain!=1) return -1.;
  if(m_lascalib[type][chan].getType() == type) return m_lascalib[type][chan].getSigma(gain);
  return -99.;
} // getSigma

int TileLaserObject::getType(int chan, int gain, int type) const{
  if(type-firstType<0 || type-firstType>=nbTypes) return -1;
  if(chan<0 || chan>=nbDiodes+nbPmts) return -1;
  if(gain!=0 && gain!=1) return -1;
  return m_lascalib[type][chan].getType();
} // getMean

int TileLaserObject::getN(int chan, int gain, int type) const{
  if(type-firstType<0 || type-firstType>=nbTypes) return -1;
  if(chan<0 || chan>=nbDiodes+nbPmts) return -1;
  if(gain!=0 && gain!=1) return -1;
  return m_lascalib[type][chan].getN();
} // getN

bool TileLaserObject::isSet(int chan, int gain, int type) const{
  if(type-firstType<0 || type-firstType>=nbTypes) return false;
  if(chan<0 || chan>=nbDiodes+nbPmts) return false;
  if(gain!=0 && gain!=1) return false;
  return m_lascalib[type][chan].isSet(gain);
} // getSigma

void TileLaserObject::setCalibType(int type) {
  m_calibtype = type;
}

int TileLaserObject::getCalibType() const{
  return m_calibtype;
}

void TileLaserObject::setCalib(int chan, int type, double sumXinQDC, double sumX2inQDC, int nevts, int gain){
  if(!m_lascalib[type][chan].isSet(gain)){
    m_lascalib[type][chan].setCalib(type,sumXinQDC,sumX2inQDC,nevts,gain);
  } // IF
} // setCalib




int TileLaserObject::getDiodeADC(const unsigned int i, const unsigned int gain) const
{
  if(i<nbDiodes){
    if(gain==0){
      return m_diodesLG[i].getDiodeADC();
    } // IF
    else{
      return m_diodesHG[i].getDiodeADC();
    } // IF
  } else {
    return 0;
  }
}

double TileLaserObject::getDiodePedestal(const unsigned int i, const unsigned int gain) const
{
  if(i<nbDiodes){
    if(gain==0){
      return m_diodesLG[i].getDiodePedestal();
    } // IF
    else{
      return m_diodesHG[i].getDiodePedestal();
    } // IF
  }else{
    return 0.0;
  }
}

double TileLaserObject::getDiodeSigmaPedestal(const unsigned int i, const unsigned int gain) const
{
  if(i<nbDiodes){
    if(gain==0){
      return m_diodesLG[i].getDiodeSigmaPedestal();
    } // IF
    else{
      return m_diodesHG[i].getDiodeSigmaPedestal();
    } // IF
  }else{
    return 0.0;
  }
}


double TileLaserObject::getAlpha(const unsigned int i, const unsigned int gain) const
{
  if(i<nbDiodes){
    if(gain==0){
      return m_diodesLG[i].getAlpha();
    } // IF
    else{
      return m_diodesHG[i].getAlpha();
    } // IF
  }else{
    return 0.0;
  }
}

double TileLaserObject::getSigmaAlpha(const unsigned int i, const unsigned int gain) const
{
  if(i<nbDiodes){
    if(gain==0){
      return m_diodesLG[i].getSigmaAlpha();
    } // IF
    else{
      return m_diodesHG[i].getSigmaAlpha();
    } // IF
  }else{
    return 0.0;
  }
}

double TileLaserObject::getPedestalAlpha(const unsigned int i, const unsigned int gain) const
{
  if(i<nbDiodes){
    if(gain==0){
      return m_diodesLG[i].getPedestalAlpha();
    } // IF
    else{
      return m_diodesHG[i].getPedestalAlpha();
    } // IF
  }else{
    return 0.0;
  }
}

double TileLaserObject::getSigmaPedAlpha(const unsigned int i, const unsigned int gain) const
{
  if(i<nbDiodes){
    if(gain==0){
      return m_diodesLG[i].getSigmaPedAlpha();
    } // IF
    else{
      return m_diodesHG[i].getSigmaPedAlpha();
    } // IF
  }else{
    return 0.0;
  }
}


int TileLaserObject::getPMADC(const unsigned int j, const unsigned int gain ) const
{
  if(j<nbPmts){
    if(gain==0){
      return m_pmtsLG[j].getPMADC();
    } // IF
    else{
      return m_pmtsHG[j].getPMADC();
    } // IF
  }else{
    return 0;
  }
}

int TileLaserObject::getTDC(const unsigned int j, const unsigned int gain ) const
{
  if(j<nbPmts){
    if(gain==0){
      return m_pmtsLG[j].getTDC();
    } // IF
    else{
      return m_pmtsHG[j].getTDC();
    } // IF
  }else{
    return 0;
  }
}

double TileLaserObject::getPMPedestal(const unsigned int j, const unsigned int gain ) const
{
  if(j<nbPmts){
    if(gain==0){
      return m_pmtsLG[j].getPMPedestal();
    } // IF
    else{
      return m_pmtsHG[j].getPMPedestal();
    } // IF
  }else{
    return 0.0;
  }
}

double TileLaserObject::getPMSigmaPedestal(const unsigned int j, const unsigned int gain) const
{
  if(j<nbPmts){
    if(gain==0){
      return m_pmtsLG[j].getPMSigmaPedestal();
    } // IF
    else{
      return m_pmtsHG[j].getPMSigmaPedestal();
    } // IF
  }else{
    return 0.0;
  }
}



void TileLaserObject::setLaser(const int Counter,
                               const int diodeCurrOrd,
                               const int diodeCurrMeas,
                               const int filtNumber,
                               const int timingDelay,
                               const int version)
{
  m_laserParameter.setLaser(Counter, diodeCurrOrd, diodeCurrMeas, filtNumber, timingDelay);
  m_version = version;
  //ATH_MSG_ERROR("TileLaserObject::setLASER " << version);
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

void TileLaserObject::setTimeouts(const bool qdc, 
				  const bool tdc)
{
  m_qdctimeout = qdc;
  m_tdctimeout = tdc;
}


void TileLaserObject::setDiode(const unsigned int diode,
                               const int diodeAdc,
                               const double diodePedestal,
                               const double diodeSigmaPedestal,
                               const double alpha,
                               const double sigmaAlpha,
                               const double pedestalAlpha,
                               const double sigmaPedAlpha,
                               const unsigned int gain)
{
  //ATH_MSG_ERROR("TileLaserObject::setDiode " << diode);
  if(diode<nbDiodes){
    if(gain==0){
      m_diodesLG[diode].setDiode(diodeAdc, diodePedestal, diodeSigmaPedestal, alpha, sigmaAlpha, pedestalAlpha, sigmaPedAlpha);
    } // IF
    else{
      m_diodesHG[diode].setDiode(diodeAdc, diodePedestal, diodeSigmaPedestal, alpha, sigmaAlpha, pedestalAlpha, sigmaPedAlpha);
    } // IF
  }else{
    //   log<<MSG::ERROR<<"Exit code in method TileLaserObject::setDiode"<<endreq;
    exit(0);
  }
}

void TileLaserObject::setPmt(const unsigned int pmt,
                             const int pmAdc,
                             const int tdc,
                             const double pmPedestal,
                             const double pmSigmaPedestal,
                             const unsigned int gain)
{
  if(pmt<nbPmts){
    if(gain==0){
      m_pmtsLG[pmt].setPmt(pmAdc, tdc, pmPedestal, pmSigmaPedestal);
    } // IF
    else{
      m_pmtsHG[pmt].setPmt(pmAdc, tdc, pmPedestal, pmSigmaPedestal);
    } // IF
  }else{
    //    log<<MSG::ERROR<<"Exit code in method TileLaserObject::setPmt"<<endreq;
    exit(0);
  }
}

void TileLaserObject::setDaqType(const unsigned int daqtype){
  m_daqtype = daqtype;
}

