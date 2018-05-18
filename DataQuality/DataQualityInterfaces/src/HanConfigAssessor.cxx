/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanConfigAssessor.cxx,v 1.17 2009-02-04 09:39:47 ponyisi Exp $
// **********************************************************************

#include "DataQualityInterfaces/HanConfigAssessor.h"
#include "DataQualityInterfaces/HanUtils.h"
#include "DataQualityInterfaces/ConditionsSingleton.h"
#include <cstring>

#include <TKey.h>
#include <TH1F.h>
#include <TClass.h>


//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::HanConfigAssessor)
///\endcond



namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

HanConfigAssessor::
HanConfigAssessor()
{
  algPars = newTObjArray("algPars");
  algLimits = newTObjArray("algLimits");
  annotations = newTObjArray("annotations");
  //algPars = 0;
  //algLimits = 0;
  //annotations = 0;
  weight = 1;
  isRegex = false;
}


HanConfigAssessor::
HanConfigAssessor( const HanConfigAssessor& other )
  : TObject(other)
  , name(other.name)
  , algName(other.algName)
  , algLibName(other.algLibName)
  , algRefName(other.algRefName)
  , weight(other.weight)
  , isRegex(other.isRegex)
{
  algPars = newTObjArray("algPars", 0, other.algPars->GetEntries());
  algLimits = newTObjArray("algLimits", 0, other.algLimits->GetEntries());
  annotations = newTObjArray("annotations", 0, other.annotations->GetEntries());
  TIter nextPar( other.algPars );
  HanConfigAlgPar* otherPar;
  while( (otherPar = dynamic_cast<HanConfigAlgPar*>( nextPar() )) != 0 ) {
    HanConfigAlgPar* par = new HanConfigAlgPar( *otherPar );
    algPars->Add( par );
  }
  
  TIter nextLim( other.algLimits );
  HanConfigAlgLimit* otherLim;
  while( (otherLim = dynamic_cast<HanConfigAlgLimit*>( nextLim() )) != 0 ) {
    HanConfigAlgLimit* lim = new HanConfigAlgLimit( *otherLim );
    algLimits->Add( lim );
  }
  
  TIter nextParMap( other.annotations );
  HanConfigParMap* otherParMap;
  while( (otherParMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
    HanConfigParMap* parMap = new HanConfigParMap( *otherParMap );
    annotations->Add( parMap );
  }
}


HanConfigAssessor&
HanConfigAssessor::
operator=( const HanConfigAssessor& other )
{
  if (this == &other) return *this;

  name       = other.name;
  algName    = other.algName;
  algLibName = other.algLibName;
  algRefName = other.algRefName;
  weight     = other.weight;
  isRegex    = other.isRegex;
  
  algPars->Delete();
  TIter nextPar( other.algPars );
  HanConfigAlgPar* otherPar;
  while( (otherPar = dynamic_cast<HanConfigAlgPar*>( nextPar() )) != 0 ) {
    HanConfigAlgPar* par = new HanConfigAlgPar( *otherPar );
    algPars->Add( par );
  }
  
  algLimits->Delete();
  TIter nextLim( other.algLimits );
  HanConfigAlgLimit* otherLim;
  while( (otherLim = dynamic_cast<HanConfigAlgLimit*>( nextLim() )) != 0 ) {
    HanConfigAlgLimit* lim = new HanConfigAlgLimit( *otherLim );
    algLimits->Add( lim );
  }
  
  annotations->Delete();
  TIter nextParMap( other.annotations );
  HanConfigParMap* otherParMap;
  while( (otherParMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
    HanConfigParMap* parMap = new HanConfigParMap( *otherParMap );
    annotations->Add( parMap );
  }

  return *this;
}


HanConfigAssessor::
~HanConfigAssessor()
{
  algPars->Delete();
  algLimits->Delete();
  annotations->Delete();
  delete algPars;
  delete algLimits;
  delete annotations;
}


std::string
HanConfigAssessor::
GetUniqueName() const
{
  return std::string( GetName() );
}


void
HanConfigAssessor::
SetName( std::string name_ )
{
  name.SetString( name_.c_str() );
}


const char*
HanConfigAssessor::
GetName() const
{
  return name.GetName();
}

const char*
HanConfigAssessor::
GetHistPath() const
{
  const HanConfigParMap* parmap = GetAnnotation("inputname");
  if (!parmap) {
    return GetName();
  } else {
    return parmap->GetValue();
  }
}

void
HanConfigAssessor::
SetAlgName( std::string name_ )
{
  algName.SetString( name_.c_str() );
}


const char*
HanConfigAssessor::
GetAlgName() const
{
  return algName.GetName();
}


void
HanConfigAssessor::
SetAlgLibName( std::string name_ )
{
  algLibName.SetString( name_.c_str() );
}


const char*
HanConfigAssessor::
GetAlgLibName() const
{
  return algLibName.GetName();
}


void
HanConfigAssessor::
SetAlgRefName( std::string name_ )
{
  algRefName.SetString( name_.c_str() );
}


std::string
HanConfigAssessor::
GetAlgRefName() const
{
  dqi::ConditionsSingleton& condSingleton=dqi::ConditionsSingleton::getInstance();
  return condSingleton.conditionalSelect(std::string(algRefName.GetName()),std::string(condSingleton.getCondition()));
}


const char*
HanConfigAssessor::
GetAlgRefString() const
{
  return algRefName.GetName();
}


void
HanConfigAssessor::
AddAlgPar( const HanConfigAlgPar& algPar_ )
{
  HanConfigAlgPar* par = new HanConfigAlgPar( algPar_ );
  algPars->Add( par );
}


HanConfigAlgPar
HanConfigAssessor::
GetAlgPar( std::string name_ ) const
{
  HanConfigAlgPar* par = dynamic_cast<HanConfigAlgPar*>( algPars->FindObject(name_.c_str()) );
  if( par == 0 ) {
    return HanConfigAlgPar();
  }
  
  return *par;
}


TIter
HanConfigAssessor::
GetAllAlgPars() const
{
  return TIter( algPars );
}


void
HanConfigAssessor::
AddAlgLimit( const HanConfigAlgLimit& algLim_ )
{
  HanConfigAlgLimit* lim = new HanConfigAlgLimit( algLim_ );
  algLimits->Add( lim );
}


HanConfigAlgLimit
HanConfigAssessor::
GetAlgLimit( std::string name_ ) const
{
  HanConfigAlgLimit* lim = dynamic_cast<HanConfigAlgLimit*>( algLimits->FindObject(name_.c_str()) );
  if( lim == 0 ) {
    return HanConfigAlgLimit();
  }
  
  return *lim;
}


TIter
HanConfigAssessor::
GetAllAlgLimits() const
{
  return TIter( algLimits );
}

void
HanConfigAssessor::
AddAnnotation( const HanConfigParMap& annotation_ )
{
  HanConfigParMap* parMap = new HanConfigParMap( annotation_ );
  annotations->Add( parMap );
}


const HanConfigParMap* 
HanConfigAssessor::
GetAnnotation( std::string name_ ) const
{
  HanConfigParMap* parMap = dynamic_cast<HanConfigParMap*>( annotations->FindObject(name_.c_str()) );
  if( parMap == 0 ) {
    //std::cerr << "WARNING: attempt to access non-existent annotation " << name_ << " on assessor " << GetName() << std::endl;
    return 0;
  }
  return parMap;
}


TIter
HanConfigAssessor::
GetAllAnnotations() const
{
  return TIter( annotations );
}

void
HanConfigAssessor::
SetWeight( float weight_ )
{
  weight = weight_;
}

float
HanConfigAssessor::
GetWeight() const
{
  return weight;
}

void
HanConfigAssessor::
SetIsRegex( bool isRegex_ )
{
  isRegex = isRegex_;
}

bool
HanConfigAssessor::
GetIsRegex() const
{
  return isRegex;
}

TSeqCollection *
HanConfigAssessor::
GetList( TDirectory* basedir, std::map<std::string,TSeqCollection*>& mp )
{
  TSeqCollection *ret = newTList( (std::string(this->name.GetName())+std::string("_")).c_str());
  TSeqCollection *configList = newTList("Config");
  TSeqCollection *resultsList = newTList("Results");
  TSeqCollection *nameList = newTObjArray("name", new TObjString(algName), 1);
	
  std::string nameString = GetUniqueName();
  
  mp[nameString] = ret;

  // fill the configList
  configList->Add(nameList); 
  if (algPars == 0)
    std::cerr << "HanConfigAssessor::GetList(): Warning: algPars == 0\n";
  else {
    TIter nextAlgPar( algPars );
    HanConfigAlgPar *par;
    TSeqCollection *algParList = newTObjArray("algPars", 0, algPars->GetEntries());
    while( (par = dynamic_cast<HanConfigAlgPar*>( nextAlgPar() )) != 0 )
      algParList->Add(par->GetList());
    
    if (algParList->IsEmpty())
      delete algParList;
    else
      configList->Add(algParList);
  }

  if (algLimits == 0)
    std::cerr << "HanConfigAssessor::GetList(): Warning: algLimits == 0\n";
  else {
    TIter nextAlgLim( algLimits );
    HanConfigAlgLimit *lim;
    TSeqCollection *algLimitList = newTObjArray("algLimits", 0, algLimits->GetEntries());
    while( (lim = dynamic_cast<HanConfigAlgLimit*>( nextAlgLim() )) != 0 )
      algLimitList->Add(lim->GetList());
    if(algLimitList->IsEmpty())
      delete algLimitList;
    else
      configList->Add(algLimitList);
  }

  TSeqCollection *parMapList(0);
  if (annotations == 0)
    std::cerr << "HanConfigAssessor::GetList(): Warning: annotations == 0\n";
  else {
    TIter nextParMap( annotations );
    HanConfigParMap *parMap;
    parMapList = newTObjArray("annotations", 0, annotations->GetEntries()+2);
    while( (parMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 )
      parMapList->Add(parMap->GetList());
  }
  std::string usedAlgRef = GetAlgRefName();
  if ( parMapList && usedAlgRef.size() ) {
    TList* refSourceList = new TList();
    std::string refOrigin(ConditionsSingleton::getInstance().getRefSourceData(usedAlgRef));
    refSourceList->SetName("refSource");
    refSourceList->Add(new TObjString(refOrigin.c_str()));
    parMapList->Add(refSourceList);
    if (refOrigin != "Multiple references") {
      std::string refInfo(ConditionsSingleton::getInstance().getRefSourceData(refOrigin));
      if (refInfo != "") {
	refSourceList = new TList();
	refSourceList->SetName("refInfo");
	refSourceList->Add(new TObjString(refInfo.c_str()));
	parMapList->Add(refSourceList);
      }
    }
  }
  if(!parMapList || parMapList->IsEmpty())
    delete parMapList;
  else
    configList->Add(parMapList);

  // Get the histogram
  if (! GetIsRegex()) {
    TKey* key = getObjKey( basedir, GetHistPath() );
    if( key != 0 ) {
      const char* className = key->GetClassName();
      if( (strncmp(className, "TH", 2) == 0) 
       || (strncmp(className, "TGraph", 6) == 0) 
       || (strncmp(className, "TProfile", 8) == 0) 
       || (strncmp(className, "TEfficiency", 11) == 0) ) {
	// TNamed* transobj = dynamic_cast<TNamed*>(key->ReadObj());
	// if (transobj != NULL) {
	std::string::size_type rslash = nameString.rfind("/");
	if (rslash == std::string::npos) {
	  rslash = 0;
	} else {
	  rslash += 1;
	}
	HanHistogramLink* hhl = new HanHistogramLink(basedir, GetHistPath());
	hhl->SetName( nameString.substr(rslash, std::string::npos).c_str() );
	ret->Add(hhl);
	//   transobj->SetName( nameString.substr(rslash, std::string::npos).c_str() );
	// } else {
	//   std::cerr << "TNamed* cast failed for " << GetHistPath() 
	// 	    << std::endl;
	//   ret->Add(transobj);
	//  }
	
      }
    }
  }

  
  // fill the list that we return
  ret->Add(configList);
  ret->Add(resultsList);
  
  return ret;
}


void
HanConfigAssessor::
Accept( const Visitor& visitor, boost::shared_ptr<dqm_core::Region> dqParent ) const
{
  visitor.Visit( this, dqParent );
}


void
HanConfigAssessor::
PrintIOStream( std::ostream& o ) const
{
  o << "\nHanConfigAssessor: " << GetName() << "\n"
    << "  Algorithm Name = \"" << GetAlgName() << "\"\n"
    << "  Algorithm Library = \"" << GetAlgLibName() << "\"\n"
    << "  Algorithm Reference = \"" << GetAlgRefString() << "\"\n"
    << "  Weight = " << GetWeight() << "\n";

  if (isRegex) {
    o << "  Is a regular expression" << std::endl;
  }
  
  if( !algPars->IsEmpty() ) {
    o << "  Algorithm Parameters = {\n";
    TIter nextPar( algPars );
    HanConfigAlgPar* par;
    while( (par = dynamic_cast<HanConfigAlgPar*>( nextPar() )) != 0 ) {
      o << "    " << par;
    }
    o << "  }\n";
  }
  
  if( !algLimits->IsEmpty() ) {
    o << "  Algorithm Limits = {\n";
    TIter nextLim( algLimits );
    HanConfigAlgLimit* lim;
    while( (lim = dynamic_cast<HanConfigAlgLimit*>( nextLim() )) != 0 ) {
      o << "    " << lim;
    }
    o << "  }\n";
  }
 
  if( !annotations->IsEmpty() ) {
    o << "  Annotations = {\n";
    TIter nextParMap( annotations );
    HanConfigParMap* parMap;
    while( (parMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
      o << "    " << parMap;
    }
    o << "  }\n";
  }


}

void HanConfigAssessor::Streamer(TBuffer &R__b)
{
   // Stream an object of class HanConfigAssessor.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
     // Automatic schema evolution is kind of broken
     TObject::Streamer(R__b);
     name.Streamer(R__b);
     algName.Streamer(R__b);
     algLibName.Streamer(R__b);
     algRefName.Streamer(R__b);
     delete algPars; delete algLimits;
     R__b >> algPars;
     R__b >> algLimits;
     if (R__v >= 2) {
       delete annotations;
       R__b >> annotations;
     }
     if (R__v >= 3) {
       R__b >> weight;
     }
     if (R__v >= 4) {
       R__b >> isRegex;
     }

     R__b.CheckByteCount(R__s, R__c, HanConfigAssessor::IsA());
   } else {
      HanConfigAssessor::Class()->WriteBuffer(R__b, this);
   }
}

} // namespace dqi


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAssessor& p )
{
  p.PrintIOStream(o);
  return o;
}


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigAssessor* p )
{
  p->PrintIOStream(o);
  return o;
}

