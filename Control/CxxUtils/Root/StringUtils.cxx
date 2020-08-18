/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StringUtils.cxx,v 1.0 2014-05-25 16:54 cburgard Exp $
/**
 * @file  StringUtils.cxx
 * @author carsten burgard <cburgarc@cern.ch>
 * @date May, 2014
 * @brief namespace for misc string utility
 */


#include "CxxUtils/StringUtils.h"
#include "StringUtils_aux.h"

#include <algorithm>
#include <iostream>

namespace CxxUtils {

  namespace StringUtils {

    /// calculate the width of a string
    /**
       \param str input string to be analyzed
       \return number of 'characters' (printable entities) in this string

       this function will return the number of 'characters' (printable
       entities) in any string. for well-behaved strings, this will be
       identical to std::string::size(). however,
         - non-printable-characters will not be counted
	     - control sequences will not be counted
         - multi-byte characters will only be counted once
     */
    size_t getStringWidth(const std::string& str){
      size_t i=0;
      size_t w=0;
      while(i<str.size()){
	int c = (unsigned char)(str[i]);
	// handle control sequences
	if (c == '\033'){
	  size_t newi = str.find_first_of("mAC",i);
	  if(newi == std::string::npos){
	    i++;
	    continue;
	  } else if(newi+1 < str.size()){
	    i = newi+1;
	    continue;
	  } else {
	    return w;
	  }
	}
	// increase the index by the number of bytes in this character
	i+=utf8_skip_data[c];
	// only count printable characters
	if(c > 31) w++;
      }
      return w;
    }


    /// writes a string to a given stream with a fixed width
    /**
       short strings will be supplemented with spaces, long strings will be truncated.
       this function respects and supports std::strings with wchar_t UTF8 entries

       \param os output stream to be used
       \param input string to be written
       \param width length of the target string in 'characters' (printable entities)
       \param align option string controlling the alignment.

       the alignment string will be searched for the following characters:
       'l': triggers left-align (flushleft)
       'r': triggers right-align (flushright)
       'c' or 'm': triggers middle-align (center)
       '.': annotate strings with '...' where truncated
    */
    void writeFixedWidth(std::ostream& os, const std::string& input, size_t width, const std::string& align){
      size_t w = getStringWidth(input);
      char a = 'l';
      if(align.find('r') != std::string::npos) a = 'r';
      else if(align.find('c') != std::string::npos || align.find('m') != std::string::npos) a = 'c';
      if(w < width){
	// we can fit the entire string and will only append spaces 
	size_t n = width - w;
	switch(a){
	case 'l':
	  os << input;
	  for(size_t i=0; i<n; ++i) os << ' '; 
	  break;
	case 'c':
	  for(size_t i=0; i<ceil(0.5*n); ++i) os << ' '; 
	  os << input;
	  for(size_t i=0; i<floor(0.5*n); ++i) os << ' '; 
	  break;
	case 'r':
	  for(size_t i=0; i<n; ++i) os << ' '; 
	  os << input;
	  break;
	default:
	  // this should never happen
	  return;
	}
      } else {
	// we can't fit the entire string and need to crop
	size_t dots = ((align.find('.') == std::string::npos) ? 0 : 3);
	// the skip_head and skip_tail variables hold the number of
	// characters to be skipped at the front and at the back of
	// the string
	size_t skip_head = 0;
	size_t skip_tail = 0;
	if(width < 2*dots) dots = 0; 
	switch(a){
	case 'l':
	  skip_head = 0;
	  skip_tail = w - width + dots;
	  break;
	case 'c':
	  // add leading dots
	  for(size_t j=0; j<dots; j++) os << '.';
	  skip_head = floor(0.5*(w - width)) + dots;
	  skip_tail = ceil (0.5*(w - width)) + dots;
	  break;
	case 'r':
	  // add leading dots
	  for(size_t i=0; i<dots; ++i) os << '.'; 
	  skip_head = w - width + dots;
	  skip_tail = 0;
	  break;
	default:
	  // this should never happen
	  break;
	}
	// here, we actually loop over the string, writing all
	// accepted characters
	size_t ichar = 0;
	size_t iwchar =0;
	while(ichar<input.size()){
	  int c = (unsigned char)(input[ichar]);
	  // take care that control sequences are added
	  if (c == '\033'){
	    size_t i = input.find_first_of("mAC",ichar);
	    if(i != std::string::npos){
	      os << input.substr(ichar,i-ichar+1);
	      if(i+1 < input.size()){
		ichar = i+1;
		continue;
	      } else break;
	    }
	  }
	  // ignore any non-printable characters
	  if(c < 32){
	    ichar++;
	    continue;
	  }
	  // compute the byte size of this character
	  size_t size = utf8_skip_data[c];
	  // add it if it's inside the accepted range
	  if(iwchar >= skip_head && iwchar < (w - skip_tail)){
	    os << input.substr(ichar,size);
	  }
	  // increase character count and index
	  iwchar++;
	  ichar+=size;
	}
	if(dots > 0){
	  switch(a){
	  case 'l':
	    // add trailing dots
	    for(size_t i=0; i<dots; ++i) os << '.'; 
	    break;
	  case 'c':
	    // add trailing dots
	    for(size_t i=0; i<dots; i++) os << '.';
	    break;
	  default:
	    // this should never happen
	    break;
	  }
	}
      }
    }

