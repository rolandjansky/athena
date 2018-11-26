/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Arprnt_h
#define Arprnt_h

extern "C" { void* arprnt_address_(void); }

/**
 * @class Arprnt
 * @brief      Class definition for Arprnt, which is used
 *       to modify the ARPRNT common.
 *       */
class Arprnt {
public:
    Arprnt();
    ~Arprnt();
    
    float& 	arpar1	(int n);
    int&	iapar2	(int n);
    float& 	arint1	(int n);
    int&	iaint2	(int n);
    
    void 	init	(void);

    // return common array lengths
    inline int lenArpar1() const {return s_lenArpar1;}
    inline int lenIapar2() const {return s_lenIapar2;}
    inline int lenArint1() const {return s_lenArint1;}
    inline int lenIaint2() const {return s_lenIaint2;}

private: 

    // Lengths of the COMMONS
    static const int s_lenArpar1 = 100;
    static const int s_lenIapar2 = 50;
    static const int s_lenArint1 = 100;
    static const int s_lenIaint2 = 50;

    struct ARPRNT;
    friend struct ARPRNT;

    struct ARPRNT
    {
	float 	arpar1[s_lenArpar1];
	int	iapar2[s_lenIapar2];
	float 	arint1[s_lenArint1];
	int	iaint2[s_lenIaint2];
    };

    int m_dummy;
    float m_realdummy;
    static ARPRNT* s_arprnt;
};

// set pointer to zero at start
Arprnt::ARPRNT* Arprnt::s_arprnt =0;

inline void
Arprnt::init(void)
{ if (!s_arprnt) s_arprnt = static_cast<ARPRNT*>(arprnt_address_()); }

inline 
Arprnt::Arprnt() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Arprnt::~Arprnt() 
{}

inline float&
Arprnt::arpar1	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenArpar1()) return m_realdummy;
    return s_arprnt->arpar1[n-1];
}

inline int&
Arprnt::iapar2	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenIapar2()) return m_dummy;
    return s_arprnt->iapar2[n-1];
}

inline float&
Arprnt::arint1	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenArint1()) return m_realdummy;
    return s_arprnt->arint1[n-1];
}

inline int&
Arprnt::iaint2	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenIaint2()) return m_dummy;
    return s_arprnt->iaint2[n-1];
}

#endif

