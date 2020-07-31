/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PtEndcapLUT.h"
#include "CLHEP/Vector/TwoVector.h"
#include <fstream>
#include <sstream>

#include "AthenaBaseComps/AthMsgStreamMacros.h"

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtEndcapLUT::PtEndcapLUT(const std::string& type,
                                       const std::string& name,
                                       const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtEndcapLUT::~PtEndcapLUT()
{
  for (TableMap::iterator it = m_tables.begin(); it != m_tables.end(); it++) {
    delete it->second;
  }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

bool TrigL2MuonSA::PtEndcapLUT::KeyType::operator<(const KeyType& other) const
{
  if (m_side   < other.m_side)   return (true);
  if (m_side   > other.m_side)   return (false);
  if (m_charge < other.m_charge) return (true);
  if (m_charge > other.m_charge) return (false);
  if (m_type   < other.m_type)   return (true);
  if (m_type   > other.m_type)   return (false);
  return (false);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

std::string TrigL2MuonSA::PtEndcapLUT::KeyType::toString() const
{
  std::ostringstream ss;
  ss << "side=" << m_side << " charge=" << m_charge << " type=" << dt2s(m_type);
  return ss.str();
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::PtEndcapLUT::readLUT(std::string lut_fileName)
{   
  std::ifstream ifs(lut_fileName.c_str());
  if (!ifs.is_open()) {
    ATH_MSG_ERROR("Cannot open EndcapLUT file " << lut_fileName);
    return StatusCode::FAILURE;
  }
  
  m_tables.clear();
  std::string line;
  int line_no = 0;
  TableType* table = NULL;
  
  while (!ifs.eof()) {
    
    getline(ifs, line);
    line_no++;
    
    if (line.empty()) continue;
    
    if (line.substr(0, 5) == "side=") {
      char side, charge, ctype[15];
      
      if (sscanf(line.c_str(), "side=%c charge=%c %14s", &side, &charge, ctype) != 3) {
	ATH_MSG_ERROR("Invalid header line " << line_no << " in EndcapLUT file " << lut_fileName);
	return StatusCode::FAILURE;
      }
      
      DataType type = s2dt(ctype);
      if ((side != '-' && side != '+') || (charge != '-' && charge != '+') || type == INVALID) {
	ATH_MSG_ERROR("Invalid header line " << line_no << " in EndcapLUT file " << lut_fileName);
	return StatusCode::FAILURE;
      }

      KeyType key(side == '+' ? 1 : 0, charge == '+' ? 1 : 0, type);
      table = new TableType();
      m_tables.insert(TableMap::value_type(key, table));
      ATH_MSG_DEBUG("Created table for " << key.toString());
      
    } else {
      
      if (table == NULL) {
	ATH_MSG_ERROR("Missing header line " << line_no << " in EndcapLUT file " << lut_fileName);
	return StatusCode::FAILURE;
      }
      
      int iEta, iPhi;
      double xcept, slope;
      if (sscanf(line.c_str(), "%3d %3d %15lf %15lf", &iEta, &iPhi, &xcept, &slope) != 4) {
	ATH_MSG_ERROR("Invalid data line " << line_no << " in EndcapLUT file " << lut_fileName);
	return StatusCode::FAILURE;
      }
      table->m_xcepts[iEta-1][iPhi] = xcept;
      table->m_slopes[iEta-1][iPhi] = slope;
    }
  }
  ifs.close();
  
  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::PtEndcapLUT::alpha(double z1, double r1, double z2, double r2) const
{
  CLHEP::Hep2Vector mid1(z1, r1);
  CLHEP::Hep2Vector mid2(z2, r2);
  CLHEP::Hep2Vector dir = mid2 - mid1;
  double a = acos(mid1.unit() * dir.unit());

  return a;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::PtEndcapLUT::radius(double z1, double r1, double s1, double z2, double r2, double s2, double deltar) const
{
  float cr1 = 0.080/400;
  float cr2 = cr1;
  double x1 = z1;

  if (fabs(x1)>=0.1) {
    double x2 = z2;
    double y1 = r1;
    double y2 = r2;
    double A1 = s1;
    double A2 = s2;
    if(!(fabs(s2+999)<0.1)) {
      A2 = s2;
      cr2 = cr1/10;
    }
    
    // find centre of circonference
    double xm = (x1+x2)/2.;
    double ym = (y1+y2)/2.;
    double c1 = (x2-x1)*xm+(y2-y1)*ym;
    double c2_1 = -x1-A1*y1;
    double c2_2 = -x2-A2*y2;
    double yR1 = (-c1-c2_1*(x2-x1))/(A1*(x2-x1)-(y2-y1));
    double yR2 = (-c1-c2_2*(x2-x1))/(A2*(x2-x1)-(y2-y1));
    double xR1 = -A1*yR1-c2_1;
    double xR2 = -A2*yR2-c2_2;
    
    double xR = ((1./cr1)*xR1+(1./cr2)*xR2)/((1./cr1)+(1./cr2));
    double yR = ((1./cr1)*yR1+(1./cr2)*yR2)/((1./cr1)+(1./cr2));
    double sign = deltar / fabs(deltar);
    double radius = 0.5*(sqrt((xR-x1)*(xR-x1)+(yR-y1)*(yR-y1))+sqrt((xR-x2)*(xR-x2)+(yR-y2)*(yR-y2)));
    return(sign * radius);
  } else {
    return 0.0;
  }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::PtEndcapLUT::lookup(int side, int charge, DataType type, int iEta, int iPhi, double value) const
{
  ATH_MSG_DEBUG("lookup(side=" << side << ",charge=" << charge
        << ",type="   << dt2s(type) << ",iEta="   << iEta
        << ",iPhi="    << iPhi << ",value="  << value << ")");
    
  if (iEta == -1) iEta =  0;
  if (iEta == 30) iEta = 29;
  
  if (iEta < 0 || iEta >= ETAS || iPhi < 0 || iPhi >= PHISEE) {
    ATH_MSG_WARNING("lookup(" << side << ", " << charge << ", " << dt2s(type) 
		    << ", " << iEta << ", " << iPhi << ") Invalid indices");
    return 0.0;
  }

  TableMap::const_iterator it = m_tables.find(KeyType(side, charge, type));

  if (it == m_tables.end()) {
    ATH_MSG_ERROR("lookup(" << side << ", " << charge << ", " << dt2s(type)
		  << ", " << iEta << ", " << iPhi << ") Invalid key");
    return 0.0;
  }

  double pt = 0;
  TableType* table = it->second;
  
  double b = table->m_xcepts[iEta][iPhi];
  double c = table->m_slopes[iEta][iPhi];
  const double PT_MAX = 500; // 500 GeV upper limit
  const double ZERO_LIMIT = 1e-5;

  if( fabs(c) < ZERO_LIMIT ) {

    if( fabs(b) < ZERO_LIMIT ) {

      pt = PT_MAX;

    } else {

      double ptinv = value / b;

      if( ptinv < 0 ) {

	ATH_MSG_WARNING("Pol2: ptinv < 0, ptinv=" << ptinv);

      } else {

	if( ptinv < (1/PT_MAX) ) pt = PT_MAX;
	else { pt = 1/ptinv; }

      }
    }

  } else {

    double judge = b*b + 4*c*value;

    if( judge > 0 ) {

      double ptinv = (-b + pow(judge,0.5)) / c / 2;

      if( ptinv < 0 ) {

	ATH_MSG_WARNING("Pol2: ptinv < 0, ptinv=" << ptinv);

      } else {

	if( ptinv < (1/PT_MAX) ) {
	  pt = PT_MAX;
	} else {
	  pt = 1/ptinv;
	}
      }
    }
  }

  ATH_MSG_DEBUG("Pol2: value=" << value);
  ATH_MSG_DEBUG("Pol2: b=" << b << " c=" << c << " pt=" << pt);
  pt *= 1000; // convert to MeV
  
  return pt;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigL2MuonSA::PtEndcapLUT::DataType TrigL2MuonSA::PtEndcapLUT::s2dt(const char* type)
{
    std::string stype(type);
    if (stype == "alphapol2")  return (ALPHAPOL2);
    if (stype == "betapol2")   return (BETAPOL2);
    if (stype == "tgcalphapol2")  return (TGCALPHAPOL2);
    if (stype == "invradiuspol2") return (INVRADIUSPOL2);
    if (stype == "cscpol2")  return (CSCPOL2);
    return (INVALID);
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

const char* TrigL2MuonSA::PtEndcapLUT::dt2s(TrigL2MuonSA::PtEndcapLUT::DataType type)
{
  switch (type) {

  case ALPHAPOL2:
    return ("alphapol2");   break;

  case BETAPOL2:
    return ("betapol2");    break;

  case TGCALPHAPOL2:
    return ("tgcalphapol2");   break;

  case INVRADIUSPOL2:
    return ("invradiuspol2");   break;

  case CSCPOL2:
    return ("cscpol2"); break;

  case INVALID:
    return ("invalid"); break;

  default:
    return ("invalid"); break;
  }
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

double TrigL2MuonSA::PtEndcapLUT::ptcombined(int iEta, int iPhi, double ApT, double BpT, double &CApT, \
					     double &CBpT) const
{
  ATH_MSG_DEBUG("pTcombined("
	<< "iEta="      << iEta
	<< "iPhi="      << iPhi
	<< "Alpha pT="  << ApT
	<< "Beta pT="   << BpT
	<< ")" );

  if (iEta == -1) iEta =  0;
  if (iEta == 30) iEta = 29;

  if (iEta < 0 || iEta >= ETAS || iPhi < 0 || iPhi >= PHIS) {
    ATH_MSG_WARNING("pTcombined("<< iEta << ", " << iPhi << ") Invalid indices");
    return 0.0;
  }

  const float ZERO_LIMIT = 1e-5;
  int iphibin=iPhi;
  int ietabin=iEta/6;
  if(iPhi==5||iPhi==6)iphibin=5;
  if(iPhi==4||iPhi==7)iphibin=4;
  if(iPhi==3||iPhi==8)iphibin=3;
  if(iPhi==2||iPhi==9)iphibin=2;
  if(iPhi==1||iPhi==10)iphibin=1;
  if(iPhi==0||iPhi==11)iphibin=0;
  CApT=0.;
  CBpT=0.;
  double Ameana = m_meana[ietabin][iphibin][0];
  double Ameanb = m_meanb[ietabin][iphibin][0];
  double Ameanc = m_meanc[ietabin][iphibin][0];
  double Bmeana = m_meana[ietabin][iphibin][1];
  double Bmeanb = m_meanb[ietabin][iphibin][1];
  double Bmeanc = m_meanc[ietabin][iphibin][1];

  double Asigmaa = m_sigmaa[ietabin][iphibin][0];
  double Asigmab = m_sigmab[ietabin][iphibin][0];
  double Asigmac = m_sigmac[ietabin][iphibin][0];
  double Bsigmaa = m_sigmaa[ietabin][iphibin][1];
  double Bsigmab = m_sigmab[ietabin][iphibin][1];
  double Bsigmac = m_sigmac[ietabin][iphibin][1];

  
  double MeanAP  = ( fabs(ApT) > ZERO_LIMIT)? (Ameana + Ameanb * exp( Ameanc / ApT)) : 1.0;
  double MeanBP  = ( fabs(BpT) > ZERO_LIMIT)? (Bmeana + Bmeanb * exp( Bmeanc / BpT)) : 1.0;
  double ApT_tmp = ( fabs(1-MeanAP) > ZERO_LIMIT)? (fabs(ApT) / (1-MeanAP)) : 1.0;
  ApT_tmp = fabs(ApT_tmp);
  if(ApT_tmp >= 500) ApT_tmp = 500;
  double BpT_tmp = ( fabs(1-MeanBP) > ZERO_LIMIT)? (fabs(BpT) / (1-MeanBP)) : 1.0;
  BpT_tmp = fabs(BpT_tmp);
  if(BpT_tmp >= 500) BpT_tmp = 500;

  CApT = ApT_tmp;
  CBpT = BpT_tmp;
  
  if(ApT == 0. ) CApT = 0.;
  if(BpT == 0. ) CBpT = 0.;
  double NSigmaA= Asigmaa * fabs(ApT_tmp) * fabs(ApT_tmp) + Asigmab * fabs(ApT_tmp) + Asigmac;
  double NSigmaB= Bsigmaa * fabs(BpT_tmp) * fabs(BpT_tmp) + Bsigmab * fabs(BpT_tmp) + Bsigmac;

  double NVsigpTA =(fabs(ApT_tmp) > ZERO_LIMIT&& fabs(NSigmaA) > ZERO_LIMIT)? (1/(NSigmaA * ApT_tmp) ): 1.0; 
  double NVsigpTB =(fabs(BpT_tmp) > ZERO_LIMIT&& fabs(NSigmaB) > ZERO_LIMIT)? (1/(NSigmaB * BpT_tmp) ): 1.0; 
  double NVsigAsq =(fabs(NSigmaA) > ZERO_LIMIT)? (1/(NSigmaA * NSigmaA))  : 1.0; 
  double NVsigBsq =(fabs(NSigmaB) > ZERO_LIMIT)? (1/(NSigmaB * NSigmaB))  : 1.0; 

  double NVsigpTAsq = NVsigpTA * NVsigpTA;
  double NVsigpTBsq = NVsigpTB * NVsigpTB;
  double pt = (fabs(NVsigAsq + NVsigBsq) > ZERO_LIMIT)? (1/sqrt((NVsigpTAsq + NVsigpTBsq)/(NVsigAsq + NVsigBsq))) : 0.;
  if(pt>500) pt = 500.;
  return pt;
}
// --------------------------------------------------------------------------------                      
// -------------------------------------------------------------------------------- 

StatusCode TrigL2MuonSA::PtEndcapLUT::readLUTSigmaMean(std::string lut_mean, std::string lut_sigma)
{
  std::ifstream ifsmean(lut_mean.c_str());
  std::ifstream ifssigma(lut_sigma.c_str());
  if (!ifsmean.is_open()) {
    ATH_MSG_ERROR("Cannot open EndcapLUT Mean file " << lut_mean);
    return StatusCode::FAILURE;
  }
  if (!ifssigma.is_open()) {
    ATH_MSG_ERROR("Cannot open EndcapLUT Sigma file " << lut_sigma);
    return StatusCode::FAILURE;
  }

  std::string line;

  for(int ei=0; ei < ETAS1; ei++){
    for(int pi=0; pi < PHIS1; pi++){
      for(int pti=0; pti < PTS1; pti++){
        m_meana[ei][pi][pti] =0.;
        m_meanb[ei][pi][pti] =0.;
        m_meanc[ei][pi][pti] =0.;
        m_sigmaa[ei][pi][pti]=0.;
        m_sigmab[ei][pi][pti]=0.;
        m_sigmac[ei][pi][pti]=0.;
      }
    }
  }

  while (!ifsmean.eof()) {
    getline(ifsmean, line);
    if (line.empty()) continue;

    int iEta, iPhi, iNP;
    double tmp_par1, tmp_par2, tmp_par3;
    if (sscanf(line.c_str(), "%d %d %d %lf %lf %lf", &iEta, &iPhi, &iNP, &tmp_par1, &tmp_par2, &tmp_par3) != 6) {
      ATH_MSG_ERROR(" Invalid data in mean EndcapLUT file " << lut_mean);
      return StatusCode::FAILURE;
    }

    m_meana[iEta][iPhi][iNP] = tmp_par1;
    m_meanb[iEta][iPhi][iNP] = tmp_par2;
    m_meanc[iEta][iPhi][iNP] = tmp_par3;
  }
  ifsmean.close();
  std::string line2;
  while (!ifssigma.eof()) {
    getline(ifssigma, line2);
    if (line2.empty()) continue;

    int iEta, iPhi, iNP;
    double tmp_par1, tmp_par2, tmp_par3;
    if (sscanf(line2.c_str(), "%d %d %d %lf %lf %lf", &iEta, &iPhi, &iNP, &tmp_par1, &tmp_par2, &tmp_par3) != 6) {
      ATH_MSG_ERROR(" Invalid data in mean EndcapLUT file " << lut_mean);
      return StatusCode::FAILURE;
    }

    m_sigmaa[iEta][iPhi][iNP] = tmp_par1;
    m_sigmab[iEta][iPhi][iNP] = tmp_par2;
    m_sigmac[iEta][iPhi][iNP] = tmp_par3;
  }
  ifssigma.close();

  return StatusCode::SUCCESS;
}
