/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////////
// Filename : Jet.cxx
// Author   : Ambreesh Gupta, Ed Frank
// Created  : Sep, 2000
//
// HISTORY;
//
//   21Sep01  Peter Loch   Implement Tucson agreement for Jet class.
//   23Apr02  Peter Loch   Move to JetEvent package
//   10Oct02  Peter Loch   New package structure implemented
//   31Jan03  Peter Loch   New navigation code implemented
//   27Jul03  Ambreesh     Modified addJet method to add a constituent
//                         if it does not belong to the jet. 
//   11Feb08  PL & RS      new JetEDM: new inheritance structure and
//                         information stores for b-tagging attached
//   Nov09    PO & PA      Externalize Signal States to ParticleSigStateImpl
//                         Remove pre/post processors
//
///////////////////////////////////////////////////////////////////////////////

// externals
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/LorentzVector.h"

// Navigation
#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h" 

// FourMom
#include "FourMom/P4PxPyPzE.h"
#include "FourMom/P4PtEtaPhiM.h"
#include "FourMom/P4EEtaPhiM.h"
#include "NavFourMom/INavigable4MomentumCollection.h"

// JetEvent
#include "JetEvent/JetStoreHelper.h"
#include "JetEvent/Jet.h"
#include "JetEvent/JetKeyDescriptor.h"
#include "JetEvent/JetConstituentIterator.h"
#include "JetEvent/JetCollection.h"

// STL
#include <algorithm>
#include <vector>

#include <cmath>
#include <sstream>


const size_t Jet::s_defaultJetId = 0 ;
const size_t Jet::s_defaultJetAuthor = 0 ;

// DO NOT change these lines EVER!!!!!
// std::string Jet::m_shpCat = "JetShapes";
// std::string Jet::m_assCat = "JetAssociations";
// std::string Jet::m_tagCat = "JetTags";
// std::string Jet::m_infCat = "JetInfo";



// size_t Jet::m_invalidKeyIndex    = size_t(-1);
// size_t Jet::m_momMapInvalidIndex = size_t(-1);



double Jet::m_ignoreWeight = 1.0e-06;

// Little helper function
bool jet_component_identity(double p1, double p2, double epsilon = 0.1){
  return fabs(p1-p2)< epsilon ;
}


//////////////////
// Constructors //
//////////////////

#define JET_CONSTRUCTOR_BASE   m_constituentsN(0)   \
    , m_RoIword(0xFFFFFFFF)                         \
    , m_jetId(Jet::s_defaultJetId)                  \
    , m_collection(NULL)			    \
    , m_jetAuthor(Jet::s_defaultJetAuthor)          \
    , m_assocStore(0)                               \
    , m_tagInfoStore(0)                             \
    , m_constituentSigState(P4SignalState::UNKNOWN) \
    , m_num_combinedLikelihood(0)                   \
    , m_pseudoJet(0)


/** @detail Default constructor instantiates empty @c Jet object without
    any constituents and with @f$ P_{jet} = \left(p_x,p_y,p_z,E\right) = 
    (0,0,0,0) @f$.
 */
Jet::Jet() 
  : base_t(3, signalstate_t(1)), 
    JET_CONSTRUCTOR_BASE

{
  this->setup(hlv_t(0.,0.,0.,0.));
}


/** @detail Construct a @c Jet from a @c INavigable4Momentum object referenced
    by the pointer to the storable object collection and the pointer to the
    object.
    
    @param[in]  pColl   pointer to storable object collection of type 
                @c INavigable4MomentumContainer
    @param[in]  pConst  pointer to constituent object of type 
                @c INavigable4Momentum
    @param[in]  wght    optional kinematic weight of constituent, default 1
  
 */
Jet::Jet(const collection_t*  pColl,
	 const constituent_t* pConst,
	 double wght) 
  : base_t(3, signalstate_t(1)), 
JET_CONSTRUCTOR_BASE
{

  this->setup(hlv_t(0.,0.,0.,0.));
  this->addConstituent(pColl,pConst,wght);
}

/** @detail Construct a @c Jet from a @c INavigable4Momentum object referenced
    by the pointer to the storable object collection and the index of the
    object in the collection.
    
    @param[in]  pColl   pointer to storable object collection of type 
                @c INavigable4MomentumContainer
    @param[in]  ind     index of type @c size_t referencing the 
                @c INavigable4Momentum object in its container.
    @param[in]  wght    optional kinematic weight of constituent, default 1
  
 */
Jet::Jet(const collection_t* pColl,index_t ind,double wght)
  : base_t(3, signalstate_t(1)), 
JET_CONSTRUCTOR_BASE{
  this->setup(hlv_t(0.,0.,0.,0.));
  this->addConstituent(pColl,ind,wght);
}


/** @detail Constructs an empty @c Jet object without
    any constituents and with an initial four-momentum given by 
    a $c I4Momentum (base) typed reconstruction object.

    @param[in] pMom pointer to an object with @c I4Momentum interface 
 */
Jet::Jet(const fourmom_t* pMom)
  : base_t(3, signalstate_t(1)), 
JET_CONSTRUCTOR_BASE{
  this->setup(hlv_t(pMom->px(),pMom->py(),pMom->pz(),pMom->e()));
}

/** @detail Constructs an empty @c Jet object without
    any constituents and with an initial four-momentum given by 
    a $c I4Momentum (base) typed reconstruction object.

    @param[in] rMom reference to an object with @c I4Momentum interface 
 */
