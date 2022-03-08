/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITKPIXELOFFLINECALIBCONDALG
#define ITKPIXELOFFLINECALIBCONDALG

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/ITkPixelOfflineCalibData.h"

#include "Gaudi/Property.h"

#include "StoreGate/StoreGateSvc.h"
#include "InDetIdentifier/PixelID.h"

namespace ITk
{

class PixelOfflineCalibCondAlg : public AthReentrantAlgorithm
{
  public:
    PixelOfflineCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixelOfflineCalibCondAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    Gaudi::Property<int> m_inputSource
    {this, "InputSource",2,"Source of data: 0 (none), 1 (text file), 2 (database)"};

    Gaudi::Property<std::string> m_textFileName
    {this, "DataFile", "ITkPixelClusterErrorData.txt","Read constants from this file"};

    Gaudi::Property<int> m_dump
    {this, "DumpConstants", 0, "Dump constants to text file"};

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey
    {this, "ReadKey", "/PIXEL/ITkClusterError", "Input key of pixreco conditions folder"};

    SG::WriteCondHandleKey<ITk::PixelOfflineCalibData> m_writeKey
    {this, "WriteKey", "ITkPixelOfflineCalibData", "Output key of pixel module data"};

    StoreGateSvc* m_detStore{nullptr};
    const PixelID* m_pixelid{nullptr};

};

} // namespace ITk

#endif
