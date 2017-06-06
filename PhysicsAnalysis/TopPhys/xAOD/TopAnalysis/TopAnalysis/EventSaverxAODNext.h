/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSAVERXAODNEXT_H_
#define EVENTSAVERXAODNEXT_H_

#include "TopAnalysis/EventSaverBase.h"

// Framework include(s):
#include "AsgTools/AsgMetadataTool.h"

class TFile;

namespace top {

  /**
  * @brief Write out the selected events in xAOD format.  
  * Next generation xAOD output - still in development
  */
  class EventSaverxAODNext : public top::EventSaverBase , public asg::AsgMetadataTool {
  
    public:

      EventSaverxAODNext();

      virtual ~EventSaverxAODNext();

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
    

      virtual void saveEventToxAOD();

      ///xAOD needs to write some more stuff to the file at the end of a job
      virtual void finalize();

    private:
      ///We need access to the configuration file to get the container names.
      std::shared_ptr<top::TopConfig> m_config;

      ///We need to hold on to the output file
      TFile* m_outputFile;
      
      bool m_saveAllObjects;
      
      // helper typedef
      typedef std::map<std::size_t,std::map<unsigned int,unsigned int>> ThinningMap_t;
      typedef ThinningMap_t::const_iterator ThinningMap_Itr; 
      
      std::shared_ptr<ThinningMap_t> savePhotons(const bool saveEventObjects);      
      std::shared_ptr<ThinningMap_t> saveElectrons(const bool saveEventObjects);
      std::shared_ptr<ThinningMap_t> saveMuons(const bool saveEventObjects);      
      std::shared_ptr<ThinningMap_t> saveTaus(const bool saveEventObjects);
      std::shared_ptr<ThinningMap_t> saveJets(const bool saveEventObjects);      
      std::shared_ptr<ThinningMap_t> saveLargeRJets(const bool saveEventObjects); 
      std::shared_ptr<ThinningMap_t> saveTrackJets(const bool saveEventObjects);
      
      std::vector<unsigned int> thinObjectSelection(const std::size_t hashValue , 
                                                    const std::vector<unsigned int>& objectList , 
                                                    const std::shared_ptr<ThinningMap_t> thinningMap) const;
      

      ClassDef(top::EventSaverxAODNext, 0);
  };

}

#endif
