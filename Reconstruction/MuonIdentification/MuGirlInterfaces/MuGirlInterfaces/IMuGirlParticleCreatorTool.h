/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuGirlParticleCreatorTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUGIRLNSMUGIRLPARTICLECREATORTOOLTOOL_H
#define IMUGIRLNSMUGIRLPARTICLECREATORTOOLTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuGirlInterfaces/CandidateSummary.h"


namespace MuGirlNS
{
static const InterfaceID IID_IMuGirlParticleCreatorTool("MuGirlNS::IMuGirlParticleCreatorTool", 1, 0);

class IMuGirlParticleCreatorTool : virtual public IAlgTool
{
public:
    static const InterfaceID& interfaceID( ) ;

    // enter declaration of your interface-defining member functions here
    virtual StatusCode fillContainer(CandidateSummaryList* SummaryList, bool onlyNTupleFill=false) = 0;

    virtual void removeRedundantCandidates(CandidateSummaryList* SummaryList, std::vector<bool>& IncludeList)=0;

};

inline const InterfaceID& MuGirlNS::IMuGirlParticleCreatorTool::interfaceID()
{
    return IID_IMuGirlParticleCreatorTool;
}

} // end of namespace

#endif
