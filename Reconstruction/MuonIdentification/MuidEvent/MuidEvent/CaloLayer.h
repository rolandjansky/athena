/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUIDEVENT_CALOLAYER_H
# define MUIDEVENT_CALOLAYER_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace Trk
{
    class Surface;
}

namespace Rec
{
    
class CaloLayer
{

public:
    CaloLayer(const Trk::Surface* surface,
	      int geom,
	      double r,
	      double z,
	      double radLen,
	      bool isBarrel);
    ~CaloLayer();

    int				geom() const;
    bool			isBarrel() const;
    void			print() const;
    const Trk::Surface*		surface (void) const;
    double			R() const;
    double			Z() const;
    double			X0() const;
    
private:
    int				m_igeom;
    bool			m_isBarrel;
    double			m_R;
    double			m_Z;
    const Trk::Surface*		m_surface;
    double			m_X0;
};

inline int 
CaloLayer::geom() const
{return m_igeom;}

inline bool
CaloLayer::isBarrel() const
{return m_isBarrel;}

inline double
CaloLayer::R() const
{return m_R;}

inline const Trk::Surface*
CaloLayer::surface (void) const
{ return m_surface; }

inline double
CaloLayer::Z() const
{return m_Z;}

inline double
CaloLayer::X0() const
{return m_X0;}
 
}	// end of namespace
 
#endif
