/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuGirlParticleCreatorTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUGIRLNSMUGIRLPARTICLECREATORTOOL_H
#define MUGIRLNSMUGIRLPARTICLECREATORTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/INTupleSvc.h"
#include "MuGirlInterfaces/IMuGirlParticleCreatorTool.h"
#include "MuGirlInterfaces/IPerformanceTruthTool.h"
//#include "IsolationTool/IIsolationTool.h"
#include "MuidInterfaces/IMuidCaloEnergy.h"

namespace Rec
{
class IMuonTrackQuery;
}

namespace NTuple
{
class Tuple;
}

class IIsolationTool;

namespace MuGirlNS
{
class CandidateSummary;
class ParticleCreatorNTuple;

class MuGirlParticleCreatorTool : virtual public IMuGirlParticleCreatorTool, public AthAlgTool
{
public:
    MuGirlParticleCreatorTool(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~MuGirlParticleCreatorTool ();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  ();

    virtual StatusCode fillContainer(CandidateSummaryList* SummaryList, bool onlyNTupleFill=false);

    virtual void removeRedundantCandidates(CandidateSummaryList* SummaryList, std::vector<bool>& IncludeList);

private:

    StringProperty               m_muGirlCombinedMuonLocation;
    StringProperty               m_inDetParticlesLocation;
    StringProperty               m_inExtrpParticlesLocation;
    StringProperty               m_msRefitParticlesLocation;
    //ToolHandle<IIsolationTool>   m_muonIsolationTool;
    ToolHandle<Rec::IMuidCaloEnergy> m_pCaloEnergyTool;
    //ToolHandle<Rec::IMuonIdentificationHelper>  m_helper;
    ToolHandle<Rec::IMuonTrackQuery>            m_trackQuery;
    ToolHandle<IPerformanceTruthTool> m_pTruthTool;
    INTupleSvc*                  m_pNTupleSvc;
    BooleanProperty              m_doTruth;
    BooleanProperty              m_doFill;
    BooleanProperty              m_doNTuple;
    BooleanProperty              m_doMSRefit; 
    StringProperty               m_sOverlap;
    FILE*                        m_fOverlap;
    NTuple::Tuple*               m_pRawNTuple;
    ParticleCreatorNTuple*       m_pNTuple;

    template <class T>
    StatusCode retrieve(ToolHandle<T>& pHandle, bool bError = true)
    {
        StatusCode sc = pHandle.retrieve();
        if (sc.isFailure())
        {
            if (bError)
                msg(MSG::ERROR) << "Cannot retrieve " << pHandle << endreq;
            else
                msg(MSG::WARNING) << "Cannot retrieve " << pHandle << endreq;
        }
        else
            msg(MSG::INFO) << "Retrieved " << pHandle << endreq;
        return sc;
    }

    //    void removeRedundantCandidates(CandidateSummaryList* SummaryList, std::vector<bool>& IncludeList);
};


} // end of namespace
#endif
