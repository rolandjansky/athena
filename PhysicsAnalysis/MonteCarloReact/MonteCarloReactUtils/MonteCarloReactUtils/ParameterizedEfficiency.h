/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MONTECARLOREACTUTILS_PARAMETRIZEDEFFICIENCY_H
#define MONTECARLOREACTUTILS_PARAMETRIZEDEFFICIENCY_H

namespace MonteCarloReact {

  /* Class to get efficiencies from a parametrization
     This needs to be implemented but might already be
     superseded by ShiftFunction */
  class ParameterizedEfficiency : public Efficiency { 
  public:
    /* Constructors */
    ParameterizedEfficiency() : Efficiency() {;}
    ParameterizedEfficiency( std::istream & ist, const EffInfo* request=0)
      : Efficiency( ist, request) {;}

    EffVal getEff( float /* x */) const { return EffVal();} 
    EffVal getEff( float /* x */, float /* y */) const { return EffVal();} 
    EffVal getEff( float /* x */, float /* y */, float /* z */)const { return EffVal();} 
    EffVal getEff( const std::vector<float>&  /* value */) const { return EffVal();} 
    
  protected:
    virtual bool m_parseInputLine( const std::string& ,
                                   const std::vector<std::string> & /* line */) { return false;}

  private:
    void m_stream( std::ostream & os) const { os << "Not yet implemented!" << std::endl;}
    
  };
}
#endif
