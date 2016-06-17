/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ADDPLANE_H
#define ADDPLANE_H

namespace xercesc_3_1 {class DOMElement;}

class AddPlane {
public:
    AddPlane() {};
    ~AddPlane() {};
    void process(const xercesc_3_1::DOMElement *element, double &zPlane, double &rMinPlane, double &rMaxPlane);
};


#endif //ADDPLANE_H