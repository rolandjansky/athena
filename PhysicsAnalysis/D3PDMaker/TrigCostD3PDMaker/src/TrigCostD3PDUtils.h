// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOSTD3PDMAKER_TRIGCOSTD3PDUTILS_H
#define TRIGCOSTD3PDMAKER_TRIGCOSTD3PDUTILS_H

//Forward declarations
class TrigMonEvent;
class TrigMonEventCollection;

namespace D3PD {

   /**
    *  @short Utility functions for Trig Cost D3PD maker
    *
    *         Handle the fact our data comes in a DataVector and we may wish to request
    *         the saving of different elements of the vector
    *
    * @author Tim Martin
    *
    * $Date: April 2013
    */

     const TrigMonEvent* getObjectFromVector(const TrigMonEventCollection& eventCollection, uint32_t objectLocation);

} // namespace D3PD

#endif // TRIGCOSTD3PDMAKER_TRIGCOSTD3PDUTILS_H
