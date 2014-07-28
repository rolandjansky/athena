/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuLUTSvc_H
#define MuLUTSvc_H


#ifndef LVL2_STANDALONE

#include <string>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"

#include "TrigmuFast/MuLUT.h"

static const InterfaceID IID_IMuLUTSvc(1496, 0, 0);

class MuLUTSvc : public Service,
                 virtual public IInterface
{
    public:
    MuLUTSvc(const std::string& name,ISvcLocator* sl);
    virtual ~MuLUTSvc() {}

    static const InterfaceID& interfaceID() { return IID_IMuLUTSvc; }

    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);

    private:
    StringProperty m_lut_filename; // file containing the LUT data

    public:

    virtual const MuLUT* muLUT(void) const;
};

#endif



#endif 
