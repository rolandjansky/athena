/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_CSCREGUTILS_H
#define TRIGL2MUONSA_CSCREGUTILS_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"

#include "GeoPrimitives/GeoPrimitives.h"


#include <iostream>
#include <vector>



namespace TrigL2MuonSA {

class ReturnCode;


class UtilTools{

public:
  UtilTools(){}

  double calc_theta(double eta) const{ return 2*std::atan(std::exp((-1)*eta)); }
  double calc_theta(double x, double y, double z) const{ return std::acos(z/std::sqrt(x*x+y*y+z*z)); } // for position not for direction theta in [0,pi]
  double calc_eta(double x, double y, double z) const {return (-1)*std::log(std::tan(calc_theta(x,y,z)/2.));}
  double calc_phi( double x, double y) const;
  double calc_dphi(double phi1, double phi2) const;
  double calc_sumsq(double x, double y) const { return std::sqrt(x*x+y*y); }
  double calc_sumsq(double x, double y, double z) const { return std::sqrt(x*x+y*y+z*z);}
  double cotan(double theta) const { return std::tan(M_PI-theta); }
  double relative_error(double measured, double reference) const {return (measured-reference)/reference; }
  double average_phi(double phi1, double phi2) const;

};


class CscRegDict: public AthAlgTool{

 public:

  //  typedef TrigL2MuonSA::ReturnCode ReturnCode;

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

  CscRegDict(const std::string &type, const std::string &name, const IInterface *parent);

  virtual StatusCode initialize() override;

  double phiCen(int module){ return m_reg_dict[module].phiCen; }
  double phiMod(int module) const { return m_reg_dict[module].phiMod; }

  int get_hash(int stationname, int stationeta, int stationphi) const; //return a module context hashId.
  Amg::Vector3D nomalVector(int module) const;
  double displacement(int module) const;
  double posCorrectionR(int module, int charge);
  double posCorrectionZ(int module, int charge);
  double idealAtanNormal(int module) const { return m_reg_dict[module].idealAtanNormal; }
  double actualAtanNormal(int module) const { return m_reg_dict[module].actualAtanNormal; }
  int stationName(int hash) const;
  int stationEta(int hash) const; 
  int stationPhi(int hash) const;

 private:
  Gaudi::Property< bool >  m_isMC { this, "MCFlag", true, "" };
  CscRegion m_reg_dict[32];//dictionary of CSC regions
  int m_module_hashes[2][2][8];//dictionary of hashIds


  ReturnCode initializeHashDictionary();
  ReturnCode initializeDictionaryForData();
  ReturnCode initializePosCorrectionParameters();

  double PhiConv(double phi); //convert phi [0,2*pi)=>[-pi,pi)

  TrigL2MuonSA::UtilTools m_util;

};


inline double CscRegDict::displacement(int module) const
{
  return (0<=module && module <32) ? m_reg_dict[module].Displacement : 0.;
}

inline int CscRegDict::stationName(int hash) const
{
   if(hash<0 || hash>31) return 999;
   else return (hash<16) ? 50 : 51;
}

inline int CscRegDict::stationEta(int hash) const
{
   if(hash<0 || hash>31) return 999;
   else{
     int secteta=(hash - hash%8)/8;// secteta: 0(Small Cside),1(Small Aside), 2(Large Cside), 3(Large Aside)
     return (secteta%2) ? 1 : -1;
   }
}

inline int CscRegDict::stationPhi(int hash) const
{
   if(hash<0 || hash>31) return 999;
   else return hash%8+1;
}


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
