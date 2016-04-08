/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENALGS_TGCALIGNMODULE_H
#define MUONALIGNGENALGS_TGCALIGNMODULE_H

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignPar.h"
#include "MuonAlignEvent/CombinedMuonAlignModule.h"

#include <iostream>
#include <vector>

/**
   @file TgcAlignModule.h
   @class TgcAlignModule

   @brief TgcAlignModule is a grouping of TgcReadoutElements, grouped according to the type of alignment.
   TGC alignment uses the following:
   1) L3 alignment uses a single TGC chamber for each AlignModule.
   3) L2, L3 and other types not yet implemented for TGC alignment. T1 might be a wheel, for example.
   See detector-specific implementation of IGeometryManagerTool::createAlignModuleList() to see how implemented.

   @author Yair Mahalalel <Yair.Mahalalel@cern.ch>
   @date 10/14/2008
*/

class TFile;
class TTree;

namespace MuonGM {
  class TgcReadoutElement;
}

namespace Muon {
  
  class TgcAlignModule : public CombinedMuonAlignModule {
    
  public:
    
    /** Constructor using AlgTool gives a MsgStream with TgcAlignModule for name and the the same output level as the AlgTool. */
    TgcAlignModule(const AlgTool* algtool,
		   const Amg::Transform3D& transform=Amg::Transform3D::Identity());

    /** This constructor gives a MsgStream with the name of the tool that created the TgcAlignModule and the same output level. */
    TgcAlignModule(MsgStream* log, 
		   const Amg::Transform3D& transform=Amg::Transform3D::Identity());

    virtual ~TgcAlignModule();
    
    /** trees in m_rootfile used to store positions of strips and wires in chambers */
    TFile* getRootFile()         { return m_rootfile; }
    TTree* getStripPositionTree() { return m_stripPositionTree; }
    TTree* getWirePositionTree()  { return m_wirePositionTree; }
    void   setRootFile(TFile* file)         { m_rootfile=file; }
    void   setStripPositionTree(TTree* tree) { m_stripPositionTree=tree; }
    void   setWirePositionTree(TTree* tree)  { m_wirePositionTree=tree; }

    //void   shiftSurface(Trk::TrkDetElementBase* det, Identifier id) const;
    //void   restoreSurfaces(Trk::TrkDetElementBase* tre) const;

  private:

    /** trees used to store positions of stripts and wires for drawing hits and troubleshooting */
    TFile*         m_rootfile;
    TTree*         m_stripPositionTree;
    TTree*         m_wirePositionTree;

    MsgStream*     m_log;
    TgcAlignModule & operator=(const TgcAlignModule &right);
    TgcAlignModule(const TgcAlignModule&);

  }; // end class
  
} // end namespace

#endif // MUONALIGNGENALGS_TGCALIGNMODULE_H
