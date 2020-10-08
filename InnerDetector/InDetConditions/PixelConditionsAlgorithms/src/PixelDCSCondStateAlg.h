/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/ 
/**
 * @file PixelConditionsAlgorithms/PixelDCSCondStatusAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Created pixel DCS module status in PixelDCSStatusData.
 */

#ifndef PIXELDCSCONDSTATEALG
#define PIXELDCSCONDSTATEALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixelDCSStateData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixelDCSCondStateAlg : public AthReentrantAlgorithm {  
  public:
    PixelDCSCondStateAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelDCSCondStateAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    const PixelID* m_pixelID{nullptr};

    ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyState
    {this, "ReadKeyState", "",    "Key of input DCS state conditions folder"};

    SG::WriteCondHandleKey<PixelDCSStateData> m_writeKeyState 
    {this, "WriteKeyState", "PixelDCSStateCondData",  "Key of output DCS state data"};

};

#endif
