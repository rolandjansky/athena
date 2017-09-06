/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanConfigMetadata_h
#define dqiHanConfigMetadata_h

#include <string>
#include <map>
#include <iostream>

#include <TObject.h>
#include <TObjString.h>
#include <TFile.h>

#include "DataQualityInterfaces/HanConfigParMap.h"

class TSeqCollection;

namespace dqi {

class HanConfigMetadata : public TObject {
public:
  HanConfigMetadata();
  HanConfigMetadata( const HanConfigMetadata& other );
  HanConfigMetadata& operator=( const HanConfigMetadata& other );
  virtual ~HanConfigMetadata();

  virtual void SetName( std::string name );
  virtual const char* GetName() const;

  virtual void                AddKeyVal( const HanConfigParMap& keyval_ );
  virtual HanConfigParMap     GetKeyVal( std::string name_ ) const;
  virtual TIter               GetAllKeyVals() const;

  virtual TSeqCollection*     GetList( const TDirectory* basedir, std::map<std::string,TSeqCollection*>& mp );

  virtual void PrintIOStream( std::ostream& o ) const;
 
protected:
  TObjString m_name;
  TSeqCollection *m_metadata;

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( HanConfigMetadata, 1 )
///\endcond
};

}

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigMetadata& p );
std::ostream& operator<<( std::ostream& o, const dqi::HanConfigMetadata* p );

#endif
