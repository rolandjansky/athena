/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILL_EBC_FROM_FLAT_H
#define FILL_EBC_FROM_FLAT_H

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

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

    std::vector<std::string> *m_name;
    std::vector<std::string> *m_inputstream;
    std::vector<std::string> *m_outputstream;
    std::vector<std::string> *m_description;
    std::vector<std::string> *m_logic;
    std::vector<ULong_t> *m_nAcceptedEvents;
    std::vector<Double_t> *m_nWeightedAcceptedEvents;
    std::vector<Int_t> *m_isComplete;
    std::vector<Int_t> *m_cycle;
    std::vector<Int_t> *m_parentIndex;
    std::vector<Int_t> *m_nbChildren;
    std::vector< std::vector<UInt_t> > *m_childrenIndices;
};

#endif // FILL_EBC_FROM_FLAT_H
