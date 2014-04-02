/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataQualityInterfaces/HanConfigCompAlg.h"
#include "DataQualityInterfaces/HanUtils.h"

//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::HanConfigCompAlg)
///\endcond

namespace dqi {

HanConfigCompAlg::
HanConfigCompAlg()
{
  this->subAlgs = newTList("subAlgs");
  this->libs = newTList("libs");
}

HanConfigCompAlg::
HanConfigCompAlg( const HanConfigCompAlg& other )
  :TObject(other),
   name(other.name)
{
  this->subAlgs = newTList("subAlgs");
  this->libs = newTList("libs");
  TIter nextAlg(other.subAlgs);
  TObjString* otherAlg;
  while((otherAlg = dynamic_cast<TObjString*>(nextAlg())) != 0){
    TObjString* alg = new TObjString(*otherAlg);
    this->subAlgs->Add(alg);
  }
  TIter nextLib(other.libs);
  TObjString* otherLib;
  while((otherLib = dynamic_cast<TObjString*>(nextLib())) != 0){
    TObjString* lib = new TObjString(*otherLib);
    this->libs->Add(lib);
  }
}

HanConfigCompAlg& 
HanConfigCompAlg::operator=( const HanConfigCompAlg& other )
{
  if (this == &other) return *this;

  this->name = other.name;
  this->subAlgs = newTList("subAlgs");
  this->libs = newTList("libs");
  TIter nextAlg(other.subAlgs);
  TObjString* otherAlg;
  while((otherAlg = dynamic_cast<TObjString*>(nextAlg())) != 0){
    TObjString* alg = new TObjString(*otherAlg);
    this->subAlgs->Add(alg);
  }
  TIter nextLib(other.libs);
  TObjString* otherLib;
  while((otherLib = dynamic_cast<TObjString*>(nextLib())) != 0){
    TObjString* lib = new TObjString(*otherLib);
    this->libs->Add(lib);
  }
  return *this;
}

HanConfigCompAlg::
~HanConfigCompAlg(){
  this->subAlgs->Delete();
  this->libs->Delete();
  delete subAlgs;
  delete libs;
}

void 
HanConfigCompAlg::
SetName( std::string name )
{
  this->name.SetString(name.c_str());
}

const char* 
HanConfigCompAlg::
GetName() const
{
  return this->name.GetName();
}

void 
HanConfigCompAlg::
AddAlg( std::string alg )
{
  TObjString* newAlg = new TObjString(alg.c_str());
  this->subAlgs->Add(newAlg);
}

TIter 
HanConfigCompAlg::
GetAllAlg() const
{
  return TIter(this->subAlgs);
}

void 
HanConfigCompAlg::
AddLib( std::string lib )
{
  TObjString* newLib = new TObjString(lib.c_str());
  this->libs->Add(newLib);
}

TIter 
HanConfigCompAlg::
GetAllLib() const
{
  return TIter(this->libs);
}

void 
HanConfigCompAlg::
PrintIOStream( std::ostream& o ) const
{
  o << "\nHanConfigCompAlg: " << this->GetName() << "\n";

  if (!( subAlgs->IsEmpty())) {
    o << " Sub-Algorithms = {\n";
    TIter nextAlg(subAlgs);
    TObjString* alg;
    while((alg=dynamic_cast<TObjString*>(nextAlg()))!=0){
      o << "    " << alg;
    }
    o << " }\n";
  }
  if (!( libs->IsEmpty())) {
    o << " Libraries = {\n";
    TIter nextLib(libs);
    TObjString* lib;
    while((lib=dynamic_cast<TObjString*>(nextLib()))!=0){
      o << "    " << lib;
    }
    o << " }\n";
  }
}

void HanConfigCompAlg::Streamer(TBuffer &R__b)
{
   // Stream an object of class dqi::HanConfigCompAlg.

   if (R__b.IsReading()) {
     //delete subAlgs; delete libs;
      R__b.ReadClassBuffer(dqi::HanConfigCompAlg::Class(),this);
   } else {
      R__b.WriteClassBuffer(dqi::HanConfigCompAlg::Class(),this);
   }
}

}//namespace dqi

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigCompAlg& p ){
  p.PrintIOStream(o);
  return o;
}

std::ostream& operator<<( std::ostream& o, const dqi::HanConfigCompAlg* p ){
  p->PrintIOStream(o);
  return o;
}
