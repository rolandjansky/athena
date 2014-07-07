/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStreamCnvTest/RpcByteStreamEncoder.h"

RpcByteStreamEncoder::RpcByteStreamEncoder(const RpcPadContainer* padContainer) :
  m_padContainer(padContainer),
  m_byteStream(),
  m_byteStreamWord(0),
  m_highest(true)
{ }

RpcByteStreamEncoder::~RpcByteStreamEncoder()
{ }

StatusCode RpcByteStreamEncoder::encodeByteStream()
{
  StatusCode sc;
  // Iterate on the pads and add them to the bytestream
  RpcPadContainer::const_iterator pad_it;
  for (pad_it = m_padContainer->begin() ; pad_it != m_padContainer->end() ; ++pad_it)
    {
      const RpcPad* newpad = (*pad_it);
      addPad(newpad);
      sc = StatusCode::SUCCESS;
    }
  return sc;
}

void RpcByteStreamEncoder::addRx()
{ } 

void RpcByteStreamEncoder::addPad(const RpcPad * pad)
{ 
  // Add the pad header
  PadReadOutStructure pros;
  ubit16 inputData[3] = {0, pad->onlineId(), pad->status()};
  ubit16 padHeader = pros.makeHeader(inputData);
  addWord(padHeader);

  // Iterate on the matrices and add them to the bytestream
  RpcPad::const_iterator cma_it;
  for (cma_it = pad->begin() ; cma_it != pad->end() ; ++cma_it)
    {
      const RpcCoinMatrix* newcma = (*cma_it);
      addMatrix(newcma);
    }
  // Add the pad footer
  ubit16 errorCode = pad->errorCode();
  ubit16 padFooter = pros.makeFooter(errorCode);
  addWord(padFooter);
}

void RpcByteStreamEncoder::addMatrix(const RpcCoinMatrix * matrix)
{ 
  MatrixReadOutStructure mros;
  ubit16 dataWord;

  // Add the matrix header
  ubit16 inputData[3] = {0, matrix->onlineId(), matrix->fel1Id()}; 
  dataWord = mros.makeHeader(inputData);
  addWord(dataWord);
  
  // Add the matrix subheader
  dataWord = mros.makeSubHeader();
  addWord(dataWord);  

  // Matrix Body
  RpcCoinMatrix::const_iterator chan_it;
  for (chan_it = matrix->begin() ; chan_it != matrix->end() ; ++chan_it)
    {
      const RpcFiredChannel* newchan = (*chan_it);
      addFiredChannel(newchan);
    }
  // Add the matrix footer
  dataWord = mros.makeFooter(matrix->crc());
  addWord(dataWord);
}

void RpcByteStreamEncoder::addFiredChannel(const RpcFiredChannel * firedChannel)
{
  MatrixReadOutStructure mros;
  ubit16 dataWord;
  ubit16 inputData[5];

  // Add the fired channel
  inputData[0] = 0;
  inputData[1] = firedChannel->bcid();
  inputData[2] = firedChannel->time();
  inputData[3] = firedChannel->ijk();
  inputData[4] = firedChannel->channel();

  dataWord = mros.makeBody(inputData);
  addWord(dataWord);
}

void RpcByteStreamEncoder::addWord(ubit16 dataWord)
{
  unsigned int dataWord32 = (unsigned int) dataWord;
  if (m_highest)
    {
      m_byteStreamWord = (dataWord32<<16);
      m_highest = false;
    }
  else
    {
      m_byteStreamWord = m_byteStreamWord | dataWord32;
      m_byteStream->push_back(m_byteStreamWord);
      m_byteStreamWord = 0;
      m_highest = true;
    }
}






