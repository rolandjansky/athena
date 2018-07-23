/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_SELECTOBJECT_H
#define EVENTCONTAINERS_SELECTOBJECT_H

// This is a Selection class that provides iterator over selected
// objects contained in the IdentifiableContainer.  The Identifiable in the
// Container is presumed to be a DigitCollection (DC). DC 
// should define OBJECT class in its scope, as well as const_iterator
// class for accessing the OBJECTs. 
// 
//  The concrete class should implement the select methods, applied to 
// both the collection and the objects. 
// 
// The const_iterator classes, both in DC and DCC, should follow the 
// standard STL iterator syntax.

template < class DCC, typename OBJECT = typename DCC::IDENTIFIABLE::DIGIT >
class SelectObject
{
public:

	typedef typename DCC::IDENTIFIABLE DC;
//	typedef typename DC::OBJECT OBJECT;
//	typedef OBJECT OBJECT;
	typedef SelectObject<DCC,OBJECT>  MyType;
	typedef typename DCC::const_iterator DC_const_iterator;
	typedef typename DC::const_iterator Object_const_iterator;

	class const_iterator 
	{
	public:
		const_iterator() {} 
		const_iterator& operator = ( const const_iterator & it ) 
			{ m_dcc = it.m_dcc; 
			  m_digit_it = it.m_digit_it;
			  m_dc_it = it.m_dc_it;
			  m_dc = it.m_dc; 
                          return *this; 
                        }

		const_iterator& operator ++ () // pre increment
			{ 
			  ++m_digit_it;
			  advanceObject(); 
		  	  return *this; 
			}

		void advanceObject(){
			
			  for(;	m_digit_it!=m_dc->end();++m_digit_it) {
			     //  check if this digit is accepted. 
			     if(m_select->select(*m_digit_it)){
				return  ;
			     }
			  } 
		         // reached end of a collection, 
			 ++m_dc_it;
			 advanceCollection(); 
			 return ;
		}

		void advanceCollection(){
			 for( ; m_dc_it != m_dcc->end();++m_dc_it)
			   {
				m_dc       = *m_dc_it ; 
				if(m_select->select(m_dc)){
 	 			 for(m_digit_it = m_dc->begin() ;  
				  m_digit_it!=m_dc->end();++m_digit_it) {
				  //  check if this digit is accepted. 
			          if(m_select->select(*m_digit_it)) 
				    return ; 
			         }
				}
			  }
			  // reach here only when mc_dc_it == end
			  return ; 

			}

		const_iterator operator ++ ( int ) // post increment
			{       const_iterator tmp = *this; 
				++*this;  
				return tmp; 
			}

		const OBJECT*  operator * () 
			{ return *m_digit_it; }

		// comparison operator.
		// if m_dc_it == end, don't compare digit_it
		bool operator != ( const const_iterator it ) const
		 { if( it.m_dcc != m_dcc || it.m_dc_it != m_dc_it ) 
			return true; 
		   if( m_dcc && m_dc_it != m_dcc->end()) {
		      // check digit_it only if it is not end
		      if(it.m_digit_it != m_digit_it) return true; 
		      return false; 
		   } 
		   return false; 		   
		 }
		bool operator == ( const const_iterator it ) const
			{ return it.m_dcc == m_dcc 
			      && it.m_dc_it == m_dc_it 
			      && ((it.m_digit_it == m_digit_it) || 
				  (m_dcc && (m_dc_it==m_dcc->end())));
			// if both dc_it == end, then don't check digit_it
			}

		//	protected:
		// friend class MyType; 
		friend class SelectObject<DCC,OBJECT> ;

		const_iterator( MyType* s, const DCC* dcc,  DC_const_iterator& dc_it )
			: m_dcc(dcc), m_dc_it(dc_it),m_select(s) 
	         {
		  if(m_dcc && m_dc_it!=m_dcc->end()) 
		   {
		     advanceCollection(); 
		   }

		 }

		const DCC* m_dcc; 
		DC_const_iterator m_dc_it;
		const DC* m_dc;  // cache DC 
		Object_const_iterator m_digit_it;
		MyType* m_select; 
	};

	// friend class const_iterator; 
	// Constructor initializes the DataLinkVector 
	SelectObject(const DCC* dcc) : m_dcc(dcc) 
	{ }

	// return iterator on first entry
	const_iterator begin() 
	  { DC_const_iterator dc_it = m_dcc->begin(); 
	    return const_iterator ( this, m_dcc, dc_it ); 
	  }

	const_iterator end() 
	  { DC_const_iterator dc_it = m_dcc->end(); 
	    return const_iterator ( this, m_dcc, dc_it );
	  }

	//  apply selection to the collection
	virtual bool select(const DC* dc) = 0 ; 
	//  apply selection to the object 
	virtual bool select(const OBJECT* dc) = 0 ; 

private: 
	SelectObject( )
	{ }

	// pointer to the container it is processing. 
	const DCC* m_dcc; 

};

#endif 
