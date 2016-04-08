/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef HiParnt_h
#define HiParnt_h

extern "C" { void* hiparnt_address_(void); }

/**
@class HiParnt
@brief      Class definition for HiParnt, which is used
      to modify the Hijing HIPARNT common.
*/
class HiParnt {
public:
    HiParnt();
    ~HiParnt();
    
    float& 	hipr1	(int n);
    int&	ihpr2	(int n);
    float& 	hint1	(int n);
    int&	ihnt2	(int n);
    
    void 	init	(void);

    // return common array lengths
    inline int lenHipr1() const {return s_lenHipr1;}
    inline int lenIhpr2() const {return s_lenIhpr2;}
    inline int lenHint1() const {return s_lenHint1;}
    inline int lenIhnt2() const {return s_lenIhnt2;}

private: 

    // Lengths of the COMMONS
    static const int s_lenHipr1 = 100;
    static const int s_lenIhpr2 = 50;
    static const int s_lenHint1 = 100;
    static const int s_lenIhnt2 = 50;

    struct HIPARNT;
    friend struct HIPARNT;

    struct HIPARNT
    {
	float 	hipr1[s_lenHipr1];
	int	ihpr2[s_lenIhpr2];
	float 	hint1[s_lenHint1];
	int	ihnt2[s_lenIhnt2];
    };

    int m_dummy;
    float m_realdummy;
    static HIPARNT* s_hiparnt;
};

// set pointer to zero at start
HiParnt::HIPARNT* HiParnt::s_hiparnt =0;

inline void
HiParnt::init(void)
{ if (!s_hiparnt) s_hiparnt = static_cast<HIPARNT*>(hiparnt_address_()); }

inline 
HiParnt::HiParnt() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
HiParnt::~HiParnt() 
{}

inline float&
HiParnt::hipr1	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenHipr1()) return m_realdummy;
    return s_hiparnt->hipr1[n-1];
}

inline int&
HiParnt::ihpr2	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenIhpr2()) return m_dummy;
    return s_hiparnt->ihpr2[n-1];
}

inline float&
HiParnt::hint1	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenHint1()) return m_realdummy;
    return s_hiparnt->hint1[n-1];
}

// access ihnt2 in common
inline int&
HiParnt::ihnt2	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenIhnt2()) return m_dummy;
    return s_hiparnt->ihnt2[n-1];
}

#endif
