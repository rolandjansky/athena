/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiHanConfigAlgLimit_h
#define dqiHanConfigAlgLimit_h

#include <iostream>
#include <string>

#include <TObject.h>
#include <TObjString.h>

class TSeqCollection;

namespace dqi {

class HanConfigAlgLimit : public TObject {
public:

  HanConfigAlgLimit();
  virtual ~HanConfigAlgLimit();
  
  virtual void         SetName( std::string name_ );
  virtual const char*  GetName() const;
  
  virtual void         SetGreen( float value_ );
  virtual float        GetGreen() const;
  
  virtual void         SetRed( float value_ );
  virtual float        GetRed() const;
  
  virtual void         PrintIOStream( std::ostream& o ) const;

  virtual TSeqCollection *      GetList();

//Get rid of Root macros that confuse Doxygen
///\cond CLASSDEF
  ClassDef( HanConfigAlgLimit, 1 ) // A persistable object containing an algorithm limit
///\endcond

protected:

  TObjString m_name;
  Float_t    m_greenValue;
  Float_t    m_redValue;

};

} // namespace dqi

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAlgLimit& l );
std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAlgLimit* l );


#endif

