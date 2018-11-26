/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Anim_h
#define Anim_h

extern "C" { void* anim_address_(void); }

/**
 * @class Anim
 * @brief      Class definition for Anim, which is used
 *       to modify the ANIM common.
 *       */
class Anim {
public:
    Anim();
    ~Anim();
    
    int& 	nevent	(void);
    int& 	isoft	(void);
    int& 	isflag	(void);
    int& 	izpc	(void);
    
    void 	init	(void);

private: 

    struct ANIM;
    friend struct ANIM;

    struct ANIM
    {
	int 	nevent;
	int 	isoft;
	int 	isflag;
	int 	izpc;
    };

    int m_dummy;
    float m_realdummy;
    static ANIM* s_anim;
};

// set pointer to zero at start
Anim::ANIM* Anim::s_anim = 0;

inline void
Anim::init(void)
{ if (!s_anim) s_anim = static_cast<ANIM*>(anim_address_()); }

inline 
Anim::Anim() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Anim::~Anim() 
{}

inline int& Anim::nevent(void) { init();   return s_anim->nevent; }
inline int& Anim::isoft(void) { init();    return s_anim->isoft; }
inline int& Anim::isflag(void) { init();   return s_anim->isflag; }
inline int& Anim::izpc(void) { init();     return s_anim->izpc; }



#endif

