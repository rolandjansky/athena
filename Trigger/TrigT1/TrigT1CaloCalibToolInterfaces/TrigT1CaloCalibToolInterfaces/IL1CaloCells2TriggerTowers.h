/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  ***************************************************************************
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *

//
// Pure abstract base class interface to L1CaloCells2TriggerTowers
//

#ifndef TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOCELLS2TRIGGERTOWERS_H
#define TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOCELLS2TRIGGERTOWERS_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

#include <vector>

class CaloCellContainer;
class LArDigitContainer;
class TileDigitsContainer;
class CaloCell;
class Identifier;
class LArDigit;
class TileDigits;
class LArOnOffIdMapping;

namespace LVL1{

  class IL1CaloCells2TriggerTowers : virtual public asg::IAsgTool{
      
    /// Declare the interface that the class provides
    ASG_TOOL_INTERFACE( LVL1::IL1CaloCells2TriggerTowers )
      
    public:

      virtual bool initCaloCellsTriggerTowers(const CaloCellContainer& cellContainer) = 0;
      virtual bool initLArDigitsTriggerTowers(const LArDigitContainer& larDigitContainer) = 0;
      virtual bool initTileDigitsTriggerTowers(const TileDigitsContainer& tileDigitsContainer) = 0;

      virtual std::vector<const CaloCell*> caloCells(const Identifier& ttId) const = 0;
      virtual std::vector<std::vector<const CaloCell*> > caloCellsByLayer(const Identifier& ttId) const = 0;
      virtual std::vector<int> layerNames(const Identifier& ttID) const = 0;
      virtual int layerNames(const CaloCell* cell) const = 0;
      virtual float energy(const std::vector<const CaloCell*> &cells) const = 0;
      virtual float et(const std::vector<const CaloCell*> &cells) const = 0;
      virtual float energy(const Identifier& ttId) const = 0;
      virtual float et(const Identifier& ttId) const = 0;
      // Return a vector digits belonging the to requested trigger tower
      virtual std::vector<double> samples(const Identifier& ttId) const = 0;
      virtual std::vector<const LArDigit*> larDigits(const Identifier& ttId) const = 0;
      virtual std::vector<const TileDigits*> tileDigits(const Identifier& ttId) const = 0;

      virtual void dumpCaloCells() const = 0;
      virtual void dumpCaloCells(const Identifier& ttId) const = 0;

      virtual void dumpDigits(const Identifier& ttId) const = 0;

      virtual void dumpLArDigits(const Identifier& ttId) const = 0; /* ttId must be a LAr TTId */
      virtual void dumpLArDigits() const = 0;

      virtual void dumpTileDigits(const Identifier& ttId) const = 0; /* ttId must be a Tile TTId */
      virtual void dumpTileDigits() const = 0;

    protected:
      virtual float calcEnergyOrEt(const std::vector<const CaloCell*> &cells,const unsigned int mode) const = 0;

      virtual void reset() = 0;
      virtual void resetCaloCells() = 0;
      virtual void resetLArDigits() = 0;
      virtual void resetTileDigits() = 0;


      virtual void dump(const std::vector<const CaloCell*>& vCells) const = 0;
      virtual void dump(const std::vector<const LArDigit*>& vCells) const = 0;
      virtual void dump(const std::vector<const TileDigits*>& vCells) const = 0;

  };
} // end of namespace
#endif