    /// finds the nearest matching parenthesis in a string from a given position
    /**
       \param str string to search
       \param nextpos starting position of parenthesis check 
       \param paropen opening parenthesis (arbitrary string)
       \param parclose closing parenthesis (arbitrary string)
       \return position of matching parenthesis close in the string
    */
    size_type findParenthesisMatch(const std::string& str,
                                   size_type nextpos,
                                   const std::string& paropen,
                                   const std::string& parclose)
    {
      size_type openbrace = 0;
      size_type closebrace = 0;
      size_type bracestack = 1;
      while((bracestack > 0) && (nextpos < str.size())){
	openbrace = str.find(paropen, nextpos+1);
	closebrace = str.find(parclose, nextpos+1);
	nextpos++;
	if(openbrace < closebrace){
	  bracestack++;
	  nextpos = openbrace;
	} else {
	  bracestack--;
	  nextpos = closebrace;
	}
      }
      return nextpos;
    }

    /// reverse-finds the nearest matching parenthesis in a string from a given position
    /**
       \param str string to search
       \param nextpos starting position of parenthesis check 
       \param paropen opening parenthesis (arbitrary string)
       \param parclose closing parenthesis (arbitrary string)
       \return position of matching parenthesis close in the string
    */
    size_type rfindParenthesisMatch(const std::string& str,
                                    size_type nextpos,
                                    const std::string& paropen,
                                    const std::string& parclose)
    {
      size_type openbrace = 0;
      size_type closebrace = 0;
      size_type bracestack = 1;
      while((bracestack > 0) && (nextpos < str.size())){
	openbrace = str.rfind(paropen, nextpos-1);
	closebrace = str.rfind(parclose, nextpos-1);
	// this line is correct and important!
	closebrace = std::min(closebrace, closebrace+1);
	// it helps to avoid overflows of 'closebrace' that lead to wrong return values!
	nextpos--;
	if(openbrace < closebrace){
	  bracestack++;
	  nextpos = closebrace;
	} else {
	  bracestack--;
	  nextpos = openbrace;
	}
      }
      return nextpos;
    }

    /// finds the next "free" occurrence of needle in haystack
    /**
       \param haystack string to search
       \param needle string to search for
       \param paropen opening parenthesis, denoting the beginning of a "closed block"
       \param parclose closing parenthesis, denoting the end of a "closed block"
       \param startpos position to start searching at
       \return position of the next free occurrence
    */
    size_type findFree(const std::string& haystack,
                       const std::string& needle,
                       const std::string& paropen,
                       const std::string& parclose,
                       size_type startpos)
    {
      size_type needlepos = haystack.find(needle, startpos);
      size_type nextparopen = haystack.find(paropen, startpos);
      while(needlepos != std::string::npos && needlepos > nextparopen){
	startpos = findParenthesisMatch(haystack, nextparopen, paropen, parclose)+1;
	needlepos = haystack.find(needle, startpos);
	nextparopen = haystack.find(paropen, startpos);
      }
      return needlepos;
    }  

