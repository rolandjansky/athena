/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PANTAUALGS_TAUFEATURE_H
#define PANTAUALGS_TAUFEATURE_H

#include <string>
#include <map>
#include <vector>

namespace PanTau {
  
  /** @class TauFeature
      Class containing features of a tau seed.
      @author Sebastian.Fleischmann@cern.ch
  */

  class TauFeature {

    typedef std::map<std::string, double> FeatureMap;
    typedef FeatureMap::iterator FeatureMapIter;
    typedef FeatureMap::const_iterator FeatureMapConstIter;
    typedef std::map<std::string, const std::vector<double> > VectorFeatureMap;
    typedef VectorFeatureMap::iterator VectorFeatureMapIter;
    typedef VectorFeatureMap::const_iterator VectorFeatureMapConstIter;

  public:
    /** Default constructor */
    TauFeature();

    /** Destructor */
    virtual ~TauFeature();

    /** returns the value of the feature given by its name*/
    double value(const std::string& name, bool& isValid) const;

    /** returns the value of a vector feature given by its name*/
    const std::vector<double>& vecValue(const std::string& name) const;

    /** adds a new feature*/
    bool addFeature(const std::string& name,
		    const double value);

    /** adds a new vector feature*/
    bool addVecFeature(const std::string& name,
		       const std::vector<double>& value);

    /** returns the size of the m_featureMap */
    int nValues() const;
    
    /** returns the size of the m_vecFeatureMap */
    int nVecValues() const;
    
    
    void add(PanTau::TauFeature* otherFeatures);
        
  protected:
    
    /** The map containg all features */
    FeatureMap m_featureMap;
    /** The map containg all features */
    VectorFeatureMap m_vecFeatureMap;
    
  };

}

#endif // PANTAUALGS_TAUFEATURE_H
