// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTHINNING_TRIGELECTRONTHINNINGTOOL_H
#define TRIGTHINNING_TRIGELECTRONTHINNINGTOOL_H

#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "xAODTrigEgamma/TrigElectronContainer.h"

class TrigElectronThinningTool : public AthAlgTool, public DerivationFramework::IThinningTool {
public:
  TrigElectronThinningTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TrigElectronThinningTool() {}
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode doThinning() const;
private:
  

  StatusCode selectElectrons(const xAOD::TrigElectronContainer* container) const;
  ServiceHandle<IThinningSvc> m_thinningSvc;
  std::vector<std::string> m_keys; //!< the list of SG keys over which thinning should run, empty means over all
  double m_ptThreshold;
  bool m_followLinks; //!< decide if the Element links to the object used to build electrons are to be followed and appropriate thinning done on them
  // can imagine other selection, especially the quality 
  template<class C>
  StatusCode thinLinks(const C* container, const std::set<typename C::const_value_type>& tokeep) const;
  std::vector<void*> m_clusters;
  std::vector<void*> m_tracks;
};

#endif
