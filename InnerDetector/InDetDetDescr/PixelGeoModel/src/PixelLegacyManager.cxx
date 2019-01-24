/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelLegacyManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>
#include <cmath>


using namespace std;

PixelLegacyManager::PixelLegacyManager(IRDBAccessSvc * rdbSvc, 
				       const std::string & detectorKey, 
				       const std::string & detectorNode)
  :
    m_BarrelInSFrame(false),
    m_EndcapInSFrame(false),
    m_EndConeSFrame(false),
    m_dc1Geometry(false)
{

  // These are for the new description of the Pixel Frame
  m_pfba = rdbSvc->getRecordsetPtr("PFBA",     detectorKey, detectorNode);
  m_pbba = rdbSvc->getRecordsetPtr("PBBA",     detectorKey, detectorNode);
  m_ptba = rdbSvc->getRecordsetPtr("PTBA",     detectorKey, detectorNode);
  m_pfec = rdbSvc->getRecordsetPtr("PFEC",     detectorKey, detectorNode);
  m_pbec = rdbSvc->getRecordsetPtr("PBEC",     detectorKey, detectorNode);
  m_ptec = rdbSvc->getRecordsetPtr("PTEC",     detectorKey, detectorNode);
  m_pecn = rdbSvc->getRecordsetPtr("PECN",     detectorKey, detectorNode);
  m_pecf = rdbSvc->getRecordsetPtr("PECF",     detectorKey, detectorNode);
  m_pecb = rdbSvc->getRecordsetPtr("PECB",     detectorKey, detectorNode);
  m_pect = rdbSvc->getRecordsetPtr("PECT",     detectorKey, detectorNode);

  // These are for the design
  m_pxbi = rdbSvc->getRecordsetPtr("PXBI",     detectorKey, detectorNode);
  m_pdch = rdbSvc->getRecordsetPtr("PDCH",     detectorKey, detectorNode);
  m_pxbd = rdbSvc->getRecordsetPtr("PXBD",     detectorKey, detectorNode);

  // These are for a detailed description of the ladders and services on ladder
  m_ptla = rdbSvc->getRecordsetPtr("PTLA",     detectorKey, detectorNode);
  m_pctr = rdbSvc->getRecordsetPtr("PCTR",     detectorKey, detectorNode);
  m_pftr = rdbSvc->getRecordsetPtr("PFTR",     detectorKey, detectorNode);
  m_pttr = rdbSvc->getRecordsetPtr("PTTR",     detectorKey, detectorNode);
  m_pome = rdbSvc->getRecordsetPtr("POME",     detectorKey, detectorNode);
  m_poti = rdbSvc->getRecordsetPtr("POTI",     detectorKey, detectorNode);
  m_pobi = rdbSvc->getRecordsetPtr("POBI",     detectorKey, detectorNode);
  m_poai = rdbSvc->getRecordsetPtr("POAI",     detectorKey, detectorNode);
  m_poci = rdbSvc->getRecordsetPtr("POCI",     detectorKey, detectorNode);
  m_posi = rdbSvc->getRecordsetPtr("POSI",     detectorKey, detectorNode);
  m_pccf = rdbSvc->getRecordsetPtr("PCCF",     detectorKey, detectorNode);
  m_pcff = rdbSvc->getRecordsetPtr("PCFF",     detectorKey, detectorNode);

}


bool PixelLegacyManager::oldFrame()
{
  // if m_pfba exists we are using the old frame.
  return (m_pfba->size());
} 

// PixelBarrelFrame 
int PixelLegacyManager::PixelBarrelNBFrame() 
{
  if (m_BarrelInSFrame) {
    return (*m_pfba)[0]->getInt("NBOXI");
  } else {
    return (*m_pfec)[0]->getInt("NBOXI");
  }
}

int PixelLegacyManager::PixelBarrelNTFrame() 
{
  if (m_BarrelInSFrame) {
    return (*m_pfba)[0]->getInt("NTRDI");
  } else {
    return (*m_pfec)[0]->getInt("NTRDI");
  }
}

double PixelLegacyManager::PixelBarrelBFrameWidth() 
{
  if (m_BarrelInSFrame) {
    return (*m_pfba)[0]->getDouble("WIDTH1")*Gaudi::Units::cm;
  } else {
    return (*m_pfec)[0]->getDouble("WIDTH1")*Gaudi::Units::cm;
  }
}

