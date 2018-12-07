/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Para8_h
#define Para8_h

extern "C" { void* para8_address_(void); }

/**
 * @class Para8
 * @brief      Class definition for Para8, which is used
 *       to modify the PARA8 common.
 *       */
class Para8 {
public:
    Para8();
    ~Para8();
    
    int& 	idpert	(void);
    int& 	npertd	(void);
    int& 	idxsec	(void);
    
    void 	init	(void);

private: 

    struct PARA8;
    friend struct PARA8;

    struct PARA8
    {
	int 	idpert;
	int 	npertd;
	int 	idxsec;
    };

    int m_dummy;
    float m_realdummy;
    static PARA8* s_para8;
};

// set pointer to zero at start
Para8::PARA8* Para8::s_para8 = 0;

inline void
Para8::init(void)
{ if (!s_para8) s_para8 = static_cast<PARA8*>(para8_address_()); }

inline 
Para8::Para8() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Para8::~Para8() 
{}

inline int& Para8::idpert	(void) { init(); return s_para8->idpert; }
inline int& Para8::npertd	(void) { init(); return s_para8->npertd; }
inline int& Para8::idxsec	(void) { init(); return s_para8->idxsec; }

#endif

