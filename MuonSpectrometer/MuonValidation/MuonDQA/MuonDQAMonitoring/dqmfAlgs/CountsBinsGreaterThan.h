/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqiCountsBinsGreaterThan_h
#define dqiCountsBinsGreaterThan_h

#include <string>

#include "dqm_core/Algorithm.h"


namespace dqi {

  class CountsBinsGreaterThan : public dqm_core::Algorithm {
  public:

    CountsBinsGreaterThan();
    CountsBinsGreaterThan( const CountsBinsGreaterThan& other );
  
    virtual ~CountsBinsGreaterThan();
    virtual dqm_core::Algorithm*  clone();
    virtual dqm_core::Result*     execute( const std::string& name, const TObject& object,
					   const dqm_core::AlgorithmConfig& config );
    virtual void                  printDescription();

  protected:

    double              GetFirstFromMap(const std::string & pname, const std::map<std::string,std::vector<double> > & params);
    std::vector<double> GetVectorFromMap(const std::string & pname, const std::map<std::string,std::vector<double> > & params);
    double              GetFromMap(const std::string & pname, const std::map<std::string,double> & params );

    std::string  name;
  };

} //namespace dqi

#endif
