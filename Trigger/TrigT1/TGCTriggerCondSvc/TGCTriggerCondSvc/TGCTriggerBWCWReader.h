/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCTRIGGERBWCWREADER_H
#define TGCTRIGGERBWCWREADER_H

#include "TGCTriggerCondSvc/TGCTriggerLUTReader.h"

class TGCTriggerLUTs;
class CondAttrListCollection;

class TGCTriggerBWCWReader: public TGCTriggerLUTReader{

 public:
  TGCTriggerBWCWReader(int lutType);
  ~TGCTriggerBWCWReader() = default;
 
  bool readLUT(TGCTriggerLUTs* writeCdo) override;
  bool loadParameters(TGCTriggerLUTs* writeCdo,
                      const CondAttrListCollection* readKey) override;
 private:
  enum {TMap_HH=0, TMap_HL, TMap_LH, TMap_LL, N_TMap};
  int getType(int lDR, int hDR, int lDPhi, int hDPhi) const;
  int subSectorAdd(int ssid, int modid, int phimod2, int type) const;
};


#endif
