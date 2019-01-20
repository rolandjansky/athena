/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           ElectronConstituents.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/
              
#include "egammaEvent/ElectronConstituent.h"

namespace Analysis
{
  

  ElectronConstituent::ElectronConstituent()
  {  } 
  
  ElectronConstituent::ElectronConstituent(NameType& name)
  { m_name = name; }  
  
  
  void ElectronConstituent::setName(NameType& name) {m_name = name;}      
  
  NameType ElectronConstituent::name() const { return m_name;}      

  IConstituent* ElectronConstituent::clone() const
  { return new ElectronConstituent( *this );}	

  const Electron* ElectronConstituent::electron() const
  {
    //get electron from Navigable
    if (this->size() ==0) 
      return 0;
    else if (this->size() > 1)
      return 0;
    // this shouldn't happen
    else 
      return getConstituentPtr((this->getConstituents()).begin());
  }
  
  void  ElectronConstituent::set_electron(const ElectronContainer* theContainer,
		       const Electron* the_electron,
		       double weight)
  {
    double newWeight = weight;
    // electron already in collection
    if ( this->contains(the_electron) )
      {
	// update weight
	newWeight *= this->getElectronWeight(the_electron);
	// remove the previous electron 
	this->remove(the_electron);
      }
    
    // from Navigable base
    this->putElement(theContainer,the_electron,newWeight);
    
  }
  
  void  ElectronConstituent::set_electron(const ElectronContainer* theContainer,
		       index_type& theIndex,
		       double weight)
  {
    double newWeight = weight;
    // electron already in collection
    if ( this->contains(theContainer,theIndex) )
      {
	// update weight
	newWeight *= this->getElectronWeight(theContainer,theIndex);
	// remove the previous electron
	this->remove(theContainer,theIndex);
      }
    
    // from Navigable base
    this->putElement(theContainer,theIndex,newWeight);
    
  }
  
  double ElectronConstituent::getElectronWeight(const Electron* the_electron) const
  {
    // from Navigable
    return (this->contains(the_electron) )
      ? this->getParameter(the_electron)
      : 0.;
  }
  
  double ElectronConstituent::getElectronWeight(const ElectronContainer* theContainer,
				      index_type& theIndex) const
  {
    // from Navigable
    return (this->contains(theContainer,theIndex))
      ? this->getParameter(theContainer,theIndex)
      : 0.;
  }
  
  
}
