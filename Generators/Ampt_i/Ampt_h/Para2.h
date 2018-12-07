/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Para2_h
#define Para2_h

extern "C" { void* para2_address_(void); }

/**
 * @class Para2
 * @brief      Class definition for Para2, which is used
 *       to modify the PARA2 common block.
 *       */
class Para2 {
public:
    Para2();
    ~Para2();
    
    double& 	xmp(void);
    double& 	xmu(void);
    double& 	alpha(void);
    double& 	rscut2(void);
    double& 	cutof2(void);
    
    void 	init	(void);

private: 

    struct PARA2;
    friend struct PARA2;

    struct PARA2
    {
      double xmp;
      double xmu;
      double alpha;
      double rscut2;
      double cutof2;
    };

    int m_dummy;
    double m_realdummy;
    static PARA2* s_para2;
};

// set pointer to zero at start
Para2::PARA2* Para2::s_para2 = 0;

inline void
Para2::init(void)
{ if (!s_para2) s_para2 = static_cast<PARA2*>(para2_address_()); }

inline 
Para2::Para2() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Para2::~Para2() 
{}

inline double& Para2::xmp	(void) { init(); return s_para2->xmp; }
inline double& Para2::xmu	(void) { init(); return s_para2->xmu; }
inline double& Para2::alpha	(void) { init(); return s_para2->alpha; }
inline double& Para2::rscut2	(void) { init(); return s_para2->rscut2; }
inline double& Para2::cutof2	(void) { init(); return s_para2->cutof2; }

#endif

