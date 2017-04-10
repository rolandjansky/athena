/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2AllRoiUnpacking.h
// PACKAGE:  Trigger/TrigAlgorithms/T2AllRoiUnpacking
//
// AUTHOR:   gerbaudo@cern.ch
//
// Description: Unpack several L2 calorimeter ROIs and store them in a single output grid.
//              Used T2L1Unpacking (by Matthew Tamsett) as a template for this AllTEAlgo.
// Updates:
// Apr2012 (gerbaudo@cern.ch)
//         add the option to merge EM cells to a coarser granularity.
// ********************************************************************

#ifndef TRIGT2CALOJET_T2ALLROIUNPACKING_H
#define TRIGT2CALOJET_T2ALLROIUNPACKING_H

#include "TrigT2CaloJet/Trig3MomentumMerger.h"
#include "TrigT2CaloJet/T2CaloJetGridFromCells.h"
#include "TrigInterfaces/AllTEAlgo.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include <ostream>

class ITrigTimerSvc;
class TrigT2Jet;
class Trig3Momentum;
//class ITrigDataAccess;

class T2AllRoiUnpacking: public HLT::AllTEAlgo {


 public:
  T2AllRoiUnpacking(const std::string & name, ISvcLocator* pSvcLocator);
  ~T2AllRoiUnpacking();
  HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
                            unsigned int output);
  HLT::ErrorCode hltInitialize();
  HLT::ErrorCode hltFinalize();
  //! N of elements initially allocated in the grid
  /*! This is just a reasonable initial size for the vector (currently
    using 2^13, which corresponds to ~10 ROI at low lumi after noise
    suppr.). Also, this should be a static member (problems without
    namespace?).
   */
  const size_t kAllocatedGridCells_;
 private:
  // functions
  //! Process one trigger element
  /*!
    Cells are stored in m_grid, which should be allocated beforehand.
    @param te TriggerElement to be processed
    @param grid where the cells are stored
   */
  HLT::ErrorCode processTriggerElement(const HLT::TriggerElement *te,
				       std::vector<Trig3Momentum> *grid);
  bool initializeTimers(); //!< initialize the timers
  bool initializeHelperTools(); //!< initialize tools
  void resetTimers(); //!< reset the monitoring timers
  bool inputIsValid(const std::vector<std::vector<HLT::TriggerElement*> >& input) const;
  HLT::TriggerElement* setupOutputTe(unsigned int type_out) ; //!< prepare the output trigger element
  void storeTimers(); //!< store timings in the monitoring variables
  HLT::ErrorCode finalizeOutput(HLT::TriggerElement* outputTE); //! finalize the output TE (allocate TrigT2Jet and attach its grid)
  void finalizeAccessories();  //!< finalize everything that's not the output
  void determineOverlaps(const EtaPhiRectangle &l2Roi); //!< determine overlaps and store them in m_overlapRegions
  // Properties:
  std::string  m_jetOutputKey;
  double m_roiEtaHalfWidth; //!< half width (in eta) of the L2 ROI that will be read out
  double m_roiPhiHalfWidth; //!< half width (in phi) of the L2 ROI that will be read out
  bool m_mergeEmCells;      //!< whether or not the EM cells shoud be merged
  // non-configurable datamembers (see T2CaloJetGridFromCells.cxx for details)
  std::vector<Trig3Momentum>  m_tmpGrid;  //!< tmp grid used to extract before merge
  std::vector<int> m_ttEmSamplings;       //!< all possible EM samplings values
  std::vector<int> m_ttHecSamplings;      //!< all possible HEC samplings values
  int m_tileSampling;                     //!< tilecal sampling
  bool m_prepare;                         //!< prepare T2CaloJetGridFromCells
  Trig3MomentumMerger m_tmm;              //!< cell merger


 protected:
  ToolHandle< T2CaloJetGridFromCells > m_caloGridFromCellsTool;
  bool m_retrievedGridTool;
  std::vector<Trig3Momentum>* m_grid;
  //! @f$ (\eta,\phi) @f$ regions that have been already processed
  /*! This datamember is reset at each event */
  std::vector< EtaPhiRectangle > m_processedRegions;
  //! @f$ (\eta,\phi) @f$ regions that overlap with regions that have already been processed
  /*! This datamember is reset at each TriggerElement */
  std::vector< EtaPhiRectangle > m_overlapRegions;
  /** For Logging, in all tools we should use (*m_log) to avoid recalling of the MsgStream all the time. */
  MsgStream* m_log;
  
  TrigTimer *m_cell_unpacking_timer; //!< unpacking time ('addCell' only)
  TrigTimer *m_unpacking_timer;      //!< unpacking time (geometry, overlap, and 'addCell')
  TrigTimer *m_RoI_timer;            //!< time required to set up the output TE ('addRoI')
  TrigTimer *m_merge_timer;          //!< time required merge the EM cells ('mergeEmCells')

  // output
  TrigT2Jet     *m_jet;
  
  // Monitored Variables
  float               m_UnpckTime;      //!< see corresponding timer for details
  float               m_cellUnpckTime;  //!< see corresponding timer for details
  float               m_RoITime;        //!< see corresponding timer for details
  float               m_mergeTime;      //!< see corresponding timer for details
};

//----------------------------------------------------------
//
// helper classes
//
//----------------------------------------------------------

