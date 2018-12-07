/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Resdcy_h
#define Resdcy_h

extern "C" { void* resdcy_address_(void); }

/**
 * @class Resdcy
 * @brief      Class definition for Resdcy, which is used
 *       to modify the RESDCY common.
 *       */
class Resdcy {
public:
    Resdcy();
    ~Resdcy();
    
    int& 	nsav	(void);
    int& 	iksdcy	(void);
    
    void 	init	(void);

private: 

    struct RESDCY;
    friend struct RESDCY;

    struct RESDCY
    {
	int 	nsav;
	int 	iksdcy;
    };

    int m_dummy;
    float m_realdummy;
    static RESDCY* s_resdcy;
};

// set pointer to zero at start
Resdcy::RESDCY* Resdcy::s_resdcy = 0;

inline void
Resdcy::init(void)
{ if (!s_resdcy) s_resdcy = static_cast<RESDCY*>(resdcy_address_()); }

inline 
Resdcy::Resdcy() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Resdcy::~Resdcy() 
{}

inline int& Resdcy::nsav	(void) { init(); return s_resdcy->nsav; }
inline int& Resdcy::iksdcy	(void) { init(); return s_resdcy->iksdcy; }

#endif

