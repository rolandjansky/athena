/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef _ExpressionParserUser_H_
#define _ExpressionParserUser_H_

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <type_traits>
#include <array>
#include "AthenaBaseComps/IDynamicDataConsumer.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataHandle.h"

#include "ExpressionEvaluation/IProxyLoader.h"
#include "ExpressionEvaluation/ExpressionParser.h"

// Forward declarations
namespace ExpressionParsing {
  class SGxAODProxyLoader;
  class MultipleProxyLoader;
  using AppendLoaderFunc = std::function<StatusCode(ExpressionParsing::MultipleProxyLoader &)>;
  inline StatusCode NoLoaderFunc(ExpressionParsing::MultipleProxyLoader &) { return StatusCode::SUCCESS;}

   /// Helper class to handle  an array expression parsers to be used in the generic code
  template <unsigned short NUM_PARSER>
  class MultiParserHelper {
  public:
     /// create an expression parser for each of the given expression strings
     static
     std::array< std::unique_ptr<ExpressionParsing::ExpressionParser>, NUM_PARSER >
     createParser(ExpressionParsing::IProxyLoader &proxy_loader,
                  const std::vector< std::string> &selection_string,
                  unsigned short num_parser);

     /// get the union of all variables used by the given list of expression parsers.
     static
     std::vector<std::string> getVariables(const std::array<std::unique_ptr<ExpressionParsing::ExpressionParser>,NUM_PARSER> &parser);
  };

   /// Helper class to handle a single expression parser to be used in the generic code
  class SingleParserHelper {
  public:
     /// Create a single expression parser for the given expression
     static
     std::unique_ptr<ExpressionParsing::ExpressionParser>
     createParser(ExpressionParsing::IProxyLoader &proxy_loader,
                                     const std::string &selection_string,
                                     unsigned short num_parser);

     /// Get the variables from used by the given expression parser
     static
     std::vector<std::string> getVariables(const std::unique_ptr<ExpressionParsing::ExpressionParser> &parser);

  };

  /// type of a helper class to create n-parser for the given expression or list of expressions.
  template <unsigned short NUM_PARSER>
  using ParserHelper = typename std::conditional< NUM_PARSER == 1, class SingleParserHelper, class MultiParserHelper<NUM_PARSER> >::type;

  /// type to be used for expression string(s) for a single or multiple expression parsers
  template <unsigned short NUM_PARSER>
  using SelectionArg  = typename std::conditional< NUM_PARSER == 1, std::string, std::vector< std::string > >::type;
}


template <class T_Base, unsigned short const NUM_PARSER>
class ExpressionParserUserBase
   : public T_Base,
     virtual public IDynamicDataConsumer
{
public:
   template <typename...Args>
   ExpressionParserUserBase(Args...args) : T_Base(args...) {}
   virtual ~ExpressionParserUserBase() = default;


   StatusCode finalizeParser();

   virtual bool updateDataNeeds(const std::vector<const DataObjID*> &input_data_in,
                                const std::vector<const DataObjID*> &output_data_in,
                                std::vector<Gaudi::DataHandle *> &new_input_handles,
                                std::vector<Gaudi::DataHandle *> &new_output_handles) override;

protected:
   StatusCode _initializeParser(const ExpressionParsing::SelectionArg<NUM_PARSER> &selection_string,
                                ExpressionParsing::AppendLoaderFunc pre_xaod_loader=ExpressionParsing::NoLoaderFunc,
                                ExpressionParsing::AppendLoaderFunc post_xaod_loader=ExpressionParsing::NoLoaderFunc);

   /// The expression parser
   ExpressionParsing::SGxAODProxyLoader                *m_xAODProxyLoader = nullptr;
   std::unique_ptr<ExpressionParsing::IProxyLoader>     m_proxyLoaders;
   typename std::conditional< NUM_PARSER == 1,
                              std::unique_ptr<ExpressionParsing::ExpressionParser>,
                              std::array< std::unique_ptr<ExpressionParsing::ExpressionParser>, NUM_PARSER> >::type m_parser;

};

template <class T_Base, unsigned short const NUM_PARSER = 1>
class ExpressionParserUser :  public ExpressionParserUserBase<T_Base, NUM_PARSER>
{
public:
   template <typename...Args>
   ExpressionParserUser(Args...args) : ExpressionParserUserBase<T_Base,NUM_PARSER>(args...) {}
   StatusCode initializeParser(const ExpressionParsing::SelectionArg<NUM_PARSER> &selection_string);

};
#endif
