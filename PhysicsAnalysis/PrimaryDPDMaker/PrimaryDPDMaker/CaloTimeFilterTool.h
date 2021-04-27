/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_CaloTimeFilterTool_H
#define PrimaryDPDMaker_CaloTimeFilterTool_H

#include "AsgTools/AsgTool.h"
#include "PrimaryDPDMaker/ICaloTimeFilterTool.h"



class CaloTimeFilterTool : public asg::AsgTool, virtual public ICaloTimeFilterTool {
    
 public:    
    CaloTimeFilterTool( const std::string& tool_name);

    ASG_TOOL_CLASS( CaloTimeFilterTool, ICaloTimeFilterTool )

    virtual ~CaloTimeFilterTool() = default;
    
  
  virtual
  StatusCode getTimeDifference(TimingFilterInformation& time_info, const SG::ReadHandleKey<LArCollisionTime>& read_key,
                                         const EventContext& ctx) const override;
 
  virtual StatusCode getTimeDifference(TimingFilterInformation& time_info) override;
 
 protected:
  
  void fillTimeDifference(TimingFilterInformation& time_info, const LArCollisionTime* larCollisionTime ) const;
  Gaudi::Property<float> m_timeCut{this, "timeDiffCut", 5.};
  Gaudi::Property<int>  m_mincellsperside{this, "MinCellsPerSide", 9.};
  Gaudi::Property<std::string> m_containerName{this,"ContainerName", "LArCollisionTime"};
  int   m_nevt;
 
};

#endif

