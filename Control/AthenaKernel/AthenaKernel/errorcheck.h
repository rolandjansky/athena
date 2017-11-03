// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: errorcheck.h,v 1.6 2009-04-09 15:11:16 ssnyder Exp $
/**
 * @file  AthenaKernel/errorcheck.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2006
 * @brief Helpers for checking error return status codes and reporting errors.
 *
 * A typical sequence of code for calling something in Atlas that
 * returns a StatusCode looks something like this:
 *
 *@code
 *  StatusCode sc = something();
 *  if (! sc.isSuccess() ) {
 *    log << "Some error message" << endmsg;
 *    return sc;
 *  }
 @endcode
 *
 * This has several undesirable properties.  First, it is overly verbose.
 * 80% of the code in this example is devoted to error handling; if there
 * are many of these, it becomes difficult to read what the code is
 * actually doing.
 *
 * Second, there is no standardization of the error messages produced,
 * and they are sometimes difficult to trace back to the actual offending
 * source code.
 *
 * Third, code like this typically gets written by cut-and-paste,
 * which is notoriously error-prone.
 *
 * This header defines some helpers to address these issues.
 *
 * For the simple case, you can use the macro @c CHECK.  This macro
 * encapsulates the actions shown in the example above; it could be
 * rewritten as
 *
 *@code
 *  CHECK( something() );
 @endcode
 *
 * The error message produced by this will contain the text of the
 * expression that failed, the failed @c StatusCode, the source file
 * and line, the name of the containing function, and the
 * algorithm/tool/service name.  Note that in order to get this
 * last name, the macro uses the @c this pointer; this means that this
 * macro cannot be used in contexts where @c this is not available.
 * In this case, use the macro @c CHECK_WITH_CONTEXT, which allows
 * an explicit specification of the context name:
 *
 *@code
 *  CHECK_WITH_CONTEXT( something(), "algname" );
 @endcode
 *
 * In some cases, a simple check of the @c StatusCode is not enough and
 * @c CHECK cannot be used.  One can still avail oneself of the standardized
 * error reporting provided here with the macro
 * @c REPORT_ERROR, which takes a @c StatusCode as an argument.
 * Additional text can be added with the output streaming operator,
 * as for @c MsgStream.  (No need to use @c endmsg.)
 *
 *@code
 *  StatusCode sc = something();
 *  if ( ! sc.isSuccess() && ! failure_is_ok ) {
 *    REPORT_ERROR (sc) << "something()";
 *    return sc;
 *  }
 @endcode
 *
 * This will report the same kind of error as @c CHECK would.
 *
 * @c CHECK will return the failing status code to the caller and log the
 * appropriate type of error message.  If you need to control what gets
 * returned to the caller, you can use @c CHECK_RECOVERABLE and
 * @c CHECK_FATAL (and the corresponding @c _WITH_CONTEXT) versions.
 * If these macros detect a failure, they always return a @c RECOVERABLE
 * or @c FATAL status to the caller, respectively.  In addition,
 * @c CHECK_CODE and @c CHECK_CODE_WITH_CONTEXT may be used to return
 * an arbitrary status code to the caller.
 *
 * Like @c CHECK, @c REPORT_ERROR requires that @c this be available.
 * If this is not the case, then as before there is also a macro
 * @c REPORT_ERROR_WITH_CONTEXT that takes the context name explicitly:
 *
 *@code
 *  StatusCode sc = something();
 *  if ( ! sc.isSuccess() && ! failure_is_ok ) {
 *    REPORT_ERROR_WITH_CONTEXT (sc, "algname") << "something()";
 *    return sc;
 *  }
 @endcode
 *
 * In addition, there are @c REPORT_MESSAGE and @c REPORT_MESSAGE_WITH_CONTEXT
 * macros.  These do not take a @c StatusCode argument, but do take
 * a logging level argument, so you can emit messages other than errors.
 * Example:
 *
 *@code
 *   REPORT_MESSAGE (MSG::INFO) << "Something happened."
 @endcode
 *
 * In the case where you want to change the source line, file, function, etc.,
 * from what's provided by default, you can instantiate an instance of
 * @c errorcheck::ReportMessage directly.  This may also be used
 * if you need to use multiple statements to generate the output message.
 * The message will be issued when then object is destroyed.  Example:
 *
 *@code
 *   {
 *     errorcheck::ReportMessage msg (MSG::INFO, ERRORCHECK_ARGS, "context");
 *     msg << "testing ";
 *     for (int i=1; i<=3; i++)
 *       msg << i << " ";
 *   }
 @endcode
 */


#ifndef ATHENAKERNEL_ERRORCHECK_H
#define ATHENAKERNEL_ERRORCHECK_H


#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include <string>
#include <atomic>

class INamedInterface;


