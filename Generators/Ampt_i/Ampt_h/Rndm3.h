/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Rndm3_h
#define Rndm3_h

extern "C" { void* rndm3_address_(void); }

/**
 * @class Rndm3
 * @brief      Class definition for Rndm3, which is used
 *       to modify the RNDM3 common.
 *       */
class Rndm3 {
public:
    Rndm3();
    ~Rndm3();
    
    int& 	iseedp	(void);
    
    void 	init	(void);

private: 

    struct RNDM3;
    friend struct RNDM3;

    struct RNDM3
    {
	int 	iseedp;
    };

    int m_dummy;
    float m_realdummy;
    static RNDM3* s_rndm3;
};

// set pointer to zero at start
Rndm3::RNDM3* Rndm3::s_rndm3 = 0;

inline void
Rndm3::init(void)
{ if (!s_rndm3) s_rndm3 = static_cast<RNDM3*>(rndm3_address_()); }

inline 
Rndm3::Rndm3() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Rndm3::~Rndm3() 
{}

inline int& Rndm3::iseedp	(void) { init(); return s_rndm3->iseedp; }

#endif

