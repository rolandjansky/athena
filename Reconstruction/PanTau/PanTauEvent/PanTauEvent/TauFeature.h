/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TauFeature.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef PANTAUEVENTTAUFEATURE_H
#define PANTAUEVENTTAUFEATURE_H


#include <string>
#include <map>
#include <vector>

#include "GaudiKernel/MsgStream.h"

namespace PanTau {


/** @class TauFeature
    Class containing features of a tau seed.
    @author Sebastian.Fleischmann@cern.ch
    */

class TauFeature {

//struct cmp_fn {
//bool operator()(const std::string& s1, const std::string& s2) const {
//return (s1 < s2);
//}
//};
//typedef std::map<std::string, double, cmp_fn> FeatureMap;
typedef std::map<std::string, double> FeatureMap;
typedef FeatureMap::iterator FeatureMapIter;
typedef FeatureMap::const_iterator FeatureMapConstIter;
typedef std::map<std::string, const std::vector<double> > VectorFeatureMap;
typedef VectorFeatureMap::iterator VectorFeatureMapIter;
typedef VectorFeatureMap::const_iterator VectorFeatureMapConstIter;

public:
    /** Default constructor */
    TauFeature();
    /** Copy Constructor */
    //TauFeature(const TauFeature& feature);
//    /** Assignment operator */
//    TauFeature& operator=(const TauFeature& seed);

//    /**
//     * Full constructor. 
//     *
//     * @param[in] name name of the feature
//     * @param[in] value value of the feature
//     */
//     TauFeature(
//         const std::string& name,
//         const double& value
//         );

    /** Destructor */
    ~TauFeature() = default;

    /** returns the value of the feature given by its name*/
    double value(const std::string& ItsName, bool& isValid) const;

    /** returns the value of a vector feature given by its name*/
    const std::vector<double>& vecValue(const std::string& name) const;

    /** adds a new feature*/
    bool addFeature(
        const std::string& name,
        const double& value);

    /** adds a new vector feature*/
    bool addVecFeature(
        const std::string& name,
        const std::vector<double>& value);

    /** returns the size of the m_featureMap */
    int nValues() const;
    
    /** returns the size of the m_vecFeatureMap */
    int nVecValues() const;
    
    
    void add(PanTau::TauFeature* otherFeatures);
    
    void addFeaturesFromMap(std::map<std::string, double>, std::string prefix);
    
    MsgStream& dump( MsgStream& out, MSG::Level debugLevel ) const;
    
private:
    mutable MsgStream m_log;
    
protected:
    
    /** The map containg all features */
    FeatureMap m_featureMap;
    /** The map containg all features */
    VectorFeatureMap m_vecFeatureMap;
    
};


}

/**Overload of << operator for MsgStream for debug output*/ 
// inline MsgStream& operator << ( MsgStream& sl, const PanTau::TauFeature& feature)
//   { return feature.dump(sl); }

#endif // PANTAUEVENTTAUFEATURE_H
