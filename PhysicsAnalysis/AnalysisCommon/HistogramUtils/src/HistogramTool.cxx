/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HistogramTool.h"

#include "ExpressionEvaluation/TriggerDecisionProxyLoader.h"
#include "ExpressionEvaluation/SGxAODProxyLoader.h"
#include "ExpressionEvaluation/SGNTUPProxyLoader.h"
#include "ExpressionEvaluation/MultipleProxyLoader.h"
#include "ExpressionEvaluation/StackElement.h"
#include "GaudiKernel/StatusCode.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this source file makes heavy use of preprocessor macros
// while this make it harder to understand the inner workings, it
// helps to keep the code maintainable, reduces copy & paste errors,
// and helps to keep the code short and clear by avoiding code
// duplication.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// this precompiler macro serves to allow a coherent way of declaring histogram properties to histogram tools
#define DECLARE_HIST_PROPERTIES() \
  declareProperty( "Title",           m_histTitle,         "Histogram Title"); \
  declareProperty( "ClassName",       m_histClassName,     "Histogram Class ('TH[1/2/3][D/F/I]')"); \
  declareProperty( "OnlyAtCutStages", m_cutStages,         "Cut stages to at which this histogram should be filled");  

// this precompiler macro serves to allow a coherent way of declaring axis properties to histogram tools
#define DECLARE_AXIS_PROPERTIES(AXIS) \
  declareProperty(         "Title"#AXIS, m_axis ## AXIS.m_title,                        "Title of "#AXIS"-axis"); \
  declareProperty(           "Var"#AXIS, m_axis ## AXIS.m_variable, "Definition of variable along "#AXIS"-axis"); \
  declareProperty(         "NBins"#AXIS, m_axis ## AXIS.m_nBins,               "Number of bins of "#AXIS"-axis (regular binning)"); \
  declareProperty(           "Min"#AXIS, m_axis ## AXIS.m_min,                        "Minimum of "#AXIS"-axis (regular binning)"); \
  declareProperty(           "Max"#AXIS, m_axis ## AXIS.m_max,                        "Maximum of "#AXIS"-axis (regular binning)"); \
  declareProperty( "BinBoundaries"#AXIS, m_axis ## AXIS.m_binBoundaries,       "Bin boundaries of "#AXIS"-axis (variable binning)");

// this precompiler macro serves to allow a coherent way of calling histogram constructors
#define CREATE_HISTOGRAM(TARGETVARIABLE,CLASSNAME,BASETYPE,CONSTRUCTOR)	\
  if(CLASSNAME == #BASETYPE"F"){        /* call TH*F constructor */	\
    TARGETVARIABLE = new  BASETYPE ## F CONSTRUCTOR ;			\
  } else if(CLASSNAME == #BASETYPE"D"){ /* call TH*D constructor */	\
    TARGETVARIABLE = new  BASETYPE ## D CONSTRUCTOR;			\
  } else if(CLASSNAME == #BASETYPE"I"){	/* call TH*I constructor */	\
    TARGETVARIABLE = new  BASETYPE ## I CONSTRUCTOR ;			\
  } else if(CLASSNAME == #BASETYPE"C"){ /* call TH*C constructor */	\
    TARGETVARIABLE = new  BASETYPE ## C CONSTRUCTOR ;	                \
  } else { /* no match found */                                         \
    ATH_MSG_FATAL("Histogram class '"+CLASSNAME+"' is unknown to this tool!"); \
    return StatusCode::FAILURE;						\
  }

// this precompiler macro should make the histogram constructor calls more readable (for variable binnings)
#define VARBINS(AXIS)							\
  this->m_axis ## AXIS.m_binBoundaries.size(),this->m_axis ## AXIS.m_binBoundaries.data()

// this precompiler macro should make the histogram constructor calls more readable (for fixed binnings)
#define FIXBINS(AXIS)							\
  this->m_axis ## AXIS.m_nBins,this->m_axis ## AXIS.m_min,this->m_axis ## AXIS.m_max

// this precompiler macro checks the axis for validity and should make the initialization code more readable
#define CHECK_AXIS(AXIS)						\
  std::cout << this->name() << ": " << std::endl; this->m_axis ## AXIS.print(); \
  if(this->m_axis ## AXIS.isEmpty()){					\
    ATH_MSG_WARNING("HistogramTool '"+this->name()+"' has empty "#AXIS"-axis definition!"); \
    return StatusCode::SUCCESS;	/* TODO: this is a workaround since initialize is called by retrieve before setting the properties. FIXME! */ \
  } else if(!this->m_axis ## AXIS.isValid()){				\
    ATH_MSG_FATAL("HistogramTool '"+this->name()+"' has invalid "#AXIS"-axis definition!"); \
    return StatusCode::FAILURE;						\
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// default constructor of the AxisDetails struct
HistogramUtils::AxisDetails::AxisDetails():
  m_title(""),
  m_variable(""),
  m_nBins(0),
  m_min(0.),
  m_max(0.)
{
  // empty
}

/// check if an AxidDetails object has variable binning
/**
   \return true if the axis has a variable binning, false otherwise
*/
bool HistogramUtils::AxisDetails::hasVariableBinning() const {
  return (m_binBoundaries.size() > 0);
}


/// print the axis details
void HistogramUtils::AxisDetails::print() const {
  if(m_title.empty())
    std::cout << "[untitled axis]";
  else std::cout << m_title;
  std::cout << " << '" << m_variable << "' : ";
  if(this->hasVariableBinning()){
    for(size_t i=0; i<this->m_binBoundaries.size(); ++i){
      std::cout << m_binBoundaries[i] << " ";
    }
  } else {
    std::cout << this->m_nBins << " bins " << m_min << " -- " << m_max;
  }
  std::cout << std::endl;
}

/// check if an AxisDetails object is valid
/**
   \return true if the axis details are sensible and consistent, false
   otherwise
 */
bool HistogramUtils::AxisDetails::isValid() const {
  if(this->hasVariableBinning()){
    for(size_t i=1; i<m_binBoundaries.size(); i++){
      if(!(m_binBoundaries[i] > m_binBoundaries[i-1]))
	return false;
    }
  } else {
    if(m_max < m_min) return false;
    if(m_nBins < 1) return false;
  }
  return true;
}

/// check if an AxisDetails object is empty
/**
   \return true if the axis details empty
 */

bool HistogramUtils::AxisDetails::isEmpty() const {
  if(this->m_max == 0. && this->m_min == 0. && this->m_nBins==0 && this->m_binBoundaries.size() == 0) return true;
  return false;
}

/// default constructor of a TH1 tool
HistogramUtils::HistogramToolTH1::HistogramToolTH1(const std::string& type, const std::string& name, const IInterface* parent) :
  AthHistogramTool(type,name,parent),
  m_parserX(0)
#ifndef XAOD_ANALYSIS
  , m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
#endif
{
  declareInterface<IAthHistogramTool>( this );
  
  DECLARE_HIST_PROPERTIES();
  DECLARE_AXIS_PROPERTIES(X);
}

/// default constructor of a TH1 tool
HistogramUtils::HistogramToolTH2::HistogramToolTH2(const std::string& type, const std::string& name, const IInterface* parent) :
  AthHistogramTool(type,name,parent),
  m_parserX(0),
  m_parserY(0)
#ifndef XAOD_ANALYSIS
  , m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
#endif
{
  declareInterface<IAthHistogramTool>( this );
  
  DECLARE_HIST_PROPERTIES();
  DECLARE_AXIS_PROPERTIES(X);
  DECLARE_AXIS_PROPERTIES(Y);
}

/// default constructor of a TH1 tool
HistogramUtils::HistogramToolTH3::HistogramToolTH3(const std::string& type, const std::string& name, const IInterface* parent) :
  AthHistogramTool(type,name,parent),
  m_parserX(0),
  m_parserY(0),
  m_parserZ(0)
#ifndef XAOD_ANALYSIS
  , m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
#endif
{
  declareInterface<IAthHistogramTool>( this );
  
  DECLARE_HIST_PROPERTIES();
  DECLARE_AXIS_PROPERTIES(X);
  DECLARE_AXIS_PROPERTIES(Y);
  DECLARE_AXIS_PROPERTIES(Z);
}

/// fill the histogram from the event store
/**
   \param weight event weight to be used
   
   depending on what the expression parser finds in the event store
   for the given expression, the histogram will be filled once per
   event (for scalar values) or multiple times per event (for vector
   values).
 */
StatusCode HistogramUtils::HistogramToolTH1::fill( const double weight){
  ExpressionParsing::StackElement valX = m_parserX->evaluate();
  if(valX.isScalar()){
    // we found a scalar, we fill it into the histogram with the given
    // weight
    this->m_histogram->Fill(valX.scalarValue<double>(),weight);
  } else if(valX.isVector()){
    // we found a vector, we fill each entry into the histogram with
    // the given weight
    std::vector<double> vecX(valX.vectorValue<double>());
    auto end(vecX.end());
    for(auto itr = vecX.begin(); itr != end; itr++){
      this->m_histogram->Fill(*itr,weight);
    }
  } else {
    // what we found in the event store is neither a scalar nor a vector
    // it must be of some awkward type that can't be filled in a histogram
    // therefore, we fail gracefully
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// initialize this tool
/**
   will perform various initialization tasks:
     - check validity of axis definitions
     - instantiate and initialze helper classes for expression parsing
     - prepare histogram name
     - instantiate histogram according to axis definitions
     - register histogram with THistSvc

   \return SUCCESS if everything went well, FAILURE otherwise
*/
StatusCode HistogramUtils::HistogramToolTH1::initialize(){
  // check the axis details
  CHECK_AXIS(X);
  
  // initialize proxy loaders for expression parsing
  ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
#ifndef XAOD_ANALYSIS
  proxyLoaders->push_back(new ExpressionParsing::TriggerDecisionProxyLoader(m_trigDecisionTool));
#endif
  proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
  proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
  
  // load the expressions
  this->m_parserX = new ExpressionParsing::ExpressionParser(proxyLoaders);
  this->m_parserX->loadExpression(this->m_axisX.m_variable);
  
  // process histogram name
  std::string name(this->name());
  while(name.find(".") != std::string::npos){
    name.replace(name.find("."),1,"/");
  }

  // instantiate histogram
  TH1* hist(0);
  if(this->m_axisX.hasVariableBinning()){
    CREATE_HISTOGRAM(hist,this->m_histClassName,TH1,(name.c_str(),this->m_histTitle.c_str(), 
						     VARBINS(X)));
  } else {
    CREATE_HISTOGRAM(hist,this->m_histClassName,TH1,(name.c_str(),this->m_histTitle.c_str(), 
						     FIXBINS(X)));
  }

  if(((TString)(hist->GetXaxis()->GetTitle())).IsNull()) hist->GetXaxis()->SetTitle(m_axisX.m_variable.c_str());

  // register histogram with THistSvc
  hist->SetDirectory(NULL);
  this->m_histogram = this->bookGetPointer( *(hist) );
  if(!m_histogram) return StatusCode::FAILURE;

  ATH_MSG_DEBUG("m_histname: " << m_histogram->GetName());
  ATH_MSG_DEBUG("m_prefix: " << m_prefix );
  ATH_MSG_DEBUG("m_rootDir: " << m_rootDir );
  ATH_MSG_DEBUG("m_histNamePrefix: " << m_histNamePrefix );
  ATH_MSG_DEBUG("m_histNamePostfix: " << m_histNamePostfix );
  ATH_MSG_DEBUG("m_histTitlePrefix: " << m_histTitlePrefix );
  ATH_MSG_DEBUG("m_histTitlePostfix: " << m_histTitlePostfix );

  return StatusCode::SUCCESS;
}

StatusCode HistogramUtils::HistogramToolTH1::finalize(){
  return StatusCode::SUCCESS;
}

/// fill the histogram from the event store
/**
   \param weight event weight to be used
   
   depending on what the expression parser finds in the event store
   for the given expression, the histogram will be filled once per
   event (for scalar values) or multiple times per event (for vector
   values).
 */
StatusCode HistogramUtils::HistogramToolTH2::fill( const double weight){
  ExpressionParsing::StackElement valX = m_parserX->evaluate();
  ExpressionParsing::StackElement valY = m_parserY->evaluate();
  if(valX.isScalar()){
    if(valY.isScalar()){
      this->m_histogram->Fill(valX.scalarValue<double>(),valY.scalarValue<double>(),weight);
    } else if(valY.isVector()){
      double tmpX = valX.scalarValue<double>();
      std::vector<double> vecY(valY.vectorValue<double>());
      auto end(vecY.end());
      for(auto itr = vecY.begin(); itr != end; ++itr){
	this->m_histogram->Fill(tmpX,*itr,weight);
      }
    } else {
      return StatusCode::FAILURE;
    }
  } else if(valX.isVector()){
    if(valY.isScalar()){
      double tmpY = valY.scalarValue<double>();
      std::vector<double> vecX(valX.vectorValue<double>());
      auto end(vecX.end());
      for(auto itr = vecX.begin(); itr != end; ++itr){
	this->m_histogram->Fill(*itr,tmpY,weight);
      }
    } else if(valY.isVector()){
      std::vector<double> vecX(valX.vectorValue<double>());
      std::vector<double> vecY(valY.vectorValue<double>());
      auto endX(vecX.end());
      auto endY(vecY.end());
      for(auto itrX = vecX.begin(); itrX != endX; ++itrX){
	for(auto itrY = vecY.begin(); itrY != endY; ++itrY){
	  this->m_histogram->Fill(*itrX,*itrY,weight);
	}
      }
    } else {
      return StatusCode::FAILURE;
    }
  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// initialize this tool
/**
   will perform various initialization tasks:
     - check validity of axis definitions
     - instantiate and initialze helper classes for expression parsing
     - prepare histogram name
     - instantiate histogram according to axis definitions
     - register histogram with THistSvc

   \return SUCCESS if everything went well, FAILURE otherwise
*/
StatusCode HistogramUtils::HistogramToolTH2::initialize(){
  // check the axis details
  CHECK_AXIS(X);
  CHECK_AXIS(Y);
  
  // initialize proxy loaders for expression parsing
  ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
#ifndef XAOD_ANALYSIS
  proxyLoaders->push_back(new ExpressionParsing::TriggerDecisionProxyLoader(m_trigDecisionTool));
#endif
  proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
  proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
  
  // load the expressions
  this->m_parserX = new ExpressionParsing::ExpressionParser(proxyLoaders);
  this->m_parserX->loadExpression(this->m_axisX.m_variable);
  this->m_parserY = new ExpressionParsing::ExpressionParser(proxyLoaders);
  this->m_parserY->loadExpression(this->m_axisY.m_variable);
  
  // process histogram name
  std::string name(this->name());
  while(name.find(".") != std::string::npos){
    name.replace(name.find("."),1,"/");
  }

  // instantiate histogram
  TH2* hist(0);
  if(this->m_axisX.hasVariableBinning()){
    if(this->m_axisY.hasVariableBinning()){
      CREATE_HISTOGRAM(hist,this->m_histClassName,TH2,(name.c_str(),this->m_histTitle.c_str(), 
						       VARBINS(X),
						       VARBINS(Y)));
    } else {
      CREATE_HISTOGRAM(hist,this->m_histClassName,TH2,(name.c_str(),this->m_histTitle.c_str(), 
						       VARBINS(X),
						       FIXBINS(Y)));
    }
  } else {
    if(this->m_axisY.hasVariableBinning()){
      CREATE_HISTOGRAM(hist,this->m_histClassName,TH2,(name.c_str(),this->m_histTitle.c_str(), 
						       FIXBINS(X),
						       VARBINS(Y)));
    } else {
      CREATE_HISTOGRAM(hist,this->m_histClassName,TH2,(name.c_str(),this->m_histTitle.c_str(), 
						       FIXBINS(X),
						       FIXBINS(Y)));
    }
  }
  if(((TString)(hist->GetXaxis()->GetTitle())).IsNull()) hist->GetXaxis()->SetTitle(m_axisX.m_variable.c_str());
  if(((TString)(hist->GetYaxis()->GetTitle())).IsNull()) hist->GetYaxis()->SetTitle(m_axisY.m_variable.c_str());


  // register histogram with THistSvc
  hist->SetDirectory(NULL);
  this->m_histogram = dynamic_cast<TH2*>(this->bookGetPointer( *(hist), this->m_rootDir, this->m_prefix ));
  if(!m_histogram) return StatusCode::FAILURE;

  ATH_MSG_DEBUG("m_histname: " << m_histogram->GetName());
  ATH_MSG_DEBUG("m_prefix: " << m_prefix );
  ATH_MSG_DEBUG("m_rootDir: " << m_rootDir );
  ATH_MSG_DEBUG("m_histNamePrefix: " << m_histNamePrefix );
  ATH_MSG_DEBUG("m_histNamePostfix: " << m_histNamePostfix );
  ATH_MSG_DEBUG("m_histTitlePrefix: " << m_histTitlePrefix );
  ATH_MSG_DEBUG("m_histTitlePostfix: " << m_histTitlePostfix );

  return StatusCode::SUCCESS;
}

StatusCode HistogramUtils::HistogramToolTH2::finalize(){
  return StatusCode::SUCCESS;
}

/// fill the histogram from the event store
/**
   \param weight event weight to be used
   
   depending on what the expression parser finds in the event store
   for the given expression, the histogram will be filled once per
   event (for scalar values) or multiple times per event (for vector
   values).
 */

StatusCode HistogramUtils::HistogramToolTH3::fill( const double weight){
  ExpressionParsing::StackElement valX = m_parserX->evaluate();
  ExpressionParsing::StackElement valY = m_parserY->evaluate();
  ExpressionParsing::StackElement valZ = m_parserZ->evaluate();
  if(valX.isScalar()){
    if(valY.isScalar()){
      if(valZ.isScalar()){
	this->m_histogram->Fill(valX.scalarValue<double>(),valY.scalarValue<double>(),weight);
      } else if(valZ.isVector()){
	double tmpX = valX.scalarValue<double>();
	double tmpY = valY.scalarValue<double>();
	std::vector<double> vecZ(valZ.vectorValue<double>());
	auto end(vecZ.end());
	for(auto itr = vecZ.begin(); itr != end; ++itr){
	  this->m_histogram->Fill(tmpX,tmpY,*itr,weight);
	}
      } else {
	return StatusCode::FAILURE;
      }
    } else if(valY.isVector()){
      if(valZ.isScalar()){
	double tmpX = valX.scalarValue<double>();
	std::vector<double> vecY(valY.vectorValue<double>());
	double tmpZ = valZ.scalarValue<double>();
	auto end(vecY.end());
	for(auto itr = vecY.begin(); itr != end; ++itr){
	  this->m_histogram->Fill(tmpX,*itr,tmpZ,weight);
	}
      } else if(valZ.isVector()){
	double tmpX = valX.scalarValue<double>();
	std::vector<double> vecY(valY.vectorValue<double>());
	std::vector<double> vecZ(valZ.vectorValue<double>());
	auto endY(vecY.end());
	auto endZ(vecZ.end());
	for(auto itrY = vecY.begin(); itrY != endY; ++itrY){
	  for(auto itrZ = vecZ.begin(); itrZ != endZ; ++itrZ){
	    this->m_histogram->Fill(tmpX,*itrY,*itrZ,weight);
	  }
	}
      } else {
	return StatusCode::FAILURE;
      }
    } else {
      return StatusCode::FAILURE;
    }
  } else if(valX.isVector()){
    if(valY.isScalar()){
      if(valZ.isScalar()){
	std::vector<double> vecX(valX.vectorValue<double>());
	double tmpY = valY.scalarValue<double>();
	double tmpZ = valZ.scalarValue<double>();
	auto end(vecX.end());
	for(auto itr = vecX.begin(); itr != end; ++itr){
	  this->m_histogram->Fill(*itr,tmpY,tmpZ,weight);
	}
      } else if(valZ.isVector()){
	std::vector<double> vecX(valX.vectorValue<double>());
	double tmpY = valY.scalarValue<double>();
	std::vector<double> vecZ(valZ.vectorValue<double>());
	auto endX(vecX.end());
	auto endZ(vecZ.end());
	for(auto itrX = vecX.begin(); itrX != endX; ++itrX){
	  for(auto itrZ = vecZ.begin(); itrZ != endZ; ++itrZ){
	    this->m_histogram->Fill(*itrX,tmpY,*itrZ,weight);
	  }
	}
      } else {
	return StatusCode::FAILURE;
      }
    } else if(valY.isVector()){
      if(valZ.isScalar()){
	std::vector<double> vecX(valX.vectorValue<double>());
	std::vector<double> vecY(valY.vectorValue<double>());
	double tmpZ = valZ.scalarValue<double>();
	auto endX(vecX.end());
	auto endY(vecY.end());
	for(auto itrX = vecX.begin(); itrX != endX; ++itrX){
	  for(auto itrY = vecY.begin(); itrY != endY; ++itrY){
	    this->m_histogram->Fill(*itrX,*itrY,tmpZ,weight);
	  }
	}
      } else if(valZ.isVector()){
	std::vector<double> vecX(valX.vectorValue<double>());
	std::vector<double> vecY(valY.vectorValue<double>());
	std::vector<double> vecZ(valZ.vectorValue<double>());
	auto endX(vecX.end());
	auto endY(vecY.end());
	auto endZ(vecZ.end());
	for(auto itrX = vecX.begin(); itrX != endX; ++itrX){
	  for(auto itrY = vecY.begin(); itrY != endY; ++itrY){
	    for(auto itrZ = vecZ.begin(); itrZ != endZ; ++itrZ){
	      this->m_histogram->Fill(*itrX,*itrY,*itrZ,weight);
	    }
	  }
	}
      } else {
	return StatusCode::FAILURE;
      }
    } else {
      return StatusCode::FAILURE;
    }
  } else {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// initialize this tool
/**
   will perform various initialization tasks:
     - check validity of axis definitions
     - instantiate and initialze helper classes for expression parsing
     - prepare histogram name
     - instantiate histogram according to axis definitions
     - register histogram with THistSvc

   \return SUCCESS if everything went well, FAILURE otherwise
*/
StatusCode HistogramUtils::HistogramToolTH3::initialize(){
  // check axis
  CHECK_AXIS(X);
  CHECK_AXIS(Y);
  CHECK_AXIS(Z);
  
  // initialize proxy loaders for expression parsing
  ExpressionParsing::MultipleProxyLoader *proxyLoaders = new ExpressionParsing::MultipleProxyLoader();
#ifndef XAOD_ANALYSIS
  proxyLoaders->push_back(new ExpressionParsing::TriggerDecisionProxyLoader(m_trigDecisionTool));
#endif
  proxyLoaders->push_back(new ExpressionParsing::SGxAODProxyLoader(evtStore()));
  proxyLoaders->push_back(new ExpressionParsing::SGNTUPProxyLoader(evtStore()));
  
  // load the expressions
  this->m_parserX = new ExpressionParsing::ExpressionParser(proxyLoaders);
  this->m_parserX->loadExpression(this->m_axisX.m_variable);
  this->m_parserY = new ExpressionParsing::ExpressionParser(proxyLoaders);
  this->m_parserY->loadExpression(this->m_axisY.m_variable);
  this->m_parserZ = new ExpressionParsing::ExpressionParser(proxyLoaders);
  this->m_parserZ->loadExpression(this->m_axisZ.m_variable);
  
  // process histogram name
  std::string name(this->name());
  while(name.find(".") != std::string::npos){
    name.replace(name.find("."),1,"/");
  }

  // instantiate histogram
  TH3* hist(0);
  if(this->m_axisX.hasVariableBinning() && this->m_axisY.hasVariableBinning() && this->m_axisZ.hasVariableBinning()){
    CREATE_HISTOGRAM(hist,this->m_histClassName,TH3,(name.c_str(),this->m_histTitle.c_str(), 
						     VARBINS(X),
						     VARBINS(Y),
						     VARBINS(Z)));
  } else if(!this->m_axisX.hasVariableBinning() && !this->m_axisY.hasVariableBinning() && !this->m_axisZ.hasVariableBinning()){
    CREATE_HISTOGRAM(hist,this->m_histClassName,TH3,(name.c_str(),this->m_histTitle.c_str(), 
						     FIXBINS(X),
						     FIXBINS(Y),
						     FIXBINS(Z)));
  } else {
    ATH_MSG_FATAL("TH3-type histograms can only be constructed with fixed bins on all axis or variable bins on all axis - mixed binnings are not supported!");
    return StatusCode::FAILURE;
  }

  if(((TString)(hist->GetXaxis()->GetTitle())).IsNull()) hist->GetXaxis()->SetTitle(m_axisX.m_variable.c_str());
  if(((TString)(hist->GetYaxis()->GetTitle())).IsNull()) hist->GetYaxis()->SetTitle(m_axisY.m_variable.c_str());
  if(((TString)(hist->GetZaxis()->GetTitle())).IsNull()) hist->GetZaxis()->SetTitle(m_axisZ.m_variable.c_str());

  // register histogram with THistSvc
  hist->SetDirectory(NULL);
  this->m_histogram = dynamic_cast<TH3*>(this->bookGetPointer( *(hist), this->m_rootDir, this->m_prefix ));
  if(!m_histogram) return StatusCode::FAILURE;

  ATH_MSG_DEBUG("m_histname: " << m_histogram->GetName());
  ATH_MSG_DEBUG("m_prefix: " << m_prefix );
  ATH_MSG_DEBUG("m_rootDir: " << m_rootDir );
  ATH_MSG_DEBUG("m_histNamePrefix: " << m_histNamePrefix );
  ATH_MSG_DEBUG("m_histNamePostfix: " << m_histNamePostfix );
  ATH_MSG_DEBUG("m_histTitlePrefix: " << m_histTitlePrefix );
  ATH_MSG_DEBUG("m_histTitlePostfix: " << m_histTitlePostfix );

  return StatusCode::SUCCESS;
}

StatusCode HistogramUtils::HistogramToolTH3::finalize(){
  return StatusCode::SUCCESS;
}

// TODO: implement destructor
// TODO: implement finalize
// TODO: implement copy constructor 
// TODO: investigate use of shared resources for ExpressionParsing classes
