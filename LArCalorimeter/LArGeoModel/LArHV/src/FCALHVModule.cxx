/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/FCALHVModule.h"
#include "LArHV/FCALHVManager.h"
#include "LArHV/FCALHVLine.h"

#include <stdexcept>

class FCALHVModule::Clockwork {
public:
  Clockwork(const FCALHVManager* managerPtr
	    , const FCALHVModule* modulePtr
	    , unsigned int side
	    , unsigned int sector
	    , unsigned int sampling)
    : manager(managerPtr)
    , iSide(side)
    , iSector(sector)
    , iSampling(sampling) {
    for(int iLine=0; iLine<4; ++iLine) {
      hvLine[iLine] = new FCALHVLine(modulePtr,iLine);
    }
  }
  ~Clockwork() {
    for(int iLine=0; iLine<4; ++iLine) {
      delete hvLine[iLine];
    }
  }

  const FCALHVManager *manager;
  unsigned int iSide;
  unsigned int iSector;
  unsigned int iSampling;
  const FCALHVLine* hvLine[4];
};

FCALHVModule::FCALHVModule(const FCALHVManager* manager
			   , unsigned int iSide
			   , unsigned int iSector
			   , unsigned int iSampling)
  :m_c(new Clockwork(manager,this,iSide,iSector,iSampling))
{
}

FCALHVModule::~FCALHVModule()
{
  delete m_c;
}

unsigned int FCALHVModule::getSideIndex() const
{
  return m_c->iSide;
}

unsigned int FCALHVModule::getSamplingIndex() const
{
  return m_c->iSampling;
}

unsigned int FCALHVModule::getSectorIndex() const
{
  return m_c->iSector;
}

unsigned int FCALHVModule::getNumHVLines() const
{
  return 4;
}

const FCALHVLine& FCALHVModule::getHVLine(unsigned int iLine) const
{
  // Check bounds and throw error if out of range.
  if (iLine>3) {
    std::string msg = std::string("FCALHVModule requesting out of range HV line, number ") + std::to_string(iLine);
    throw std::runtime_error(msg.c_str());
  }

  return *(m_c->hvLine[iLine]);
}

const FCALHVManager& FCALHVModule::getManager() const
{
  return *(m_c->manager);
}
