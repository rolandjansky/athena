/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_CollisionFilterAlg_H
#define PrimaryDPDMaker_CollisionFilterAlg_H


#include "PrimaryDPDMaker/ICaloTimeFilterTool.h"
#include "PrimaryDPDMaker/IMBTSTimeFilterTool.h"


#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////


class CollisionFilterAlg : public AthReentrantAlgorithm 
{
 public:
  CollisionFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CollisionFilterAlg()=default; 

  virtual StatusCode initialize() override ;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 private:

  ToolHandle<ICaloTimeFilterTool> m_calofilter;
  ToolHandle<IMBTSTimeFilterTool> m_mbtsfilter;

  SG::ReadHandleKey<TileCellContainer>  m_MBTS_key{this, "MBTSContainerKey", "MBTSContainer", ""};
  SG::ReadHandleKey<LArCollisionTime>   m_LArTimeKey{this, "LARCollisionTimeKey","LArCollisionTime" , "" };
 
  
  Gaudi::Property<unsigned int> m_prescale{this, "CounterPrescale",  1000};
  Gaudi::Property<bool> m_doCaloTimeFilter{this, "DoCaloTimeFilter" , true};
  Gaudi::Property<bool> m_doMBTSTimeFilter{this, "DoMBTSTimeFilter" , true};
  

  mutable std::atomic<unsigned int> m_counter{0};
 
  mutable std::atomic<unsigned int> m_caloaccept{0};
  mutable std::atomic<unsigned int> m_mbtsaccept{0};
   mutable std::atomic<unsigned int> m_overlap{0};

};

#endif // PrimaryDPDMaker_CollisionFilterAlg_H

