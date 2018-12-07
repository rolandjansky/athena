/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Xyembed_h
#define Xyembed_h

extern "C" { void* xyembed_address_(void); }

/**
 * @class Xyembed
 * @brief      Class definition for Xyembed, which is used
 *       to modify the XYEMBED common.
 *       */
class Xyembed {
public:
    Xyembed();
    ~Xyembed();
    
    int& 	nxyjet	(void);
    float&	xyjet	(int i, int j);
    
    void 	init	(void);

    // return common array lengths
    inline int  lenI() const {return s_lenI;}
    inline int  lenJ() const {return s_lenJ;}

private: 

    // Lengths of the COMMONS
    static const int s_lenI     = 10001;
    static const int s_lenJ     = 2;

    struct XYEMBED;
    friend struct XYEMBED;

    struct XYEMBED
    {
        int     nxyjet;
	float	xyjet[s_lenJ][s_lenI];
    };

    int m_dummy;
    float m_realdummy;
    static XYEMBED* s_xyembed;
};

// set pointer to zero at start
Xyembed::XYEMBED* Xyembed::s_xyembed = 0;

inline void
Xyembed::init(void)
{ if (!s_xyembed) s_xyembed = static_cast<XYEMBED*>(xyembed_address_()); }

inline 
Xyembed::Xyembed() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Xyembed::~Xyembed() 
{}

inline float&
Xyembed::xyjet     (int i, int j)
{
    init(); // check COMMON is initialized
    if( i < 1 || i > lenI() ||
        j < 1 || j > lenJ() ) return m_realdummy;

    return s_xyembed->xyjet[j-1][i-1];
}

inline int& Xyembed::nxyjet       (void) { init(); return s_xyembed->nxyjet; }

#endif

