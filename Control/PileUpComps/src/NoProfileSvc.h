/*  -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_NOPROFILESVC
#define PILEUPCOMPS_NOPROFILESVC 1
/** @file NoProfileSvc.h
 * @brief A IBeamLuminosity service configured flat.
 *
 * @author atarce
 */
#include "PileUpTools/IBeamLuminosity.h"
#include "AthenaBaseComps/AthService.h"
#include "Gaudi/Property.h"

class NoProfileSvc : public extends<AthService, IBeamLuminosity>
{
public:
  /// \name Constructor / Destructor
  //@{
  NoProfileSvc(const std::string& name, ISvcLocator* svc);
  virtual ~NoProfileSvc();
  //@}
  /// \name AthService methods
  //@{
  virtual StatusCode initialize() override final;
  //@}
  /// \name IBeamLuminosity methods
  //@{
  virtual float scaleFactor(unsigned int run, unsigned int lumi, bool & updated) override final;
  //@}
};
#endif

