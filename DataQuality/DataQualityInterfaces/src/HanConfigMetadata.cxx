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
  metadata = newTList("metadata");
}

HanConfigMetadata::
HanConfigMetadata( const HanConfigMetadata& other )
  :TObject(other),
   name(other.name)
{
  metadata = newTObjArray("metadata", 0, other.metadata->GetEntries());
  TIter nextParMap( other.metadata );
  HanConfigParMap* otherParMap;
  while( (otherParMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
    HanConfigParMap* parMap = new HanConfigParMap( *otherParMap );
    metadata->Add( parMap );
  }
}

HanConfigMetadata& 
HanConfigMetadata::operator=( const HanConfigMetadata& other )
{
  if (this == &other) return *this;

  name = other.name;
  metadata->Delete(); delete metadata;
  metadata = newTObjArray("metadata", 0, other.metadata->GetEntries());
  TIter nextParMap( other.metadata );
  HanConfigParMap* otherParMap;
  while( (otherParMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
    HanConfigParMap* parMap = new HanConfigParMap( *otherParMap );
    metadata->Add( parMap );
  }

  return *this;
}

HanConfigMetadata::
~HanConfigMetadata(){
  metadata->Delete();
  delete metadata;
}

void 
HanConfigMetadata::
SetName( std::string name_ )
{
  name.SetString(name_.c_str());
}

const char* 
HanConfigMetadata::
GetName() const
{
  return name.GetName();
}

void 
HanConfigMetadata::
AddKeyVal( const HanConfigParMap& keyval_ )
{
  HanConfigParMap* parMap = new HanConfigParMap( keyval_ );
  metadata->Add( parMap );
}

HanConfigParMap
HanConfigMetadata::
GetKeyVal( std::string name_ ) const
{
  HanConfigParMap* parMap = dynamic_cast<HanConfigParMap*>( metadata->FindObject(name_.c_str()) );
  if( parMap == 0 ) {
    return HanConfigParMap();
  }

  return *parMap;
}


TIter
HanConfigMetadata::
GetAllKeyVals() const
{
  return TIter( metadata );
}

TSeqCollection *
HanConfigMetadata::
GetList( const TDirectory* /* basedir */, std::map<std::string,TSeqCollection*>& mp )
{
  TSeqCollection* mdList = newTList(GetName());
  mp[mdList->GetName()] = mdList;
  TIter mdIter(metadata);
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

  if (!( metadata->IsEmpty())) {
    TIter nextParMap(metadata);
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
