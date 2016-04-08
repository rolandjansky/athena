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
  inline int	lenI() const {return s_lenI;}

private: 

  // Lengths of array in HiMain2 common
  static const int s_lenI	= 2000;

  struct BKGOUT;
  friend struct BKGOUT;

  struct BKGOUT {
    int    	ievent;
    int    	npv;
    float  	vx	[s_lenI];
    float  	vy	[s_lenI];
    float  	vz	[s_lenI];
    float  	px	[s_lenI];
    float  	py	[s_lenI];
    float  	pz	[s_lenI];
    float 	ptof	[s_lenI];
    int   	irp	[s_lenI];
    int   	ityp	[s_lenI];
  };

  int  m_dummy;
  float  m_realdummy;

  static BKGOUT* s_bkgout;
};

// set pointer to zero at start
BkgOut::BKGOUT* BkgOut::s_bkgout =0;

inline void
BkgOut::init(void)
{ if (!s_bkgout) s_bkgout = static_cast<BKGOUT*>(bkgout_address_()); }

// Constructor
inline
BkgOut::BkgOut()
    : m_dummy		(-999),
      m_realdummy	(-999.)
{}

// Destructor
inline
BkgOut::~BkgOut()
{}

inline int&
BkgOut::ievent   (void) {
  init();
  return s_bkgout->ievent;
}

inline int&
BkgOut::npv   (void) {
  init();
  return s_bkgout->npv;
}

inline float&
BkgOut::vx	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_realdummy;
  return s_bkgout->vx[i-1];
}

inline float&
BkgOut::vy	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_realdummy;
  return s_bkgout->vy[i-1];
}

inline float&
BkgOut::vz	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_realdummy;
  return s_bkgout->vz[i-1];
}

inline float&
BkgOut::px	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_realdummy;
  return s_bkgout->px[i-1];
}

inline float&
BkgOut::py	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_realdummy;
  return s_bkgout->py[i-1];
}

inline float&
BkgOut::pz	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_realdummy;
  return s_bkgout->pz[i-1];
}

inline float&
BkgOut::ptof	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_realdummy;
  return s_bkgout->ptof[i-1];
}

inline int&
BkgOut::irp	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_dummy;
  return s_bkgout->irp[i-1];
}

inline int&
BkgOut::ityp	(int i) {
  init(); // check COMMON is initialized
  if( i < 1 || i > lenI()  ) return m_dummy;
  return s_bkgout->ityp[i-1];
}

#endif
