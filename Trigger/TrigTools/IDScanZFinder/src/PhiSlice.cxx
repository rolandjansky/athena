/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "IDScanZFinder/PhiSlice.h"

#include <cmath>
#include <iostream>

//Constructor with input parameters
PhiSlice::PhiSlice( int SliceIndex, 
		    double ZBinWidth,  double InversePhiBinWidth,
		    double ZTolerance, double KTolerance, double PTolerance, 
		    double ZMinimum, 
		    double ZMaximum, 
		    int LayerMaximum, int BarrelMaximum )
{
	m_internalHistogramsAreValid = false;
	m_pTolerance = PTolerance;
	m_kTolerance = KTolerance;
	m_zTolerance = ZTolerance;
	m_sliceIndex = SliceIndex;

	m_invZBinWidth   = 1.0 / ZBinWidth;
	m_invPhiBinWidth = InversePhiBinWidth;
	m_halfZBins      = ceil( 0.5*(ZMaximum-ZMinimum) * m_invZBinWidth );
	m_zBins          = 2*m_halfZBins;
	m_zeroOffset     = floor(ZMinimum * m_invZBinWidth );

	m_layerRhos.resize( LayerMaximum );
	m_layerZs.resize( LayerMaximum );
	m_layerPhis.resize( LayerMaximum );
	m_zMinimum = ZMinimum;
	m_zMaximum = ZMaximum;
	m_barrelMaximum = BarrelMaximum;
	m_sliceStart.resize( LayerMaximum );
	m_sliceEnd.resize( LayerMaximum );

	//     std::cout << "PhiSlice::\tzBins " << m_zBins << "\t halfBins " << m_halfZBins 
	//               << "\twidth " << ZBinWidth << "\tiwidth " << m_invZBinWidth << std::endl;
}

//Destructor
PhiSlice::~PhiSlice()
{
}

//Store a space point
void PhiSlice::AddPoint( double RhoValue, double ZValue, double PhiValue,
			 long LayerIndex  )
{
  //std::cout << "PhiValue:         " << PhiValue << std::endl;
  //std::cout << "RhoValue:         " << RhoValue << std::endl;
  //std::cout << "ZValue:         " << ZValue << std::endl;
  //std::cout << "LayerIndex:         " << LayerIndex << std::endl;
  //std::cout << "m_layerRhos.size(): " << m_layerRhos.size() << std::endl;
  //std::cout << "m_layerZs.size(): " << m_layerZs.size()<< std::endl;
  //std::cout << "m_layerPhis.size(): " << m_layerPhis.size()<< std::endl;
  //std::cout << "m_layerRhos[ LayerIndex ].size(): " << m_layerRhos[ LayerIndex ].size()<< std::endl;
  //std::cout << "m_layerZs[ LayerIndex ].size(): " << m_layerZs[ LayerIndex ].size()<< std::endl;
  //std::cout << "m_layerPhis[ LayerIndex ].size(): " << m_layerPhis[ LayerIndex ].size()<< std::endl;
  m_layerRhos[ LayerIndex ].push_back( RhoValue );
  m_layerZs[ LayerIndex ].push_back( ZValue );
  m_layerPhis[ LayerIndex ].push_back( PhiValue );
}