/*! \brief A class describing a rectangle in @f$ \eta @f$, @f$ \phi @f$
 *
 * An EtaPhiRectangle object can be used to keep track of a rectangle
 * in these coordinates. It is meant to be a simple object that takes
 * care of the @f$ 2\pi @f$ ambigiuties and that can perform simple operations
 * such as compute the overlap between two rectangles.  While the
 * minimum and maximum @f$ \phi @f$ values can be provided in any range,
 * internally all @f$ \phi angles @f$ are stored and handled within
 * (@f$ -\pi @f$,@f$ +\pi @f$].
 * Maybe at some point you could use TVector2.h (but they store x,y) or gsl::polar.
 */

//! \todo should probably define an EtaPhiPair instead of EtaPhiPoint...and use it everywhere
typedef std::pair< double, double > EtaPhiPoint;
//----------------------------------------------------------
class EtaPhiRectangle{
 public:
  //! default c'tor
  EtaPhiRectangle():
    m_etaMin(0.), m_etaMax(0.), m_etaCen(0.), m_etaHw(0.),
    m_phiMin(0.), m_phiMax(0.), m_phiCen(0.), m_phiHw(0.),
    m_wrapsAroundPi(false) {};
  //! constructor: defined like this because these are usually the edges we get for an ROI
  EtaPhiRectangle(const double &etaMin, const double &etaMax,
		 const double &phiMin, const double &phiMax);
  double area() const { return 2.0*m_etaHw*2.0*m_phiHw; };
  double eta() const { return m_etaCen; };
  double phi() const { return m_phiCen; };
  double etaMin() const { return m_etaMin; };
  double etaMax() const { return m_etaMax; };
  double phiMin() const { return m_phiMin; };
  double phiMax() const { return m_phiMax; };
  double etaHalfWidth() const {return m_etaHw; };
  double phiHalfWidth() const {return m_phiHw; };
  //! determine whether a given point in (@f$ \eta @f$, @f$ \phi @f$) is inside this EtaPhiRectangle
  bool contains(const EtaPhiPoint &point) const;
  //! same as above, but less safe agaist eta-phi swap
  bool contains(const double &eta, const double &phi) const { return contains(std::make_pair(eta,phi));};
  //! determine whether two rectangles overlap
  static double overlappingArea(const EtaPhiRectangle &lhs,
				const EtaPhiRectangle &rhs);
  //! compute the rectangle corresponding to the overlapping area
  static EtaPhiRectangle overlappingRectangle(const EtaPhiRectangle &lhs,
					     const EtaPhiRectangle &rhs);
  //! convert any angle to its equivalent in ( @f$ -\pi @f$, @f$ +\pi @f$]
  static double phi_mpi_pi(const double &val);
  //! convert any angle to its equivalent in ( 0 , @f$ +2\pi @f$]
  static double phi_zero_2pi(const double &val);
  //! print the rectangle
  void print(std::ostream& stream) const;

 protected:
  //! compute the location of the center of the rectangle
  /*! In fact this method is initializing the internal representation
    of the rectangle, with center, half width, etc.
   */
  void computeCenterCoord();
 private:
  double m_etaMin; //!< minimum eta
  double m_etaMax; //!< maximum eta
  double m_etaCen; //!< central eta
  double m_etaHw;  //!< eta half width
  double m_phiMin; //!< minimum phi
  double m_phiMax; //!< maximum phi
  double m_phiCen; //!< central phi
  double m_phiHw;  //!< phi half width
  bool m_wrapsAroundPi; //!< whether the rectangle crosses over @f$ \phi = \pi @f$

}; // end EtaPhiRectangle

std::ostream& operator<< (std::ostream& stream, const EtaPhiRectangle &epr);

//----------------------------------------------------------

/*! \brief compute L2 roi boundaries

Given a L1 ROI descriptor, compute the boundaries of a calorimeter
L2 ROI. We want to the ROI dimensions (halfWidths) to be
configurable, so we pass them in through the c'tor. The
calculation is done accounting for upper boundaries, and treating
for the FCAL in a special way.

\param trd the L1 ROI descriptor
\param etaHalfWidth desired half width in eta at L2 (usually 0.5)
\param phiHalfWidth desired half width in phi at L2 (usually 0.5)
*/
class L2CaloRoiBoundsCalculator {
 public:
  L2CaloRoiBoundsCalculator(const TrigRoiDescriptor* trd,
			    const double &etaHalfWidth,
			    const double &phiHalfWidth);
  double etaMin() const {return m_etaMin; } ;
  double etaMax() const {return m_etaMax; } ;
  double phiMin() const {return m_phiMin; } ;
  double phiMax() const {return m_phiMax; } ;
 private:
  //! actually compute and store the bounds
  void computeBounds(const TrigRoiDescriptor* trd,
		     const double &etaHalfWidth,
		     const double &phiHalfWidth);
  double m_etaMin, m_etaMax;  //!< eta range
  double m_phiMin, m_phiMax;  //!< phi range
  double m_roiEtaLimit;       //!< a reasonable maximum in @f$ \eta @f$ (4.8, does not include FCAL)
  double m_fcalEtaMin;	      //!< FCAL minumum @f$ \eta @f$ (3.0)
  double m_fcalEtaMax;	      //!< FCAL maxumum @f$ \eta @f$ (5.0)
  double m_minFcalEtaCenter;  //!< if the \eta center is above this (3.2), then we think it's FCAL
}; // end L2CaloRoiBoundsCalculator

//----------------------------------------------------------


//----------------------------------------------------------


#endif
