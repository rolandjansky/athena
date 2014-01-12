/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITGCGEOMETRYSVC_H
#define ITGCGEOMETRYSVC_H

#include "AthenaKernel/IOVSvcDefs.h"


// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_TGCgeometrySvc("TGCgeometrySvc", 1 , 0);

class ITGCgeometrySvc : virtual public IInterface 
{
    public:        
    
    struct Entry
    {
        float eta;
        float phi;
        float r;
        float z;
        float width;
        int sta;
        bool isStrip;
    };

    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_TGCgeometrySvc; }


    const Entry& getEntry(int , int , int , int , int ) const = 0;
    int robNumber(double , double ) const = 0;
    int rdoID(double , double ) const = 0;  
    
    virtual bool isConfigured(void) const = 0;

    virtual StatusCode matchTagInfo(IOVSVC_CALLBACK_ARGS) = 0;

};
 
#endif // ITGCGEOMETRYSVC_H