//Make the wide layer structure
void PhiSlice::MakeWideLayers( std::vector< std::vector<double> >* AllLayerRhos, 
			       std::vector< std::vector<double> >* AllLayerZs,
			       std::vector< std::vector<double> >* AllLayerPhis, 
			       std::vector< std::vector<int> >*    AllSliceWidths,
			       int FilledLayerTotal, 
			       std::vector<long>* FilledLayerIndices )
{
	//Append the hits in this slice to the overall structure (ignore the inner layer, we don't use that for the second point)
	for ( int filledLayer = 0; filledLayer < FilledLayerTotal; filledLayer++ )
	{
		//Get the actual index of the next filled layer
		int layerIndex = ( *FilledLayerIndices )[ filledLayer ];

		//Add to the overall rho vector...
		( *AllLayerRhos )[ layerIndex ].insert(
				( *AllLayerRhos )[ layerIndex ].end(), // ...at the end...
				m_layerRhos[ layerIndex ].begin(),
				m_layerRhos[ layerIndex ].end() ); // ...the whole of our rho vector

		//Likewise for z
		( *AllLayerZs )[ layerIndex ].insert(
				( *AllLayerZs )[ layerIndex ].end(),
				m_layerZs[ layerIndex ].begin(),
				m_layerZs[ layerIndex ].end() );

		//Likewise for phi
		( *AllLayerPhis )[ layerIndex ].insert(
				( *AllLayerPhis )[ layerIndex ].end(),
				m_layerPhis[ layerIndex ].begin(),
				m_layerPhis[ layerIndex ].end() );

		//Store the cumulative number of spacepoints per slice
		int previousTotal = ( *AllSliceWidths )[ layerIndex ][ m_sliceIndex ];
		( *AllSliceWidths )[ layerIndex ][ m_sliceIndex + 1 ] = m_layerRhos[ layerIndex ].size() + previousTotal;

		//Store the start and end of this slice
		m_sliceStart[ layerIndex ] = ( *AllSliceWidths )[ layerIndex ][ m_sliceIndex ];
		m_sliceEnd[ layerIndex ] = ( *AllSliceWidths )[ layerIndex ][ m_sliceIndex + 1 ];
	}

	//Store pointers for later
	m_allLayerRhos = AllLayerRhos;
	m_allLayerZs = AllLayerZs;
	m_allLayerPhis = AllLayerPhis;
	m_allSliceWidths = AllSliceWidths;
	m_filledLayerTotal = FilledLayerTotal;
	m_filledLayerIndices = FilledLayerIndices;

	//Clean up
	m_layerRhos.clear();
	m_layerZs.clear();
	m_layerPhis.clear();
}

//Add points to internal z-axis histogram (thread safe)
void PhiSlice::MakeHistogram( std::vector< std::vector<long> >& ExtraSlices, 
			      long PhiToSubtract, 
			      int InnerLayerLimit, int TripletMode, bool ChargeAware )
{
	//Ininitalise internal histograms
	m_internalHistogramsAreValid = false;
	m_hitHistogram = std::vector<long>( m_zBins + 1, 0 );
	m_weightHistogram = std::vector<double>( m_zBins + 1, 0.0 );
	if ( ChargeAware )
	{
		m_otherChargeHitHistogram = std::vector<long>( m_zBins + 1, 0 );
		m_otherChargeWeightHistogram = std::vector<double>( m_zBins + 1, 0.0 );
	}

	//Add hits to these internal hsitograms
	GetHistogram( &m_hitHistogram, 
		      &m_weightHistogram,
		      &m_otherChargeHitHistogram, 
		      &m_otherChargeWeightHistogram,
		      ExtraSlices, 
		      PhiToSubtract, InnerLayerLimit, TripletMode, ChargeAware );

	//Mark internal flag done
	m_internalHistogramsAreValid = true;
}

//Merge internal and global z-axis histogram
void PhiSlice::AddHistogram( std::vector<long>*   HitHistogram, 
			     std::vector<double>* WeightHistogram,
			     std::vector<long>*   OtherChargeHitHistogram, 
			     std::vector<double>* OtherChargeWeightHistogram )
{
	if ( m_internalHistogramsAreValid )
	{
		//Add the internal histogram bin values to the global histograms
		for ( unsigned int binIndex = 0; binIndex < m_hitHistogram.size(); binIndex++ )
		{
			( *HitHistogram )[ binIndex ]    += m_hitHistogram[ binIndex ];
			( *WeightHistogram )[ binIndex ] += m_weightHistogram[ binIndex ];
		}
		for ( unsigned int binIndex = 0; binIndex < m_otherChargeHitHistogram.size(); binIndex++ )
		{
			( *OtherChargeHitHistogram )[ binIndex ]    += m_otherChargeHitHistogram[ binIndex ];
			( *OtherChargeWeightHistogram )[ binIndex ] += m_otherChargeWeightHistogram[ binIndex ];
		}
	}
	//	else
	//	{
	//		//cerr << "Internal phi-slice histograms have not been made - run MakeHistograms" << endl;
	//		//exit(1);
	//	}
}

