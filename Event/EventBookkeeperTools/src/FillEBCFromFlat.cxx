/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FillEBCFromFlat.h"

#include <iostream>

FillEBCFromFlat::FillEBCFromFlat(StoreGateSvc_t &inputStore, EventBookkeeperCollection *coll, int wantIsComplete)
  : m_inputStore(inputStore), m_coll(coll), m_wantIsComplete(wantIsComplete)
{
  if (m_wantIsComplete) {
    m_offset = 0;
  } else {
    m_offset = *(m_inputStore->retrieve<int>("CutFlowTree/CutFlow_Nb"));
  }
  initialise();
}

void FillEBCFromFlat::initialise()
{
  //std::cout << "FillEBCFromFlat::initialise()" << std::endl;

  name = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/name");
  inputstream = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/inputstream");
  outputstream = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/outputstream");
  description = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/description");
  logic = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/logic");
  nAcceptedEvents = m_inputStore->retrieve<std::vector<ULong_t> >("CutFlowTree/nAcceptedEvents");
  nWeightedAcceptedEvents = m_inputStore->retrieve<std::vector<Double_t> >("CutFlowTree/nWeightedAcceptedEvents");
  isComplete = m_inputStore->retrieve<std::vector<Int_t> >("CutFlowTree/isComplete");
  cycle = m_inputStore->retrieve<std::vector<Int_t> >("CutFlowTree/cycle");
  parentIndex = m_inputStore->retrieve<std::vector<Int_t> >("CutFlowTree/parentIndex");
  nbChildren = m_inputStore->retrieve<std::vector<Int_t> >("CutFlowTree/nbChildren");
  childrenIndices = m_inputStore->retrieve<std::vector< std::vector<UInt_t> > >("CutFlowTree/childrenIndices");
}

void FillEBCFromFlat::fill()
{
  //std::cout << "FillEBCFromFlat::fill()" << std::endl;

  unsigned int nCuts = name->size();

  for (unsigned int i = 0; i < nCuts; ++i) {
    if (isComplete->at(i) != m_wantIsComplete) continue;
    
    // Only add top level EventBookkeepers here 
    if (parentIndex->at(i) != -1) continue;

    EventBookkeeper *eb = newEventBookkeeper(i);
    m_coll->push_back(eb);

    if (nbChildren->at(i) > 0) {
      addChildren(eb, i);
    }
  }
}

EventBookkeeper *FillEBCFromFlat::newEventBookkeeper(unsigned int index) const
{
  //std::cout << "FillEBCFromFlat::newEventBookkeeper(" << index << ") - " << name->at(index) << std::endl;

  EventBookkeeper *eb = new EventBookkeeper(name->at(index));
  eb->setInputStream(inputstream->at(index));
  eb->setOutputStream(outputstream->at(index));
  eb->setDescription(description->at(index));
  eb->setLogic(logic->at(index));
  eb->setNAcceptedEvents(nAcceptedEvents->at(index));
  eb->setNWeightedAcceptedEvents(nWeightedAcceptedEvents->at(index));
  eb->setCycle(cycle->at(index));
  return eb;
}

void FillEBCFromFlat::addChildren(EventBookkeeper *eb, unsigned int indexOfEb) const
{
  //std::cout << "FillEBCFromFlat::addChildren(@" << eb << ", " << indexOfEb <<")" << std::endl;

  for (std::vector<UInt_t>::const_iterator childIndex = childrenIndices->at(indexOfEb).begin(); childIndex != childrenIndices->at(indexOfEb).end(); ++childIndex) {
    unsigned int corrChildIndex = (*childIndex) + m_offset;
    if (corrChildIndex == indexOfEb) {
      std::cout << "FillEBCFromFlat::addChildren() WARNING - corrChildIndex == indexOfEb == " << indexOfEb << std::endl;
      continue;
    }

    EventBookkeeper *ebChild = newEventBookkeeper(corrChildIndex);
    if (nbChildren->at(corrChildIndex) > 0) {
      addChildren(ebChild, corrChildIndex);
    }
    eb->AddChild(ebChild);
  }
}

