/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETOVERLAY_INDETSDOOVERLAY_H
#define INDETOVERLAY_INDETSDOOVERLAY_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetSimData/InDetSimDataCollection.h"

class InDetSDOOverlay : public AthAlgorithm
{
public:
  InDetSDOOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:
  SG::ReadHandleKey<InDetSimDataCollection> m_bkgInputKey{ this, "BkgInputKey", "", "ReadHandleKey for Background Input InDetSimDataCollection" };
  SG::ReadHandleKey<InDetSimDataCollection> m_signalInputKey{ this, "SignalInputKey", "", "ReadHandleKey for Signal Input InDetSimDataCollection" };
  SG::WriteHandleKey<InDetSimDataCollection> m_outputKey{ this, "OutputKey", "", "WriteHandleKey for Output InDetSimDataCollection" };

};

#endif // INDETOVERLAY_INDETSDOOVERLAY_H
