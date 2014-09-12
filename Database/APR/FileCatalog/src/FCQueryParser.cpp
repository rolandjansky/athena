/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FileCatalog/FCQueryParser.h"
#include <locale>
//#include <string>
#include <algorithm>
//#include <iostream>
#include <iterator>
namespace pool {
  struct toUpper{
    toUpper(const std::locale& l):loc(l){}
    char operator()(char c) const { return std::toupper(c,loc); }
  private:
    std::locale const& loc;
  };

  struct isWord{
    bool operator()(const std::string& word) const{
      if(word=="AND"||word=="and"||word=="OR"||word=="or"||
	 word=="LIKE"||word=="like"||word=="BETWEEN"||word=="between"){
	return true;
      }else if(word.find('>')!=word.npos){
	return true;
      }else if(word.find('<')!=word.npos){
	return true;
      }else if(word.find('=')!=word.npos){
	return true;
      }else if(word.find('!')!=word.npos){
	return true;
      }else if(word.find_first_not_of("0123456789")==word.npos){
	return true;
      }else{
	return false;
      }
    }
  };
  FCQueryParser::FCQueryParser( const std::string& whereClause):m_whereClause(whereClause){}
  void FCQueryParser::parse(){
    if(!m_whereClause.empty()){
      //trim white spaces
      this->trim(m_whereClause);
      //std::cout<<"trimed("<<m_whereClause<<")"<<std::endl;
      //remove quoted;
      std::string noquote=this->removeQuoted(m_whereClause);
      //std::cout<<"noquote "<<noquote<<std::endl;
      //chop according to white space;
      std::vector<std::string> chopresult;
      std::vector<std::string> result;
      chop( noquote," =<>", chopresult);
      //remove simbols, numbers and keywords
      std::remove_copy_if(chopresult.begin(),chopresult.end(),
			  std::back_inserter(result),isWord());
      //remove duplicates
      std::sort(result.begin(),result.end());
      std::unique_copy(result.begin(),result.end(),std::back_inserter(m_parseResult));
      //std::copy(m_parseResult.begin(),m_parseResult.end(),std::ostream_iterator<std::string>(std::cout," "));   
      //std::cout<<std::endl;
    }
  }
  
  const std::vector<std::string>& FCQueryParser::parseResult() const{
    return m_parseResult;
  }

  bool FCQueryParser::isMetaQuery() const{
    if( m_whereClause.empty() ){
      return false;
    }
    for(std::vector<std::string>::const_iterator it=m_parseResult.begin();
	it!=m_parseResult.end();++it){
      if((*it)!="pfname"&&(*it)!="lfname"&&(*it)!="guid"){
	return true;
      }
    }
    return false;
  }

  std::string FCQueryParser::removeQuoted(const std::string& phrase) const{
    std::string result;
    bool withinQuotes=false;
    for( std::string::size_type i=0; i!=phrase.length(); ++i ){
      if ( phrase[i] == '\'' ){
	if(withinQuotes){
	  withinQuotes=false;
	}else{
	  withinQuotes=true;
	}
      }else{
	if(!withinQuotes){
	  result.push_back(phrase[i]);
	}
      }
    }
    return result;
  }
 
  /**
     chop a string according to deliminators 
  */
  void FCQueryParser::chop( const std::string& input,
			    const std::string& delim, 
			    std::vector<std::string>& result) const{
    std::string::size_type begIdx,endIdx;
    std::string elem;
    std::string current(input);
    while( (begIdx=current.find_first_not_of(delim))!= current.npos ){
      endIdx=current.find_first_of(delim);
      if(endIdx != current.npos ){
        elem=current.substr(begIdx,endIdx-begIdx);
        current=current.substr(endIdx+1);
        result.push_back(elem);
      }else{
        elem=current;
        result.push_back(elem);
        break;
      }
    }
  }
  /**
     trim white spaces at each end of the string
  */
  void FCQueryParser::trim(std::string& input) const {
    if ( ! input.empty() ) {
      if(*(input.begin())==' '){
        std::string::size_type idx=input.find_first_not_of(' ');
        input=input.substr(idx);
      }
      if(*(input.end()-1)==' '){
        std::string::size_type idx=input.find_last_not_of(' ');
        input=input.substr(0,idx+1);
      }
    }
  }
}//ns pool
