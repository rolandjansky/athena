/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUOLAALG_H
#define TAUOLAALG_H

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


/// @brief This Algorithm provides an easy interface to Tauola C++ interface
/// @author Nadia Davidson, Marcin Wolter
/// @todo Convert to use standard GenModule base class
class TauolaAlg : public AthAlgorithm {
public:

  /// Constructor
  TauolaAlg (const std::string& name, ISvcLocator* pSvcLocator);

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

};

#endif
