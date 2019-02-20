/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Filename: GhostTrackSystematicsMaker.h
// Description:
// Author: Fabian Wilk
// Created: Mon Oct 24 16:54:17 2016


#ifndef _TOP_GHOSTTRACKSYSTEMATICSMAKER_H_
#define _TOP_GHOSTTRACKSYSTEMATICSMAKER_H_

#include <list>
#include <set>
#include <string>
#include <vector>
#include <utility>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"

#include "PATInterfaces/SystematicSet.h"

#include "xAODJet/JetContainer.h"


// Forward Declarations.
namespace InDet {
    class InDetTrackSystematicsTool;
    class InDetTrackSmearingTool;
    class InDetTrackBiasingTool;
    class InDetTrackTruthFilterTool;
    class InDetTrackTruthOriginTool;
    class JetTrackFilterTool;
}

namespace top {
    class TopConfig;
}

namespace top {

    /*
     * @brief Tool for making systematics that affect ghost-associated tracks of
     *  jets available.
     *
     *  Ghost-associated tracks are available on jets through the
     *  getAssociatedObjects function which retrieves a
     *
     *      std::vector<const xAOD::IParticle*>
     *
     *  container that is decorated onto each jet object.
     *
     *  This tool, produces one copy of this container for each of the affecting
     *  and requested systematics. Some systematics change only the number of
     *  tracks (efficiency-style systematics) while other only smear (some) of
     *  the track particle properties.
     *
     *  The requested systematics are retrieved from top::TopConfig, filtered to
     *  only contain those affecting the ghost tracks, and propagated back to
     *  the top::TopConfig instance so that they become available to any tool
     *  which wishes to query
     *
     *      - the affecting systematics,
     *      - the decoration name used for the varied ghost track container
     *
     *  Generally, a tool should use the function
     *
     *      top::TopConfig::systMapJetGhostTrack()
     *
     *  to retrieve the mapping from the hash value of a systematic to the
     *  CP::SystematicSet of that variation for all (configured and requested)
     *  systematics that affect the ghost tracks (this map includes the nominal
     *  systematic). The decoration name can then be retrieved using
     *
     *      top::TopConfig::decoKeyJetGhostTrack(std::size_t hash)
     */
    class GhostTrackSystematicsMaker final : public asg::AsgTool {
    public:
        explicit GhostTrackSystematicsMaker(const std::string & name);
        virtual ~GhostTrackSystematicsMaker();

        GhostTrackSystematicsMaker(const GhostTrackSystematicsMaker & rhs) = delete;
        GhostTrackSystematicsMaker(GhostTrackSystematicsMaker && rhs) = delete;
        GhostTrackSystematicsMaker & operator=(const GhostTrackSystematicsMaker & rhs) = delete;

        StatusCode initialize();
        StatusCode execute(bool);

        /*
         * @brief Retrieve the specified (requested) systematics list.*
         *
         *  Unless systematics have been requested through the
         *  specifiedSystematics setter function, this will be an empty list.
         */
        inline virtual const std::list<CP::SystematicSet> & specifiedSystematics() const;

        /*
         * @brief Retrieve the recommended systematics list.
         *
         *  This will return a meaningless (probably empty) list unless the
         *  setter function specifiedSystematics has been called or the tool has
         *  been initialized (which call specifiedSystematics using the data
         *  obtained from top::TopConfig).
         */
        inline const std::list<CP::SystematicSet> & recommendedSystematics() const;

        /*!
         * @brief Specify the systematics to be used.
         * @param specifiedSystematics Set of systematics (by name) to be used;
         *  an empty set will result in no systematic variation.
         */
        virtual void specifiedSystematics(const std::set<std::string> & specifiedSystematics);

    protected:
        StatusCode applyNoOpSystematic(xAOD::JetContainer * nominal,
                                       const CP::SystematicSet & syst) const;

        StatusCode applyTruthFilterSystematic(xAOD::JetContainer * nominal,
                                              InDet::InDetTrackTruthFilterTool * tool,
                                              const CP::SystematicSet & syst) const;

        StatusCode applySmearingSystematic(xAOD::JetContainer * nominal,
                                           InDet::InDetTrackSmearingTool * tool,
                                           const CP::SystematicSet & syst) const;

        StatusCode applyBiasingSystematic(xAOD::JetContainer * nominal,
                                          InDet::InDetTrackBiasingTool * tool,
                                          const CP::SystematicSet & syst) const;

        StatusCode applyJetTrackFilterSystematic(xAOD::JetContainer * nominal,
                                                 InDet::JetTrackFilterTool * tool,
                                                 const CP::SystematicSet & syst) const;

    private:
        StatusCode retrieveGhostTrackCPTool();

    private:
        std::shared_ptr<top::TopConfig> m_config;
	double m_jetPtCut,m_jetEtaCut;
        std::vector<std::uint32_t> m_runPeriods;

        std::list<CP::SystematicSet> m_specifiedSystematics;
        std::list<CP::SystematicSet> m_recommendedSystematics;

        CP::SystematicSet m_nominalSystematicSet;

	

        struct {
            void makeUnique(){
                static auto exec = [](std::vector<CP::SystematicSet> & data){
                    std::sort(data.begin(), data.end());
                    data.erase(std::unique(data.begin(), data.end()),
                               data.end());
                };

                exec(smearing);
                exec(truthFilter);
                exec(bias);
                exec(jetTrackFilter);
            }

            void removeNonSpecified(const std::list<CP::SystematicSet> & specified){
                auto fnCmp = [&specified](CP::SystematicSet & s){
                    return std::find(specified.begin(),
                                     specified.end(),
                                     s) == specified.end();
                };

                auto exec = [&specified, &fnCmp](std::vector<CP::SystematicSet> & data){
                    data.erase(std::remove_if(data.begin(), data.end(), fnCmp),
                               data.end());
                };

                exec(smearing);
                exec(truthFilter);
                exec(bias);
                exec(jetTrackFilter);
            }

            std::size_t numSystematics() const {
                return smearing.size() + truthFilter.size() + bias.size() + jetTrackFilter.size();
            }

            std::vector<CP::SystematicSet> smearing, truthFilter, bias, jetTrackFilter;
        } m_systs;

        struct {
            ToolHandle<InDet::InDetTrackSmearingTool> smearing;
            ToolHandle<InDet::InDetTrackTruthOriginTool> truthOrigin;
            ToolHandle<InDet::InDetTrackTruthFilterTool> truthFilter;
            ToolHandle<InDet::JetTrackFilterTool> jetTrackFilter;

            std::vector<ToolHandle<InDet::InDetTrackBiasingTool> > bias;
        } m_tools;
    };

    /* inline virtual */ const std::list<CP::SystematicSet> & GhostTrackSystematicsMaker::specifiedSystematics() const {
        return m_specifiedSystematics;

    }
    /* inline */ const std::list<CP::SystematicSet> & GhostTrackSystematicsMaker::recommendedSystematics() const {
        return m_recommendedSystematics;
    }


}

#endif /* _TOP_GHOSTTRACKSYSTEMATICSMAKER_H_ */
