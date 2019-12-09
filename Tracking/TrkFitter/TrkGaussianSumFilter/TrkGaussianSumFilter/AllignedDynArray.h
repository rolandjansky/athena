/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
  AllignedDynArray.h  -  description
  -------------------------------------------------
 begin                : 26th November 2019
 author               : amorley, Christos
 decription           : Dynamic array fullfilling alignment requirements
 *********************************************************************************/

#ifndef GSFUtils_AllignedDynArray_H
#define GSFUtils_AllignedDynArray_H

#include <stdlib.h>
namespace GSFUtils{

template<typename T,int Alignment>
/*
 * Dynamically allocate un-initialized storage where the storage is 
 * aligned according to Alignment i.e the storage starts at an adrress 
 * that should be divisible with Alignment 
 *
 * This is usefull for arrays of simple types like int,float,double
 *  
 * Notes :
 * For fixed size buffers i.e when the size is known at runtime prefer : 
 * std::aligned_storage/std::align
 *
 */

class AlignedDynArray
{
public:
  AlignedDynArray() = delete;
  AlignedDynArray(AlignedDynArray const&) = delete;
  AlignedDynArray& operator=(AlignedDynArray const&) = delete;
 
  explicit AlignedDynArray(size_t n);
  AlignedDynArray(AlignedDynArray&&);
  AlignedDynArray& operator=(AlignedDynArray&&);
  ~AlignedDynArray();

  ///conversions to T*
  operator T*(); 
  operator const T*() const; 
 
  /// index array operators  
  T& operator [](const std::size_t pos);
  const T& operator [](const std::size_t pos) const ;
 
  ///size of allocated buffer
  std::size_t size() const;

private:
  void cleanup();
  T*  m_buffer=nullptr;
  void* m_raw=nullptr;
  size_t m_size=0;
}; 

}//namespace GSFUtils

//impl .icc
#include "TrkGaussianSumFilter/AllignedDynArray.icc"

#endif
