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
    typedef typename DCC::const_iterator Cont_iterator;
//    typedef typename DCC::Hash_Container::const_iterator Hash_iterator;

    class const_iterator
    {
        Cont_iterator m_cont_itr;
        const SelectAllObjectMT<DCC,OBJECT>* m_Parent;
        Object_const_iterator m_digit_it;
        const DC* m_dc;  // cache DC
        friend class SelectAllObjectMT<DCC,OBJECT>;

        void AdvanceCollection() {

SKIPNEXT:

            ++m_cont_itr;
            if(m_cont_itr != m_Parent->m_dcc->end()) {
                m_dc = *m_cont_itr;
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
            m_cont_itr(),
            m_Parent(nullptr),
            m_digit_it(),
            m_dc(nullptr)
        {}

        const_iterator(const Cont_iterator &itr, const SelectAllObjectMT<DCC,OBJECT>* parent,
                       Object_const_iterator digitit, const DC* dc )
            :
            m_cont_itr(itr),
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
            if( it.m_cont_itr != m_cont_itr || it.m_digit_it != m_digit_it )
                return true;

            return false;
        }

        bool operator == ( const const_iterator it ) const
        {
            return( it.m_cont_itr == m_cont_itr && it.m_digit_it == m_digit_it );
        }



    };

    SelectAllObjectMT(const DCC* dcc) : m_dcc(dcc)
    { }

    SelectAllObjectMT( ) = delete;

    const_iterator begin() {
        auto b = m_dcc->begin();
NEXT:
        const DC* dc = nullptr;
        Object_const_iterator digit_it;
        if(b!= m_dcc->end()) {
            dc = *b;
            digit_it = dc->begin();
            if(digit_it == dc->end()){ 
               ++b;
               goto NEXT;
            }
        }
        return const_iterator(b, this, digit_it, dc);
    }

    const_iterator end() {
        auto b = m_dcc->end();
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
};


#endif
