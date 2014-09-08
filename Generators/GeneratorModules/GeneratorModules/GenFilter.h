/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORMODULES_GENFILTER_H
#define GENERATORMODULES_GENFILTER_H

#include "GeneratorModules/GenBase.h"


/// @brief Base class for event generator filtering modules
///
/// @author Ian Hinchliffe (initial code, December 2001)
/// @author Andy Buckley (updates to AthAlgorithm, etc., Dec 2011)
/// @author Andy Buckley (conversion to GenBase and tidying, Nov 2013)
///
/// This class is the base class used to specify the behavior of all
/// event generator filtering modules and is meant to capture
/// the common behavior of these modules.  GenFilter inherits the
/// GenBase interface and is principally used in Generators/GeneratorFilters.
///
/// The following virtual methods should be overloaded in the child class:
///   StatusCode filterInitialize() (optional)
///   StatusCode filterEvent()
///   StatusCode filterFinalize() (optional)
///
/// LeptonFilter or ElectronFilter are good templates for learning to implement
/// new filters.
///
class GenFilter : public GenBase {
public:

  GenFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GenFilter() { }

  /// @name Event loop algorithm methods: not to be overloaded
  //@{
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  //@}

  /// @name Gen-specific event loop methods: to be overloaded!
  //@{
  virtual StatusCode filterInitialize() { return StatusCode::SUCCESS; }
  virtual StatusCode filterEvent() = 0;
  virtual StatusCode filterFinalize() { return StatusCode::SUCCESS; }
  //@}

  /// @name Counters
  //@{
  int nPassed() const { return m_nPass; }
  int nFailed() const { return m_nFail; }
  int nNeeded() const { return m_nNeeded; }
  //@}


protected:

  /// @name Counters and requirements
  //@{
  int m_nPass;
  int m_nFail;
  int m_nNeeded;
  //@}

};


#endif
