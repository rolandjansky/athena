/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRREGIONXMLHANDLER_H
#define TRREGIONXMLHANDLER_H

#include <string>
#include "StoreGate/StoreGateSvc.h"
#include "AthenaBaseComps/AthMessaging.h"

class TRTTransitionRadiation;

class TRRegionXMLHandler : public AthMessaging {
public:
  TRRegionXMLHandler(TRTTransitionRadiation*);
  void Process(const std::string& name);

private:
  TRTTransitionRadiation * m_theProcess;
  StoreGateSvc* m_storeGate;
  bool m_initialLayoutIdDict;
};

#endif
