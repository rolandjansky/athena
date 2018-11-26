/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Rndf77_h
#define Rndf77_h

extern "C" { void* rndf77_address_(void); }

/**
 * @class Rndf77
 * @brief      Class definition for Rndf77, which is used
 *       to modify the RNDF77 common.
 *       */
class Rndf77 {
public:
    Rndf77();
    ~Rndf77();
    
    int& 	nseed	(void);
    
    void 	init	(void);

private: 

    struct RNDF77;
    friend struct RNDF77;

    struct RNDF77
    {
	int 	nseed;
    };

    int m_dummy;
    float m_realdummy;
    static RNDF77* s_rndf77;
};

// set pointer to zero at start
Rndf77::RNDF77* Rndf77::s_rndf77 = 0;

inline void
Rndf77::init(void)
{ if (!s_rndf77) s_rndf77 = static_cast<RNDF77*>(rndf77_address_()); }

inline 
Rndf77::Rndf77() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Rndf77::~Rndf77() 
{}

inline int& Rndf77::nseed(void) { init();   return s_rndf77->nseed; }



#endif

