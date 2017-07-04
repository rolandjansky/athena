/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanConfigAssessor_h
#define dqiHanConfigAssessor_h

#include <iostream>
#include <string>

#include <TList.h>
#include <TFile.h>
#include <TObject.h>
#include <TObjString.h>
#include <map>

#include "DataQualityInterfaces/HanConfigAlgPar.h"
#include "DataQualityInterfaces/HanConfigAlgLimit.h"
#include "DataQualityInterfaces/HanConfigParMap.h"

#ifndef __CINT__
#include <boost/shared_ptr.hpp>
#endif

namespace dqm_core {
class Parameter;
class Region;
class Node;
}


namespace dqi {

class HanConfigAssessor : public TObject {
public:

#ifndef __CINT__
  class Visitor {
  public:
    virtual ~Visitor() { }
    virtual boost::shared_ptr<dqm_core::Node>
    Visit( const HanConfigAssessor* node, boost::shared_ptr<dqm_core::Region> dqParent ) const = 0;
  };
#endif
  
  HanConfigAssessor();
  HanConfigAssessor( const HanConfigAssessor& other );
  HanConfigAssessor& operator=( const HanConfigAssessor& other );
  
  virtual ~HanConfigAssessor();
  
  virtual std::string    GetUniqueName() const;
  
  virtual void           SetName( std::string name_ );
  virtual const char*    GetName() const;

  virtual const char*    GetHistPath() const;
  
  virtual void           SetAlgName( std::string name_ );
  virtual const char*    GetAlgName() const;
  
  virtual void           SetAlgLibName( std::string name_ );
  virtual const char*    GetAlgLibName() const;
  
  virtual void           SetAlgRefName( std::string name_ );
  virtual std::string    GetAlgRefName() const;

  virtual const char*    GetAlgRefString() const;
  
  virtual void                AddAlgPar( const HanConfigAlgPar& algPar_ );
  virtual HanConfigAlgPar     GetAlgPar( std::string name_ ) const;
  virtual TIter               GetAllAlgPars() const;
  
  virtual void                AddAlgLimit( const HanConfigAlgLimit& algLim_ );
  virtual HanConfigAlgLimit   GetAlgLimit( std::string name_ ) const;
  virtual TIter               GetAllAlgLimits() const;

  virtual void                AddAnnotation( const HanConfigParMap& annotation_ );
  virtual const HanConfigParMap* GetAnnotation( std::string name_ ) const;
  virtual TIter               GetAllAnnotations() const;

  virtual void           SetWeight( float weight_ );
  virtual float          GetWeight() const;

  virtual void           SetIsRegex( bool isRegex_ );
  virtual bool           GetIsRegex() const;

  virtual TSeqCollection*     GetList( TDirectory* basedir, std::map<std::string,TSeqCollection*>& mp );


#ifndef __CINT__
  virtual void           Accept( const Visitor& visitor, boost::shared_ptr<dqm_core::Region> dqParent ) const;
#endif // __CINT__

  virtual void           PrintIOStream( std::ostream& o ) const;
  
protected:

  TObjString m_name;
  
  TObjString m_algName;
  TObjString m_algLibName;
  TObjString m_algRefName;
  
  TSeqCollection* m_algPars;
  TSeqCollection* m_algLimits;
  TSeqCollection* m_annotations;

  float m_weight;
  bool m_isRegex;

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( HanConfigAssessor, 5 ) // A histogram with an associated algorithm, producing an assessment
///\endcond
  
};

} // namespace dqi

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAssessor& p );
std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAssessor* p );

#endif
