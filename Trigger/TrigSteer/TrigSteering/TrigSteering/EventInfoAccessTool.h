/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : EventInfoAccessTool
 *
 * @brief Tool to change EventInfo based on trigger result
 *
 * @author Francesca Pastore <francesca.pastore@cern.ch>
 *
 **********************************************************************************/

#ifndef TRIGSTEERING_HLT_EventInfoAccessTool_H
#define TRIGSTEERING_HLT_EventInfoAccessTool_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include <vector>

class StatusCode;
class MsgStream;

namespace HLT {
   class SteeringChain;

   static const InterfaceID IID_EventInfoAccessTool("HLT::IEventInfoAccessTool", 1, 0);
   /**
      @class IEventInfoAccessTool interface
      @author Francesca Pastore <francesca.pastore@cern.ch>
   */
   class IEventInfoAccessTool : public virtual IAlgTool
   {
   public:
     virtual StatusCode updateStreamTag(const std::vector<SteeringChain*>& activeChains) = 0;   
     virtual StatusCode setStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& set_streams) = 0;   
     virtual StatusCode removeStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& remove_streams) = 0;   
     virtual StatusCode addStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& new_streams) = 0;   
     virtual StatusCode getStreamTags(std::vector< xAOD::EventInfo::StreamTag >& streamTags) = 0;   
     static const InterfaceID& interfaceID() { return IID_EventInfoAccessTool; } //!< std interface declaration
   };


   /**
      @class EventInfoAccessTool Tool to change EventInfo based on trigger result
      @author Francesca Pastore <francesca.pastore@cern.ch>
   */
   class EventInfoAccessTool : virtual public AthAlgTool, public virtual IEventInfoAccessTool
   {
   public:

      EventInfoAccessTool(const std::string& name, const std::string& type,
                          const IInterface* parent); //!< std Gaudi constructor

      virtual ~EventInfoAccessTool() {} //!< virtual desctructor

      StatusCode initialize(); //!< Gaudi initialize
      StatusCode finalize();   //!< Gaudi finalize

      StatusCode setStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& set_streams);
      StatusCode removeStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& remove_streams);
      StatusCode addStreamTags(const std::vector< xAOD::EventInfo::StreamTag >& new_streams);
      StatusCode getStreamTags(std::vector< xAOD::EventInfo::StreamTag >& streamTags);
      StatusCode updateStreamTag(const std::vector<SteeringChain*>& activeChains);


      StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ); //!< Gaudi interface method
      static const InterfaceID& interfaceID() { return IEventInfoAccessTool::interfaceID(); }  //!< Gaudi interface method

   protected:

      StringArrayProperty m_listOfChainsAddingStreamTag; //!< list of chain:streamtag that are allowed to add streamtag to EventInfo even if in rerun
 
   };

} // end namespace

#endif
