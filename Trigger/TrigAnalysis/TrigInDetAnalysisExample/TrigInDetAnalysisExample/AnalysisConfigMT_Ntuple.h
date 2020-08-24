/* emacs: this is -*- c++ -*- */
/**
 **     @file    AnalysisConfigMT_Ntuple.h
 **
 **              Ehis is the MT analysis class - we really don;t want to 
 **              have to duplicate everything so we instead just inherit 
 **              from the normal class and just implement the new loop 
 **              method 
 **
 **
 **     @author  mark sutton
 **     @date    Monday 18th May  2020 22:55:37 BST 
 **
 **     Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef TrigInDetAnalysisExample_AnalysisConfigMT_Ntuple_H
#define TrigInDetAnalysisExample_AnalysisConfigMT_Ntuple_H

#include "TrigInDetAnalysisExample/AnalysisConfig_Ntuple.h"


class AnalysisConfigMT_Ntuple : public AnalysisConfig_Ntuple { 
    
public:
    
    // Full constructor: test/reference/selection
    // - analysisInstanceName: the name of the analysis chain being created
    // - xxxChainName: the name of the chain to be used as test/reference/selection; must be "StoreGate" in case of direct access to SG containers
    // - xxxType: the type of tracks to be retrieved from the test/reference/selection chain or container
    // - xxxKey:  the key for tracks to be retrieved from the test/reference/selection chain or container
    // - roiInfo: in case the test chain is a real chain, this is used to specify RoI widths; in case the test chain is a fake chain, this is used 
    //   for RoI position too
    // - all standard operations are performed in loops over 0=test 1=reference 2=selection

  AnalysisConfigMT_Ntuple( TIDARoiDescriptor* roiInfo, 
			   const std::vector<std::string>& chainNames, std::string outputFileName="TrkNtuple.root", 
			   double tauEtCutOffline=0.0, int TruthPdgId = 0, bool keepAllEvents_=false ) : 
    AnalysisConfig_Ntuple( roiInfo, chainNames, outputFileName, tauEtCutOffline, TruthPdgId, keepAllEvents_ ),
    m_fiducial_radius(47)
  { }  

  virtual ~AnalysisConfigMT_Ntuple() { }

protected:

  virtual void loop();

private: 
  
  double m_fiducial_radius; 

};

  
#endif  // TrigInDetAnalysisExample_AnalysisConfigMT_Ntuple_H

