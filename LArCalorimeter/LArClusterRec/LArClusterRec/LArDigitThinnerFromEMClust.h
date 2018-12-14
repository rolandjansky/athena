/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @class LArDigitsThinner
 * @brief Algorithm to copy a selected subset of LArDigits
 * 
 */


#ifndef LARCLUSTERREC_LArDigitThinnerFromEMClust_H
#define LARCLUSTERREC_LArDigitThinnerFromEMClust_H

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "LArIdentifier/LArOnlineID.h"

#include <bitset>

#include <vector>
#include <string>

#include "xAODCaloEvent/CaloClusterContainer.h"
#include "AthContainers/ConstDataVector.h"
class HWIdentifier;
class Identifier;

class LArDigitContainer;
class LArOnOffIdMapping;

class LArDigitThinnerFromEMClust : public AthReentrantAlgorithm
{
 public:
  
  LArDigitThinnerFromEMClust(const std::string& name,
			     ISvcLocator* pSvcLocator);

  ~LArDigitThinnerFromEMClust();
  
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
 
  private:
    
    const LArOnlineID* m_onlineID;

    SG::ReadHandleKey<LArDigitContainer> m_inputContainerKey;
    SG::WriteHandleKey<ConstDataVector<LArDigitContainer> > m_outputContainerKey;
    SG::ReadHandleKey<xAOD::CaloClusterContainer> m_clusterContainerKey;

    SG::ReadCondHandleKey<LArOnOffIdMapping> m_larCablingKey;

    //size_t m_nchannels;

    //unsigned int m_nevent;
    //unsigned int m_ncells;

};

#endif
