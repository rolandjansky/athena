// emacs: this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHI_SLICE_H
#define PHI_SLICE_H

#include <vector>

class PhiSlice
{
public:
  //Default constructor
  PhiSlice() = delete;

  //Constructor with input parameters
  PhiSlice( int SliceIndex, 
	    double ZBinWidth,  double InversePhiBinWidth,
	    double ZTolerance, double KTolerance, double PTolerance, 
	    double ZMinimum,   double ZMaximum,
	    int LayerMaximum,  int BarrelMaximum );

  //Destructor
  ~PhiSlice();

  //Store a space point
  void AddPoint( double RhoValue, double ZValue, double PhiValue, long LayerIndex );

  //Make the wide layer structure
  void MakeWideLayers( std::vector< std::vector<double> >* AllLayerRhos, std::vector< std::vector<double> >* AllLayerZs,
		       std::vector< std::vector<double> >* AllLayerPhis, std::vector< std::vector<int> >* AllSliceWidths,
		       int FilledLayerTotal, std::vector<long> * FilledLayerIndices );

  //Add points to the z-axis histograms (not thread safe if acting on global histograms)
  void GetHistogram( std::vector<long>* HitHistogram, std::vector<double>* WeightHistogram,
		     std::vector<long>* OtherChargeHitHistogram, std::vector<double>* OtherChargeWeightHistogram,
		     std::vector< std::vector<long> > & ExtraSlices, 
		     long PhiToSubtract, 
		     int InnerLayerLimit,
		     int TripletMode, 
		     bool ChargeAware,
		     std::vector< std::vector<long> >* AllHits = 0, 
		     std::vector< std::vector<double> >* AllWeights = 0 );

  //Add points to internal z-axis histogram (thread safe, incompatible with histogram-per-slice mode)
  void MakeHistogram( std::vector< std::vector<long> >& ExtraSlices, 
		      long PhiToSubtract, 
		      int InnerLayerLimit, 
		      int TripletMode, 
		      bool ChargeAware );

  //Merge internal and global z-axis histogram (not thread safe)
  void AddHistogram( std::vector<long>* HitHistogram, std::vector<double>* WeightHistogram,
		     std::vector<long>* OtherChargeHitHistogram, std::vector<double>* OtherChargeWeightHistogram );

private:
  //Look for a third spacepoint
  int FindTriplet( int OuterFilledLayer, int OuterPointIndex,
		   double CurrentZValue, double CurrentKValue, double CurrentPValue,
		   std::vector< std::vector<long> >& ExtraSlices, 
		   long PhiToSubtract, 
		   bool FastTriplet, 
		   int OuterSliceIndex );

  //Fill neighbouring slices for histogram-per-slice mode
  void FillNeighbours( int InnerLayerIndex, int OuterSliceIndex, int TripletsFound, int AxisZIndex, double AxisZ,
		       std::vector< std::vector<long> >& ExtraSlices, long PhiToSubtract, bool ChargeAware,
		       std::vector< std::vector<long> >* AllHits, 
		       std::vector< std::vector<double> >* AllWeights );

  std::vector< std::vector<double> >   m_layerRhos, m_layerZs, m_layerPhis;
  std::vector< std::vector<double> >*  m_allLayerRhos;
  std::vector< std::vector<double> >*  m_allLayerZs;
  std::vector< std::vector<double> >*  m_allLayerPhis;
  std::vector< std::vector<int> >*     m_allSliceWidths;
  std::vector<long>*                   m_filledLayerIndices;
  std::vector<long>                    m_hitHistogram, m_otherChargeHitHistogram, m_sliceStart, m_sliceEnd;
  std::vector<double>                  m_weightHistogram, m_otherChargeWeightHistogram;
  double                  m_invZBinWidth;
  double m_invPhiBinWidth; 
  double m_zTolerance;
  double m_kTolerance;
  double m_pTolerance;
  double m_zMinimum;
  double m_zMaximum;
  int    m_zBins; 
  int    m_halfZBins;
  int    m_zeroOffset;
  int    m_sliceIndex;
  int    m_filledLayerTotal;
  //int    m_layerMaximum;
  int    m_barrelMaximum;
  bool   m_internalHistogramsAreValid;
};

#endif
