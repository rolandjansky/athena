/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DetectorElement_H
#define DetectorElement_H

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include <iostream>
#include <string>
#include <vector>

namespace MuonGM {


class DetectorElement {
public:
    std::string name;
    std::string logVolName;
    
    static const  AbsMaterialManager *matManager;
	
    DetectorElement(std::string n): name(n)
	{
            logVolName = "";
	}
    void setLogVolName(std::string str)
        {
            logVolName = str;
        }
    

    virtual void print() {std::cout<<"generic detector element"<<std::endl;}
    virtual ~DetectorElement() {}
    static void setMaterialManager(const AbsMaterialManager & matMan) {
        matManager=&matMan;
    }

};
} // namespace MuonGM

#endif
