/* -*- C++ -*- */

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPTOOLS_PILEUPTOOLBASE_H
#define PILEUPTOOLS_PILEUPTOOLBASE_H
/** @file PileUpToolBase.h
 * @brief helper base class IPileUpTool::toProcess().
 * $Id: PileUpStream.h,v 1.18 2008-10-31 18:34:42 calaf Exp $
 * @author Paolo Calafiura - ATLAS Collaboration
 */
#include "AthenaBaseComps/AthAlgTool.h"
#include "PileUpTools/IPileUpTool.h"

static constexpr unsigned int crazyParticleBarcode(std::numeric_limits<int32_t>::max());
//Barcodes at the HepMC level are int

class PileUpToolBase :  public extends<AthAlgTool, IPileUpTool> {
public:
  /// \name structors and AlgTool implementation
  //@{
  PileUpToolBase(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual StatusCode initialize() override {
    return StatusCode::SUCCESS;
  }
  //@}

  ///the method this base class helps implementing
  virtual bool toProcess(int bunchXing) const override {
    //closed interval [m_firstXing,m_lastXing]
    return !((m_firstXing > bunchXing) || (bunchXing > m_lastXing));
  }
  ///dummy implementation to allow compilation while all Digitization
  ///packages are migrated to use this new interface.
  using IPileUpTool::processAllSubEvents;
  virtual StatusCode processAllSubEvents(const EventContext&) override {
    return StatusCode::SUCCESS;
  }
  virtual StatusCode
  processBunchXing(int /*bunchXing*/,
                   SubEventIterator /*bSubEvents*/,
                   SubEventIterator /*eSubEvents*/) override {
    return StatusCode::SUCCESS;
  }
  ///dummy implementation of passing filter
  virtual bool filterPassed() const override { return m_filterPassed; }
  ///dummy implementation of filter reset
  virtual void resetFilter() override { m_filterPassed=true; }

protected:
  Gaudi::Property<int> m_firstXing{this, "FirstXing", -999,
      "First bunch-crossing in which det is live"};
  Gaudi::Property<int> m_lastXing {this, "LastXing", 999,
      "Last bunch-crossing in which det is live"};
  Gaudi::Property<int> m_vetoThisBarcode{this, "ParticleBarcodeVeto", crazyParticleBarcode,
      "Barcode of particle to ignore"};
  bool m_filterPassed{true}; ///stores whether the filter for this PileUpTool was passed.
};
#endif
