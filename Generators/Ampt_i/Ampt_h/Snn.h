/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Snn_h
#define Snn_h

extern "C" { void* snn_address_(void); }

/**
 * @class Snn
 * @brief      Class definition for Snn, which is used
 *       to modify the SNN common.
 *       */
class Snn {
public:
    Snn();
    ~Snn();
    
    float& 	efrm	(void);
    int& 	npart1	(void);
    int& 	npart2	(void);
    float& 	epsiPz	(void);
    float& 	epsiPt	(void);
    float& 	pzproj	(void);
    float& 	pztarg	(void);
    
    void 	init	(void);

private: 

    struct SNN;
    friend struct SNN;

    struct SNN
    {
	float 	efrm;
	int 	npart1;
	int 	npart2;
	float 	epsiPz;
	float 	epsiPt;
	float 	pzproj;
	float 	pztarg;
    };

    int m_dummy;
    float m_realdummy;
    static SNN* s_snn;
};

// set pointer to zero at start
Snn::SNN* Snn::s_snn = 0;

inline void
Snn::init(void)
{ if (!s_snn) s_snn = static_cast<SNN*>(snn_address_()); }

inline 
Snn::Snn() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Snn::~Snn() 
{}

inline float& Snn::efrm(void)   { init();   return s_snn->efrm; }
inline int&   Snn::npart1(void) { init();   return s_snn->npart1; }
inline int&   Snn::npart2(void) { init();   return s_snn->npart2; }
inline float& Snn::epsiPz(void) { init();   return s_snn->epsiPz; }
inline float& Snn::epsiPt(void) { init();   return s_snn->epsiPt; }
inline float& Snn::pzproj(void) { init();   return s_snn->pzproj; }
inline float& Snn::pztarg(void) { init();   return s_snn->pztarg; }



#endif

