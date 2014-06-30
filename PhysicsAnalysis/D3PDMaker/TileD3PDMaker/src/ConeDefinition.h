/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONEDEFINITION_H
#define CONEDEFINITION_H

#include "GaudiKernel/MsgStream.h"

#include <vector>
#include <string>
#include <map>

class AthMessaging;

/**
 * @class ConeDefinition 
 *
 * Definition of the cone shape used to retrieve Ecore and Eloss in TrackInCaloTools
 *
 * @author Bruno Lenzi
 */

class ConeDefinition {
	public:
		ConeDefinition(std::string Name, std::vector<std::string> coneDef, AthMessaging *msg);
		~ConeDefinition() {};
		
    void setConeDefinition(std::vector<std::string> coneDef);
    void resetConeDefinition();
    void printConeDefinition(const MSG::Level lvl = MSG::DEBUG);
    bool getConeFromString(std::string str, double *cone);
    bool getNxMFromString(std::string str, std::pair<int, int> &NM_pair);
    bool getEllipticalConeFromString(std::string str, std::pair<double, double> &EtaPhi_pair);
    
    AthMessaging *m_msg;
    std::string name;
    std::vector<std::string> m_coneDef;
    std::map<int, bool> m_Crossed;
    std::map<int, bool> m_Param;
    std::map<int, double> m_Cone;
    std::map<int, std::pair<int,int> > m_NxM;
    std::map<int, std::pair<double,double> > m_ElCone;
};

#endif
