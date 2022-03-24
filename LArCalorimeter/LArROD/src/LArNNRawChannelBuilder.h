/*
   Copyright (C) 2022 CERN for the benefit of the ATLAS collaboration
 */

#ifndef LARNNRAWCHANNELBUILDER_H
#define LARNNRAWCHANNELBUILDER_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadCondHandle.h"

#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

#include "LArElecCalib/ILArPedestal.h"
#include "LArRawConditions/LArADC2MeV.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include <vector>


//Event classes
class LArDigitContainer;
class LArRawChannelContainer;
class CaloCellContainer;
class CaloSuperCellDetDescrManager;

class LArOnlineID_Base;


class LArNNRawChannelBuilder : public AthReentrantAlgorithm {

public:
LArNNRawChannelBuilder(const std::string& name, ISvcLocator* pSvcLocator);

virtual StatusCode initialize() override;
virtual StatusCode execute(const EventContext& ctx) const override;


private:
//Event input:
SG::ReadHandleKey<LArDigitContainer>m_digitKey{this, "LArDigitKey", "FREE",
                                               "SG Key of LArDigitContaiiner"};
//Event output:
SG::WriteHandleKey<LArRawChannelContainer>m_rawChannelKey{this, "LArRawChannelKey", "LArRawChannels",
                                                          "SG key of the output LArRawChannelContainer"};
SG::WriteHandleKey<CaloCellContainer>m_cellKey{this, "CaloCellKey", "SCellnoBCID",
                                               "SG key of the output CaloCellContainer"};

//Conditions input:
SG::ReadCondHandleKey<ILArPedestal>m_pedestalKey{this, "PedestalKey", "LArPedestal", "SG Key of Pedestal conditions object"};
SG::ReadCondHandleKey<LArADC2MeV>m_adc2MeVKey{this, "ADC2MeVKey", "LArADC2MeV", "SG Key of ADC2MeV conditions object"};

SG::ReadCondHandleKey<LArOnOffIdMapping>m_cablingKey{this, "CablingKey", "LArOnOffIdMap", "SG Key of LArOnOffIdMapping object"};


//The following matters only in the MC case, when we have a 32 sample shapes
Gaudi::Property<int>m_firstSample{this, "firstSample", 0, "first of the 32 sampels of the MC shape to be used"};

// Use the code for SuperCells
Gaudi::Property<bool>m_isSC{this, "IsSuperCell", false, "code should produce SuperCells"};

//Identifier helper
const LArOnlineID_Base* m_onlineId = nullptr;

/// Geometry manager.
const CaloSuperCellDetDescrManager* m_sem_mgr = 0;

Gaudi::Property<std::string>m_nn_json{this, "NNJsonPath", "",
                                              "Path to json containing the lwtnn network"};
Gaudi::Property<std::string>m_input_node{this, "NetworkInputNode", "",
"Name of the input node"};
Gaudi::Property<std::string>m_network_output{this, "NetworkOutputNode", "",
"Name of the output node"};

};


#endif
