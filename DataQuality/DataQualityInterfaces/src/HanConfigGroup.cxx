/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: HanConfigGroup.cxx,v 1.14 2008-12-04 16:40:16 ponyisi Exp $
// **********************************************************************

#include "DataQualityInterfaces/HanConfigGroup.h"

#include "dqm_core/Parameter.h"
#include "dqm_core/Region.h"


//Get rid of Root macros that confuse Doxygen
///\cond CLASSIMP
ClassImp(dqi::HanConfigGroup)
///\endcond


namespace dqi {

// *********************************************************************
// Public Methods
// *********************************************************************

HanConfigGroup::
HanConfigGroup()
{
}


HanConfigGroup::
HanConfigGroup( const HanConfigGroup& other )
  : HanConfigAssessor(other)
  , m_pathName(other.m_pathName)
{
  TIter nextAssess( &other.m_assessors );
  HanConfigAssessor* otherAssess;
  while( (otherAssess = dynamic_cast<HanConfigAssessor*>( nextAssess() )) != 0 ) {
    HanConfigAssessor* acpy = new HanConfigAssessor( *otherAssess );
    m_assessors.Add( acpy );
  }
  
  TIter nextGroup( &other.m_groups );
  HanConfigGroup* otherGroup;
  while( (otherGroup = dynamic_cast<HanConfigGroup*>( nextGroup() )) != 0 ) {
    HanConfigGroup* gcpy = new HanConfigGroup( *otherGroup );
    m_groups.Add( gcpy );
  }
}


HanConfigGroup&
HanConfigGroup::
operator=( const HanConfigGroup& other )
{
  HanConfigAssessor::operator=(other);
  m_pathName = other.m_pathName;
  
  TIter nextAssess( &other.m_assessors );
  HanConfigAssessor* otherAssess;
  while( (otherAssess = dynamic_cast<HanConfigAssessor*>( nextAssess() )) != 0 ) {
    HanConfigAssessor* acpy = new HanConfigAssessor( *otherAssess );
    m_assessors.Add( acpy );
  }
  
  TIter nextGroup( &other.m_groups );
  HanConfigGroup* otherGroup;
  while( (otherGroup = dynamic_cast<HanConfigGroup*>( nextGroup() )) != 0 ) {
    HanConfigGroup* gcpy = new HanConfigGroup( *otherGroup );
    m_groups.Add( gcpy );
  }

  return *this;
}


HanConfigGroup::
~HanConfigGroup()
{
  // Assume that the same object has not been added to both lists
  m_assessors.Delete();
  m_groups.Delete();
}


std::string
HanConfigGroup::
GetUniqueName() const
{
  return std::string( GetPathName() );
}


void
HanConfigGroup::
SetPathName( const std::string& name_ )
{
  m_pathName.SetString( name_.c_str() );
}


const char*
HanConfigGroup::
GetPathName() const
{
  return m_pathName.GetName();
}


void
HanConfigGroup::
AddAssessor( const HanConfigAssessor& hcass_ )
{
  HanConfigAssessor* acpy = new HanConfigAssessor( hcass_ );
  m_assessors.Add( acpy );
}


const HanConfigAssessor 
HanConfigGroup::
GetAssessor( const std::string& name_ ) const
{
  HanConfigAssessor* hca = dynamic_cast<HanConfigAssessor*>( m_assessors.FindObject(name_.c_str()) );
  if( hca == 0 ) {
    return HanConfigAssessor();
  }
  
  return *hca;
}


TIter
HanConfigGroup::
GetAllAssessors() const
{
  return TIter( &m_assessors );
}


void
HanConfigGroup::
AddGroup( const HanConfigGroup& hcg_ )
{
  HanConfigGroup* gcpy = new HanConfigGroup( hcg_ );
  m_groups.Add( gcpy );
}


HanConfigGroup
HanConfigGroup::
GetGroup( const std::string& name_ ) const
{
  HanConfigGroup* hcg = dynamic_cast<HanConfigGroup*>( m_groups.FindObject(name_.c_str()) );
  if( hcg == 0 ) {
    return HanConfigGroup();
  }
  
  return *hcg;
}


TIter
HanConfigGroup::
GetAllGroups() const
{
  return TIter( &m_groups );
}


HanConfigGroup*
HanConfigGroup::
GetNode( const std::string& name_ ) const
{
  if( m_groups.IsEmpty() ) {
    return 0;
  }

  std::string::size_type k = name_.find_first_of('/');
  if( k != std::string::npos ) {
    std::string dName( name_, 0, k );
    std::string pName( name_, k+1, std::string::npos );
    if( dName != "" ) {
      if( dName == std::string( GetName() ) ) {
        return GetNode( pName );
      }
      HanConfigGroup* subreg = dynamic_cast<HanConfigGroup*>( m_groups.FindObject(dName.c_str()) );
      if( subreg == 0 ) {
        return 0;
      }
      return subreg->GetNode( pName );
    }
    return GetNode( pName );
  }
  
  HanConfigGroup* subreg = dynamic_cast<HanConfigGroup*>( m_groups.FindObject(name_.c_str()) );
  return subreg;
}


TSeqCollection *
HanConfigGroup::
GetList( TDirectory* basedir, std::map<std::string,TSeqCollection*>& mp )
{
  // Let the original method do all the work
  TSeqCollection *ret = HanConfigAssessor::GetList( basedir, mp );
  ret->SetName( this->m_name.GetName() );

  // Iterate through containing m_groups and m_assessors and add them to the childrenList
  TIter nextAssess( &m_assessors );
  HanConfigAssessor* hca;
  while( (hca = dynamic_cast<HanConfigAssessor*>( nextAssess() )) != 0 )
    ret->Add( hca->GetList(basedir,mp) );

  TIter nextGroup( &m_groups );
  HanConfigGroup* hcg;
  while( (hcg = dynamic_cast<HanConfigGroup*>( nextGroup() )) != 0 )
    ret->Add( hcg->GetList(basedir,mp) );

	return ret;
}


void
HanConfigGroup::
Accept( const Visitor& visitor, boost::shared_ptr<dqm_core::Region> dqParent ) const
{
  std::string nodeName( GetName() );
  boost::shared_ptr<dqm_core::Region> dqr;
  if( nodeName != "top_level" ) {
    boost::shared_ptr<dqm_core::Node> dqp(visitor.Visit( this, dqParent ));
    dqr = boost::dynamic_pointer_cast<dqm_core::Region>( dqp );
  }
  else {
    dqr = dqParent;
  }

  // Accept the same visitor on all containing m_groups and m_assessors

  TIter nextAssess( &m_assessors );
  HanConfigAssessor* hca;
  while( (hca = dynamic_cast<HanConfigAssessor*>( nextAssess() )) != 0 ) {
    hca->Accept( visitor, dqr );
  }
  
  TIter nextGroup( &m_groups );
  HanConfigGroup* hcg;
  while( (hcg = dynamic_cast<HanConfigGroup*>( nextGroup() )) != 0 ) {
    hcg->Accept( visitor, dqr );
  }
}


void
HanConfigGroup::
PrintIOStream( std::ostream& o ) const
{
  o << "\nHanConfigGroup: " << GetName() << "\n"
    << "  Algorithm Name = \"" << GetAlgName() << "\"\n"
    << "  Algorithm Library = \"" << GetAlgLibName() << "\"\n"
    << "  Algorithm Reference = \"" << GetAlgRefName() << "\"\n"
    << "  Weight = " << GetWeight() << "\n";
  
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
  
  if( !m_groups.IsEmpty() ) {
    o << "\n>>  BEGIN SUB REGIONS of \"" << GetName() << "\"\n";
    TIter nextGroup( &m_groups );
    HanConfigGroup* hcg;
    while( (hcg = dynamic_cast<HanConfigGroup*>( nextGroup() )) != 0 ) {
      o << hcg;
    }
    o << "\n<<  END SUB REGIONS of \"" << GetName() << "\"\n";
  }
  
  if( !m_assessors.IsEmpty() ) {
    o << "\n>>  BEGIN ASSESSMENTS of \"" << GetName() << "\"\n";
    TIter nextAssess( &m_assessors );
    HanConfigAssessor* hca;
    while( (hca = dynamic_cast<HanConfigAssessor*>( nextAssess() )) != 0 ) {
      o << hca;
    }
    o << "\n<<  END ASSESSMENTS of \"" << GetName() << "\"\n";
  }

}

} // namespace dqi


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigGroup& g )
{
  g.PrintIOStream(o);
  return o;
}


std::ostream& operator<<( std::ostream& o, const dqi::HanConfigGroup* g )
{
  g->PrintIOStream(o);
  return o;
}

