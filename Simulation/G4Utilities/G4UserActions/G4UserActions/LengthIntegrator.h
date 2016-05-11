/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LengthIntegrator_H
#define LengthIntegrator_H

#include "G4AtlasTools/UserActionBase.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <map>

#include "G4Pow.hh"

class TProfile;
class TProfile2D;


// User action to evaluate the thickness (in %r.l. or i.l.) of all detectors
// traversed by outgoing particles

class LengthIntegrator final: public UserActionBase {
  
 public:
  LengthIntegrator(const std::string& type, const std::string& name, const IInterface* parent);
  
  virtual void BeginOfEvent(const G4Event*) override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void Step(const G4Step*) override;
  
  virtual StatusCode queryInterface(const InterfaceID&, void**) override;
  
  virtual StatusCode initialize() override;

  virtual StatusCode finalize() override;
  
 private:

  G4Pow* m_g4pow;

  void addToDetThick(std::string, double, double);

  void regAndFillHist(const std::string&,const std::pair<double,double>&);

  double m_etaPrimary ;
  double m_phiPrimary ;
  std::map<std::string,std::pair<double,double>,std::less<std::string> > m_detThick;

  // profiles for rad length
  TProfile2D* m_rzProfRL;
  std::map<std::string,TProfile*,std::less<std::string> > m_etaMapRL;
  std::map<std::string,TProfile*,std::less<std::string> > m_phiMapRL;

  // profiles for int length
  TProfile2D* m_rzProfIL;
  std::map<std::string,TProfile*,std::less<std::string> > m_etaMapIL;
  std::map<std::string,TProfile*,std::less<std::string> > m_phiMapIL;

  ServiceHandle<ITHistSvc> m_hSvc;

  std::map<std::string,TProfile2D*,std::less<std::string> > m_rzMapRL;
  std::map<std::string,TProfile2D*,std::less<std::string> > m_xyMapRL;

  std::map<std::string,TProfile2D*,std::less<std::string> > m_rzMapIL;
  std::map<std::string,TProfile2D*,std::less<std::string> > m_xyMapIL;

};

#endif
