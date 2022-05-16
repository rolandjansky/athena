// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTPHIROADFILTERTOOL_H
#define HTTPHIROADFILTERTOOL_H

/**
 * @file HTTPhiRoadFilterTool.h
 * @author Elliot Lipeles - lipeles@cern.ch
 * @date March 28th, 2021
 * @brief Implements road filtering using eta module patterns
 *
 * Declarations in this file:
 *      class HTTPhiRoadFilterTool : public AthAlgTool, virtual public IHTTRoadFilterTool
 *
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTObjects/HTTTypes.h"
#include "TrigHTTObjects/HTTVectors.h"
#include "TrigHTTObjects/HTTRoad.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTTrackPars.h"
#include "TrigHTTHough/IHTTRoadFilterTool.h"

#include "TFile.h"

#include <string>
#include <vector>
#include <map>
#include <boost/dynamic_bitset_fwd.hpp>

class ITrigHTTBankSvc;
class ITrigHTTMappingSvc;


class HTTPhiRoadFilterTool : public extends <AthAlgTool, IHTTRoadFilterTool>
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTPhiRoadFilterTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;

        ///////////////////////////////////////////////////////////////////////
        // IHTTRoadFilterTool

        virtual StatusCode filterRoads(const std::vector<HTTRoad*> & prefilter_roads, std::vector<HTTRoad*> & postfilter_roads) override;

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles
        ServiceHandle<ITrigHTTMappingSvc> m_HTTMapping {this, "TrigHTTMappingSvc", "TrigHTTMappingSvc"};

        ///////////////////////////////////////////////////////////////////////
        // Properties

	Gaudi::Property <unsigned> m_threshold {this, "threshold", 0, "Minimum number of hit layers to fire a road"};
	Gaudi::Property <std::vector<float> > m_window {this, "window", {}, "Distance from nominal path to keep hit, list of length nLayers"};
	Gaudi::Property <float> m_ptscaling {this, "ptscaling", 0.0, "Add a pT dependent resolution to each resolution in window"};

  
        //////////////////////////////////////////////////////////////////////
        // Event Storage
        std::vector<HTTRoad> m_postfilter_roads;
  
        ///////////////////////////////////////////////////////////////////////
        // Convenience
        unsigned m_nLayers = 0U; // alias to m_HTTMapping->PlaneMap1stStage()->getNLogiLayers();
  
        ///////////////////////////////////////////////////////////////////////
        // Metadata and Monitoring

        unsigned m_event = 0;
        std::string m_name; // Gets the instance name from the full gaudi name

        ///////////////////////////////////////////////////////////////////////
        // Helpers
        HTTRoad_Hough buildRoad(HTTRoad* origr) const;
};


#endif // HTTPHIROADFILTERTOOL_H
