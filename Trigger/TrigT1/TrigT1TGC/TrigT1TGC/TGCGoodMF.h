/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        TGCGoodMF.h
*/
// ====================================================================
#ifndef TGCGoodMF_h
#define TGCGoodMF_h

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/MsgStream.h"

#include <map>

namespace LVL1TGCTrigger {

class TGCArguments;

// ====================================================================
//
// class definition
//
// ====================================================================

class TGCGoodMF {
public:
  TGCGoodMF(TGCArguments*);
  ~TGCGoodMF() = default;

  TGCGoodMF(const TGCGoodMF& right);
  const TGCGoodMF& operator=(const TGCGoodMF& right);

  bool test_GoodMF(int moduleId, int sscId, int subsector) const;
  bool readBadMFList();

  TGCArguments* tgcArgs() const;

private:
  std::map<int, std::map<int,int> > m_mapisgoodMF; //Number of moduleId
  TGCArguments* m_tgcArgs;

};

inline 
TGCArguments* TGCGoodMF::tgcArgs() const {
  return m_tgcArgs;
}

} //end of namespace bracket

#endif // TGCGoodMF_h
