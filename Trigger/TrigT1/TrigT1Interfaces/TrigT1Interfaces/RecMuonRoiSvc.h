// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1INTERFACES_RECMUONROISVC_H
#define TRIGT1INTERFACES_RECMUONROISVC_H

// STL include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthService.h"


namespace LVL1 {

   // identifire of RPC/TGC RecRoI service, which should be consistent
   //  with the class definition in each package
   const std::string ID_RecRpcRoiSvc = "LVL1RPC::RPCRecRoiSvc";
   const std::string ID_RecTgcRoiSvc = "LVL1TGC::TGCRecRoiSvc";

   /**
    *  @short Base class for the RPC and TGC RoI reconstruction services
    *
    *         The eta-phi positions of the muon RoIs have to be decoded using
    *         the cabling maps of the detectors. This class provides a common
    *         base class for the RPC and TGC RoI reconstruction services,
    *         which are implemented in their own separate packages.
    *
    * @author Tadashi Maeno <Tadashi.Maeno.cern.ch>
    *
    * $Revision: 646643 $
    * $Date: 2015-02-12 23:58:09 +0100 (Thu, 12 Feb 2015) $
    */
   class RecMuonRoiSvc : public AthService {

   public:
      RecMuonRoiSvc ( const std::string& name, ISvcLocator* svc )
         : AthService( name, svc ) {}
      virtual ~RecMuonRoiSvc ( void ) {}

     static const InterfaceID& interfaceID() {
       return IService::interfaceID();
     }
     
     virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIF ) {
       return AthService::queryInterface( riid, ppvIF );
     }

      virtual StatusCode initialize ( void ) { return StatusCode::SUCCESS; }
      virtual StatusCode finalize ( void ) { return StatusCode::SUCCESS; }

      /// pure virtual function for sub-detector-specific RoI reconstruction
      virtual void reconstruct ( const unsigned int & roiWord ) const = 0;

      /// pure virtual function to return phi coord of RoI
      virtual double phi ( void ) const = 0;

      /// pure virtual function returns eta coord of RoI
      virtual double eta ( void ) const = 0;

      /// virtual function for writing the RoI/ROB map
      virtual bool writeRoiRobMap ( const std::string & /*filename*/ ) 
      { return true; }

   }; // class RecMuonRoISvc

} // namespace LVL1

#endif // TRIGT1INTERFACES_RECMUONROISVC_H
