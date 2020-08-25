/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/ExpressionEvaluator.h"
#include "AGDDControl/AGDDTokenizer.h"

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include <algorithm>

ExpressionEvaluator::ExpressionEvaluator()
{
  m_calc.clear();
  m_calc.setStdMath();                 // set standard constants and functions
  // Set Geant4 system of units
  m_calc.setSystemOfUnits(1.e+3, 1./1.60217733e-25, 1.e+9, 1./1.60217733e-10,1.0, 1.0, 1.0);
  m_fileCurrentlyParsed = "";
}

ExpressionEvaluator::~ExpressionEvaluator()
{
  m_CTable.clear();
  m_PCTable.clear();
  m_calc.clear();
}

bool ExpressionEvaluator::RegisterConstant( std::string& c, double v )
{
  double value = v;

  if( m_calc.status() != HepTool::Evaluator::OK )
  {
    std::cerr << "Expression evaluator:: Error registering constant " << c << std::endl;
    m_calc.print_error();
    std::cout << std::endl;
    return false;
  }

  RegisterVariable( c, value );
  return true;
}
bool ExpressionEvaluator::RegisterArray( std::string& c, std::vector<double> v)
{  
   for(unsigned int i=0; i<v.size(); i++)
   {
    double value = v[i];
    std::stringstream ss;
    std::string index;
    ss << i;
    ss >> index;
    std::string name = c+"_"+index+"_";
    RegisterVariable( name, value );       
   }
   return true;
}

bool ExpressionEvaluator::RegisterVariable( const std::string& var_name, double value)
{
 m_real_vars.push_back(var_name);
 m_calc.setVariable( var_name.c_str(), value );
 return true;
}

bool ExpressionEvaluator::is_real_variable(const std::string& var_name)
{
 
 std::vector<std::string>::iterator start = m_real_vars.begin();
 std::vector<std::string>::iterator end = m_real_vars.end();
 std::vector<std::string>::iterator iter;

 iter = find(start, end, var_name);
 
 if (iter == end)
 {
  return false;
 }
 else
 {
  return true;
 }
}

bool ExpressionEvaluator::is_delimiter(char c)
{
 if(strchr(" ()|&=!><+-%*/^", c) || c==9 || c==0 || c=='\r' || c=='\n' || c=='\t' || c=='\0' || isspace(c)) return true;
 else return false;
}

double ExpressionEvaluator::EvaluateString(const std::string& str)
{
 std::string str_mod = str;
  const char* c_str_mod = str.c_str(); //string to be modified with file namespace! 
  std::vector<int> variable_ends;  //variable names to be changed
  int cur_variable_end = 0;
  while(*c_str_mod)
  {
   if(is_delimiter(*c_str_mod) || isdigit(*c_str_mod))
   {
    c_str_mod++;
    cur_variable_end++;
   }
   else if(isalpha(*c_str_mod))
   {
    char variable[80];
    char* token;
    token = variable;
    *token = '\0';
    while(!is_delimiter(*c_str_mod))
    {
     *token=*c_str_mod;
     token++;
     c_str_mod++;
     cur_variable_end++;
    }
    *token = '\0';
    std::string variable_to_check = variable;
    if(is_real_variable(variable_to_check))
    {
     variable_ends.push_back(cur_variable_end);
    }
   }
   else
   {
    c_str_mod++;
    cur_variable_end++;
   }
  std::string::size_type shift = 0;
  std::string::size_type ns_length = m_fileCurrentlyParsed.size();
  for(unsigned int i=0; i<variable_ends.size(); i++)
  {
   str_mod.insert(shift+variable_ends[i],m_fileCurrentlyParsed);
   shift += ns_length;
  }
 }
 double result = m_calc.evaluate( str_mod.c_str() );
 return result;
}
/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/
/*********************************************************************************************************/


bool ExpressionEvaluator::RegisterPhysConstant( std::string& c, std::string value, std::string unit  )
{
  std::string expr = value;
  expr += "*(";
  expr += unit;
  expr += ")";

  double dvalue      = EvaluateString( expr );

  if( m_calc.status() != HepTool::Evaluator::OK )
  {
    std::cerr << "Expression evaluator:: Error registering quantity "
              << c << std::endl;
    m_calc.print_error();
    std::cout << std::endl;
    return false;
  }

  RegisterVariable( c, dvalue );
  return true;
}

