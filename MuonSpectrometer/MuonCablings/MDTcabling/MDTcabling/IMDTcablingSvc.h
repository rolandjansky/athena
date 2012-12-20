/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMDTCABLINGSVC_H
#define IMDTCABLINGSVC_H

#include "GaudiKernel/IInterface.h"

#include "AthenaKernel/IOVSvcDefs.h"

#include "MDTcabling/MDTGeometry.h"


// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IMDTcablingSvc(1494, 0, 0);

class IMDTcablingSvc : virtual public IInterface
{
    public:

    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IMDTcablingSvc; }

    virtual bool getOnlineIDfromOfflineID(int name, int StationEta,
                                  int StationPhi,int MultiLayer, int TubeLayer,
				  int Tube, 
                                  unsigned short int& SubsystemId,
				  unsigned short int& MrodId,
				  unsigned short int& LinkId,
				  unsigned short int& TdcId,
				  unsigned short int& ChannelId) const = 0;

    virtual bool getOfflineIDfromOnlineID(unsigned short int SubsystemId,
				  unsigned short int MrodId,
				  unsigned short int LinkId,
				  unsigned short int TdcId,
				  unsigned short int ChannelId,
                                  int& name, int& StationEta,
                                  int& StationPhi,int& MultiLayer, 
                                  int& TubeLayer,int& Tube) const = 0;
    
    virtual const MdtIdHelper* idHelper(void) const = 0;
    

    virtual const MDTGeometry* mdtGeometry(void) const = 0;
     
    virtual const MDTGeometry::ROBid  getROBid(float phiMin,float PhiMax,
                                           float etaMin,float etaMax) const = 0;
    
    virtual const MDTGeometry::HASHid getHASHid(float phiMin,float PhiMax,
                                           float etaMin,float etaMax) const = 0;

    virtual bool isTestBeamSetup(void) const = 0;
     
    // call back functions
    virtual StatusCode matchTagInfo(IOVSVC_CALLBACK_ARGS) = 0;
    virtual StatusCode updateAMDC(IOVSVC_CALLBACK_ARGS)   = 0;
    virtual StatusCode loadCablingMap(IOVSVC_CALLBACK_ARGS_P(I,keys)) = 0;
};

#endif // IMDTCABLINGSVC_H
