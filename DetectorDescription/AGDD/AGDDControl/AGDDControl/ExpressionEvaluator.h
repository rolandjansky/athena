/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXPRESSION_EVALUATOR_H
#define EXPRESSION_EVALUATOR_H

#include "CLHEP/Evaluator/Evaluator.h"

#include <string>
#include <vector>
#include <map>

typedef std::map< std::string, double> ConstantsTable;
typedef std::map< std::string, double> PhysicalConstantsTable;
typedef std::map< std::string, std::string> ExpressionsTable;

class ExpressionEvaluator
{
public:
	ExpressionEvaluator();
  	~ExpressionEvaluator();

  	bool RegisterConstant( std::string& c, double v );
  	bool RegisterPhysConstant( std::string&, std::string, std::string  );
  	bool RegisterExpression( std::string& c, std::string v  );
  	bool RegisterArray( std::string& c, std::vector<double> v );
  	bool RegisterVariable( const std::string& var_name, double value);
  
  	double EvaluateString(const std::string& str);
  	bool is_delimiter(char c);
  	bool is_real_variable(const std::string& var_name);
  	double Eval( const std::string& expr );
  	double Eval( const char* expr );
	
	static std::string trim(const std::string);
	static std::vector<std::string>& tokenize(const std::string&,const std::string&); 
  
  	void setFileCurrentlyParsed(std::string set)
  	{
   		m_fileCurrentlyParsed = set;
  	}

private:
  	HepTool::Evaluator     m_calc;
  	ConstantsTable         m_CTable;
  	PhysicalConstantsTable m_PCTable;
  	std::string m_fileCurrentlyParsed;
  	std::vector<std::string> m_real_vars;
};

#endif // GDML_EXPRESSION_EVALUATOR_H

