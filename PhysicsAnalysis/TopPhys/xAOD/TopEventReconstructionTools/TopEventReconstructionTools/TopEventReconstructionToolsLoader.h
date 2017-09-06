/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RECONSTRUCTIONTOOLLOADER_H_
#define RECONSTRUCTIONTOOLLOADER_H_

#include "TopEventSelectionTools/ToolLoaderBase.h"

namespace top {

/**
 * @brief Load a ttbar event reconstruction code based on the same.
 *
 * Names for reconstruction (rather than selection) should start with "RECO:"
 * so that we can tell them apart.
 *
 * Why is this a class? So that ROOT can load it at run-time!  The only
 * important function is the initTool one, after that has been called then this
 * class can be safely disposed of.
 *
 * Note that the name of this tool is the name of the package with the word
 * Loader after it.  That's important so the main code can find the tools that
 * this package provides!  For the same reason it inherits from
 * top::ToolLoaderBase.
 *
 */
class TopEventReconstructionToolsLoader : public ToolLoaderBase {
public:
    /**
     * @brief Some sort of factory that can create the tool you need given a
     * string.
     *
     * @param name The name of the tool to load, e.g. RECO:KLFITTERLJETS.
     * @param line Not used here yet, but could be used to send configuration options to the tool.
     * @param config The TopConfig object, which is needed by the KLFitterTool
     * @param outputFile Not used here, but could be used so the tool has direct access to the output file.
     * @param wk Not used, but needed by EventLoop for some of the selection tools.
     * @return A pointer to a fully initialised and ready to use tool.
     */
    top::EventSelectorBase* initTool(const std::string& name, const std::string& line, TFile* outputFile, std::shared_ptr<top::TopConfig> config,EL::Worker* wk = nullptr);

    ClassDef(top::TopEventReconstructionToolsLoader, 0)
};

}

#endif
