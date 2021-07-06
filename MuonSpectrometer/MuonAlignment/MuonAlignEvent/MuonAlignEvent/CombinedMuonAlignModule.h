/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENALGS_COMBINEDMUONALIGNMODULE_H
#define MUONALIGNGENALGS_COMBINEDMUONALIGNMODULE_H

#include <iostream>
#include <vector>

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignPar.h"

/**
   @file CombinedMuonAlignModule.h
   @class CombinedMuonAlignModule

   @brief CombinedMuonAlignModule is a grouping of MdtReadoutElements,
   TgcReadoutElements, and RpcReadoutElements, grouped according to the type of alignment.
   @author Robert Harrington <roberth@bu.edu>
   @date 06/09/2009
*/

namespace MuonGM {
    class MdtReadoutElement;
    class TgcReadoutElement;
    class RpcReadoutElement;
}  // namespace MuonGM

namespace Muon {

    class CombinedMuonAlignModule : public Trk::AlignModule {
    public:
        /** Constructor using AlgTool gives a MsgStream with CombinedMuonAlignModule for name
            and the the same output level as the AlgTool. */
        CombinedMuonAlignModule(const AlgTool* algtool, const Amg::Transform3D& transform = Amg::Transform3D::Identity());

        /** This constructor gives a MsgStream with the name of the tool that created
            the CombinedMuonAlignModule and the same output level. */
        CombinedMuonAlignModule(MsgStream* log, const Amg::Transform3D& transform = Amg::Transform3D::Identity());

        virtual ~CombinedMuonAlignModule();

        virtual void shiftSurface(Trk::TrkDetElementBase* det, Identifier tubeId) const;
        virtual void restoreSurfaces(Trk::TrkDetElementBase* mre) const;

    private:
        MsgStream* m_log;
        CombinedMuonAlignModule& operator=(const CombinedMuonAlignModule& right);
        CombinedMuonAlignModule(const CombinedMuonAlignModule&);

    };  // end class

}  // namespace Muon

#endif  // MUONALIGNGENALGS_COMBINEDMUONALIGNMODULE_H
