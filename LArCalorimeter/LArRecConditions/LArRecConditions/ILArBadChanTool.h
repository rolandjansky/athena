/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ILArBadChanTool_H
#define ILArBadChanTool_H

#include "Identifier/HWIdentifier.h"
#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/LArBadFeb.h"
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"


class ILArBadChanTool : public virtual IAlgTool {
public:
  virtual ~ILArBadChanTool() {}

  static const InterfaceID& interfaceID() { 
    static const InterfaceID IID_ILArBadChanTool("ILArBadChanTool", 1 , 0);
    return IID_ILArBadChanTool; 
  }

  /// status access via LAr hardware identifier
  virtual LArBadChannel status( HWIdentifier id) const = 0;

  /// status access via LAr FEB id, and channel number within that FEB (0 to 127)
  virtual LArBadChannel status(const HWIdentifier& febId, int chanNumber) const = 0;

  /// status access via offline identifier
  virtual LArBadChannel offlineStatus( Identifier id) const = 0;

  /// returns true if the FEB (identified by it's HWIdentifier) is missing 
  virtual bool febMissing(  HWIdentifier febId) const = 0;

  /// returns true if the FEB (identified by it's HWIdentifier) does not have 
  /// any bad channels 
  virtual bool febAllGood(  HWIdentifier febId) const = 0;

  /// returns the complete FEB status for a FEB HWIdentifier
  virtual LArBadFeb febStatus( HWIdentifier febId) const = 0;

  /// returns the HWIdentifiers of all the problematic FEBs, no matter what the FEB problem is
  virtual std::vector<HWIdentifier> missingFEBs() const = 0;

  /// returns all the information about problematic FEBs in one container
  virtual const std::vector< std::pair<HWIdentifier,LArBadFeb> >& fullBadFebsState() const = 0;

  // Callback functions
  virtual StatusCode updateFromDB(IOVSVC_CALLBACK_ARGS) = 0;
  virtual StatusCode updateBadFebsFromDB(IOVSVC_CALLBACK_ARGS) = 0;

};


#endif
