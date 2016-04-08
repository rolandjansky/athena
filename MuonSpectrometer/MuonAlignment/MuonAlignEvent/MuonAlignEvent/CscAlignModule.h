/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENALGS_CSCALIGNMODULE_H
#define MUONALIGNGENALGS_CSCALIGNMODULE_H

#include "TrkAlignEvent/AlignModule.h"
#include "TrkAlignEvent/AlignPar.h"
#include "MuonAlignEvent/CombinedMuonAlignModule.h"

#include <iostream>
#include <vector>

/**
   @file CscAlignModule.h
   @class CscAlignModule

   @brief CscAlignModule is a grouping of CscReadoutElements, grouped according to the type of alignment.
   The code has been modified from "MdtAlignModule"

   @author Prolay Mal <prolay@physics.arizona.edu>
   @date September 7, 2009
*/

class TFile;
class TTree;

namespace MuonGM {
  class CscReadoutElement;
}

namespace Muon {
  
  class CscAlignModule : public CombinedMuonAlignModule {
    
  public:
    
    /** Constructor using AlgTool gives a MsgStream with CscAlignModule for name 
	and the the same output level as the AlgTool. */
    CscAlignModule(const AlgTool* algtool, 
		   const Amg::Transform3D& transform=Amg::Transform3D::Identity());

    /** This constructor gives a MsgStream with the name of the tool that created 
	the CscAlignModule and the same output level. */
    CscAlignModule(MsgStream* log,
		   const Amg::Transform3D& transform=Amg::Transform3D::Identity());

    virtual ~CscAlignModule();
    
    /** tree in m_rootfile used to store positions of tubes in chambers */
    TFile* getRootFile()         { return m_rootfile; }
    TTree* getTubePositionTree() { return m_tubePositionTree; }
    void   setRootFile(TFile* file)         { m_rootfile=file; }
    void   setTubePositionTree(TTree* tree) { m_tubePositionTree=tree; }
    
    //void   shiftSurface(Trk::TrkDetElementBase* det, Identifier tubeId) const;
    //void   restoreSurfaces(Trk::TrkDetElementBase* mre) const;
    
  private:

    
    /** tree used to store positions of tubes for drawing hits and troubleshooting */
    TFile*         m_rootfile;
    TTree*         m_tubePositionTree;

    MsgStream*     m_log;
    CscAlignModule & operator=(const CscAlignModule &right);
    CscAlignModule(const CscAlignModule&);

  }; // end class
  
} // end namespace

#endif // MUONALIGNGENALGS_CSCALIGNMODULE_H
