/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 *  SCT_OnlineId.cxx
 *  SCT_Cabling
 *
 *  Created by sroe on 21/10/2008.
 *
 */

#include "SCT_Cabling/SCT_OnlineId.h"
#include <iostream>
namespace{
  ///Templated function to find a particular nibble, known at compile-time; type T must be castable to unsigned int
  template <unsigned int nibbleNumber, class T>
  inline
  unsigned int  nibble(const T number){
    return (number>>(4*nibbleNumber)) & 0xf;
  }
}

SCT_OnlineId::SCT_OnlineId(): m_onlineId(SCT_OnlineId::INVALID_ONLINE_ID){
  //nop
}

//
SCT_OnlineId::SCT_OnlineId(const std::uint32_t onlineId):m_onlineId(onlineId){
  //nop
}

SCT_OnlineId::SCT_OnlineId(const std::uint32_t rodId, const std::uint32_t fibre){
  //cursory checks on range only
  if (not (fibreInRange(fibre) and rodIdInRange(rodId) )){
    std::cout<<"SCT_OnlineId: Trying to create an invalid online Id from RodId 0x"<<std::hex<<rodId<<std::dec<<"  and fibre: "<<fibre<<std::endl;
    m_onlineId = INVALID_ONLINE_ID;
  } else {
    m_onlineId = rodId + (fibre<<24);
  }
}

SCT_OnlineId &
SCT_OnlineId::operator++(){
  m_onlineId+=(1<<24);
  if (not fibreInRange(fibre())) m_onlineId=INVALID_ONLINE_ID;
  return *this;
}

SCT_OnlineId 
SCT_OnlineId::operator++(int){
  SCT_OnlineId temp=*this;
  m_onlineId+=(1<<24);
  if (not fibreInRange(fibre())) m_onlineId=INVALID_ONLINE_ID;
  return temp;
}

//
std::uint32_t
SCT_OnlineId::rod() const {
  return m_onlineId & 0xFFFFFF;
}

//
std::uint32_t
SCT_OnlineId::fibre() const {
  return m_onlineId>>24;
}

//
SCT_OnlineId::operator unsigned int() const{
  return m_onlineId;
}

//
bool
SCT_OnlineId::is_valid(const bool usingDbCabling) const{
  return ( fibreInRange(fibre()) and rodIdInRange(rod(), usingDbCabling) );
}

//
bool
SCT_OnlineId::is_valid() const{
  return ( fibreInRange(fibre()) and rodIdInRange(rod()) );
}

  
//
bool SCT_OnlineId::fibreInRange(std::uint32_t f) const{
  return (f<NUM_FIBRES);
}

//
bool SCT_OnlineId::rodIdInRange(std::uint32_t r) const {
  const std::uint32_t lowestRodId(0x210000);
  const std::uint32_t highestRodId=0x24010F;
  return ((r >=lowestRodId) and (r<=highestRodId)) ;
}

//
bool SCT_OnlineId::rodIdInRange(std::uint32_t r, const bool usingDbCabling) const{
  const std::uint32_t lowestRodId(0x210000);
  const std::uint32_t highestRodId=0x24010F;
  //choose one unique Rod as a signature to check for either database or montecarlo.
  //0x220010 is unique to the text file, 0x220105 is unique to the database
  //as an alternative, more thorough (but slower) check, the SCT_OnlineIdType function should be used
  const std::uint32_t uniqueToOppositeCase = usingDbCabling?0x220010:0x220105;
  return ((r >=lowestRodId) and (r<=highestRodId) and (r!=uniqueToOppositeCase)) ;
}
  
//
unsigned int 
SCT_OnlineId::index() const{
  //number is of form 0xII2X0Y0Z, X=[1-4], Y=[0-2], Z=[0xO-0xf], II is 0 to 95
  //so Z is nibble 0; Y is nibble 2; X is nibble 4
  // BUT long ago...
  //MC cabling was stored in a file with robs in different format, so the online Id will be in form
  //0xII2X00YZ, X=[1-4], Y=[0-2], Z=[0xO-0xf], II is 0 to 95
  unsigned int Z(nibble<0>(m_onlineId));
  unsigned int Y(nibble<2>(m_onlineId));
  /**
  if ((Z > 11) or (nibble<1>(m_onlineId)==1) ){//its montecarlo
    Y=1;
    //possible Z values at this point are 12,13,14,15 (nibble 1 is zero) and 0,1,2,3,4,5 (nibble 1 is 1) so we map these onto 0-9
    Z= (Z > 11) ? (Z-6) : Z;
  }
  **/
  const unsigned int X(nibble<4>(m_onlineId) - 1);
  unsigned int result= ((X + (Y * 4)) * 16 + Z) * 96 + (fibre());  //should return an index in range 0-19872
  if (result > MAX_INDEX){
    std::cout<<"SCT_OnlineId: Invalid index for online Id "<<std::hex<<m_onlineId<<std::dec<<" X, Y, Z: "<<X<<", "<<Y<<", "<<Z<<std::endl;
    result=INVALID_INDEX;
  } 
  return result;
}
  
  

