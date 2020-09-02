/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ADDPLANE_H
#define ADDPLANE_H

#include <xercesc/dom/DOM.hpp>

class AddPlane {
public:
    AddPlane() {};
    ~AddPlane() {};
    void process(const xercesc::DOMElement *element, double &zPlane, double &rMinPlane, double &rMaxPlane);
};


#endif //ADDPLANE_H
