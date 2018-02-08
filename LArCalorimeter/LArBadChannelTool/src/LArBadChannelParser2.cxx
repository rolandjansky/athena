/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArBadChannelTool/LArBadChannelParser2.h"

#include "GaudiKernel/MsgStream.h"
#include <iostream>

LArBadChannelParser2::LArBadChannelParser2(const std::string& file, MsgStream* const log) : 
  m_linenumber(0), m_filename(file), m_log(log)
{
  m_fin.open(m_filename.c_str());
  if(!m_log) {
    std::cerr << "WARNING \t LArBadChannelParser was given a NULL MsgStream pointer!" << std::endl;
  }
}

LArBadChannelParser2::~LArBadChannelParser2()
{
  m_fin.close();
}

bool LArBadChannelParser2::fileStatusGood() const
{
  return m_fin.good();
}

std::vector< LArBadChannelParser2::ISLine>
LArBadChannelParser2::parseISfile( int nint, int minString, 
				   int firstWildcard)
{
  std::vector< LArBadChannelParser2::ISLine> result;

  while (m_fin.good()) {
    std::string readLine;
    std::getline(m_fin, readLine);
    if(!m_fin.good()) return result;
    
    ++m_linenumber;
    ISLine parsedLine;
    if (parseLine( readLine, parsedLine, nint, minString, firstWildcard)) {
      result.push_back(parsedLine);
      // (*m_log) << MSG::DEBUG << "LArBadChannelParser ACCEPTED line " << readLine << endmsg;
    }
    else {
      (*m_log) << MSG::WARNING << "LArBadChannelParser REJECTED line " << readLine << endmsg;
    }
  }
  return result;
}

bool LArBadChannelParser2::parseLine( std::string& readLine, ISLine& parsedLine,
				      int nint, int minString, int firstWildcard) 
{
  std::string::size_type commentPosition = readLine.find('#');
  if(commentPosition != std::string::npos)	//if a comment is found
    readLine.erase(commentPosition);     	//trim off the comment

  std::string readWord;
  std::vector<std::string> command;

  std::istringstream stringIn(readLine);
  stringIn >> readWord;
  while(stringIn) {
    command.push_back(readWord);
    stringIn >> readWord;
  }
  if(command.size() > 0) {
    bool success = interpretLine( command, parsedLine, nint, minString, firstWildcard);
    return success;
  }
  return false; // empty command
}

bool LArBadChannelParser2::interpretLine( const std::vector<std::string>& command, 
					  ISLine& parsedLine,
					  int nint, int minString, int firstWildcard)
{
  if((int)command.size() < nint+minString) {
    (*m_log) << MSG::WARNING << "LArBadChannelParser REJECTED line " << m_linenumber \
	     << ":\t not enough parameters given" << endmsg; 
    return false;
  }

  for( int i=0; i<nint; i++) {
    int nextInt(0);
    if (firstWildcard >=0 && i>=firstWildcard && command[i] == "*") {
      parsedLine.intVec.push_back(-1);
    }
    else if( stringToInt(nextInt, command[i]) && nextInt >= 0) {
      parsedLine.intVec.push_back( nextInt);
    }
    else {
      (*m_log) << MSG::WARNING << "LArBadChannelParser REJECTED line " << m_linenumber \
	       << ":\t word " << i + 1 << " must be a non-negative integer: " << command[i] << endmsg;
      return false;
    }
  }

  // just copy the remaining words to the string vector
  parsedLine.stringVec.insert( parsedLine.stringVec.end(), command.begin()+nint, command.end());

  return true;
}
