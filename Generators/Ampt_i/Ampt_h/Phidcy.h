/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Phidcy_h
#define Phidcy_h

extern "C" { void* phidcy_address_(void); }

/**
 * @class Phidcy
 * @brief      Class definition for Phidcy, which is used
 *       to modify the PHIDCY common.
 *       */
class Phidcy {
public:
    Phidcy();
    ~Phidcy();
    
    int& 	iphidcy	(void);
    float& 	pttrig	(void);
    int& 	ntrig	(void);
    int& 	maxmiss	(void);
    int& 	ipi0dcy	(void);
    
    void 	init	(void);

private: 

    struct PHIDCY;
    friend struct PHIDCY;

    struct PHIDCY
    {
	int 	iphidcy;
	float 	pttrig;
	int 	ntrig;
	int 	maxmiss;
	int 	ipi0dcy;
    };

    int m_dummy;
    float m_realdummy;
    static PHIDCY* s_phidcy;
};

// set pointer to zero at start
Phidcy::PHIDCY* Phidcy::s_phidcy = 0;

inline void
Phidcy::init(void)
{ if (!s_phidcy) s_phidcy = static_cast<PHIDCY*>(phidcy_address_()); }

inline 
Phidcy::Phidcy() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Phidcy::~Phidcy() 
{}

inline int& Phidcy::iphidcy	(void) { init(); return s_phidcy->iphidcy; }
inline float& Phidcy::pttrig	(void) { init(); return s_phidcy->pttrig; }
inline int& Phidcy::ntrig	(void) { init(); return s_phidcy->ntrig; }
inline int& Phidcy::maxmiss	(void) { init(); return s_phidcy->maxmiss; }
inline int& Phidcy::ipi0dcy	(void) { init(); return s_phidcy->ipi0dcy; }

#endif

