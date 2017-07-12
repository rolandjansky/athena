/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page DCMathSegmentMaker_page DCMathSegmentMaker Package

@author Niels.van.Eldik@cern.ch 

@section DCMathSegmentMaker_DescriptionDCMathSegmentMaker General description

TODO: This page needs an update - DCMath now supports calling arbitrary segment finders

The DCMathSegmentMaker is a AlgTool to find segments in MDT chambers. 
The algorithm performes a combinatorical search using pairs of MDT hits to create segments seeds. 
For each pair of MDT hits, the four tangent lines to the drift circles are calculated. 
Other hits in the chamber are associated with the seed lines to form segment candidates. 
After this stage a 2D fit is performed to the MDT drift circles to find the segment parameters. 

If trigger hits are provided to the DCMathSegmentMaker, they will also be associated to segments.
They are not included in the fit as their errors are much larger that those of the MDT hits so they 
don't add precision to the segment in most cases. 
The DCMathSegmentMaker calculates the number of delta electrons, out-of-time hits and 
holes on the segment. The information added to the segment in form of a Muon::MdtSegmentQuality.
It can be used to identify fake segments.

A detailed write-up of the algorithm can be found at:

The ATLAS muon spectrometer, calibration and pattern recognition,
PhD Thesis, Niels van Eldik
    

@section DCMathSegmentMaker_UsageDCMathSegmentMaker Usage of the DCMathSegmentMaker

The DCMathSegmentMaker inherits from the IMuonSegmentMaker interface. 
Clients should not directly use the DCMathSegmentMaker but rather use the package via the interface.

In your header file:

@code
  #include "MuonRecToolInterfaces,IMuonSegmentMaker.h"
 
  ....
@endcode

within your class:

@code

  ToolHandle<Muon::IMuonSegmentMaker> m_mdtSegmentMaker;

@endcode


In the cxx file:

In the constructor:

  , m_mdtSegmentMaker("Muon::DCMathSegmentMaker/MySegmentMaker,this")

In initialize():


@code
 
  if( m_mdtSegmentMaker.retrieve().isSuccess() ){
    log<<MSG::INFO << "Retrieved " << m_mdtSegmentMaker << endmsg;
  }else{
    log<<MSG::FATAL<<"Could not get " << m_mdtSegmentMaker <<endmsg;
    return StatusCode::FAILURE;
  }    

@endcode

To use the segment maker, call one of the find-methods

m_mdtSegmentMaker->find( );



@section DCMathSegmentMaker_ExtrasDCMathSegmentMaker Extra Pages

*/
