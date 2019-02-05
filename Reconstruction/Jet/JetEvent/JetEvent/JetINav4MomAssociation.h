// emacs this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETINAVIGABLE4MOMENTUMASSOCIATION_H
#define JETEVENT_JETINAVIGABLE4MOMENTUMASSOCIATION_H
/////////////////////////////////////////
///
/// \class JetINav4MomAssociation
/// \brief A JetAssociation object for a generic object.
///
/// \author Bertrand Chapleau
/// \author P.A. Delsart (moved to JetEvent package)
/// \date summer 2011
////////////////////////////////////////////
#include<vector>
#include<string>

#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "JetEvent/JetAssociationBase.h"
  
  
// string to identify the constituent
   
typedef std::string NameType;
  
class JetINav4MomAssociation : public JetAssociationBase,
                                          public Navigable<INavigable4MomentumCollection, double>
{

  typedef Navigable<INavigable4MomentumCollection,double>::external_index_type index_type;
  typedef Navigable<INavigable4MomentumCollection,double>::constituent_iter  iter_type;
      
      
public:
  JetINav4MomAssociation();      //!< constructor
  JetINav4MomAssociation(NameType& name) ;      //!< constructor
  ~JetINav4MomAssociation() = default;      //!< destructor
      
  virtual JetAssociationBase* clone() const; 
  //!< a clone method for the proper workings of the copy constructor
      
  const INavigable4Momentum* inav4momentum() const; //!< get jet directly without token      
      
  // Set functions
  void set_inav4momentum(const INavigable4MomentumCollection* theContainer,
                         const INavigable4Momentum* the_inav4momentum,  double weight=1);
      
  void set_inav4momentum(const INavigable4MomentumCollection* theContainer,
                         index_type& theIndex,   double weight=1);
      
  void set_association(const INavigable4MomentumCollection* theContainer,
                       const INavigable4Momentum* the_inav4momentum,  double weight=1) 
  {set_inav4momentum(theContainer, the_inav4momentum, weight);}
      
  void set_association(const INavigable4MomentumCollection* theContainer,
                       index_type& theIndex,   double weight=1)
  {set_inav4momentum(theContainer, theIndex, weight);}
      
  double getinav4momentumWeight(const INavigable4Momentum* the_inav4momentum) const ;
      
  double getinav4momentumWeight(const INavigable4MomentumCollection* theContainer,
                                index_type& theIndex) const;
      
} ;
  


#endif