//Add points to the z-axis histogram (thread unsafe if acting on global histogram)
void PhiSlice::GetHistogram( std::vector<long>*    HitHistogram, 
			     std::vector<double>*  WeightHistogram,
			     std::vector<long>*    OtherChargeHitHistogram, 
			     std::vector<double>*  OtherChargeWeightHistogram,
			     std::vector< std::vector<long> >&  ExtraSlices, 
			     long PhiToSubtract, 
			     int InnerLayerLimit, 
			     int TripletMode, 
			     bool ChargeAware,
			     std::vector< std::vector<long> >*   AllHits, 
			     std::vector< std::vector<double> >* AllWeights )
{
	//Keep the inner loop maximum sensible
	if ( InnerLayerLimit >= m_filledLayerTotal )
	{
		InnerLayerLimit = m_filledLayerTotal - 1;
	}

	//Make the pairs of spacepoints, from the inside layer out
	for ( int innerFilledLayer = 0; innerFilledLayer < InnerLayerLimit; innerFilledLayer++ )
	{
		//Get the actual index of the inner filled layer
		int innerLayerIndex = ( *m_filledLayerIndices )[ innerFilledLayer ];

		//Loop over all the points in the inner layer
		for ( unsigned int innerPointIndex = m_sliceStart[ innerLayerIndex ]; innerPointIndex < m_sliceEnd[ innerLayerIndex ]; innerPointIndex++ )
		{
			//Get the inner point
			double innerPointZ = ( *m_allLayerZs )[ innerLayerIndex ][ innerPointIndex ];
			double innerPointRho = ( *m_allLayerRhos )[ innerLayerIndex ][ innerPointIndex ];
			double innerPointPhi = ( *m_allLayerPhis )[ innerLayerIndex ][ innerPointIndex ];

			//Loop over all layers further out
			for ( int outerFilledLayer = innerFilledLayer + 1; outerFilledLayer < m_filledLayerTotal; outerFilledLayer++ )
			{
				//Get the actual index of the outer filled layer
				int outerLayerIndex = ( *m_filledLayerIndices )[ outerFilledLayer ];

				//Work out how many neighbouring slices to examine
				int neighbourNumber = ExtraSlices[ innerLayerIndex ][ outerLayerIndex ];
				if ( innerLayerIndex > m_barrelMaximum || outerLayerIndex > m_barrelMaximum )
				{
					neighbourNumber -= PhiToSubtract;
				}
				if ( neighbourNumber < 1 )
				{
					neighbourNumber = 1;
				}

				//Work out which slices that corresponds to
				int minimumSlice = m_sliceIndex - neighbourNumber;
				if ( minimumSlice < 0 )
				{
					minimumSlice = 0;
				}
				unsigned int maximumSlice = m_sliceIndex + neighbourNumber + 1;
				if ( maximumSlice >= ( *m_allSliceWidths )[ outerLayerIndex ].size() )
				{
					maximumSlice = ( *m_allSliceWidths )[ outerLayerIndex ].size() - 1;
				}

				//Work out the space point indices corresponding to those slices
				int minimumPointIndex = ( *m_allSliceWidths )[ outerLayerIndex ][ minimumSlice ];
				int maximumPointIndex = ( *m_allSliceWidths )[ outerLayerIndex ][ maximumSlice ];

				//Loop over the points in the outer layer
				for ( int outerPointIndex = minimumPointIndex; outerPointIndex < maximumPointIndex; outerPointIndex++ )
				{
					//Get the outer point
					double outerPointZ = ( *m_allLayerZs )[ outerLayerIndex ][ outerPointIndex ];
					double outerPointRho = ( *m_allLayerRhos )[ outerLayerIndex ][ outerPointIndex ];
					double outerPointPhi = ( *m_allLayerPhis )[ outerLayerIndex ][ outerPointIndex ];

					//Calculate the z axis position
					double invDeltaRho = 1.0 / ( innerPointRho - outerPointRho );
					double axisZ = ( outerPointZ * innerPointRho - innerPointZ * outerPointRho ) * invDeltaRho;

					//Cut on the maximum z value
					if ( axisZ>m_zMinimum && axisZ<m_zMaximum )
					{
						//Get the index of the outer slice for charge-awareness
						int outerSliceIndex = floor( outerPointPhi * m_invPhiBinWidth );

						//Look for triplet
						if ( TripletMode )
						{
							//Calculate some extra values
							double kValue = 2.0 * ( outerPointPhi - innerPointPhi ) * invDeltaRho;
							double pValue = ( outerPointPhi * innerPointRho - innerPointPhi * outerPointRho ) * invDeltaRho;


							//Try and identify a triplet
							int tripletsFound = FindTriplet( outerFilledLayer, outerPointIndex,
											 axisZ, kValue, pValue,
											 ExtraSlices, PhiToSubtract, ( TripletMode == 1 ), outerSliceIndex );

							//Store if at least one triplet found
							if ( tripletsFound )
							  {
								/// Calculate the corresponding histogram bin
							        /// NB: quicker to simply subtract m_zMinimum from each spacepoint at the start
							        ///     and simply add m_zMinimum to the final vertices after calculation.
                                                                ///
							        ///     Or better still, subtract m_zMinimum from each spacepoint, then scale 
                                                                ///     the coordinates by m_invZBinWidth, then you have "vertex Z positions"
                                                                ///     already in the "binning" space and you need only take the int(..)
							        ///
							        ///     This would avoids calculating the subtraction, and the multiplication 
							        ///     for every double/triplet etc - something to bare in mind for the future 
							        ///     I think 
							        int axisZIndex = rint( axisZ * m_invZBinWidth - m_zeroOffset );
							        /// int axisZIndex = rint( (axisZ-m_zMinimum) * m_invZBinWidth);
								
								//If separate slice histograms, fill neighbouring slices
								if ( AllHits && AllWeights )
								{
									FillNeighbours( innerLayerIndex, outerSliceIndex, tripletsFound, axisZIndex, axisZ,
											ExtraSlices, PhiToSubtract, ChargeAware,
											AllHits, AllWeights );
								}
								else
								{
									//Increment the main histograms
									if ( !ChargeAware || outerSliceIndex <= m_sliceIndex + 1 )
									{
										( *HitHistogram )[ axisZIndex ]    += tripletsFound;
										( *WeightHistogram )[ axisZIndex ] += axisZ * ( double )tripletsFound;
									}
									if ( ChargeAware && outerSliceIndex >= m_sliceIndex - 1 )
									{
										( *OtherChargeHitHistogram )[ axisZIndex ]    += tripletsFound;
										( *OtherChargeWeightHistogram )[ axisZIndex ] += axisZ * ( double )tripletsFound;
									}
								}
							}
						}
						else
						{
							//Calculate the corresponding histogram bin
							int axisZIndex = rint( axisZ * m_invZBinWidth - m_zeroOffset );
						        // int axisZIndex = rint( (axisZ-m_zMinimum) * m_invZBinWidth);
							
						      	//If separate slice histograms, fill neighbouring slices
							if ( AllHits && AllWeights )
							{
								FillNeighbours( innerLayerIndex, outerSliceIndex, 1, axisZIndex, axisZ,
										ExtraSlices, PhiToSubtract, ChargeAware,
										AllHits, AllWeights );
							}
							else
							{
								//Increment the histograms
								if ( !ChargeAware || outerSliceIndex <= m_sliceIndex + 1 )
								{
									( *HitHistogram )[ axisZIndex ]++;
									( *WeightHistogram )[ axisZIndex ] += axisZ;
								}
								if ( ChargeAware && outerSliceIndex >= m_sliceIndex - 1 )
								{
									( *OtherChargeHitHistogram )[ axisZIndex ]++;
									( *OtherChargeWeightHistogram )[ axisZIndex ] += axisZ;
								}
							}
						}
					}
				}
			}
		}
	}
}

