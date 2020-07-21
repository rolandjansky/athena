/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

// modified base on GhostTrackSystematicsMaker.h from Fabian Wilk
// Description:
// Author: Andrea Knue
// Created: June 2020


#ifndef _TOP_TRACKSYSTEMATICSMAKER_H_
#define _TOP_TRACKSYSTEMATICSMAKER_H_

#include <list>
#include <set>
#include <string>
#include <vector>
#include <utility>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "PATInterfaces/SystematicSet.h"

#include "xAODTracking/TrackParticleContainer.h"


// Forward Declarations.
namespace InDet {
  class InDetTrackSystematicsTool;
  class InDetTrackSmearingTool;
  class InDetTrackBiasingTool;
  class InDetTrackTruthFilterTool;
  class InDetTrackTruthOriginTool;
}

namespace top {
  class TopConfig;
}

namespace top {

  class TrackSystematicsMaker final: public asg::AsgTool {
  public:
    explicit TrackSystematicsMaker(const std::string& name);
    virtual ~TrackSystematicsMaker();

    TrackSystematicsMaker(const TrackSystematicsMaker& rhs) = delete;
    TrackSystematicsMaker(TrackSystematicsMaker&& rhs) = delete;
    TrackSystematicsMaker& operator = (const TrackSystematicsMaker& rhs) = delete;

    StatusCode initialize();
    StatusCode execute(bool);

    /*
     * @brief Retrieve the specified (requested) systematics list.*
     *
     *  Unless systematics have been requested through the
     *  specifiedSystematics setter function, this will be an empty list.
     */
    inline const std::list<CP::SystematicSet>& specifiedSystematics() const;

    /*
     * @brief Retrieve the recommended systematics list.
     *
     *  This will return a meaningless (probably empty) list unless the
     *  setter function specifiedSystematics has been called or the tool has
     *  been initialized (which call specifiedSystematics using the data
     *  obtained from top::TopConfig).
     */
    inline const std::list<CP::SystematicSet>& recommendedSystematics() const;

    /*!
     * @brief Specify the systematics to be used.
     * @param specifiedSystematics Set of systematics (by name) to be used;
     *  an empty set will result in no systematic variation.
     */
    virtual void specifiedSystematics(const std::set<std::string>& specifiedSystematics);

  protected:


    StatusCode applyTruthFilterSystematic(InDet::InDetTrackTruthFilterTool* tool,
                                          const CP::SystematicSet& syst) const;

    StatusCode applySmearingSystematic(InDet::InDetTrackSmearingTool* tool,
                                       const CP::SystematicSet& syst) const;

    StatusCode applyBiasingSystematic(InDet::InDetTrackBiasingTool* tool,
                                      const CP::SystematicSet& syst) const;

  private:
    StatusCode retrieveTrackCPTool();
  private:
    std::shared_ptr<top::TopConfig> m_config;
    std::vector<std::uint32_t> m_runPeriods;

    std::list<CP::SystematicSet> m_specifiedSystematics;
    std::list<CP::SystematicSet> m_recommendedSystematics;

    CP::SystematicSet m_nominalSystematicSet;



    struct {
      void makeUnique() {
        static auto exec = [](std::vector<CP::SystematicSet>& data) {
                             std::sort(data.begin(), data.end());
                             data.erase(std::unique(data.begin(), data.end()),
                                        data.end());
                           };

        exec(smearing);
        exec(truthFilter);
        exec(bias);
      }

      void removeNonSpecified(const std::list<CP::SystematicSet>& specified) {
        auto fnCmp = [&specified](CP::SystematicSet& s) {
                       return std::find(specified.begin(),
                                        specified.end(),
                                        s) == specified.end();
                     };

        auto exec = [&specified, &fnCmp](std::vector<CP::SystematicSet>& data) {
                      data.erase(std::remove_if(data.begin(), data.end(), fnCmp),
                                 data.end());
                    };

        exec(smearing);
        exec(truthFilter);
        exec(bias);
      }

      std::size_t numSystematics() const {
        return smearing.size() + truthFilter.size() + bias.size();
      }

      std::vector<CP::SystematicSet> smearing, truthFilter, bias;
    } m_systs;

    ToolHandle<InDet::InDetTrackSmearingTool> m_smearingTool;
    std::vector<ToolHandle<InDet::InDetTrackBiasingTool> > m_biasTool;
    ToolHandle<InDet::InDetTrackTruthOriginTool> m_truthOriginTool;
    ToolHandle<InDet::InDetTrackTruthFilterTool> m_truthFilterTool;

  };

}

#endif /* _TOP_TRACKSYSTEMATICSMAKER_H_ */
