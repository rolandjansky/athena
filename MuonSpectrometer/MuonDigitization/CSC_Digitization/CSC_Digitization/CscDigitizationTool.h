/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONDIGITIZATION_CSCDIGITIZATIONTOOL_H
#define MUONDIGITIZATION_CSCDIGITIZATIONTOOL_H 

#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"

#include "GaudiKernel/ToolHandle.h"
//#include "AthenaBaseComps/AthAlgTool.h"
#include "CSC_Digitization/CSC_Digitizer.h"
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonSimEvent/CSCSimHit.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "HitManagement/TimedHitCollection.h"
#include "CscCalibTools/ICscCalibTool.h"

#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimData/CscSimDataCollection.h"

#include "EventInfo/PileUpEventInfo.h"  /*SubEvent*/

#include "xAODEventInfo/EventInfo.h"             // NEW EDM
#include "xAODEventInfo/EventAuxInfo.h"          // NEW EDM

#include "PileUpTools/PileUpToolBase.h"

class PileUpMergeSvc;

// Author: Ketevi A. Assamagan
// BNL, October 27, 2003

// Digitization algorithm for the CSC:
// get the hit container from Storegate
// loop over the hits
// digitize each hit
// loop over the digit and build the digit container
// store the digits in StoreGate

class CscDigitizationTool : virtual public IMuonDigitizationTool, public PileUpToolBase {

  typedef std::vector<CscSimData::Deposit> deposits;
  typedef std::map<IdentifierHash,std::pair<double,double> > csc_map;
  typedef std::map<IdentifierHash,std::vector<float> > csc_newmap;
  
  
public:
  
  CscDigitizationTool(const std::string& type, const std::string& name, const IInterface* pIID);

  ~CscDigitizationTool();
  
  virtual StatusCode initialize() override final;

  virtual StatusCode finalize()  override final;

  // PileUpTool methods...
  ///called at the end of the subevts loop. Not (necessarily) able to access subEvents
  virtual StatusCode mergeEvent()  override final;
  
  ///called for each active bunch-crossing to process current subEvents. bunchXing is in ns
  virtual  StatusCode processBunchXing(
                                int bunchXing,
                                SubEventIterator bSubEvents,
                                SubEventIterator eSubEvents
                                ) override final;

  /// return false if not interested in  certain xing times (in ns)
  /// implemented by default in PileUpToolBase as FirstXing<=bunchXing<=LastXing
  //  virtual bool toProcess(int bunchXing) const;
  virtual StatusCode prepareEvent(unsigned int /*nInputEvents*/)  override final;
 
  /// alternative interface which uses the PileUpMergeSvc to obtain
  /// all the required SubEvents.
  virtual StatusCode processAllSubEvents()  override final;

  /// Used by CscDigitBuilder. Just calls processAllSubEvents -
  /// leaving for back-compatibility (IMuonDigitizationTool)
  virtual StatusCode digitize()  override final;

public: //possibly these should be private?
  StatusCode FillCollectionWithNewDigitEDM(csc_newmap& data_SampleMap, //csc_newmap& data_SampleMapOddPhase,
                                           std::map<IdentifierHash,deposits>& myDeposits,
                                           CscSimDataCollection* sdoContainer, bool phaseToSet);
  StatusCode FillCollectionWithOldDigitEDM(csc_map& data_map, std::map<IdentifierHash,deposits>& myDeposits,
                                           CscSimDataCollection* sdoContainer);

  StatusCode CoreDigitization(CscSimDataCollection* sdoCollection);
  
  // accessors
  ServiceHandle<IAtRndmGenSvc> getRndmSvc() const { return m_rndmSvc; }    // Random number service
  CLHEP::HepRandomEngine  *getRndmEngine() const { return m_rndmEngine; } // Random number engine used 


 private:
  
  ToolHandle<ICscCalibTool> m_pcalib;

  CscDigitContainer         * m_container;

  const MuonGM::MuonDetectorManager * m_geoMgr;
  CSC_Digitizer             * m_cscDigitizer;

  const CscIdHelper         * m_cscIdHelper;

  double m_pedestal;
  bool m_maskBadChannel;
  double m_amplification;
  
  //pile-up
  TimedHitCollection<CSCSimHit>* m_thpcCSC; // the hits
  std::list<CSCSimHitCollection*> m_cscHitCollList; // only for pileup events..

  double m_timeWindowLowerOffset;
  double m_timeWindowUpperOffset;
  bool   m_isPileUp;
  bool   m_newDigitEDM;
  double m_driftVelocity;
  double m_electronEnergy;
  bool   m_NInterFixed;
  unsigned int m_run;
  unsigned int m_evt;

  ///////////////////////////////////////////////////////////////////
  // Access to the event methods:
  ///////////////////////////////////////////////////////////////////
private:
  
  // Get next event and extract collection of hit collections:
  StatusCode getNextEvent();
  
protected:  
  
  PileUpMergeSvc *m_mergeSvc; // Pile up service
  std::string m_inputObjectName; // name of the input objects
  std::string m_outputObjectName; // name of the output digits
  
  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  std::string m_rndmEngineName;// name of random engine

};

#endif // MUONDIGITIZATION_CSCDIGITIZATIONTOOL_H
