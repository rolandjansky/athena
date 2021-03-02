#include "ExpressionEvaluation/ExpressionParserUser.h"
#include "ExpressionEvaluation/ExpressionParserUser.icc"
#include "ExpressionEvaluation/ExpressionParserUserWithTrigSupport.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthFilterAlgorithm.h"
#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

template class ExpressionParserUserBase<AthAlgTool,1>;
template class ExpressionParserUserBase<AthFilterAlgorithm,1>;
template class ExpressionParserUserBase<AthAnalysisAlgorithm,1>;
template class ExpressionParserUser<AthAlgTool>;
template class ExpressionParserUser<AthFilterAlgorithm>;
template class ExpressionParserUser<AthAnalysisAlgorithm>;

// multi parser users
template class ExpressionParserUserBase<AthAlgTool,2>;
template class ExpressionParserUserBase<AthFilterAlgorithm,2>;
template class ExpressionParserUserBase<AthAnalysisAlgorithm,2>;
template class ExpressionParserUser<AthAlgTool,2>;
template class ExpressionParserUser<AthFilterAlgorithm,2>;
template class ExpressionParserUser<AthAnalysisAlgorithm,2>;
