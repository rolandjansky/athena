/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ASCIIIOINPUT_H
#define ASCIIIOINPUT_H

#include <string>
#include <vector>
#include <fstream>

/** @class AsciiInput

    @author W. H. Bell <W.Bell@cern.ch>
    
    @brief A class to provide read access to an ASCII input file.
*/
class AsciiInput {
 public:
  AsciiInput(const std::string& fileName);
  ~AsciiInput();

  /** Open the input file.  A non-zero value is returned if this is
      not successful. */
  int open();

  /** Close the input file.  A non-zero value is returned if this is
      not successful. */
  int close();

  /** Read one line of the input text file into a vector of strings.
      Any number of space or tab characters are used as separators.
      If the row contains only separators or the end of file has been
      reached the returned vector will be empty. */
  std::vector<std::string> readRow();
  
  /** A helper function to convert a string into a string vector, by
      using any number of space or tab characters as separators. */
  static std::vector<std::string> strToStrVec(std::string inputString);

  /** A helper function to convert a string to a long value. */
  static long strToLong(std::string inputString);

  /** A helper function to convert a string to a double value. */
  static double strToDouble(std::string inputString);

 private:

  /** Input file name */
  std::string m_fileName;
  
  /** Input file stream */
  std::ifstream m_file;

  /** Size of the character buffers */
  static const int MAX_LINE_LENGTH = 500;
  
  /** Character buffers used while parsing the input file */
  char m_lineBuffer[MAX_LINE_LENGTH];
};

#endif
