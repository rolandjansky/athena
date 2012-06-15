/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CLHEP/HepPDT/CommonParticleData.hh"
#include "CLHEP/HepPDT/Constituent.hh"
#include "CLHEP/HepPDT/DefTable.hh"
#include "CLHEP/HepPDT/Measurement.hh"
#include "CLHEP/HepPDT/PDGtoQQTable.hh"
#include "CLHEP/HepPDT/ParticleID.hh"
#include "CLHEP/HepPDT/QQChannel.hh"
#include "CLHEP/HepPDT/QQDecay.hh"
#include "CLHEP/HepPDT/QQDecayTable.hh"
#include "CLHEP/HepPDT/ResonanceStructure.hh"
#include "CLHEP/HepPDT/SimpleDecayModel.hh"
#include "CLHEP/HepPDT/SimpleParticle.hh"
#include "CLHEP/HepPDT/SpinState.hh"

// functions declared in HepPDT but not defined
inline void HepPDT::QQDecayTable::writeTable(std::ostream& os) const { os << "1"; }
inline void HepPDT::QQChannel::write(std::ostream & os) const { os << "1"; }
inline void HepPDT::QQDecay::write(std::ostream & os) const { os << "1"; }
