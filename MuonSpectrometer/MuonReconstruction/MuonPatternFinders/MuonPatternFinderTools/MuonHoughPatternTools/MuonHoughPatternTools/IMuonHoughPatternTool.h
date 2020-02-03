/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCollection.h"

/** Must declare this, with name of interface*/
static const InterfaceID IID_IMuonHoughPatternTool("IMuonHoughPatternTool", 1, 0);

class MuonHoughHitContainer;

namespace Muon {
  
  class MuonPrdPattern;}

class IMuonHoughPatternTool : virtual public IAlgTool 
{
 public:
  /** @todo should be rethought and possibly using the Moore Interface */

  /** Declared here, and defined below*/
  static const InterfaceID& interfaceID();
  
  /** Builds Patterns */
  virtual void makePatterns(const MuonHoughHitContainer* hitcontainer, MuonHoughPatternContainerShip& houghpatterns) const = 0;

  /** returns phi patterns */
  virtual MuonPrdPatternCollection* getPhiMuonPatterns(MuonHoughPatternContainerShip& houghpatterns) const = 0;  
  /** returns eta patterns */
  virtual MuonPrdPatternCollection* getEtaMuonPatterns(MuonHoughPatternContainerShip& houghpatterns) const =0;
  /** returns houghpatterns arrays*/
  virtual MuonHoughPatternContainerShip emptyHoughPattern() const =0;
  /** resets houghpattern arrays*/
  virtual void reset(MuonHoughPatternContainerShip& houghpattern) const =0;
};

inline const InterfaceID& IMuonHoughPatternTool::interfaceID()
{ 
  return IID_IMuonHoughPatternTool; 
}
