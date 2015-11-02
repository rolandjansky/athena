/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef TrigSteeringEvent_TrigPassFlags_h
#define TrigSteeringEvent_TrigPassFlags_h

#include <vector>
#include <algorithm>
#include <stdexcept>

#include "xAODCore/CLASS_DEF.h"
// TrigPassFlagsCollection is included at the end (required by Trigger EDM schema)

/**
 * A Flag is an ordered collection of bits (vector<bool>) that can
 * hold additional (boolean) information about a trigger object. This
 * information can be stored by a Hypo algorithm, together with the
 * trigger object in the same TE. Since trigger objects are often
 * stored within a container in the TE, the TrigPassFlags class acts a
 * container for the corresponding flags.
 *
 * Note that all flags witin a TrigPassFlags object have to be of the
 * same size (given as parameter flagSize in the constructor)
 *
 * @warning Hypo algorithms should use the helper functions (see below) to modify the TrigPassFlags content
 **/
class TrigPassFlags {
public:
   TrigPassFlags();
   /**
    * @brief Constructor to be used by the HLT algorithms
    *   Initialized the flags vector.
    * @param size the size of container for which the flags will be recorded. This determines the length of the outer vector<vector<bool> > of m_flagsPerObject.
    * @param flagSize the size of the flags. This determines the length of the inner vector<bool> m_flagsPerObject.
    * @param cont pointer to the container, if given then the member functions will be performing additional check if the bits setting is the same (desired initailly container)
    **/
   TrigPassFlags(const unsigned int size, const unsigned int flagSize, const void *cont=0);
   
   /**
    * @brief Set bit of the flag at index position.
    * @param position postion of the flag in the vector of flags
    * @param bitPosition position of the bit within the flag
    * @param bitValue enable/disable the bit
    * @warning Hypo algorithms should use the helper function HLT::setFlagBit (see below) to modify the TrigPassFlags
    **/
   void setFlagBit(const unsigned int position, const unsigned int bitPosition, const bool bitValue, const void *cont=0 ); 

   /**
    * @brief Set the flag at index position
    * @param position postion of the flag in the vector of flags
    * @param flagValue value of the flag (can be used if flagSize is smaller than sizeof(int)
    * @warning Hypo algorithms should use the helper function HLT::setFlag (see below) to modify the TrigPassFlags
    **/
   void setFlag(const unsigned int position, const std::vector<bool>& flag, const void *cont=0 ); 
   
   /**
    * @brief Returns the bit 'bitPosition' of the flag at index position
    * @warning Hypo algorithms should use the helper function HLT::getFlagBit (see below) to access the TrigPassFlags
    **/
   bool getFlagBit(const unsigned int position, const unsigned int bitPosition) const;

   /**
    * @brief Returns the flag (vector<bool>) at index position
    * @warning Hypo algorithms should use the helper function HLT::getFlag (see below) to access the TrigPassFlags
    **/
   const std::vector<bool>& getFlag(const unsigned int position) const;

   /**
    * @brief gets size of the container object vector
    **/
   unsigned int size() const { return m_flagsPerObject.size(); }

   /**
    * @brief gets size of the flag vector for the object at index
    **/
   unsigned int flagSize() const { return m_flagsPerObject[0].size(); }

private:
   friend class TrigPassFlagsCnv_p1;

   const void *m_container_ptr;
   std::vector<std::vector<bool> > m_flagsPerObject; //!< list of trainsient n-bit value (STL docu assures that a vector of booleans is efficient)
};

CLASS_DEF( TrigPassFlags , 74395451 , 1 )


/**
 * @brief Helper functions to be used by the Hypo algorithms
 *
 **/
namespace HLT {
   template<class CONTAINER>
   TrigPassFlags* makeTrigPassFlags(const CONTAINER* cont,const unsigned int flagSize) {
      return new TrigPassFlags(cont->size(), flagSize, (const void*)cont );
   }
   
