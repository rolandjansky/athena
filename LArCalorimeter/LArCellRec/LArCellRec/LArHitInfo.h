/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLREC_LARCELLBUILDERFROMLARHITWITHNOISE_H
#define LARCELLREC_LARCELLBUILDERFROMLARHITWITHNOISE_H

#include "LArSimEvent/LArHit.h"
#include "CaloDetDescr/CaloDetDescrElement.h"


class LArHitInfo 
{
  private: 
    const LArHit * m_hit ;
    std::vector<float> m_SIGMANOISE;
    const CaloDetDescrElement * m_caloDDE ;
    float m_Escale;
    float m_Epart2Edep;
    
  public: 
    LArHitInfo():m_hit(0),m_SIGMANOISE(0),m_caloDDE(0),m_Escale(0.),
      m_Epart2Edep(0.){}

    LArHitInfo(const CaloDetDescrElement* caloDDE):m_hit(0),m_SIGMANOISE(0),m_caloDDE(caloDDE),
      m_Escale(0.),m_Epart2Edep(0.)
      {}

    inline float energy() const {if (m_hit!=0) { return m_hit->energy() ;} else{return 0.;}}
    inline float time()   const {if (m_hit!=0) { return m_hit->time();} else{return 0.;}}
    inline const LArHit * hit() const {return m_hit ;}
    
    inline std::vector<float> SIGMANOISE() const {return m_SIGMANOISE;}
    inline const CaloDetDescrElement* caloDDE() const {return m_caloDDE;}  
    inline float Escale() const {return m_Escale;}
    inline float Epart2Edep() const {return m_Epart2Edep;}
    inline bool hasBeenHit() const { if (m_hit==0) {return false;} else {return true;} }
    

    inline void setHit(const LArHit* hit) 
      {
	m_hit=hit;
      }


    inline void setHasNotBeenHit() 
      {
	m_hit=0;
      }


    inline void setSIGMANOISE(const std::vector<float> & SIGMANOISE) 
      {	
	m_SIGMANOISE.clear();	
	for(int i=0;i<3;++i) m_SIGMANOISE.push_back(SIGMANOISE[i]);
      }

    inline void setEscale(const float & Escale) 
      {	
	m_Escale=Escale;
      }
    inline void setCaloDDE(const CaloDetDescrElement* caloDDE) 
      {	
	m_caloDDE=caloDDE;
      }

    inline void setEpart2Edep(const float & Epart2Edep) 
      {	
	m_Epart2Edep=Epart2Edep;
      }

    void print() const
      {
	std::cout<<"Hit: e="<<m_hit->energy()<<" t="<<m_hit->time()<<std::endl;
	if(m_SIGMANOISE.size()!=0)
	  std::cout<<"SigmaNoise: "<<m_SIGMANOISE[2]<<"(HG) "
		                   <<m_SIGMANOISE[1]<<"(MG) "
	                           <<m_SIGMANOISE[0]<<"(LG) "
		   <<std::endl;	
      }
    
}; 

#endif 






