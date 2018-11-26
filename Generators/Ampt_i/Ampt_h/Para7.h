/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Para7_h
#define Para7_h

extern "C" { void* para7_address_(void); }

/**
 * @class Para7
 * @brief      Class definition for Para7, which is used
 *       to modify the PARA7 common.
 *       */
class Para7 {
public:
    Para7();
    ~Para7();
    
    int& 	ioscar	(void);
    int& 	nsmbbbar	(void);
    int& 	nsmmeson	(void);
    
    void 	init	(void);

private: 

    struct PARA7;
    friend struct PARA7;

    struct PARA7
    {
	int 	ioscar;
	int 	nsmbbbar;
	int 	nsmmeson;
    };

    int m_dummy;
    float m_realdummy;
    static PARA7* s_para7;
};

// set pointer to zero at start
Para7::PARA7* Para7::s_para7 = 0;

inline void
Para7::init(void)
{ if (!s_para7) s_para7 = static_cast<PARA7*>(para7_address_()); }

inline 
Para7::Para7() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Para7::~Para7() 
{}

inline int& Para7::ioscar	(void) { init(); return s_para7->ioscar; }
inline int& Para7::nsmbbbar	(void) { init(); return s_para7->nsmbbbar; }
inline int& Para7::nsmmeson	(void) { init(); return s_para7->nsmmeson; }

#endif

