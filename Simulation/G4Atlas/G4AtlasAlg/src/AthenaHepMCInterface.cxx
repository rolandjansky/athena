/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaHepMCInterface.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "FadsKinematics/FadsGeneratorT.h"
#include "FadsKinematics/GeneratorCenter.h"
#include "TruthHepMCEventConverter.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "StoreGate/ReadHandle.h"

FADS::FadsGeneratorT<AthenaHepMCInterface> gen("AthenaHepMCInterface");

AthenaHepMCInterface::AthenaHepMCInterface(std::string s)
  : FADS::FadsGenerator(s),
   m_msg("AthenaHepMCInterface")
{
  FADS::GeneratorCenter::GetGeneratorCenter()->DeleteHepMCEvent(false);
  FADS::GeneratorCenter::GetGeneratorCenter()->
    SetHepMCEventConverter(new TruthHepMCEventConverter);
}
AthenaHepMCInterface::~AthenaHepMCInterface()
{
  ATH_MSG_DEBUG("\b the AthenaHepMCInterface is being destroyed "<<"\n"
        << " should you wish to use it again type: "<<"\n"
                << " /Generator/Select AthenaHepMCInterface "<<"\n");
}
void AthenaHepMCInterface::Initialize()
{}
void AthenaHepMCInterface::Terminate()
{}
const
HepMC::GenEvent* AthenaHepMCInterface::GenerateAnEvent()
{
  SG::ReadHandle<McEventCollection> mcCollptr("BeamTruthEvent");//"GEN_EVENT");
  if (!mcCollptr.isValid())
    {
      ATH_MSG_WARNING("No McEventCollection found.");
      return nullptr;
    }
  if (0==mcCollptr->size())
    {
      ATH_MSG_WARNING("No GenEvents in McEventCollection!");
      return nullptr;
    }
  McEventCollection::const_iterator it = mcCollptr->begin();
  // getting only the first event here.
  const HepMC::GenEvent *p_evt = (*it);
  return p_evt;
}
