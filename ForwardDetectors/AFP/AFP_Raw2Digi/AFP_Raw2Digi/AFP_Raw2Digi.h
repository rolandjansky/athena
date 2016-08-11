/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RAW2DIGI_RAW2DIGI_H
#define RAW2DIGI_RAW2DIGI_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!


#include "AFP_DigiEv/AFP_SiDigi.h"
#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "AFP_RawEv/AFP_RawData.h"
#include "AFP_RawEv/AFP_RawDataCollection.h"
#include "AFP_RawEv/AFP_RawDataContainer.h"
#include <iostream>
#include <string>


class AFP_Raw2Digi : public ::AthAlgorithm {
public:
  AFP_Raw2Digi(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~AFP_Raw2Digi();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:

  std::string m_AFP_RawDataCollectionKey;
//  std::string m_SiDigiCollectionName;
std::string m_SiHitContainerName;
};

#endif //> !RAW2DIGI_RAW2DIGI_H