Jet::Jet(const fourmom_t& rMom)
  : base_t(3, signalstate_t(1)), 
JET_CONSTRUCTOR_BASE{
  this->setup(hlv_t(rMom.px(),rMom.py(),rMom.pz(),rMom.e()));
}
/** @detail Constructs an empty @c Jet object without
    any constituents and with an initial four-momentum given by 
    a $c CLHEP::HepLorentzVector typed reconstruction object.

    @param[in] rVec reference to an object with @c CLHEP::HepLorentzVector interface 
 */
Jet::Jet(const hlv_t& rVec)
  : base_t(3, signalstate_t(1)), 
JET_CONSTRUCTOR_BASE
{
  this->setup(rVec);
}
/** @detail Constructs an empty @c Jet object without
    any constituents and with an initial four-momentum given by a list of
    numbers representing a four-momentum.

    @param[in] px  @f$ p_x @f$
    @param[in] py  @f$ p_y @f$
    @param[in] pz  @f$ p_z @f$
    @param[in] e   @f$ e @f$
 */
Jet::Jet(double px, double py, double pz, double e)
  : base_t(3, signalstate_t(1)), 
JET_CONSTRUCTOR_BASE
{
  this->setup(hlv_t(px,py,pz,e));
}
/** @detail Constructs a copy of a given @c Jet object.

    @param[in] pJet pointer to a @c Jet object
 */
Jet::Jet(const Jet* pJet) 
  : base_t((const base_t) *pJet) , 
    JET_CONSTRUCTOR_BASE
{

  copy_from_jet( pJet, CopyAll);
  
}

// constructor with arguements
Jet::Jet(const Jet* pJet, bool copyStores, bool copyConstituents)
  : base_t( (const base_t) *pJet),
    JET_CONSTRUCTOR_BASE
{

  CopyDataMembers cmd0 = CopyDataMembers ( copyStores ? 
    ( CopyMoments | CopyAssociations | CopyTagInfos ) : CopyNone ) ;
  
  CopyDataMembers cmd1 = CopyDataMembers ( copyConstituents ? CopyConstituents : CopyNone  ) ;

  //std::cout <<"Jet copyStores:"<< copyStores << "  "<< cmd0 << "|  copyConstituents:"<<copyConstituents<< " "<< cmd1<< "  size ="<< this->size()  <<std::endl;

  copy_from_jet( pJet, cmd0 , cmd1);   
}

// constructor with arguements
Jet::Jet(const Jet* pJet,
	 CopyDataMembers cmd0, CopyDataMembers cmd1,
	 CopyDataMembers cmd2, CopyDataMembers cmd3)
  : base_t( (const base_t) *pJet ),
    JET_CONSTRUCTOR_BASE
{
  copy_from_jet( pJet, cmd0 , cmd1, cmd2, cmd3 );
}

/** @detail Constructs a copy of a given @c Jet object.

    @param[in] a @c Jet object
*/

Jet::Jet(const Jet& rJet)
  : IAthenaBarCode(),
    INavigable(),
    I4Momentum(),
    INavigable4Momentum(),
    base_t(rJet),
    JET_CONSTRUCTOR_BASE
{
  copy_from_jet(&rJet, CopyAll);
}

void Jet::setup(const hlv_t& v)
{

  this->setupKine(v);

}

void Jet::copy_from_jet( const Jet* pJet, CopyDataMembers cdm0, CopyDataMembers cdm1 ,
			 CopyDataMembers cdm2 , CopyDataMembers cdm3)
{
  CopyDataMembers cdm = CopyDataMembers(cdm0 | cdm1 | cdm2 | cdm3);
  

  //std::cout<<"Jet::copy_from_jet this="<<  this->constit4MomGetter.empty() << "  pjet="<< pJet->constit4MomGetter.empty() << std::endl;

  // copy all kinematics
  this->setSignalState(P4SignalState::JETFINAL);
  this->set4Mom(pJet->hlv());

  this->setSignalState(statename_t::UNCALIBRATED);
  this->set4Mom(pJet->hlv(P4SignalState::JETEMSCALE));
  this->setSignalState(statename_t::JETCONSTITUENTSCALE);
  this->set4Mom(pJet->hlv(statename_t::JETCONSTITUENTSCALE));
  this->setSignalState(statename_t::JETFINAL);
  this->setConstituentSignalState(pJet->constituentSignalState());


  this->m_RoIword = pJet->m_RoIword;
  m_jetAuthor = pJet->m_jetAuthor;
  m_num_combinedLikelihood = pJet->m_num_combinedLikelihood;

  m_pseudoJet = pJet->m_pseudoJet;

  // copy members if requested
  if ( cdm & CopyConstituents )
    {
      //std::cout<< "copy constituent "<< std::endl;
      const_iterator firstConst = pJet->firstConstituent();
      const_iterator lastConst  = pJet->lastConstituent();
      for ( ; firstConst != lastConst; firstConst++ )
	{
	  Jet::index_type theIndex;
	  if ( pJet->getIndex(firstConst,theIndex) ) // getIndex by iterator more efficient ?
	    {
	      this->putElement(pJet->getContainer(firstConst),
			       theIndex,
			       pJet->getWeight(firstConst));
	    }
	}
    } else {
      this->navigableBase().removeAll();
    }

  // this line MUST stay after copyconstituents otherwise m_constituentsN will be double.
  m_constituentsN = pJet->m_constituentsN;
  
  if ( cdm & CopyMoments )
    {
      const shape_map_t* oMap = pJet->getMomentMap();
      //std::cout << "  Copying moment  other map "<< oMap<< "  author="<< pJet->jetAuthor() <<"  id="<< pJet->id()<< std::endl;
      if ( oMap != 0 ){

        const JetMomentMap::record_t *oldrec = oMap->getRecord( pJet->id() );
        // copy the moments :
        //std::cout << "  oldrec = "<< oldrec << std::endl;
        if( oldrec){
          const shape_map_t* mMap = this->getMomentMap(true);
          // this should guaranty we have a map.

          JetMomentMap::record_t *newrec = 
            new JetMomentMap::record_t(  *oldrec ); 
          
          // assign them in the map
          mMap->assignRecord(m_jetId, newrec);
        }
      }
    }


  if ( cdm & CopyAssociations )
    if ( bool(pJet->m_assocStore) )
      {
	m_assocStore = new assostore_t;
	
	const assostore_t& old = *pJet->m_assocStore;
	m_assocStore->reserve (old.size());
	for (size_t i = 0; i < old.size(); i++)
	  if (bool(old[i])) m_assocStore->push_back (old[i]->clone());
	  else m_assocStore->push_back ( 0 );
      }
  if ( cdm & CopyTagInfos )
    if ( bool(pJet->m_tagInfoStore) )
      {
	m_tagInfoStore = new tagstore_t;
	
	const tagstore_t& old = *pJet->m_tagInfoStore;
	m_tagInfoStore->reserve (old.size());
	for (size_t i = 0; i < old.size(); i++)
	  if (bool(old[i])) m_tagInfoStore->push_back (old[i]->clone());
	  else m_tagInfoStore->push_back(0);
      }
  
}


