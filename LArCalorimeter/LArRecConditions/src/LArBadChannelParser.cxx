/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArBadChannelParser.h"

#include <sstream>
#include <algorithm>

LArBadChannelParser::LArBadChannelParser(
  const std::string& file, MsgStream* const log, 
  unsigned int numInts, int numStrings, unsigned int firstWildcard
  ) : 
  m_linenumber(0), m_filename(file), m_log(log), 
  m_numInts(numInts), m_numStrings(numStrings), 
  m_firstWildcard(firstWildcard)
{
  m_fin.open(m_filename.c_str());
  if(!m_log)
    std::cerr << "ERROR \t LArBadChannelParser was given a NULL MsgStream pointer!" << std::endl;
}

LArBadChannelParser::~LArBadChannelParser()
{
  m_fin.close();
}

std::vector<std::string> LArBadChannelParser::parseLine(std::string& line) const
{
  std::string::size_type commentPosition = line.find('#');
  if(commentPosition != std::string::npos)	//if a comment is found
    line.erase(commentPosition);		//trim off the comment
  // notice that the input parameter can be changed as a side-effect

  std::vector<std::string> result;
  std::string readWord;
  std::istringstream stringIn(line);	

  stringIn >> readWord;
  while(stringIn)
  {
    result.push_back(readWord);
    stringIn >> readWord;
  }
  return result;
}


int upper(int c) {
  return std::toupper((unsigned char)c);
}


std::vector<int> LArBadChannelParser::getIdFields(const std::vector<std::string>& words)
{
  std::vector<int> result;
  std::string w;
  for(unsigned int i = 0; i < m_numInts; ++i)
  {
    w=words[i]; //Copy to non-const string

    //Allow 'B' or 'EMB' for Barrel or 'E' for Endcap in first field
    if (i==0) {  
      std::transform(w.begin(), w.end(), w.begin(), upper);

      if (w.compare(0,3,"EMB")==0 || w[0]=='B')
	w=std::string("0"); //Replace for barrel
      else if (w[0]=='E')
	w=std::string("1"); //Replace for endcap
    }
    else if (i==1) {  //Allow 'A' and 'C' do describe side in second field
      if (w=="A" || w=="a")
	w=std::string("1"); //Replace for positive side
      else if (w=="C" || w=="c")
	w=std::string("0"); //Replace for negative side
    }

    int idComponent(0);
    bool canBeWildcard(m_firstWildcard > 0 && i + 1 >= m_firstWildcard);
    if(canBeWildcard && w == "*")
      result.push_back(-1);  	// -1 signifies wildcard 
    else if(stringToNumber<int>(idComponent, w) && idComponent >= 0)
      result.push_back(idComponent);
    else
    {
      (*m_log) << MSG::WARNING << "LArBadChannelParser REJECTED line " << m_linenumber \
        << " -\t word " << i + 1 << " must be a non-negative integer " \
        << (canBeWildcard ? "or " : "and not " ) \
        << "a wildcard: " << words[i] << endmsg;
      result.clear();
      return result;	// return empty vector on error
    }
  }
  return result;
}
