/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERAWUTILS_TILETT_SELECTOR_H
#define TILERAWUTILS_TILETT_SELECTOR_H

#include "Identifier/Identifier.h" 

class TileRoI_Map; 

template <class CONTAINER >
class TileTT_Selector
{

  public: 
    /** Constructors 
    */ 
    /** define an iterator
    */ 


    /** typedefs 
    */ 

//    typedef TileRawChannelContainer CONTAINER; 
//    typedef TileRawChannelCollection COLLECTION;
//    typedef TileRawChannel OBJECT; 

    typedef typename CONTAINER::IDENTIFIABLE  COLLECTION;
    typedef typename CONTAINER::const_iterator  COLL_IT; 
    typedef typename COLLECTION::const_iterator OBJ_IT; 
    typedef typename COLLECTION::OBJECT         OBJECT; 
    typedef typename COLLECTION::ID COLL_ID; 
    typedef typename std::vector<COLL_ID>::const_iterator COLL_ID_IT; 

    // a vector of ROI_ID 
    // typedef TileRoI_ID	ROI_ID; 
    typedef Identifier TT_ID; 
    typedef std::vector<TT_ID> VEC_TT_ID; 
    typedef VEC_TT_ID::const_iterator TT_ID_IT; 

    typedef std::pair<OBJ_IT,OBJ_IT> ITPAIR; 
    typedef std::vector<ITPAIR>  VEC_ITPAIR; 
    typedef typename VEC_ITPAIR::const_iterator ITPAIR_IT; 

    // a map of Collection ID and vector 
    //     a Collection is associated with a list of TT_ID 
    typedef std::map<const COLLECTION*, VEC_TT_ID>  MAP_COLL_ROI  ;
    typedef typename MAP_COLL_ROI::const_iterator   MAP_IT        ;

    class const_iterator 
     {
        friend class TileTT_Selector<CONTAINER> ; 
	public:
	const_iterator() ; 

	const_iterator& operator = ( const const_iterator & it ) ; 

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

   TileTT_Selector ( const TileRoI_Map* map= 0, 
	    const CONTAINER* c=NULL );

   virtual ~TileTT_Selector () {} ; 

   void setMap(const TileRoI_Map* map) ; 
   void setContainer(const CONTAINER* c) ;
   void setRoIs( const VEC_TT_ID& ids) ; 

   const_iterator begin() const ; 
   const_iterator end() const ; 

 private: 
   void initialize(); 

   const TileRoI_Map* m_roiMap; 
   const CONTAINER* m_container; 

   VEC_ITPAIR  m_vecItPair;
   
};

#include "TileRawUtils/TileTT_Selector.icc" 

#endif 
