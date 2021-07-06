/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENALGS_MDTALIGNMODULE_H
#define MUONALIGNGENALGS_MDTALIGNMODULE_H

#include <iostream>
#include <vector>

#include "MuonAlignEvent/CombinedMuonAlignModule.h"
#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignPar.h"

/**
   @file MdtAlignModule.h
   @class MdtAlignModule

   @brief MdtAlignModule is a grouping of MdtReadoutElements, grouped according to the type of alignment.
   MDT alignment uses the following:
   1) L1 alignment uses an AlignModule for each endcap and one for the barrel for a total of 3 AlignModule elements in the AlignModuleList.
   2) L3 alignment uses a single MDT chamber for each AlignModule.
   3) L2 and other types not yet implemented for MDT alignment.
   See detector-specific implementation of IGeometryManagerTool::createAlignModuleList() to see how implemented.

   @author Robert Harrington <roberth@bu.edu>
   @date 10/14/2008
*/

class TFile;
class TTree;

namespace MuonGM {
    class MdtReadoutElement;
}

namespace Muon {

    class MdtAlignModule : public CombinedMuonAlignModule {
    public:
        /** Constructor using AlgTool gives a MsgStream with MdtAlignModule for name
            and the the same output level as the AlgTool. */
        MdtAlignModule(const AlgTool* algtool, const Amg::Transform3D& transform = Amg::Transform3D::Identity());

        /** This constructor gives a MsgStream with the name of the tool that created
            the MdtAlignModule and the same output level. */
        MdtAlignModule(MsgStream* log, const Amg::Transform3D& transform = Amg::Transform3D::Identity());

        virtual ~MdtAlignModule();

        /** tree in m_rootfile used to store positions of tubes in chambers */
        TFile* getRootFile() { return m_rootfile; }
        TTree* getTubePositionTree() { return m_tubePositionTree; }
        void setRootFile(TFile* file) { m_rootfile = file; }
        void setTubePositionTree(TTree* tree) { m_tubePositionTree = tree; }

        // void   shiftSurface(Trk::TrkDetElementBase* det, Identifier tubeId) const;
        // void   restoreSurfaces(Trk::TrkDetElementBase* mre) const;

    private:
        /** tree used to store positions of tubes for drawing hits and troubleshooting */
        TFile* m_rootfile;
        TTree* m_tubePositionTree;

        MsgStream* m_log;
        MdtAlignModule& operator=(const MdtAlignModule& right);
        MdtAlignModule(const MdtAlignModule&);

    };  // end class

}  // namespace Muon

#endif  // MUONALIGNGENALGS_MDTALIGNMODULE_H
