/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTAINERS_SELECTALLOBJECTMT_H
#define EVENTCONTAINERS_SELECTALLOBJECTMT_H

//after SelectAllObjectMT is destroyed all iterators become invalid!
template < class DCC, typename OBJECT = typename DCC::IDENTIFIABLE::DIGIT >
class SelectAllObjectMT
{
public:
    typedef typename DCC::IDENTIFIABLE DC;
//	typedef typename DC::OBJECT OBJECT;
//	typedef OBJECT OBJECT;
    typedef SelectAllObjectMT<DCC,OBJECT>  MyType;
    typedef typename DC::const_iterator Object_const_iterator;
    typedef typename DCC::Hash_Container::const_iterator Hash_iterator;

    class const_iterator
    {
        Hash_iterator m_hash_itr;
        const SelectAllObjectMT<DCC,OBJECT>* m_Parent;
        Object_const_iterator m_digit_it;
        const DC* m_dc;  // cache DC
        friend class SelectAllObjectMT<DCC,OBJECT>;

        void AdvanceCollection() {

SKIPNEXT:

            ++m_hash_itr;
            if(m_hash_itr != m_Parent->m_hashes.cend()) {
                m_dc = m_Parent->m_dcc->indexFindPtr(*m_hash_itr);
#ifdef IdentifiableCacheBaseRemove
                if(m_dc == nullptr) goto SKIPNEXT;//In case we implement remove
#endif
                m_digit_it = m_dc->begin();
                if(m_digit_it == m_dc->end()) goto SKIPNEXT;
            } else {
                m_dc = nullptr;
                m_digit_it = Object_const_iterator();
            }
        }

    public:

        const_iterator()
            :
            m_hash_itr(),
            m_Parent(nullptr),
            m_digit_it(),
            m_dc(nullptr)
        {}

        const_iterator(const Hash_iterator &itr, const SelectAllObjectMT<DCC,OBJECT>* parent,
                       Object_const_iterator digitit, const DC* dc )
            :
            m_hash_itr(itr),
            m_Parent(parent),
            m_digit_it(digitit),
            m_dc(dc)
        {}

        const OBJECT*  operator * ()
        {
            return  *m_digit_it;
        }




        const_iterator& operator ++ () // pre increment
        {
            ++m_digit_it;
            if(m_digit_it != m_dc->end()) {
                // OK
            } else
            {
                // reached end of a collection,
                AdvanceCollection();
            }
            return *this;

        }


        const_iterator operator ++ ( int ) // post increment
        {   const_iterator tmp = *this;
            ++*this;
            return tmp;
        }


        bool operator != ( const const_iterator it ) const
        {
            if( it.m_hash_itr != m_hash_itr || it.m_digit_it != m_digit_it )
                return true;

            return false;
        }

        bool operator == ( const const_iterator it ) const
        {
            return( it.m_hash_itr == m_hash_itr && it.m_digit_it == m_digit_it );
        }



    };

    SelectAllObjectMT(const DCC* dcc) : m_dcc(dcc), m_hashes(dcc->GetAllCurrentHashs())
    { }

    SelectAllObjectMT( ) = delete;

    const_iterator begin() {
        Hash_iterator b = m_hashes.cbegin();
NEXT:
        const DC* dc = nullptr;
        Object_const_iterator digit_it;
        if(b!= m_hashes.cend()) {
            dc = m_dcc->indexFindPtr(*b);
            digit_it = dc->begin();
            if(digit_it == dc->end()){ 
               ++b;
               goto NEXT;
            }
        }
        return const_iterator(b, this, digit_it, dc);
    }

    const_iterator end() {
        Hash_iterator b = m_hashes.cend();
        const DC* dc = nullptr;
        Object_const_iterator digit_it;
        return const_iterator(b, this, digit_it, dc);
    }

    //Compatability with C++11
    const_iterator cbegin() {
        return begin();
    }
    const_iterator cend() {
        return end();
    }


private:
    // pointer to the container it is processing.
    const DCC* m_dcc;
    const typename DCC::Hash_Container m_hashes;
};


#endif
