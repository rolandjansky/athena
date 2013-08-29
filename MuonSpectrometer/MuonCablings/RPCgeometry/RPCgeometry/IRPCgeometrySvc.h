/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRPCGEOMETRYSVC_H
#define IRPCGEOMETRYSVC_H

#include "GaudiKernel/IInterface.h"

#include "AthenaKernel/IOVSvcDefs.h"

#include "RPCgeometry/RPCGeometry.h"

//#include <vector>


// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IRPCgeometrySvc(1491, 0, 0);

class IRPCgeometrySvc : virtual public IInterface
{
    public:

    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IRPCgeometrySvc; }


    virtual unsigned long int give_strip_code
                    (std::string,int,int,int,int,int,int,int,int) const = 0;
    virtual unsigned long int give_strip_code
                    (int,int,int,int,int,int,int,int) const = 0;

    virtual bool give_strip_index(unsigned long int code,int& Jtype, 
                          int& StationEta, int& StationPhi, int& DoubletR, 
                          int& DoubletZ,   int& DoubletP,   int& GasGap, 
                          int& MeasuresPhi,int& Strip) const = 0;

    virtual bool give_strip_coordinates(unsigned long int, float*) const = 0;
    virtual bool give_strip_radius(unsigned long int,float&) const = 0;
    virtual bool give_phi_strip_size(unsigned int code,float& size) const = 0;
    virtual bool give_eta_strip_size(unsigned int code,float& size) const = 0;
    virtual bool existCode(unsigned int) const = 0;

    virtual const RPCGeometry* rpcGeometry() const = 0;
    
    virtual std::vector<uint32_t> getRobId (float, float, float, float) const = 0;
    virtual std::vector<uint32_t> getPadHash (float, float, float, float) const = 0;
    
    // call back functions
    virtual StatusCode initFromCablingSvc(IOVSVC_CALLBACK_ARGS) = 0;
    virtual StatusCode updateAMDC(IOVSVC_CALLBACK_ARGS) = 0;
};

#endif // IRPCGEOMETRYSVC_H
