/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_CSCREGUTILS_H
#define TRIGL2MUONSA_CSCREGUTILS_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include "GeoPrimitives/GeoPrimitives.h"


#include <iostream>
#include <vector>
using namespace std;



namespace TrigL2MuonSA {

class UtilTools;
class ReturnCode;


class CscRegDict: public AthAlgTool{
  
 public:
  

  
  typedef struct {
    double etaMin;
    double etaMax;
    double phiMin;
    double phiMax;
    double phiCen;
    double phiMod; //actual phi coordinates of center for data
    double Displacement;
    double idealAtanNormal; // surface on which segments will be defined
    double actualAtanNormal;//surface of first wirelayer is defined by: n*x=a, where n=(cos(phiCen)sin(AtanNormal),sin(phiCen)sin(AtanNormal),cos(AtanNormal)); a=Displacement
    double posCorrectionPlusR;
    double posCorrectionMinusR;
  } CscRegion;
  
  static const InterfaceID &interfaceID();
  CscRegDict(const std::string &type, const std::string &name, const IInterface *parent);
  ~CscRegDict();
  
  StatusCode initialize();
  StatusCode finalize();
  
  double phiCen(int module){ return m_reg_dict[module].phiCen; }
  double phiMod(int module){ return m_reg_dict[module].phiMod; }
  
  int get_hash(int stationname, int stationeta, int stationphi); //return a module context hashId.
  Amg::Vector3D nomalVector(int module);
  double displacement(int module){ return m_reg_dict[module].Displacement; }
  double posCorrectionR(int module, int charge);
  double posCorrectionZ(int module, int charge);
  double idealAtanNormal(int module){ return m_reg_dict[module].idealAtanNormal; }
  double actualAtanNormal(int module){ return m_reg_dict[module].actualAtanNormal; }
  
  
 private:
  BooleanProperty  m_isMC;
  CscRegion m_reg_dict[32];//dictionary of CSC regions
  int m_module_hashes[2][2][8];//dictionary of hashIds
  
  
  ReturnCode initializeHashDictionary();
  ReturnCode initializeDictionaryForData();
  ReturnCode initializePosCorrectionParameters();
  
  double PhiConv(double phi); //convert phi [0,2*pi)=>[-pi,pi)
  
  TrigL2MuonSA::UtilTools *m_util;
  
};
  








class UtilTools{
  
public:
  UtilTools(){}
  ~UtilTools(){}
  
  double calc_theta(double eta){ return 2*atan(exp((-1)*eta)); }
  double calc_theta(double x, double y, double z){ return acos(z/sqrt(x*x+y*y+z*z)); } // for position (not for direction)
  double calc_eta(double x, double y, double z){return (-1)*log(tan(calc_theta(x,y,z)/2.));}
  double calc_phi( double x, double y);
  double calc_dphi(double phi1, double phi2);
  double calc_sumsq(double x, double y){ return sqrt(x*x+y*y); }
  double calc_sumsq(double x, double y, double z){ return sqrt(x*x+y*y+z*z);}
  double cotan(double theta){ return tan(M_PI-theta); }
  double relative_error(double measured, double reference){return (measured-reference)/reference; }
  double average_phi(double phi1, double phi2);

  
};




class ReturnCode{
  
public:
  ReturnCode(){}
  ReturnCode(unsigned int retcode){m_retcode=retcode;}
  ~ReturnCode(){}
  
  enum{
    FAILURE=0,
    SUCCESS=1
  };
  
  bool isSuccess(){return (m_retcode==SUCCESS);}
  
  void operator= (unsigned int retcode){m_retcode=retcode;}
  bool operator== ( unsigned int code){return (m_retcode==code);}
  bool operator!= ( unsigned int code){return (m_retcode!=code);}
  
private:
  unsigned int m_retcode;
  
};


}//namespace TrigL2MuonSA




#endif /* TRIGL2MUONSA_CSCREGUTILS_H */
