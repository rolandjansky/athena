/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           JetConstituents.h  -  Description
                             -------------------
    begin   : January, 2005
    authors : Kyle Cranmer, Andreas Wildauer
    email   : cranmer@cern.ch, andreas.wildauer@cern.ch

    changes : 

***************************************************************************/
                                        
#include "JetTagEvent/JetConstituent.h"

namespace Analysis
{
  

  JetConstituent::JetConstituent():
    m_wtCryoLocal    ( 0 ),
    m_energyInSample ( CaloSampling::Unknown )
  {   } 
  
  JetConstituent::JetConstituent(NameType& name):
    m_wtCryoLocal    ( 0 ),
    m_energyInSample ( CaloSampling::Unknown )//reserve space in vector 
  { m_name = name; }  
  
  
  void JetConstituent::setName(NameType& name) {m_name = name;}      
  
  NameType JetConstituent::name() const { return m_name;}      

  IConstituent* JetConstituent::clone() const
  { return new JetConstituent( *this );}	

  const Jet* JetConstituent::jet() const
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
  
  void  JetConstituent::set_jet(const JetCollection* theContainer,
		       const Jet* the_jet,
		       double weight)
  {
    double newWeight = weight;
    // jet already in collection
    if ( this->contains(the_jet) )
      {
	// update weight
	newWeight *= this->getJetWeight(the_jet);
	// remove the previous jet 
	this->remove(the_jet);
      }
    
    // from Navigable base
    this->putElement(theContainer,the_jet,newWeight);
    
  }
  
  void  JetConstituent::set_jet(const JetCollection* theContainer,
		       index_type& theIndex,
		       double weight)
  {
    double newWeight = weight;
    // jet already in collection
    if ( this->contains(theContainer,theIndex) )
      {
	// update weight
	newWeight *= this->getJetWeight(theContainer,theIndex);
	// remove the previous jet
	this->remove(theContainer,theIndex);
      }
    
    // from Navigable base
    this->putElement(theContainer,theIndex,newWeight);
    
  }
  
  double JetConstituent::getJetWeight(const Jet* the_jet) const
  {
    // from Navigable
    return (this->contains(the_jet) )
      ? this->getParameter(the_jet)
      : 0.;
  }
  
  double JetConstituent::getJetWeight(const JetCollection* theContainer,
				      index_type& theIndex) const
  {
    // from Navigable
    return (this->contains(theContainer,theIndex))
      ? this->getParameter(theContainer,theIndex)
      : 0.;
  }
  
  
  ///This is the energy in a sample with the navigation weight and hadronic calibration.
  double JetConstituent::energyInSample( CaloSampling::CaloSample s ) const 
  {
    return   m_energyInSample[s];
  }
  
  /**The estimate of the energy cryostat.  
     Normally this is wtCryo*sqrt(EMB3*tileBar0) where EMB3&tilebar0 do
     NOT have any hadronic calibration applied.  
     In order to avoid holding a lot more information in the ParticleJet we return
     wtCryoLocal * sqrt(EMB3_calib * tileBar0_calib) 
     where 
     EMB3_calib = average_EMB3_weight * EMB3
     tileBar0_calib = average_TileBar0_weight * tileBar0
     and thus
     wtCryoLocal = wtCryo / sqrt( average_TileBar0_weight * average_EMB3_weight).
  */
  double JetConstituent::energyInCryostat() const 
  {
    return m_wtCryoLocal*sqrt( fabs(m_energyInSample[CaloSampling::EMB3])
			       *fabs(m_energyInSample[CaloSampling::TileBar0]) );  
  }
  
  /// set energy in sample WITH hadronic weights
  void JetConstituent::set_energyInSample(CaloSampling::CaloSample s, double e)
  {
    m_energyInSample[s] = e;
  }
  
  /** Set wtCryoLocal defined as wtCryoLocal = wtCryo / sqrt(
      average_TileBar0_weight * average_EMB3_weight) */
  void JetConstituent::set_wtCryo(double wtCryoLocal)
  {
    m_wtCryoLocal = wtCryoLocal;  
  }
  
}
