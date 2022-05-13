// Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#ifndef HTTROADUNIONTOOL_H
#define HTTROADUNIONTOOL_H

/**
 * @file HTTRoadUnionTool.h
 * @author Riley Xu - riley.xu@cern.ch
 * @date November 20th, 2020
 * @brief Wrapper class to combine multiple road-finding tools
 *
 * Declarations in this file:
 *      class HTTRoadUnionTool : public AthAlgTool, virtual public IHTTRoadFinderTool
 */

#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigHTTHough/IHTTRoadFinderTool.h"


// This class is merely a lightweight wrapper around multiple road finder tools.
class HTTRoadUnionTool : public extends <AthAlgTool, IHTTRoadFinderTool>
{
    public:

        ///////////////////////////////////////////////////////////////////////
        // AthAlgTool

        HTTRoadUnionTool(const std::string&, const std::string&, const IInterface*);

        virtual StatusCode initialize() override;

        ///////////////////////////////////////////////////////////////////////
        // IHTTRoadFinderTool

        virtual StatusCode getRoads(const std::vector<const HTTHit*> & hits, std::vector<HTTRoad*> & roads) override;

        ///////////////////////////////////////////////////////////////////////
        // HTTRoadUnionTool

        ToolHandleArray<IHTTRoadFinderTool> const & tools() const { return m_tools; }

    private:

        ///////////////////////////////////////////////////////////////////////
        // Handles

        ToolHandleArray<IHTTRoadFinderTool> m_tools;
};

#endif
