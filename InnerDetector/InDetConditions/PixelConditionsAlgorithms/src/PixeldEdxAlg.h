/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsAlgorithms/PixeldEdxAlg.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date Octorber, 2019
 * @brief Created pixel dEdx data in PixeldEdxData.
 */


#ifndef PIXELDEDXALG_H
#define PIXELDEDXALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "StoreGate/WriteCondHandleKey.h"
#include "PixelConditionsData/PixeldEdxData.h"

#include "InDetIdentifier/PixelID.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

class PixeldEdxAlg : public AthAlgorithm {  
  public:
    PixeldEdxAlg(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~PixeldEdxAlg() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:

    const PixelID* m_pixelID;

    Gaudi::Property<std::string> m_filename
    {this, "CalibrationFile", "mcpar_signed_234.txt","Read dEdx from this file"}; 

    Gaudi::Property<bool> m_readfromcool
    {this, "ReadFromCOOL", false, "Read from COOL database"}; 

    Gaudi::Property<double> m_mindedxformass
    {this, "MinimumdEdxForMass", 1.8, "Minimum dEdx for mass"};

    ServiceHandle<ICondSvc> m_condSvc
    {this, "CondSvc", "CondSvc", ""};

    SG::ReadCondHandleKey<AthenaAttributeList> m_readKey
    {this, "ReadKey", "/PIXEL/PixdEdx", "Input readout folder"};

    SG::WriteCondHandleKey<PixeldEdxData> m_writeKey
    {this, "WriteKey", "PixeldEdxData", "Output data"};

};

#endif
