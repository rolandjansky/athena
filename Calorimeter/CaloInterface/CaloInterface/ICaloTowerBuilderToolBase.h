/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOINTERFACE_ICALOTOWERBUILDERTOOLBASE_H
#define CALOINTERFACE_ICALOTOWERBUILDERTOOLBASE_H
///////////////////////////////////////////////////////////////////////////////
/// \brief ICaloTowerBuilderToolBase is abstract interface for CALOTOWERBUILDERTOOLBASE
///
/// ICaloTowerBuilderToolBase is an abstract class to access tower builder 
/// tool.
///
///     Concrete tools, derived from the CaloTowerBuilderToolBase base abstract
///	class are controlled via this interface.
///
/// \author Luca Fiorini <Luca.Fiorini@cern.ch>
/// \date October 4, 2007 -
///
///////////////////////////////////////////////////////////////////////////////

#include "CaloEvent/CaloTowerSeg.h"
#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <string>

class CaloTowerContainer;
class CaloTowerSeg;
class CaloCellContainer;


// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_ICaloTowerBuilderToolBase("ICaloTowerBuilderToolBase", 1 , 0); 

class ICaloTowerBuilderToolBase : virtual public IAlgTool
{
 public:
  
  //Virtual destructor
  virtual ~ICaloTowerBuilderToolBase() {}

  /// common initialization

  /**
   * @brief Run tower building and add results to the tower container.
   *        If a cell container is provided, use that; otherwise, fetch
   *        from SG (the key is given by a job property).
   *        If @c subseg is provided, then we do the building
   *        only within the rectangular region that it describes.
   *        The segmentation of the tower container must match
   *        the region over which we're running the tower building.
   *
   * @param theContainer The tower container to fill.
   * @param theCell The cell container to read.  If null, we fetch from SG.
   * @param subseg If provided, run tower building only within this window.
   *               The tower container segmentation must match.
   */
  virtual StatusCode execute(CaloTowerContainer* theContainer,
                             const CaloCellContainer* theCell=0,
                             const CaloTowerSeg::SubSeg* subseg = 0) = 0;
  virtual StatusCode initialize() =0;

  virtual StatusCode initializeTool() = 0;

  virtual void setTowerSeg(const CaloTowerSeg& theTowerSeg) = 0;

  virtual StatusCode LoadCalibration(IOVSVC_CALLBACK_ARGS) = 0;

  static const InterfaceID& interfaceID() {
    static const InterfaceID IID("ICaloTowerBuilderToolBase", 1 , 0);
    return IID;
  }

 protected:


};
#endif
