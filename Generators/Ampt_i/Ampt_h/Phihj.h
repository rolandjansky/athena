/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Phihj_h
#define Phihj_h

extern "C" { void* phihj_address_(void); }

/**
 * @class Phihj
 * @brief      Class definition for Phihj, which is used
 *       to modify the PHIHJ common.
 *       */
class Phihj {
public:
    Phihj();
    ~Phihj();
    
    int& 	iphirp	(void);
    float& 	phirp	(void);
    
    void 	init	(void);

private: 

    struct PHIHJ;
    friend struct PHIHJ;

    struct PHIHJ
    {
	int 	iphirp;
	float 	phirp;
    };

    int m_dummy;
    float m_realdummy;
    static PHIHJ* s_phihj;
};

// set pointer to zero at start
Phihj::PHIHJ* Phihj::s_phihj = 0;

inline void
Phihj::init(void)
{ if (!s_phihj) s_phihj = static_cast<PHIHJ*>(phihj_address_()); }

inline 
Phihj::Phihj() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Phihj::~Phihj() 
{}

inline int& Phihj::iphirp	(void) { init(); return s_phihj->iphirp; }
inline float& Phihj::phirp	(void) { init(); return s_phihj->phirp; }

#endif

