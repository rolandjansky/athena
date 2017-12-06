/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DriftCircle.h
//   Header file for class TRT_DriftCircle
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement DriftCircle for TRT
///////////////////////////////////////////////////////////////////
// Version 7.0 15/07/2007 Peter Hansen
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_TRT_DRIFTCIRCLE_H
#define TRKPREPRAWDATA_TRT_DRIFTCIRCLE_H

// Base class
#include "TrkPrepRawData/PrepRawData.h"
#include "InDetReadoutGeometry/TRT_BaseElement.h"

class TRT_DriftCircleContainerCnv;
class TRT_DriftCircleContainerCnv_p0;
class MsgStream;


namespace InDet{

class TRT_DriftCircle :   public Trk::PrepRawData
{
	friend class ::TRT_DriftCircleContainerCnv;
	friend class TRT_DriftCircleContainerCnv_p1;
	friend class ::TRT_DriftCircleContainerCnv_p0;
	///////////////////////////////////////////////////////////////////
	// Public methods:
	///////////////////////////////////////////////////////////////////
	public:
	
	/** default, copy = operator constructors */
	TRT_DriftCircle();
	TRT_DriftCircle(const TRT_DriftCircle &);
	TRT_DriftCircle &operator=(const TRT_DriftCircle &);
	
        /** Constructor with parameters:
	compact id of the DriftCircle, 
	the driftRadius and its error
	the RDO dataword with additional validity bit 
	this class owns the pointers passed (except the TRT_BaseElement),
        */
	TRT_DriftCircle( 
		const Identifier &clusId,
		const Amg::Vector2D& driftRadius,
		const std::vector<Identifier>& rdoList,
		const Amg::MatrixX* errDriftRadius,
		const InDetDD::TRT_BaseElement* detEl,
                const unsigned int word=0
		);

	TRT_DriftCircle( 
		const Identifier &clusId,
		const Amg::Vector2D& driftRadius,
		const Amg::MatrixX* errDriftRadius,
		const InDetDD::TRT_BaseElement* detEl,
                const unsigned int word=0
		);
                
        TRT_DriftCircle( 
                const Identifier &Id, 
                const Amg::Vector2D& driftRadius,
		std::vector<Identifier>&& rdoList,
                std::unique_ptr<const Amg::MatrixX> errDriftRadius,
                const InDetDD::TRT_BaseElement* detEl,
                const unsigned int word);

	/** Destructor*/
	virtual ~TRT_DriftCircle();

	//accesors

        /** returns the TRT dataword */
        virtual unsigned int getWord() const;

        /** returns the leading edge bin
         *  defined as in TRT_LoLumRawData to be the first 0-1 transition */
        virtual int driftTimeBin() const;

        /** returns the trailing edge bin */
        virtual int trailingEdge() const;
	
	/** returns true if the high level threshold was passed */
	virtual bool highLevel() const ;

	/** returns true if the first bin is high */
        virtual bool firstBinHigh() const; 

	/** returns true if the last bin is high */
        virtual bool lastBinHigh() const; 
	
	/** returns  Time over threshold in ns  */
	virtual double timeOverThreshold() const ;  

        /** returns number of high bins between LE and TE (these included) */
	virtual int numberOfHighsBetweenEdges() const;                                             
        /** returns number of low bins between LE and TE (these included) */
	virtual int numberOfLowsBetweenEdges() const;                                             
        /** returns the raw driftTime */
        virtual double rawDriftTime() const;
	
        /** returns the raw driftTime,
	 * the passed boolean indicates if the drift time is valid or not.
         * depreciated for 13.0.20 and later */
        virtual double driftTime(bool& valid) const;

	/** return true if the corrected drift time is OK */
        virtual bool driftTimeValid() const; 
        
	/** return the detector element corresponding to this PRD */
	virtual const InDetDD::TRT_BaseElement* detectorElement() const;

        // modifiers

	/** set driftTimeValid flag */
        virtual void setDriftTimeValid(bool valid);

        //analysers

	/** returns true if the hit is caused by noise with a high
            probability. This is a temporary feature. To be replaced
            by a tool that can be configured for different gas
            speeds etc */
	virtual bool isNoise() const;

        //debug printers

        /** dump information about the PRD object. */
        virtual MsgStream&    dump( MsgStream&    stream) const;

       /** dump information about the PRD object. */
        virtual std::ostream& dump( std::ostream& stream) const;


	private:
	// not const because of DataPool
	const InDetDD::TRT_BaseElement* m_detEl;
        unsigned int m_word;

};

