/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETCOLLECTION_H
#define JETEVENT_JETCOLLECTION_H
//********************************************************************
//NAME:     JetCollection.h
//********************************************************************

// INCLUDE HEADER FILES:

#include "AthContainers/DataVector.h"
#include "AthLinks/DataLink.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "JetEvent/Jet.h"
#include "JetEvent/JetKeyDescriptor.h"
#include "JetEvent/JetMomentMap.h"

#include "NavFourMom/IParticleContainer.h"
// Needed for constructing the mirror container hierarchy
// This tells StoreGate that DV<Jet> derives from DV<IParticle>
DATAVECTOR_BASE( Jet, IParticle );

namespace fastjet {
  class ClusterSequence ;
}

class JetCollection : public DataVector<Jet>
{
 public:

  // JetCollection types ---------------------
  typedef DataVector<Jet>::iterator iterator;  
  enum OrderedVar { ByEtDown     =  0,
                    ByEtUp       =  1,
		    ByPtDown     =  2,
		    ByPtUp       =  3,
                    ByEDown      =  4,
                    ByEUp        =  5,
                    ByEtaUp      =  6,
		    ByEtaDown    =  7,
		    ByAbsEtaUp   =  8,
		    ByAbsEtaDown =  9,
		    ByPhiDown    = 10,
		    ByPhiUp      = 11,
		    InEtaPhiDown = 12,
		    InEtaPhiUp   = 13,
		    Random       = 14 };
  // --------------------------------------

  // constructor 
  JetCollection(SG::OwnershipPolicy own = SG::OWN_ELEMENTS);



  virtual ~JetCollection() ;
  //  void print();
    
   
  // make ByEtDown the default
  bool isOrdered(JetCollection::OrderedVar ordering = JetCollection::ByEtDown ) const
    { return ordering == m_ordered; };
  
  void setOrdered(JetCollection::OrderedVar ordered )
    { m_ordered = ordered; return; };

  JetCollection::OrderedVar ordered() const
    { return m_ordered; };


  // we redefine the insertion mechanisms to be sure to capture the jet moments
  // associated to the inserted jets and assign an id within this collection
  void push_back(Jet* j);
  
  iterator insert(iterator position, Jet* j);

  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last);

  // cppcheck-suppress duplInheritedMember
  void clear();
  // cppcheck-suppress duplInheritedMember
  void clear(SG::OwnershipPolicy ownPolicy);


 private:
  
  OrderedVar m_ordered;

 public:
  
  /// link to the JetKeyDescriptor is mandatory for persistency. Access done through the JetKeyDescriptorInstance
  DataLink<JetKeyDescriptor>  m_keyStore;

  void setAuthor(std::string author);
  std::string author() const;

  const JetMomentMap * getMomentMap() const;
  std::string getMomentMapName() const;

  JetKeyDescriptorInstance * keyDesc() const ;

  protected:
  friend class JetCollectionCnv_p6;

#if !defined(__REFLEX__) && !defined(__CLING__)
  mutable DataLink<JetMomentMap>      m_momentMapLink;
#endif
  

  /** the author of this jetcollection. We don't forbid it to be different from 
   the m_jetAuthor of individual Jet*/
  size_t m_jetAuthor;



  size_t getNextId();
  /** the next free identifier for jets inside this collection */
  size_t m_nextId;

  
  void acquireJet(Jet *j);

  /// This JetKeyDescriptorInstance points by default to the global JetKeyDescriptorInstance's store. It is redirected to the key store from the input file when read out. This allows 2 events to be read at the same time.
  mutable JetKeyDescriptorInstance m_keyDescInstance;


  /** false if the map is recorded in SG*/
  mutable bool m_ownMap; // it is mutable because a map can be created from a const collection

  void recordedInSG(std::string key);

  void resetJetId(std::size_t);


protected:
  /// Pointer to the fastjet object this jet is build from. WARNING : this pointer is meant to be used in reco flow only.
  ///  It is transient only,  provided only to ease calculation of some jet quantities, and will be invalid for jets read from files.
  const fastjet::ClusterSequence* m_fastjetClustSeq;
public:
  /// Access to the internal fastjet object. CAN RETURN NULL, see the description of m_pseudoJet
  const fastjet::ClusterSequence * fastjetClustSeq() const {return m_fastjetClustSeq;} 
  void setFastjetClustSeq(const fastjet::ClusterSequence * cs){ m_fastjetClustSeq = cs;}


};

// auto-symlink:
SG_BASE( JetCollection, DataVector<Jet> );

CLASS_DEF( JetCollection , 1162448536 , 1 )
     //CLASS_DEF(JetCollection<Jet>, 3005, 1 )
     //CLASS_DEF(JetCollection<CombinedJetSums> , 3007, 1 )


template <class InputIterator>
void JetCollection::insert(iterator position, InputIterator first, InputIterator last){
  InputIterator it = first;
  for(; it != last; ++it) acquireJet(*it);
  DataVector<Jet>::insert(position,first,last);
}
/**  @class JetCollection                                                                                                  
     @brief Container for Jets                                                    
                                                                                  
     JetCollection is a DataVector of Jets.
     It is the main data object and acts as a central point for all jet related software.     
     It holds a JetMomentMap and JetKeyDescriptorInstance which it serves to the
     jets it holds.

     It redefines the push_back and insert method in order to 
        - let the jets know they are owned by a JetCollection 
        - perform the related operations (moment map)     

     It also defines an order enum (OrderedVar), and holds the order status       
     of the collection (ByEtDown, ByEtUp,etc..)                                   

     *IMPORTANT* : one should *not* record this object by using StoreGate::record(),
     but rather use the JetCollectionHelper::record(). The reason is this function performs
     all the necessary operations needed to deal with the JetMomentMap.
     
     */                                                                             
#endif
