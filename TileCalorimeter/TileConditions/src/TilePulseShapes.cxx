/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileConditions/TilePulseShapes.h"
#include <iostream>
#include <fstream>
#include <string>
#include "PathResolver/PathResolver.h"

TilePulseShapes::TilePulseShapes()
  : m_loaded(false)
  , m_shapes(NULL)
{
}

TilePulseShapes::~TilePulseShapes()
{
  if (m_shapes) delete m_shapes;
}

void 
TilePulseShapes::load(MsgStream &log) {

  bool debug = (log.level() <= MSG::DEBUG);
  if(debug) log<<MSG::DEBUG<<"TilePulseShapes::load"<<endmsg;

  m_shapes = new TilePulseShapesStruct;

  // Read in CIS Low gain pulse shape, big cap 100 pF
  ReadFile(log,m_filenameLoCIS, "tlcis", "ylcis", m_shapes->m_tlcis, m_shapes->m_ylcis);

  // Read in CIS Low gain derivative shape, big cap 100 pF
  ReadFile(log,m_filenameLoCISDer,"tdlcis","ydlcis",m_shapes->m_tdlcis,m_shapes->m_ydlcis);

  // Read in CIS Low gain pulse shape, small cap 5.2 pF
  ReadFile(log,m_filenameSLoCIS,"tslcis","yslcis",m_shapes->m_tslcis,m_shapes->m_yslcis);

  // Read in CIS Low gain derivative shape, small cap 5.2 pF
  ReadFile(log,m_filenameSLoCISDer,"tdslcis","ydslcis",m_shapes->m_tdslcis,m_shapes->m_ydslcis);
  
  // Read in CIS Low gain leakage 5.2 pF cap
  ReadFile(log,m_filenameSLeakLo,"tsleaklo","sleaklo",m_shapes->m_tsleaklo,m_shapes->m_sleaklo);
  
  // Read in CIS Low gain leakage 100 pF cap
  ReadFile(log,m_filenameLeakLo,"tleaklo","leaklo",m_shapes->m_tleaklo,m_shapes->m_leaklo);
  
  // Read in CIS Low gain leakage Derivative 5.2 pF cap
  ReadFile(log,m_filenameDSLeakLo,"tdsleaklo","dsleaklo",m_shapes->m_tdsleaklo,m_shapes->m_dsleaklo);

  // Read in CIS Low gain leakage Derivative 100 pF cap
  ReadFile(log,m_filenameDLeakLo,"tdleaklo","dleaklo",m_shapes->m_tdleaklo,m_shapes->m_dleaklo);
  
  // Read in CIS High gain pulse shape, big cap 100 pF
  ReadFile(log,m_filenameHiCIS,"thcis","yhcis",m_shapes->m_thcis,m_shapes->m_yhcis);
  
  // Read in CIS High gain derivative shape, big cap 100 pF
  ReadFile(log,m_filenameHiCISDer,"tdhcis","ydhcis",m_shapes->m_tdhcis,m_shapes->m_ydhcis);
 
  // Read in CIS High gain pulse shape, small cap 5.2 pF
  ReadFile(log,m_filenameSHiCIS,"tshcis","yshcis",m_shapes->m_tshcis,m_shapes->m_yshcis);
  
  // Read in CIS High gain derivative shape, small cap 5.2 pF
  ReadFile(log,m_filenameSHiCISDer,"tdshcis","ydshcis",m_shapes->m_tdshcis,m_shapes->m_ydshcis);
  
  // Read in CIS High gain leakage 5.2 pF cap
  ReadFile(log,m_filenameSLeakHi,"tsleakhi","sleakhi",m_shapes->m_tsleakhi,m_shapes->m_sleakhi);
  
  // Read in CIS High gain leakage 100 pF cap
  ReadFile(log,m_filenameLeakHi,"tleakhi","leakhi",m_shapes->m_tleakhi,m_shapes->m_leakhi);
  
  // Read in CIS High gain leakage Derivative 5.2 pF cap
  ReadFile(log,m_filenameDSLeakHi,"tdsleakhi","dsleakhi",m_shapes->m_tdsleakhi,m_shapes->m_dsleakhi);
  
  // Read in CIS High gain leakage Derivative 100 pF cap
  ReadFile(log,m_filenameDLeakHi,"tdleakhi","dleakhi",m_shapes->m_tdleakhi,m_shapes->m_dleakhi);
  
  // Read in Low gain pulse shape Physics
  ReadFile(log,m_filenameLoPhys,"tlphys","ylphys",m_shapes->m_tlphys,m_shapes->m_ylphys,4);
  
  // Read in Low gain pulse shape derivative for Physics
  ReadFile(log,m_filenameLoPhysDer,"tdlphys","ydlphys",m_shapes->m_tdlphys,m_shapes->m_ydlphys);
  
  // Read in Hi gain pulse shape Physics
  ReadFile(log,m_filenameHiPhys,"thphys","yhphys",m_shapes->m_thphys,m_shapes->m_yhphys,4);
  
  // Read in Hi gain pulse shape derivative for Physics
  ReadFile(log,m_filenameHiPhysDer,"tdhphys","ydhphys",m_shapes->m_tdhphys,m_shapes->m_ydhphys);

  // Read in Low gain pulse shape Laser
  ReadFile(log,m_filenameLoLas,"tllas","yllas",m_shapes->m_tllas,m_shapes->m_yllas);
  
  // Read in Low gain pulse shape derivative for Laser
  ReadFile(log,m_filenameLoLasDer,"tdllas","ydllas",m_shapes->m_tdllas,m_shapes->m_ydllas);
  
  // Read in Hi gain pulse shape Laser
  ReadFile(log,m_filenameHiLas,"thlas","yhlas",m_shapes->m_thlas,m_shapes->m_yhlas);
  
  // Read in Hi gain pulse shape derivative for Laser
  ReadFile(log,m_filenameHiLasDer,"tdhlas","ydhlas",m_shapes->m_tdhlas,m_shapes->m_ydhlas);
  
  // Read in Digitizer noise RMS as a function of channel number (1) Original noise
  ReadFile(log,m_filenameOrigNoise,"noiseOrigLo","noiseOrigHi",m_shapes->m_noiseOrigLo,m_shapes->m_noiseOrigHi,-1);
  
  // (2) Noise with resistors added to PMT channels (so-called noise-killers)
  ReadFile(log,m_filenameNkNoise,"noiseNkLo","noiseNkHi",m_shapes->m_noiseNkLo,m_shapes->m_noiseNkHi,-1);
  
  m_loaded = true;
}

