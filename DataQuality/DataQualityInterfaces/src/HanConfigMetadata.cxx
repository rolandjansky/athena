/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityInterfaces/HanConfigMetadata.h"
#include "DataQualityInterfaces/HanUtils.h"

//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::HanConfigMetadata)
///\endcond

namespace dqi {

HanConfigMetadata::
HanConfigMetadata()
{
  m_metadata = newTList("metadata");
}

HanConfigMetadata::
HanConfigMetadata( const HanConfigMetadata& other )
  :TObject(other),
   m_name(other.m_name)
{
  m_metadata = newTObjArray("metadata", 0, other.m_metadata->GetEntries());
  TIter nextParMap( other.m_metadata );
  HanConfigParMap* otherParMap;
  while( (otherParMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
    HanConfigParMap* parMap = new HanConfigParMap( *otherParMap );
    m_metadata->Add( parMap );
  }
}

HanConfigMetadata& 
HanConfigMetadata::operator=( const HanConfigMetadata& other )
{
  if (this == &other) return *this;

  m_name = other.m_name;
  m_metadata->Delete(); delete m_metadata;
  m_metadata = newTObjArray("metadata", 0, other.m_metadata->GetEntries());
  TIter nextParMap( other.m_metadata );
  HanConfigParMap* otherParMap;
  while( (otherParMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
    HanConfigParMap* parMap = new HanConfigParMap( *otherParMap );
    m_metadata->Add( parMap );
  }

  return *this;
}

HanConfigMetadata::
~HanConfigMetadata(){
  m_metadata->Delete();
  delete m_metadata;
}

void 
HanConfigMetadata::
SetName( std::string name_ )
{
  m_name.SetString(name_.c_str());
}

const char* 
HanConfigMetadata::
GetName() const
{
  return m_name.GetName();
}

void 
HanConfigMetadata::
AddKeyVal( const HanConfigParMap& keyval_ )
{
  HanConfigParMap* parMap = new HanConfigParMap( keyval_ );
  m_metadata->Add( parMap );
}

HanConfigParMap
HanConfigMetadata::
GetKeyVal( std::string name_ ) const
{
  HanConfigParMap* parMap = dynamic_cast<HanConfigParMap*>( m_metadata->FindObject(name_.c_str()) );
  if( parMap == 0 ) {
    return HanConfigParMap();
  }

  return *parMap;
}


TIter
HanConfigMetadata::
GetAllKeyVals() const
{
  return TIter( m_metadata );
}

TSeqCollection *
HanConfigMetadata::
GetList( const TDirectory* /* basedir */, std::map<std::string,TSeqCollection*>& mp )
{
  TSeqCollection* mdList = newTList(GetName());
  mp[mdList->GetName()] = mdList;
  TIter mdIter(m_metadata);
  HanConfigParMap* parMap(0);
  while ((parMap = dynamic_cast<HanConfigParMap*>(mdIter()))) {
    mdList->Add(parMap->GetList());
  }
  return mdList;
}


void 
HanConfigMetadata::
PrintIOStream( std::ostream& o ) const
{
  o << "\nHanConfigMetadata: " << this->GetName() << "\n";

  if (!( m_metadata->IsEmpty())) {
    TIter nextParMap(m_metadata);
    HanConfigParMap* parMap;
    while( (parMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
      o << "    " << parMap;
    }
  }
}

}//namespace dqi

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigMetadata& p ){
  p.PrintIOStream(o);
  return o;
}

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigMetadata* p ){
  p->PrintIOStream(o);
  return o;
}
