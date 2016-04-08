/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for HyFlow, which is used
//      to modify HYFLOW common.

#ifndef HyFlow_h
#define HyFlow_h

extern "C" { void* hyflow_address_(void); }

class HyFlow {
public:
    HyFlow();
    ~HyFlow();

    double&    	ytfl	(void);
    double&    	ylfl	(void);
    double&    	tf	(void);
    double&    	fpart	(void);

    void	init	(void);

private: 

    struct HYFLOW;
    friend struct HYFLOW;

    struct HYFLOW
    {
      double  	ytfl;
      double  	ylfl;
      double  	tf;
      double  	fpart;
    };

    static HYFLOW* s_hyflow;
};

// set pointer to zero at start
HyFlow::HYFLOW* HyFlow::s_hyflow =0;

inline void
HyFlow::init(void)
{ if (!s_hyflow) s_hyflow = static_cast<HYFLOW*>(hyflow_address_()); }

// Constructor
inline
HyFlow::HyFlow()
{}

// Destructor
inline
HyFlow::~HyFlow()
{}

inline double&
HyFlow::ytfl	(void)
{
    init();
    return s_hyflow->ytfl;
}

inline double&
HyFlow::ylfl	(void)
{
    init();
    return s_hyflow->ylfl;
}

inline double&
HyFlow::tf	(void)
{
    init();
    return s_hyflow->tf;
}

inline double&
HyFlow::fpart	(void)
{
    init();
    return s_hyflow->fpart;
}

#endif
