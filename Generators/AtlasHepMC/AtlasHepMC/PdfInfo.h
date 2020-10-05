/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Author: Andrii Verbytskyi andrii.verbytskyi@mpp.mpg.de */

#ifndef ATLASHEPMC_PDFINFO_H
#define ATLASHEPMC_PDFINFO_H
#ifdef HEPMC3
#include "HepMC3/GenEvent.h"
#include "HepMC3/PrintStreams.h"
namespace HepMC
{
typedef std::shared_ptr<HepMC3::GenPdfInfo>  GenPdfInfoPtr;
}
#else
#include "HepMC/PdfInfo.h"
namespace HepMC
{
typedef HepMC::PdfInfo*  GenPdfInfoPtr;
}
#endif
#endif
