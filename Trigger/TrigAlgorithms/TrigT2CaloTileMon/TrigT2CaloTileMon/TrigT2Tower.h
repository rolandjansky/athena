/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

 NAME:     TrigT2Tower.h
 PACKAGE:  Trigger/TrigAlgorithms/TrigT2CaloTileMon

 AUTHOR:   Jens Damaske
 PURPOSE:  Compines 3 Trig3Momentum Objects to a Tower

 *******************************************************************/

#ifndef TRIGT2CALOTILEMON_TRIGTT2TOWER_H
#define TRIGT2CALOTILEMON_TRIGTT2TOWER_H

#include "TrigCaloEvent/Trig3Momentum.h"
/// #include "Trig3Momentum.h"

#include "GaudiKernel/MsgStream.h"

#include <sstream>
#include <iostream>
#include <cmath>

/**
   3 cells of type Trig3Momentum per tower
   "cell.inCone is used as cell.inTower"
*/
class TrigT2Tower{
   
   public:
      /** Default Constructor */
      TrigT2Tower();
      TrigT2Tower(const Trig3Momentum& newCell, MsgStream& log, double etaShift=0.0);

      // Get methods
      /** Get the energy */
      double e()      const {return m_e;  }
      /** Get the eta position */
      double eta()    const {return m_eta;  }
      /** Get the phi position */
      double phi()    const {return m_phi;  }
      /** Return if tower consists of 3 cells */
      bool isComplete() const {return m_full;  }
      /** Check if newCell belongs to this tower */
      bool isMember(const Trig3Momentum& newCell, MsgStream& log, double etaShift=0.0) const;
      Trig3Momentum cell(int i) const {return m_cells[i];   }

      // Set methods
      /** Set energy */
      void setE(double e)            {m_e = e;  }
      /** Set eta */
      void setEta(double eta)        {m_eta = eta;  }
      /** Set phi */
      void setPhi(double phi)        {m_phi = phi;  }
      /** Flag if tower is full */
      void setComplete(bool isComplete)    {m_full = isComplete;}
      /** Insert cell */
      bool insertCell(const Trig3Momentum& newCell, MsgStream& log);

      // debugging
      void print(MsgStream& log, MSG::Level level) const;

   private:
      /** Energy */
      double m_e;
      /** Eta */
      double m_eta;
      /** Phi */
      double m_phi;
      /** is full tower */
      bool m_full;
      /** tower cells one for each layer */
      Trig3Momentum m_cells[3];
      
};

#endif