/** @detail The @c Jet destructor destroys the association and tag info stores,
    if they have ever been created. It does not destroy the objects in these
    stores, though. These should be referenced through an @C ElementLink 
    only, i.e, owned by their own collections, or owned by any other data
    object.

    NOTE: Above is not true at the moment.  Jet does own the referenced
          objects.  This should change once split store is implemented.
 */

Jet::~Jet()
{
  // FIXME jet owns stores for right now
  
  if( m_jetId != s_defaultJetId){
    // we may have an entry in a moment map
    // and need to remove ourselves from the map
    const shape_map_t *map = getMomentMap();
    if(map) map->removeRecord( m_jetId );
  }

  if ( m_assocStore != 0 )   {
    for (size_t i = 0; i < m_assocStore->size(); i++)
      delete (*m_assocStore)[i];
    delete m_assocStore;
  }
  if ( m_tagInfoStore != 0 ) {
    for (size_t i = 0; i < m_tagInfoStore->size(); i++)
      delete (*m_tagInfoStore)[i];
    delete m_tagInfoStore;
  }

}


/////////////////////////////
// Jet Constituent Algebra //
/////////////////////////////

/** @detail This method adds a constituent referenced by a pointer to its 
    storable container and a pointer to itself to a @c Jet. The @c Jet
    four-momentum is updated with the constituent four-momentum, taking into
    account an optional kinematic weight. If the constituent object was already
    a member of the @c Jet, the old and new weights are added. 
    \warning There is no restriction on the numerical range for these weights 
    when adding a constituent. Clients are responsible in their own algorithm 
    implementations to make sure that the weights make sense in the context of 
    their analysis.
    
    @param[in]  pColl   pointer to storable object collection of type 
                @c INavigable4MomentumContainer
    @param[in]  pConst  pointer to constituent object of type 
                @c INavigable4Momentum
    @param[in]  wght    optional kinematic weight of constituent, default 1
 */
bool Jet::addConstituent(const collection_t*  pColl,
			 const constituent_t* pConst,
			 double wght)
{
  double w = this->contains(pConst) ? wght + this->getWeight(pConst) : wght;
  this->putElement(pColl,pConst,w);
  this->updateKine(pConst,wght);
  return true;
}

/** @detail This method adds a constituent referenced by a pointer to its 
    storable container and its index in this container, to a @c Jet. The @c Jet
    four-momentum is updated with the constituent four-momentum, taking into
    account an optional kinematic weight. If the constituent object was already
    a member of the @c Jet, the old and new weights are added. 
    \warning There is no restriction on the numerical range for these weights 
    when adding a constituent. Clients are responsible in their own algorithm 
    implementations to make sure that the weights make sense in the context of 
    their analysis.
    
    @param[in]  pColl   pointer to storable object collection of type 
                @c INavigable4MomentumContainer
    @param[in]  ind     index of type @c size_t referencing the 
                @c INavigable4Momentum object in its container.
    @param[in]  wght    optional kinematic weight of constituent, default 1
 */
bool Jet::addConstituent(const collection_t* pColl,index_t ind,double wght)
{

  const constituent_t * constit = (pColl->operator[])(ind) ;
  double w =  this->contains(pColl,ind) ? wght + this->getWeight(pColl,ind) : wght;
  this->putElement(pColl,ind,w);
  this->updateKine(constit,wght);
  return true;
}

