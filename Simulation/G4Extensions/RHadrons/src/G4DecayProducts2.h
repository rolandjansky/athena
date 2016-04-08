/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DecayProducts2_H
#define G4DecayProducts2_H

//Copy of G4DecayProducts class, with up to 2000 entries
#include "G4DecayProducts.hh"
class G4DecayProducts2 : public G4DecayProducts{
public:
  enum {MaxNumberOfProducts=2000};

  //So we can "access" the private members of G4DecayProducts! (Don't try this at home!!)
  struct G4DecayProductsStruct{
    enum {MaxNumberOfProducts = 64};
    G4int                         numberOfProducts;
    G4DynamicParticle*            theParentParticle;
    G4DynamicParticle*            theProductVector[MaxNumberOfProducts];
  };

  G4int PushProducts(G4DynamicParticle *aParticle)
  {
    G4int & numberOfProducts2 = ((G4DecayProductsStruct*)this)->numberOfProducts;//number of products in G4DecayProducts
    if ( numberOfProducts2 < MaxNumberOfProducts) {
      G4DynamicParticle** theProductVector2 = ((G4DecayProductsStruct*)this)->theProductVector;//going to push the products onto the list in G4DecayProducts, even though it's only 64 long... fortunately, it's the last member variable of the class, and we allocated enough room past the end of the class in memory...
      theProductVector2[numberOfProducts2]= aParticle;
      numberOfProducts2 += 1;
      G4cout << "G4DecayProducts2::PushProducts "
             << " has numberOfProducts="
             << numberOfProducts2 << G4endl;
    } else {
      G4cerr << "G4DecayProducts2::PushProducts "
             << " exceeds MaxNumberOfProducts(="
             << G4int(MaxNumberOfProducts) << ")" << G4endl;
    }
    return numberOfProducts2;
  }

};
#endif // G4DecayProducts2_H