   /**
    * @brief Set bit of the flag at index position for a given TrigPassFlags object
    *
    **/
   template<class T, class CONTAINER>
   void setFlagBit(TrigPassFlags * flags, const T* obj, const CONTAINER* container, const unsigned int bitPosition, const bool bitValue = true) {
      typename CONTAINER::const_iterator i = std::find(container->begin(), container->end(), obj);
      if ( i != container->end() ) 
         flags->setFlagBit(i-container->begin(), bitPosition, bitValue, container); 
      else
         throw std::runtime_error("The CONTAINER passed does not match the CONTAINER that created the TrigPassFlags");
   }


   /**
    * @brief Set the flag at index position
    * @param obj trigger object to which the flag corresponds
    * @param container container of trigger objects (same pointer as given in the constructor)
    * @param flag value of the flag (use helper functions to convert from int or long to vector<bool>)
    **/
   template<class T, class CONTAINER>
   void setFlag(TrigPassFlags * flags,  const T* obj, const CONTAINER* container, const std::vector<bool>& flag) {
      typename CONTAINER::const_iterator i = std::find(container->begin(), container->end(), obj);
      if ( i != container->end() ) 
         flags->setFlag(i-container->begin(), flag, container); 
      else
         throw std::runtime_error("The CONTAINER passed does not match the CONTAINER that created the TrigPassFlags");
   }


   /**
    * @brief Returns the bit 'bitPosition' of the flag at index position
    **/
   template<class T, class CONTAINER>
   bool getFlagBit(const TrigPassFlags *flags,const T* obj, const CONTAINER* container, const unsigned int position, const unsigned int bitPosition) {
      typename CONTAINER::const_iterator i = std::find(container->begin(), container->end(), obj);
      if ( i != container->end() )
         return flags->getFlagBit(i-container->begin(),position, bitPosition);
      throw std::runtime_error("The CONTAINER passed does not match the CONTAINER that created the TrigPassFlags");
   }
   
   /**
    * @brief Returns the flag at index position
    **/
   template<class T, class CONTAINER>
   const std::vector<bool>& getFlag(const TrigPassFlags *flags,const T* obj, const CONTAINER* container, const size_t position) {
      typename CONTAINER::const_iterator i = std::find(container->begin(), container->end(), obj);
      if ( i != container->end() )
         return flags->getFlag(i-container->begin(),position);
      throw std::runtime_error("The CONTAINER passed does not match the CONTAINER that created the TrigPassFlags");
   }
   
   template<typename T>
   std::vector<bool> AsFlag(T flag_t, const size_t size) {  // T should be an unsigned type, otherwise this will give a compilation warning
      if(size>8*sizeof(T)) // check of T has at list 'size' bits
         throw std::runtime_error("AsFlag(): type T has less bits than required by 'size'");
      if(flag_t >= ( ((unsigned long long)1)<<size)  ) // check of T has at least 'size' bits (with 'size' bits '1<<size' is the lowest number I can not represent.)
         throw std::runtime_error("AsFlag(): the flag is larger then bits available");


      std::vector<bool> flag(size);
      int mask = 0x01;
      for(size_t idx=0; idx<size; ++idx, mask<<=1)
         flag[idx] = (flag_t&mask)!=0;
      return flag;
   }

   template<typename T>
   T FlagAs(const std::vector<bool> flag) {
      if(8*sizeof(T)<flag.size()) // check of T has enough bits to represent flag
         throw std::runtime_error("FlagAs(): the flag size does not fit into the requested type");
      T v = 0;
      int mask = 0x01;
      for(size_t idx=0; idx<flag.size(); ++idx, mask<<=1)
         if(flag[idx]) v += mask;
      return v;
   }

} // eof HLT namespace

#include "TrigSteeringEvent/TrigPassFlagsCollection.h"

#endif // TrigSteeringEvent_TrigPassFlags_h




