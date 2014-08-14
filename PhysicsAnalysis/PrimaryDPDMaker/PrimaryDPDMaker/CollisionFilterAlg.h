// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_CollisionFilterAlg_H
#define PrimaryDPDMaker_CollisionFilterAlg_H

#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

/////////////////////////////////////////////////////////////////////////////

class CaloTimeFilterTool;
class MBTSTimeFilterTool;

class CollisionFilterAlg : public AthFilterAlgorithm 
{
 public:
  CollisionFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CollisionFilterAlg(); 

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:

  ToolHandle<CaloTimeFilterTool> m_calofilter;
  ToolHandle<MBTSTimeFilterTool> m_mbtsfilter;

  unsigned int m_prescale;
  unsigned int m_counter;
  unsigned int m_caloaccept;
  unsigned int m_mbtsaccept;
  unsigned int m_overlap;

  bool m_doCaloTimeFilter;
  bool m_doMBTSTimeFilter;
};

#endif // PrimaryDPDMaker_CollisionFilterAlg_H

