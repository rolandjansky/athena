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
#include "GaudiKernel/Property.h"
template <class TYPE> class SvcFactory;

class NoProfileSvc : virtual public IBeamLuminosity, public AthService {
public:
  virtual ~NoProfileSvc();
  virtual float scaleFactor(unsigned int run, unsigned int lumi, bool & updated);
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
protected:
  friend class SvcFactory<NoProfileSvc>;
  NoProfileSvc(const std::string& name,ISvcLocator* svc);


};
#endif

