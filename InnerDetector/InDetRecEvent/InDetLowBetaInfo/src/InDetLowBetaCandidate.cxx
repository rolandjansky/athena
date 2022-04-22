/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/


//////////////////////////////////////////////////////////////////
// InDetLowBetaCandidate.cxx
//   Implementation file for class InDetLowBetaCandidate
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Christopher Marino 
///////////////////////////////////////////////////////////////////

#include "InDetLowBetaInfo/InDetLowBetaCandidate.h"
#include "GaudiKernel/MsgStream.h"

namespace InDet {

  InDetLowBetaCandidate::InDetLowBetaCandidate():m_TRTCorrBitsOverThreshold(), m_TRTTrailingEdge(), m_TRTTrailingEdgeError(), m_TRTNLastBits(), m_TRTdEdx(), m_TRTLikelihoodBeta(), m_TRTLikelihoodError(), m_TRTHighTbits() {}

  InDetLowBetaCandidate::InDetLowBetaCandidate(float TRTCorrBitsOverThreshold,
					       float TRTTrailingEdge,
					       float TRTTrailingEdgeError,
					       int   TRTNLastBits,
					       float TRTdEdx,
					       float TRTLikelihoodBeta,
					       float TRTLikelihoodError,
					       float   TRTHighTbits):m_TRTCorrBitsOverThreshold(TRTCorrBitsOverThreshold), m_TRTTrailingEdge( TRTTrailingEdge), m_TRTTrailingEdgeError( TRTTrailingEdgeError), m_TRTNLastBits(TRTNLastBits), m_TRTdEdx(TRTdEdx), m_TRTLikelihoodBeta(TRTLikelihoodBeta), m_TRTLikelihoodError(TRTLikelihoodError), m_TRTHighTbits( TRTHighTbits ) {}
   
/**
  InDetLowBetaCandidate::InDetLowBetaCandidate(const InDetLowBetaCandidate& rhs) :
     m_TRTCorrBitsOverThreshold(rhs.m_TRTCorrBitsOverThreshold),
     m_TRTTrailingEdge(rhs.m_TRTTrailingEdge),
     m_TRTTrailingEdgeError(rhs.m_TRTTrailingEdgeError),
     m_TRTNLastBits(rhs.m_TRTNLastBits),
     m_TRTdEdx(rhs.m_TRTdEdx),
     m_TRTLikelihoodBeta(rhs.m_TRTLikelihoodBeta),
     m_TRTLikelihoodError(rhs.m_TRTLikelihoodError),
     m_TRTHighTbits(rhs.m_TRTHighTbits){}
  
  InDetLowBetaCandidate &InDetLowBetaCandidate::operator= (const InDetLowBetaCandidate& rhs)
  {
    if (this!=&rhs)
    {
      m_TRTCorrBitsOverThreshold = rhs.m_TRTCorrBitsOverThreshold;
      m_TRTTrailingEdge = rhs.m_TRTTrailingEdge;
      m_TRTTrailingEdgeError = rhs.m_TRTTrailingEdgeError;
      m_TRTNLastBits = rhs.m_TRTNLastBits;
      m_TRTdEdx = rhs.m_TRTdEdx;
      m_TRTLikelihoodBeta = rhs.m_TRTLikelihoodBeta;
      m_TRTLikelihoodError = rhs.m_TRTLikelihoodError;
      m_TRTHighTbits = rhs.m_TRTHighTbits;
    }
    return *this;
  }
**/

  InDetLowBetaCandidate::~InDetLowBetaCandidate() = default;

  MsgStream& InDetLowBetaCandidate::dump(MsgStream& sl) const {
	    sl << "Printing InDet::InDetLowBetaCandidate: " << endmsg;
	    sl << "TRTCorrBitsOverThreshold: " << m_TRTCorrBitsOverThreshold << endmsg;
	    sl << "TRTTrailingEdge: " << m_TRTTrailingEdge << endmsg;
	    sl << "TRTTrailingEdgeError: " << m_TRTTrailingEdgeError << endmsg;
	    sl << "TRTNLastBits: " << m_TRTNLastBits << endmsg;
	    sl << "TRTdEdx: " << m_TRTdEdx << endmsg;
	    sl << "TRTLikelihoodBeta: " << m_TRTLikelihoodBeta << endmsg;
	    sl << "TRTLikelihoodError: " << m_TRTLikelihoodError << endmsg;
	    sl << "TRTHighTbits: " << m_TRTHighTbits << endmsg;
	    return sl;
	  }


  MsgStream& operator << ( MsgStream& sl, const InDetLowBetaCandidate& sf)
	  { return sf.dump(sl); }
	

	    
  float InDetLowBetaCandidate::getTRTCorrBitsOverThreshold() const
          {	    
	    return  m_TRTCorrBitsOverThreshold;
	  }

  float InDetLowBetaCandidate::getTRTTrailingEdge() const
	  {	    
	    return  m_TRTTrailingEdge;	  
	  }
  
  float InDetLowBetaCandidate::getTRTTrailingEdgeError() const
         {	    
	    return  m_TRTTrailingEdgeError;	  
	  }

  int InDetLowBetaCandidate::getTRTNLastBits() const
	  { 
	    return  m_TRTNLastBits;	  
	  }
  float InDetLowBetaCandidate::getTRTdEdx() const
	  {	    
	    return  m_TRTdEdx;	  
	  }
  float InDetLowBetaCandidate::getTRTLikelihoodBeta() const
	  {	    
	    return  m_TRTLikelihoodBeta;	  
	  }
  float InDetLowBetaCandidate::getTRTLikelihoodError() const
	  {	    
	    return  m_TRTLikelihoodError;	  
	  } 
  float InDetLowBetaCandidate::getTRTHighTbits() const
	  { 
	    return  m_TRTHighTbits;	  
	  }
	    
} // end of namespace




