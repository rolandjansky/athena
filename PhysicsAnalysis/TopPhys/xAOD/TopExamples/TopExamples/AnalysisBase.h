/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISBASE_H_
#define ANALYSISBASE_H_

#include "TopExamples/ContainerNames.h"

class TFile;

namespace top {
class Event;

/**
 * @brief An interface so that the analysis codes look the same and can be
 * run by the mini-to-plots executable.
 */
class AnalysisBase {
public:
    /**
     * @brief This is run once only at the very start.  You might want to book
     * some histograms or something.
     */
    AnalysisBase() {
        //container names
      m_containerNames.eventInfoName ="EventInfo";
      m_containerNames.electronCollectionName = "CorrectedSelectedElectrons";
      m_containerNames.muonCollectionName = "CorrectedSelectedMuons";
      m_containerNames.jetCollectionName = "CorrectedSelectedAntiKt4EMTopoJets";
      m_containerNames.largeJetCollectionName = "None";
      m_containerNames.tauCollectionName = "None";
      m_containerNames.truthCollectionName = "None";
      m_containerNames.metName = "MET_Reference_AntiKt4EMTopo";
    }

    /**
     * @brief Do nothing.
     */
    virtual ~AnalysisBase() {}

    /**
     * @brief Perform some task only when a new file is opened.
     *
     * For example, in the current system we save the cutflow that was generated
     * by the grid job in the file.  Every time we open a new file we want to
     * add this to the cutflow from the previous file.
     *
     * Don't try to delete the file - maybe we should change this to a reference?
     */
    virtual void newFile(TFile*) = 0;

    /**
     * @brief Write the analysis code that needs to be run for every event in
     * this function.
     */
    virtual void event(const top::Event&) = 0;

    /**
     * @brief This is run once at the very end.  You might want to save your
     * histograms. And / or normalise them. Or something like that.  The file
     * will be opened for you at the end of the job to save your output to.
     */
    virtual void finalise(TFile*) = 0;

    const ContainerNames containerNames() const { return m_containerNames; }

    ContainerNames* SetContainerNames() { return &m_containerNames; }

private:
    top::ContainerNames m_containerNames;
};

}

#endif /* ANALYSISBASE_H_ */