double PixelLegacyManager::PixelBarrelTFrameWidth() 
{
  if (m_BarrelInSFrame) {
    return (*m_pfba)[0]->getDouble("WIDTH2")*Gaudi::Units::cm;
  } else {
    return (*m_pfec)[0]->getDouble("WIDTH2")*Gaudi::Units::cm;
  }
}

double PixelLegacyManager::PixelBarrelFrameLength() 
{
  if (m_BarrelInSFrame) {
    return (*m_pfba)[0]->getDouble("LENGTH")*Gaudi::Units::cm;
  } else {
    return (*m_pfec)[0]->getDouble("LENGTH")*Gaudi::Units::cm;
  }
}

double PixelLegacyManager::PixelBarrelFrameOffset() 
{
  if (m_BarrelInSFrame) {
    return (*m_pfba)[0]->getDouble("OFFSET")*Gaudi::Units::cm;
  } else {
    return (*m_pfec)[0]->getDouble("OFFSET")*Gaudi::Units::cm;
  }
}

// PixelEndcapFrame 
int PixelLegacyManager::PixelEndcapNBFrame() 
{
  return (*m_pecf)[0]->getInt("NBOXI");
}

int PixelLegacyManager::PixelEndcapNTFrame()
{
  return (*m_pecf)[0]->getInt("NTRDI");
}

double PixelLegacyManager::PixelEndcapBFrameWidth()
{
  return (*m_pecf)[0]->getDouble("WIDTH1")*Gaudi::Units::cm;
}

double PixelLegacyManager::PixelEndcapTFrameWidth() 
{
  return (*m_pecf)[0]->getDouble("WIDTH2")*Gaudi::Units::cm;
}

double PixelLegacyManager::PixelEndcapFrameLength() 
{
  return (*m_pecf)[0]->getDouble("LENGTH")*Gaudi::Units::cm;
}

double PixelLegacyManager::PixelEndcapFrameOffset() 
{
  return (*m_pecf)[0]->getDouble("OFFSET")*Gaudi::Units::cm;
}

double PixelLegacyManager::PixelBFrameHalfLength() 
{
  if (m_BarrelInSFrame) {
    return (*m_pbba)[0]->getDouble("DZ")*Gaudi::Units::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*m_pecb)[0]->getDouble("DZ")*Gaudi::Units::cm;
    } else {
      return (*m_pbec)[0]->getDouble("DZ")*Gaudi::Units::cm;
    }
  }
}

double PixelLegacyManager::PixelBFrameHalfWidth() 
{
  if (m_BarrelInSFrame) {
    return (*m_pbba)[0]->getDouble("DY")*Gaudi::Units::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*m_pecb)[0]->getDouble("DY")*Gaudi::Units::cm;
    } else {
      return (*m_pbec)[0]->getDouble("DY")*Gaudi::Units::cm;
    }
  }
}

double PixelLegacyManager::PixelBFrameHalfThickness() 
{
  if (m_BarrelInSFrame) {
    return (*m_pbba)[0]->getDouble("DX")*Gaudi::Units::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*m_pecb)[0]->getDouble("DX")*Gaudi::Units::cm;
    } else {
      return (*m_pbec)[0]->getDouble("DX")*Gaudi::Units::cm;
    }
  }
}

double PixelLegacyManager::PixelTFrameHalfLength() 
{
  if (m_BarrelInSFrame) {
    return (*m_ptba)[0]->getDouble("DZ")*Gaudi::Units::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*m_pect)[0]->getDouble("DZ")*Gaudi::Units::cm;
    } else {
      if (m_EndConeSFrame) {
        return (*m_pecn)[0]->getDouble("DZ")*Gaudi::Units::cm;
      } else {
        return (*m_ptec)[0]->getDouble("DZ")*Gaudi::Units::cm;
      } 
    }
  }
}

double PixelLegacyManager::PixelTFrameHalfWidthY() 
{
  if (m_BarrelInSFrame) {
    return (*m_ptba)[0]->getDouble("DY")*Gaudi::Units::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*m_pect)[0]->getDouble("DY")*Gaudi::Units::cm;
    } else {
      if (m_EndConeSFrame) {
        return (*m_pecn)[0]->getDouble("DY")*Gaudi::Units::cm;
      } else {
        return (*m_ptec)[0]->getDouble("DY")*Gaudi::Units::cm;
      } 
    }
  }
}