    /// reverse-finds the next "free" occurrence of needle in haystack
    /**
       \param haystack string to search
       \param needle string to search for
       \param paropen opening parenthesis, denoting the beginning of a "closed block"
       \param parclose closing parenthesis, denoting the end of a "closed block"
       \param startpos position to start searching at
       \return position of the previous free occurrence
    */
    size_type rfindFree(const std::string& haystack,
                        const std::string& needle,
                        const std::string& paropen,
                        const std::string& parclose,
                        size_type startpos)
    {
      size_type needlepos = haystack.rfind(needle, startpos);
      size_type nextparclose = haystack.rfind(parclose, startpos);
      while(needlepos != std::string::npos && needlepos < nextparclose){
	startpos = rfindParenthesisMatch(haystack, nextparclose, paropen, parclose)-1;
	// this line is correct and important! 
	startpos = std::min(startpos+1, startpos-1); 
	// it helps to avoid overflows of 'startpos' that result in non-terminating function calls!
	needlepos = haystack.rfind(needle, startpos);
	nextparclose = haystack.rfind(parclose, startpos);
      }
      return needlepos;
    }  

    /// finds the next "free" occurrence of any needle in haystack
    /**
       \param haystack string to search
       \param needles string with characters to search for
       \param paropen opening parenthesis, denoting the beginning of a "closed block"
       \param parclose closing parenthesis, denoting the end of a "closed block"
       \param startpos position to start searching at
       \return position of the next free occurrence
    */
    size_type findFreeOf(const std::string& haystack,
                         const std::string& needles,
                         const std::string& paropen,
                         const std::string& parclose,
                         size_type startpos)
    {
      size_type needlepos = haystack.find_first_of(needles, startpos);
      size_type nextparopen = haystack.find(paropen, startpos);
      while(needlepos != std::string::npos && needlepos > nextparopen){
	startpos = findParenthesisMatch(haystack, nextparopen, paropen, parclose)+1;
	needlepos = haystack.find_first_of(needles, startpos);
	nextparopen = haystack.find(paropen, startpos);
      }
      return needlepos;
    }  

    /// reverse-finds the next "free" occurrence of any needle in haystack
    /**
       \param haystack string to search
       \param needles string with characters to search for
       \param paropen opening parenthesis, denoting the beginning of a "closed block"
       \param parclose closing parenthesis, denoting the end of a "closed block"
       \param startpos position to start searching at
       \return position of the previous free occurrence
    */
    size_type rfindFreeOf(const std::string& haystack,
                          const std::string& needles,
                          const std::string& paropen,
                          const std::string& parclose,
                          size_type startpos)
    {
      size_type needlepos = haystack.find_last_of(needles, startpos);
      size_type nextparclose = haystack.rfind(parclose, startpos);
      while(needlepos != std::string::npos && needlepos < nextparclose){
	startpos = rfindParenthesisMatch(haystack, nextparclose, paropen, parclose);
	// this line is correct and important! 
	startpos = std::min(startpos+1, startpos-1); 
	// it helps to avoid overflows of 'startpos' that result in non-terminating function calls!
	needlepos = haystack.find_last_of(needles, startpos);
	nextparclose = haystack.rfind(parclose, startpos);
	// this line is correct and important! 
	nextparclose = std::min(nextparclose, nextparclose+1);
	// it helps to avoid overflows of 'nextparclose' that result wrong return values
      }
      return needlepos;
    }  


