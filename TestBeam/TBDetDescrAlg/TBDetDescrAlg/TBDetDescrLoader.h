/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBDetDescrLoader_H
#define TBDetDescrLoader_H

#include "AthenaBaseComps/AthAlgorithm.h"

class StoreGateSvc;
class TBDetDescrManager;

#include <string>

class TBDetDescrLoader : public AthAlgorithm {
public:
    //Constructor
    TBDetDescrLoader(std::string name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TBDetDescrLoader();                         
    
    //Gaudi Hooks
    StatusCode initialize();    
    StatusCode execute();
    StatusCode finalize();

private:

    std::string m_TBDetDescrManager;
    std::string m_TBElementContainer;
    int m_readAction;
    int m_writeAction;

    TBDetDescrManager * m_TBDDM;
    const TBDetDescrManager * c_TBDDM;
};

#endif