 MsgStream&    operator << (MsgStream& stream,    const TRT_DriftCircle& prd);
 std::ostream& operator << (std::ostream& stream, const TRT_DriftCircle& prd);


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline unsigned int TRT_DriftCircle::getWord() const
{
       return m_word;
}

inline int TRT_DriftCircle::driftTimeBin() const
{
  unsigned  mask = 0x02000000;
  unsigned  word_LE = m_word>>6;
  word_LE = word_LE<<6;
 
  mask >>=1;
  bool SawZero = false;
  int i;
  for(i=1;i<18;++i)
  { 
    if      (  (word_LE & mask) && SawZero) break;
    else if ( !(word_LE & mask) ) SawZero = true; 
    mask>>=1;
    if(i==7 || i==15) mask>>=1;
  }
  if(i==18) i=0;
  return i;
}  

inline int TRT_DriftCircle::trailingEdge() const
{
  unsigned mask = 0x00000001;
  unsigned mask_word = 0x0001fff0; // 11111111 1 11110000   
  unsigned mask_last_bit =0x10; //10000
  
  unsigned word_TE = m_word & mask_word;
  
  bool SawZero=true;
  bool SawZero1=false;
  bool SawZero2=false;
  bool SawUnit1=false;
  int i=0;
  int j=0;
  int k=0;
  
  if(word_TE & mask_last_bit) 
  {
  
	for (j = 0; j < 11; ++j)
	{
		mask_last_bit=mask_last_bit<<1;
		
		if(j==3) mask_last_bit=mask_last_bit<<1;
		
		if ( !(word_TE & mask_last_bit) )
		{
			SawZero2 = true;
			break;			
		}
	}
	
	if(SawZero2 == false) return 19;

	if(SawZero2 == true){
		for (k = j+1; k < 11; ++k)
		{
			mask_last_bit=mask_last_bit<<1;

			if(k==3) mask_last_bit=mask_last_bit<<1;

			if ( word_TE & mask_last_bit )
			{
				SawUnit1 = true;
				break;					
			}
		} 
	}
	
	if(SawUnit1 == false && SawZero2 == true) return 19;
	
  }
  
  //+++++++++++++++++++++++++++++++++++++
  
  for (i = 0; i < 24; ++i)
  {
  
	if(!(word_TE & mask) && i>3)
	{
	  SawZero1 = true;
	}
    if(SawZero1){  
		if ( (word_TE & mask) && SawZero )
			break;
		else if ( !(word_TE & mask) )
			SawZero = true;
    }
    mask <<= 1;
    if (i == 7 || i == 15)
      mask <<= 1;
  }
 
  if ( 24 == i )
    return i;

  return (23 - i);

}

inline bool TRT_DriftCircle::highLevel() const 
{
  return (m_word & 0x04020100);
}


inline bool
TRT_DriftCircle::firstBinHigh() const 
{ 
  return (m_word & 0x02000000);
}

inline bool
TRT_DriftCircle::lastBinHigh() const
{
  return (m_word & 0x1);
}

inline double TRT_DriftCircle::timeOverThreshold() const
{
  double binWidth = 3.125;

  int LE = driftTimeBin( );
  int TE = trailingEdge( );

  if ( (0 == LE) || (24 == LE) || (24 == TE) || (0 == TE) || (23 == LE) )
  {
    return 0;
  }

  double time = (double) (TE - LE + 1) * binWidth;

  return time;

}

inline double TRT_DriftCircle::rawDriftTime() const
{
  return (driftTimeBin()+0.5)*3.125;
}

inline bool TRT_DriftCircle::driftTimeValid() const
{
  return m_word & 0x08000000; 
}

inline void TRT_DriftCircle::setDriftTimeValid(bool valid)
{
  unsigned maskfalse = 0xF7FFFFFF; 
  unsigned masktrue  = 0x08000000; 
  if( valid ) {
    m_word |= masktrue;
  } else {
    m_word &= maskfalse; }
}

inline int TRT_DriftCircle::numberOfHighsBetweenEdges() const
{
  int LE = driftTimeBin( );
  int TE = trailingEdge( );
  unsigned  mask = 0x02000000;
  int nhigh=0;
  int i;
  for (i = 0; i < 24; ++i)
    {
      if ( (m_word & mask) && i>=LE && i<=TE ) nhigh++;
      mask >>= 1;
      if (i == 7 || i == 15) mask >>= 1;
    }
  return nhigh;
}

inline int TRT_DriftCircle::numberOfLowsBetweenEdges() const
{
  int LE = driftTimeBin( );
  int TE = trailingEdge( );
  return (TE-LE+1-numberOfHighsBetweenEdges());
}


inline bool TRT_DriftCircle::isNoise() const                                                       
{
  if( numberOfHighsBetweenEdges()<3 ) return true;
  if( timeOverThreshold()<7.)         return true;
  return false;
}

inline const InDetDD::TRT_BaseElement* TRT_DriftCircle::detectorElement() const
{
	return m_detEl;
}

}
#endif // TRKPREPRAWDATA_TRT_DRIFTCIRCLE_H

