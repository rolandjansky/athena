/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for LuDatr which is used
//      to modify LUDATR common.

#ifndef LuDatr_h
#define LuDatr_h

extern "C" { void* ludatr_address_(void); }

class LuDatr {
public:
    LuDatr();
    ~LuDatr();
    
    int&	mrlu	(int n);
    float& 	rrlu	(int n);
    
    void 	init	(void);

    // return common array lengths
    inline int lenMrlu() const {return s_lenMrlu;}
    inline int lenRrlu() const {return s_lenRrlu;}

private: 

    // Lengths of the COMMONS
    static const int s_lenMrlu = 6;
    static const int s_lenRrlu = 100;

    struct LUDATR;
    friend struct LUDATR;

    struct LUDATR
    {
	int 	mrlu[s_lenMrlu];
	float	rrlu[s_lenRrlu];
    };

    int m_dummy;
    float m_realdummy;
    static LUDATR* s_ludatr;
};

// set pointer to zero at start
LuDatr::LUDATR* LuDatr::s_ludatr =0;

inline void
LuDatr::init(void)
{ if (!s_ludatr) s_ludatr = static_cast<LUDATR*>(ludatr_address_()); }

inline 
LuDatr::LuDatr() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
LuDatr::~LuDatr() 
{}

inline int&
LuDatr::mrlu	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMrlu()) return m_dummy;
    return s_ludatr->mrlu[n-1];
}

inline float&
LuDatr::rrlu	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenRrlu()) return m_realdummy;
    return s_ludatr->rrlu[n-1];
}

#endif
