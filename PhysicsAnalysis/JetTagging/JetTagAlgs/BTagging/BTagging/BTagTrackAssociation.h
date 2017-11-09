/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#ifndef BTAGGING_BTAGTRACKASSOCIATION_H
#define BTAGGING_BTAGTRACKASSOCIATION_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "BTagging/IBTagTrackAssociation.h"

#include <string>

/** The namespace of all packages in PhysicsAnalysis/JetTagging */
namespace Analysis
{
    class IBTagTrackAssociation;

    class BTagTrackAssociation : public AthAlgTool, virtual public IBTagTrackAssociation
    {
        public:
            /** Constructors and destructors */
            BTagTrackAssociation(const std::string&,const std::string&,const IInterface*); //NameType& name);
            virtual ~BTagTrackAssociation();

            virtual StatusCode initialize() override;
            virtual StatusCode finalize() override;
            virtual StatusCode BTagTrackAssociation_exec(jetcollection_t* theJets) const override;

        private:
            std::string m_AssociatedTrackLinks;
            std::string m_AssociatedMuonLinks;
            std::string m_TrackContainerName;
            std::string m_MuonContainerName;
            std::string m_TrackAssociationName;
            std::string m_MuonAssociationName;

    }; // End class

} // End namespace

#endif // BTAGTRACKASSOCIATION_H
