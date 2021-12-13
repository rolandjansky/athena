/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelParser_H
#define LArBadChannelParser_H

#include "GaudiKernel/MsgStream.h"
#include <fstream>

class LArBadChannelParser
{
public:

	/** 
	@brief A parser object must be initialized with the 
	filename that it is to parse. It must also be given a 
	MsgStream pointer for logging messages. */
	LArBadChannelParser(
		const std::string& filename, MsgStream* const log, 
		unsigned int numInts, int numStrings, unsigned int firstWildcard = 0);

	virtual	~LArBadChannelParser();

	/**
	@brief You can use this function to check 
	whether file-reading will succeed. If false, 
	parseFile() will return an empty vector. */
	bool fileStatusGood() const;

 	/** 
	@brief Parse the file using the following format for each line:
	First, exactly numInts integers are required. Then, if numString is negative,
	at least std::abs(numString) strings are required, otherwise exactly numString 
	strings are required. These strings are converted to a vector<T>.
	A vector of all successfully parsed lines is returned.
	The integer part of a line may contain wildcards, starting from position
	firstWildcard (where counting begins from 1); firstWildcard = 0 means 
	wildcards are not allowed. The wildcard character in the input file 
	is "*", it is replaced by -1 in the parsed output. Comments in the ASCII 
	file are demarcated with the '#' character. Lines that fail to parse are 
	skipped, with diagnostic messages sent to the message stream. */

	template<typename T>
	std::vector< std::pair<std::vector<int>, std::vector<T> > > parseFile();

private:

	// Can not construct a parser without giving filename or MsgStream.
	LArBadChannelParser();

	std::vector<int> getIdFields(const std::vector<std::string>& words);
	std::vector<std::string> parseLine(std::string& line) const;

	template<typename T> 
	bool stringToNumber(T& theNumber, const std::string& theStr) const;

	template<typename T> 
	bool convertStrings(std::vector<T>& result, const std::vector<std::string>::const_iterator begin, 
		const std::vector<std::string>::const_iterator end) const;

	std::ifstream m_fin;			//to read the file
	unsigned int m_linenumber;		//line number currently being parsed
	const std::string m_filename;
	MsgStream* const m_log;

	const unsigned int m_numInts;		//the number of integers required to represent the Identifier(s)
	const int m_numStrings;			//the number of strings required to represent the status
	const unsigned int m_firstWildcard;	// The first admissible position for a wildcard ('*')
						// The counting is from 1, i.e. first  = 1
						// If zero, then wildcards are not allowed
};

inline bool LArBadChannelParser::fileStatusGood() const
{
	return m_fin.good();
}

template<typename T>
inline std::vector< std::pair<std::vector<int>, std::vector<T> > > LArBadChannelParser::parseFile()
{
  typedef std::pair< std::vector<int>, std::vector<T> > LineData;
  std::vector<LineData> result;

  if(!fileStatusGood())
  {
    (*m_log) << MSG::ERROR << "LArBadChannelParser - Could not open file: " << m_filename << endmsg;
    return result;	//empty result
  }

  while(true)
  {
    std::string line;
    std::getline(m_fin, line);
    if(!fileStatusGood())
      break;	// loop termination at end of file
    ++m_linenumber;
    std::vector<std::string> input = parseLine(line);

    if(input.size() < m_numInts + std::abs(m_numStrings)) // if line is too short, skip it
    {
      if(input.size() != 0) // complain only for non-empty lines
        {(*m_log) << MSG::WARNING << "LArBadChannelParser REJECTED line " << m_linenumber \
          << " -\t not enough parameters given: " << input.size() << endmsg;}
      continue;
    }

    LineData parsedData;

    parsedData.first = getIdFields(input);
    if(parsedData.first.size() == 0)
      continue;	// getIdFields failed, with error message. Skip this line.

    const std::vector<std::string>::const_iterator start = input.begin() + m_numInts;
    const std::vector<std::string>::const_iterator stop = \
      (m_numStrings < 0) ? static_cast<const std::vector<std::string>::const_iterator>(input.end()) : (start + m_numStrings);

    if(stop > start)
    { 
      if(!convertStrings<T>(parsedData.second, start, stop))
        continue; //convertStrings failed with error message. Skip this line.
    }

    if(stop < input.end())
      (*m_log) << MSG::WARNING << "LArBadChannelParser IGNORED unexpected input on line " << m_linenumber << "." << endmsg;

    result.push_back(parsedData);
    (*m_log) << MSG::VERBOSE << "LArBadChannelParser ACCEPTED line " << m_linenumber << " -\t " << line << endmsg;
  }

  (*m_log) << MSG::DEBUG << "LArBadChannelParser - Parsed " << m_linenumber << " lines in file '" << m_filename << "'." << endmsg;
  return result;
}

// The convertStrings function reads the elements in the input line after the integer part, 
// and converts them to type T. The code can be easily extended by adding specialized template functions
// to convert the elements to any desired type. The default template function below will work for any
// standard C++ numeric type, and the specialized template function below that works for the trivial case
// where T is string type.

template<typename T> //works for any standard C++ numeric type 
inline bool LArBadChannelParser::convertStrings(std::vector<T>& result, 
  const std::vector<std::string>::const_iterator begin, const std::vector<std::string>::const_iterator end) const
{
  result.clear();
  result.reserve(static_cast<unsigned int>(end - begin));
  for(std::vector<std::string>::const_iterator iter = begin; iter != end; ++iter)
  {
    T element;
    if(stringToNumber<T>(element, *iter))
      result.push_back(element);
    else
    {
      (*m_log) << MSG::WARNING << "LArBadChannelParser REJECTED line " << m_linenumber \
        << " -\t failed to extract status." << endmsg;
      return false;
    }
  }
  return true;
}

//trivial case: convert string to string
template<>
inline bool LArBadChannelParser::convertStrings(std::vector<std::string>& result, 
  const std::vector<std::string>::const_iterator begin, const std::vector<std::string>::const_iterator end) const
{
  result.assign(begin, end);
  return true;
}

//works for any built-in numeric C++ type
template<typename T>
inline bool LArBadChannelParser::stringToNumber(T& theNumber, const std::string& theString) const
{
        std::istringstream iss(theString);
        //return !(iss >> std::dec >> theNumber).fail() && (static_cast<int>(theString.size()) ==  iss.tellg());
        return !(iss >> std::dec >> theNumber).fail();
       //the second condition checks for invalid input of the form "123abc"
}

#endif	//end of #ifndef LArBadChannelParser_H

