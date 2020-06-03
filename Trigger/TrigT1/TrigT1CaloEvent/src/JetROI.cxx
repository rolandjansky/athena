/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          Jetroi.cxx  -  description
                             -------------------
    begin                : Mon Jan 22 2001
    email                : moyse@heppch.ph.qmw.ac.uk
 ***************************************************************************/


#include "TrigT1CaloEvent/JetROI.h"
namespace LVL1 {

JetROI::JetROI(double phi, double eta, int energy, unsigned long int roiWord,
  unsigned int cluster4, unsigned int cluster6, unsigned int cluster8, 
  bool saturated):
  m_phi(phi),
  m_eta(eta),
  m_energy(energy),
  m_cluster4(cluster4),
  m_cluster6(cluster6),
  m_cluster8(cluster8),
  m_roiWord(roiWord),
  m_isSaturated(saturated)
{
}

JetROI::~JetROI(){
	//delete m_algorithmType;
}

/** returns the 32bit ROI word.
*\todo implement hardware coords in ROI word*/
unsigned int LVL1::JetROI::roiWord() const{
  return m_roiWord;
}

/** returns TRUE if threshold number threshold_number has been passed by 
this ROI. */
bool LVL1::JetROI::thresholdPassed(int thresh) const {
  if (m_roiWord & (1<<(thresh-1)) )return true;
  return false;
}

/** returns TRUE if threshold number threshold_number has been passed by
this ROI. */
bool LVL1::JetROI::fwdThresholdPassed(int thresh) const {
  if (m_roiWord & (1<<(thresh+7)) )return true;
  return false;
}

/** returns true if the RoI is a forward jet*/
bool LVL1::JetROI::isForward() const{
  if ((m_eta<-3.1)||(m_eta>3.1)) return true;
  return false;
}

/** returns type of thresholds number <em>threshold_number</em>.
\todo implement*/
LVL1::TrigT1CaloDefs::JetWindowSize LVL1::JetROI::thresholdType(int /*thresh*/) const{
  // shouldn't ever be reached.
  return TrigT1CaloDefs::JetWinError;
}


/** returns eta coord of ROI */
double LVL1::JetROI::eta() const {
  return m_eta;
}

/** returns phi coord of ROI */
double LVL1::JetROI::phi() const {
  return m_phi;
}


/**returns energy of ROI core*/
int LVL1::JetROI::energy() const {
	return m_energy;
}

 /** returns TRUE if ROI constructed from a 
	Jet Element that was saturated - in other
	words this ROI is signalling that it does not
	know it's correct energy. 
	*/
bool LVL1::JetROI::saturated() const {
  return m_isSaturated;
}


}//end of LVL1 namespace defn
