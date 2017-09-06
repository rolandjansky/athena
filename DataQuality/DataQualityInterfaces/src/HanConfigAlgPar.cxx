/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanConfigAlgPar.cxx,v 1.4 2007-12-14 16:04:40 mgwilson Exp $
// **********************************************************************

#include "DataQualityInterfaces/HanConfigAlgPar.h"

#include <iomanip>
#include <sstream>

#include <TObjString.h>


//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::HanConfigAlgPar)
///\endcond


namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

HanConfigAlgPar::
HanConfigAlgPar()
  : m_name("")
  , m_value(0.0)
{
}


HanConfigAlgPar::
~HanConfigAlgPar()
{
}


void
HanConfigAlgPar::
SetName( std::string name_ )
{
  m_name.SetString( name_.c_str() );
}


const char*
HanConfigAlgPar::
GetName() const
{
  return m_name.GetName();
}


void
HanConfigAlgPar::
SetValue( float value_ )
{
  m_value = value_;
}


float
HanConfigAlgPar::
GetValue() const
{
  return m_value;
}


void
HanConfigAlgPar::
PrintIOStream( std::ostream& o ) const
{
  o << GetName() << " = " << GetValue() << "\n";
}

TList *
HanConfigAlgPar::
GetList()
{
	TList *ret = new TList();
	
  std::ostringstream parValStr;
  parValStr << std::setprecision(4) << this->m_value;
	ret->SetName( this->GetName() );
	ret->Add( new TObjString(parValStr.str().c_str()) );
	
	return ret;
}

} // namespace dqi


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAlgPar& p )
{
  p.PrintIOStream(o);
  return o;
}


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAlgPar* p )
{
  p->PrintIOStream(o);
  return o;
}

