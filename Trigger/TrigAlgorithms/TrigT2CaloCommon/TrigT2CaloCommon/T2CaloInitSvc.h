/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T2CaloInitSvc_H
#define T2CaloInitSvc_H

/** ===============================================================
 *     T2CaloInitSvc.h
 *  ===============================================================
 *    Description:  T2CaloInitSvc class to initialize objects  
 *                  used by L2 Calorimiter trigger algorithms
 *    
 *    Created:      Sept 13, 2005
 *         By:      Xin Wu 
 */

#include <string>
#include "AthenaBaseComps/AthService.h"
#include "TrigT2CaloCommon/IT2CaloInitSvc.h"

class T2CaloInitSvc : public AthService,
                 virtual public IT2CaloInitSvc
{
    public:
    T2CaloInitSvc(const std::string& name,ISvcLocator* sl);
    virtual ~T2CaloInitSvc() {}

    virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

    virtual StatusCode initialize(void);
    virtual StatusCode finalize(void);

    private:

};

#endif



