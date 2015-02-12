/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           SecVtxInfo.cxx  -  Description
                             -------------------
    begin   : Spring 2004
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch
    changes :

 ***************************************************************************/

#include "JetTagInfo/SecVtxInfo.h"

namespace Analysis
{

/** Default constructor */
SecVtxInfo::SecVtxInfo() : BaseTagInfo(),
        m_numSelTracksForFit(-1),
        m_dist(-1.),
        m_rphidist(-1.),
        m_prob(-1.),
        m_mass(-1.),
        m_energyFraction(-1.),
        m_fittedTrks(std::vector<const Rec::TrackParticle*>()),
        m_mult(-1),
        m_fitType(NoFit),
        m_secVtxPos(Trk::RecVertex()),
        m_NGood2TrackVertices(-1)
      {
        m_fittedTrks.clear();
      }

/** constructor with infotype */
SecVtxInfo::SecVtxInfo(TagInfoType tagJetInfoType) : BaseTagInfo(tagJetInfoType),
        m_numSelTracksForFit(-1),
        m_dist(-1.),
        m_rphidist(-1.),
        m_prob(-1.),
        m_mass(-1.),
        m_energyFraction(-1.),
        m_fittedTrks(std::vector<const Rec::TrackParticle*>()),
        m_mult(-1),
        m_fitType(NoFit),
        m_secVtxPos(Trk::RecVertex()),
        m_NGood2TrackVertices(-1)
      {
        m_fittedTrks.clear();
      }

/** Copy constructor */
SecVtxInfo::SecVtxInfo(const SecVtxInfo& rhs) : BaseTagInfo(rhs),
        m_numSelTracksForFit(rhs.m_numSelTracksForFit),
        m_dist(rhs.m_dist),
        m_rphidist(rhs.m_rphidist),
        m_prob(rhs.m_prob),
        m_mass(rhs.m_mass),
        m_energyFraction(rhs.m_energyFraction),
        m_fittedTrks(rhs.m_fittedTrks), // copies pointers only (tracks are in SG!)
        m_mult(rhs.m_mult),
        m_fitType(rhs.m_fitType),
        m_secVtxPos(rhs.m_secVtxPos),
        m_NGood2TrackVertices(rhs.m_NGood2TrackVertices)
     {}

/** assigenment operator */
SecVtxInfo& SecVtxInfo::operator= (const SecVtxInfo& rhs)
{
    if (this!=&rhs)
    {
        m_numSelTracksForFit=rhs.m_numSelTracksForFit;
        m_tagLikelihood=rhs.m_tagLikelihood;
        m_dist=rhs.m_dist;
        m_rphidist=rhs.m_rphidist;
        m_prob=rhs.m_prob;
        m_mass=rhs.m_mass;
        m_energyFraction=rhs.m_energyFraction;
        m_fittedTrks=rhs.m_fittedTrks; // copies pointers only (tracks are in SG!)
        m_mult=rhs.m_mult;
        m_fitType=rhs.m_fitType;
        m_secVtxPos=rhs.m_secVtxPos;
        m_NGood2TrackVertices=rhs.m_NGood2TrackVertices;
    }
    return *this;
}

/** Default destructor */
SecVtxInfo::~SecVtxInfo()
{}

}
