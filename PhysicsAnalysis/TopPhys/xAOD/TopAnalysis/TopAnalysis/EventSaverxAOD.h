/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSAVERXAOD_H_
#define EVENTSAVERXAOD_H_

#include "TopAnalysis/EventSaverBase.h"

// Framework include(s):
#include "AsgTools/AsgTool.h"

class TFile;

namespace top {

/**
 * @brief Write out the selected events in xAOD format.  At some point this will
 * be the future.  But probably not right now.  Still too many missing features.
 */
class EventSaverxAOD : public top::EventSaverBase , public asg::AsgTool {
  
public:
    /**
     * @brief Kept simple, set the output container prefix (so they have a
     * different name to the input containers).
     */
    EventSaverxAOD();

    virtual ~EventSaverxAOD();

    /**
     * @brief This does some configuration of the output file.  It also keeps track
     * of which branches should be saved.
     *
     * You surely don't need all the branches, but if you do remove the
     * corresponding line.
     *
     * We can't do much in the constructor, like I would like, because we're
     * letting root make this.  So instead we have a function here.
     *
     * @param config The top::TopConfig object that contains lots of settings and
     * stuff.
     * @param file The output TFile that you want to save things to.
     * @param extraBranches List of extra branches that (currently) are attached to
     * EventInfo.  e.g. the branch that tells you if an event passed a certain
     * selection.  If you add it here, it'll be written to the output file.
     */
    virtual void initialize(std::shared_ptr<top::TopConfig> config, TFile* file, const std::vector<std::string>& extraBranches);
    
    //Keep the asg::AsgTool happy
    virtual StatusCode initialize(){return StatusCode::SUCCESS;}
    
    /**
     * @brief Save an event.
     *
     * Why am I only writing out the nominal tree.
     *
     * I thought the fill for the xaod would allow me to specify the tree name,
     * but it seems I can only do that in the writeTo.  Maybe their is a clever
     * way around it, but I'm too tired of this.
     *
     * Why not save shallow copies?  This would be nice.  But aren't shallow
     * copies unsorted?  We'd have to remember to sort them.
     *
     * It looks like branch removal from things in containers (jets, leptons) is
     * still not working.  So the xAOD files are still huge.
     *
     * @param event The top event for this particular systematic variation.
     * @param xaodEvent The xAOD event object, so we can actually save.
     */
    virtual void saveEvent(const top::Event& event);

    ///xAOD needs to write some more stuff to the file at the end of a job
    virtual void finalize();

private:
    ///Name to prepend to the output containers
    const std::string m_prefix;

    ///We need access to the configuration file to get the container names.
    std::shared_ptr<top::TopConfig> m_config;

    ///We need to hold on to the output file
    TFile* m_outputFile;

    ClassDef(top::EventSaverxAOD, 0);
};

}

#endif
