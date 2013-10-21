/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////
/// this class inherites from the automatically created
/// TrkValidationNtupleOutputTrack to have independent
/// interface to the ntuple
//////////////////////////////////////////////////////////

#ifndef TRACKDATANTUPLETREE_H
#define TRACKDATANTUPLETREE_H

#include "TrkValidationNtupleOutputTrack.h"

class TrackDataNtupleTree : public TrkValidationNtupleOutputTrack {
public :
   TrackDataNtupleTree(TTree *tree=0, bool includingDAFdata = false);
   virtual ~TrackDataNtupleTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
private :
    bool m_includingDAFdata;
};

#endif // TRACKDATANTUPLETREE_H
