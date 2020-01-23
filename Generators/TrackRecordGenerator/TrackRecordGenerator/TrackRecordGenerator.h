/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@class TrackRecordGenerator

@brief Track Record Generator.  The class takes input simulation track records and uses them as generator records.

@author 
   Zach Marshall <ZLMarshall@lbl.gov>

*/

#ifndef TRACKRECORDGENERATOR_H
#define TRACKRECORDGENERATOR_H

#include "GeneratorModules/GenModule.h"

// Members
#include "CLHEP/Vector/LorentzVector.h"
#include "HepMC/Polarization.h"

#include <vector>
#include <string>

class TrackRecordGenerator : public GenModule {
 public:
   TrackRecordGenerator(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~TrackRecordGenerator();
   virtual StatusCode callGenerator();
   virtual StatusCode fillEvt(HepMC::GenEvent* evt);
 
 private:
 
   // Event record holders
   int m_events;
   std::vector<int> m_pdgCode;
   std::vector<CLHEP::HepLorentzVector> m_fourPos;
   std::vector<CLHEP::HepLorentzVector> m_fourMom;
   CLHEP::Hep3Vector m_center;
   std::vector<HepMC::Polarization> m_polarization;
 
   // Options for track records 
   double m_smearTR;  //!< Amount by which to smear TR position
   double m_smearTRp; //!< Amount by which to smear TR momentum
   std::string m_recordName;  //!< TrackRecord collection name
   bool m_stopParticles;      //!< Stop particles before simulation
   float m_stopped_tminus, m_stopped_tplus; //!< Bounds for random time
   bool m_add_cL; //!< For stopped particles, add c*L to the time
};

#endif
