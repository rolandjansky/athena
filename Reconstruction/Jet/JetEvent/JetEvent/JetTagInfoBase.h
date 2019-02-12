/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETTAGINFOBASE_H
#define JETEVENT_JETTAGINFOBASE_H

#include <vector>
#include <string>

/** string to identify the tagger tool.
    For backward compability.
    This will likely be removed in the near future.
    
    put it into namespace Analysis, where everybody expects it to be
*/

namespace Analysis {
  typedef std::string TagInfoType;
  
  class ITagInfo
  {
  public:
    
    //!< destructor
    virtual ~ITagInfo() = default;
    
    //!< to set the tag likelihood for one tagger
    virtual void setTagLikelihood(const std::vector<double>&) = 0;
    virtual void setTagLikelihood(std::vector<double>&&) = 0;
    
    //!< returns the tag likelihood of one tagger
    virtual const std::vector<double>& tagLikelihood(void) const = 0;
    
    //!< set the weight for one tagger
    virtual void setWeight(double weight) = 0;
    
    //!< get the weight of one tagger
    virtual double weight() const = 0;
    
    //!< sets the isValid flag to true, cannot be reversed!
    virtual void makeValid() = 0;
    
    //!< returns true if the tag was successfull
    virtual bool isValid() const = 0;
    
    //!< returns the infoType of the info objects. This has to be set by the tagger.
    virtual TagInfoType infoType() const = 0;
    
    //!< a clone method for the proper workings of the copy constructor
    virtual ITagInfo* clone() const = 0;
  };
}

class JetTagInfoBase : public Analysis::ITagInfo
{
  // make the templated converter base friend
  template <class PERS>
    friend class JetConverterBase;
  
 public:
  //!< a clone method for the proper workings of the copy constructor
  virtual JetTagInfoBase* clone() const = 0;
};


/** @class JetTagInfoBase:

    This is the abstract base class for additional information to be put into the JetTag
    class. Users can inherit from this baseclass, extend the interface and datamembers with
    information their taggers need to store and then use it for tagging and pass it into the TagJet.
    
    Now inherits from ITagInfo. This allows for reading ParticleJets from rel 13  in rel 14.
    Thanks to Scott Snyder for the suggestion.
    
    @author Rolf.Seuster@cern.ch
*/
#endif
