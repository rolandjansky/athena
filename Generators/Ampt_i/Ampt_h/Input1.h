/*
 *   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
 *   */


#ifndef Input1_h
#define Input1_h

extern "C" { void* input1_address_(void); }

/**
 * @class Input1
 * @brief      Class definition for Input1, which is used
 *       to modify the INPUT1 common.
 *       */
class Input1 {
public:
    Input1();
    ~Input1();
    
    int& 	masspr	(void);
    int& 	massta	(void);
    int& 	iseed	(void);
    int& 	iavoid	(void);
    float&	dt	(void);
    
    void 	init	(void);

private: 

    struct INPUT1;
    friend struct INPUT1;

    struct INPUT1
    {
	int 	masspr;
	int 	massta;
	int 	iseed;
	int 	iavoid;
	float	dt;
    };

    int m_dummy;
    float m_realdummy;
    static INPUT1* s_input1;
};

// set pointer to zero at start
Input1::INPUT1* Input1::s_input1 = 0;

inline void
Input1::init(void)
{ if (!s_input1) s_input1 = static_cast<INPUT1*>(input1_address_()); }

inline 
Input1::Input1() 
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

inline 
Input1::~Input1() 
{}

inline int&
Input1::masspr	(void)
{
    init(); // check COMMON is initialized
    return s_input1->masspr;
}

inline int&
Input1::massta  (void)
{
    init(); // check COMMON is initialized
    return s_input1->massta;
}

inline int&
Input1::iseed  (void)
{
    init(); // check COMMON is initialized
    return s_input1->iseed;
}

inline int&
Input1::iavoid  (void)
{
    init(); // check COMMON is initialized
    return s_input1->iavoid;
}

inline float&
Input1::dt  (void)
{
    init(); // check COMMON is initialized
    return s_input1->dt;
}

#endif

