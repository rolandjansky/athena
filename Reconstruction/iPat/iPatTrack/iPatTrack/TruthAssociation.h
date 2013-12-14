/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 stores the Truth information associated with a reconstructed Track

 ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TruthAssociation.h,v 1.5 2004-07-16 22:22:14 pop Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IPATTRACK_TRUTHASSOCIATION_H
# define IPATTRACK_TRUTHASSOCIATION_H

//<<<<<< INCLUDES                                                       >>>>>>
//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class TruthAssociation
{

public:

    TruthAssociation (void);
    TruthAssociation (int barcode, int kine);

    virtual ~TruthAssociation (void);
  
    // implicit copy constructor
    // implicit assignment operator

    int			barcode (void) const;
    int			kine (void) const;
    void		print (void) const;
    void		spoiltB_LayerCluster(void);
    int			spoiltB_LayerClusters (void) const;
    void		spoiltDriftHit(void);
    int			spoiltDriftHits (void) const;
    void		spoiltOuterPixelCluster(void);
    int			spoiltOuterPixelClusters (void) const;
    int                 spoiltPixelClusters (void) const;
    void		spoiltStripCluster(void);
    int			spoiltStripClusters (void) const;
    void		uniqueB_LayerCluster(void);
    int			uniqueB_LayerClusters (void) const;
    void		uniqueDriftHit(void);
    int			uniqueDriftHits (void) const;
    void		uniqueOuterPixelCluster(void);
    int                 uniqueOuterPixelClusters (void) const;
    int			uniquePixelClusters (void) const;
    void		uniqueStripCluster(void);
    int			uniqueStripClusters (void) const;
    void		wrongB_LayerCluster(void);
    int			wrongB_LayerClusters (void) const;
    void		wrongDriftHit(void);
    int			wrongDriftHits (void) const;
    void		wrongOuterPixelCluster(void);
    int                 wrongOuterPixelClusters (void) const;
    int			wrongPixelClusters (void) const;
    void		wrongStripCluster(void);
    int			wrongStripClusters (void) const;
    
private:

    int			m_barcode;
    int			m_kine;
    int			m_spoiltB_LayerClusters;
    int			m_spoiltDriftHits;
    int			m_spoiltOuterPixelClusters;
    int			m_spoiltStripClusters;
    int			m_uniqueB_LayerClusters;
    int			m_uniqueDriftHits;
    int			m_uniqueOuterPixelClusters;
    int			m_uniqueStripClusters;
    int			m_wrongB_LayerClusters;
    int			m_wrongDriftHits;
    int			m_wrongOuterPixelClusters;
    int			m_wrongStripClusters;
};

//<<<<<< INLINE CLASS STRUCTURE INITIALIZATION                          >>>>>>

inline
TruthAssociation::TruthAssociation (void)
    :	m_barcode			(0),
	m_kine				(0),
	m_spoiltB_LayerClusters		(0),
	m_spoiltDriftHits		(0),
	m_spoiltOuterPixelClusters	(0),
	m_spoiltStripClusters		(0),
	m_uniqueB_LayerClusters		(0),
	m_uniqueDriftHits		(0),
	m_uniqueOuterPixelClusters	(0),
	m_uniqueStripClusters		(0),
	m_wrongB_LayerClusters		(0),
	m_wrongDriftHits		(0),
	m_wrongOuterPixelClusters	(0),
	m_wrongStripClusters		(0)
{}

inline
TruthAssociation::TruthAssociation (int barcode, int kine)
    :	m_barcode			(barcode),
	m_kine				(kine),
	m_spoiltB_LayerClusters		(0),
	m_spoiltDriftHits		(0),
	m_spoiltOuterPixelClusters	(0),
	m_spoiltStripClusters		(0),
	m_uniqueB_LayerClusters		(0),
	m_uniqueDriftHits		(0),
	m_uniqueOuterPixelClusters	(0),
	m_uniqueStripClusters		(0),
	m_wrongB_LayerClusters		(0),
	m_wrongDriftHits		(0),
	m_wrongOuterPixelClusters	(0),
	m_wrongStripClusters		(0)
{}

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline int
TruthAssociation::barcode (void) const
{ return m_barcode; }

inline int
TruthAssociation::kine (void) const
{ return m_kine; }

inline void
TruthAssociation::spoiltB_LayerCluster (void)
{ ++m_spoiltB_LayerClusters; }

inline int
TruthAssociation::spoiltB_LayerClusters (void) const
{ return m_spoiltB_LayerClusters; }

inline void
TruthAssociation::spoiltDriftHit (void)
{ ++m_spoiltDriftHits; }

inline int
TruthAssociation::spoiltDriftHits (void) const
{ return m_spoiltDriftHits; }

inline void
TruthAssociation::spoiltOuterPixelCluster (void)
{ ++m_spoiltOuterPixelClusters; }

inline int
TruthAssociation::spoiltOuterPixelClusters (void) const
{ return m_spoiltOuterPixelClusters; }

inline int
TruthAssociation::spoiltPixelClusters (void) const
{ return (m_spoiltB_LayerClusters+m_spoiltOuterPixelClusters); }

inline void
TruthAssociation::spoiltStripCluster (void)
{ ++m_spoiltStripClusters; }

inline int
TruthAssociation::spoiltStripClusters (void) const
{ return m_spoiltStripClusters; }

inline void
TruthAssociation::uniqueB_LayerCluster (void)
{ ++m_uniqueB_LayerClusters; }

inline int
TruthAssociation::uniqueB_LayerClusters (void) const
{ return m_uniqueB_LayerClusters; }

inline void
TruthAssociation::uniqueDriftHit (void)
{ ++m_uniqueDriftHits; }

inline int
TruthAssociation::uniqueDriftHits (void) const
{ return m_uniqueDriftHits; }

inline void
TruthAssociation::uniqueOuterPixelCluster (void)
{ ++m_uniqueOuterPixelClusters; }

inline int
TruthAssociation::uniqueOuterPixelClusters (void) const
{ return m_uniqueOuterPixelClusters; }

inline int
TruthAssociation::uniquePixelClusters (void) const
{ return (m_uniqueB_LayerClusters+m_uniqueOuterPixelClusters); }

inline void
TruthAssociation::uniqueStripCluster (void)
{ ++m_uniqueStripClusters; }

inline int
TruthAssociation::uniqueStripClusters (void) const
{ return m_uniqueStripClusters; }

inline void
TruthAssociation::wrongB_LayerCluster (void)
{ ++m_wrongB_LayerClusters; }

inline int
TruthAssociation::wrongB_LayerClusters (void) const
{ return m_wrongB_LayerClusters; }

inline void
TruthAssociation::wrongDriftHit (void)
{ ++m_wrongDriftHits; }

inline int
TruthAssociation::wrongDriftHits (void) const
{ return m_wrongDriftHits; }

inline void
TruthAssociation::wrongOuterPixelCluster (void)
{ ++m_wrongOuterPixelClusters; }

inline int
TruthAssociation::wrongOuterPixelClusters (void) const
{ return m_wrongOuterPixelClusters; }

inline int
TruthAssociation::wrongPixelClusters (void) const
{ return (m_wrongB_LayerClusters+m_wrongOuterPixelClusters); }

inline void
TruthAssociation::wrongStripCluster (void)
{ ++m_wrongStripClusters; }

inline int
TruthAssociation::wrongStripClusters (void) const
{ return m_wrongStripClusters; }

#endif // IPATTRACK_TRUTHASSOCIATION_H