    /// replaces unicode, latex and html symbols with one another as desired
    /**
       \param str input string
       \param inputFormat format of the input string
       \param outputFormat format of the output string
       \return new string with replaced symbols
    */
    std::string replaceSymbols(const std::string& str, StringUtils::FORMAT inputFormat, StringUtils::FORMAT outputFormat){
      if(inputFormat == outputFormat) return str;
      size_t index = 0;
      std::string output = "";
      output.reserve(2*str.size());
      while(index < str.size()){
	size_t nextIndex = std::string::npos;
	size_t symbolId = 0;
	for(size_t i=0; i<STRINGUTILS__N_MAP_FORMAT; ++i){
	  size_t idx = str.find(map_format[i][inputFormat],index);
	  const std::string& symbol = map_format[i][inputFormat];
	  if(idx < nextIndex){
	    const size_t endindex = index + map_format[i][inputFormat].size();
	    // make sure that we have not selected something that's in the middle of a word
	    if(((index == 0) || !(
				  StringUtils::letters.find(str[index-1]) != std::string::npos && 
				  StringUtils::letters.find(symbol[0])    != std::string::npos
				  ))
		&& 
	       ((endindex+1 == str.size()) || !(
						StringUtils::letters.find(str[endindex])           != std::string::npos && 
						StringUtils::letters.find(symbol[symbol.size()-1]) != std::string::npos
						))
	       ){
	      nextIndex = idx;
	      symbolId = i;
	    }
	  }
	}
	if(nextIndex != std::string::npos){
	  output += str.substr(index,nextIndex-index);
	  output += map_format[symbolId][outputFormat];
	  index = nextIndex + map_format[symbolId][inputFormat].size();
	} else {
	  output += str.substr(index);
	  break;
	}
	// in the case of LATEX-code, we need to take care about the
	// fact that latex commands without an explicitly given
	// argument eat up all the space behind them
	if(inputFormat == StringUtils::LATEX && map_format[symbolId][StringUtils::LATEX][0]=='\\' 
	   && map_format[symbolId][StringUtils::LATEX].find_first_of("{}") == std::string::npos){
	  index = str.find_first_not_of("\n \t",index);
	  index = str.find_first_not_of("{}",index);
	} else if(outputFormat == StringUtils::LATEX && map_format[symbolId][StringUtils::LATEX][0]=='\\' 
		  && map_format[symbolId][StringUtils::LATEX].find_first_of("{}") == std::string::npos){
	  output += "{}";
	}
      }
      return output;
    }

    /// replaces unicode superscript and subscript with the corresponding ascii characters and vice-versa
    /**
       \param str input string
       \param inputType type of script to be removed
       \param outputType type of script to be inserted
       \return new string with replaced characters
    */
    std::string replaceSpecialScript(const std::string& str, StringUtils::SPECIALSCRIPT inputType, StringUtils::SPECIALSCRIPT outputType){
      // if the input and output type are identical 
      if(inputType == outputType) return str;
      size_t index = 0;
      // allocate the string
      std::string output;
      output.reserve(2*str.size());
      while(index != std::string::npos){
	size_t symbolId = 0;
	size_t nextIndex = std::string::npos;
	// loop over all known symbols
	for(size_t i=0; i<STRINGUTILS__N_MAP_SPECIALSCRIPT; ++i){
	  // see which one turns up first in the string
	  size_t idx = str.find(map_specialscript[i][inputType],index);
	  if(idx < nextIndex){
	    // save it
	    nextIndex = idx;
	    symbolId = i;
	  }
	}
	if(nextIndex != std::string::npos){
	  output += str.substr(index,nextIndex-index);
	  output += map_specialscript[symbolId][outputType];
	  index = nextIndex + map_specialscript[symbolId][inputType].size();
	} else {
	  output += str.substr(index);
	  index = nextIndex;
	}
      }
      return output;
    }

  
    /// finds the next entity of a certain type of special script
    /**
       \param str input string
       \param scripttype type of script to be found
       \param pos starting position for the search
       \return position of next special script entity of the given type
    */
    size_type findBeginSpecialScript(const std::string& str,
                                     StringUtils::SPECIALSCRIPT scripttype,
                                     size_type pos)
    {
      while(pos < str.size()){
	for(size_t i=0; i<STRINGUTILS__N_MAP_SPECIALSCRIPT; ++i){
	  if(str.find(map_specialscript[i][scripttype]) != std::string::npos) return pos;
	}
      }
      return std::string::npos;
    }
  
    /// finds the next entity not belonging to a certain type of special script
    /**
       \param str input string
       \param scripttype type of script to be ignored
       \param pos starting position for the search
       \return position of next entity not belonging to the given type of special script
    */
    size_type findEndSpecialScript(const std::string& str,
                                   StringUtils::SPECIALSCRIPT scripttype,
                                   size_type pos)
    {
      while(pos < str.size()){
	bool ok = false;
	for(size_t i=0; i<STRINGUTILS__N_MAP_SPECIALSCRIPT; ++i){
	  if(str.find(map_specialscript[i][scripttype]) != std::string::npos) ok = true;
	}
	if(!ok) return pos;
	pos++;
      }
      return std::string::npos;
    }

