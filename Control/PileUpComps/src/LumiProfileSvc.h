/*  -*- C++ -*- */

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PILEUPCOMPS_LUMIPROFILESVC
#define PILEUPCOMPS_LUMIPROFILESVC 1
/** @file LumiProfileSvc.h
 * @brief A IBeamLuminosity service configured with a run/lumi list.
 *
 * @author atarce
 */
#include "PileUpTools/IBeamLuminosity.h"
#include "AthenaBaseComps/AthService.h"
#include "Gaudi/Property.h"

class LumiProfileSvc : public extends<AthService, IBeamLuminosity>
{
public:
  /// \name Constructor / Destructor
  //@{
  LumiProfileSvc(const std::string& name,ISvcLocator* svc);
  virtual ~LumiProfileSvc();
  //@}
  /// \name AthService methods
  //@{
  virtual StatusCode initialize() override final;
  //@}
  /// \name IBeamLuminosity methods
  //@{
  virtual float scaleFactor(unsigned int run, unsigned int lumi, bool & updated) override final;
  //@}
private:
  /// look-up table of lumi scale factor by run, lumiblock
  SimpleProperty< std::vector< uint64_t > >  m_runlumilist;
  SimpleProperty< std::vector< float > >  m_scalefactorlist;
  /// map from properties
  std::map<uint64_t,float> m_scaleFactorMap;
  /// cached scale factor
  float m_currentSF;
  /// cached iovtime
  uint64_t m_now;
};
#endif

