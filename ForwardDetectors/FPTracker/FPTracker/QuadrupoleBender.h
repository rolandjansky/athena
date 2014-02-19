/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_QUADRUPOLEBENDER_H
#define FPTRACKER_QUADRUPOLEBENDER_H

#include "IBender.h"
#include "IQuadFocuser.h"
namespace FPTracker{

  class IFocuser;
  class IParticle;

 class QuadrupoleBender: public IBender{
  public:
    QuadrupoleBender(IQuadFocuser::Ptr_t focuser, 
		     double magnetLength,
		     double magnetStrength,
		     int side);


    void bend(IParticle&)     const;
    std::string label()       const;

  private:
    
    const static  std::string s_label;

    IQuadFocuser::Ptr_t       m_focuser;
    double                    m_length;
    double                    m_bendConstant;
    double                    m_dside;

    void swap(QuadrupoleBender&);
  };
}
#endif
