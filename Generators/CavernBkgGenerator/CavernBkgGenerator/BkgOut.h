/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BkgOut_h
#define BkgOut_h

extern "C" { void* bkgout_address_(void); }

class BkgOut {
public:
  BkgOut();
  ~BkgOut();
    
  int&    	ievent	(void);
  int&    	npv	(void);
  float&  	vx	(int i);
  float&  	vy	(int i);
  float&  	vz	(int i);
  float&  	px	(int i);
  float&  	py	(int i);
  float&  	pz	(int i);
  float&  	ptof	(int i);
  int&    	irp	(int i);
  int&    	ityp	(int i);

  void	init	(void);

  // return common array lengths
  inline int	lenI() const {return _lenI;}

private: 

  // Lengths of array in HiMain2 common
  static const int _lenI	= 2000;

  struct BKGOUT;
  friend struct BKGOUT;

  struct BKGOUT {
    int    	ievent;
    int    	npv;
    float  	vx	[_lenI];
    float  	vy	[_lenI];
    float  	vz	[_lenI];
    float  	px	[_lenI];
    float  	py	[_lenI];
    float  	pz	[_lenI];
    float 	ptof	[_lenI];
    int   	irp	[_lenI];
    int   	ityp	[_lenI];
  };

  int  _dummy;
  float  _realdummy;

  static BKGOUT* _bkgout;
};

// set pointer to zero at start
BkgOut::BKGOUT* BkgOut::_bkgout =0;

inline void
BkgOut::init(void)
{ if (!_bkgout) _bkgout = static_cast<BKGOUT*>(bkgout_address_()); }

// Constructor
inline
BkgOut::BkgOut()
    : _dummy		(-999),
      _realdummy	(-999.)
{}

// Destructor
inline
BkgOut::~BkgOut()
{}

inline int&
BkgOut::ievent   (void) {
  init();
  return _bkgout->ievent;
}

inline int&
BkgOut::npv   (void) {
  init();
  return _bkgout->npv;
}

inline float&
BkgOut::vx	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _realdummy;
  return _bkgout->vx[i-1];
}

inline float&
BkgOut::vy	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _realdummy;
  return _bkgout->vy[i-1];
}

inline float&
BkgOut::vz	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _realdummy;
  return _bkgout->vz[i-1];
}

inline float&
BkgOut::px	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _realdummy;
  return _bkgout->px[i-1];
}

inline float&
BkgOut::py	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _realdummy;
  return _bkgout->py[i-1];
}

inline float&
BkgOut::pz	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _realdummy;
  return _bkgout->pz[i-1];
}

inline float&
BkgOut::ptof	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _realdummy;
  return _bkgout->ptof[i-1];
}

inline int&
BkgOut::irp	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _dummy;
  return _bkgout->irp[i-1];
}

inline int&
BkgOut::ityp	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return _dummy;
  return _bkgout->ityp[i-1];
}

#endif
