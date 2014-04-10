/* -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
class PileUpToolBase :  virtual public IPileUpTool, public AthAlgTool {
public:
 /// \name structors and AlgTool implementation
 //@{
  PileUpToolBase(const std::string& name, 
		 const std::string& type,
		 const IInterface* parent);
  virtual StatusCode queryInterface(const InterfaceID&, void**);
  virtual StatusCode initialize() {
    return StatusCode::SUCCESS;
  }
  //@}

  ///the method this base class helps implementing
  virtual bool toProcess(int bunchXing) const {
    //closed interval [m_firstXing,m_lastXing]
    return !((m_firstXing > bunchXing) || (bunchXing > m_lastXing));
  }
  ///dummy implementation to allow compilation while all Digitization
  ///packages are migrated to use this new interface.
  virtual StatusCode processAllSubEvents() {
    return StatusCode::SUCCESS;
  }
 protected:
  int m_firstXing; ///>Property: first xing in which det is live
  int m_lastXing; ///>Property: last xing in which det is live
};
#endif