bool
TilePulseShapes::ReadFile(MsgStream &log, std::string fname, const char *xname, const char *yname,
                          std::vector<double> & x, std::vector<double> & y, int nskip)
{
  bool status=false;
  std::string fileName = PathResolver::find_file(fname, "DATAPATH");
  std::ifstream inFile(fileName.c_str());

  if(inFile.is_open()) {
    bool debug = (log.level() <= MSG::DEBUG);
    bool verbose = (log.level() <= MSG::VERBOSE);
    if(debug) log<<MSG::DEBUG<<"Reading file '"<<fname<<"'"<<endmsg;
    std::string name;
    for(int i=0; i<nskip && !inFile.eof(); ++i) { // skip nskip lines
      if ((std::getline(inFile,name)).fail()) break;
    }
    x.reserve(100);
    y.reserve(100);
    int i=0;
    double xx,yy;
    while ( inFile>>xx>>yy ) {
      if (nskip<0) { // skip one word
          xx=yy;
          inFile>>yy;
      }
      x.push_back(xx);
      y.push_back(yy);
      if(verbose)
          log<<MSG::VERBOSE<<" i="<<(i++)
             <<" "<<xname<<"="<<xx
             <<" "<<yname<<"="<<yy
             <<endmsg; 
    }
    status=true;
  } else {
    log<<MSG::ERROR<<"Unable to open file '"<<fname<<"'"<<endmsg;
  }
  inFile.close();
  return status;
}
