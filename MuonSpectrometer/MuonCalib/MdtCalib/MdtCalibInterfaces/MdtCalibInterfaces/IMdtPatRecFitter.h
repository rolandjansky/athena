/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMdtSegmentFitter.h
//   Header file for class IMdtPatRecFitter
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// rauscher@cern.ch
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_IMDTPATRECFITTER_H
#define MUONCALIB_IMDTPATRECFITTER_H

//MdtCalibInterfaces
#include "IMdtSegmentFitter.h"

namespace MuonCalib {
class MdtCalibHitBase;

/** @class IMdtPatRecFitter
Interface class for MdtSegment fitters performing a pattern recognition

@author rauscher@cern.ch
*/


class IMdtPatRecFitter : public IMdtSegmentFitter
	{
	public:
	/** constructor */
		inline IMdtPatRecFitter() : IMdtSegmentFitter(), m_refine_segment(false), m_refit(false)
			{
			}
	/** destructor */
		inline virtual ~IMdtPatRecFitter() {}
	
	/** number of hits selected for track */
		virtual unsigned int numberOfTrackHits(void) const =0;

	/** get selected track hits */
		virtual const std::vector<const MdtCalibHitBase*> & trackHits(void) const =0;

	/** set refine segment flag 
	 @param flag if true the hit selection is changed in the segment
	*/
		inline void SetRefineSegmentFlag(const bool & flag)
			{
			m_refine_segment=flag;
			}

	/** get refine segment flag */
		inline const bool & RefineSegmentFlag() const
			{
			return m_refine_segment;
			}
			
	/** switch on/off chi^2 refit after hit selection */
		inline void switchOnRefit(void)
			{
			m_refit=true;
			}
		inline void switchOffRefit(void)
			{
			m_refit=false;
			}
	/** return refit flag */
		inline const bool & refit(void) const
			{
			return m_refit;
			}
	/** set road width */
		virtual void setRoadWidth(const double &)=0;
	protected:
	//! flags
		bool m_refine_segment;
		bool m_refit;
	};


}

#endif