bool ExpressionEvaluator::RegisterExpression( std::string& name, std::string text )
{
  std::string expr = "(";
  expr += text;
  expr += ")";
  double value = EvaluateString( expr );

  if( m_calc.status() != HepTool::Evaluator::OK )
  {
    std::cerr << "Expression evaluator:: Error registering expression " << name << std::endl;
    m_calc.print_error();
    std::cout << std::endl;
    return false;
  }

  RegisterVariable( name, value );
  return true;
}

double ExpressionEvaluator::Eval( const std::string& expr )
{
  return Eval( expr.c_str() );
}

double ExpressionEvaluator::Eval( const char* expr_mod )
{
  
  std::string expr = expr_mod;
  std::string::size_type start_index = 0;
  std::string::size_type end_index = 0;
  while(true)
  {
   start_index = expr.find('[', start_index); 
   if(start_index == std::string::npos) break;
   std::string::size_type boundary_index = expr.find(']', start_index);
   expr.replace(start_index,1,1,'_');
   end_index = expr.find(',', start_index);
   if(end_index != std::string::npos && end_index < boundary_index)
   {
    start_index++;
    std::string var1 = expr.substr(start_index, end_index-start_index);
    double eval1 = EvaluateString( var1 );
    std::stringstream ss1;
    std::string str1;
    ss1 << eval1;
    ss1 >> str1;
    expr.replace(start_index, end_index-start_index, str1, 0, str1.size());
   }
   else
   {
    end_index = boundary_index;
    if(end_index != std::string::npos)
    {
     start_index++;
     std::string var1 = expr.substr(start_index, end_index-start_index);
     double eval1 = EvaluateString( var1 );
     std::stringstream ss1;
     std::string str1;
     ss1 << eval1;
     ss1 >> str1;
     expr.replace(start_index, end_index-start_index, str1, 0, str1.size());
    }
   }
  }
  start_index = 0;
  end_index = 0;
  while(true)
  {
   start_index = expr.find(',', start_index);
   if(start_index == std::string::npos) break;
   expr.replace(start_index,1,1,'_');
   end_index = expr.find(']', start_index);
   start_index++;
   std::string var2 = expr.substr(start_index, end_index-start_index);
   double eval2 = EvaluateString( var2 );
   std::stringstream ss2;
   std::string str2;
   ss2 << eval2;
   ss2 >> str2;
   expr.replace(start_index, end_index-start_index, str2, 0, str2.size());
  }
  start_index = 0;
  end_index = 0;
  while(true)
  {
   start_index = expr.find(']', start_index);
   if(start_index == std::string::npos) break;
   expr.replace(start_index,1,1,'_');
  }
  double result = EvaluateString( expr );
  if( m_calc.status() != HepTool::Evaluator::OK )
  {
    std::cerr << expr << std::endl;
    for (int i=0; i<m_calc.error_position(); i++)
    {
      std::cerr << "-";
    }
    std::cerr << "^\a" << std::endl;
    m_calc.print_error();
    std::cerr << std::endl;
  }
  return result;
}

std::string ExpressionEvaluator::trim (const std::string s)
//-------------------------------------------------------------
{
        static const std::string null_string;
        if (s.size () == 0) return (s);
        static std::string temp = "";
	temp=s;
        std::string::size_type i;
        i = temp.find_first_not_of (' ');
        if (i == std::string::npos) return (null_string);
// There is at least 1 non blank character in s.
        if (i > 0)
        {
                temp = temp.substr (i);
        }
        i = temp.find_last_not_of (' ');
        if (i < temp.size ())
        {
                temp = temp.substr (0, i + 1);
        }
        return (temp);
}
    
std::vector<std::string>& ExpressionEvaluator::tokenize(const std::string& sep,const std::string& expr)
{
	static std::vector<std::string> tempvect;
	tempvect.clear();
	AGDDTokenizer aa(sep,expr);
	for (unsigned int i=0;i<aa.size();i++) tempvect.push_back(trim(aa[i]));
	return tempvect;
}
