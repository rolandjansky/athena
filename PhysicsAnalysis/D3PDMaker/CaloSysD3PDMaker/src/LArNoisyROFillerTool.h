/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Laurent Duflot <duflot at lal.in2p3.fr>
 * @brief Block filler tool for noisy FEB information
*/

#ifndef LArNoisyROFillerTool_h
#define LArNoisyROFillerTool_h


#include <vector>

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "LArRecEvent/LArNoisyROSummary.h"
//class LArNoisyROSummary;


namespace D3PD
{

class LArNoisyROFillerTool : 
    public D3PD::BlockFillerTool<LArNoisyROSummary>
{

 public:
  LArNoisyROFillerTool(const std::string& type,
		       const std::string& name,
		       const IInterface* parent);

  /// Book variables for this block.
  virtual StatusCode book();

  /// fill variables
  virtual StatusCode fill (const LArNoisyROSummary& c);

 private:

  bool m_SaveNB;

  int* m_nNoisyFEB;
  bool m_SaveFEBIDs;
  std::vector<unsigned long>* m_NoisyFEBIDs;

  int* m_nNoisyPA;
  bool m_SavePAIDs;
  std::vector< std::pair<unsigned long, std::vector<int> > >* m_NoisyPAIDs;
    
};

}

#endif