//Look for a third spacepoint
int PhiSlice::FindTriplet( int OuterFilledLayer, int OuterPointIndex,
			   double CurrentZValue,
			   double CurrentKValue,
			   double CurrentPValue,
			   std::vector< std::vector<long> >& ExtraSlices, 
			   long PhiToSubtract, bool FastTriplet, int OuterSliceIndex )
{
	int tripletHits = 0;

	//Find out the real index of the outer filled layer
	int outerLayerIndex = ( *m_filledLayerIndices )[ OuterFilledLayer ];

	//Get the coordinates of the outer point
	double outerPointZ = ( *m_allLayerZs )[ outerLayerIndex ][ OuterPointIndex ];
	double outerPointRho = ( *m_allLayerRhos )[ outerLayerIndex ][ OuterPointIndex ];
	double outerPointPhi = ( *m_allLayerPhis )[ outerLayerIndex ][ OuterPointIndex ];

	//Loop over layers further out to find triplets
	for ( int tripletFilledLayer = OuterFilledLayer + 1; tripletFilledLayer < m_filledLayerTotal; tripletFilledLayer++ )
	{
		//Get the actual index of the triplet filled layer
		int tripletLayerIndex = ( *m_filledLayerIndices )[ tripletFilledLayer ];

		//Work out how many neighbouring slices to examine
		int neighbourNumber = ExtraSlices[ outerLayerIndex ][ tripletLayerIndex ];
		if ( outerLayerIndex > m_barrelMaximum || tripletLayerIndex > m_barrelMaximum )
		{
			neighbourNumber -= PhiToSubtract;
		}
		if ( neighbourNumber < 1 )
		{
			neighbourNumber = 1;
		}

		//Work out the lowest-index slice to examine
		int minimumSlice = OuterSliceIndex - neighbourNumber;
		if ( OuterSliceIndex >= m_sliceIndex )
		{
			//Keep the phi bending consistent
			minimumSlice = OuterSliceIndex - 1;
		}
		if ( minimumSlice < 0 )
		{
			minimumSlice = 0;
		}

		//Work out the highest-index slice to examine
		unsigned int maximumSlice = OuterSliceIndex + neighbourNumber + 1;
		if ( OuterSliceIndex <= m_sliceIndex )
		{
			//Keep the phi bending consistent
			maximumSlice = OuterSliceIndex + 2;
		}
		if ( maximumSlice >= ( *m_allSliceWidths )[ tripletLayerIndex ].size() )
		{
			maximumSlice = ( *m_allSliceWidths )[ tripletLayerIndex ].size() - 1;
		}

		//Work out the space point indices corresponding to those slices
		int minimumPointIndex = ( *m_allSliceWidths )[ tripletLayerIndex ][ minimumSlice ];
		int maximumPointIndex = ( *m_allSliceWidths )[ tripletLayerIndex ][ maximumSlice ];

		//Loop over the points in the triplet layer
		for ( int tripletPointIndex = minimumPointIndex; tripletPointIndex < maximumPointIndex; tripletPointIndex++ )
		{
			//Get the triplet point
			double tripletPointZ = ( *m_allLayerZs )[ tripletLayerIndex ][ tripletPointIndex ];
			double tripletPointRho = ( *m_allLayerRhos )[ tripletLayerIndex ][ tripletPointIndex ];
			double tripletPointPhi = ( *m_allLayerPhis )[ tripletLayerIndex ][ tripletPointIndex ];

			//Calculate the z value using the outer and triplet points
			double invDeltaRho = 1.0 / ( outerPointRho - tripletPointRho );
			double tripletZValue = ( tripletPointZ * outerPointRho - outerPointZ * tripletPointRho ) * invDeltaRho;
			if ( fabs( tripletZValue - CurrentZValue ) > m_zTolerance )
			{
				continue;
			}

			//Calculate the k value using the outer and triplet points
			double tripletKValue = 2.0 * ( tripletPointPhi - outerPointPhi ) * invDeltaRho;
			if ( fabs( tripletKValue - CurrentKValue ) > m_kTolerance )
			{
				continue;
			}

			//Calculate the p value using the outer and triplet points
			double tripletPValue = ( tripletPointPhi * outerPointRho - outerPointPhi * tripletPointRho ) * invDeltaRho;
			if ( fabs( tripletPValue - CurrentPValue ) < m_pTolerance )
			{
				if ( FastTriplet )
				{
					return 1;
				}
				else
				{
					tripletHits++;
				}	
			}
		}
	}

	return tripletHits;
}