/** @detail This method adds a constituent referenced by a pointer to its 
    storable container and its index in this container, to a @c Jet. The @c Jet
    four-momentum is updated with the constituent four-momentum, taking into
    account an optional kinematic weight. If the constituent object was already
    a member of the @c Jet, the old and new weights are added. 
    \warning There is no restriction on the numerical range for these weights 
    when adding a constituent. Clients are responsible in their own algorithm 
    implementations to make sure that the weights make sense in the context of 
    their analysis.
    
    @param[in]  pColl   pointer to storable object collection of type 
                @c INavigable4MomentumContainer
    @param[in]  ind     index of type @c size_t referencing the 
                @c INavigable4Momentum object in its container.
    @param[in]  sizeHint If nonzero, the expected final size of the jet.
    @param[in]  noKine  if true, don't recalculate kinematics.
    @param[in]  wght    optional kinematic weight of constituent, default 1
 */
bool Jet::addConstituent(const collection_t* pColl,
                         index_t ind,
                         size_t sizeHint,
                         bool noKine,
                         double wght)
{
  // Avoid multiple linear searches if we can.
  // Unfortunately, we can't avoid it for the case in which
  // the constituent is already in the jet (without changing
  // the Navigable interface, anyway); but that should be the
  // uncommon case anyway.
  //std::cout << " adding constit.  e="<< this->e() << "  c_e="<< (pColl->operator[])(ind)->e() << " size "<< this->size()<< std::endl;
  if (this->contains(pColl, ind))
    this->reweight(pColl,ind, wght + this->getWeight(pColl, ind));
  else
    this->insertElement(pColl, ind, wght, sizeHint);

  if (!noKine)
    this->updateKine(((pColl->operator[])(ind))->hlv(),wght);
  return true;
}

// remove constituents
bool Jet::removeConstituent(const constituent_t* pConst)
{
  if ( this->contains(pConst) )
    {
      double w(this->getWeight(pConst));
      //this->updateKine(pConst->hlv(),-w);
      this->updateKine(pConst,-w);
      this->remove(pConst);
      return true;
    }
  return false;
}

// add Jet to Jet with a global weight
bool Jet::addJet(const Jet* pJet, double wght)
{
  const_iterator fC(pJet->begin());
  const_iterator lC(pJet->end());
  for ( ; fC != lC ; fC++ )
    {
      index_t ind(index_t(-1));
      if ( pJet->getIndex(fC,ind) )
	{
	  if ( !(this->contains(*fC)) )
	    {
	      this->addConstituent(pJet->getContainer(fC),
				   ind,
				   wght*(pJet->getWeight(fC)));
	    }
	}
    }
  return pJet->size() > 0;
}

//////////////////////////////////
// Jet Kinematics Manipulations //
//////////////////////////////////

// remove all children with kinematic weight 0
bool Jet::cleanUp()
{
  unsigned int aCtr = 0;
  // check content
  if ( this->size() > 0 )
    {
      const_iterator fC(this->begin());
      const_iterator lC(this->end());
      for ( ; fC != lC; fC++ )
	{
	  double weight(this->getWeight(fC));
	  if ( fabs(weight)< m_ignoreWeight )
	    {
	      aCtr++;
	      this->removeConstituent(*fC);
	    }
	}
    }
  return aCtr > 0;
}

// re-weight child in Jet
bool Jet::reweight(const constituent_t* pConst,double wght)
{
  if ( this->contains(pConst) )
    {
      const collection_t* pColl = this->getContainer(pConst);
      this->removeConstituent(pConst);
      return this->addConstituent(pColl,pConst,wght);
    }
  return false;
}

bool Jet::reweight(const collection_t* pColl,index_t ind,double wght)
{
  //  std::cout << "reweighting  "<< wght << std::endl;
  return this->removeConstituent(pColl,ind)
    ? this->addConstituent(pColl,ind,wght)
    : false;
}

////////////////////
// Jet Kinematics //       ///// kinematic variables from P4PxPyPzE /////
////////////////////

void Jet::scale_momentum(double scale_factor){
  this->set4Mom(this->hlv()*scale_factor);
}

Jet * Jet::get_scaled_copy(double scale_factor) const{
  Jet* newjet = new Jet(this,true,true);
  newjet->scale_momentum(scale_factor);
  return newjet;
}

/////////////////////
// Jet Composition //
/////////////////////

// test Jets on identity by composition
bool Jet::isIdentical(const Jet* pJet ) const
{
  // need to have same size (number of constituents)
  if ( pJet->size() != this->size() ) return false;

  if( pJet->size() == 0 ){ 
    // fall back on mometum comparison
    bool isEqual =  ( fabs( pJet->e() - e() )< m_ignoreWeight ) ;
    isEqual = isEqual &&  ( fabs( pJet->px() - px() )< m_ignoreWeight) ;
    isEqual = isEqual &&  ( fabs( pJet->py() - py() )< m_ignoreWeight) ;
    isEqual = isEqual &&  ( fabs( pJet->pz() - pz() )< m_ignoreWeight) ;
    return isEqual;
  }

  bool isEqual = true;
  const_iterator fC(pJet->begin());
  const_iterator lC(pJet->end());

  while ( isEqual && fC != lC )
    {
      double wJ(pJet->getWeight(fC));
      const_iterator fK(this->begin());
      const_iterator lK(this->end());
      while ( ( fK != lK ) && !((*fC) == (*fK) &&
				fabs(this->getWeight(fK)-wJ) < m_ignoreWeight) )
 
	{ fK++; }
      isEqual = fK != lK;
      fC++;
    }

  return isEqual;
}

