/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PrimaryDPDMaker_MBTSTimeFilterTool_H
#define PrimaryDPDMaker_MBTSTimeFilterTool_H

#include "PrimaryDPDMaker/IMBTSTimeFilterTool.h"
#include "AsgTools/AsgTool.h"
#include "TileIdentifier/TileTBID.h"

class MBTSTimeFilterTool : public asg::AsgTool, virtual public IMBTSTimeFilterTool
{
 public:    
    MBTSTimeFilterTool( const std::string& tool_name);
    ASG_TOOL_CLASS( MBTSTimeFilterTool, IMBTSTimeFilterTool )

 
    virtual ~MBTSTimeFilterTool() = default;

 
  /** Overriding initialize and finalize */
  virtual StatusCode initialize() override;
  
  virtual StatusCode getTimeDifference(TimingFilterInformation& time_info,
                                        const SG::ReadHandleKey<TileCellContainer>& key,
                                        const EventContext& ctx) const  override;

 virtual  StatusCode getTimeDifference(TimingFilterInformation& time_info) override;
 protected:
    /// Once the container is retrieve the filling mechanism is the same.
    void fillTimeDifference(TimingFilterInformation& time_info, const TileCellContainer* container) const;
 
  /** For access to the tile test beam identifiers */
  const TileTBID* m_tileTBID{nullptr};

    // Value in pC, from T2MbtsFex.cxx
  Gaudi::Property<float> m_chargethreshold{this, "ChargeThreshold", 60.0/222.0};
  Gaudi::Property<int>   m_minhitsperside{this, "MinHitsPerSide",     2 }; 
  Gaudi::Property<float> m_maxtimediff{this, "MaxTimeDifference", 10.0 }; 
  Gaudi::Property<std::string> m_mbtsContainerName{this, "MBTSContainerName", "MBTSContainer"};

};

#endif

