/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/TauFeature.h"

#include <cmath>
#include <stdexcept>
#include <iostream>



static const std::vector<double> s_defaultVec(0);

PanTau::TauFeature2::TauFeature2()
    :
    m_featureMap(),
    m_vecFeatureMap()

{
}



PanTau::TauFeature2::~TauFeature2()
{
}


double PanTau::TauFeature2::value(const std::string& ItsName, bool& isValid) const
{
    FeatureMapConstIter iter = m_featureMap.find(ItsName);
if(m_featureMap.end() == iter)
{
     // not found
     isValid=false;
     //std::cout << "Could not find key called '" << ItsName << "' in feature map. Either the spelling is wrong or the feature does not exist. Returning -999999." << std::endl;
     return -999999.;
}
// entry found, return value
isValid=true;
return (*iter).second; 
}

const std::vector<double>& PanTau::TauFeature2::vecValue(const std::string& name) const {
    VectorFeatureMapConstIter iter = m_vecFeatureMap.find(name);
    if(m_vecFeatureMap.end() == iter)
    {
        return s_defaultVec;
    }
    return (*iter).second;
}

bool PanTau::TauFeature2::addFeature(
        const std::string& name,
        const double& value) {

    if (std::isnan(value)) {
        throw std::runtime_error("TauFeature2::addFeature: Given " + name + " value is NaN!");
    }
    if (std::isinf(value)){
        throw std::runtime_error("TauFeature2::addFeature: Given " + name + " value is inf!");
    }
    std::pair<FeatureMapConstIter, bool> result = m_featureMap.insert(make_pair(name, value));
    return result.second;
}

bool PanTau::TauFeature2::addVecFeature(
        const std::string& name,
        const std::vector<double>& value) {
    std::pair<VectorFeatureMapConstIter, bool> result = m_vecFeatureMap.insert(make_pair(name, value));
    return result.second;
}


int PanTau::TauFeature2::nValues() const {
    return m_featureMap.size();
}


int PanTau::TauFeature2::nVecValues() const {
    return m_vecFeatureMap.size();
}


void PanTau::TauFeature2::add(PanTau::TauFeature2* otherFeatures) {
    
    //add the scalar features
    FeatureMapIter itScalar = otherFeatures->m_featureMap.begin();
    for(; itScalar != otherFeatures->m_featureMap.end(); itScalar++) {
        std::string key = itScalar->first;
        double      val = itScalar->second;
        bool        isOK = this->addFeature(key, val);
        if(isOK == false) {
            throw std::runtime_error("PanTau::TauFeature2::add( PanTau::TauFeature2* ): Error when adding scalar feature " + key);
        }
    }//end loop over scalar features to be added
    
    //add the vector features
    VectorFeatureMapIter itVector = otherFeatures->m_vecFeatureMap.begin();
    for(; itVector != otherFeatures->m_vecFeatureMap.end(); itVector++) {
        std::string         key = itVector->first;
        std::vector<double> val = itVector->second;
        bool        isOK = this->addVecFeature(key, val);
        if(isOK == false) {
            throw std::runtime_error("PanTau::TauFeature2::add( PanTau::TauFeature2* ): Error when adding vector feature " + key);
        }
    }//end loop over vector features to be added
    
}



void PanTau::TauFeature2::addFeaturesFromMap(std::map<std::string, double> otherMap, std::string prefix = "") {
    
    FeatureMapConstIter iter    = otherMap.begin();
    FeatureMapConstIter iterEnd = otherMap.end();
    
    for(; iter != iterEnd; iter++) {
        std::string oKey = prefix + iter->first;
        double      oVal = iter->second;
        addFeature(oKey, oVal);
    }
    
    return;
}

/*
std::ostream& operator<< ( std::ostream& out, PanTau::TauFeature2 
                            xAOD::FileMetaData_v1::MetaDataType type ) {
MsgStream& PanTau::TauFeature2::dump( MsgStream& out, MSG::Level debugLevel = MSG::VERBOSE ) const
{ 
    std::string name("TauFeature2: ");
    out << debugLevel << name << endreq;
    FeatureMapConstIter iter = m_featureMap.begin();
    FeatureMapConstIter iterEnd = m_featureMap.end();
    for ( ; iter!=iterEnd; iter++) {
        out << debugLevel << (*iter).first << " : \t" << (*iter).second <<endreq;
    }
    
    std::string nameVec("TauVecFeature: ");
    out << debugLevel << nameVec << endreq;
    VectorFeatureMapConstIter iterVec = m_vecFeatureMap.begin();
    VectorFeatureMapConstIter iterEndVec = m_vecFeatureMap.end();
    for ( ; iterVec!=iterEndVec; iterVec++) {
        out << debugLevel << (*iterVec).first << " : \t" << (*iterVec).second.size() << " entries" <<endreq;
    }
    return out; 
}
*/

