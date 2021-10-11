/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCGoodMF_h
#define TGCGoodMF_h

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "GaudiKernel/MsgStream.h"

#include <map>

namespace LVL1TGCTrigger {

class TGCArguments;

class TGCGoodMF {
public:
  TGCGoodMF(TGCArguments*,const std::string& version);
  ~TGCGoodMF() = default;

  TGCGoodMF(const TGCGoodMF& right);
  const TGCGoodMF& operator=(const TGCGoodMF& right);

  bool test_GoodMF(int moduleId, int sscId, int subsector) const;
  bool readBadMFList();

  const TGCArguments* tgcArgs() const { return m_tgcArgs; }

private:
  std::map<int, std::map<int,int> > m_mapisgoodMF; //Number of moduleId
  TGCArguments* m_tgcArgs;
  std::string m_verName;

};

} //end of namespace bracket

#endif // TGCGoodMF_h
