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
	virtual const InDetDD::TRT_BaseElement* detectorElement() const override final;

  /** Interface method checking the type*/
  virtual bool type(Trk::PrepRawDataType::Type type) const override final;

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
  virtual MsgStream&    dump( MsgStream&    stream) const override;

  /** dump information about the PRD object. */
  virtual std::ostream& dump( std::ostream& stream) const override;


	private:
	// not const because of DataPool
	const InDetDD::TRT_BaseElement* m_detEl;
  unsigned int m_word;
};

 MsgStream&    operator << (MsgStream& stream,    const TRT_DriftCircle& prd);
 std::ostream& operator << (std::ostream& stream, const TRT_DriftCircle& prd);
}

#include "InDetPrepRawData/TRT_DriftCircle.icc"
#endif // TRKPREPRAWDATA_TRT_DRIFTCIRCLE_H

