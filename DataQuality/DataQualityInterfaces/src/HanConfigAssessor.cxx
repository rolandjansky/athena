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
  m_algPars = newTObjArray("algPars");
  m_algLimits = newTObjArray("algLimits");
  m_annotations = newTObjArray("annotations");
  //m_algPars = 0;
  //m_algLimits = 0;
  //m_annotations = 0;
  m_weight = 1;
  m_isRegex = false;
}


HanConfigAssessor::
HanConfigAssessor( const HanConfigAssessor& other )
  : TObject(other)
  , m_name(other.m_name)
  , m_algName(other.m_algName)
  , m_algLibName(other.m_algLibName)
  , m_algRefName(other.m_algRefName)
  , m_weight(other.m_weight)
  , m_isRegex(other.m_isRegex)
{
  m_algPars = newTObjArray("algPars", 0, other.m_algPars->GetEntries());
  m_algLimits = newTObjArray("algLimits", 0, other.m_algLimits->GetEntries());
  m_annotations = newTObjArray("annotations", 0, other.m_annotations->GetEntries());
  TIter nextPar( other.m_algPars );
  HanConfigAlgPar* otherPar;
  while( (otherPar = dynamic_cast<HanConfigAlgPar*>( nextPar() )) != 0 ) {
    HanConfigAlgPar* par = new HanConfigAlgPar( *otherPar );
    m_algPars->Add( par );
  }
  
  TIter nextLim( other.m_algLimits );
  HanConfigAlgLimit* otherLim;
  while( (otherLim = dynamic_cast<HanConfigAlgLimit*>( nextLim() )) != 0 ) {
    HanConfigAlgLimit* lim = new HanConfigAlgLimit( *otherLim );
    m_algLimits->Add( lim );
  }
  
  TIter nextParMap( other.m_annotations );
  HanConfigParMap* otherParMap;
  while( (otherParMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
    HanConfigParMap* parMap = new HanConfigParMap( *otherParMap );
    m_annotations->Add( parMap );
  }
}


HanConfigAssessor&
HanConfigAssessor::
operator=( const HanConfigAssessor& other )
{
  if (this == &other) return *this;

  m_name       = other.m_name;
  m_algName    = other.m_algName;
  m_algLibName = other.m_algLibName;
  m_algRefName = other.m_algRefName;
  m_weight     = other.m_weight;
  m_isRegex    = other.m_isRegex;
  
  m_algPars->Delete();
  TIter nextPar( other.m_algPars );
  HanConfigAlgPar* otherPar;
  while( (otherPar = dynamic_cast<HanConfigAlgPar*>( nextPar() )) != 0 ) {
    HanConfigAlgPar* par = new HanConfigAlgPar( *otherPar );
    m_algPars->Add( par );
  }
  
  m_algLimits->Delete();
  TIter nextLim( other.m_algLimits );
  HanConfigAlgLimit* otherLim;
  while( (otherLim = dynamic_cast<HanConfigAlgLimit*>( nextLim() )) != 0 ) {
    HanConfigAlgLimit* lim = new HanConfigAlgLimit( *otherLim );
    m_algLimits->Add( lim );
  }
  
  m_annotations->Delete();
  TIter nextParMap( other.m_annotations );
  HanConfigParMap* otherParMap;
  while( (otherParMap = dynamic_cast<HanConfigParMap*>( nextParMap() )) != 0 ) {
    HanConfigParMap* parMap = new HanConfigParMap( *otherParMap );
    m_annotations->Add( parMap );
  }

  return *this;
}


HanConfigAssessor::
~HanConfigAssessor()
{
  m_algPars->Delete();
  m_algLimits->Delete();
  m_annotations->Delete();
  delete m_algPars;
  delete m_algLimits;
  delete m_annotations;
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
  m_name.SetString( name_.c_str() );
}


const char*
HanConfigAssessor::
GetName() const
{
  return m_name.GetName();
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
  m_algName.SetString( name_.c_str() );
}


const char*
HanConfigAssessor::
GetAlgName() const
{
  return m_algName.GetName();
}


void
HanConfigAssessor::
SetAlgLibName( std::string name_ )
{
  m_algLibName.SetString( name_.c_str() );
}


const char*
HanConfigAssessor::
GetAlgLibName() const
{
  return m_algLibName.GetName();
}


void
HanConfigAssessor::
SetAlgRefName( std::string name_ )
{
  m_algRefName.SetString( name_.c_str() );
}


std::string
HanConfigAssessor::
GetAlgRefName() const
{
  dqi::ConditionsSingleton& condSingleton=dqi::ConditionsSingleton::getInstance();
  return condSingleton.conditionalSelect(std::string(m_algRefName.GetName()),std::string(condSingleton.getCondition()));
}


const char*
HanConfigAssessor::
GetAlgRefString() const
{
  return m_algRefName.GetName();
}


void
HanConfigAssessor::
AddAlgPar( const HanConfigAlgPar& algPar_ )
{
  HanConfigAlgPar* par = new HanConfigAlgPar( algPar_ );
  m_algPars->Add( par );
}


HanConfigAlgPar
HanConfigAssessor::
GetAlgPar( std::string name_ ) const
{
  HanConfigAlgPar* par = dynamic_cast<HanConfigAlgPar*>( m_algPars->FindObject(name_.c_str()) );
  if( par == 0 ) {
    return HanConfigAlgPar();
  }
  
  return *par;
}


TIter
HanConfigAssessor::
GetAllAlgPars() const
{
  return TIter( m_algPars );
}


void
HanConfigAssessor::
AddAlgLimit( const HanConfigAlgLimit& algLim_ )
{
  HanConfigAlgLimit* lim = new HanConfigAlgLimit( algLim_ );
  m_algLimits->Add( lim );
}


HanConfigAlgLimit
HanConfigAssessor::
GetAlgLimit( std::string name_ ) const
{
  HanConfigAlgLimit* lim = dynamic_cast<HanConfigAlgLimit*>( m_algLimits->FindObject(name_.c_str()) );
  if( lim == 0 ) {
    return HanConfigAlgLimit();
  }
  
  return *lim;
}


TIter
HanConfigAssessor::
GetAllAlgLimits() const
{
  return TIter( m_algLimits );
}

void
HanConfigAssessor::
AddAnnotation( const HanConfigParMap& annotation_ )
{
  HanConfigParMap* parMap = new HanConfigParMap( annotation_ );
  m_annotations->Add( parMap );
}


const HanConfigParMap* 
HanConfigAssessor::
GetAnnotation( std::string name_ ) const
{
  HanConfigParMap* parMap = dynamic_cast<HanConfigParMap*>( m_annotations->FindObject(name_.c_str()) );
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
  return TIter( m_annotations );
}

void
HanConfigAssessor::
SetWeight( float weight_ )
{
  m_weight = weight_;
}

float
HanConfigAssessor::
GetWeight() const
{
  return m_weight;
}

void
HanConfigAssessor::
SetIsRegex( bool isRegex_ )
{
  m_isRegex = isRegex_;
}

bool
HanConfigAssessor::
GetIsRegex() const
{
  return m_isRegex;
}

TSeqCollection *
HanConfigAssessor::
GetList( TDirectory* basedir, std::map<std::string,TSeqCollection*>& mp )
{
  TSeqCollection *ret = newTList( (std::string(this->m_name.GetName())+std::string("_")).c_str());
  TSeqCollection *configList = newTList("Config");
  TSeqCollection *resultsList = newTList("Results");
  TSeqCollection *nameList = newTObjArray("name", new TObjString(m_algName), 1);
	
  std::string nameString = GetUniqueName();
  
  mp[nameString] = ret;

  // fill the configList
  configList->Add(nameList); 
  if (m_algPars == 0)
    std::cerr << "HanConfigAssessor::GetList(): Warning: m_algPars == 0\n";
  else {
    TIter nextAlgPar( m_algPars );
    HanConfigAlgPar *par;
    TSeqCollection *algParList = newTObjArray("algPars", 0, m_algPars->GetEntries());
    while( (par = dynamic_cast<HanConfigAlgPar*>( nextAlgPar() )) != 0 )
      algParList->Add(par->GetList());
    
    if (algParList->IsEmpty())
      delete algParList;
    else
      configList->Add(algParList);
  }

  if (m_algLimits == 0)
    std::cerr << "HanConfigAssessor::GetList(): Warning: m_algLimits == 0\n";
  else {
    TIter nextAlgLim( m_algLimits );
    HanConfigAlgLimit *lim;
    TSeqCollection *algLimitList = newTObjArray("algLimits", 0, m_algLimits->GetEntries());
    while( (lim = dynamic_cast<HanConfigAlgLimit*>( nextAlgLim() )) != 0 )
      algLimitList->Add(lim->GetList());
    if(algLimitList->IsEmpty())
      delete algLimitList;
    else
      configList->Add(algLimitList);
  }

  TSeqCollection *parMapList(0);
  if (m_annotations == 0)
    std::cerr << "HanConfigAssessor::GetList(): Warning: annotations == 0\n";
  else {
    TIter nextParMap( m_annotations );
    HanConfigParMap *parMap;
    parMapList = newTObjArray("annotations", 0, m_annotations->GetEntries()+2);
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

  if (m_isRegex) {
    o << "  Is a regular expression" << std::endl;
  }
  
  if( !m_algPars->IsEmpty() ) {
    o << "  Algorithm Parameters = {\n";
    TIter nextPar( m_algPars );
    HanConfigAlgPar* par;
    while( (par = dynamic_cast<HanConfigAlgPar*>( nextPar() )) != 0 ) {
      o << "    " << par;
    }
    o << "  }\n";
  }
  
  if( !m_algLimits->IsEmpty() ) {
    o << "  Algorithm Limits = {\n";
    TIter nextLim( m_algLimits );
    HanConfigAlgLimit* lim;
    while( (lim = dynamic_cast<HanConfigAlgLimit*>( nextLim() )) != 0 ) {
      o << "    " << lim;
    }
    o << "  }\n";
  }
 
  if( !m_annotations->IsEmpty() ) {
    o << "  Annotations = {\n";
    TIter nextParMap( m_annotations );
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
     m_name.Streamer(R__b);
     m_algName.Streamer(R__b);
     m_algLibName.Streamer(R__b);
     m_algRefName.Streamer(R__b);
     delete m_algPars; delete m_algLimits;
     R__b >> m_algPars;
     R__b >> m_algLimits;
     if (R__v >= 2) {
       delete m_annotations;
       R__b >> m_annotations;
     }
     if (R__v >= 3) {
       R__b >> m_weight;
     }
     if (R__v >= 4) {
       R__b >> m_isRegex;
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

