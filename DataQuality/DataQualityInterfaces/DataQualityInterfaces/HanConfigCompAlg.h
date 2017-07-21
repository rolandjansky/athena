/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanConfigCompAlg_h
#define dqiHanConfigCompAlg_h

#include <string>
#include <map>
#include <iostream>

#include <TObject.h>
#include <TObjString.h>
#include <TFile.h>

#include "DataQualityInterfaces/HanConfigAssessor.h"

class TSeqCollection;

namespace dqi {

class HanConfigCompAlg : public TObject {
public:
  HanConfigCompAlg();
  HanConfigCompAlg( const HanConfigCompAlg& other );
  HanConfigCompAlg& operator=( const HanConfigCompAlg& other );
  virtual ~HanConfigCompAlg();

  virtual void SetName( std::string name );
  virtual const char* GetName() const;

  virtual void AddAlg( std::string alg );
  virtual TIter GetAllAlg() const;

  virtual void AddLib( std::string lib );
  virtual TIter GetAllLib() const;

  virtual void PrintIOStream( std::ostream& o ) const;
 
protected:
  TObjString m_name;
  TSeqCollection *m_subAlgs;
  TSeqCollection *m_libs;

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( HanConfigCompAlg, 2 )
///\endcond
};

}

#endif
