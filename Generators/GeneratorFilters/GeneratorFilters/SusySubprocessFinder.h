/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATORFILTERS_SUSYSUBPROCESSFINDER_H
#define GENERATORFILTERS_SUSYSUBPROCESSFINDER_H

#include "GeneratorModules/GenFilter.h"


/**
 * When a generator filter is used, info on the rejected events is lost.
 * In order to be able to use subprocess cross-sections (in a SUSY context),
 * we need to know the generator filter efficiencies for each subprocess.
 *
 * The SusySubprocessFinder provides that:
 *  o it finds the two initial SUSY particles in a SUSY event,
 *  o finds the appropriate subprocess code,
 *  o at the end of the run prints two tables showing for each subprocess
 *    the number of events generated and the number of events passing the filter
 *
 * Usage:
 *  o Make two instances of the SusySubprocessFinder
 *  o Call then e.g. "beforeFilter" and "afterFilter", and let the attribute "BeforeFilter"
 *    be set to True and False, respectively
 *  o Instead of having the filter directly in topAlg, as is the usual way,
 *    put it in an AthSequence, with "beforeFilter" and "afterFilter" placed before
 *    and after it the generation.
 *
 * The two SusySubprocessFinders always accept the events: they are only filters in order
 * to do printing. Hence: The "beforeFilter" will always be run, as will the original filter,
 * but the "AfterFilter" will only be run if the original filter accepts the event.
 *
 * Example:
 *
 * ===== 1) Add the lines below:
 *
 * from GeneratorFilters.GeneratorFiltersConf import SusySubprocessFinder
 * from AthenaCommon.AlgSequence import AthSequencer
 *
 * topAlg += AthSequencer("FilterSeq")
 *
 * before = SusySubprocessFinder("BeforeFilter")
 * before.BeforeFilter = True
 * after = SusySubprocessFinder("AfterFilter")
 * after.BeforeFilter = False
 *
 * topAlg.FilterSeq += before
 *
 *
 * ===== 2) Then replace e.g.
 *
 *  topAlg += MultiElecMuTauFilter()
 *
 * by
 *
 *  topAlg.FilterSeq += MultiElecMuTauFilter()
 *  topAlg.FilterSeq += after
 *
 * @author Borge Kile Gjelsten <gjelsten@cern.ch>
 *
 **/
class SusySubprocessFinder : public GenFilter {
public:

  SusySubprocessFinder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode filterInitialize();
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();

  virtual bool isSUSY(int);
  virtual int subProcessFromPdgids(int,int);
  virtual std::string subProcessText(int);
  virtual void findInitialSusy(int[],int);

  virtual void verboseMC();

  /// @todo Can we make this better?
  static const int nSubprocMax = 250;

private:

  bool m_beforeFilter;
  int m_findmode;
  int nSubproc[nSubprocMax];
  std::string beforeORafter;

  bool m_verbose;
  std::string m_verbose_mode;
  int m_verbose_nPart;

};


#endif
