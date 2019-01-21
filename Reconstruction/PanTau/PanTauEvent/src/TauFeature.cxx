/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauEvent/TauFeature.h"
#include "GaudiKernel/MsgStream.h"

#include <cmath>
#include <stdexcept>



static const std::vector<double> s_defaultVec(0);

PanTau::TauFeature::TauFeature()
    :
    m_log(0),
    m_featureMap(),
    m_vecFeatureMap()

{
}



double PanTau::TauFeature::value(const std::string& ItsName, bool& isValid) const
{
    FeatureMapConstIter iter = m_featureMap.find(ItsName);
if(m_featureMap.end() == iter)
{
     // not found
     isValid=false;
     m_log << "Could not find key called '" << ItsName << "' in feature map. Either the spelling is wrong or the feature does not exist. Returning -999999." << endmsg;
     return -999999.;
}
// entry found, return value
isValid=true;
return (*iter).second; 
}

const std::vector<double>& PanTau::TauFeature::vecValue(const std::string& name) const {
    VectorFeatureMapConstIter iter = m_vecFeatureMap.find(name);
    if(m_vecFeatureMap.end() == iter)
    {
        return s_defaultVec;
    }
    return (*iter).second;
}

bool PanTau::TauFeature::addFeature(
        const std::string& name,
        const double& value) {

    if (std::isnan(value)) {
        throw std::runtime_error("TauFeature::addFeature: Given " + name + " value is NaN!");
    }
    if (std::isinf(value)){
        throw std::runtime_error("TauFeature::addFeature: Given " + name + " value is inf!");
    }
    std::pair<FeatureMapConstIter, bool> result = m_featureMap.insert(make_pair(name, value));
    return result.second;
}

bool PanTau::TauFeature::addVecFeature(
        const std::string& name,
        const std::vector<double>& value) {
    std::pair<VectorFeatureMapConstIter, bool> result = m_vecFeatureMap.insert(make_pair(name, value));
    return result.second;
}


int PanTau::TauFeature::nValues() const {
    return m_featureMap.size();
}


int PanTau::TauFeature::nVecValues() const {
    return m_vecFeatureMap.size();
}


void PanTau::TauFeature::add(PanTau::TauFeature* otherFeatures) {
    
    //add the scalar features
    FeatureMapIter itScalar = otherFeatures->m_featureMap.begin();
    for(; itScalar != otherFeatures->m_featureMap.end(); itScalar++) {
        std::string key = itScalar->first;
        double      val = itScalar->second;
        bool        isOK = this->addFeature(key, val);
        if(isOK == false) {
            throw std::runtime_error("PanTau::TauFeature::add( PanTau::TauFeature* ): Error when adding scalar feature " + key);
        }
    }//end loop over scalar features to be added
    
    //add the vector features
    VectorFeatureMapIter itVector = otherFeatures->m_vecFeatureMap.begin();
    for(; itVector != otherFeatures->m_vecFeatureMap.end(); itVector++) {
        std::string         key = itVector->first;
        std::vector<double> val = itVector->second;
        bool        isOK = this->addVecFeature(key, val);
        if(isOK == false) {
            throw std::runtime_error("PanTau::TauFeature::add( PanTau::TauFeature* ): Error when adding vector feature " + key);
        }
    }//end loop over vector features to be added
    
}



void PanTau::TauFeature::addFeaturesFromMap(std::map<std::string, double> otherMap, std::string prefix = "") {
    
    FeatureMapConstIter iter    = otherMap.begin();
    FeatureMapConstIter iterEnd = otherMap.end();
    
    for(; iter != iterEnd; iter++) {
        std::string oKey = prefix + iter->first;
        double      oVal = iter->second;
        addFeature(oKey, oVal);
    }
    
    return;
}

MsgStream& PanTau::TauFeature::dump( MsgStream& out, MSG::Level debugLevel = MSG::VERBOSE ) const
{ 
    std::string name("TauFeature: ");
    out << debugLevel << name << endmsg;
    FeatureMapConstIter iter = m_featureMap.begin();
    FeatureMapConstIter iterEnd = m_featureMap.end();
    for ( ; iter!=iterEnd; iter++) {
        out << debugLevel << (*iter).first << " : \t" << (*iter).second <<endmsg;
    }
    
    std::string nameVec("TauVecFeature: ");
    out << debugLevel << nameVec << endmsg;
    VectorFeatureMapConstIter iterVec = m_vecFeatureMap.begin();
    VectorFeatureMapConstIter iterEndVec = m_vecFeatureMap.end();
    for ( ; iterVec!=iterEndVec; iterVec++) {
        out << debugLevel << (*iterVec).first << " : \t" << (*iterVec).second.size() << " entries" <<endmsg;
    }
    return out; 
}
