/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IHFORTOOL_H
#define IHFORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include <vector>

// Forward declaration
namespace HepMC {class FourVector ;}

static const InterfaceID IID_IHforTool("IHforTool", 1, 0);


/// @brief Tool interface for post-hoc heavy flavor overlap removal
/// @author Dominic Hirschbuehl
/// @author Michiel Sanders
/// @todo Why is an interface needed?
class IHforTool : virtual public IAlgTool {
public:

  static const InterfaceID& interfaceID() { return IID_IHforTool; }
  virtual StatusCode initialize() = 0;
  virtual StatusCode execute() = 0;

  /// @name Functions to retrieve the b/c quarks
  //@{
  virtual const std::vector<HepMC::FourVector> & get_bQuarks_MPI() = 0;
  virtual const std::vector<HepMC::FourVector> & get_bQuarks_GS() = 0;
  virtual const std::vector<HepMC::FourVector> & get_bQuarks_ME() = 0;
  virtual const std::vector<HepMC::FourVector> & get_bQuarks_MEin() = 0;
  virtual const std::vector<HepMC::FourVector> & get_bQuarks_topdecay() = 0;
  virtual const std::vector<HepMC::FourVector> & get_bQuarks_PDF() = 0;
  virtual const std::vector<HepMC::FourVector> & get_bQuarks_unknown() = 0;
  virtual const std::vector<HepMC::FourVector> & get_cQuarks_MPI() = 0;
  virtual const std::vector<HepMC::FourVector> & get_cQuarks_GS() = 0;
  virtual const std::vector<HepMC::FourVector> & get_cQuarks_ME() = 0;
  virtual const std::vector<HepMC::FourVector> & get_cQuarks_MEin() = 0;
  virtual const std::vector<HepMC::FourVector> & get_cQuarks_topdecay() = 0;
  virtual const std::vector<HepMC::FourVector> & get_cQuarks_PDF() = 0;
  virtual const std::vector<HepMC::FourVector> & get_cQuarks_unknown() = 0;
  //@}

  virtual std::string getDecision(const std::string& schema="") = 0;

};


#endif
