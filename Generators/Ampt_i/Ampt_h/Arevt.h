/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Arevt_h
#define Arevt_h

extern "C" { void* arevt_address_(void); }

/**
 * @class Arevt
 * @brief      Class definition for Arevt, which is used
 *       to modify the AREVT common.
 *       */
class Arevt {
public:
    Arevt();
    ~Arevt();
    
    int& 	iaevt	(void);
    int& 	iarun	(void);
    int& 	miss	(void);
    
    void 	init	(void);

private: 

    struct AREVT;
    friend struct AREVT;

    struct AREVT
    {
	int 	iaevt;
	int 	iarun;
	int 	miss;
    };

    int m_dummy;
    float m_realdummy;
    static AREVT* s_arevt;
};

// set pointer to zero at start
Arevt::AREVT* Arevt::s_arevt = 0;

inline void
Arevt::init(void)
{ if (!s_arevt) s_arevt = static_cast<AREVT*>(arevt_address_()); }

inline 
Arevt::Arevt() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Arevt::~Arevt() 
{}

inline int& Arevt::iaevt(void) { init();  return s_arevt->iaevt; }
inline int& Arevt::iarun(void) { init();  return s_arevt->iarun; }
inline int& Arevt::miss(void) { init();   return s_arevt->miss; }



#endif

