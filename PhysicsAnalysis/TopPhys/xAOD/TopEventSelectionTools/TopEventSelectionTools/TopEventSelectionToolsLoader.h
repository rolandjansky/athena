/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSELECTIONTOOLLOADER_H_
#define EVENTSELECTIONTOOLLOADER_H_

#include "TopEventSelectionTools/ToolLoaderBase.h"

namespace top {

/**
 * @brief This is used to load tools in this package in a factory like way.  It
 * has one important function.  After that has been called, you can dispose of
 * this class.
 *
 * A few tricky aspects are:
 * 1) Don't use C++11 in this because of the ClassDef and Root5.  In Root6
 * you'll be glad to hear that it's fine but we're not there yet.
 * 2) Don't put any xAOD headers in here.  Same reason.
 */
class TopEventSelectionToolsLoader : public ToolLoaderBase {
public:
    /**
     * @brief Convert the string name into a tool-object.
     *
     * If the tool is not found by the function then it will print an error
     * message and terminate the program execution.  No point running the whole
     * analysis if you made a typo in the cuts.
     *
     * Any new tools that you write will have to be added to this function so
     * it knows how to correctly load them.  The order is a bit important, because
     * it's done by string matching.  So MET+MWT has to come before MET, for
     * example.
     *
     * @param name The name of the event selection used when saving histograms.
     * @param line This should be a line of text like "JET_N 25000 >= 2".  The
     * exact format of the line depends on which tool you are trying to setup.
     * @param outputFile Some of the tools (histograms for example) need access
     * to the output file.
     * @param wk Used by EventLoop, try not to worry about it too much.
     *
     * @return Always a valid, configured tool or a nullptr if a tool matching
     * the name was not found (since this might not be the only tool loader in
     * the program).
     */
    top::EventSelectorBase* initTool(const std::string& name, const std::string& line, TFile* outputFile, std::shared_ptr<top::TopConfig> config,EL::Worker* wk = nullptr);

    ClassDef(top::TopEventSelectionToolsLoader, 0)
};

}

#endif
