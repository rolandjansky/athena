/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILL_EBC_FROM_FLAT_H
#define FILL_EBC_FROM_FLAT_H

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGate.h"

#include "SGTools/StlVectorClids.h"
#include "SGTools/BuiltinsClids.h"

#include <string>
#include <vector>

#include <TTree.h>

#include "EventBookkeeperMetaData/EventBookkeeperCollection.h"

class FillEBCFromFlat {
  public:
    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;

    FillEBCFromFlat(StoreGateSvc_t &inputStore, EventBookkeeperCollection *coll, int wantIsComplete);
    void fill();

  private:
    void initialise();
    EventBookkeeper *newEventBookkeeper(unsigned int index) const;
    void addChildren(EventBookkeeper *eb, unsigned int indexOfEb) const;

  private:
    StoreGateSvc_t m_inputStore;
    EventBookkeeperCollection *m_coll;
    int m_wantIsComplete;

    unsigned int m_offset;

    std::vector<std::string> *name;
    std::vector<std::string> *inputstream;
    std::vector<std::string> *outputstream;
    std::vector<std::string> *description;
    std::vector<std::string> *logic;
    std::vector<ULong_t> *nAcceptedEvents;
    std::vector<Double_t> *nWeightedAcceptedEvents;
    std::vector<Int_t> *isComplete;
    std::vector<Int_t> *cycle;
    std::vector<Int_t> *parentIndex;
    std::vector<Int_t> *nbChildren;
    std::vector< std::vector<UInt_t> > *childrenIndices;
};

#endif // FILL_EBC_FROM_FLAT_H