    /// removes all non-printable characters from a string
    /**
       \param str input string
       \param allowNonAscii true will allow non-ascii characters, falls will remove them (default true)
       \return modified string
    */
    std::string stripUnprintableCharacters(const std::string& str, bool allowNonAscii){
      size_t i=0;
      std::string output;
      output.reserve(str.size());
      while(i<str.size()){
	int c = (unsigned char)(str[i]);
	// handle control sequences
	if (c == '\033'){
	  size_t newi = str.find_first_of("mAC",i);
	  if(newi == std::string::npos){
	    i++;
	    continue;
	  } else if(newi+1 < str.size()){
	    i = newi+1;
	    continue;
	  } else {
	    return output;
	  }
	}
	// find the number of bytes in this character
	size_t length = utf8_skip_data[c];
	if(c > 31){
	  if(c < 128 || allowNonAscii){
	    output += str.substr(i,length);
	  }
	}
	i+=length;
      }
      return output;
    }

    /// guesses the format of a string
    /**
       \param input input string
       \return supposed StringUtils::FORMAT
    */
    StringUtils::FORMAT guessFormat(const std::string& input){
      boost::match_results<std::string::const_iterator> what;
      if(boost::regex_search(input.begin(), input.end(), what, StringUtils::generic_latex_regex))       return StringUtils::LATEX;
      if(boost::regex_search(input.begin(), input.end(), what, StringUtils::generic_html_entity_regex)) return StringUtils::HTML;
      if(boost::regex_search(input.begin(), input.end(), what, StringUtils::generic_xml_regex))         return StringUtils::HTML;
      for(size_t i=0; i<input.size(); ++i){
	if((unsigned char)(input[i]) > 128){
	  return StringUtils::UNICODE;
	}
      }
      return StringUtils::ASCII;
    }

    
    /// reads the next LaTeX token
    /** 
	\param latex input string
	\param token reference to the string where the token will be stored
	\param start starting position
    */
    inline size_t readLaTeXToken(const std::string& latex, std::string& token, size_t start){
      token.clear();
      if(start+2 > latex.size()) return std::string::npos;
      if(latex[start] == '{'){
	size_type endgrp = StringUtils::findParenthesisMatch(latex,start,"{","}");
	token = latex.substr(start+1,endgrp-start-1);
	return endgrp+1;
      } else {
	token += latex[start];
	return start+1;
      }
    }

