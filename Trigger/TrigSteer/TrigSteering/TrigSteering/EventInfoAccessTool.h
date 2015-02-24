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

      StatusCode updateStreamTag(const std::vector<SteeringChain*>& activeChains);


      StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ); //!< Gaudi interface method
      static const InterfaceID& interfaceID() { return IEventInfoAccessTool::interfaceID(); }  //!< Gaudi interface method

   protected:

      MsgStream& msg(const MSG::Level lvl) { return (*m_log) << lvl; }
      bool msgLvl (const MSG::Level lvl) const;

      MsgStream* m_log;             //!< MsgStream used within all none Gaudi classes of this package

      StringArrayProperty m_listOfChainsAddingStreamTag; //!< list of chain:streamtag that are allowed to add streamtag to EventInfo even if in rerun

      ServiceHandle< StoreGateSvc > m_storeGate; //!< pointer to StoreGate
 
   };


   inline bool
   EventInfoAccessTool::msgLvl(const MSG::Level lvl) const {
      if (m_log->level() <= lvl) {
         (*m_log) << lvl;
         return true;
      } else {
         return false;
      }
   }



} // end namespace




#endif
