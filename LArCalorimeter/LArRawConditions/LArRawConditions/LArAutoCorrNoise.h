/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARAUTOCORRNOISE
#define LARRAWCONDITIONS_LARAUTOCORRNOISE

#include <vector>

#include "LArRawConditions/LArMCSym.h"

class LArAutoCorrNoise
{

public:
  LArAutoCorrNoise() = delete;

  LArAutoCorrNoise( std::vector<std::map<HWIdentifier, std::vector<float>>>&& input, const LArMCSym* larMCsym );

  ~LArAutoCorrNoise();

  const std::vector<float>& autoCorrSqrt( const HWIdentifier& id, int gain ) const;
  const std::vector<float>& autoCorrSqrt( const Identifier& id, int gain ) const;

private:
  const LArMCSym*                                         m_larMCsym;
  std::vector<std::map<HWIdentifier, std::vector<float>>> m_autoCorrNoise;
};

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArAutoCorrNoise, 224426548, 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( LArAutoCorrNoise, 107335812 );

#endif
