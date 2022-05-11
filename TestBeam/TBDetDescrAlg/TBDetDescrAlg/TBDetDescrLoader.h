/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBDetDescrLoader_H
#define TBDetDescrLoader_H

#include "AthenaBaseComps/AthAlgorithm.h"

class TBDetDescrManager;

#include <string>

class TBDetDescrLoader : public AthAlgorithm {
public:
    //Constructor
    TBDetDescrLoader(const std::string& name, ISvcLocator* pSvcLocator);

    //Destructor 
    virtual ~TBDetDescrLoader();                         
    
    //Gaudi Hooks
    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

private:

    std::string m_TBDetDescrManager;
    std::string m_TBElementContainer;
    int m_readAction;
    int m_writeAction;

    TBDetDescrManager * m_TBDDM;
    const TBDetDescrManager * c_TBDDM;
};

#endif
