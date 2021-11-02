/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef micromegasHandler_H
#define micromegasHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class MMDetectorDescription;


class micromegasHandler:public XMLHandler {
public:
        micromegasHandler(const std::string&,
                          AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
        MMDetectorDescription* GetMMCurrent();
private:
        MMDetectorDescription* m_mmCurrent;
};

#endif
