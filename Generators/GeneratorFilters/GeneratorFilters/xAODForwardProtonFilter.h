/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_XAODFORWARDPROTONFILTER_H
#define GENERATORFILTERS_XAODFORWARDPROTONFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"



/// Description: Filter based on kinematics of forward proton
/// @author Rafal Staszewski, Jul 2011
class xAODForwardProtonFilter : public GenFilter {
public:

  xAODForwardProtonFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterEvent() override;

private:
  Gaudi::Property<double> m_xiMin{this,"xi_min",0.01," "};
  Gaudi::Property<double> m_xiMax{this,"xi_max",0.20," "}; 
  Gaudi::Property<double> m_ptMin{this,"pt_min",0," "}; //[MeV]
  Gaudi::Property<double> m_ptMax{this,"pt_max",10e3," "}; //[MeV]
  Gaudi::Property<double> m_E0{this,"beam_energy",7e6," "}; //[MeV]
  Gaudi::Property<bool> m_DoubleTag {this,"double_tag",false," "};
  Gaudi::Property<bool> m_Single_tagA {this,"double_tagA",false," "};// require proton on A, regardless of C
  Gaudi::Property<bool> m_Single_tagC {this,"double_tagC",false," "};// require proton on C, regardelss of A
  

};

#endif
