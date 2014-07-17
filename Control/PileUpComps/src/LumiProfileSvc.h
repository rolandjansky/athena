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
#include "GaudiKernel/Property.h"
template <class TYPE> class SvcFactory;

class LumiProfileSvc : virtual public IBeamLuminosity, public AthService {
public:
  virtual ~LumiProfileSvc();
  virtual float scaleFactor(unsigned int run, unsigned int lumi, bool & updated);
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
protected:
  friend class SvcFactory<LumiProfileSvc>;
  LumiProfileSvc(const std::string& name,ISvcLocator* svc);
private:
  /// look-up table of lumi scale factor by run, lumiblock
  SimpleProperty< std::vector< uint64_t > >  m_runlumilist;
  SimpleProperty< std::vector< float > >  m_scalefactorlist;
  /// map from properties
  std::map<uint64_t,float> m_scaleFactorMap;
  /// cached sf
  float m_currentSF;
  /// cached iovtime
  uint64_t m_now;
  /// 

};
#endif

