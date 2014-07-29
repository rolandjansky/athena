/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// Description:
//      Class definition for LuDatr which is used
//      to modify LUDATR common.

#ifndef LuDatr_h
#define LuDatr_h

extern "C" { void* ludatr_address_(void); }

class LuDatr {
public:
    LuDatr();
    ~LuDatr();
    
    int&	mrlu	(int n);
    float& 	rrlu	(int n);
    
    void 	init	(void);

    // return common array lengths
    inline int lenMrlu() const {return _lenMrlu;}
    inline int lenRrlu() const {return _lenRrlu;}

private: 

    // Lengths of the COMMONS
    static const int _lenMrlu = 6;
    static const int _lenRrlu = 100;

    struct LUDATR;
    friend struct LUDATR;

    struct LUDATR
    {
	int 	mrlu[_lenMrlu];
	float	rrlu[_lenRrlu];
    };

    int _dummy;
    float _realdummy;
    static LUDATR* _ludatr;
};

// set pointer to zero at start
LuDatr::LUDATR* LuDatr::_ludatr =0;

inline void
LuDatr::init(void)
{ if (!_ludatr) _ludatr = static_cast<LUDATR*>(ludatr_address_()); }

inline 
LuDatr::LuDatr() 
    : _dummy		(-999),
      _realdummy	(-999.)
{}

inline 
LuDatr::~LuDatr() 
{}

inline int&
LuDatr::mrlu	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenMrlu()) return _dummy;
    return _ludatr->mrlu[n-1];
}

inline float&
LuDatr::rrlu	(int n)
{
    init(); // check COMMON is initialized
    if(n < 1 || n > lenRrlu()) return _realdummy;
    return _ludatr->rrlu[n-1];
}

#endif
