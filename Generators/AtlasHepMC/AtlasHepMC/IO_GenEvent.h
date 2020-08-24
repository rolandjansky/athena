/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_IOGENEVENT_H
#define ATLASHEPMC_IOGENEVENT_H
#ifdef HEPMC3
#include "HepMC3/Version.h"
#include "HepMC3/Reader.h"
#include "HepMC3/Writer.h"
#include "HepMC3/ReaderAsciiHepMC2.h"
#include "HepMC3/WriterAsciiHepMC2.h"
namespace HepMC
{
typedef HepMC3::WriterAsciiHepMC2   WriterAsciiHepMC2;
typedef HepMC3::ReaderAsciiHepMC2   ReaderAsciiHepMC2;
}
#else
#include "HepMC/IO_GenEvent.h"
#endif
#endif
