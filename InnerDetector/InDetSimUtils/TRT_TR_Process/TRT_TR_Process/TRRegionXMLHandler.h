/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRREGIONXMLHANDLER_H
#define TRREGIONXMLHANDLER_H

#include "FadsXMLParser/DescriptionFactory.h"
#include <string>
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/MsgStreamMember.h"

class TRTTransitionRadiation;

class TRRegionXMLHandler: public FADS::DescriptionFactory {
 public:
  TRRegionXMLHandler(std::string s, TRTTransitionRadiation*);
  void BuildDescription();
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }
  bool msgLevel (MSG::Level lvl)    { return m_msg.get().level() <= lvl; }

 private:
  TRTTransitionRadiation * m_theProcess;
  StoreGateSvc* m_storeGate;
  bool m_initialLayoutIdDict;
  mutable Athena::MsgStreamMember m_msg;
};

#endif