//Fill neighbouring slices for histogram-per-slice mode
void PhiSlice::FillNeighbours( int InnerLayerIndex, 
			       int OuterSliceIndex, 
			       int TripletsFound, 
			       int AxisZIndex, 
			       double AxisZ,
			       std::vector< std::vector<long> >& ExtraSlices, 
			       long PhiToSubtract, 
			       bool ChargeAware,
			       std::vector< std::vector<long> >* AllHits, 
			       std::vector< std::vector<double> >* AllWeights )
{
	//Determine which neighbouring slices are affected
	int neighbourMaximum = 0;
	if ( InnerLayerIndex > 0 )
	{
		neighbourMaximum = ExtraSlices[0][ InnerLayerIndex ];

		if ( InnerLayerIndex > m_barrelMaximum )
		{
			neighbourMaximum -= PhiToSubtract;
		}
		if ( neighbourMaximum < 1 )
		{
			neighbourMaximum = 1;
		}
	}
	int neighbourMinimum = neighbourMaximum;
	if ( ChargeAware && InnerLayerIndex > 0 )
	{
		neighbourMinimum = 1;
	}

	//Restrict loop ranges to slices that exist
	int startNeighbour1 = m_sliceIndex - neighbourMinimum;
	if ( startNeighbour1 < 0 )
	{
		startNeighbour1 = 0;
	}
	int endNeighbour1 = m_sliceIndex + neighbourMaximum;
	if ( endNeighbour1 > (int)( AllHits[0].size() - 1 ) )
	{
		endNeighbour1 = AllHits[0].size() - 1;
	}
	int startNeighbour2 = m_sliceIndex - neighbourMaximum;
	if ( startNeighbour2 < 0 )
	{
		startNeighbour2 = 0;
	}
	int endNeighbour2 = m_sliceIndex + neighbourMinimum;
	if ( endNeighbour2 > (int)( AllHits[0].size() - 1 ) )
	{
		endNeighbour2 = AllHits[0].size() - 1;
	}

	//Increment the appropriate histograms
	if ( !ChargeAware || OuterSliceIndex <= m_sliceIndex + 1 )
	{
		for ( int neighbourIndex = startNeighbour1; neighbourIndex <= endNeighbour1; neighbourIndex++ )
		{
			AllHits[0][ neighbourIndex ][ AxisZIndex ] += TripletsFound;
			AllWeights[0][ neighbourIndex ][ AxisZIndex ] += AxisZ * ( double )TripletsFound;
		}
	}
	if ( ChargeAware && OuterSliceIndex >= m_sliceIndex - 1 )
	{
		for ( int neighbourIndex = startNeighbour2; neighbourIndex <= endNeighbour2; neighbourIndex++ )
		{
			AllHits[1][ neighbourIndex ][ AxisZIndex ] += TripletsFound;
			AllWeights[1][ neighbourIndex ][ AxisZIndex ] += AxisZ * ( double )TripletsFound;
		}
	}
}
