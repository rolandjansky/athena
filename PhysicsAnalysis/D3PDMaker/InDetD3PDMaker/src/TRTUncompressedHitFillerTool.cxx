/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TRTUncompressedHitFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "AtlasHepMC/GenParticle.h"
#include "GeoAdaptors/GeoTRTUncompressedHit.h"

D3PD::TRTUncompressedHitFillerTool::TRTUncompressedHitFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<TRTUncompressedHit> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode D3PD::TRTUncompressedHitFillerTool::initialize(){

  CHECK( Base::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode D3PD::TRTUncompressedHitFillerTool::book(){
  CHECK( addVariable("hitID",            m_hitID           ) );
  CHECK( addVariable("particleEncoding", m_particleEncoding) );
  CHECK( addVariable("kineticEnergy",    m_kineticEnergy   ) );
  CHECK( addVariable("energyDeposit",    m_energyDeposit   ) );
  CHECK( addVariable("preStepX",         m_preStepX        ) );
  CHECK( addVariable("preStepY",         m_preStepY        ) );
  CHECK( addVariable("preStepZ",         m_preStepZ        ) );
  CHECK( addVariable("postStepX",        m_postStepX       ) );
  CHECK( addVariable("postStepY",        m_postStepY       ) );
  CHECK( addVariable("postStepZ",        m_postStepZ       ) );
  CHECK( addVariable("globalTime",       m_globalTime      ) );

  CHECK( addVariable("globalPosX",       m_globalPosX      ) );
  CHECK( addVariable("globalPosY",       m_globalPosY      ) );
  CHECK( addVariable("globalPosZ",       m_globalPosZ      ) );
  CHECK( addVariable("globalPosR",       m_globalPosR      ) );

  CHECK( addVariable("TruthBarcode",     m_truthbarcode    ) );
  CHECK( addVariable("TruthBarcode2",    m_truthbarcode2   ) );

  return StatusCode::SUCCESS;
}


/*

StatusCode D3PD::TRTUncompressedHitFillerTool::fill (const TRTUncompressedHit& hit)
{

  //REPORT_MESSAGE (MSG::DEBUG) << "got hit";

  // read out the TRTUncompressedHit
  *m_hitID            = hit.GetHitID();
  *m_particleEncoding = hit.GetParticleEncoding();
  *m_kineticEnergy    = hit.GetKineticEnergy();
  *m_energyDeposit    = hit.GetEnergyDeposit();
  *m_preStepX         = hit.GetPreStepX();
  *m_preStepY         = hit.GetPreStepY();
  *m_preStepZ         = hit.GetPreStepZ();
  *m_postStepX        = hit.GetPostStepX();
  *m_postStepY        = hit.GetPostStepY();
  *m_postStepZ        = hit.GetPostStepZ();
  *m_globalTime       = hit.GetGlobalTime();

  // resolve global position
  GeoTRTUncompressedHit ghit(hit);
  const HepGeom::Point3D<double>& globalPos = ghit.getGlobalPosition();
  *m_globalPosX = globalPos.x();
  *m_globalPosY = globalPos.y();
  *m_globalPosZ = globalPos.z();
  *m_globalPosR = globalPos.perp();

  // particle barcode2 directly from TRTUncompressedHit
  const HepMcParticleLink& mcLink = hit.particleLink();
  *m_truthbarcode2    = mcLink.barcode();

  // particle barcode from associated GenParticle
  const HepMC::GenParticle* genPart = mcLink.cptr();
  *m_truthbarcode = genPart ? genPart->barcode() : -999;

  return StatusCode::SUCCESS;
}


*/

StatusCode D3PD::TRTUncompressedHitFillerTool::fill (const TRTUncompressedHit& /*hit*/)
{

  //REPORT_MESSAGE (MSG::DEBUG) << "got hit";

  // read out the TRTUncompressedHit
  *m_hitID            = 1;
  *m_particleEncoding = 1;
  *m_kineticEnergy    = 1.0;
  *m_energyDeposit    = 1.0;
  *m_preStepX         = 1.0;
  *m_preStepY         = 1.0;
  *m_preStepZ         = 1.0;
  *m_postStepX        = 1.0;
  *m_postStepY        = 1.0;
  *m_postStepZ        = 1.0;
  *m_globalTime       = 1.0;

  // resolve global position
  *m_globalPosX = 1.0;
  *m_globalPosY = 1.0;
  *m_globalPosZ = 1.0;
  *m_globalPosR = 1.0;

  // particle barcode2 directly from TRTUncompressedHit
  *m_truthbarcode2    = -999;

  // particle barcode from associated GenParticle
  *m_truthbarcode = -999;

  return StatusCode::SUCCESS;
}
