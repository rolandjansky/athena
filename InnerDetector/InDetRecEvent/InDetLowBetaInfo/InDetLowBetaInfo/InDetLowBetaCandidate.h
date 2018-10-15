/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   InDetLowBetaCandidate.h
//   Header file for class InDetLowBetaCandidate
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Christopher Marino
///////////////////////////////////////////////////////////////////

#ifndef INDETLOWBETACANDIDATE_H
#define INDETLOWBETACANDIDATE_H

//#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/CLASS_DEF.h"

class MsgStream;

namespace InDet{

  class InDetLowBetaCandidate{
   
  public:
    
    InDetLowBetaCandidate();
    
    InDetLowBetaCandidate( float TRTCorrBitsOverThreshold,
			   float TRTTrailingEdge,
			   float TRTTrailingEdgeError,
			   int   TRTNLastBits,
			   float TRTdEdx,
			   float TRTLikelihoodBeta,
			   float TRTLikelihoodError,
			   float TRTHighTbits
		      	   );

    virtual ~InDetLowBetaCandidate();
    InDetLowBetaCandidate(const InDetLowBetaCandidate& rhs) = default;
    InDetLowBetaCandidate &operator=(const InDetLowBetaCandidate &) = default;
    InDetLowBetaCandidate &operator=(InDetLowBetaCandidate &&) = default;
    
    /** Output Method for MsgStream, to be overloaded by child classes */
    virtual MsgStream& dump(MsgStream& sl) const;


    float getTRTCorrBitsOverThreshold() const;
    float getTRTTrailingEdge() const;
    float getTRTTrailingEdgeError() const;
    int   getTRTNLastBits() const;
    float getTRTdEdx() const;
    float getTRTLikelihoodBeta() const;
    float getTRTLikelihoodError() const;
    float getTRTHighTbits() const;

  protected:
    float m_TRTCorrBitsOverThreshold;
    float m_TRTTrailingEdge;
    float m_TRTTrailingEdgeError;
    int   m_TRTNLastBits;
    float m_TRTdEdx;
    float m_TRTLikelihoodBeta;
    float m_TRTLikelihoodError;
    float   m_TRTHighTbits;

  };  //end of class definitions

  MsgStream& operator << ( MsgStream& sl, const InDetLowBetaCandidate& sf);

}//end of namsepace

CLASS_DEF(InDet::InDetLowBetaCandidate,115223127  , 1 )

#endif // INDETLOWBETACANDIDATE_H









