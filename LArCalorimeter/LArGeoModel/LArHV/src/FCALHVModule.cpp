/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/FCALHVModule.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVLine.h"

#include <sstream>
#include <stdexcept>

class FCALHVModule::Clockwork {
public:

  const FCALHVManager *manager;
  unsigned int iSide;
  unsigned int iSector;
  unsigned int iSampling;
  FCALHVLineConstLink hvLine[4];
};



//##ModelId=47ABB2AD00A9
FCALHVModule::FCALHVModule(const FCALHVManager *manager, unsigned int iSide, unsigned int iSector, unsigned int iSampling):c(new Clockwork())
{
  c->manager=manager;
  c->iSide=iSide;
  c->iSector=iSector;
  c->iSampling=iSampling;
}

//##ModelId=47ABB2AD00B2
unsigned int FCALHVModule::getSideIndex() const
{
  return c->iSide;
}

//##ModelId=47ABB2AD00B4
unsigned int FCALHVModule::getSamplingIndex() const
{
  return c->iSampling;
}

//##ModelId=47ABB2AD00B6
unsigned int FCALHVModule::getSectorIndex() const
{
  return c->iSector;
}

//##ModelId=47ABB2AD00B8
unsigned int FCALHVModule::getNumHVLines() const
{
  return 4;
}

//##ModelId=47ABB2AD00BA
FCALHVLineConstLink FCALHVModule::getHVLine(unsigned int iLine) const
{
  // Check bounds and throw error if out of range.
  if (iLine>3) {
    std::ostringstream msg;
    msg << "FCALHVModule requesting out of range HV line, number " << iLine;
    throw std::runtime_error(msg.str().c_str());
  }

  if (!c->hvLine[iLine]) c->hvLine[iLine]= FCALHVLineConstLink(new FCALHVLine(this,iLine));
  return c->hvLine[iLine];
}

//##ModelId=47ABB2AD00BD
FCALHVModule::~FCALHVModule()
{
  delete c;
}


const FCALHVManager *FCALHVModule::getManager() const
{
  return c->manager;
}
