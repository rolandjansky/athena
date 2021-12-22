/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCHandler_H
#define sTGCHandler_H

#include "AGDDControl/XMLHandler.h"
#include <string>

class sTGCDetectorDescription;


class sTGCHandler:public XMLHandler {
public:
        sTGCHandler(const std::string&,
                    AGDDController& c);
        virtual void ElementHandle(AGDDController& c,
                                   xercesc::DOMNode *t) override;
        sTGCDetectorDescription* GetsTGCCurrent();
private:
        sTGCDetectorDescription* m_sTGCCurrent = nullptr;
};

#endif
