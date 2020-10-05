/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGVERTEX_H
#define TRIGINDETEVENT_TRIGVERTEX_H

#include <list>

#include "CLHEP/Geometry/Point3D.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include <math.h>
#include <map>
#include <ostream>
#include "GaudiKernel/MsgStream.h"

/** @class TrigVertex
    encapsulates LVL2 vertex parameters (in the global reference frame), 
    covariance matrix, and some auxilliary information about the vertex.
  
    Note that covariance is represented by a std::vector<double> in a packed form: 

    cov(x,x) cov(x,y) cov(x,z) cov(y,y) cov(y,z) cov(z,z)   
*/

typedef std::list< const TrigInDetTrack* >   TrackInVertexList;

class TrigVertex{

 public:
  enum AlgoId{NULLID=0, SITRACKID=1, IDSCANID=2, HISTOPRMVTXSITRACKID=3, VKALSECVTXID=4, BPHYSSECID=5,
	      BSFULLSITRACKID=6, BSFULLIDSCANID=7, BSSPLITSITRACKID=8, BSSPLITIDSCANID=9, HISTOPRMVTXIDSCANID=10,
	      HISTOPRMVTXEFID=11, HISTOPRMVTXSITRACKIDTAU=12, HISTOPRMVTXEFIDTAU=13, BJETSECVTXID=14, 
              BSFULL_STRATEGY_B_ID=15, BSFULL_STRATEGY_A_ID=16, BSSPLIT_STRATEGY_B_ID=17, BSSPLIT_STRATEGY_A_ID=18,
	      HISTOPRMVTXFTKID=19, HISTOPRMVTXFTKIDTAU=20, BSFULL_STRATEGY_F_ID=21, BSSPLIT_STRATEGY_F_ID = 22,
	      HISTOPRMVTXFTKID_REFIT = 23};

  // Constructors
  TrigVertex() :  m_x(0.0), m_y(0.0), m_z(0.), m_position(0,0,0), 
    m_chiSquared(0.0), m_nDOF(0), 
    m_tracks(NULL), m_algId(NULLID),
    m_ownTracks(false)
    {
      for(int i=0;i<6;i++) m_cov[i]=0.0;
      m_mass=0.0;m_massVar=0.0;m_P=NULL;m_energyFraction=0.0;
      m_nTwoTracksSecVtx=0;
      m_decayLength = m_decayLengthSignificance = 0;
    }

  TrigVertex( double zPosition ) : m_x(0.0), m_y(0.0), m_z(zPosition),m_position(0,0,zPosition),
    m_chiSquared(0.0), m_nDOF(0), 
    m_tracks(NULL), m_algId(NULLID),
    m_ownTracks(false)
    {
      for(int i=0;i<6;i++) m_cov[i]=0.0;
      m_mass=0.0;m_massVar=0.0;m_P=NULL;m_energyFraction=0.0;
      m_nTwoTracksSecVtx=0;
      m_decayLength = m_decayLengthSignificance = 0;
    }

  TrigVertex( double zPosition, AlgoId id ) : m_x(0.0), m_y(0.0), m_z(zPosition), m_position(0,0,zPosition),
    m_chiSquared(0.0), m_nDOF(0),
    m_tracks(NULL), m_algId(id),
    m_ownTracks(false)
    {
      for(int i=0;i<6;i++) m_cov[i]=0.0;
      m_mass=0.0;m_massVar=0.0;m_P=NULL;m_energyFraction=0.0;
      m_nTwoTracksSecVtx=0;
      m_decayLength = m_decayLengthSignificance = 0;
    }

  TrigVertex( double zPosition, double cv, AlgoId id ) : m_x(0.0), m_y(0.0), m_z(zPosition), m_position(0,0,zPosition),
    m_chiSquared(0.0), m_nDOF(0),
    m_tracks(NULL), m_algId(id),
    m_ownTracks(false)
    {
      for(int i=0;i<5;i++) m_cov[i]=0.0;
      m_cov[5]=cv;
      m_mass=0.0;m_massVar=0.0;m_P=NULL;m_energyFraction=0.0;
      m_nTwoTracksSecVtx=0;
      m_decayLength = m_decayLengthSignificance = 0;
    }

  TrigVertex(double x, double y, double z, double cv[6], double chi2, int ndf, TrackInVertexList* tracks) :
    m_x(x), m_y(y), m_z(z), m_position(x,y,z), m_chiSquared(chi2), 
    m_nDOF(ndf), m_tracks(tracks), m_algId(NULLID), 
    m_ownTracks(false)
    {
      for(int i=0;i<6;i++) m_cov[i]=cv[i];
      m_mass=0.0;m_massVar=0.0;m_P=NULL;m_energyFraction=0.0;
      m_nTwoTracksSecVtx=0;
      m_decayLength = m_decayLengthSignificance = 0;
    }
    
