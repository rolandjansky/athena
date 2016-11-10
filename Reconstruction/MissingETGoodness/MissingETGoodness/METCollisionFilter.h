// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MissingETGoodness_METCollisionFilter_H
#define MissingETGoodness_METCollisionFilter_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "MissingETGoodness/Goodies.h"
#include "CaloEvent/CaloClusterContainer.h"

#include <string>
#include <vector>

/////////////////////////////////////////////////////////////////////////////

class CaloTimeFilterTool;
class MBTSTimeFilterTool;
class IGoodRunsListSelectorTool;

class METCollisionFilter : public AthAlgorithm 
{
 public:
  METCollisionFilter( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~METCollisionFilter(); 

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  StatusCode doClusterTimes(bool& passCut);

  // MB: reference to MET Goodies map for storing derived met goodness quantities
  static MET::Goodies& s_goodies;

  unsigned int m_prescale;
  unsigned int m_counter;
  unsigned int m_caloaccept;
  unsigned int m_mbtsaccept;
  unsigned int m_caloclusteraccept;
  unsigned int m_overlap;

  double m_clusterMinEta;
  double m_clusterMinE;
  double m_clusterTimeMean;
  double m_clusterTimeMeanDelta;
  double m_clusterTimeACDelta;

  bool m_doLBFilter;
  bool m_doCaloTimeFilter;
  bool m_doCaloClusterTimeFilter;
  bool m_doMBTSTimeFilter;

  std::string m_prefix;
  std::vector<std::string> m_grlnameVec;
  std::vector<std::string> m_brlnameVec;

  ToolHandle<CaloTimeFilterTool> m_calofilter;
  ToolHandle<MBTSTimeFilterTool> m_mbtsfilter;
  ToolHandle< IGoodRunsListSelectorTool > m_GoodRunsListSelectorTool;

};

#endif // MissingETGoodness_METCollisionFilter_H

