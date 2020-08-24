// -*- C++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/*! \file Herwig7HepMC.cxx
 *  \brief Implementation of the Herwig7 HepMC interface.
 *  \author Andrii.verbytskyi@mpp.mpg.de
 */


#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "ThePEG/EventRecord/Event.h"
#include "ThePEG/EventRecord/SubProcess.h"
#include "ThePEG/Handlers/XComb.h"
#include "ThePEG/Handlers/EventHandler.h"
#include "ThePEG/PDF/PDF.h"

#include "Herwig/API/HerwigAPI.h"



#include "ThePEG/Vectors/HepMCConverter.h"
#ifdef HWVER_IS_72
namespace ThePEG {
  template<>
  struct HepMCTraits<HepMC::GenEvent>
    : public HepMCTraitsBase<HepMC::GenEvent,
                             HepMC::GenParticle,
                             HepMC::GenParticle *,
                             HepMC::GenVertex,
                             HepMC::GenVertex *,
                             HepMC::Polarization,
                             HepMC::PdfInfo>
  {
    static bool hasUnits() {
      return true;
    }
  };
}
#else
namespace ThePEG {
  template<>
  struct HepMCTraits<HepMC::GenEvent>
    : public HepMCTraitsBase<HepMC::GenEvent,
                             HepMC::GenParticle,
                             HepMC::GenVertex,
                             HepMC::Polarization,
                             HepMC::PdfInfo>
  {
    static bool hasUnits() {
      return true;
    }
  };
}
#endif
  
void   convert_to_HepMC(const ThePEG::Event & event, HepMC::GenEvent & evt, bool nocopies,ThePEG::Energy eunit, ThePEG::Length lunit){
  ThePEG::HepMCConverter<HepMC::GenEvent>::convert(event, evt, nocopies, eunit, lunit);
}