  TrigVertex(double x, double y, double z, double cv[6], double chi2, int ndf, TrackInVertexList* tracks,
             double mass, double energyFraction, int n2trkvtx,
             double decayLength, double decayLengthSignificance,
             AlgoId algo_id) :
    m_x(x), m_y(y), m_z(z), m_mass(mass), m_massVar(0),
    m_energyFraction(energyFraction),
    m_nTwoTracksSecVtx(n2trkvtx),
    m_decayLength(decayLength),
    m_decayLengthSignificance(decayLengthSignificance),
    m_position(x,y,z), m_chiSquared(chi2), 
    m_nDOF(ndf), m_tracks(tracks), m_algId(algo_id), 
    m_ownTracks(false)
  {
    for(int i=0;i<6;i++) m_cov[i]=cv[i];
    m_P=NULL;
  }

  // Destructor
  ~TrigVertex() 
    {
      if (m_ownTracks) {
        for (TrackInVertexList::iterator i = m_tracks->begin();
             i != m_tracks->end();
             ++i)
          delete *i;
      }
      if(m_tracks!=NULL) delete m_tracks;
      if(m_P!=NULL) delete m_P;
    }

  // Methods to retrieve data members 

  void algorithmId(const AlgoId  id) { m_algId = id;}//!< sets author ID - algorithm that's created this vertex
  AlgoId algorithmId() const { return m_algId;}//!< returns author ID - algorithm that's created this vertex

  const HepGeom::Point3D<double>& position() const { return m_position; } //!< position in HepGeom::Point3D<double> form
  double chi2()          const                     { return m_chiSquared; } //!< \f$ \chi^2\f$ of the vertex fit
  int    ndof()          const                     { return m_nDOF; } //!< Number of degree-of-freedom of the vertex fit
  const double* cov()    const                     { return &m_cov[0];} //!< covariance of the vertex position, packed as follows  

  TrackInVertexList* tracks() { return m_tracks; }//!< std::list of track pointers associated with the vertex 
  const TrackInVertexList* tracks() const  { return m_tracks; }//!< std::list of track pointers associated with the vertex 
  double x() const { return m_x; } //!< x-position
  double y() const { return m_y; } //!< y-position
  double z() const { return m_z; } //!< z-position

  double mass() const   { return m_mass; }//!< vertex mass estimated after the vertex fit
  double massVariance() const { return m_massVar; }//!< variance of the vertex mass estimate 
  const TrigInDetTrackFitPar* getMotherTrack() const { return m_P;} //!< parameters of a mother particle reconstructed after the vertex fit 

  double energyFraction() const { return m_energyFraction; } //!< energy ratio E(secondary vertex)/E(jet)
  int nTwoTracksSecVtx() const { return m_nTwoTracksSecVtx; }//!<  number of 2-track vertices
  double decayLength() const { return m_decayLength; } //!< decay length to the primary vertex used to find this secondary vertex
  double decayLengthSignificance() const { return m_decayLengthSignificance;} //!< decay length divided by its error

  // Methods to set data members 

  void setMass (double m)          { m_mass = m; }
  void setMassVariance (double m)  { m_massVar = m; }
  void setMotherTrack(const TrigInDetTrackFitPar* P) { m_P=P;}

  void setEnergyFraction (double e) { m_energyFraction = e; }
  void setNTwoTrackSecVtx (int n)   { m_nTwoTracksSecVtx = n; }

  void setDecayLength (double v) {m_decayLength = v;}
  void setDecayLengthSignificance (double v) {m_decayLengthSignificance = v;}
 
 private:
  friend class TrigVertexCnv_p1;
  friend class TrigVertexCnv_p2;
  

  double m_x,m_y,m_z,m_mass,m_massVar;
  double m_cov[6];

  double m_energyFraction;//!< energy ratio E(secondary vertex)/E(jet)

  int m_nTwoTracksSecVtx;//!<  number of 2-track vertices

  double m_decayLength;
  double m_decayLengthSignificance; 
  
  HepGeom::Point3D<double> m_position;
  double     m_chiSquared;
  int        m_nDOF;
  TrackInVertexList*  m_tracks;
  AlgoId m_algId;
  const TrigInDetTrackFitPar* m_P;
  bool m_ownTracks;

};

std::string str( const TrigVertex& v );                      //<! printing helper
MsgStream& operator<< ( MsgStream& m, const TrigVertex& v ); //<! printing helper (wraps above)
bool operator== ( const TrigVertex& a, const TrigVertex& b ); 
inline bool operator!= ( const TrigVertex& a, const TrigVertex& b ) { return !(a==b); }

/** @brief comparison with feedback
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map where there are varaibel names and differences
 * between two obejcts
 * @param variableChange - map to record the differences
 * In case of collections (or objects when the size may be different) that information can also be returned in variableChange
 */
void diff( const TrigVertex& a, const TrigVertex& b, std::map<std::string, double>& variableChange ); 


CLASS_DEF( TrigVertex , 224516498 , 1 )

#endif // TRIGINDETEVENT_TRIGVERTEX_H
