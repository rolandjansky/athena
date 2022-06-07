/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef L1CALO_BS_DECODER_UTIL_H
#define L1CALO_BS_DECODER_UTIL_H

#include <list>

class L1CaloRdoRodInfo;

namespace eformat { 
   template <class TPointer> class ROBFragment;
}

class L1CaloBsDecoderUtil
{
public:

   static void decodeRodInfo( const eformat::ROBFragment<const uint32_t*>* rod,
                              std::list<L1CaloRdoRodInfo>& dat );

   template <typename Tar, typename Dat, typename Iter>
   static Tar& findRdo( const Tar& target, Dat& data, Iter begin, Iter end );

   template <typename Tar, typename Dat>
   static Tar& findRdo( const Tar& target, Dat& data );

private:
   L1CaloBsDecoderUtil();
};

template <typename Tar, typename Dat, typename Iter> Tar&
L1CaloBsDecoderUtil::findRdo( const Tar& target, Dat& data, Iter begin, Iter end )
{
   while ( (begin != end) && ( target < (*begin) ) ) 
      ++begin;

   if ( begin == end )
      return *(data.insert( end.base(), target ));  
 
   if ( begin->sameDatum( target ) )
      return *begin;

   return *(data.insert( begin.base(), target ));
}

template <typename Tar, typename Dat> Tar&
L1CaloBsDecoderUtil::findRdo( const Tar& target, Dat& data )
{
   return findRdo( target, data, data.rbegin(), data.rend() );
} 

#endif