double PixelLegacyManager::PixelTFrameHalfWidthXzn() 
{
  if (m_BarrelInSFrame) {
    return (*m_ptba)[0]->getDouble("DX1")*Gaudi::Units::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*m_pect)[0]->getDouble("DX1")*Gaudi::Units::cm;
    } else {    
      if (m_EndConeSFrame) {
        return (*m_pecn)[0]->getDouble("DX1")*Gaudi::Units::cm;
      } else {
        return (*m_ptec)[0]->getDouble("DX1")*Gaudi::Units::cm;
      } 
    }
  }
}

double PixelLegacyManager::PixelTFrameHalfWidthXzp() 
{
  if (m_BarrelInSFrame) {
    return (*m_ptba)[0]->getDouble("DX2")*Gaudi::Units::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*m_pect)[0]->getDouble("DX2")*Gaudi::Units::cm;
    } else {
      if (m_EndConeSFrame) {
        return (*m_pecn)[0]->getDouble("DX2")*Gaudi::Units::cm;
      } else {
        return (*m_ptec)[0]->getDouble("DX2")*Gaudi::Units::cm;
      } 
    }
  }
}

double PixelLegacyManager::PixelTFrameDzDr()
{
  if (m_BarrelInSFrame) {
    return (*m_ptba)[0]->getDouble("DZDR")*Gaudi::Units::deg;
  } else {
    if (m_EndcapInSFrame) {
      return (*m_pect)[0]->getDouble("DZDR")*Gaudi::Units::cm;
    } else {
      if (m_EndConeSFrame) {
        return (*m_pecn)[0]->getDouble("DZDR")*Gaudi::Units::deg;
      } else {
        return (*m_ptec)[0]->getDouble("DZDR")*Gaudi::Units::deg;
      } 
    }
  }
}

double PixelLegacyManager::PixelBarrelFrameECRadius()
{
  return (*m_pecn)[0]->getDouble("RADIUS")*Gaudi::Units::cm;
}

double PixelLegacyManager::PixelBarrelFrameECZPos() 
{
  return (*m_pecn)[0]->getDouble("Z")*Gaudi::Units::cm;
}

double PixelLegacyManager::PixelBarrelFrameECAlphaX() 
{
  return (*m_pecn)[0]->getDouble("ANGLEX")*Gaudi::Units::deg;
}

double PixelLegacyManager::PixelBarrelFrameECAlphaY() 
{
  return (*m_pecn)[0]->getDouble("ANGLEY")*Gaudi::Units::deg;
}


double PixelLegacyManager::PixelLadderThickness() 
{
  return 2 * 1.48972*Gaudi::Units::mm;
}

double PixelLegacyManager::PixelLadderLength() 
{
  return 2 * (*m_pxbi)[0]->getDouble("DZLADDER")*Gaudi::Units::cm;
}

double PixelLegacyManager::PixelLadderServicesX() 
{
  return 0.5*PixelLadderThickness();
}

double PixelLegacyManager::PixelLadderServicesY() 
{
  return 3*Gaudi::Units::mm;
}


double PixelLegacyManager::PixelLadderCableOffsetX() 
{
  return 0.099*Gaudi::Units::mm;
}

double PixelLegacyManager::PixelLadderCableOffsetY() 
{
  return 4*Gaudi::Units::mm;
}

double PixelLegacyManager::PixelLadderConnectorOffsetX() 
{
  return -5.8*Gaudi::Units::mm;
}

double PixelLegacyManager::PixelLadderPigtailOffsetY() 
{
  return -0.5*Gaudi::Units::mm - PixelLadderCableOffsetY();
}


//
// Cable
//
int 
PixelLegacyManager::PixelCableElements()
{
  return (*m_pome)[0]->getInt("NPOCI");  
}

double 
PixelLegacyManager::PixelCableZStart(int index)
{
  // In old code two cables were connected to middle. Correction stops them touching.
  double correction = (index == 7) ? 0.000001*Gaudi::Units::cm : 0;
  return ((*m_poci)[index]->getDouble("Z")  -  (*m_poci)[index]->getDouble("DZ")) * Gaudi::Units::cm + correction;
}

double 
PixelLegacyManager::PixelCableZEnd(int index)
{
  // In old code two cables were connected to middle. Correction stops them touching.
  double correction = (index == 7) ? 0.000001*Gaudi::Units::cm : 0;
  return ((*m_poci)[index]->getDouble("Z") +  (*m_poci)[index]->getDouble("DZ")) * Gaudi::Units::cm + correction;
}

