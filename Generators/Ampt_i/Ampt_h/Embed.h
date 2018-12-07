/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Embed_h
#define Embed_h

extern "C" { void* embed_address_(void); }

/**
 * @class Embed
 * @brief      Class definition for Embed, which is used
 *       to modify the EMBED common.
 *       */
class Embed {
public:
    Embed();
    ~Embed();
    
    int& 	iembed	(void);
    int& 	nsembd	(void);
    float& 	pxqembd	(void);
    float& 	pyqembd	(void);
    float& 	xembd	(void);
    float& 	yembd	(void);
    float& 	psembd	(void);
    float& 	tmaxembd	(void);
    float& 	phidecomp	(void);
    
    void 	init	(void);

private: 

    struct EMBED;
    friend struct EMBED;

    struct EMBED
    {
	int 	iembed;
	int 	nsembd;
	float 	pxqembd;
	float 	pyqembd;
	float 	xembd;
	float 	yembd;
	float 	psembd;
	float 	tmaxembd;
	float 	phidecomp;
    };

    int m_dummy;
    float m_realdummy;
    static EMBED* s_embed;
};

// set pointer to zero at start
Embed::EMBED* Embed::s_embed = 0;

inline void
Embed::init(void)
{ if (!s_embed) s_embed = static_cast<EMBED*>(embed_address_()); }

inline 
Embed::Embed() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Embed::~Embed() 
{}

inline int& Embed::iembed	(void) { init(); return s_embed->iembed; }
inline int& Embed::nsembd	(void) { init(); return s_embed->nsembd; }
inline float& Embed::pxqembd	(void) { init(); return s_embed->pxqembd; }
inline float& Embed::pyqembd	(void) { init(); return s_embed->pyqembd; }
inline float& Embed::xembd	(void) { init(); return s_embed->xembd; }
inline float& Embed::yembd	(void) { init(); return s_embed->yembd; }
inline float& Embed::psembd	(void) { init(); return s_embed->psembd; }
inline float& Embed::tmaxembd	(void) { init(); return s_embed->tmaxembd; }
inline float& Embed::phidecomp	(void) { init(); return s_embed->phidecomp; }

#endif

