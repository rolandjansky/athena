/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetEvent/JetINav4MomAssociation.h"

#include "JetEvent/JetINav4MomAssociation.h"

JetINav4MomAssociation::JetINav4MomAssociation()
{  } 
  
JetINav4MomAssociation::JetINav4MomAssociation(NameType& name)
{
  this->setName(name);
}  
  
JetAssociationBase* JetINav4MomAssociation::clone() const
{
  return new JetINav4MomAssociation( *this );
}
  
const INavigable4Momentum* JetINav4MomAssociation::inav4momentum() const
{
  //get jet from Navigable
  if (this->size() ==0) 
    return 0;
  else if (this->size() > 1)
    return 0;
  // this shouldn't happen
  else 
    return getConstituentPtr((this->getConstituents()).begin());
}
  
void  JetINav4MomAssociation::set_inav4momentum(const INavigable4MomentumCollection* theContainer,
                                                           const INavigable4Momentum* the_inav4momentum,
                                                           double weight)
{
  double newWeight = weight;
  // jet already in collection
  if ( this->contains(the_inav4momentum) )
    {
      // update weight
      newWeight *= this->getinav4momentumWeight(the_inav4momentum);
      // remove the previous jet 
      this->remove(the_inav4momentum);
    }
    
  // from Navigable base
  this->putElement(theContainer,the_inav4momentum,newWeight);
}
  
void  JetINav4MomAssociation::set_inav4momentum(const INavigable4MomentumCollection* theContainer,
                                                           index_type& theIndex,
                                                           double weight)
{
  double newWeight = weight;
  // jet already in collection
  if ( this->contains(theContainer,theIndex) )
    {
      // update weight
      newWeight *= this->getinav4momentumWeight(theContainer,theIndex);
      // remove the previous jet
      this->remove(theContainer,theIndex);
    }
    
  // from Navigable base
  this->putElement(theContainer,theIndex,newWeight);
}
  
double JetINav4MomAssociation::getinav4momentumWeight(const INavigable4Momentum* the_inav4momentum) const
{
  // from Navigable
  return (this->contains(the_inav4momentum) )
    ? this->getParameter(the_inav4momentum)
    : 0.;
}
  
double JetINav4MomAssociation::getinav4momentumWeight(const INavigable4MomentumCollection* theContainer,
                                                                 index_type& theIndex) const
{
  // from Navigable
  return (this->contains(theContainer,theIndex))
    ? this->getParameter(theContainer,theIndex)
    : 0.;
}