// Print-out a single line to describe the Jet
std::string Jet::str() {
  std::ostringstream out;
  out << jetAuthor() << ": NConstituents: " << size() << "; Eta: " << eta() << "; Phi: " << phi() << "; Pt: " << pt();
  return out.str();
}

// Difference between two jets - Non-class method required for trigger
void diff(const Jet& rJet1, const Jet& rJet2, std::map<std::string,double> varDiff)
{
  // Since this isn't part of the Jet Class, we need to re-define it here
  // not very elegant
  double ignoreWeight = 1.0e-06;
  // Compare sizes
  if(rJet1.size() != rJet2.size())
    varDiff["NConstituents"] = rJet1.size() - rJet2.size();

  // Compare 4 momenta
  if(fabs(rJet1.e() - rJet2.e()) > ignoreWeight)
    varDiff["e"] = rJet1.e() - rJet2.e();
  if(fabs(rJet1.px() - rJet2.px()) > ignoreWeight)
    varDiff["px"] = rJet1.px() - rJet2.px();
  if(fabs(rJet1.py() - rJet2.py()) > ignoreWeight)
    varDiff["py"] = rJet1.py() - rJet2.py();
  if(fabs(rJet1.pz() - rJet2.pz()) > ignoreWeight)
    varDiff["pz"] = rJet1.pz() - rJet2.pz();
}



// find overlap region
Jet* Jet::getOverlap(const Jet* pJet, bool /*noKine = false*/) const
{
  if ( pJet->size() == 0 || this->size() == 0 ) return (Jet*)0;

  // Dereferencing ELs is a little expensive.
  // Get the constituents into a vector so that we only
  // need to do it once.
  std::vector<const constituent_type*> constituents (this->begin(),
                                                     this->end());
  //unused - fix warning
  //size_t sz = std::min (this->size(), pJet->size());

  // Defer allocating the new jet until we know we have something
  // to put into it.
  Jet* newJet = 0;
  // de-compose Jet
  const_iterator fC(pJet->begin());
  const_iterator lC(pJet->end());
  for ( ; fC != lC; fC++ )
    {
      const constituent_type* c = *fC;
      if ( std::find (constituents.begin(), constituents.end(), c) != 
           constituents.end() )
      {
        if (!newJet){
          newJet = new Jet(this, true, false); // we copy the stores, not the constituent
	  newJet->set4Mom(hlv_t(0. ,0. , 0., 0.)) ; //and reset its fourmom to nill	  
	}
        index_t index;
#ifndef NDEBUG
        bool result =
#endif
          this->getIndex (fC, index);
        assert (result);
        newJet->addConstituent(this->getContainer(fC),
                               index);
//                                sz,
//                                noKine);
	//std::cout<< "getoverlap  new e="<< newJet->e() << " / e1="<< e() << "  e2="<< pJet->e()<<"  |  eta= "<< newJet->eta() << "  f="<< newJet->pt()/pt() << std::endl;
      }
    }
  return newJet;
}

///////////////////////
// Kinematic Updates //    ///// protected /////
///////////////////////

Jet::hlv_t Jet::constituent_sum4Mom() const
{
  hlv_t fmom(0.,0.,0.,0.);
  JetConstituentIterator fC = JetConstituentIterator::first(this);
  JetConstituentIterator lC = JetConstituentIterator::last(this);
  
  for (;fC!=lC;++fC)
    {
      if (! fC.get_real_constit()) continue; // constit may be invalid if broken EL
      fmom += (fC.hlv() * this->getWeight(fC.get_real_constit()));
    }
  return fmom;
}

Jet::hlv_t Jet::constituent_sum4Mom(Jet::signalstate_t s) const
{
  hlv_t fmom(0.,0.,0.,0.);
  JetConstituentIterator fC = JetConstituentIterator::first(this,s);
  JetConstituentIterator lC = JetConstituentIterator::last(this);

  for (;fC!=lC;++fC)
    {
      if (! fC.get_real_constit()) continue; // constit may be invalid if broken EL
      fmom += (fC.hlv() * this->getWeight(fC.get_real_constit()));
    }
  return fmom;
}

/*!
  Recalculate overall jet kinematics from all constituents. This method 
  removes all corrections or calibrations applied and sets the overall
  @c Jet kinematics to the (weighted) sum of constituent four-momentums. 
 */
void Jet::updateKine()
{
  // reset four momentum
  this->setPx(0.);
  this->setPy(0.);
  this->setPz(0.);
  this->setE(0.);

  // loop children
  
  JetConstituentIterator fC = JetConstituentIterator::first(this, m_constituentSigState);
  JetConstituentIterator lC = JetConstituentIterator::last(this);
  for (;fC!=lC;++fC)
    {
      if (! fC.get_real_constit()) continue; // constit may be invalid if broken EL
      this->updateKine(fC.hlv(),this->getWeight(fC.get_real_constit()));
    }
}

/*!
  Update the @c Jet four-momentum by adding another four-vector with a weight.
 */
void Jet::updateKine(const hlv_t& rVec,double wght)
{
  this->set4Mom(this->hlv()+rVec*wght);
}

void Jet::updateKine(const Jet::constituent_t* pConst,double wght)
{
  
  // Attempt to cast into an ISignalState object
  const ISignalState* pSigState = dynamic_cast<const ISignalState*>(pConst);
  
  if(bool(pSigState) && m_constituentSigState != P4SignalState::UNKNOWN)
    {
      // Copy kinematics for the chosen Signal State
      this->updateKine(pSigState->hlv(m_constituentSigState),wght);
    } else 
    this->updateKine(pConst->hlv(),wght);
}

