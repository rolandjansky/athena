//Dear emacs, this is -*-c++-*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARCALIBLINEMAPPING_
#define LARRECCONDITIONS_LARCALIBLINEMAPPING_

#include "Identifier/HWIdentifier.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include <vector>

class LArOnlineID;

class LArCalibLineMapping {

  friend class LArCalibLineMappingAlg; //The conditions alg filling this object

 public:
  LArCalibLineMapping()=delete;
  LArCalibLineMapping(const LArOnlineID* onlineId);

  const std::vector<HWIdentifier>& calibSlotLine(const HWIdentifier id) const;
  const std::vector<HWIdentifier>& calibSlotLineFromHash(const IdentifierHash h) const;
  
 private:
  std::vector<std::vector<HWIdentifier> > m_onlHashToCalibLines;
  const LArOnlineID* m_onlineId ;

};


//Inline methods:
inline const std::vector<HWIdentifier>& LArCalibLineMapping::calibSlotLineFromHash(const IdentifierHash h) const{
  return m_onlHashToCalibLines[h];
}


#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF( LArCalibLineMapping,142865765 , 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArCalibLineMapping>, 30856397 , 1)

#endif
