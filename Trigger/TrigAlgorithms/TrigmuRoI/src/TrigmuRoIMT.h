/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUROI_TRIGMUROIMT_H
#define TRIGMUROI_TRIGMUROIMT_H

#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigMuonToolInterfaces/ITrigMuonRoITool.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

class TrigmuRoIMT : public AthReentrantAlgorithm
{
   public:

      TrigmuRoIMT(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi algorthm constructor
      virtual StatusCode initialize() override;
      virtual StatusCode execute(const EventContext& ctx) const override;

   private:
   
      ToolHandle<ITrigMuonRoITool> m_trigMuonRoITool{this, "RoITool", "TrigMuonRoITool"};
      ServiceHandle<LVL1::RecMuonRoiSvc> m_recRPCRoiSvc;
      ServiceHandle<LVL1::RecMuonRoiSvc> m_recTGCRoiSvc;

      SG::WriteHandleKey<TrigRoiDescriptorCollection> m_roisWriteHandleKey {this,"RoisWriteHandleKey","Unspecified", "Output collection of RoIs"};

      ToolHandle<GenericMonitoringTool> m_monTool{this,"MonTool","","Monitoring tool"};
      Gaudi::Property<int> m_minValueForOutOfTimeBC{this, "MinValueForOutOfTimeBC", -9999, "Min value for out of time bunch crossing"};
      Gaudi::Property<int> m_maxValueForOutOfTimeBC{this, "MaxValueForOutOfTimeBC", 9999, "Max value for out of time bunch crossing"};

      unsigned int getBitMaskValue( const unsigned int uintValue, const unsigned int mask ) const;
};

#endif
