/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL2MuonSA/PtEndcapLUT.h"
#include "CLHEP/Vector/TwoVector.h"
#include <fstream>
#include <sstream>

TrigL2MuonSA::PtEndcapLUT::PtEndcapLUT(MsgStream* msg) :
  m_msg(msg)
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
    msg() << MSG::ERROR << "Cannot open EndcapLUT file " << lut_fileName << endreq;
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
	msg() << MSG::ERROR << "Invalid header line " << line_no << " in EndcapLUT file "
	      << lut_fileName << endreq;
	return StatusCode::FAILURE;
      }
      
      DataType type = s2dt(ctype);
      if ((side != '-' && side != '+') || (charge != '-' && charge != '+') || type == INVALID) {
	msg() << MSG::ERROR << "Invalid header line " << line_no << " in EndcapLUT file " << lut_fileName << endreq;
	return StatusCode::FAILURE;
      }

      KeyType key(side == '+' ? 1 : 0, charge == '+' ? 1 : 0, type);
      table = new TableType();
      m_tables.insert(TableMap::value_type(key, table));
      msg() << MSG::DEBUG << "Created table for " << key.toString() << endreq;
      
    } else {
      
      if (table == NULL) {
	msg() << MSG::ERROR << "Missing header line " << line_no << " in EndcapLUT file " << lut_fileName << endreq;
	return StatusCode::FAILURE;
      }
      
      int iEta, iPhi;
      double xcept, slope;
      if (sscanf(line.c_str(), "%3d %3d %15lf %15lf", &iEta, &iPhi, &xcept, &slope) != 4) {
	msg() << MSG::ERROR << "Invalid data line " << line_no << " in EndcapLUT file " << lut_fileName << endreq;
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
  msg() << MSG::DEBUG << "lookup("
        << "side="    << side
        << ",charge=" << charge
        << ",type="   << dt2s(type)
        << ",iEta="   << iEta
        << ",iPhi="    << iPhi
        << ",value="  << value
        << ")" << endreq;
    
  if (iEta == -1) iEta =  0;
  if (iEta == 30) iEta = 29;
  
  if (iEta < 0 || iEta >= ETAS || iPhi < 0 || iPhi >= PHISEE) {
    msg() << MSG::WARNING << "lookup(" << side << ", " << charge << ", " << dt2s(type) 
	  << ", " << iEta << ", " << iPhi << ") Invalid indices" << endreq;
    return 0.0;
  }

  TableMap::const_iterator it = m_tables.find(KeyType(side, charge, type));

  if (it == m_tables.end()) {
    msg() << MSG::ERROR << "lookup(" << side << ", " << charge << ", " << dt2s(type)
	  << ", " << iEta << ", " << iPhi << ") Invalid key" << endreq;
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

	msg() << MSG::WARNING << "Pol2: ptinv < 0, ptinv=" << ptinv << endreq;

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

	msg() << MSG::WARNING << "Pol2: ptinv < 0, ptinv=" << ptinv << endreq;

      } else {

	if( ptinv < (1/PT_MAX) ) {
	  pt = PT_MAX;
	} else {
	  pt = 1/ptinv;
	}
      }
    }
  }

  msg() << MSG::DEBUG << "Pol2: value=" << value << endreq;
  msg() << MSG::DEBUG << "Pol2: b=" << b << " c=" << c << " pt=" << pt << endreq;
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

  case INVALID:
    return ("invalid"); break;

  default:
    return ("invalid"); break;
  }
}
