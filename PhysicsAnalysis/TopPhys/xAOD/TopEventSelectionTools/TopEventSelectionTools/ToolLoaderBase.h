/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOOLLOADERBASE_H_
#define TOOLLOADERBASE_H_

#include <string>
#include <memory>

#include "TClass.h"

class TFile;

namespace EL {
class Worker;
}

namespace top {
  class EventSelectorBase;
  class TopConfig;

/**
 * @brief This allows user libraries (and TopEventSelectionTools and
 * TopEventReconstructionTools) to supply event selection-like tools to the
 * main top-xaod executable.
 *
 * To add one of these to your own library you must inherit from this class and
 * implement initTool.  The source code for this (or the getting started twiki)
 * should act as a good reference.  Basically you want an if (blah == "mytool")
 * then new MyTool().
 */
class ToolLoaderBase {
public:
    ///Constructor doesn't do anything.
    ToolLoaderBase() {}

    ///Destructor doesn't do anything.
    virtual ~ToolLoaderBase() {}

    /**
     * @brief If you make a new library and you want it to contain some event
     * selection tools you must implement this method.
     *
     * This is the only important method for this class and is loaded at the
     * start of top-xaod when constructor the event selection.
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
    virtual top::EventSelectorBase* initTool(const std::string& name, const std::string& line, TFile* outputFile, std::shared_ptr<top::TopConfig> config,EL::Worker* wk = nullptr) = 0;

    ClassDef(top::ToolLoaderBase, 0);
};

}

#endif