double 
PixelLegacyManager::PixelCableWidth(int index)
{
  return (*m_poci)[index]->getDouble("DY") * Gaudi::Units::cm;
}

double 
PixelLegacyManager::PixelCableThickness(int index)
{
  return (*m_poci)[index]->getDouble("DX")*Gaudi::Units::cm;
}

double 
PixelLegacyManager::PixelCableStackOffset(int index)
{
  return (*m_poci)[index]->getDouble("X")*Gaudi::Units::cm;
}


std::string
PixelLegacyManager::PixelCableLabel(int index)
{
  int label = (int)(*m_poci)[index]->getDouble("IBOX");
  std::ostringstream o;
  o << label;
  return o.str();
}


//
/// TMT
//
int PixelLegacyManager::PixelTMTNumParts()
{
  return (*m_ptla)[0]->getInt("NCENT") + (*m_ptla)[0]->getInt("NEND") + (*m_ptla)[0]->getInt("NTRAP");
}

double PixelLegacyManager::PixelTMTVariable(int iPart, const std::string & varName)
{
  int ncent = (*m_ptla)[0]->getInt("NCENT");
  int nend  = (*m_ptla)[0]->getInt("NEND");
  if (iPart < ncent) {
    return (*m_pctr)[iPart]->getDouble(varName);
  } else if (iPart < nend+ncent) {
    return (*m_pftr)[iPart-ncent]->getDouble(varName);
  } else {
    return (*m_pttr)[iPart-ncent-nend]->getDouble(varName);
  }
}

double PixelLegacyManager::PixelTMTDzdr(int iPart)
{
  return PixelTMTVariable(iPart, "DZDR") * Gaudi::Units::deg;
}


