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

#include "GaudiKernel/MsgStream.h"
#include "CLIDSvc/CLASS_DEF.h"

class MsgStream;

namespace InDet{

  class InDetLowBetaCandidate{
   
    /** default, copy = operator constructors */
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
    InDetLowBetaCandidate(const InDetLowBetaCandidate& rhs);    
    InDetLowBetaCandidate &operator=(const InDetLowBetaCandidate &);
    /////////virtual VxCandidate* clone() const; 

    /** Output Method for MsgStream, to be overloaded by child classes */
    virtual MsgStream& dump(MsgStream& sl) const;
    /** Output Method for std::ostream, to be overloaded by child classes */
    //virtual std::ostream& dump(std::ostream& sl) const;

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
  //std::ostream& operator << ( std::ostream& sl, const InDetLowBetaCandidate& sf);

}//end of namsepace

CLASS_DEF(InDet::InDetLowBetaCandidate,115223127  , 1 )

#endif // INDETLOWBETACANDIDATE_H









