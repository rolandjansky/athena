/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtCalibTool.h
//   Header file for class MdtCalibTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//  Niels.Van.Eldik@cern.ch, 
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Modifications:
// Tue Feb 28 2006 by Oliver Kortner: segment selection added in
//                                    order to avoid ambiguities,
//                                    by default this selection is
//                                    disabled and must be enabled
//                                    via the job option
//                                    MdtCalibTool.SelectorSwitch=1
///////////////////////////////////////////////////////////////////


#ifndef MUONCALIB_MDTCALIBTOOL_H
#define MUONCALIB_MDTCALIBTOOL_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"

#include "MuonCalibITools/IMuonCalibTool.h"
#include "MuonCalibITools/IRegionSelectorTool.h"

#include <map>


class MdtIdHelper;

namespace MuonCalib {

  class IMdtCalibTool;
  class IMdtSegmentSelectorTool;
  class IMdtSegmentSelector;
  class MdtCalibEventLoop;
  class IMdtCalibrationOutput;
  class MuonCalibEvent;

/**@class MdtCalibTool
   Steers MdtCalibration within athena. 

   Modification: 28.02.2006 by O.Kortner:
    A simple segment selection can be enabled via the job option
    MdtCalibTool.SelectorSwitch=1. If the selection is switchted on,
    the following selection is applied to chambers with more than one
    segment in an event: the segments which have more hits on the
    reconstructed track than the others are kept; if two segments have
    the same number of track hits, the segment with the smaller chi^2 is
    selected.

   @author Niels.Van.Eldik@cern.ch, 
*/
  class MdtCalibTool : public AthAlgTool, virtual public IMuonCalibTool {
  public:
  /**AlgTool constructor */
  MdtCalibTool(const std::string&,const std::string&,const IInterface*);

  /**Virtual destructor*/
  virtual ~MdtCalibTool(){};

  /** AlgTool initailize method.*/
  StatusCode initialize();

  /** AlgTool finalize method */
  StatusCode finalize();  

  /** Interface method: handle MuonCalibPatternCollection */
  //  virtual bool handle( const MuonCalibPatternCollection* patterns );

  virtual bool handleEvent( const MuonCalibEvent* event );

  /** perform analysis */
  virtual bool analyse();

  private:
  /** for now create a new eventloop for every calibration region  */
  typedef std::map< IRegionSelectorTool::RegionKey, MdtCalibEventLoop* > RegionEventMap;

  private:
  /** routines to pass segments to event loop */   
  void handleMuonSegment( const MuonCalibSegment* seg );

  /** generate RegionKey from identifier */
  IRegionSelectorTool::RegionKey keyFromIdentifier( const IRegionSelectorTool::id_type& id ) const;

  /** get EventLoop for given region key */
  MdtCalibEventLoop* getEventLoopForRegion( const IRegionSelectorTool::RegionKey& key );

  /** Map containing the segments (stored in the eventloop) per calibration region */
  RegionEventMap m_segmentsPerRegion;
   
  /** RtCalibration initialization */
  ToolHandle<IMdtCalibTool> m_mdtCalibTool;      //!<  RtCalibrationCreator ToolHandle

  /** Switch for the MdtCalibTool internal segment selection **/
  int m_SelectorSwitch; //!< selector switch (is set to 0 by the default)

  /** MdtSegmentSelector initialization */
  ToolHandle<IMdtSegmentSelectorTool> m_segmentSelectorTool; //!< MdtSegmentSelectorTool ToolHandle
  const IMdtSegmentSelector* m_segmentSelector; //!< MdtSegmentSelector

  /** RegionSelector initialization */
  ToolHandle<IRegionSelectorTool> m_regionSelector; //!< RegionSelector ToolHandle
   
  const MdtIdHelper* m_mdtIdHelper; //!< pointer to MdtIdHelper

  /** maximum number of iterations */
  unsigned int m_maxIterations;
  };

}

#endif