// Internal out-of-line functions go in this namespace.
namespace errorcheck {


/**
 * @brief Return the context name from a context (@c this) pointer.
 * @param context The context from which to get the name.
 * @return The name of this context.
 *
 * This is the specialization for an @c INamedInterface.
 */
std::string context_name (const INamedInterface* context);


/**
 * @brief Return the context name from a context (@c this) pointer.
 * @param context The context from which to get the name.
 * @return The name of this context.
 *
 * This is the specialization for an unknown context type; it returns
 * a blank name.
 */
std::string context_name (const void* context);


/**
 * @brief Helper class to use to report a message.
 * To report a message, create an instance of this class.
 * A standard header, containing the file/line, context name,
 * and so forth, is automatically put at the start of the message.
 * More text may be added using the standard @c MsgStream streaming
 * operators.  The message is actually emitted when this object
 * is destroyed.
 */
class ReportMessage
  : public MsgStream
{
public:
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
  ReportMessage (MSG::Level level,
                 int line,
                 const char* file,
                 const char* func,
                 const std::string& context,
                 StatusCode sc);


  /**
   * @brief Constructor.
   * @param level The error logging level for the message.
   * @param line The source line from which the report is being made.
   * @param file The source file name from which the report is being made.
   * @param func The name of the function from which the report is being made.
   * @param context The name of the context (algorithm/tool/service/etc.)
   *                from which the report is being made.
   */
  ReportMessage (MSG::Level level,
                 int line,
                 const char* file,
                 const char* func,
                 const std::string& context);


  /**
   * @brief Destructor.
   * This will cause the message to be emitted, if it hasn't already been.
   */
  ~ReportMessage();


  /**
   * @brief Emit the message.
   * We override this method from @c MsgStream in order to fix up
   * the message punctuation.
   */
  virtual MsgStream& doOutput();

  /**
   * @brief Convert to a @c MsgStream reference.
   *
   * Trying to use a @c ReportMessage like
   *
   *@code
   *   ReportMessage(...) << "foo";
   @endcode
   *
   * doesn't work, because (as of Gaudi 19) the @c MsgStream inserters
   * are written as free functions:
   *
   *@code
   *    MsgStream& operator<< (MsgStream&, ...);
   @endcode
   *
   * and the non-const @c MsgStream reference won't bind to the temporary.
   * We can work around this problem with this method, which will convert
   * the temporary to a non-const reference.  So, this should work:
   *
   *@code
   *   ReportMessage(...).stream() << "foo";
   @endcode
   */
  MsgStream& msgstream() { return *this; }


  /**
   * @brief If set to true, hide the source file and line number
   *        in the output.
   *
   *        This is intended for use in regression tests, where
   *        it's undesirable to have the output change if source lines
   *        are added or deleted.
   */
  static void hideErrorLocus (bool flag = true);


  /**
   * @brief If set to true, hide function names in the output.
   *        in the output.
   *
   *        This is intended for use in regression tests, where
   *        function names may be formatted differently on different
   *        platforms.
   */
  static void hideFunctionNames (bool flag = true);


private:
  /**
   * @brief Generate the common header for messages.
   * @param level The error logging level for the message.
   * @param line The source line from which the report is being made.
   * @param file The source file name from which the report is being made.
   * @param func The name of the function from which the report is being made.
   */
  void format_common (MSG::Level level,
                      int line,
                      const char* file,
                      const char* func);

  /// The position in the output message after the standard header.
  std::string::size_type m_pos;

  /// If true, hide the source file and line number in output messages.
  static std::atomic<bool> s_hide_error_locus;

  /// If true, hide the function names in output messages.
  static std::atomic<bool> s_hide_function_names;
};


} // namespace errorcheck


// Macro to get the current function name.
// Currently, this relies on a gcc extension, and will just return
// an empty string for another compiler.  A similar mechanism (__func__)
// has been proposed for the next revision of the C++ standard (cf. n1642);
// if that makes it in, it can be used instead.
#if defined(__GNUC__)
# define ERRORCHECK_FNAME __PRETTY_FUNCTION__
#else
# define ERRORCHECK_FNAME ""
#endif


// Collect all the arguments describing the source location:
// line number, file name, function name.
#define ERRORCHECK_ARGS __LINE__, __FILE__, ERRORCHECK_FNAME


/**
 * @brief Report an error, with an explicitly specified context name.
 * @param SC The @c StatusCode.
 * @param CONTEXT_NAME The name of the current context.
 *
 * Additional text may be appended with the output stream operator.
 */
#define REPORT_ERROR_WITH_CONTEXT(SC, CONTEXT_NAME)             \
  errorcheck::ReportMessage (MSG::ERROR, ERRORCHECK_ARGS, CONTEXT_NAME, SC) \
    . msgstream()


/**
 * @brief Report a message, with an explicitly specified context name.
 * @param LVL The error logging level.
 * @param CONTEXT_NAME The name of the current context.
 *
 * Additional text may be appended with the output stream operator.
 */
#define REPORT_MESSAGE_WITH_CONTEXT(LVL, CONTEXT_NAME)             \
  errorcheck::ReportMessage (LVL, ERRORCHECK_ARGS, CONTEXT_NAME).msgstream()


/**
 * @brief Report an error.  @c this must be available to get the context name.
 * @param SC The @c StatusCode.
 *
 * Additional text may be appended with the output stream operator.
 */
