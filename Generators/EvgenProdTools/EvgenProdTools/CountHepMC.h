/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVGENPRODTOOLS_COUNTHEPMC_H
#define EVGENPRODTOOLS_COUNTHEPMC_H

#include "GeneratorModules/GenBase.h"


/// @brief Count the number of events to pass all algorithms/filters
///
/// CountHepMC counts the number of events / weights to pass algs
/// in the evgen production chain. Its main use seems to be handling
/// complex situations like "flattened" weighted jet slice production,
/// where it is responsible for deciding when to escape the event loop.
///
/// @todo This is hideously complex. With the new MC14 evgen alg sequence
///  structure, can it now be (greatly) simplifed?
///
class CountHepMC : public GenBase {
public:

  CountHepMC(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CountHepMC();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  int m_nPass;
  int m_nCount;

  double m_wtpass;
  double m_wthepmc;
  double m_wtall;

  int m_firstEv;

  bool m_corHepMC;
  bool m_corEvtID;
  bool m_useEventWeight;

  /// Vector of names of streams
  StringArrayProperty m_streamNames;

  /// Vector of Algorithm stream
  /// @todo Why *pointers* to vectors?
  std::vector<Algorithm*>* m_streamAlgs;

  /// Vector of Algorithms that this stream requires
  /// @todo Why *pointers* to vectors?
  std::vector<Algorithm*>* m_requireAlgs;

  /// Vector of Algorithms that this stream requires
  /// @todo Why *pointers* to vectors?
  std::vector<Algorithm*>* m_vetoAlgs;

  /// Vector of Algorithms that this stream requires
  /// @todo Why *pointers* to vectors?
  std::vector<Algorithm*>* m_acceptAlgs;

  /// Decode specified list of Algorithms
  StatusCode decodeAlgorithms(StringArrayProperty& theNames, std::vector<Algorithm*>* theAlgs);
  StatusCode decodeStreamAlgs();
  /// Handler for RequireAlgs Property
  void streamAlgsHandler(Property&);

  /// Handler for RequireAlgs
  StatusCode decodeAlgRequires();
  /// Handler for VetoAlgs
  StatusCode decodeAlgVetos();
  /// Handler for AcceptAlgs
  StatusCode decodeAlgAccepts();

  /// Decode list of Algorithms that this stream is vetoed by
  bool isEventAccepted() const;
  bool isTestHepMCPassed() const;
  bool isStreamRechecked;

};


#endif