double PixelLegacyManager::PixelTMTPosX(int iPart)
{
  return PixelTMTVariable(iPart, "X") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelTMTPosZ(int iPart)
{
  return PixelTMTVariable(iPart, "Z") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelTMTLength(int iPart)
{
  return 2 * PixelTMTVariable(iPart, "DZ") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelTMTWidthX2(int iPart)
{
  return 2 * PixelTMTVariable(iPart, "DX2") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelTMTWidthX1(int iPart)
{
  return 2 * PixelTMTVariable(iPart, "DX1") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelTMTWidthY(int iPart)
{
  return 2 * PixelTMTVariable(iPart, "DY") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelTMTBaseX1(int iPart)
{
  double theta = PixelTMTDzdr(iPart);
  if (theta == 0) {
    return PixelTMTVariable(iPart, "X")*Gaudi::Units::cm + 0.25*(PixelTMTWidthX1(iPart)+PixelTMTWidthX2(iPart));
  } else {
    return  PixelTMTVariable(iPart, "X")*Gaudi::Units::cm - 0.5*PixelTMTLength(iPart) * tan(theta) + 0.5*PixelTMTWidthX1(iPart);
  }
}

double PixelLegacyManager::PixelTMTBaseX2(int iPart)
{
  double theta = PixelTMTDzdr(iPart);
  if (theta == 0) {
    return PixelTMTVariable(iPart, "X")*Gaudi::Units::cm + 0.25*(PixelTMTWidthX1(iPart)+PixelTMTWidthX2(iPart));
  } else {
    return PixelTMTVariable(iPart, "X")*Gaudi::Units::cm + 0.5*PixelTMTLength(iPart) * tan(theta) + 0.5*PixelTMTWidthX2(iPart);
  }
}

double PixelLegacyManager::PixelTMTPosY(int iPart)
{
  return PixelTMTVariable(iPart, "Y") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelTMTPosZ1(int iPart)
{
  return PixelTMTPosZ(iPart) - 0.5*PixelTMTLength(iPart);
}

double PixelLegacyManager::PixelTMTPosZ2(int iPart)
{
  return PixelTMTPosZ(iPart) + 0.5*PixelTMTLength(iPart);
}

bool PixelLegacyManager::PixelTMTPerModule(int iPart)
{ 
  int ncent =  (*m_ptla)[0]->getInt("NCENT");
  int nend =  (*m_ptla)[0]->getInt("NEND");
  return (iPart >= ncent+nend);
}

//
// Omega parameters
//
double PixelLegacyManager::PixelOmegaUpperBendX()
{
  return (*m_poti)[2]->getDouble("X") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaUpperBendY()
{
  return (*m_poti)[2]->getDouble("Y") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaUpperBendRadius()
{
  return (*m_poti)[2]->getDouble("RMAX") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaLowerBendX()
{
  return (*m_poti)[0]->getDouble("X") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaLowerBendY()
{
  return (*m_poti)[0]->getDouble("Y") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaLowerBendRadius()
{
  return (*m_poti)[0]->getDouble("RMAX") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaWallThickness()
{
  return ((*m_poti)[0]->getDouble("RMAX") - (*m_poti)[0]->getDouble("RMIN")) * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaLength()
{
  return 2. * (*m_poti)[0]->getDouble("DZ") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaStartY()
{
  return ((*m_posi)[0]->getDouble("Y") + 0.5*(*m_posi)[0]->getDouble("DY")) * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaEndY()
{
  return ((*m_posi)[1]->getDouble("Y") - 0.5*(*m_posi)[1]->getDouble("DY")) * Gaudi::Units::cm;
}

//
// Al Tube
//

double PixelLegacyManager::PixelAlTubeUpperBendX()
{
  return (*m_poti)[5]->getDouble("X") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelAlTubeUpperBendY()
{
  return (*m_poti)[5]->getDouble("Y") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelAlTubeUpperBendRadius()
{
  return (*m_poti)[5]->getDouble("RMAX") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelAlTubeLowerBendX()
{
  return (*m_poti)[3]->getDouble("X") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelAlTubeLowerBendY()
{
  return (*m_poti)[3]->getDouble("Y") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelAlTubeLowerBendRadius()
{
  return (*m_poti)[3]->getDouble("RMAX") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelAlTubeWallThickness()
{
  return ((*m_poti)[3]->getDouble("RMAX") - (*m_poti)[3]->getDouble("RMIN")) * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelAlTubeLength()
{
  return 2 * (*m_poti)[3]->getDouble("DZ") * Gaudi::Units::cm;
}

//
// Glue
// 

int PixelLegacyManager::PixelNumOmegaGlueElements()
{
  return 3;
}

double PixelLegacyManager::PixelOmegaGlueStartX(int index)
{
  return ((*m_posi)[index+2]->getDouble("X") - 0.5*(*m_posi)[index+2]->getDouble("DX")) * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaGlueThickness(int index)
{
  return (*m_posi)[index+2]->getDouble("DX") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaGlueStartY(int index)
{
  return ((*m_posi)[index+2]->getDouble("Y") - 0.5*(*m_posi)[index+2]->getDouble("DY")) * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaGlueEndY(int index)
{
  return ((*m_posi)[index+2]->getDouble("Y") + 0.5*(*m_posi)[index+2]->getDouble("DY")) * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaGlueLength(int index)
{
  return 2 * (*m_posi)[index+2]->getDouble("DZ") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelOmegaGluePosZ(int index)
{
  return (*m_posi)[index+2]->getDouble("Z") * Gaudi::Units::cm;
}

int PixelLegacyManager::PixelOmegaGlueTypeNum(int index)
{
  return index;
}

//
// Fluid
// 

double PixelLegacyManager::PixelFluidZ1(int index)
{
  double dz = (*m_pcff)[index%2]->getDouble("DZ")*Gaudi::Units::cm;
  double posz = (*m_pcff)[index%2]->getDouble("Z")*Gaudi::Units::cm;
  return posz-dz;
}

double PixelLegacyManager::PixelFluidZ2(int index)
{
  double dz = (*m_pcff)[index%2]->getDouble("DZ")*Gaudi::Units::cm;
  double posz = (*m_pcff)[index%2]->getDouble("Z")*Gaudi::Units::cm;
  return posz+dz;
}

double PixelLegacyManager::PixelFluidThick1(int index)
{
  if (index < 2) {
    return 2*(*m_pcff)[index%2]->getDouble("DX1")*Gaudi::Units::cm;
  } else {
    return 2*(*m_pcff)[index%2]->getDouble("DX2")*Gaudi::Units::cm;
  }
}


double PixelLegacyManager::PixelFluidThick2(int index)
{
  if (index < 2) {
    return 2*(*m_pcff)[index%2]->getDouble("DX2")*Gaudi::Units::cm;
  } else {
    return 2*(*m_pcff)[index%2]->getDouble("DX1")*Gaudi::Units::cm;
  }
}

double PixelLegacyManager::PixelFluidWidth(int index)
{
  return 2*(*m_pcff)[index%2]->getDouble("DY")*Gaudi::Units::cm;
}


double PixelLegacyManager::PixelFluidX(int index)
{
  return (*m_pcff)[index%2]->getDouble("X")*Gaudi::Units::cm;
}

double PixelLegacyManager::PixelFluidY(int index)
{
  return (*m_pcff)[index%2]->getDouble("Y")*Gaudi::Units::cm;
}

int PixelLegacyManager::PixelFluidType(int index)
{
  return index;
}

int PixelLegacyManager::PixelFluidNumTypes()
{
  return 4;
}

int PixelLegacyManager::PixelFluidMatType(int index) {
  return (int)(*m_pcff)[index%2]->getDouble("MAT");
}

int PixelLegacyManager::PixelFluidOrient(int layer, int phi) 
{
  std::string var = "ORIENT_";
  std::ostringstream o;
  o << phi;
  var = var+o.str();
  return (int)(*m_pccf)[layer]->getInt(var) - 1;
}


//
// Pigtail
//
double PixelLegacyManager::PixelPigtailThickness()
{
  return (*m_poai)[0]->getDouble("DX") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelPigtailStartY()
{
  return -0.5*(*m_poai)[0]->getDouble("DY") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelPigtailEndY()
{
  return  0.5*(*m_poai)[0]->getDouble("DY") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelPigtailFlatWidthZ()
{
  // Assume its actually the full width but in old geometry it was interpreted as a half width so we
  // multiply by 2. This gives the flat section twice the width of the curved section which I don't think was the 
  // intention.
  return 2*(*m_poai)[0]->getDouble("DZ") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelPigtailWidthZ()
{
  return 2*(*m_pobi)[0]->getDouble("DZ") * Gaudi::Units::cm;
}

// FIXME some weird offset
double PixelLegacyManager::PixelPigtailPosX()
{
  return (*m_poai)[0]->getDouble("X") * Gaudi::Units::cm + PixelLadderConnectorOffsetX();
}

double PixelLegacyManager::PixelPigtailPosZ()
{
  return (*m_poai)[0]->getDouble("Z") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelPigtailBendX()
{
  return (*m_pobi)[0]->getDouble("X") * Gaudi::Units::cm + PixelLadderConnectorOffsetX();
}

double PixelLegacyManager::PixelPigtailBendY()
{
  return (*m_pobi)[0]->getDouble("Y") * Gaudi::Units::cm + PixelLadderPigtailOffsetY();
}

double PixelLegacyManager::PixelPigtailBendRMin()
{
  return (*m_pobi)[0]->getDouble("RMIN") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelPigtailBendRMax()
{
  return (*m_pobi)[0]->getDouble("RMAX") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelPigtailBendPhiMin()
{
  return (*m_pobi)[0]->getDouble("PHI1") * Gaudi::Units::deg;
}

double PixelLegacyManager::PixelPigtailBendPhiMax()
{
  return (*m_pobi)[0]->getDouble("PHI2") * Gaudi::Units::deg;
}

double PixelLegacyManager::PixelPigtailEnvelopeLength()
{
  // FIXME Check
  return 2*(*m_posi)[9]->getDouble("DZ") * Gaudi::Units::cm;
}

//
// Connector
//
int PixelLegacyManager::PixelNumConnectorElements()
{
  return (*m_pome)[0]->getInt("NPOAI") - 1;
}

double PixelLegacyManager::PixelConnectorWidthX(int index)
{
  return (*m_poai)[index+1]->getDouble("DX") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelConnectorWidthY(int index)
{
  return (*m_poai)[index+1]->getDouble("DY") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelConnectorWidthZ(int index)
{
  return 2*(*m_poai)[index+1]->getDouble("DZ") * Gaudi::Units::cm;
}

double PixelLegacyManager::PixelConnectorPosX(int index)
{
  return (*m_poai)[index+1]->getDouble("X") * Gaudi::Units::cm + PixelLadderConnectorOffsetX();
}

double PixelLegacyManager::PixelConnectorPosY(int)
{
  return 0;
}

double PixelLegacyManager::PixelConnectorPosZ(int index)
{
  // same as  (*m_pobi)[0]->getDouble("Z") * Gaudi::Units::cm;
  return (*m_poai)[index+1]->getDouble("Z") * Gaudi::Units::cm;
}


//
// Design
//

int  PixelLegacyManager::designType(bool isBlayer)
{
  return (isBlayer) ? 0 : 1;
}

int PixelLegacyManager::DesignNumChipsPhi()
{
  return  (*m_pdch)[0]->getInt("NRPCHIP");
}    


int PixelLegacyManager::DesignNumChipsEta() {
  return  (*m_pdch)[0]->getInt("NZCHIP");
}

int PixelLegacyManager::DesignNumRowsPerChip(bool isBLayer) {
  double ActiveArea = DesignRPActiveArea();
  double GapRP = DesignGapRP();
  double pitchRP = DesignPitchRP(isBLayer);
  int numTotal =  static_cast<int>((ActiveArea - GapRP)/pitchRP + 0.5);
  return numTotal / DesignNumChipsPhi();
}

int PixelLegacyManager::DesignNumColsPerChip(bool isBLayer) {
  double ActiveArea = DesignZActiveArea();
  double GapZ = DesignGapZ();
  double pitchZ = DesignPitchZ(isBLayer); 
  int CPR = DesignCircuitsEta();
  return static_cast<int>((ActiveArea - GapZ*CPR)/(CPR*pitchZ) + 0.5);
}


int PixelLegacyManager::DesignDiodesPhiTotal(bool isBLayer)
{
  double ActiveArea = DesignRPActiveArea();
  double pitchRP = DesignPitchRP(isBLayer);
  return static_cast<int>((ActiveArea)/pitchRP + 0.5);
}

int PixelLegacyManager::DesignDiodesEtaTotal(bool isBLayer)
{
  double ActiveArea = DesignRPActiveArea();
  double pitchRP = DesignPitchRP(isBLayer);
  return static_cast<int>((ActiveArea)/pitchRP + 0.5);
}


int  PixelLegacyManager::DesignNumEmptyRowsInGap()
{
  return static_cast<int>((*m_pdch)[0]->getInt("NYCONNEC"));
}

// Ganged Pixels
int PixelLegacyManager::NumberOfEmptyRows()
{
  return static_cast<int>((*m_pdch)[0]->getInt("NYCONNEC"));

}

int PixelLegacyManager::EmptyRows(int index)
{
  std::ostringstream A;
  A << "_" << index;
  return static_cast<int>((*m_pdch)[0]->getInt("JYEMPTY"+A.str())) - 1;
}

int PixelLegacyManager::EmptyRowConnections(int index)
{
  std::ostringstream A;
  A << "_" << index;
  return static_cast<int>((*m_pdch)[0]->getInt("JYCONNEC"+A.str())) - 1;
}

double PixelLegacyManager::DesignRPActiveArea()
{
  return (*m_pxbi)[0]->getDouble("DYACTIVE")*Gaudi::Units::cm;

}

double PixelLegacyManager::DesignZActiveArea()
{
  return (*m_pxbi)[0]->getDouble("DZELEB")*Gaudi::Units::cm;
}
    
double PixelLegacyManager::DesignPitchRP(bool isBLayer)
{
  int type = designType(isBLayer);
  return (*m_pxbd)[type]->getDouble("PITCHRP")*Gaudi::Units::cm;
}

// FIXME m_dc1Geometry
double PixelLegacyManager::DesignPitchZ(bool isBLayer)
{
  int type = designType(isBLayer);
  double pitchZ;
  if(isBLayer && m_dc1Geometry) {
    // Override NOVA 
    pitchZ = 300 * Gaudi::Units::micrometer; 
  } else {
    pitchZ = (*m_pxbd)[type]->getDouble("PITCHZ") * Gaudi::Units::cm;
  }
  return pitchZ;
}

double PixelLegacyManager::DesignPitchZLong(bool isBLayer) 
{
  return DesignPitchZ(isBLayer) + DesignGapZ()/2;
}


double PixelLegacyManager::DesignGapRP()
{
  return (*m_pdch)[0]->getDouble("GAPRP")*Gaudi::Units::cm;
}

double PixelLegacyManager::DesignGapZ()
{
  return (*m_pdch)[0]->getDouble("GAPZ")*Gaudi::Units::cm;
}

int PixelLegacyManager::DesignCircuitsEta()
{
  return DesignNumChipsEta();
}
