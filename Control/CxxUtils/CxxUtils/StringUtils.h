// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StringUtils.cxx,v 1.0 2014-05-25 16:54 cburgard Exp $
/**
 * @file  StringUtils.cxx
 * @author carsten burgard <cburgarc@cern.ch>
 * @date May, 2014
 * @brief namespace for misc string utility
 *
 * This file provides a set of StringUtils that go beyond the
 * capabilities of typical string utility functions (such as provided
 * by boost). This includes fail-proof trimming of strings with
 * multibyte characters and control sequences and parsing helpers that
 * understand and respect parenthesis.
 *
 * Also implemented are functions that allow easy conversion between
 * HTML-type strings, LaTeX-type strings, unicode strings and plain
 * ascii strings. The aim of these methods is converting symbols and
 * mathematical expressions in strings, including subscript and
 * superscript and special mathematical or greek symbols - they are
 * not intended as a replacement for conversion of entire documents
 * and do not respect text or background colors, sectioning commands,
 * annotations, document meta-information, non-textual elements,
 * custom macro definitions, javascript, or any other features beyond
 * basic text markup.
 */

#ifndef CXXUTILS_STRINGUTILS_H
#define CXXUTILS_STRINGUTILS_H

#include <string>
#include <ostream>
#include <boost/regex.hpp>

namespace CxxUtils {

  namespace StringUtils {

    typedef std::string::size_type size_type;

    const std::string beginNormal     = "\033[0m";
    const std::string beginBoldWhite  = "\033[1m";
    const std::string beginBoldPink   = "\033[1;35m";
    const std::string beginBoldBlue   = "\033[1;34m";
    const std::string beginBoldYellow = "\033[1;33m";
    const std::string beginBoldGreen  = "\033[1;32m";
    const std::string beginBoldRed    = "\033[1;31m";
    const std::string beginPink       = "\033[35m";
    const std::string beginBlue       = "\033[34m";
    const std::string beginYellow     = "\033[33m";
    const std::string beginGreen      = "\033[32m";
    const std::string beginRed        = "\033[31m";


    const std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string whitespace = " \t\n\r";
    
    const boost::regex generic_latex_regex("\\\\([abcdefghijklnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ]+)[ ]*");
    const boost::regex generic_xml_regex("<\\s*([abcdefghijklnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ]+)([^>])*>");
    const boost::regex generic_html_entity_regex("&[abcdefghijklnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ]+;");
    
    enum SPECIALSCRIPT {
      NORMALSCRIPT = 0,
      SUPERSCRIPT = 1,
      SUBSCRIPT = 2
    };
      
    enum FORMAT {
      ASCII = 0,
      UNICODE = 1,
      LATEX = 2,
      HTML = 3
    };

    size_t getStringWidth(const std::string& str);
    void   writeFixedWidth(std::ostream& os, const std::string& input, size_t width, const std::string& align);

    size_type findParenthesisMatch (const std::string& str,
                                    size_type nextpos,
                                    const std::string& paropen,
                                    const std::string& parclose);
    size_type rfindParenthesisMatch(const std::string& str,
                                    size_type nextpos,
                                    const std::string& paropen,
                                    const std::string& parclose);

    size_type findFree (const std::string& haystack,
                        const std::string& needle,
                        const std::string& paropen,
                        const std::string& parclose,
                        size_type startpos = 0);
    size_type rfindFree(const std::string& haystack,
                        const std::string& needle,
                        const std::string& paropen,
                        const std::string& parclose,
                        size_type startpos);

    size_type findFreeOf (const std::string& haystack,
                          const std::string& needles,
                          const std::string& paropen,
                          const std::string& parclose,
                          size_type startpos = 0);
    size_type rfindFreeOf(const std::string& haystack, 
                          const std::string& needles, 
                          const std::string& paropen,
                          const std::string& parclose,
                          size_type startpos);

    std::string replaceSymbols(const std::string& str, StringUtils::FORMAT inputFormat, StringUtils::FORMAT outputFormat);
    size_type findBeginSpecialScript(const std::string& str,
                                     StringUtils::SPECIALSCRIPT scripttype,
                                     size_type pos = 0);
    size_type findEndSpecialScript(const std::string& str,
                                   StringUtils::SPECIALSCRIPT scripttype,
                                   size_type pos = 0);

    StringUtils::FORMAT guessFormat(const std::string& input);
    std::string convertText(const std::string& input, StringUtils::FORMAT inputFormat, StringUtils::FORMAT outputFormat);
    std::string convertText(const std::string& input, StringUtils::FORMAT outputFormat);

    std::string stripUnprintableCharacters(const std::string& str, bool allowNonAscii = true);
    std::string replaceSpecialScript(const std::string& str, StringUtils::SPECIALSCRIPT inputType, StringUtils::SPECIALSCRIPT outputType);

    inline bool isOnlyWhitespace(const std::string&str){
      return (str.find_first_not_of(StringUtils::whitespace) == std::string::npos);
    }

    inline std::string trim(const std::string& str){
      size_t start = str.find_first_not_of(StringUtils::whitespace);
      size_t end = str.find_last_not_of(StringUtils::whitespace);
      return str.substr(start,end-start+1);
    }
  }

}

#endif //CXXUTILS_STRINGUTILS_H
