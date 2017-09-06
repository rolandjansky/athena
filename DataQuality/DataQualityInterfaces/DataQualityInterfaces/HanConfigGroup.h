/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanConfigGroup_h
#define dqiHanConfigGroup_h

#include <TList.h>
#include <TObject.h>
#include <TObjString.h>
#include <map>

#ifndef __CINT__
#include <boost/shared_ptr.hpp>
#endif // __CINT__

#include "DataQualityInterfaces/HanConfigAssessor.h"


namespace dqi {

class HanConfigGroup : public HanConfigAssessor {
public:

  HanConfigGroup();
  HanConfigGroup( const HanConfigGroup& other );
  HanConfigGroup& operator=( const HanConfigGroup& other );
  
  virtual ~HanConfigGroup();
  
  virtual std::string        GetUniqueName() const;
  
  virtual void               SetPathName( const std::string& name_ );
  virtual const char*        GetPathName() const;
  
  virtual void               AddAssessor( const HanConfigAssessor& hcass_ );
  virtual const HanConfigAssessor  GetAssessor( const std::string& name_ ) const;
  virtual TIter              GetAllAssessors() const;

  virtual void               AddGroup( const HanConfigGroup& hcg_ );
  virtual HanConfigGroup     GetGroup( const std::string& name_ ) const;
  virtual TIter              GetAllGroups() const;
  
  virtual HanConfigGroup*    GetNode( const std::string& name_ ) const;
  
  virtual TSeqCollection*    GetList( TDirectory* basedir, std::map<std::string,TSeqCollection*>& mp );

#ifndef __CINT__
  virtual void  Accept( const Visitor& visitor, boost::shared_ptr<dqm_core::Region> dqParent ) const;
#endif // __CINT__

  virtual void  PrintIOStream( std::ostream& o ) const;
  
//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( HanConfigGroup, 1 ) // A group of assessors and other groups
///\endcond

protected:

  TObjString m_pathName;
  
  TList m_assessors;
  TList m_groups;

};

} // namespace dqi

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigGroup& g );
std::ostream& operator<<( std::ostream& o, const dqi::HanConfigGroup* g );

#endif
