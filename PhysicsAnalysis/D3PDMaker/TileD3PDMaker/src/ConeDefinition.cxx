/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackInCaloTools/ConeDefinition.h"
#include "AthenaBaseComps/AthMessaging.h"

#define NLAYERS 11

//====================================================================================================
ConeDefinition::ConeDefinition(std::string Name, std::vector<std::string> coneDef, AthMessaging *msg){
    name = Name;
    m_msg = msg;
    resetConeDefinition();
    setConeDefinition(coneDef);
    printConeDefinition(MSG::INFO);
} // CONEDEFINITION

//=============================================================================================
/** Get a vector of strings containing the cone definition per layer (can be the same for all).
 Each string has to contain "param" (parametrized eloss), "crossed" (crossed cells),
 a cone size or "NxM" where N (M) is the number of cells in eta (phi) **/
void ConeDefinition::setConeDefinition(std::vector<std::string> coneDef){
    m_msg->msg(MSG::DEBUG) << "In setConeDefinition" << endreq;
    if (coneDef.size() != 1 && coneDef.size() != NLAYERS)
    {
        m_msg->msg(MSG::WARNING) << "Invalid definition for " << name << "!"
        << ". Has to be a vector of size 1 or " << NLAYERS << endreq;
        m_msg->msg(MSG::WARNING) << "Got a vector of size: " << coneDef.size() << endreq;
        return;
    }
    resetConeDefinition();
    // If user gave only one value, use the same for all layers
    if (coneDef.size() == 1)
        coneDef = std::vector<std::string>(NLAYERS, coneDef[0]);
    // For each layer, define what quantity will be used: the parametrization, a cone, NxM cells or the crossed cells
    double cone;
    std::pair<int, int> NM_pair = std::make_pair<int, int> (0,0);
    std::pair<double, double> EtaPhi_pair = std::make_pair<double, double> (0.,0.);
    for (unsigned int i = 0; i < coneDef.size(); i++)
    {
        std::string str = coneDef[i];
        if (str == "crossed")
            m_Crossed[i] = true;
        else if (str == "param")
            m_Param[i] = true;
        else if ( getConeFromString(str, &cone) )
            m_Cone[i] = cone;
        else if ( getNxMFromString(str, NM_pair) )
            m_NxM[i] = NM_pair;
        else if ( getEllipticalConeFromString(str, EtaPhi_pair) )
            m_ElCone[i] = EtaPhi_pair;
        else
        {
            m_msg->msg(MSG::WARNING) << "Invalid string for " << name << ": " << str << endreq;
            m_msg->msg(MSG::WARNING) << "Setting everything back" << endreq;
            setConeDefinition(m_coneDef);
            return;
        }
    }
    m_coneDef = coneDef;
    m_msg->msg(MSG::DEBUG) << name << " definition set successfully" << endreq;
    printConeDefinition();
}

//=========================================
/** Reset cone definition, clearing all maps used to hold it **/
void ConeDefinition::resetConeDefinition(){
    m_msg->msg(MSG::DEBUG) << "In resetConeDefinition" << endreq;
    m_Crossed.clear();
    m_Param.clear();
    m_Cone.clear();
    m_NxM.clear();
    m_ElCone.clear();
}

//=============================================================
/** Print the definition **/
void ConeDefinition::printConeDefinition(const MSG::Level lvl){
    m_msg->msg(lvl) << "In printConeDefinition" << endreq;
    m_msg->msg(lvl) << name << " size: " << m_coneDef.size() << endreq;
    for (unsigned int i = 0; i < m_coneDef.size(); i++)
        m_msg->msg(lvl) << "m_coneDef[" << i << "] : " << m_coneDef[i] << endreq;
    m_msg->msg(lvl) << "******************************************" << endreq;
    m_msg->msg(lvl) << "Layer  Crossed  Param  Cone   NxM  EtaxPhi" << endreq;
    m_msg->msg(lvl) << "******************************************" << endreq;
    for (unsigned int i = 0; i < NLAYERS; i++)
        m_msg->msg(lvl) << "  " << i << "       " << m_Crossed.count(i) << "       " << m_Param.count(i)
        << "    " << (m_Cone.count(i) ? m_Cone[i]: 0) << "    "
        << (m_NxM.count(i) ? m_NxM[i].first : 0) << (m_NxM.count(i) ? m_NxM[i].second : 0) << "  "
        << (m_ElCone.count(i) ? m_ElCone[i].first : 0) << "x" << (m_ElCone.count(i) ? m_ElCone[i].second : 0) << endreq;
    m_msg->msg(lvl) << "***********************************" << endreq;
}

//====================================================================
/** Given a string that might represent the cone size, set the cone to the given value and return true,
 or return false if the string conversion fails **/
bool ConeDefinition::getConeFromString(std::string str, double *cone){
    char* end = 0 ;
    double c = strtod( str.c_str(), &end );
    if ( *end != 0 ) // Invalid input string
        return false;
    if (int(c) != 0)
        return false;
    *cone = c;
    return true;
}

//===================================================================================
/** Get a string of type "NxM" and set a pair containing the N cells in eta and the M in phi.
 Return true (false) if the string conversion succeeded (failed) **/
bool ConeDefinition::getNxMFromString(std::string str, std::pair<int, int> &NM_pair){
    if (str.size() != 3 || str[1] != 'x' ||
        !atoi(const_cast<const char*> (&str[0])) || !atoi(const_cast<const char*> (&str[2])) )
        return false;
    NM_pair.first = atoi( const_cast<const char*> (&str[0]) );
    NM_pair.second = atoi( const_cast<const char*> (&str[2]) );
    return true;
}

//====================================================================================================
/** Get a string that represents an elliptical cone "NxM" where N and M are the axis in eta, phi
 and set a pair containing those values.
 Return true (false) if the string conversion succeeded (failed) **/
bool ConeDefinition::getEllipticalConeFromString(std::string str, std::pair<double, double> &NM_pair){
    std::string::size_type x = str.find('x');
    if (x != std::string::npos) {
        std::string N = str.substr(0, x);
        std::string M = str.substr(x+1);
        if ((NM_pair.first = atof( N.c_str() ) ) &&
            (NM_pair.second = atof( M.c_str() ) ) )
            return true;
    }
    return false;
}
