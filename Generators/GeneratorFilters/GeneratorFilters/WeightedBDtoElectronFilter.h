/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_WEIGHTEDBDTOELECTRONFILTER_H
#define GENERATORFILTERS_WEIGHTEDBDTOELECTRONFILTER_H

#include "GeneratorModules/GenFilter.h"

class TRandom3;


/// Require presence of electrons that have B hadrons as ancestors.
///
/// A binning in eta and pt may be chosen and prescale weights applied.
/// @author Jochen Hartert
class WeightedBDtoElectronFilter:public GenFilter {
public:

  WeightedBDtoElectronFilter(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode filterInitialize();
  StatusCode filterFinalize();
  StatusCode filterEvent();

private:

  /// Check if the particle has a B hadron as parent or ancestor
  HepMC::ConstGenParticlePtr FindBParent(HepMC::ConstGenParticlePtr part);
  /// Generate random number an check if electron passes the prescale weight
  bool PassPrescaleCheck( double etaAbs, double pt );

  /// @todo Move this lot into inline functions in the cxx file
  /// Convert long to string
  std::string longToStr(const long n) const;
  /// Check if PID is that of a B Baryon
  bool isBBaryon(const int pID) const;
  /// Check if PID is that of a B Meson
  bool isBMeson(const int pID) const;
  /// Check if PID is that of a B Hadron
  bool isBHadron(const int pID) const { return (isBBaryon(pID)||isBMeson(pID)); }
  /// Check if PID is that of a D Baryon
  bool isDBaryon(const int pID) const;
  /// Check if PID is that of a D Meson
  bool isDMeson(const int pID) const;
  /// Check if PID is that of a D Hadron
  bool isDHadron(const int pID) const { return (isDBaryon(pID)||isDMeson(pID)); }

  /// Random number generator
  TRandom3* m_rand;

  /// eta bin limits
  std::vector<double> m_etaBins;
  /// pt bin limits
  std::vector<double> m_ptBins;
  /// Prescale factors for all eta and pt bins
  std::vector<double> m_binPrescaleFactors;

  // Determined from boundaries of the above bins
  /// pt min cut for electrons
  double m_PtMin;
  /// pt max cut for electrons
  double m_PtMax;
  /// |eta| cut for electrons
  double m_EtaRange;

};

#endif
