/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkBPhys/BSelectionParser.h"

using namespace DerivationFramework;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
BSelectionParser::BSelectionParser(std::string expr, std::string offset) : 
  m_expr(expr),
  m_nodeType(FINAL)
{
  // remove blank spaces
  m_expr.erase(std::remove_if(m_expr.begin(), m_expr.end(), isspace), m_expr.end());

  std::cout << "Info in <BSelectionParser::BSelectionParser>: Expression: " << offset << m_expr << std::endl;
  
  // replace braces with tokens
  std::map<std::string, std::string> tokenMap;
  replaceBraces(m_expr, tokenMap);  
  
  // check if the expression is composite:
  
  // ORs
  if(m_expr.find_first_of('|')!=std::string::npos) {
    m_nodeType = OR;
    // break expression into sub-expressions
    std::vector<std::string> expressions;
    boost::char_separator<char> sep("|");
    boost::tokenizer<boost::char_separator<char> > tok(m_expr,sep);
    for(boost::tokenizer<boost::char_separator<char> >::iterator beg=tok.begin(); beg!=tok.end(); ++beg){
      std::string str_i=(*beg);
      expressions.push_back(str_i);
    }  
    // create daughter node for each sub-expression:
    for(uint i=0; i<expressions.size(); ++i) {
      std::string subExpr = expressions[i];
      putBackBraces(subExpr, tokenMap);
      m_daughters.push_back( new BSelectionParser(subExpr, offset+"  "));
    }
    // end here:
    return;
  }
  
  // ANDs
  if(m_expr.find_first_of('&')!=std::string::npos) {
    m_nodeType = AND;
    // break expression into sub-expressions
    std::vector<std::string> expressions;
    boost::char_separator<char> sep("&");
    boost::tokenizer<boost::char_separator<char> > tok(m_expr,sep);
    for(boost::tokenizer<boost::char_separator<char> >::iterator beg=tok.begin(); beg!=tok.end(); ++beg){
      std::string str_i=(*beg);
      expressions.push_back(str_i);
    }  
    // create daughter node for each sub-expression:
    for(uint i=0; i<expressions.size(); ++i) {
      std::string subExpr = expressions[i];
      putBackBraces(subExpr, tokenMap);
      m_daughters.push_back( new BSelectionParser(subExpr, offset+"  "));
    }
    // end here:
    return;
  }
  
  // are there leftover braces in the expression
  if(!tokenMap.empty()) {
    m_nodeType = OR;
    putBackBraces(m_expr, tokenMap);
    m_daughters.push_back( new BSelectionParser(m_expr, offset+"  "));   
    return;
  }
  
  // expression is simple. Check if it is ok
  if( std::count(m_expr.begin(), m_expr.end(), '.')==1 &&
    ((std::count(m_expr.begin(), m_expr.end(), '=')==1) !=  //XOR
     (std::count(m_expr.begin(), m_expr.end(), '>')==1)))
  {
    // get the collection name
    size_t pos = m_expr.find_first_of('.');
    m_collection     = m_expr.substr(0, pos);
    std::string rest = m_expr.substr(pos+1, std::string::npos);
    // determine flag and count
    std::string countStr;
    if(std::find(rest.begin(), rest.end(), '=')!=rest.end()) {
      m_compType = EQ;
      size_t pos = rest.find_first_of('=');
      m_flag  = rest.substr(0, pos);
      countStr = rest.substr(pos+1, std::string::npos);
    } else {
      m_compType = GT;
      size_t pos = rest.find_first_of('>');
      m_flag  = rest.substr(0, pos);
      countStr = rest.substr(pos+1, std::string::npos);
    }
    std::cout << "Info in <BSelectionParser::BSelectionParser>: " << offset << "  collection: \"" << m_collection << "\"" << std::endl;
    std::cout << "Info in <BSelectionParser::BSelectionParser>: " << offset << "  flag:       \"" << m_flag << "\"" << std::endl;
    try {
      m_count = std::stoi(countStr);
    } catch(...) {
      std::cout<<"Error in <BSelectionParser::BSelectionParser>: Cannot convert string \"" << countStr << "\" to int" <<std::endl;
      throw 1;
    }
    std::cout << "Info in <BSelectionParser::BSelectionParser>: " << offset << "  count:      " << m_count << std::endl;
  }else{
    std::cout<<"Error in <BSelectionParser::BSelectionParser>: Cannot parse the expression " << m_expr <<std::endl;
    throw 1;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
/** evaluate
 */
bool BSelectionParser::eval(ServiceHandle<StoreGateSvc>& evtStore) {
  if(m_nodeType == FINAL) {
    // retrieve the collection
    xAOD::VertexContainer*    container = NULL;
    
    // retrieve from the StoreGate
    evtStore->retrieve(container, m_collection);
    if(!container) {
      std::cout<<"Warning in <BSelectionParser::BSelectionParser>: cannot retrieve container " << m_collection <<std::endl;
      return false;
    }

    //std::cout << "DAN " << container->size() << std::endl;
    
    // loop over candidates:
    SG::AuxElement::Accessor<Char_t> flagAcc(m_flag); 
    int count = 0;
    xAOD::VertexContainer::iterator itr = container->begin();
    for(; itr!=container->end(); ++itr) {
      //std::cout << "DAN   flag = " << int(flagAcc(**itr)) << std::endl;
      if(flagAcc(**itr)) ++count;  // count the ones flagged as passed
    }
    
    // pass the verdict
    //std::cout << "DAN count = " << count << std::endl;
    if(m_compType==EQ) 
      return count==m_count;
    else
      return count>m_count;
    
  } else if(m_nodeType == OR ) {
    // evaluate logical ORs
    for(uint i=0; i<m_daughters.size(); ++i) {
      if(m_daughters[i]->eval(evtStore)) return true;
    }
    return false;
  } else {
    // evaluate logical ANDs
    for(uint i=0; i<m_daughters.size(); ++i) {
      if(!m_daughters[i]->eval(evtStore)) return false;
    }
    return true;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
BSelectionParser::~BSelectionParser() {
  // delete daughters
  for(uint i=0; i<m_daughters.size(); ++i) {
    delete m_daughters[i];
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void BSelectionParser::replaceBraces(std::string& str, std::map<std::string, std::string>& tokenMap) {
  try{
    size_t pos = std::string::npos;
    int exprCount = 0;
    int bracesCount = 0;
    for(size_t i = 0; i<str.length(); ++i) {
      // openning braces
      if(str[i]=='(') {
        // is this the first brace?
        if(bracesCount==0) pos = i;
        bracesCount++;
      }
      
      // closing brace
      if(str[i]==')') {
        // close the braces
        bracesCount--;
        
        // is there too many closing braces?
        if(bracesCount<0) throw 1;
        
        //did we just close the outermost brace?
        if(bracesCount==0) {
          std::string subExpr = str.substr(pos+1, i-pos-1);
          // store in the map:
          tokenMap["#"+std::to_string(exprCount++)] = subExpr;
        }
      }
    } // end of loop over string
    
    // are there unclosed branches?
    if(bracesCount!=0) throw 1;
    
  } catch(...) {
    // handle errors
    std::cout<<"Error in <BSelectionParser::replaceBraces>: unmatched braces? : " << str <<std::endl;
    throw 1;
  }
  
  // replace expressions with tokens
  for(std::map<std::string, std::string>::iterator token = tokenMap.begin(); token!=tokenMap.end(); ++token) 
  {
    // std::cout << "DAN " << str << std::endl;
    // std::cout << "DAN tokenMap[" << token->first << "] = \"" << token->second << "\"" << std::endl;
    std::string what = "("+token->second+")";
    size_t start_pos = str.find(what);
    str.replace(start_pos, what.length(), token->first);
    // std::cout << "DAN replaced expr: " << str << std::endl;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
void BSelectionParser::putBackBraces(std::string& str, std::map<std::string, std::string>& tokenMap) {
  for(std::map<std::string, std::string>::iterator token = tokenMap.begin(); token!=tokenMap.end(); ++token) 
  {
    size_t start_pos = str.find(token->first);
    if(start_pos != std::string::npos)
      str.replace(start_pos, token->first.length(), token->second);
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
