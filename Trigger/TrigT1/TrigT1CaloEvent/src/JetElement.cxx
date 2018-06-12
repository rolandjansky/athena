/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          JetElement.cpp  -  description
                             -------------------
    begin                : Mon Sep 25 2000
    email                : e.moyse@qmw.ac.uk
 ***************************************************************************/




#ifndef  TRIGGERSPACE
#include "TrigT1CaloEvent/JetElement.h"
#else
#include "JetElement.h"
#endif


namespace LVL1 {

JetElement::JetElement():
  m_phi(0.0),
  m_eta(0.0),
  m_key(0),
  m_peak(0),
  m_emEnergy(1),
  m_hadEnergy(1),
  m_em_error(1),
  m_had_error(1),
  m_link_error(1)
{	
}

/** constructs a jet element with one timeslice */
JetElement::JetElement(double phi, double eta,
                       int emEnergy, int hadEnergy, unsigned int key, 
                       int emError, int hadError, int linkError):
  m_phi(phi),
  m_eta(eta),
  m_key(key),
  m_peak(0),
  m_emEnergy(1,emEnergy),
  m_hadEnergy(1,hadEnergy),
  m_em_error(1,emError),
  m_had_error(1,hadError),
  m_link_error(1,linkError)
{
}

/** constructs a jet element with multiple timeslices */
JetElement::JetElement(double phi, double eta,
                       const std::vector<int>& emEnergy,
         	       const std::vector<int>& hadEnergy,
		       unsigned int key,
		       const std::vector<int>& emError,
		       const std::vector<int>& hadError,
		       const std::vector<int>& linkError, int peak):
  m_phi(phi),
  m_eta(eta),
  m_key(key),
  m_peak(peak),
  m_emEnergy(emEnergy),
  m_hadEnergy(hadEnergy),
  m_em_error(emError),
  m_had_error(hadError),
  m_link_error(linkError)
{
}

JetElement::~JetElement(){
}


/** returns eta coord */
double JetElement::eta() const{
	return m_eta;
}

/** returns phi coord of tower */
double JetElement::phi() const{
	return m_phi;
}

/** returns em ET */
int JetElement::emEnergy() const{
  if (m_emEnergy[m_peak] < m_layerSaturationThreshold) {
    return m_emEnergy[m_peak];
  }
  else {
    return m_layerSaturationThreshold;
  }
}

/** returns had ET */
int JetElement::hadEnergy() const{
  if (m_hadEnergy[m_peak] < m_layerSaturationThreshold) {
    return m_hadEnergy[m_peak];
  }
  else {
    return m_layerSaturationThreshold;
  }
}

/** returns jet element ET<br>
    The LVL1 Calo trigger uses a 10 bit integer i.e. 0-1023 GeV
    JetElements are treated as saturated if any of tower, layer or
    element ET exceed their dynamic range */
int JetElement::energy() const{
  if ( (m_emEnergy[m_peak]  >= m_layerSaturationThreshold) ||
       (m_hadEnergy[m_peak] >= m_layerSaturationThreshold) ||
       (m_emEnergy[m_peak] + m_hadEnergy[m_peak] >= m_saturationThreshold) ){
    return m_saturationThreshold;
  }else{
    return m_emEnergy[m_peak] + m_hadEnergy[m_peak];
  }
}

/** return ET for specified slice */
int JetElement::emSliceEnergy(int slice) const{
  if (slice >=0 && slice < (int)m_emEnergy.size()) {
    if (m_emEnergy[slice] < m_layerSaturationThreshold) {
      return m_emEnergy[slice];
    }
    else {
      return m_layerSaturationThreshold;
    }
  }
  else return 0;
}

int JetElement::hadSliceEnergy(int slice) const{
  if (slice >=0 && slice < (int)m_hadEnergy.size()) {
    if (m_hadEnergy[slice] < m_layerSaturationThreshold) {
      return m_hadEnergy[slice];
    }
    else {
      return m_layerSaturationThreshold;
    }
  }
  else return 0;
}

int JetElement::sliceEnergy(int slice) const{
  if ( (emSliceEnergy(slice)  >= m_layerSaturationThreshold) ||
       (hadSliceEnergy(slice) >= m_layerSaturationThreshold) ||
       (emSliceEnergy(slice) + hadSliceEnergy(slice) >= m_saturationThreshold) ){
    return m_saturationThreshold;
  }
  else{
    return emSliceEnergy(slice) + hadSliceEnergy(slice);
  }
}

/** this will add energy to the JE. It's really intended for
JetElementMaker to use when creating JEs. */
void JetElement::addEnergy(int emEnergy, int hadEnergy){
  m_emEnergy[m_peak]  += emEnergy;
  if (m_emEnergy[m_peak] > m_layerSaturationThreshold) 
      m_emEnergy[m_peak] = m_layerSaturationThreshold;
  m_hadEnergy[m_peak] += hadEnergy;
  if (m_hadEnergy[m_peak] > m_layerSaturationThreshold)
      m_hadEnergy[m_peak] = m_layerSaturationThreshold;
  return;
}

/** add data for one timeslice. Used by bytestream converter. */
void JetElement::addSlice(int slice, int emEnergy, int hadEnergy,
                          int emError, int hadError, int linkError){
  m_emEnergy[slice]   = 
   ((emEnergy < m_layerSaturationThreshold) ? emEnergy : m_layerSaturationThreshold);
  m_hadEnergy[slice]  = 
   ((hadEnergy < m_layerSaturationThreshold) ? hadEnergy : m_layerSaturationThreshold);
  m_em_error[slice]   = emError;
  m_had_error[slice]  = hadError;
  m_link_error[slice] = linkError;
  return;
}

/** returns JE key - that is the key the JE had
when it was created. Added for bug-testing.*/
unsigned int JetElement::key() const{
  return m_key;
}

/** returns TRUE if the JetElement is saturated
(i.e. either the energy of the JetElement itself or that of one of its
components exceeds its dynamic range) and FALSE otherwise. */
bool JetElement::isSaturated() const{
  if ( (m_emEnergy[m_peak] >= m_layerSaturationThreshold) ||
       (m_hadEnergy[m_peak] >= m_layerSaturationThreshold) ||
       (m_emEnergy[m_peak] + m_hadEnergy[m_peak] >= m_saturationThreshold) ){
    return true;
  }else{
    return false;
  }
}


/** returns TRUE if the Em layer of the JetElement is saturated */
bool JetElement::isEmSaturated() const{
  if ( m_emEnergy[m_peak] >= m_layerSaturationThreshold ){
    return true;
  }else{
    return false;
  }
}

/** returns TRUE if the Had layer of the JetElement is saturated */
bool JetElement::isHadSaturated() const{
  if ( m_hadEnergy[m_peak] >= m_layerSaturationThreshold ){
    return true;
  }else{
    return false;
  }
}

/** return Em error */
int JetElement::emError() const{
  return m_em_error[m_peak];
}

/** return Had error */
int JetElement::hadError() const{
  return m_had_error[m_peak];
}

/** return link error */
int JetElement::linkError() const{
  return m_link_error[m_peak];
}

/** return triggered slice offset */
int JetElement::peak() const{
  return m_peak;
}

/** return Em energy vector reference (all timeslices) */
const std::vector<int>& JetElement::emEnergyVec() const{
  return m_emEnergy;
}

/** return Had energy vector reference (all timeslices) */
const std::vector<int>& JetElement::hadEnergyVec() const{
  return m_hadEnergy;
}

/** return Em error vector reference (all timeslices) */
const std::vector<int>& JetElement::emErrorVec() const{
  return m_em_error;
}

/** return Had error vector reference (all timeslices) */
const std::vector<int>& JetElement::hadErrorVec() const{
  return m_had_error;
}

/** return link error vector reference (all timeslices) */
const std::vector<int>& JetElement::linkErrorVec() const{
  return m_link_error;
}

/** return coord of JE */
Coordinate JetElement::coord() const{
  return Coordinate(m_phi,m_eta);
}

}// end of LVL1 namespace
