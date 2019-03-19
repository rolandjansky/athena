/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERAWUTILS_TILERAWROISELECTOR_H
#define TILERAWUTILS_TILERAWROISELECTOR_H


#include "TileEvent/TileRawChannelContainer.h" 
#include "TileEvent/TileRawChannelCollection.h" 
#include "TileEvent/TileRawChannel.h"
#include "Identifier/Identifier.h"
#include <vector>
#include <map>

class TileRoI_Map; 

class TileRawRoISelector
{

  public: 
    /** Constructors 
    */ 
    /** define an iterator
    */ 


    /** typedefs 
    */ 

    typedef TileRawChannelContainer CONTAINER; 
    typedef TileRawChannelCollection COLLECTION;
    typedef TileRawChannel OBJECT; 
    typedef CONTAINER::const_iterator  COLL_IT; 
    typedef COLLECTION::const_iterator OBJ_IT; 

    typedef COLLECTION::ID COLL_ID; 

    // a vector of ROI_ID 
    // typedef TileRoI_ID	ROI_ID; 
    typedef Identifier	TT_ID; 
    typedef std::vector<TT_ID> VEC_TT_ID; 
    typedef VEC_TT_ID::const_iterator TT_ID_IT; 

    typedef std::pair<OBJ_IT,OBJ_IT> ITPAIR; 
    typedef std::vector<ITPAIR>  VEC_ITPAIR; 
    typedef VEC_ITPAIR::const_iterator ITPAIR_IT; 

    // a map of Collection ID and vector 
    //     a Collection is associated with a list of TT_ID 
    typedef std::map<const COLLECTION*, VEC_TT_ID>  MAP_COLL_ROI  ;
    typedef MAP_COLL_ROI::const_iterator   MAP_IT        ;

    class const_iterator 
     {
        friend class TileRawRoISelector ; 
	public:
	const_iterator() ; 

        const_iterator& operator = ( const const_iterator & it ) = default;

	const_iterator& operator ++ () ; 

	const_iterator operator ++ ( int ); 

	const OBJECT*  operator * () ; 

	bool operator != ( const const_iterator& it ) const ;

	bool operator == ( const const_iterator& it ) const ;


	private: 
	  const_iterator( const VEC_ITPAIR* v ) ; 
	  const_iterator( const VEC_ITPAIR* v, const ITPAIR_IT& it ) ; 

	  const VEC_ITPAIR* m_vec_itpair; 
	  ITPAIR_IT  m_pair_it; 
	  OBJ_IT     m_obj_it    ;
	  OBJ_IT     m_obj_it_end    ;

     };

   friend class const_iterator; 

   TileRawRoISelector ( TileRoI_Map* map=NULL, 
	    TileRawChannelContainer* c=NULL );

   virtual ~TileRawRoISelector () = default;

   void setMap(TileRoI_Map* map) ; 
   void setContainer(TileRawChannelContainer* c) ;
   void setRoIs( const VEC_TT_ID& ids) ; 

   const_iterator begin() const ; 
   const_iterator end() const ; 

 private: 
   void initialize(); 

   TileRoI_Map* m_roiMap; 
   TileRawChannelContainer* m_container; 

   VEC_ITPAIR  m_vecItPair;
   
};


#endif 
