/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Input2_h
#define Input2_h

extern "C" { void* input2_address_(void); }

/**
 * @class Input2
 * @brief      Class definition for Input2, which is used
 *       to modify the INPUT2 common.
 *       */
class Input2 {
public:
    Input2();
    ~Input2();
    
    int& 	ilab	(void);
    int& 	manyb	(void);
    int& 	ntmax	(void);
    int& 	icoll	(void);
    int& 	insys	(void);
    int& 	ipot	(void);
    int& 	mode	(void);
    int& 	imomen	(void);
    int& 	nfreq	(void);
    int& 	icflow	(void);
    int& 	icrho	(void);
    int& 	icou	(void);
    int& 	kpoten	(void);
    int& 	kmul	(void);
    
    void 	init	(void);

private: 

    struct INPUT2;
    friend struct INPUT2;

    struct INPUT2
    {
	int 	ilab;
	int 	manyb;
	int 	ntmax;
	int 	icoll;
	int 	insys;
	int 	ipot;
	int 	mode;
	int 	imomen;
	int 	nfreq;
	int 	icflow;
	int 	icrho;
	int 	icou;
	int 	kpoten;
	int 	kmul;
    };

    int m_dummy;
    float m_realdummy;
    static INPUT2* s_input2;
};

// set pointer to zero at start
Input2::INPUT2* Input2::s_input2 = 0;

inline void
Input2::init(void)
{ if (!s_input2) s_input2 = static_cast<INPUT2*>(input2_address_()); }

inline 
Input2::Input2() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Input2::~Input2() 
{}

inline int& Input2::ilab(void) { init();   return s_input2->ilab; }
inline int& Input2::manyb(void) { init();  return s_input2->manyb; }
inline int& Input2::ntmax(void) { init();  return s_input2->ntmax; }
inline int& Input2::icoll(void) { init();  return s_input2->icoll; }
inline int& Input2::insys(void) { init();  return s_input2->insys; }
inline int& Input2::ipot(void) { init();   return s_input2->ipot; }
inline int& Input2::mode(void) { init();   return s_input2->mode; }
inline int& Input2::imomen(void) { init(); return s_input2->imomen; }
inline int& Input2::nfreq(void) { init();  return s_input2->nfreq; }
inline int& Input2::icflow(void) { init(); return s_input2->icflow; }
inline int& Input2::icrho(void) { init();  return s_input2->icrho; }
inline int& Input2::icou(void) { init();   return s_input2->icou; }
inline int& Input2::kpoten(void) { init(); return s_input2->kpoten; }
inline int& Input2::kmul(void) { init();   return s_input2->kmul; }



#endif

