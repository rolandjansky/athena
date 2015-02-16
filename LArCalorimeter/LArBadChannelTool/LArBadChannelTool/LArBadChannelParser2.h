/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannelParser2_H
#define LArBadChannelParser2_H 

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

class MsgStream;

class LArBadChannelParser2 {
public:

  struct ISLine {
    std::vector<int>         intVec;
    std::vector<std::string> stringVec;
  };

  /** 
      @brief A parser object must be initialized with the 
      filename that it is to parse. It must also be given a 
      MsgStream pointer for logging messages. */
  LArBadChannelParser2(const std::string& filename, MsgStream* const messager);

  ~LArBadChannelParser2();

  /// parse the file using the format 
  /// "nint consecutive integers and then at least minString strings"
  /// and return the result as a vector of all successfully parsed lines.
  /// The int part of a line may contain wildcards, starting from position
  /// firstWildcard; firstWildcard=-1 means "no wildcards".
  /// The wildcard character in the input file is "*", it is replaced 
  /// by -1 in the parsed output.
  /// Lines that failed to parse are skipped, with diagnostic messages
  /// sent to the message stream.
  std::vector< ISLine> parseISfile( int nint, int minString, 
				    int firstWildcard=-1);

  /**
     @brief You can use this function to check 
     whether the file was successfully opened for reading */
  bool fileStatusGood() const;

private:

  std::ifstream     m_fin;	       //to read the file
  unsigned int      m_linenumber;      //line number currently being parsed
  const std::string m_filename;
  MsgStream* const  m_log;

  bool parseLine( std::string& readLine, ISLine& parsedLine,
		  int nint, int minString, int firstWildcard);

  bool interpretLine( const std::vector<std::string>& command, ISLine& parsedLine,
		      int nint, int minString, int firstWildcard);

  bool stringToInt(int& theInt, const std::string& theStr) const;

};

inline bool LArBadChannelParser2::stringToInt(int& theInt, const std::string& theStr) const
{
  std::istringstream iss(theStr);
  //the second condition checks for invalid input of the form "123abc"
  //return !(iss >> std::dec >> theInt).fail() && (static_cast<int>(theStr.size()) ==  iss.tellg());
  iss >> std::dec >> theInt;
  return !iss.fail() && iss.eof();
}

#endif
