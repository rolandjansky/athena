/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanConfigParMap_h
#define dqiHanConfigParMap_h

#include <iostream>
#include <string>

#include <TObject.h>
#include <TList.h>
#include <TObjString.h>


namespace dqi {

class HanConfigParMap : public TObject {
public:

  HanConfigParMap();
  HanConfigParMap(const HanConfigParMap& other);
  virtual ~HanConfigParMap();
  
  virtual void         SetName( const std::string name_ );
  virtual const char*  GetName() const;
  
  virtual void         SetValue( const std::string value_ );
  virtual const char*  GetValue() const;
  
  virtual void         PrintIOStream( std::ostream& o ) const;

  virtual TList *      GetList();

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( HanConfigParMap, 1 ) // A persistable object containing a string pair
///\endcond

protected:

  TObjString m_name;
  TObjString m_value;

};

} // namespace dqi

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigParMap& p );
std::ostream& operator<<( std::ostream& o, const dqi::HanConfigParMap* p );


#endif

