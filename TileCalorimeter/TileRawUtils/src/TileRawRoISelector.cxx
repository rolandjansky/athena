/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TileRawUtils/TileRawRoISelector.h" 
#include "TileRawUtils/TileRoI_Map.h" 


// Implementation of const_iterator 

// constructor 
TileRawRoISelector::const_iterator::const_iterator
(const VEC_ITPAIR* v)
{
	m_vec_itpair = v; 
	m_pair_it = v->begin() ; 
	if(m_pair_it != v->end()){
	  m_obj_it = (*m_pair_it).first ; 
	  m_obj_it_end = (*m_pair_it).second ; 
        } 
	
 	return; 
}

// constructor for end iterator 
TileRawRoISelector::const_iterator::const_iterator
(const VEC_ITPAIR* v, const ITPAIR_IT& it)
{
	m_vec_itpair = v; 
	m_pair_it = it ; 
	
 	return; 
}

TileRawRoISelector::const_iterator::const_iterator
() 
{
	m_vec_itpair = NULL ; 
 	return; 
}


// ++ operator 
TileRawRoISelector::const_iterator& 
TileRawRoISelector::const_iterator:: operator ++ ()
{
  ++m_obj_it; 
  if(m_obj_it == m_obj_it_end) {
	// end of this RoI, get next RoI 
	++m_pair_it; 
	if(m_pair_it != m_vec_itpair->end() ) 
	{
	  m_obj_it = (*m_pair_it).first; 
	  m_obj_it_end = (*m_pair_it).second; 
	}
  } 

  return *this; 
}


// ++ operator 
TileRawRoISelector::const_iterator 
TileRawRoISelector::const_iterator:: operator ++ (int )
{
 const_iterator tmp = *this; 
 ++tmp;  
 return tmp;
} 


// != operator 
bool 
TileRawRoISelector::const_iterator:: operator != 
	( const TileRawRoISelector::const_iterator & it ) const 
{
  // compare m_pair_it first because for end, m_pair_it is end. 
  if(it.m_pair_it == it.m_vec_itpair->end() ) 
  {   return 	  m_pair_it     != it.m_pair_it   ; 
  } else 
  return 
	  m_pair_it     != it.m_pair_it         || 
	  m_obj_it      != it.m_obj_it          ||
	  m_obj_it_end  != it.m_obj_it_end      ||
	  m_vec_itpair  != it.m_vec_itpair     ; 

}

// == operator 
bool 
TileRawRoISelector::const_iterator:: operator ==
	( const TileRawRoISelector::const_iterator & it ) const 
{
  // compare m_map_it first because for end, m_map_it is end. 

  // if m_pair_it is end, only compare the pair iterator 
  if(m_pair_it == m_vec_itpair->end() ) 
     return 	  m_pair_it     == it.m_pair_it   ; 

  return 
	  m_pair_it     == it.m_pair_it         &&
	  m_obj_it      == it.m_obj_it          &&
	  m_obj_it_end  == it.m_obj_it_end      &&
	  m_vec_itpair  == it.m_vec_itpair     ; 
}


// == operator 
const TileRawRoISelector::OBJECT *
TileRawRoISelector::const_iterator:: operator *	() 
{
  return *m_obj_it; 
}
/***************  Implementation of TileRawRoISelector
* 
*/

// Constructor 
TileRawRoISelector ::TileRawRoISelector ( TileRoI_Map* m, 
	    TileRawChannelContainer* c  ):
m_roiMap(m),
m_container(c) 
{

}


// set methods 
void TileRawRoISelector :: setMap(TileRoI_Map* m) {
 m_roiMap = m; 
}
   

void TileRawRoISelector::setContainer(TileRawChannelContainer* c) {
 m_container = c; 
}

/*
void TileRawRoISelector :: setRoIs( const VEC_TT_ID& ids) 
{

  // clean it up. 
  if(m_vecItPair.size() > 0 ) {
	m_vecItPair.erase(m_vecItPair.begin(), m_vecItPair.end()); 
  } 

  TT_ID_IT it = ids.begin(); 
  TT_ID_IT it_end = ids.end() ; 

  for(; it!=it_end; ++it){
  // 
	const TT_ID&  rId = (*it); 
 	std::vector<COLL_ID> cIDs = m_roiMap->CollectionID(rId); 
	std::vector<COLL_ID>::const_iterator cId_it = cIDs.begin();
	std::vector<COLL_ID>::const_iterator cId_it_end = cIDs.end();
	for(; cId_it!=cId_it_end;++cId_it){
	 const COLL_ID& cId = (*cId_it); 
	 CONTAINER::const_iterator coll_it = m_container->find(cId);
	 if(coll_it!=m_container->end()){
	  // Collection exist 
	  const COLLECTION* coll = (*coll_it); 
	  OBJ_IT b; 
	  OBJ_IT e; 
	  coll->getTT(rId,b,e); 
	  if(b!=e){
	    m_vecItPair.push_back(ITPAIR(b,e) ) ; 
  	  } 
         }
	}
  }
  

}
*/

 TileRawRoISelector::const_iterator
 TileRawRoISelector ::begin() const
{
  return const_iterator(&m_vecItPair); 
}

 TileRawRoISelector::const_iterator
 TileRawRoISelector ::end() const 
{
  return const_iterator(&m_vecItPair, m_vecItPair.end() ); 
}
