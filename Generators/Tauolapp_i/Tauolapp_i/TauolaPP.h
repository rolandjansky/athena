/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUOLAPP_H
#define TAUOLAPP_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"


#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"

#include "GaudiKernel/IPartPropSvc.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"

class StoreGateSvc;

class IAtRndmGenSvc;


/// @brief This Algorithm provides an easy interface to Tauola C++ interface
/// @author Nadia Davidson, Marcin Wolter
/// @todo Convert to use standard GenModule base class
class TauolaPP : public AthAlgorithm {
public:

  /// Constructor
  TauolaPP (const std::string& name, ISvcLocator* pSvcLocator);

  /// Initialization of Tauola++ and setting of JO configurables
  StatusCode initialize();

  /// Pass each event in the McEventCollection to Tauola to (re)decay the taus
  StatusCode execute();


private:

  /// Event record container key
  std::string m_key;

  /// @name Variables used to configure Tauola
  //@{

  /// PDG ID of particle to study
   int m_decay_particle;

  /// TAUOLA decay mode of particles with same charge as "decay_particle"
  int m_decay_mode_same;

  /// TAUOLA decay mode of particles with opposite charge as "decay_particle"
  int m_decay_mode_opp;

  /// tau mass to be taken by TAUOLA
  double m_tau_mass;

  // TAUOLA switch for spin effects 
  bool m_spin_correlation;

  // TAUOLA switch for radiative corrections for leptonic tau decays 
  bool m_setRadiation;

  // TAUOLA cut-off for radiative corrections
  double m_setRadiationCutOff;

  static IAtRndmGenSvc* &atRndmGenSvc();
  static std::string &tauolapp_stream();

};

#endif
