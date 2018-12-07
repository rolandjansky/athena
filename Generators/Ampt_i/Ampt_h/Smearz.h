/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Smearz_h
#define Smearz_h

extern "C" { void* smearz_address_(void); }

/**
 * @class Smearz
 * @brief      Class definition for Smearz, which is used
 *       to modify the SMEARZ common.
 *       */
class Smearz {
public:
    Smearz();
    ~Smearz();
    
    double& 	smearp	(void);
    double& 	smearh	(void);
    
    void 	init	(void);

private: 

    struct SMEARZ;
    friend struct SMEARZ;

    struct SMEARZ
    {
	double 	smearp;
	double 	smearh;
    };

    int m_dummy;
    double m_realdummy;
    static SMEARZ* s_smearz;
};

// set pointer to zero at start
Smearz::SMEARZ* Smearz::s_smearz = 0;

inline void
Smearz::init(void)
{ if (!s_smearz) s_smearz = static_cast<SMEARZ*>(smearz_address_()); }

inline 
Smearz::Smearz() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Smearz::~Smearz() 
{}

inline double& Smearz::smearp	(void) { init(); return s_smearz->smearp; }
inline double& Smearz::smearh	(void) { init(); return s_smearz->smearh; }

#endif

