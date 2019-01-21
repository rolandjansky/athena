// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
//
// filename: TrigZFinder.h
// 
// author: Nikos Konstantinidis <n.konstantinidis@ucl.ac.uk>
//         
//		 
// Description: NON ATHENA Internals for the ZFinder primary z vertex finding AlgTool 
// 
// -------------------------------
// ATLAS Collaboration
////////////////////////////////////////////////////////////////////////////////


#ifndef __TRIGZFINDERINTERNAL_H__
#define __TRIGZFINDERINTERNAL_H__

#include <cmath>
#include <vector>
#include <string>

#include "IDScanZFinder/ZFinderConstants.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"



class TrigZFinderInternal
{
  public: 

    struct vertex { 
      vertex( double z, double weight ) : _z(z), _weight(weight) { } 
      double _z;
      double _weight;
    };

  public:

    TrigZFinderInternal( const std::string&, const std::string& );
    virtual ~TrigZFinderInternal(){};
    //  void initializeInternal(long maxLayers=20, long lastBarrel=7); 
    void initializeInternal(long maxLayers, long lastBarrel); 

    std::vector<vertex>* findZInternal( const std::vector< TrigSiSpacePointBase >& spVec, const IRoiDescriptor& roi);

    const std::vector< std::vector<long> >*   GetnHisto() { return m_nHisto; }
    const std::vector< std::vector<double> >* GetzHisto() { return m_zHisto; }

    long  GetNMax() { return m_NMax; } 

    //  void setLayers(long maxLayers=20, long lastBarrelLayer=7) {
    void setLayers(long maxLayers, long lastBarrelLayer) {
      m_IdScan_MaxNumLayers = maxLayers;        // dphiEC depends on this value
      m_IdScan_LastBrlLayer = lastBarrelLayer;  // dphiBrl depends on this value
    } 

  protected:  // member functions

    // fills phi, rho, z, layer of spacepoints to simple vectors

    long fillVectors( const std::vector<TrigSiSpacePointBase>& spVec, const IRoiDescriptor& roi,
        std::vector<double>& phi, std::vector<double>& rho, std::vector<double>& zed, 
        std::vector<long>&   lyr, std::vector<long>&   filledLayers);

    std::string getType() const { return mType; }
    std::string getName() const { return mName; }

    int GetInternalStatus() const { return m_Status; }
    int SetInternalStatus(int s)  { m_Status = s; return m_Status; }

    double computeZV(double r1, double z1, double r2, double z2) const;
    double computeZV(double r1, double p1, double z1, double r2, double p2, double z2) const;

    void   SetReturnValue(double d) { mreturnval=d; }
    double GetReturnValue() const   { return mreturnval; }


  protected:  // data members

    /// maximum number of layers and last barrel layer 
    long   m_IdScan_MaxNumLayers; 
    long   m_IdScan_LastBrlLayer; 


    // To be read in from jobOptions by IdScanMain

    double m_invPhiSliceSize;     // the inverse size of the phi slices 
    long   m_NumPhiSlices;        // the number of phi slices, given the width of the RoI 

    double m_phiBinSize;          // the size of the phi slices 
    double m_usedphiBinSize;      // the size of the phi slices 
    double m_ROIphiWidth;         // the phi width of the ROI 
    double m_usedROIphiWidth;     // the phi width of the ROI 
    double m_minZBinSize;         // z-histo bin size: m_minZBinSize+|etaRoI|*m_zBinSizeEtaCoeff (to account for worse resolution in high eta)
    double m_zBinSizeEtaCoeff;    // z-histo bin size: m_minZBinSize+|etaRoI|*m_zBinSizeEtaCoeff (to account for worse resolution in high eta)

    long   m_numberOfPeaks;       // how many z-positions to return in findZ

    bool   m_pixOnly;             // use only Pixel space points

    std::string mType;  // type information for internal book keeping
    std::string mName;  // name information for the same

    int    m_Status;   // return status of the algorithm: 0=ok, -1=error

    bool m_chargeAware ;  // maintain separate sets of z histos for + & - tracks
    bool m_zHistoPerPhi;  // maintain one set of z histos per each phi slice

    double m_dphideta; // how, as a function of eta, the number of phi neighbours decreases
    double m_neighborMultiplier; // extra factor to manually increase the number of phi neighbors
    //  long extraPhi[IdScan_MaxNumLayers][IdScan_MaxNumLayers]; // number of phi neighbours to look at
    std::vector< std::vector<long> > extraPhi; // ( IdScan_MaxNumLayers, std::vector<long>(IdScan_MaxNumLayers) ); // number of phi neighbours to look at

    // access the ZFinder histogram from outside the findZInternal method

    std::vector < std::vector<long>   >  m_nHisto[2]; // the actual z histogram count of pairs
    std::vector < std::vector<double> >  m_zHisto[2]; // summed z position histograms

    long m_NMax; // maximum number of z histogram entries

    int m_nFirstLayers;    // When the pairs of SPs are made, the inner SP comes from up to this "filled layer"

    double m_vrtxDistCut;  // The minimum fractional distance between two output vertices
    double m_vrtxMixing ;  // If two vertices are found to be too close, "mix" the second into first
    int m_nvrtxSeparation; // The minimum number of zbins that any two output vertices should by separated by
    bool m_preferCentralZ; // Among peaks of same height, should precedence go to the one with smaller |z|

    bool m_trustSPprovider; // Should we re-extract the RoI phi range from the phis of the SPs from the SPP

    double mreturnval; // return value for algorithm

    bool   m_fullScanMode;

    int m_tripletMode;
    double m_tripletDZ;
    double m_tripletDK;
    double m_tripletDP;

    int    m_maxLayer;

    double m_minVtxSignificance;
    double m_percentile;


    /// to apply a hreshold to the found vertex candidates

    double  m_weightThreshold;

  std::vector<int> m_new2old; //transform table for new layer numbering scheme

};


#endif