Jet* Jet::clone() const 
{
  Jet* jet = new Jet(this, false, false); 
  return jet; 
}

Jet* Jet::clone(bool copyStores, bool copyConstituents) const 
{
  Jet* jet = new Jet(this, copyStores,copyConstituents); 
  return jet; 
}

JetKeyDescriptorInstance * Jet::keyDesc() const {
  if( m_collection ) return m_collection->keyDesc();
  return &JetKeyDescriptorInstance::s_instance;
}

/////////////
// Moments //
/////////////

void Jet::setShape(const mkey_t& shapeName,shape_t shapeValue,
		    bool createIfMissing) const 
{
  // use check function to find index of moment - index is generated if
  // not yet in key descriptor store
  size_t keyIndex;
  if ( this->checkKey(JetKeyConstants::ShapeCat,shapeName,keyIndex,true) )
    {      
      // access jet moment map
      const shape_map_t* map = this->getMomentMap(createIfMissing);
      // store data
      if ( map != 0 ) 
	{
	  map->setMoment(m_jetId,keyIndex,shapeValue); 
          //	  m_newMomKeys = true;
	}
    }
}

// get shape keys
std::vector<Jet::mkey_t> Jet::getShapeKeys() const
{
  std::vector<Jet::mkey_t> keys;
  const shape_map_t* pMap = this->getMomentMap(); 
  if ( pMap != 0 ) 
    {
      size_t n(pMap->numberOfMoments(m_jetId));
      keys.reserve(n/2); 
      for ( size_t i(0); i < n; ++i )
        {
          float data(0.);
          if ( pMap->getMoment(m_jetId,i,data) ) 
            keys.push_back(keyDesc()->getKey(JetKeyConstants::ShapeCat,i));
        }
    }
  
  return keys;
}

Jet::shape_t Jet::getShape(const mkey_t& shapeName,bool addIfMissing) const
{

  size_t keyIndex;
  if ( this->checkKey(JetKeyConstants::ShapeCat,shapeName,keyIndex,false) )
    {
      const shape_map_t* pMap = this->getMomentMap(addIfMissing);
      return pMap != 0 ? pMap->getMoment(m_jetId,keyIndex) : shape_t();
    }
  return shape_t();
}

//////////////////
// Associations //
//////////////////

/// RSRSRS
// void Jet::fillAssoToken(INavigationToken& /* token */ ) const
// {
//   assostore_t::const_iterator fAss(m_assocStore->begin());
//   assostore_t::const_iterator lAss(m_assocStore->end());
//   for ( ; fAss != lAss; fAss++ )
//     {
//       // RS HERE
//       // (*fAss)->fillToken(token);
//     }
// }

////////////////////////////
// Constituent Navigation //
////////////////////////////

/////////////
// TagInfo //
/////////////


void Jet::setTagInfo(const mkey_t& key,
		     const taginfo_t* pTagInfo,
		     bool /* useLink */ ) const
{
  // use check function
  size_t aInd;
  //
  if ( ! bool(m_tagInfoStore) )
    {
      // checkKey autoamtically build new store !!!
      m_tagInfoStore = new tagstore_t();
    }
  if ( this->checkKeyStore(JetKeyConstants::TagCat,key,m_tagInfoStore,aInd, true) )
    {
      removeInfo(aInd);
      (m_tagInfoStore->operator[])(aInd) = pTagInfo;
    }
}

void Jet::removeInfo (unsigned int index) const
{
  delete (m_tagInfoStore->operator[])(index);
  (m_tagInfoStore->operator[])(index) = 0;
}

void Jet::removeInfo(const mkey_t& key ) const
{
  if ( bool(m_tagInfoStore) ) {
    size_t aInd;
    if ( this->checkKeyStore(JetKeyConstants::TagCat,key,m_tagInfoStore,aInd, false) ) {
      removeInfo(aInd);
    }
  }
}


void Jet::setupKine(const hlv_t& vIn)
{
  this->set4Mom(vIn, statename_t::JETEMSCALE);
  this->set4Mom(vIn, statename_t::JETCONSTITUENTSCALE);
  this->set4Mom(vIn, statename_t::JETFINAL);
}

///////////////////
// Signal States //
///////////////////


void Jet::setRawE(double e)
{ setE(e,P4SignalState::JETEMSCALE); }
void Jet::setRawPx(double px)
{ setPx(px,P4SignalState::JETEMSCALE); }
void Jet::setRawPy(double py)
{ setPy(py,P4SignalState::JETEMSCALE); }
void Jet::setRawPz(double pz)
{ setPz(pz,P4SignalState::JETEMSCALE); }

void Jet::setCScaleE(double e)
{ setE(e,P4SignalState::JETCONSTITUENTSCALE); }
void Jet::setCScalePx(double px)
{ setPx(px,P4SignalState::JETCONSTITUENTSCALE); }
void Jet::setCScalePy(double py)
{ setPy(py,P4SignalState::JETCONSTITUENTSCALE); }
void Jet::setCScalePz(double pz)
{ setPz(pz,P4SignalState::JETCONSTITUENTSCALE); }

