/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           MuonConstituents.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/
                                        
#include "MuonIDEvent/MuonConstituent.h"

namespace Analysis
{
  

  MuonConstituent::MuonConstituent()
  {  } 
  
  MuonConstituent::MuonConstituent(NameType& name)
  { m_name = name; }  
  
  void MuonConstituent::setName(NameType& name) {m_name = name;}      
  
  NameType MuonConstituent::name() const { return m_name;}      

  IConstituent* MuonConstituent::clone() const
  { return new MuonConstituent( *this );}	

  const Muon* MuonConstituent::muon() const
  {
    //get muon from Navigable
    if (this->size() ==0) 
      return 0;
    else if (this->size() > 1)
      return 0;
    // this shouldn't happen
    else 
      return getConstituentPtr((this->getConstituents()).begin());
  }
  
  void  MuonConstituent::set_muon(const MuonContainer* theContainer,
		       const Muon* the_muon,
		       double weight)
  {
    double newWeight = weight;
    // muon already in collection
    if ( this->contains(the_muon) )
      {
	// update weight
	newWeight *= this->getMuonWeight(the_muon);
	// remove the previous muon 
	this->remove(the_muon);
      }
    
    // from Navigable base
    this->putElement(theContainer,the_muon,newWeight);
    
  }
  
  void  MuonConstituent::set_muon(const MuonContainer* theContainer,
		       index_type& theIndex,
		       double weight)
  {
    double newWeight = weight;
    // muon already in collection
    if ( this->contains(theContainer,theIndex) )
      {
	// update weight
	newWeight *= this->getMuonWeight(theContainer,theIndex);
	// remove the previous muon
	this->remove(theContainer,theIndex);
      }
    
    // from Navigable base
    this->putElement(theContainer,theIndex,newWeight);
    
  }
  
  double MuonConstituent::getMuonWeight(const Muon* the_muon) const
  {
    // from Navigable
    return (this->contains(the_muon) )
      ? this->getParameter(the_muon)
      : 0.;
  }
  
  double MuonConstituent::getMuonWeight(const MuonContainer* theContainer,
				      index_type& theIndex) const
  {
    // from Navigable
    return (this->contains(theContainer,theIndex))
      ? this->getParameter(theContainer,theIndex)
      : 0.;
  }
  
  
}
