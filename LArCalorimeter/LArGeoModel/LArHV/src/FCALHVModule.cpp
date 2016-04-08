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
FCALHVModule::FCALHVModule(const FCALHVManager *manager, unsigned int iSide, unsigned int iSector, unsigned int iSampling):m_c(new Clockwork())
{
  m_c->manager=manager;
  m_c->iSide=iSide;
  m_c->iSector=iSector;
  m_c->iSampling=iSampling;
}

//##ModelId=47ABB2AD00B2
unsigned int FCALHVModule::getSideIndex() const
{
  return m_c->iSide;
}

//##ModelId=47ABB2AD00B4
unsigned int FCALHVModule::getSamplingIndex() const
{
  return m_c->iSampling;
}

//##ModelId=47ABB2AD00B6
unsigned int FCALHVModule::getSectorIndex() const
{
  return m_c->iSector;
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

  if (!m_c->hvLine[iLine]) m_c->hvLine[iLine]= FCALHVLineConstLink(new FCALHVLine(this,iLine));
  return m_c->hvLine[iLine];
}

//##ModelId=47ABB2AD00BD
FCALHVModule::~FCALHVModule()
{
  delete m_c;
}


const FCALHVManager *FCALHVModule::getManager() const
{
  return m_c->manager;
}