#define REPORT_ERROR(SC)                                           \
  REPORT_ERROR_WITH_CONTEXT(SC, errorcheck::context_name (this))


/**
 * @brief Report a message.  @c this must be available to get the context name.
 * @param LVL The error logging level.
 *
 * Additional text may be appended with the output stream operator.
 */
#define REPORT_MESSAGE(LVL)                                           \
  REPORT_MESSAGE_WITH_CONTEXT(LVL, errorcheck::context_name (this))


/**
 * @brief Internal macro, used when a CHECK macro detects a failure.
 *
 * Output an appropriate error message and return the status object.
 */
#define CHECK_FAILED(EXP, CONTEXT_NAME, SC) do { \
      errorcheck::ReportMessage (SC.isRecoverable() ?     \
                                   MSG::ERROR :           \
                                   MSG::FATAL,            \
                                 ERRORCHECK_ARGS,         \
                                 CONTEXT_NAME,            \
                                 SC) . msgstream()        \
                                   << #EXP;               \
      return SC;                                          \
  } while(0)


/**
 * @brief Evaluate an expression and check for errors,
 *        with an explicitly specified context name.
 * @param EXP The expression to evaluate.
 * @param CONTEXT_NAME The name of the current context.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an error message will be produced,
 * and the failing @c StatusCode will be returned to the caller.
 */
#define CHECK_WITH_CONTEXT(EXP, CONTEXT_NAME) do {        \
    StatusCode sc__ = (EXP);                              \
    if (! sc__.isSuccess())                               \
      CHECK_FAILED(EXP, CONTEXT_NAME, sc__);              \
  } while (0)


/**
 * @brief Evaluate an expression and check for errors.
 *        @c this must be available to get the context name.
 * @param EXP The expression to evaluate.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an error message will be produced,
 * and the failing @c StatusCode will be returned to the caller.
 */
#define CHECK(EXP) CHECK_WITH_CONTEXT(EXP, errorcheck::context_name(this))


/**
 * @brief Evaluate an expression and check for errors,
 *        with an explicitly specified context name.
 * @param EXP The expression to evaluate.
 * @param CONTEXT_NAME The name of the current context.
 * @param CODE The status code to return on failure.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an error message will be produced,
 * and @c CODE will be returned to the caller.
 */
#define CHECK_CODE_WITH_CONTEXT(EXP, CONTEXT_NAME, CODE) do {  \
    StatusCode sc__ = (EXP);                                          \
    if (! sc__.isSuccess()) {                                         \
      sc__ = CODE;                                                    \
      CHECK_FAILED(EXP, CONTEXT_NAME, sc__);                          \
    }                                                                 \
  } while (0)


/**
 * @brief Evaluate an expression and check for errors.
 *        @c this must be available to get the context name.
 * @param EXP The expression to evaluate.
 * @param CODE The status code to return on failure.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an error message will be produced,
 * and @c CODE will be returned to the caller.
 */
#define CHECK_CODE(EXP, CODE) \
  CHECK_CODE_WITH_CONTEXT(EXP, errorcheck::context_name(this), CODE)


/**
 * @brief Evaluate an expression and check for errors,
 *        with an explicitly specified context name.
 * @param EXP The expression to evaluate.
 * @param CONTEXT_NAME The name of the current context.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an error message will be produced,
 * and @c RECOVERABLE will be returned to the caller.
 */
#define CHECK_RECOVERABLE_WITH_CONTEXT(EXP, CONTEXT_NAME) \
  CHECK_CODE_WITH_CONTEXT(EXP, CONTEXT_NAME, StatusCode::RECOVERABLE)


/**
 * @brief Evaluate an expression and check for errors.
 *        @c this must be available to get the context name.
 * @param EXP The expression to evaluate.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an error message will be produced,
 * and @c RECOVERABLE will be returned to the caller.
 */
#define CHECK_RECOVERABLE(EXP) \
  CHECK_RECOVERABLE_WITH_CONTEXT(EXP, errorcheck::context_name(this))


/**
 * @brief Evaluate an expression and check for errors,
 *        with an explicitly specified context name.
 * @param EXP The expression to evaluate.
 * @param CONTEXT_NAME The name of the current context.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an error message will be produced,
 * and @c FAILURE will be returned to the caller.
 */
#define CHECK_FATAL_WITH_CONTEXT(EXP, CONTEXT_NAME) \
  CHECK_CODE_WITH_CONTEXT(EXP, CONTEXT_NAME, StatusCode::FAILURE)


/**
 * @brief Evaluate an expression and check for errors.
 *        @c this must be available to get the context name.
 * @param EXP The expression to evaluate.
 *
 * This macro will evaluate @c EXP, which should return a @c StatusCode.
 * If the status is not successful, then an error message will be produced,
 * and @c FAILURE will be returned to the caller.
 */
#define CHECK_FATAL(EXP) \
  CHECK_FATAL_WITH_CONTEXT(EXP, errorcheck::context_name(this))


#endif // not ATHENAKERNEL_ERRORCODE_H
