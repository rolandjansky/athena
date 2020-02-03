/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointContaine.h
//   Header file for class SpacePointContainer
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Container for SpacePoints  collections for InnerDetector and Muons
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRKSPACEPOINT_SPACEPOINTCONTAINER_H
#define TRKSPACEPOINT_SPACEPOINTCONTAINER_H

// Base classes
#include "EventContainers/IdentifiableContainer.h"
//Needed Classes
#include "TrkSpacePoint/SpacePointCollection.h"
//#include "TrkSpacePoint/SpacePointCLASS_DEF.h"


typedef EventContainers::IdentifiableCache< SpacePointCollection > SpacePointCache;

class SpacePointContainer 
: public IdentifiableContainer<SpacePointCollection>{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  SpacePointContainer(unsigned int max);
  
  SpacePointContainer(SpacePointCache*);

  // Destructor:
  virtual ~SpacePointContainer();

   /** return class ID */
  static const CLID& classID() 
    {
      static const CLID id = 1273119430 ; 
      return id; 
      // we do not know why using the traits does not work
      //return ClassID_traits<SpacePointContainer>::ID();
    }

   /** return class ID */
  virtual const CLID& clID() const
    {
      return classID();
    }


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////



  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  SpacePointContainer() = delete;
  SpacePointContainer(const SpacePointContainer&) = delete;
  SpacePointContainer &operator=(const SpacePointContainer&) = delete;
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

};
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(SpacePointContainer,1273119430,1)


CLASS_DEF( SpacePointCache , 198940329 , 1 )


#endif // TRKSPACEPOINT_SPACEPOINTCONTAINER_H
