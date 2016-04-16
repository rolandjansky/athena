/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef TrigSteeringEvent_TrigPassBits_h
#define TrigSteeringEvent_TrigPassBits_h

#include <vector>
#include <algorithm>
#include <stdexcept>

#include "xAODCore/CLASS_DEF.h"
// TrigPassBitsCollection is included at the end (required by Trigger EDM schema)


class TrigPassBits {
public:
  TrigPassBits();

  /**
   * @brief Constructor to be used by the HLT algorithms
   *   Else the mark_passing and is_passing won't work.
   * @param sz the size of container for which the bits storing decision for particular obj will be stored.
   * @param cont pointer to the container, if given then the mark_passing will be performing additional check if the bits setting is about the same (desired initailly container)
   **/
  TrigPassBits(unsigned sz, const void *cont=0);

  /**
   * @brief Set the bit value a the position to true.
   * @warning The HLT code should be using helper defined of the same name for each container type needed.
   **/
  void markPassing( unsigned position, const void *cont=0 ); 

  /**
   * @brief Check the bit value a the position.
   * @warning The HLT code should be using helper of the same name defined below.
   **/
  bool isPassing( unsigned position ) const; 

  /**
   * @brief gets size of the bits array
   **/
  unsigned int size() const { return m_decisions.size(); }

private:
  friend class TrigPassBitsCnv_p1;
  const void *m_container_ptr;
  std::vector<bool> m_decisions; //!< trainsient bits storage (STL docu assures that this is efficient)
};

CLASS_DEF( TrigPassBits , 188050889 , 1 )

namespace HLT {
  
  template<class CONTAINER>  
  TrigPassBits* makeTrigPassBits(const CONTAINER* cont) {
    return new TrigPassBits(cont->size(), (const void*)cont );
  }



  /**
   * @brief Set the bit for the object in the associated bits object.
   *
   * The helper function should be used in the HLT algorithms instead of the raw TrigPassBits accessors.
   **/
  template<class T, class CONTAINER>
    void markPassing (TrigPassBits *bits,  const T* obj, const CONTAINER* container) {    
    typename CONTAINER::const_iterator i = std::find(container->begin(), container->end(), obj);
    if ( i != container->end() ) 
      bits->markPassing(i-container->begin(), container); 
    else
      throw std::runtime_error("When marking object as passing the trigger found object not from container");
  }
  

  /**
   * @brief Check the bit for the object in the associated bits object.
   **/
  template<class T, class CONTAINER>
    bool isPassing (const TrigPassBits *bits,const T* obj, const CONTAINER* container) {      
    typename CONTAINER::const_iterator i = std::find(container->begin(), container->end(), obj);
    if ( i != container->end() ) 
      return bits->isPassing(i-container->begin());
    else
      throw std::runtime_error("When checking that object passed the trigger found object not from container");
    return false;
  }
} // eof HLT namespace

#include "TrigSteeringEvent/TrigPassBitsCollection.h"

#endif // TrigSteeringEvent_TrigPassBits_h




