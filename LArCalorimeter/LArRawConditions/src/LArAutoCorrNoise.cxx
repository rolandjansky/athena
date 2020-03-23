/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  const HWIdentifier hwid = m_larMCsym->ZPhiSymOfl( id );
  return m_autoCorrNoise.at( gain ).at( hwid );
}
