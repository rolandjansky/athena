/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************
Created by: Marc Hohlfeld
****************************************************/
#ifndef MONTECARLOREACT_EFFICIENCY_H
#define MONTECARLOREACT_EFFICIENCY_H


#include <iostream>
#include <string>
#include <vector>

#include "MonteCarloReactUtils/EffInfo.h"
#include "MonteCarloReactUtils/EffVal.h"

namespace MonteCarloReact {
  class Efficiency {

  public:

    /* Default constructor */
    Efficiency();

    /* constructor with a stream as input */
    Efficiency( std::istream & fstr, const EffInfo* request=0); 

    /* Destructor */
    virtual ~Efficiency();

    /* Functions implemented by the derived classes */
    virtual EffVal getEff( float x) const=0;
    virtual EffVal getEff( float x, float y) const=0;
    virtual EffVal getEff( float x, float y, float z)const=0;
    virtual EffVal getEff( const std::vector<float>&  value) const=0;
    
    /* return true if exactly matched request found in file */
    bool isRequestInFile(std::istream& fp, const EffInfo* request) ;

    
    /* Validation method */
    virtual bool validateInput() const  { return true; }
    
    /* Return true if Efficiency reqquest is valid */
    bool isValid() const { return m_isValid;}

    /* Return the EfficiencyInfo */
    const EffInfo & getInfo() const { return m_spec;}

    /* Set the EfficiencyInfo */
    void setInfo(EffInfo & spec );
    
    virtual void clear() {m_isValid = false ; m_spec.clear();}

  protected:
  
    EffInfo m_spec;
    bool m_isValid;
    
    void m_doStream( std::ostream & os ) const;  

    bool m_makeEfficiency(std::istream& fstr, const EffInfo* request=0);
    
    virtual void m_stream( std::ostream & os ) const =0;
    
    virtual bool m_parseInputLine( const std::string& ,
                                   const std::vector<std::string> & /* line */) { return false;}
     
  
  private:
  
    /* parser for istream object */
    bool m_effParse(std::istream& fstr, const EffInfo* request=0);

    /* get indiviaul contents of a line in the efficiency file */
    bool m_tokenize(const std::string &, std::string &,
                    std::vector<std::string>&, const std::string & delimiters = " ");

    /* validation methods */
    bool m_doValidateInput() const ;
    
    friend std::ostream & operator << (std::ostream& os, const Efficiency& s);
  
  };
}

inline std::ostream& MonteCarloReact::operator << (std::ostream& os,
                                                   const MonteCarloReact::Efficiency & eff) {
  eff.m_doStream(os);
  return os;
}

#endif  // MONTECARLOREACT_EFFICIENCY_H
