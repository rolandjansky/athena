/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "InDetRawData/TRT_LoLumRawData.h"

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

	TRT_DriftCircle();
	TRT_DriftCircle(const TRT_DriftCircle &) =default;
	TRT_DriftCircle &operator=(const TRT_DriftCircle &)=default;
	TRT_DriftCircle &operator=(TRT_DriftCircle &&)=default;
  /** Constructor with parameters:
	 *  compact id of the DriftCircle, 
	 *  the driftRadius and its error
	 *  the RDO dataword with additional validity bit 
	 *  this class owns the pointers passed (except the TRT_BaseElement),
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
  return TRT_LoLumRawData::driftTimeBin(m_word);
}  

inline int TRT_DriftCircle::trailingEdge() const
{
  return TRT_LoLumRawData::trailingEdge(m_word);
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
  return TRT_LoLumRawData::timeOverThreshold(m_word);
}

inline double TRT_DriftCircle::rawDriftTime() const
{
  return (TRT_LoLumRawData::driftTimeBin(m_word)+0.5) * TRT_LoLumRawData::getDriftTimeBinWidth();
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
  // should return always 0 with the largest island algorithm
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
  // should return always 0 with the largest island algorithm
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

