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

  m_name = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/name");
  m_inputstream = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/inputstream");
  m_outputstream = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/outputstream");
  m_description = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/description");
  m_logic = m_inputStore->retrieve<std::vector<std::string> >("CutFlowTree/logic");
  m_nAcceptedEvents = m_inputStore->retrieve<std::vector<ULong_t> >("CutFlowTree/nAcceptedEvents");
  m_nWeightedAcceptedEvents = m_inputStore->retrieve<std::vector<Double_t> >("CutFlowTree/nWeightedAcceptedEvents");
  m_isComplete = m_inputStore->retrieve<std::vector<Int_t> >("CutFlowTree/isComplete");
  m_cycle = m_inputStore->retrieve<std::vector<Int_t> >("CutFlowTree/cycle");
  m_parentIndex = m_inputStore->retrieve<std::vector<Int_t> >("CutFlowTree/parentIndex");
  m_nbChildren = m_inputStore->retrieve<std::vector<Int_t> >("CutFlowTree/nbChildren");
  m_childrenIndices = m_inputStore->retrieve<std::vector< std::vector<UInt_t> > >("CutFlowTree/childrenIndices");
}

void FillEBCFromFlat::fill()
{
  //std::cout << "FillEBCFromFlat::fill()" << std::endl;

  unsigned int nCuts = m_name->size();

  for (unsigned int i = 0; i < nCuts; ++i) {
    if (m_isComplete->at(i) != m_wantIsComplete) continue;
    
    // Only add top level EventBookkeepers here 
    if (m_parentIndex->at(i) != -1) continue;

    EventBookkeeper *eb = newEventBookkeeper(i);
    m_coll->push_back(eb);

    if (m_nbChildren->at(i) > 0) {
      addChildren(eb, i);
    }
  }
}

EventBookkeeper *FillEBCFromFlat::newEventBookkeeper(unsigned int index) const
{
  //std::cout << "FillEBCFromFlat::newEventBookkeeper(" << index << ") - " << m_name->at(index) << std::endl;

  EventBookkeeper *eb = new EventBookkeeper(m_name->at(index));
  eb->setInputStream(m_inputstream->at(index));
  eb->setOutputStream(m_outputstream->at(index));
  eb->setDescription(m_description->at(index));
  eb->setLogic(m_logic->at(index));
  eb->setNAcceptedEvents(m_nAcceptedEvents->at(index));
  eb->setNWeightedAcceptedEvents(m_nWeightedAcceptedEvents->at(index));
  eb->setCycle(m_cycle->at(index));
  return eb;
}

void FillEBCFromFlat::addChildren(EventBookkeeper *eb, unsigned int indexOfEb) const
{
  //std::cout << "FillEBCFromFlat::addChildren(@" << eb << ", " << indexOfEb <<")" << std::endl;

  for (std::vector<UInt_t>::const_iterator childIndex = m_childrenIndices->at(indexOfEb).begin(); childIndex != m_childrenIndices->at(indexOfEb).end(); ++childIndex) {
    unsigned int corrChildIndex = (*childIndex) + m_offset;
    if (corrChildIndex == indexOfEb) {
      std::cout << "FillEBCFromFlat::addChildren() WARNING - corrChildIndex == indexOfEb == " << indexOfEb << std::endl;
      continue;
    }

    EventBookkeeper *ebChild = newEventBookkeeper(corrChildIndex);
    if (m_nbChildren->at(corrChildIndex) > 0) {
      addChildren(ebChild, corrChildIndex);
    }
    eb->AddChild(ebChild);
  }
}

