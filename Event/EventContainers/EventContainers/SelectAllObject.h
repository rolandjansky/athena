/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_SELECTALLOBJECT_H
#define EVENTCONTAINERS_SELECTALLOBJECT_H

//  This is a Selection class that provides iterator over all objects
// contained in the IdentifiableContainer.  The Identifiable in the
// Container is presumed to be a DigitCollection (DC). DC 
// should define OBJECT class in its scope, as well as const_iterator
// class for accessing the OBJECTs. 

// The const_iterator classes, both in DC and DCC, should follow the 
// standard STL iterator syntax.

template < class DCC, typename OBJECT = typename DCC::IDENTIFIABLE::DIGIT >
class SelectAllObject
{
public:

	typedef typename DCC::IDENTIFIABLE DC;
//	typedef typename DC::OBJECT OBJECT;
//	typedef OBJECT OBJECT;
	typedef SelectAllObject<DCC,OBJECT>  MyType;
	typedef typename DCC::const_iterator DC_const_iterator;
	typedef typename DC::const_iterator Object_const_iterator;

	class const_iterator 
	{
	public:
                const_iterator() 
                  : m_dcc(nullptr),
                    m_dc_it(),
                    m_dc(nullptr),
                    m_digit_it()
                {}

		const_iterator& operator ++ () // pre increment
			{ ++m_digit_it; 
			  if(m_digit_it != m_dc->end()) {
			  // OK 
			  } else 
			  {
			    // reached end of a collection, 
			    ++m_dc_it; 
				
			    // advance until we see a valid digit, 
			    //   or all the way to the end.
			    while(m_dc_it != m_dcc->end() ){
				m_dc       = *m_dc_it ; 
				m_digit_it = m_dc->begin() ;  
				if(m_digit_it != m_dc->end()){
					break; 
				} else 
				{   ++m_dc_it; 
				}
			    } 
			  }
			  return *this; 

			}

		const_iterator operator ++ ( int ) // post increment
			{       const_iterator tmp = *this; 
				++tmp;  
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
		friend class SelectAllObject<DCC,OBJECT> ;

		const_iterator( const DCC* dcc,  DC_const_iterator& dc_it )
                  : m_dcc(dcc), m_dc_it(dc_it), m_dc(0)
	         {
		  while(m_dcc && m_dc_it!=m_dcc->end()) 
		   {
		     m_dc = *m_dc_it; 
		     m_digit_it = m_dc->begin(); 
		     if(m_digit_it != m_dc->end() ) break; 
		     ++m_dc_it; 
		     // FIXME !! need to check if m_digit_it is end.

		   }

		 }

		const DCC* m_dcc; 
		DC_const_iterator m_dc_it;
		const DC* m_dc;  // cache DC 
		Object_const_iterator m_digit_it;

	};

	// friend class const_iterator; 
	// Constructor initializes the DataLinkVector 
	SelectAllObject(const DCC* dcc) : m_dcc(dcc) 
	{ }

	// return iterator on first entry
	const_iterator begin() 
	  { DC_const_iterator dc_it = m_dcc->begin(); 
	    return const_iterator ( m_dcc, dc_it ); 
	  }

	const_iterator end() 
	  { DC_const_iterator dc_it = m_dcc->end(); 
	    return const_iterator ( m_dcc, dc_it );
	  }

private: 
	SelectAllObject( )
	{ }

	// pointer to the container it is processing. 
	const DCC* m_dcc; 

};


#endif 
