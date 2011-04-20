/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************
Created by: Marc Hohlfeld
****************************************************/
#ifndef MONTECARLOREACT_RESOLUTION_H
#define MONTECARLOREACT_RESOLUTION_H

#include "MonteCarloReactUtils/ResInfo.h"

#include <iostream>
#include <string>
#include <vector>
#include <TF1.h>
#include <TF2.h>

namespace MonteCarloReact {

  class Resolution {

  public:

    /* Constructor */
    Resolution();
   
    Resolution( std::istream & fstr, const ResInfo* request=0);

    virtual ~Resolution();			

    /* 1D only */
    virtual void getFunc(TF1& f1,  float x) const=0;
    /* 2D only */
    virtual void getFunc(TF1& f1, float x, float y) const=0;
    /* any  number of dimensions */
    virtual void getFunc(TF1& f1,  const std::vector<float>&  value) const=0;
    /* 1D only */
    virtual void getFunc(TF2& f2,  float x) const=0;
    /* 2D only */
    virtual void getFunc(TF2& f2, float x, float y) const=0;
    /* any  number of dimensions */
    virtual void getFunc(TF2& f2,  const std::vector<float>&  value) const=0;

    
    /* 1D only */
    virtual double getFuncVal( float x, float xval) const=0;
    /* 2D only */
    virtual double getFuncVal( float x, float y, float xval) const=0;  
    /* any  number of dimensions */
    virtual double getFuncVal( const std::vector<float>&  value, float xval) const=0;

  
    /* return true if exactly matched request found in file */
    bool isRequestInFile(std::istream& fp, const ResInfo* request) ;

    bool isValid() const { return m_isValid;}

    const ResInfo & getInfo() const { return m_spec;}

    void setInfo( const ResInfo & spec ) { m_spec = spec;}
    
    virtual void clear() {m_isValid = false ; m_spec.clear();}

  protected:
  
    ResInfo m_spec;
    bool m_isValid;
  
    virtual void m_stream( std::ostream & os ) const =0;

    void m_doStream( std::ostream & os ) const;  

    bool m_makeResolution(std::istream& fstr, const ResInfo* request=0); 
  
    virtual bool m_parseInputLine( const std::string& ,
                                   const std::vector<std::string> & /* line */) { return false;}
    
  private:

    bool m_resParse(std::istream& fstr, const ResInfo* request=0);

    bool m_tokenize(const std::string &, std::string &,
                  std::vector<std::string>&, const std::string & delimiters = " ");
   
    bool m_doValidateInput() const ;
  
    friend std::ostream & operator << (std::ostream& os, const Resolution& s);

    virtual bool m_validateInput() const  { return true; }
  };
}

inline std::ostream& MonteCarloReact::operator << (std::ostream& os, const MonteCarloReact::Resolution & res) {
  res.m_doStream(os);
  return os;
}

#endif  // RESOLUTION_HPP
