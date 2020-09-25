// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: HitFilter.h
// 
// author: Nikos Konstantinidis
//         nk@hep.ucl.ac.uk
//		 
// 
// Description: - main pattern recognition algorithm of IdScan 
// 
// date: 28/07/2003
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#if ! defined( IDSCAN_IHITFILTER_H )
#define IDSCAN_IHITFILTER_H

#include <vector>

#include "HF_Constants.h"

#include "FilterBin.h"
#include "Group.h"
#include "IdScanSpPoint.h"

struct groupCleanerParams;
struct GCP_loose;
struct GCP_tight;


////////////////////////////////////////////////////////////////////////////////
////    HitFilter
////////////////////////////////////////////////////////////////////////////////

class iHitFilter
{
public:
  
  iHitFilter() 
    : m_fullScan(false), m_binMap(NULL), m_loose(NULL), m_tight(NULL), m_internalSPs(NULL), m_zVertex(0),
      m_IdScan_MaxNumLayers(19), m_IdScan_MaxEta(3.0) { }
 
  ~iHitFilter();
  
  int initialise(long maxlayers, long maxBarrelLayer, double maxeta=3.0);

  double  getLayerThreshold() const { return m_layerThreshold; }

  void setLayerThresholdTmp(double d) { m_layerThresholdTmp = d; }

protected:                      // member functions

  //  GroupList execute( hitVec& );
  GroupList execute();
  
  long makeHashKey( double phi, double eta ) const;
  long makeHashKey( double* r,  double* f, 
		    double k21, double rr1, 
		    double fr1, groupCleanerParams* params) const;
  void neighborKeys( long key, long* np ) const;

  void clusterBins( GroupList& gList );
  void joinGroup( long key, FilterBin& bin, GroupList::iterator giter, GroupList& glist );

  void groupCleaner( GroupList::iterator& gItr, GroupList& cleangroups ) ;
  void fillMapGC1( GroupList::iterator& inputItr, groupCleanerParams* params ) const;
  void fillMapGC2( GroupList::iterator& inputItr, groupCleanerParams* params ) const;

  double dzdr(double* r, double* z)  const;
  double ktemp(double* r, double* f) const;
  void fitGroup(Group&); 
  void fitGroupWithoutHitRemoval(Group&);
  
  void cleanInternalSPs();

  void  makeIDScanSPs( std::vector<const ExternalSpacePoint* >, double, double, double, double, double );

protected:                      // data members
  
  // to be read in from jobOptions
  
  double m_ROIphiHalfWidth;
  double m_ROIetaHalfWidth;

  double m_useROIphiHalfWidth;

  bool   m_fullScan;
  long   m_maxPhiBins;
  bool   m_HF;

  double m_phiBinSize;
  double m_etaBinSize;
  double m_layerThreshold;    // This is the value that you set once.
  double m_layerThresholdTmp; // This value is set for each RoI, depending on number of missing layers
  bool   m_enhanceLayer0;
  bool   m_cloneRemoval;
  bool   m_usePtSign;
  long   m_cloneRemovalCut;
  double m_invPhiBinSize;
  double m_invEtaBinSize;

  double m_looseTight;
  double m_pTcutInMeV;
  double m_dPhidRCut;

  double m_loose_deltadzdrCut;
  double m_loose_etaCutInner;
  double m_loose_etaCutOuter;
  double m_loose_d0Cut;
  double m_loose_dPhidRBinSize;
  long   m_loose_phi0Bins;

  double m_tight_deltadzdrCut;
  double m_tight_etaCutInner;
  double m_tight_etaCutOuter;
  double m_tight_d0Cut;
  double m_tight_dPhidRBinSize;
  long   m_tight_phi0Bins;

  FilterMap* m_binMap;

  GCP_loose* m_loose;         // loose set of parameters for group cleaner (look at GC_Constants.h)
  GCP_tight* m_tight;         // tight set of parameters for group cleaner

  bool       m_printDiagnosticMessages;

  hitVec*    m_internalSPs;

  double     m_zVertex;  // roi z vertex 

  long m_binCounter;

  double m_maxBeamSpotShift;


  /// locally set parameters for number of layers (and max eta ??)
  long   m_IdScan_MaxNumLayers;
  double m_IdScan_MaxEta;

  long m_maxBarrelLayer = 6;
};


class TrackCandidateClass
{
 public:
  TrackCandidateClass(class Group*);
  ~TrackCandidateClass(){};
  Group* m_pGroup;
  int m_nHitShift;
  int m_nSharedHits;
  int m_nSharedTotal;
  double m_fom;
  double m_pt;
};

class TrackFilterClass   
{
 public:
  void addNewTrackCandidate(class Group*);
  void removeClones(int);
  TrackFilterClass(bool);
  ~TrackFilterClass();
 private:
  std::vector<TrackCandidateClass*> m_vpTCC;
  bool m_usePtSign;
};


inline long iHitFilter::makeHashKey( double phi, double eta ) const
{
//  makes the hash key for the (eta,phi) mapping in the hit filter...
//
  long etabin = (long) ( (eta+m_IdScan_MaxEta)*m_invEtaBinSize );
  long phibin = (long) ( phi*m_invPhiBinSize); 

  return (HashKeyCoeff*etabin + phibin);

}


inline void iHitFilter::neighborKeys( long key, long* np ) const
{
//   Finds the hash keys for the 8 neighbouring bins. Look at HitFilter::makeHashKey 
//   to understand the decoding...
//
  long bin1 = key/HashKeyCoeff;  // eta bin for hitFilter, phi0 bin for groupCleaner
  long bin2 = key%HashKeyCoeff;  // phi bin for hitFilter, dphi/drho bin for groupCleaner

  long bin1m1 = bin1 - 1;
  long bin1p1 = bin1 + 1;

  long bin2m1 = bin2 - 1;
  long bin2p1 = bin2 + 1;

  if ( m_fullScan && m_HF ) {
    if ( bin2m1 < 0 ) bin2m1 += m_maxPhiBins;
    if ( bin2p1 > m_maxPhiBins-1) bin2p1 -= m_maxPhiBins;
  }

  np[0] = HashKeyCoeff*bin1m1 + bin2m1;
  np[1] = HashKeyCoeff*bin1m1 + bin2;
  np[2] = HashKeyCoeff*bin1m1 + bin2p1;
  np[3] = HashKeyCoeff*bin1 + bin2m1;
  np[4] = HashKeyCoeff*bin1 + bin2p1;
  np[5] = HashKeyCoeff*bin1p1 + bin2m1;
  np[6] = HashKeyCoeff*bin1p1 + bin2;
  np[7] = HashKeyCoeff*bin1p1 + bin2p1;

  /*
  for (long i=-1; i<2; ++i) {
    for (long j=-1; j<2; ++j) {
      if ( i||j ) {
	*np++ = HashKeyCoeff*(bin1+i) + (bin2+j);
      }
    }
  }
  */

}


inline double iHitFilter::dzdr(double* r, double* z) const
{
  return (z[1]-z[0])/(r[1]-r[0]);
}


inline double iHitFilter::ktemp(double* r, double* f) const
{
  return (f[1]*r[1]-f[0]*r[0])/(r[1]-r[0]);
}


#endif
