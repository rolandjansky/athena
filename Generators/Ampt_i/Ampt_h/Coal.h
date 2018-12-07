/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Coal_h
#define Coal_h

extern "C" { void* coal_address_(void); }

/**
 * @class Coal
 * @brief      Class definition for Coal, which is used
 *       to modify the COAL common.
 *       */
class Coal {
public:
    Coal();
    ~Coal();
    
    double& 	dpcoal	(void);
    double& 	drcoal	(void);
    double& 	ecrit1	(void);
    
    void 	init	(void);

private: 

    struct COAL;
    friend struct COAL;

    struct COAL
    {
	double 	dpcoal;
	double 	drcoal;
	double 	ecrit1;
    };

    int m_dummy;
    double m_realdummy;
    static COAL* s_coal;
};

// set pointer to zero at start
Coal::COAL* Coal::s_coal = 0;

inline void
Coal::init(void)
{ if (!s_coal) s_coal = static_cast<COAL*>(coal_address_()); }

inline 
Coal::Coal() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Coal::~Coal() 
{}

inline double& Coal::dpcoal	(void) { init(); return s_coal->dpcoal; }
inline double& Coal::drcoal	(void) { init(); return s_coal->drcoal; }
inline double& Coal::ecrit1	(void) { init(); return s_coal->ecrit1; }

#endif