void Jet::setCalE(double e)
{ setE(e,P4SignalState::JETFINAL); }
void Jet::setCalPx(double px)
{ setPx(px,P4SignalState::JETFINAL); }
void Jet::setCalPy(double py)
{ setPy(py,P4SignalState::JETFINAL); }
void Jet::setCalPz(double pz)
{ setPz(pz,P4SignalState::JETFINAL); }

double Jet::getRawE()  const { return e(P4SignalState::JETEMSCALE);  }
double Jet::getRawPx() const { return px(P4SignalState::JETEMSCALE); } 
double Jet::getRawPy() const { return py(P4SignalState::JETEMSCALE); } 
double Jet::getRawPz() const { return pz(P4SignalState::JETEMSCALE); } 

double Jet::getCScaleE()  const { return e(P4SignalState::JETCONSTITUENTSCALE);  }
double Jet::getCScalePx() const { return px(P4SignalState::JETCONSTITUENTSCALE); }
double Jet::getCScalePy() const { return py(P4SignalState::JETCONSTITUENTSCALE); }
double Jet::getCScalePz() const { return pz(P4SignalState::JETCONSTITUENTSCALE); }

double Jet::getCalE()  const { return e(P4SignalState::JETFINAL);  }
double Jet::getCalPx() const { return px(P4SignalState::JETFINAL); }
double Jet::getCalPy() const { return py(P4SignalState::JETFINAL); }
double Jet::getCalPz() const { return pz(P4SignalState::JETFINAL); }

double Jet::ei()  const { return this->e(); }

double Jet::pxi() const { return this->px(); }

double Jet::pyi() const { return this->py(); }

double Jet::pzi() const { return this->pz(); }

double Jet::pti() const { return this->pt(); }

bool Jet::finalScaleEqualsEMScale() const {
  bool ok = jet_component_identity( getRawE() , getCalE() );
  ok &= jet_component_identity( getRawPx() , getCalPx() );
  ok &= jet_component_identity( getRawPy() , getCalPy() );
  ok &= jet_component_identity( getRawPz() , getCalPz() );
  return ok;
}

///////////                                                        /////////// 
// Public:                                               CombinedLikelihood //
///////////                                                        ///////////


const std::vector<double>& Jet::combinedLikelihood() const
{ 
  static std::vector<double> combinedLikelihood;
  combinedLikelihood.resize(m_num_combinedLikelihood);
  std::string base="LikeLihood_";
  for(unsigned int i=0; i<m_num_combinedLikelihood; i++){
    std::stringstream s; s<< i; 
    combinedLikelihood[i] = getShape(base+s.str()) ;
  }

  return combinedLikelihood;
}

void Jet::setCombinedLikelihood(const std::vector<double>& 
				       combinedLikelihood)
{ 
  std::string base="LikeLihood_";
  for(unsigned int i=0; i<combinedLikelihood.size(); i++){
    std::stringstream s; s<< i; 
    setShape(base+s.str(), combinedLikelihood[i] );
  }	
  m_num_combinedLikelihood = combinedLikelihood.size();
}


double Jet::getFlavourTagWeight() const
{
  double w = 0.;
  if ( this->jetAuthor().find("Atlfast") != std::string::npos) {
    // Fastsim
    w = -30.;
    const JetTagInfoBase* pos(this->tagInfo("SV1IP3D"));
    if (pos != 0) {
      std::vector<double> prob = pos->tagLikelihood();
      if(prob.size()>1) {
        double pb = prob[0];
        double pu = prob[1];
	if (pb != 0 && pu != 0) 
	  w = log(pb/pu);
	else 
	  w = 0.; //Hopefully a warning appears in the BTagger Tool
      }
    } else {
      // Use "binary" random b-tag only if no IP3D nor SV1 Info
      if (m_num_combinedLikelihood > 0) {
        std::vector<double> Likelihood = combinedLikelihood();
        if (Likelihood[0] > 0.9) w = 100;
      }
    }
  } else {
    // FullSim
    bool found = false;
    // get W from IP3D:
    const JetTagInfoBase* pos(this->tagInfo("IP3D"));
    if( pos != 0 ) {
      found = true;
      std::vector<double> prob = pos->tagLikelihood();
      if(prob.size()>1) {
        double pb = prob[0];
        double pu = prob[1];
	if (pb > 0. && pu > 0.) {
	  if(pb!=1.||pu!=1.e9) {
	    w = log(pb/pu);
	  }
	} else 
	  w = 0.;  //Hopefully a warning appears in the BTagger Tool
      }
    }
    // get W from SV:
    pos = this->tagInfo("SV1");
    if( pos != 0 ) {
      found = true;
      std::vector<double> prob = pos->tagLikelihood();
      if(prob.size()>1) {
        double pb = prob[0];
        double pu = prob[1];
	if (pb > 0. && pu > 0.)  //Hopefully a warning appears in the BTagger Tool if not the case
	  w += log(pb/pu);
      }
    }
    if (!found) w = -30.;
  }
  return w;
}

double Jet::getFlavourTagWeight(const std::string& infoName) const
{
  double w = -100.;
  const JetTagInfoBase* pos(this->tagInfo(infoName));
  if( pos != 0 ) {
    std::vector<double> prob = pos->tagLikelihood();
    if(prob.size() == 1) {
      w = prob[0];
    } else {
      if(prob.size() > 1) {
        double pb = prob[0];
        double pu = prob[1]; 
        if(pb<=0.) { // A different protection than the above one... In any case, this is not OK and should be watched out !
          w = -30.;
        } else if (pu<=0.) {
          w = +100.;
        } else {
          w = log(pb/pu);
        }
      }
    }
  }
  return w;
}




