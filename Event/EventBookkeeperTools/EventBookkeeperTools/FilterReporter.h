/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack
/// @author Tadej Novak


#ifndef EVENT_BOOKKEEPER_TOOLS__FILTER_REPORTER_H
#define EVENT_BOOKKEEPER_TOOLS__FILTER_REPORTER_H

class EventContext;
class FilterReporterParams;


/// \brief a guard class for use with ref FilterReporterParams
///
/// In order to ensure that the filter decision will always be set
/// as part of `execute()`, this guard class should be used like
/// this:
///
/// ```
/// StatusCode Foo::execute()
/// {
///   ...
///   static constexpr bool DEFAULT_DECISION = false;
///   FilterReporter filter (m_filterParams, DEFAULT_DECISION);
///   ...
///   filter.setPassed ();
///   return StatusCode::SUCCESS;
/// }
/// ```

class FilterReporter final
{
  //
  // public interface
  //

  /// \brief standard constructor
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   out of memory I
public:
  FilterReporter (FilterReporterParams& val_params,
                  bool val_passedDefault);

  /// \brief standard re-entrant constructor
  /// \par Guarantee
  ///   strong
  /// \par Failures
  ///   out of memory I
public:
  FilterReporter (const FilterReporterParams& val_params,
                  bool val_passedDefault,
                  const EventContext& val_eventContext);


  /// \brief standard destructor
  /// \par Guarantee
  ///   no-fail
public:
  ~FilterReporter () noexcept;


  /// \brief report the filter decision
  /// \par Guarantee
  ///   no-fail
public:
  void setPassed (bool val_passed = true) noexcept;



  //
  // private interface
  //

  /// \brief the \ref FilterReporterParams object
private:
  const FilterReporterParams& m_params;

  /// \brief the value of passed we will set
private:
  bool m_passed {false};

  /// \brief current \ref EventContext reference
private:
  const EventContext* m_eventContext{};

};

#endif
