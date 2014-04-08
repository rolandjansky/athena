/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "tauEvent/TauPi0Candidate.h"
//#include "tauEvent/TauDetails.h"
//#include "tauEvent/TauCmp.h"
//#include "tauEvent/TauPi0Cluster.h"

namespace Analysis {

  // default constructor
  TauPi0Candidate::TauPi0Candidate()
  {
  }

  // copy constructor
  TauPi0Candidate::TauPi0Candidate(const TauPi0Candidate& rhs ) :
     m_pi0ClusterVector(rhs.m_pi0ClusterVector)
  {
  }

  // Assignment operator
  TauPi0Candidate &TauPi0Candidate::operator= ( const TauPi0Candidate& rhs )
  {
    if (this!=&rhs){
      m_pi0ClusterVector = rhs.m_pi0ClusterVector;
    }
    return *this;
  }

  // == and != operators
  // Current version returns, whether the vectors of clusters are identiacal (including the order). 
  // This is good enough for the use of operator != in TauPi0Candidate &TauPi0Candidate::operator=.
  // TODO: If == or != operators are required, which take into account that the order is unimportant 
  // for the candidates, new methods must be implemented
  bool TauPi0Candidate::operator==( const TauPi0Candidate& rhs ) const{
    if(m_pi0ClusterVector.size()!=rhs.m_pi0ClusterVector.size()) return false;
    for(unsigned iPi0Cluster=0; iPi0Cluster!=m_pi0ClusterVector.size();++iPi0Cluster){
      if (*(m_pi0ClusterVector.at(iPi0Cluster))!=*(rhs.m_pi0ClusterVector.at(iPi0Cluster))) return false;
    }
    return true;
  }

  bool TauPi0Candidate::operator!=( const TauPi0Candidate& rhs ) const{
    if(m_pi0ClusterVector.size()!=rhs.m_pi0ClusterVector.size()) return true;
    for(unsigned iPi0Cluster=0; iPi0Cluster!=m_pi0ClusterVector.size();++iPi0Cluster){
      if (*(m_pi0ClusterVector.at(iPi0Cluster))!=*(rhs.m_pi0ClusterVector.at(iPi0Cluster))) return true;
    }
    return false;
  }
  
  // destructor
  TauPi0Candidate::~TauPi0Candidate()
  {
  }

  // just for testing. TODO: Add new variables
  void TauPi0Candidate::dump()
  {
    std::cout << "in TauPi0Candidate::dump()" << std::endl;
    std::cout << "-------------------------" <<std::endl;
    std::cout << "number of Pi0Clusters = " << this->nPi0Cluster() << std::endl;
    std::cout.precision(6);

    for(int i=0; i < (int)this->nPi0Cluster(); ++i) {
      std::cout << "4 mom of cluster no " << i << ": pt = " << this->pi0Cluster(i)->cluster()->pt()  <<
                                                "\t eta = " << this->pi0Cluster(i)->cluster()->eta() <<
                                                "\t phi = " << this->pi0Cluster(i)->cluster()->phi() <<
                                                "\t   m = " << this->pi0Cluster(i)->cluster()->m()   <<
                                                "\t   E = " << this->pi0Cluster(i)->cluster()->e()   << std::endl;
                                                
    }
    std::cout << "4 mom of candidate: pt = " << std::sqrt(this->hlv().px()*this->hlv().px()+this->hlv().py()*this->hlv().py())  <<
                                 "\t eta = " << this->hlv().eta() <<
                                 "\t phi = " << this->hlv().phi() <<
                                 "\t   m = " << this->hlv().m()   <<
                                 "\t   E = " << this->hlv().e()   << std::endl;
    std::cout << std::endl << std::endl;
    std::cout << "-------------------------" << std::endl;
  }

}
