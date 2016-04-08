/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Header include
#include "InDetVKalPriVxFinderTool/InDetVKalPriVxFinderTool.h"
//-------------------------------------------------
namespace InDet {

  StatusCode InDetVKalPriVxFinderTool::CutTrk(double PInvVert, double ThetaVert, double A0Vert, double Chi2, 
         long int PixelHits,long int SctHits,long int SharedHits, long int BLayHits)
  {
     double Pt = sin(ThetaVert)/fabs(PInvVert);
//- Track quality
     if(Pt               < m_CutPt) 			return StatusCode::FAILURE;
     if(Chi2 	         > m_CutChi2) 			return StatusCode::FAILURE;
     if(fabs(A0Vert)     > m_CutA0) 			return StatusCode::FAILURE;


     if(PixelHits	    < m_CutPixelHits) 		return StatusCode::FAILURE;
     if(SctHits		    < m_CutSctHits) 		return StatusCode::FAILURE;
     if((PixelHits+SctHits) < m_CutSiHits) 		return StatusCode::FAILURE;
     if(BLayHits	    < m_CutBLayHits) 		return StatusCode::FAILURE;
     if(SharedHits	    > m_CutSharedHits) 		return StatusCode::FAILURE;

     return StatusCode::SUCCESS;
  }
 
}

