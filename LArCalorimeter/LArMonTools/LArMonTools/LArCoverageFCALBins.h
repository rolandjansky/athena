/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Sven Menke <menke@mppmu.mpg.de>
 * 
 * @class LArCoverageFCALBins
 * @brief definitions for FCal coverage histos based on atlasdd
 * LArFCalElectrodes-04 first used with DC3-05-Comm-01
 * 
 * The bins are for 2d histograms with y = |tan(theta)| sin(phi) and x
 * = |tan(theta)| cos(phi) and the following binning per sampling:
 *
 * TH2F * fcal[0] = new
 * TH2F("FCal1","FCal1",240,-0.0901834,0.0901834,138,-0.0901834,0.0901834);
 * TH2F * fcal[1] = new
 * TH2F("FCal2","FCal2",216,-0.0821644,0.0821644,126,-0.0821644,0.0821644);
 * TH2F * fcal[2] = new
 * TH2F("FCal3","FCal3",200,-0.0756658,0.0756658,114,-0.0756658,0.0756658);
 *
 * alternatively the coordinates can be chosen in physical space (mm)
 * but with the same number of bins:
 *
 * TH2F * fcal[0] = new
 * TH2F("FCal1","FCal1",240,-444.92,444.92,138,-444.92,444.92);
 * TH2F * fcal[1] = new
 * TH2F("FCal2","FCal2",216,-442.70,442.70,126,-442.70,442.70);
 * TH2F * fcal[2] = new
 * TH2F("FCal3","FCal3",200,-443.25,443.25,114,-443.25,443.25);
 * 
 */

#ifndef LARCOVERAGEFCALBINS_H
#define LARCOVERAGEFCALBINS_H

class LArCoverageFCALBins
{
public:
  
  // Constructors
  LArCoverageFCALBins() {}

  // Destructors
  virtual ~LArCoverageFCALBins() {}

  // Operators

  // Selectors 

  // Methods
  int getXBins(int iSide, int iSamp) const;

  int getYBins(int iSide, int iSamp) const;

  float getRMax(int iSide, int iSamp) const;

  float getRMin(int iSide) const;

  const unsigned short * getBins(int iSide, int iSamp, int iEta, int iPhi) const;
  
};

#endif // LARCOVERAGEFCALBINS_H