const std::vector<Jet::mkey_t>& Jet::getAssociationKeys() const
{
  return keyDesc()->getKeys (JetKeyConstants::AssoCat);
}


const Jet::assoc_t* Jet::getAssociationBase(const mkey_t& key) const
{
  return getObject<assoc_t,assostore_t>(key, m_assocStore, JetKeyConstants::AssoCat, false);
}



///////////                                                        /////////// 
// Public:                                                       Jet Author //
///////////                                                        ///////////

void Jet::setJetAuthor(const mkey_t& author) 
{
  if(m_jetAuthor == 0 ) m_jetAuthor = keyDesc()->getIndex(JetKeyConstants::InfoCat,author,true); 

  std::string full_auth = keyDesc()->getKey(JetKeyConstants::InfoCat,m_jetAuthor);  
  size_t pos = full_auth.find_first_of("_");

  if( pos != std::string::npos) // we add the _XXX_YYY part to author
    m_jetAuthor = keyDesc()->getIndex(JetKeyConstants::InfoCat,author+full_auth.substr(pos),true); 
  else
    m_jetAuthor = keyDesc()->getIndex(JetKeyConstants::InfoCat,author,true); 
}


Jet::mkey_t Jet::jetAuthor() const 
{ 
  std::string full_auth = keyDesc()->getKey(JetKeyConstants::InfoCat,m_jetAuthor);  
  // return the first part in JetAuth_XXX_YYY
  size_t found=full_auth.find_first_of("_");
  return found != std::string::npos ? full_auth.substr(0 , found) : full_auth; 
}



std::string Jet::jetAuthorAndCalibTags() const{
  return keyDesc()->getKey(JetKeyConstants::InfoCat,m_jetAuthor);  
}

void Jet::setJetAuthorAndCalibTags(const std::string& author) {
  m_jetAuthor = keyDesc()->getIndex(JetKeyConstants::InfoCat,author,true); 
}


  
bool Jet::hasCalibTag(const std::string& tag) const{
  std::string full_auth = keyDesc()->getKey(JetKeyConstants::InfoCat,m_jetAuthor);  
  std::string full_tag = ""; full_tag+="_"; full_tag+=tag;
  return ( full_auth.find(full_tag) != std::string::npos );
}

int  Jet::numCalibTag() const {
  std::string full_auth = keyDesc()->getKey(JetKeyConstants::InfoCat,m_jetAuthor);    
  size_t p = full_auth.find_first_of("_");
  int c=0;
  while(p != std::string::npos){
    c++;
    p = full_auth.find_first_of("_",p+1);
  }
  return c;
}

std::string Jet::getCalibTag(int i) const{
  std::string full_auth = keyDesc()->getKey(JetKeyConstants::InfoCat,m_jetAuthor);
  size_t p = full_auth.find_first_of("_");
  int c=0;
  while(p != std::string::npos){
    c++;
    if( c==i ){
      return full_auth.substr(p+1,full_auth.find_first_of("_",p+1)-p-1);
    }
    p = full_auth.find_first_of("_",p+1);
  }
  return "";
}

void Jet::addCalibTag(const std::string& tag){
  std::string full_auth = keyDesc()->getKey(JetKeyConstants::InfoCat,m_jetAuthor);
  full_auth +="_"+tag;
  m_jetAuthor = keyDesc()->getIndex(JetKeyConstants::InfoCat,full_auth,true); 
}
const Jet::shape_map_t* Jet::getMomentMap(bool addIfMissing) const
{
  if( (m_jetId == s_defaultJetId) && addIfMissing ){
    //  we need an id to be registered in a map
    m_jetId = OrphanJetMomentMap::instance.getNewId();
  }
  return getMomentMap();
}

const Jet::shape_map_t* Jet::getMomentMap() const
{

  // jets must be identified
  if(m_jetId == s_defaultJetId ) return NULL;
  
  if( m_collection ){ 
    // This jet belongs to a JetCollection, ask the map 
    return m_collection->getMomentMap();
  }
  // Else, this is an orphan jet, return the global instance :
  return &OrphanJetMomentMap::instance;
  
}

std::string Jet::getMomentMapName() const {
     std::string mapname = jetAuthor();
     mapname +="MomentMap";
     return mapname;
}

size_t Jet::id() const {return m_jetId;}

const JetCollection * Jet::parentCollection() const {
  return m_collection;
}

void Jet::setJetId(size_t id){ m_jetId = id ; }

void Jet::setPxPyPzE(double px, double py, double pz, double e){
  setPx(px);setPy(py);setPz(pz);setE(e);
}
void Jet::setPtEtaPhiE( double pt, double eta, double phi, double e ){
  double pz = pt*std::sinh( eta) ;  
  double px = pt*std::cos(phi);
  double py = pt*std::sin(phi);
  setPx(px);setPy(py);setPz(pz);setE(e);
}
void Jet::setPtEtaPhiM( double a, double b, double c, double d ){
  set4Mom( P4PtEtaPhiM(a,b,c,d) );
}
void Jet::setEEtaPhiM( double a, double b, double c, double d ){
  set4Mom( P4EEtaPhiM(a,b,c,d) );
}

