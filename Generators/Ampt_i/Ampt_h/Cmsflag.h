/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Cmsflag_h
#define Cmsflag_h

extern "C" { void* cmsflag_address_(void); }

/**
 * @class Cmsflag
 * @brief      Class definition for Cmsflag, which is used
 *       to modify the CMSFLAG common block.
 *       */
class Cmsflag {
public:
    Cmsflag();
    ~Cmsflag();
    
    double& 	dshadow(void);
    int& 	ishadow(void);
    
    void 	init	(void);

private: 

    struct CMSFLAG;
    friend struct CMSFLAG;

    struct CMSFLAG
    {
      double dshadow;
      int ishadow;
    };

    int m_dummy;
    double m_realdummy;
    static CMSFLAG* s_cmsflag;
};

// set pointer to zero at start
Cmsflag::CMSFLAG* Cmsflag::s_cmsflag = 0;

inline void
Cmsflag::init(void)
{ if (!s_cmsflag) s_cmsflag = static_cast<CMSFLAG*>(cmsflag_address_()); }

inline 
Cmsflag::Cmsflag() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Cmsflag::~Cmsflag() 
{}

inline double& Cmsflag::dshadow	(void) { init(); return s_cmsflag->dshadow; }
inline int& Cmsflag::ishadow	(void) { init(); return s_cmsflag->ishadow; }

#endif

