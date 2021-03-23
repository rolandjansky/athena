/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArAutoCorrNoise.h"

LArAutoCorrNoise::LArAutoCorrNoise( std::vector<std::map<HWIdentifier, std::vector<float>>>&& input,
                                    const LArMCSym* larMCsym )
  : m_larMCsym( larMCsym ),
    m_autoCorrNoise (std::move(input))
{
}

LArAutoCorrNoise::~LArAutoCorrNoise() {}

const std::vector<float>& LArAutoCorrNoise::autoCorrSqrt( const Identifier& id, int gain ) const
{
  HWIdentifier hwid(0);
  if ( m_larMCsym ) hwid = m_larMCsym->ZPhiSymOfl( id );
  // Not sure what to do with this method when no larMCsym is available
  return m_autoCorrNoise.at( gain ).at( hwid );
}

const std::vector<float>& LArAutoCorrNoise::autoCorrSqrt( const HWIdentifier& hwid, int gain ) const
{
  return m_autoCorrNoise.at( gain ).at( hwid );
}
