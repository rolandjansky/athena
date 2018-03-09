/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HFORTOOL_H
#define HFORTOOL_H

#include "GenInterfaces/IHforTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "HepMC/GenEvent.h"
#include "EventInfo/EventID.h"
#include "JetEvent/JetCollection.h"

#include <string>
#include <vector>
#include <map>

class StoreGateSvc;
class McEventCollection;
class Jet;


/// @brief Tool for post-hoc heavy flavor overlap removal in Alpgen and MadGraph
/// @author Dominic Hirschbuehl
/// @author Michiel Sanders
class HforTool : virtual public IHforTool, public AthAlgTool {
public:

  HforTool(const std::string&, const std::string&, const IInterface*);
  ~HforTool();

  virtual StatusCode initialize();
  virtual StatusCode execute();

  void setGenEvent(HepMC::GenEvent* e) { m_evt = e; }
  HepMC::GenEvent* getGenEvent() const { return m_evt; }

  /// @deprecated Use normal coding convention for method names: setGenEvent
  void SetGenEvent(HepMC::GenEvent* e) { setGenEvent(e); }
  /// @deprecated Use normal coding convention for method names: getGenEvent
  HepMC::GenEvent* GetGenEvent() const { return getGenEvent(); }

  /// @name Functions to retrieve the b/c quarks
  //@{
  const std::vector<HepMC::FourVector> & get_bQuarks_MPI();
  const std::vector<HepMC::FourVector> & get_bQuarks_GS();
  const std::vector<HepMC::FourVector> & get_bQuarks_ME();
  const std::vector<HepMC::FourVector> & get_bQuarks_MEin();
  const std::vector<HepMC::FourVector> & get_bQuarks_PDF();
  const std::vector<HepMC::FourVector> & get_bQuarks_topdecay();
  const std::vector<HepMC::FourVector> & get_bQuarks_unknown();
  const std::vector<HepMC::FourVector> & get_cQuarks_MPI();
  const std::vector<HepMC::FourVector> & get_cQuarks_GS();
  const std::vector<HepMC::FourVector> & get_cQuarks_ME();
  const std::vector<HepMC::FourVector> & get_cQuarks_MEin();
  const std::vector<HepMC::FourVector> & get_cQuarks_PDF();
  const std::vector<HepMC::FourVector> & get_cQuarks_topdecay();
  const std::vector<HepMC::FourVector> & get_cQuarks_unknown();
  //@}

  /// Function to retrieve the decision
  std::string getDecision(const std::string& schema="");


private:

  std::string m_schema;
  double m_minjetpt;
  double m_matchingcone;
  bool m_isAtlfast;
  std::string m_JetContainer;
  std::vector<std::string> m_McEventCollectionKeys;

  HepMC::GenEvent* m_evt;
  McEventCollection* m_mceventTESout;
  /// @todo Remove and use evtStore() instead
  StoreGateSvc* m_storeGate;
  const JetCollection* m_jets;

  /// Variables to classify the sample
  std::string m_sampleType;
  std::string m_userSampleType;
  bool m_isZinclusive;

  /// Variable to keep the decision
  std::string m_result;

  /// Run/event number for the previously processed event
  /// @note So that the same GenEvent doesn't get read twice for one event
  EventID::number_type m_prev_run_number;
  EventID::number_type m_prev_event_number;

  /// Lists of four-vectors for b/c quarks from MPI, indexed by abs pdgId
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_MPI;
  /// Lists of four-vectors for b/c quarks from shower gluon splitting, indexed by abs pdgId
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_GS;
  /// Lists of four-vectors for b/c quarks from matrix element (processed), indexed by abs pdgId
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_ME;
  /// Lists of four-vectors for b/c quarks from matrix element (unprocessed), indexed by abs pdgId
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_MEin;
  /// Lists of four-vectors for b/c quarks from top decay (NB: careful!), indexed by abs pdgId
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_topdecay;
  /// Lists of four-vectors for b/c quarks from the incoming proton (NB: careful!), indexed by abs pdgId
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_PDF;
  /// Lists of four-vectors for b/c quarks from unknown origin, indexed by abs pdgId
  std::map< int,std::vector<HepMC::FourVector> > m_Quarks_unknown;

  /// Name of the generator used for showering (PYTHIA or HERWIG)
  /// @todo Replace/remove/improve... need to generalize
  std::string m_ShowerGenerator;
  /// Name of the shower generator if type cannot be found in the event
  /// @note Default: ""; allows for running HforTool on, eg, SHERPA
  std::string m_DefaultShowerGenerator;

  // StoreGate key of the McEventCollection to use; taken from the first
  // collection found in the first event
  std::string m_McEventCollectionKey;


  /// Function to loop over the generated event record and find the b/c quarks,
  void findHFQuarks();

  /// Specific fn for finding HF quarks in HERWIG6 event record
  void findHFQuarksHerwig(const std::map< int,std::vector<const HepMC::GenParticle*> >& finalstate_q);
  /// Specific fn for finding HF quarks in PYTHIA6 event record
  void findHFQuarksPythia(const std::map< int,std::vector<const HepMC::GenParticle*> >& finalstate_q);
  /// If we don't know the shower generator, use this to set all to "unknown"
  void findHFQuarksUnknown(const std::map< int,std::vector<const HepMC::GenParticle*> >& finalstate_q);

  void jetBasedRemoval();
  void angularBasedRemoval();
  void keepAllRemoval();

  void checkSampleType();
  void testSampleType();


  int matchdR(std::vector<HepMC::FourVector>* quarks);

  inline double deltaR(const HepMC::FourVector& v1, const Jet& v2) {
    double dphi = std::fabs(v1.phi()-v2.phi());
    dphi = (dphi<=M_PI)? dphi : 2*M_PI-dphi;
    double deta = std::fabs(v1.eta()-v2.eta());
    return std::sqrt(dphi*dphi+deta*deta);
  }

  inline double deltaR(const HepMC::FourVector& v1, const HepMC::FourVector& v2) {
    double dphi = std::fabs(v1.phi()-v2.phi());
    dphi = (dphi<=M_PI)? dphi : 2*M_PI-dphi;
    double deta = std::fabs(v1.eta()-v2.eta());
    return std::sqrt(dphi*dphi+deta*deta);
  }

};


#endif
