/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_RESOLVEDCOLLECTION_H
#define DCMATH_RESOLVEDCOLLECTION_H

#include <vector>
#include <iostream>

namespace TrkDriftCircleMath {

  template<class Data, class IsSubset>
    class ResolvedCollection {
  public:
    typedef  std::vector<Data> DataVec;
    enum SubsetState { SubSet=0, SuperSet=1, Different=2 };
  public:
    
    const DataVec& data() const { return m_data; }
    DataVec& data() { return m_data; }

    void clear() { m_data.clear(); }

    void set( DataVec& data ) { m_data.swap(data); }

    bool insert( const Data& data ){ 

/*       std::cout << " ******** insert " << data << " current size " << m_data.size() << std::endl; */
      // add data if collection is empty
      if( m_data.empty() ) {
	m_data.push_back(data);
	return true;
      }
    
      IsSubset isSubset;

      bool inserted(false);
      

      // loop over data vector compare new element with existing ones 
      for( unsigned int i=0;i<m_data.size();++i ){

	// get subset state
	int state = isSubset( data, m_data[i] );

/* 	std::cout << " compare with " << i << "   -> " << state << std::endl; */

	// do nothing in case the new element is a subset of an already inserted element
	if( state == SubSet ){
/* 	  std::cout << " SubSet: exit with size " << m_data.size() << std::endl; */
	  return false;

	  // if the new element is a super set of an existing on replace it
	  // check all existing elements and keep track of number of replaces
	}else if( state == SuperSet ){
	  
/* 	  std::cout << " SuperSet " << std::endl; */
	  // if not inserted yet replace the subset
	  if( !inserted ){
	    inserted = true;
	    m_data[i] = data;
/* 	    std::cout << " replaced " << i << std::endl; */
	  }else{
	    // replace current element with last and restart loop 
	    if( i == m_data.size()-1 ) {
	      m_data.pop_back();
/* 	      std::cout << " removed back " << std::endl; */
	    }else{
/* 	      std::cout << " moved back to " << i << std::endl; */
	      m_data[i] = m_data.back();
	      m_data.pop_back();
	      --i;  // here we go back to also check the copied element
	    }
	  }
	}
      
      }

      // check if new element was SuperSet of existing one
      if( !inserted ) {
	// insert new element
	m_data.push_back(data);
      }
/*       std::cout << " new size " << m_data.size() << std::endl; */
      return true;
    }
    

  private:
    std::vector<Data> m_data;
  };


}

#endif
