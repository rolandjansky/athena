/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/bitPATTERN.h"

using namespace std;

bitPATTERN::bitPATTERN(std::string name,const PATTERNidentity& id,int size) : 
    RPCtrigDataObject(0,name),
    m_id(id),
    m_size(size)
{
    m_digits = new const RPCdigit* [m_size];
    reset();
}

bitPATTERN::bitPATTERN(const bitPATTERN& pattern) : 
    RPCtrigDataObject(0,pattern.name()),
    m_id(pattern.id()),
    m_size(pattern.size())
{
    m_digits = new const RPCdigit* [m_size];
    reset();
    for(int i=0;i<m_size;++i) m_digits[i] = pattern[i];
}

bitPATTERN::~bitPATTERN() 
{
    delete [] m_digits;
}

bitPATTERN&
bitPATTERN::operator=(const bitPATTERN& pattern)
{
    if(pattern.size() != m_size) return *this;

    reset();

    m_id = pattern.id();
    for(int i=0;i<m_size;++i) m_digits[i] = pattern[i];
    return *this;
}

bool
bitPATTERN::operator<(const bitPATTERN& pattern)
{
    if(m_id < pattern.id()) return true;
    else return false;
}

void
bitPATTERN::set(const RPCdigit* digit, int bit)
{
    if(bit >= m_size || bit < 0) return;
    m_digits[bit] = digit;
}

void
bitPATTERN::unset(int bit)
{
    if(bit >= m_size || bit < 0) return;
    m_digits[bit] = 0;
}


void
bitPATTERN::reset()
{
    for(int i=0;i<m_size;++i)
    {
        m_digits[i] = 0;
    }
}

bool
bitPATTERN::any() const
{
    for(int i=0;i<m_size;++i)
    {
        if (m_digits[i]) return true;
    }
    return false;
}

bool
bitPATTERN::test(int bit) const
{    
    if(bit >= m_size || bit < 0) return false;
    if (m_digits[bit]) return true;
    else return false;
}

const RPCdigit*
bitPATTERN::operator[](int bit) const
{
    if(bit >= m_size || bit < 0) return 0;
    else return m_digits[bit];
}

bitPATTERN
bitPATTERN::operator& (const bitPATTERN& pattern) const
{
    if(pattern.size() != m_size) return *this;
    bitPATTERN result(name(),id(),m_size);  

    for(int i=0;i<m_size;++i)
    {
        if ( m_digits[i] && pattern.test(i) ) result.set(m_digits[i],i);
    }    

    return result;
}

bitPATTERN& 
bitPATTERN::operator&=(const bitPATTERN& pattern)
{
    if(pattern.size() != m_size) return *this;
    for(int i=0;i<m_size;++i)
    {
        if ( !(m_digits[i] && pattern.test(i)) ) m_digits[i] = 0;
    }    
    return *this;
}

bitPATTERN
bitPATTERN::operator| (const bitPATTERN& pattern) const
{
    if(pattern.size() != m_size) return *this;
    bitPATTERN result(name(),id(),m_size);  

    for(int i=0;i<m_size;++i)
    {
        if ( m_digits[i] || pattern.test(i) ) 
        {
	    if(m_digits[i]) result.set(m_digits[i],i);
	    else result.set(pattern[i],i);
	}
    }    

    return result;
}

bitPATTERN& 
bitPATTERN::operator|=(const bitPATTERN& pattern)
{
    if(pattern.size() != m_size) return *this;
    for(int i=0;i<m_size;++i)
    {
        if ( !(m_digits[i] || pattern.test(i)) ) m_digits[i] = 0;
	else if(!m_digits[i]) m_digits[i] = pattern[i];
    }    
    return *this;
}


bitPATTERN
bitPATTERN::operator^ (const bitPATTERN& pattern) const
{
    if(pattern.size() != m_size) return *this;
    bitPATTERN result(name(),id(),m_size);  

    for(int i=0;i<m_size;++i)
    {
        if (  (m_digits[i] || pattern.test(i)) && 
             !(m_digits[i] && pattern.test(i))     ) 
        {
	    if(m_digits[i]) result.set(m_digits[i],i);
	    else result.set(pattern[i],i);
	}
    }    

    return result;
}

bitPATTERN& 
bitPATTERN::operator^=(const bitPATTERN& pattern)
{
    if(pattern.size() != m_size) return *this;
    for(int i=0;i<m_size;++i)
    {
        if ( !((m_digits[i] || pattern.test(i)) && 
               !(m_digits[i] && pattern.test(i))   )  ) m_digits[i] = 0;
        else if(!m_digits[i]) m_digits[i] = pattern[i];
    } 
    return *this;
}

bitPATTERN& 
bitPATTERN::operator<<(int num)
{
    if(num <= 0) return *this;
    for(int i=0;i<num;++i) shift_left();
    return *this;
}

bitPATTERN& 
bitPATTERN::operator>>(int num)
{
    if(num <= 0) return *this;
    for(int i=0;i<num;++i) shift_right();
    return *this;
}

void
bitPATTERN::shift_left()
{
    for(int i=1;i<m_size;++i) m_digits[i-1] = m_digits[i];
    m_digits[m_size-1] = 0;
}

void
bitPATTERN::shift_right()
{
    for(int i=m_size-2;i>=0;--i) m_digits[i+1] = m_digits[i];
    m_digits[0] = 0;
}

void bitPATTERN::dump (uint32_t& first_32_pattern ,
                       uint32_t& second_32_pattern ) const
{
   first_32_pattern  = 0x0;
   second_32_pattern = 0x0;
   
   if (m_size!=32 && m_size!=64) return;
   
   for (int i=0; i<32; ++i)
   {
       if (m_digits[i]) first_32_pattern |= (0x1<<i);
   }
   
   if (m_size == 64)
   {
      for (int i=32; i<64; ++i)
      {
          if (m_digits[i]) second_32_pattern |= (0x1<<(i-32));
       } 
   }
}


void bitPATTERN::Print(ostream& stream,bool detail) const
{
    stream << name() << " (" << size() << ") ";

    if(any()) stream << "*> ";
    else stream << " > ";

    for(int i=0;i<size();++i)
    {
	if(test(i)) stream << "1";
	else stream << "0";
    }


    stream << endl;

    if(detail )
      {
	  stream << "gives input to " << id() << endl;
          for(int i=0;i<size();++i)
          {
	      if(test(i)) 
              {
		  stream << "bit " << i << " raised by:" << endl;
                  (*this)[i]->Print(stream,false);
              }
          }
      }
}
