/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuRoadsSvc_H
#define MuRoadsSvc_H


#ifndef LVL2_STANDALONE

#include <string>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"

#include "TrigmuFast/MuRoads.h"

static const InterfaceID IID_IMuRoadsSvc(1495, 0, 0);

class MuRoadsSvc : public Service,
                 virtual public IInterface
{
    public:
    MuRoadsSvc(const std::string& name,ISvcLocator* sl);
    virtual ~MuRoadsSvc() {}

    static const InterfaceID& interfaceID() { return IID_IMuRoadsSvc; }

    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);

    
    private:
    DoubleProperty m_scaleBarrelInner;
    DoubleProperty m_scaleBarrelMiddle;
    DoubleProperty m_scaleBarrelOuter;
    
    MuRoads* m_muon_roads;
    
    public:

    virtual const MuRoads* muRoads(void) const;
};

#endif



#endif 
