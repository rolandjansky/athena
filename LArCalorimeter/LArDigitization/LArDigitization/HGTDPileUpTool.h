/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGITIZATION_HGTDPILEUPTOOL_H
#define LARDIGITIZATION_HGTDPILEUPTOOL_H
// +======================================================================+
// +                                                                      +
// +  Author : G.Unal                                                     +
// +  HGTD digitization using PileUpTool                                  +
// +                                                                      +
// +======================================================================+
//
// ....... include
//

#include "PileUpTools/PileUpToolBase.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Property.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "LArDigitization/IHGTDPileUpTool.h"

#include "StoreGate/DataHandle.h" 

class StoreGateSvc;
class PileUpMergeSvc;
class HGTD_ID;
class LArHit;
class LArHitContainer;
class HGTDLessHit;

namespace CLHEP {
  class HepRandomEngine;
}

class HGTDPileUpTool : virtual public IHGTDPileUpTool, public PileUpToolBase
{
//
// >>>>>>>> public method 
//
 public:
   
  HGTDPileUpTool(const std::string& type, 
		 const std::string& name,
		 const IInterface* parent);

  ~HGTDPileUpTool();

  virtual StatusCode initialize();

  virtual StatusCode prepareEvent(unsigned int nInputEvents);

  virtual StatusCode mergeEvent();

  virtual StatusCode processBunchXing(int bunchXing,
                                      PileUpEventInfo::SubEvent::const_iterator bSubEvents,
                                      PileUpEventInfo::SubEvent::const_iterator eSubEvents); 

  virtual StatusCode processAllSubEvents();

  virtual StatusCode fillMapFromHit(StoreGateSvc* seStore,float tbunch,bool isSignal);

  static const InterfaceID& interfaceID() { 
    return IHGTDPileUpTool::interfaceID();}

 private:

  StatusCode AddHit(const LArHit *, double);


  // We need two types containers for hits:
  
  // The set defined below is used to tell us if we've already had a
  // hit in a cell.  We store these hits in a set, so we can quickly
  // search it.  Note the use of a custom definition of a "less"
  // function for the set, so we're not just comparing hit pointers.
  typedef std::set< LArHit*, HGTDLessHit >  hits_t;
  // The hits are grouped into time bins, with the width of a bin
  // determined by a user parameter.  This map is used to associate a
  // time bin with its corresponding set of hits.
  typedef std::map < int, hits_t* >   timeBins_t;
  timeBins_t m_timeBins;

  StatusCode MakeDigit();
  int getTimeBin(double time) const;
		   
//
// >>>>>>>> private data parts
//
  PileUpMergeSvc* m_mergeSvc;  
//
// ........ Algorithm properties
//
  std::string m_DigitContainerName;    // output digit container name list
  LArHitContainer* m_DigitContainer;
  std::string m_HgtdHitContainerName; 
  bool m_NoiseOnOff;            // noise (in all sub-detectors) is on if true  
  bool m_PileUp;                // pile up or not
  //put false if you want cancel the noise in one or several sub-detectors
  bool m_CrossTalk;                // flag for Cross Talk
  std::string  m_rndmSvc;          // random service name
  bool m_rndmEvtRun;               // use run,event number for random number seeding 
  bool m_useTriggerTime;
  bool m_RndmEvtOverlay;         // Pileup and noise added by overlaying random events
  bool m_useBad;
  std::string m_timeBinType;
  double m_timeBinWidth;
  
  const HGTD_ID*        m_hgtd_id;

  double m_EnergyThresh;           // Zero suppression energy threshold

  IAtRndmGenSvc* m_AtRndmGenSvc;
  CLHEP::HepRandomEngine* m_engine;

};

class HGTDLessHit {
 public:
  bool operator() ( LArHit* const& p, LArHit* const& q ) const;
};
 
#endif
