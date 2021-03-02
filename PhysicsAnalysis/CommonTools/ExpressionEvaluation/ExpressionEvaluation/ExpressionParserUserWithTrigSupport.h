/* Dear emacs, this is -*-c++-*- */
#ifndef _ExpressionParserUserWithTrigSupport_H_
#define _ExpressionParserUserWithTrigSupport_H_

#include "ExpressionParserUser.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "ExpressionEvaluation/TriggerDecisionProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"

template <class T_Base, unsigned short const NUM_PARSER=1>
class ExpressionParserUserWithTrigSupport :  public ExpressionParserUserBase<T_Base,NUM_PARSER>
{
public:
   template <typename...Args>
   ExpressionParserUserWithTrigSupport(Args...args)
      : ExpressionParserUserBase<T_Base,NUM_PARSER>(args...),
        m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
   {
      this->declareProperty("TrigDecisionTool", m_trigDecisionTool,"");
   }
   StatusCode initializeParser(ExpressionParsing::SelectionArg<NUM_PARSER> selection_string) {
      return this->_initializeParser(selection_string,
                                     [this](ExpressionParsing::MultipleProxyLoader &proxy_loaders) -> StatusCode {
                                        // initialise TDT explicitly, needed for the tool to properly work with trigger decisions in AthAnalysisBase (until fixed)
                                        if( !this->m_trigDecisionTool.empty() ) {
                                           StatusCode sc = this->m_trigDecisionTool.retrieve();
                                           if (sc.isFailure()) return sc;
                                           proxy_loaders.push_back(new ExpressionParsing::TriggerDecisionProxyLoader(this->m_trigDecisionTool));
                                        }
                                        else {
                                           this->m_trigDecisionTool.disable();
                                        }
                                        return StatusCode::SUCCESS;
                                     },
                                     [this](ExpressionParsing::MultipleProxyLoader &proxy_loaders) -> StatusCode {
                                        proxy_loaders.push_back(new ExpressionParsing::SGNTUPProxyLoader(this->evtStore()));
                                        return StatusCode::SUCCESS;
                                     });
   }
protected:
  /// The trigger decision tool
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
};
#endif
