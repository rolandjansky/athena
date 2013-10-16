/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************                               
                               
NAME:     FakeRecMuonRoiSvc
PACKAGE:                                 
                               
AUTHORS: Jiri Masik 
CREATED: September 2004                               
                               
PURPOSE: This is just a temporary measure for the testbeam to avoid
         usage of the real RecTGCMuonRoiSvc. It is needed for
         RecMuonRoI instantiation.
                               
Modified :
         
                               
********************************************************************/                               

#ifndef FAKERECMUONROISVC_H
#define FAKERECMUONROISVC_H
#include <string> 

//namespace LVL1 {
using namespace LVL1;

class FakeRecMuonRoiSvc : public RecMuonRoiSvc {
public:
  FakeRecMuonRoiSvc(const std::string& name, ISvcLocator* svc ):RecMuonRoiSvc(name,svc){;};
  ~FakeRecMuonRoiSvc(){;}
  double phi(void) const {return 3.;}
  double eta(void) const {return 1.;}
  void   reconstruct(const unsigned int & /*roiWord*/) const {;}

};

#endif
