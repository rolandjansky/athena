/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: errorcheck.cxx,v 1.5 2009-04-09 15:11:17 ssnyder Exp $
/**
 * @file  errorcheck.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2006
 * @brief Helpers for checking error return status codes and reporting errors.
 */

#include "AthenaKernel/errorcheck.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/INamedInterface.h"
#include <ctype.h>


namespace errorcheck {


/// If true, hide the source file and line number in output messages.
std::atomic<bool> ReportMessage::s_hide_error_locus;


/// If true, hide the function names in output messages.
std::atomic<bool> ReportMessage::s_hide_function_names;


/**
 * @brief Clean `allocator' template arguments from the function f.
 * @param f Name of the function to clean.
 */
std::string clean_allocator (std::string f)
{
  std::string::size_type ipos = 0;
  while ((ipos = f.find (", std::allocator", ipos)) != std::string::npos) {
    const char* p = f.c_str() + ipos + 16;
    while (isspace (*p)) ++p;
    if (*p == '<') {
      ++p;
      int nest = 1;
      while (nest > 0 && *p) {
        if (*p == '<')
          ++nest;
        else if (*p == '>')
          --nest;
        ++p;
      }
      if (nest == 0) {
        if (ipos > 0 && f[ipos-1] != '>') {
          while (isspace (*p)) ++p;
        }
        f.erase (ipos, p-f.c_str()-ipos);
        p = f.c_str() + ipos;
      }
    }
    ipos = p - f.c_str();
  }
  return f;
}


std::string munge_string_name (const std::string& str_in)
{
  std::string s = str_in;

  std::string::size_type ipos = 0;
  while ((ipos = s.find ("std::basic_string<", ipos)) != std::string::npos) {
    std::string::size_type beg = ipos;
    ipos += 18;
    int inest = 1;
    while (inest > 0 && ipos < s.size()) {
      if (s[ipos] == '<') ++inest;
      else if (s[ipos] == '>') --inest;
      ++ipos;
    }
    s.replace (beg, ipos-beg, "std::string");
    ipos = beg+11;
  }
  
  for (size_t i = 0; i < s.size(); i++) {
    if ((i == 0 || (s[i-1] != ':' && !isalnum(s[i-1]))) &&
        strncmp (s.c_str()+i, "string", 6) == 0 &&
        !isalnum(s[i+6]))
    {
      s.replace (i, 6, "std::string");
    }
  }
  return s;
}


std::string munge_punct (const std::string& str_in)
{
  std::string s = str_in;
  for (size_t i = 0; i < s.size()-1; i++) {
    if (s[i] == ' ' && (s[i+1] == '*' || s[i+1] == '&'))
      s.erase (i, 1);
  }
  return s;
}


std::string do_replace (std::string s,
                        const std::string& pat,
                        const std::string& rep)
{
  std::string::size_type ipos = 0;
  while ((ipos = s.find (pat, ipos)) != std::string::npos)
    s.replace (ipos, pat.size(), rep);
  return s;
}


std::string munge_names (const std::string& str_in)
{
  std::string s =
    do_replace (str_in, "SG::DataProxyStorageData::pointer", "void*");
  s = do_replace (s, "DPSD::pointer", "void*");
  s = do_replace (s, "SG::auxid_t", "unsigned long");
  s = do_replace (s, "auxid_t", "unsigned long");
  s = do_replace (s, "void* ", "void*");
  s = do_replace (s, "CLID", "unsigned int");

  if (s.compare (0, 8, "virtual ") == 0)
    s.erase (0, 8);
  return s;
}



/**
 * @brief Constructor.
 * @param level The error logging level for the message.
 * @param line The source line from which the report is being made.
 * @param file The source file name from which the report is being made.
 * @param func The name of the function from which the report is being made.
 * @param context The name of the context (algorithm/tool/service/etc.)
 *                from which the report is being made.
 * @param sc The @c StatusCode to include in the error message.
 */
ReportMessage::ReportMessage (MSG::Level level,
                              int line,
                              const char* file,
                              const char* func,
                              const std::string& context,
                              StatusCode sc)
  : MsgStream (Athena::getMessageSvc(), context)
{
  // The common part.
  format_common (level, line, file, func);

  // The status code.
  *this << ": code " << sc.getCode();

  // Remember the end of the header.
  m_pos = stream().str().size();
}


/**
 * @brief Constructor.
 * @param level The error logging level for the message.
 * @param line The source line from which the report is being made.
 * @param file The source file name from which the report is being made.
 * @param func The name of the function from which the report is being made.
 * @param context The name of the context (algorithm/tool/service/etc.)
 *                from which the report is being made.
 */
ReportMessage::ReportMessage (MSG::Level level,
                              int line,
                              const char* file,
                              const char* func,
                              const std::string& context)
  : MsgStream (Athena::getMessageSvc(), context)
{
  // The common part.
  format_common (level, line, file, func);

  // Remember the end of the header.
  m_pos = stream().str().size();
}


/**
 * @brief Generate the common header for messages.
 * @param level The error logging level for the message.
 * @param line The source line from which the report is being made.
 * @param file The source file name from which the report is being made.
 * @param func The name of the function from which the report is being made.
 */
void ReportMessage::format_common (MSG::Level level,
                                   int line,
                                   const char* file,
                                   const char* func)
{
  // Logging level.
  *this << level;

  // Write the source file/line.
  if (s_hide_error_locus)
    *this << "FILE:LINE";
  else
    *this << file << ":" << line;

  // Include the function name if available.
  if (s_hide_function_names)
    *this << " (FUNC)";
  else {
    if (func && func[0] != '\0') {
      std::string cfunc = munge_names (munge_punct (munge_string_name (clean_allocator (func))));
      *this << " (" << cfunc << ")";
    }
  }
}


/**
 * @brief Destructor.
 * This will cause the message to be emitted, if it hasn't already been.
 */
ReportMessage::~ReportMessage()
{
  // Don't do anything if the message has already been emitted
  // (due to using << endmsg, for example).
  if (!stream().str().empty())
    *this << endmsg;
}


/**
 * @brief Emit the message.
 * We override this method from @c MsgStream in order to fix up
 * the message punctuation.
 */
MsgStream& ReportMessage::doOutput()
{
  // The deal here is this.  We want
  //  REPORT_MESSAGE(MSG::INFO) << "foo";
  // to get a `: ' before the `foo'.
  // But we don't want
  //  REPORT_MESSAGE(MSG::INFO);
  // to have a trailing `: '.
  // So, after we generate the common header part, we remember
  // where we are in the string.  When the message is emitted, we look
  // to see if additional text has been added.  If not, then we don't
  // need to do anything.  But if so, we insert a `: ' after the header.
  if (m_pos != stream().str().size()) {
    std::string tmp1 = stream().str();
    std::string tmp2 = tmp1.substr(0, m_pos) + ": " + tmp1.substr(m_pos);
    stream().str (tmp2);
  }
  return MsgStream::doOutput();
}


/**
 * @brief If set to true, hide the source file and line number
 *        in the output.
 *
 *        This is intended for use in regression tests, where
 *        it's undesirable to have the output change if source lines
 *        are added or deleted.
 */
void ReportMessage::hideErrorLocus (bool flag /*= true*/)
{
  s_hide_error_locus = flag;
}


/**
 * @brief If set to true, hide function names in the output.
 *        in the output.
 *
 *        This is intended for use in regression tests, where
 *        function names may be formatted differently on different
 *        platforms.
 */
void ReportMessage::hideFunctionNames (bool flag /*= true*/)
{
  s_hide_function_names = flag;
}


} // namespace errorcheck


namespace errorcheck {


/**
 * @brief Return the context name from a context (@c this) pointer.
 * @param context The context from which to get the name.
 * @return The name of this context.
 *
 * This is the specialization for an @c INamedInterface.
 */
std::string context_name (const INamedInterface* context)
{
  return context->name();
}


/**
 * @brief Return the context name from a context (@c this) pointer.
 * @param context The context from which to get the name.
 * @return The name of this context.
 *
 * This is the specialization for an unknown context type; it returns
 * a blank name.
 */
std::string context_name (const void* /*context*/)
{
  return "";
}


} // namespace errorcheck
