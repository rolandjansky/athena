/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanConfigAlgPar_h
#define dqiHanConfigAlgPar_h

#include <iostream>
#include <string>

#include <TObject.h>
#include <TList.h>
#include <TObjString.h>


namespace dqi {

class HanConfigAlgPar : public TObject {
public:

  HanConfigAlgPar();
  virtual ~HanConfigAlgPar();
  
  virtual void         SetName( std::string name_ );
  virtual const char*  GetName() const;
  
  virtual void         SetValue( float value_ );
  virtual float        GetValue() const;
  
  virtual void         PrintIOStream( std::ostream& o ) const;

  virtual TList *      GetList();

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( HanConfigAlgPar, 1 ) // A persistable object containing an algorithm parameter
///\endcond

protected:

  TObjString m_name;
  Float_t    m_value;

};

} // namespace dqi

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAlgPar& p );
std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAlgPar* p );


#endif