    /// converts a string from one format into another
    /**
       \param input input string
       \param inputFormat alleged input format (one of HTML, LATEX, UNICODE, ASCII)
       \param outputFormat desired output format (one of HTML, LATEX, UNICODE, ASCII)
       \return converted string
    */
    std::string convertText(const std::string& input, StringUtils::FORMAT inputFormat, StringUtils::FORMAT outputFormat){
      if(inputFormat == outputFormat) return input;
      if(inputFormat == StringUtils::ASCII){
	// if the input is plain ascii, we need to do nothing more than replace a couple of symbols
	return StringUtils::replaceSymbols(input,inputFormat,outputFormat);
      } 
      if(inputFormat == StringUtils::UNICODE){
	// if the input is unicode, we need to take care of superscript and subscript;
	std::string scriptreplaced;
	scriptreplaced.reserve(2*input.size());
	size_t pos = 0;
	while(pos < input.size()){
	  size_type startSuperScript = findBeginSpecialScript(input,StringUtils::SUPERSCRIPT,pos);
	  size_type startSubScript   = findBeginSpecialScript(input,StringUtils::SUBSCRIPT,pos);
	  if(startSuperScript == std::string::npos && startSubScript == std::string::npos){
	    scriptreplaced += input.substr(pos);
	    break;
	  }
	  if(startSuperScript < startSubScript){
	    size_t endScript = findEndSpecialScript(input,StringUtils::SUPERSCRIPT,startSuperScript);
	    scriptreplaced += map_superscript_starters[outputFormat];
	    scriptreplaced += StringUtils::replaceSpecialScript(input.substr(startSuperScript,endScript-startSuperScript),StringUtils::SUPERSCRIPT,StringUtils::NORMALSCRIPT);
	    scriptreplaced += map_superscript_enders[outputFormat];
	    pos = endScript;
	  } else {
	    size_t endScript = findEndSpecialScript(input,StringUtils::SUBSCRIPT,startSubScript);
	    scriptreplaced += map_subscript_starters[outputFormat];
	    scriptreplaced += StringUtils::replaceSpecialScript(input.substr(startSubScript,endScript-startSubScript),StringUtils::SUBSCRIPT,StringUtils::NORMALSCRIPT);
	    scriptreplaced += map_subscript_enders[outputFormat];
	    pos = endScript;
	  }
	}
	// now we need to replace the symbols
	std::string symbolsreplaced = StringUtils::replaceSymbols(scriptreplaced,StringUtils::UNICODE,outputFormat);
	// in the end, we just strip everything else
	return StringUtils::stripUnprintableCharacters(symbolsreplaced);
      }
      if(inputFormat == StringUtils::LATEX){
	// this is LaTeX - the parsing is a little more involved in this case
	std::string retval;
	size_t pos = 0;
	std::string latex = StringUtils::replaceSymbols(input,StringUtils::LATEX,outputFormat);
	while(pos != std::string::npos){
	  // we iterate over the string and only stop at 'special symbols' - everything else we just copy
	  size_t nextpos = latex.find_first_of("\\${^_",pos);
	  if(nextpos == std::string::npos){
	    // we are at the end of the string -- copy the rest
	    retval += latex.substr(pos);
	    break;
	  }
	  // copy everything up to the special symbol we found
	  retval += latex.substr(pos,nextpos-pos);
	  switch(latex[nextpos]){
	  case '^':
	    {
	      // superscript
	      std::string token;
	      // read the token
	      size_t endtok = readLaTeXToken(latex,token,nextpos+1);
	      if(endtok != std::string::npos){
		if(outputFormat == StringUtils::UNICODE)
		  // replace it with unicode characters
		  retval += replaceSpecialScript(StringUtils::trim(token),StringUtils::NORMALSCRIPT,StringUtils::SUPERSCRIPT);
		else if(outputFormat == StringUtils::HTML){
		  // wrap it in HTML tags
		  retval += "<sup>";
		  retval += StringUtils::trim(token);
		  retval += "</sup>";
		} else {
		  retval += token;
		}
	      }
	      pos = endtok;
	    }
	    break;
	  case '_':
	    {
	      // subscript
	      std::string token;
	      // read the token
	      size_t endtok = readLaTeXToken(latex,token,nextpos+1);
	      if(endtok != std::string::npos){
		if(outputFormat == StringUtils::UNICODE)
		  // replace it with unicode characters
		  retval += replaceSpecialScript(StringUtils::trim(token),StringUtils::NORMALSCRIPT,StringUtils::SUBSCRIPT);
		else if(outputFormat == StringUtils::HTML){
		  // wrap it in HTML tags
		  retval += "<sub>";
		  retval += StringUtils::trim(token);
		  retval += "</sub>";
		} else {
		  retval += token;
		}
	      }
	      pos = endtok;
	    }
	    break;
	  case '\\':
	    { 
	      // isolate the command
              size_type endpos = latex.find_first_not_of(StringUtils::letters,nextpos+1);
              if (endpos == std::string::npos)
                endpos = latex.size();
	      std::string cmd(latex.substr(nextpos+1,endpos-nextpos-1));
              size_type opengrp = latex.find_first_not_of(StringUtils::whitespace,endpos);
              if (opengrp == std::string::npos)
                opengrp = latex.size();
	      std::string token;
	      // read the token (argument of the command)
	      size_t endtok = readLaTeXToken(latex,token,opengrp);
	      if(outputFormat==StringUtils::HTML){
		// replace command by HTML equivalent (incomplete)
		if(cmd=="textbf") retval += "<b>"+token+"</b>";
		else if(cmd=="textit") retval += "<i>"+token+"</i>";
		// TODO: process some more commands
		else retval += token;
	      } else {
		// do nothing
		retval += token;
	      }
	      pos = endtok;
	    }
	    break;
	  case '$':
	    {
	      // find the end of the math block
	      size_t endpos = latex.find('$',nextpos+1);
	      if(endpos == std::string::npos){
		// it's unterminated, break
		retval += latex.substr(pos);
		pos = endpos;
		break;
	      }
	      // extract the content
	      std::string result = StringUtils::convertText(latex.substr(nextpos+1,endpos-nextpos-1),StringUtils::LATEX,outputFormat);
	      if(outputFormat == StringUtils::HTML){
		// typeset the "math environment"
		retval += "<span class='math' style='font-style:italic'>";
		retval += result;
		retval += "</span>";
	      } else {
		// do nothing
		retval += result;
	      }
	      pos = endpos+1;
	    }
	    break;
	  case '{':
	    {
	      // plain latex group (no command)
	      std::string token;
	      // read the token
	      size_t endtok = readLaTeXToken(latex,token,nextpos);
	      // re-insert it to get rid of the braces
	      retval += convertText(token,StringUtils::LATEX,outputFormat);
	      pos = endtok;
	    }
	    break;
	  default:
	    // this should never happen
	    throw "ERROR in CxxUtils::StringUtils::converText: an unknown key character was encountered!";
	  }
	}
	return retval;
      }
      if(inputFormat == StringUtils::HTML){
	// currently, this only removes html tags
	std::string retval;
	boost::smatch opentagmatch;
	std::string::const_iterator start = input.begin();
   //	std::string::const_iterator last = input.begin();
	std::string::const_iterator end   = input.end();
	while (boost::regex_search(start, end, opentagmatch,StringUtils::generic_xml_regex)){
	  std::string tagname(opentagmatch[1]);
	  retval += StringUtils::replaceSymbols(input.substr(std::distance(input.begin(),start),std::distance(start,opentagmatch[0].first)),StringUtils::HTML,outputFormat);
	  boost::regex closeTag("<\\s*/"+tagname+"\\s*>");
	  boost::smatch closetagmatch;
	  if(boost::regex_search(opentagmatch[0].second,end,closetagmatch,closeTag)){
	    // <tagname> content </tagname>
	    std::string content =input.substr(std::distance(input.begin(),opentagmatch[0].second),std::distance(opentagmatch[0].second,closetagmatch[0].first));
	    std::string result = StringUtils::trim(StringUtils::convertText(content,StringUtils::HTML,outputFormat));
	    if(outputFormat == StringUtils::LATEX){
	      if(tagname=="b") retval += "\\textbf{"+result+"}";
	      else if(tagname=="i") retval += "\\textit{"+result+"}";
	      else if(tagname=="sub") retval +="\\ensuremath{_{"+result+"}}";
	      else if(tagname=="sup") retval +="\\ensuremath{^{"+result+"}}";
	      // TODO: process some more tags
	      else retval += result;
	    } else if(outputFormat == StringUtils::UNICODE){
	      if     (tagname=="sub") retval +=StringUtils::replaceSpecialScript(result,StringUtils::NORMALSCRIPT,StringUtils::SUBSCRIPT);
	      else if(tagname=="sup") retval +=StringUtils::replaceSpecialScript(result,StringUtils::NORMALSCRIPT,StringUtils::SUPERSCRIPT);
	      else retval += result;
	    } else {
	      retval += result;
	    }
	    start = closetagmatch[0].second;
	  } else {
	    // lonely <tagname>
	    if(tagname=="br"){
	      if(outputFormat==StringUtils::LATEX) retval += "\\\\";
	      else retval += "\n";
	    }
	    start = opentagmatch[0].second;
	  }
	}
	retval+=StringUtils::replaceSymbols(input.substr(std::distance(input.begin(),start),std::distance(start,end)),StringUtils::HTML,outputFormat);
	return retval;
      }
      throw "ERROR in StringUtils::converText - control reached end of function!";
    }
    
    /// converts a string into any format
    /**
       This variant guesses the input format via StringUtils::guessFormat;

       \param input input string
       \param outputFormat desired output format
       \return converted string
    */
    std::string convertText(const std::string& input, StringUtils::FORMAT outputFormat){
      return StringUtils::convertText(input,StringUtils::guessFormat(input),outputFormat);
    }
  }
}
