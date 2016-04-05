/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelGeoModel/PixelLegacyManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "CLHEP/Units/SystemOfUnits.h"

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
  pfba = rdbSvc->getRecordsetPtr("PFBA",     detectorKey, detectorNode);
  pbba = rdbSvc->getRecordsetPtr("PBBA",     detectorKey, detectorNode);
  ptba = rdbSvc->getRecordsetPtr("PTBA",     detectorKey, detectorNode);
  pfec = rdbSvc->getRecordsetPtr("PFEC",     detectorKey, detectorNode);
  pbec = rdbSvc->getRecordsetPtr("PBEC",     detectorKey, detectorNode);
  ptec = rdbSvc->getRecordsetPtr("PTEC",     detectorKey, detectorNode);
  pecn = rdbSvc->getRecordsetPtr("PECN",     detectorKey, detectorNode);
  pecf = rdbSvc->getRecordsetPtr("PECF",     detectorKey, detectorNode);
  pecb = rdbSvc->getRecordsetPtr("PECB",     detectorKey, detectorNode);
  pect = rdbSvc->getRecordsetPtr("PECT",     detectorKey, detectorNode);

  // These are for the design
  pxbi = rdbSvc->getRecordsetPtr("PXBI",     detectorKey, detectorNode);
  pdch = rdbSvc->getRecordsetPtr("PDCH",     detectorKey, detectorNode);
  pxbd = rdbSvc->getRecordsetPtr("PXBD",     detectorKey, detectorNode);

  // These are for a detailed description of the ladders and services on ladder
  ptla = rdbSvc->getRecordsetPtr("PTLA",     detectorKey, detectorNode);
  pctr = rdbSvc->getRecordsetPtr("PCTR",     detectorKey, detectorNode);
  pftr = rdbSvc->getRecordsetPtr("PFTR",     detectorKey, detectorNode);
  pttr = rdbSvc->getRecordsetPtr("PTTR",     detectorKey, detectorNode);
  pome = rdbSvc->getRecordsetPtr("POME",     detectorKey, detectorNode);
  poti = rdbSvc->getRecordsetPtr("POTI",     detectorKey, detectorNode);
  pobi = rdbSvc->getRecordsetPtr("POBI",     detectorKey, detectorNode);
  poai = rdbSvc->getRecordsetPtr("POAI",     detectorKey, detectorNode);
  poci = rdbSvc->getRecordsetPtr("POCI",     detectorKey, detectorNode);
  posi = rdbSvc->getRecordsetPtr("POSI",     detectorKey, detectorNode);
  pccf = rdbSvc->getRecordsetPtr("PCCF",     detectorKey, detectorNode);
  pcff = rdbSvc->getRecordsetPtr("PCFF",     detectorKey, detectorNode);

}


bool PixelLegacyManager::oldFrame()
{
  // if pfba exists we are using the old frame.
  return (pfba->size());
} 

// PixelBarrelFrame 
int PixelLegacyManager::PixelBarrelNBFrame() 
{
  if (m_BarrelInSFrame) {
    return (*pfba)[0]->getInt("NBOXI");
  } else {
    return (*pfec)[0]->getInt("NBOXI");
  }
}

int PixelLegacyManager::PixelBarrelNTFrame() 
{
  if (m_BarrelInSFrame) {
    return (*pfba)[0]->getInt("NTRDI");
  } else {
    return (*pfec)[0]->getInt("NTRDI");
  }
}

double PixelLegacyManager::PixelBarrelBFrameWidth() 
{
  if (m_BarrelInSFrame) {
    return (*pfba)[0]->getDouble("WIDTH1")*CLHEP::cm;
  } else {
    return (*pfec)[0]->getDouble("WIDTH1")*CLHEP::cm;
  }
}

double PixelLegacyManager::PixelBarrelTFrameWidth() 
{
  if (m_BarrelInSFrame) {
    return (*pfba)[0]->getDouble("WIDTH2")*CLHEP::cm;
  } else {
    return (*pfec)[0]->getDouble("WIDTH2")*CLHEP::cm;
  }
}

double PixelLegacyManager::PixelBarrelFrameLength() 
{
  if (m_BarrelInSFrame) {
    return (*pfba)[0]->getDouble("LENGTH")*CLHEP::cm;
  } else {
    return (*pfec)[0]->getDouble("LENGTH")*CLHEP::cm;
  }
}

double PixelLegacyManager::PixelBarrelFrameOffset() 
{
  if (m_BarrelInSFrame) {
    return (*pfba)[0]->getDouble("OFFSET")*CLHEP::cm;
  } else {
    return (*pfec)[0]->getDouble("OFFSET")*CLHEP::cm;
  }
}

// PixelEndcapFrame 
int PixelLegacyManager::PixelEndcapNBFrame() 
{
  return (*pecf)[0]->getInt("NBOXI");
}

int PixelLegacyManager::PixelEndcapNTFrame()
{
  return (*pecf)[0]->getInt("NTRDI");
}

double PixelLegacyManager::PixelEndcapBFrameWidth()
{
  return (*pecf)[0]->getDouble("WIDTH1")*CLHEP::cm;
}

double PixelLegacyManager::PixelEndcapTFrameWidth() 
{
  return (*pecf)[0]->getDouble("WIDTH2")*CLHEP::cm;
}

double PixelLegacyManager::PixelEndcapFrameLength() 
{
  return (*pecf)[0]->getDouble("LENGTH")*CLHEP::cm;
}

double PixelLegacyManager::PixelEndcapFrameOffset() 
{
  return (*pecf)[0]->getDouble("OFFSET")*CLHEP::cm;
}

double PixelLegacyManager::PixelBFrameHalfLength() 
{
  if (m_BarrelInSFrame) {
    return (*pbba)[0]->getDouble("DZ")*CLHEP::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*pecb)[0]->getDouble("DZ")*CLHEP::cm;
    } else {
      return (*pbec)[0]->getDouble("DZ")*CLHEP::cm;
    }
  }
}

double PixelLegacyManager::PixelBFrameHalfWidth() 
{
  if (m_BarrelInSFrame) {
    return (*pbba)[0]->getDouble("DY")*CLHEP::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*pecb)[0]->getDouble("DY")*CLHEP::cm;
    } else {
      return (*pbec)[0]->getDouble("DY")*CLHEP::cm;
    }
  }
}

double PixelLegacyManager::PixelBFrameHalfThickness() 
{
  if (m_BarrelInSFrame) {
    return (*pbba)[0]->getDouble("DX")*CLHEP::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*pecb)[0]->getDouble("DX")*CLHEP::cm;
    } else {
      return (*pbec)[0]->getDouble("DX")*CLHEP::cm;
    }
  }
}

double PixelLegacyManager::PixelTFrameHalfLength() 
{
  if (m_BarrelInSFrame) {
    return (*ptba)[0]->getDouble("DZ")*CLHEP::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*pect)[0]->getDouble("DZ")*CLHEP::cm;
    } else {
      if (m_EndConeSFrame) {
        return (*pecn)[0]->getDouble("DZ")*CLHEP::cm;
      } else {
        return (*ptec)[0]->getDouble("DZ")*CLHEP::cm;
      } 
    }
  }
}

double PixelLegacyManager::PixelTFrameHalfWidthY() 
{
  if (m_BarrelInSFrame) {
    return (*ptba)[0]->getDouble("DY")*CLHEP::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*pect)[0]->getDouble("DY")*CLHEP::cm;
    } else {
      if (m_EndConeSFrame) {
        return (*pecn)[0]->getDouble("DY")*CLHEP::cm;
      } else {
        return (*ptec)[0]->getDouble("DY")*CLHEP::cm;
      } 
    }
  }
}

double PixelLegacyManager::PixelTFrameHalfWidthXzn() 
{
  if (m_BarrelInSFrame) {
    return (*ptba)[0]->getDouble("DX1")*CLHEP::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*pect)[0]->getDouble("DX1")*CLHEP::cm;
    } else {    
      if (m_EndConeSFrame) {
        return (*pecn)[0]->getDouble("DX1")*CLHEP::cm;
      } else {
        return (*ptec)[0]->getDouble("DX1")*CLHEP::cm;
      } 
    }
  }
}

double PixelLegacyManager::PixelTFrameHalfWidthXzp() 
{
  if (m_BarrelInSFrame) {
    return (*ptba)[0]->getDouble("DX2")*CLHEP::cm;
  } else {
    if (m_EndcapInSFrame) {
      return (*pect)[0]->getDouble("DX2")*CLHEP::cm;
    } else {
      if (m_EndConeSFrame) {
        return (*pecn)[0]->getDouble("DX2")*CLHEP::cm;
      } else {
        return (*ptec)[0]->getDouble("DX2")*CLHEP::cm;
      } 
    }
  }
}

double PixelLegacyManager::PixelTFrameDzDr()
{
  if (m_BarrelInSFrame) {
    return (*ptba)[0]->getDouble("DZDR")*CLHEP::deg;
  } else {
    if (m_EndcapInSFrame) {
      return (*pect)[0]->getDouble("DZDR")*CLHEP::cm;
    } else {
      if (m_EndConeSFrame) {
        return (*pecn)[0]->getDouble("DZDR")*CLHEP::deg;
      } else {
        return (*ptec)[0]->getDouble("DZDR")*CLHEP::deg;
      } 
    }
  }
}

double PixelLegacyManager::PixelBarrelFrameECRadius()
{
  return (*pecn)[0]->getDouble("RADIUS")*CLHEP::cm;
}

double PixelLegacyManager::PixelBarrelFrameECZPos() 
{
  return (*pecn)[0]->getDouble("Z")*CLHEP::cm;
}

double PixelLegacyManager::PixelBarrelFrameECAlphaX() 
{
  return (*pecn)[0]->getDouble("ANGLEX")*CLHEP::deg;
}

double PixelLegacyManager::PixelBarrelFrameECAlphaY() 
{
  return (*pecn)[0]->getDouble("ANGLEY")*CLHEP::deg;
}


double PixelLegacyManager::PixelLadderThickness() 
{
  return 2 * 1.48972*CLHEP::mm;
}

double PixelLegacyManager::PixelLadderLength() 
{
  return 2 * (*pxbi)[0]->getDouble("DZLADDER")*CLHEP::cm;
}

double PixelLegacyManager::PixelLadderServicesX() 
{
  return 0.5*PixelLadderThickness();
}

double PixelLegacyManager::PixelLadderServicesY() 
{
  return 3*CLHEP::mm;
}


double PixelLegacyManager::PixelLadderCableOffsetX() 
{
  return 0.099*CLHEP::mm;
}

double PixelLegacyManager::PixelLadderCableOffsetY() 
{
  return 4*CLHEP::mm;
}

double PixelLegacyManager::PixelLadderConnectorOffsetX() 
{
  return -5.8*CLHEP::mm;
}

double PixelLegacyManager::PixelLadderPigtailOffsetY() 
{
  return -0.5*CLHEP::mm - PixelLadderCableOffsetY();
}


//
// Cable
//
int 
PixelLegacyManager::PixelCableElements()
{
  return (*pome)[0]->getInt("NPOCI");  
}

double 
PixelLegacyManager::PixelCableZStart(int index)
{
  // In old code two cables were connected to middle. Correction stops them touching.
  double correction = (index == 7) ? 0.000001*CLHEP::cm : 0;
  return ((*poci)[index]->getDouble("Z")  -  (*poci)[index]->getDouble("DZ")) * CLHEP::cm + correction;
}

double 
PixelLegacyManager::PixelCableZEnd(int index)
{
  // In old code two cables were connected to middle. Correction stops them touching.
  double correction = (index == 7) ? 0.000001*CLHEP::cm : 0;
  return ((*poci)[index]->getDouble("Z") +  (*poci)[index]->getDouble("DZ")) * CLHEP::cm + correction;
}

double 
PixelLegacyManager::PixelCableWidth(int index)
{
  return (*poci)[index]->getDouble("DY") * CLHEP::cm;
}

double 
PixelLegacyManager::PixelCableThickness(int index)
{
  return (*poci)[index]->getDouble("DX")*CLHEP::cm;
}

double 
PixelLegacyManager::PixelCableStackOffset(int index)
{
  return (*poci)[index]->getDouble("X")*CLHEP::cm;
}


std::string
PixelLegacyManager::PixelCableLabel(int index)
{
  int label = (int)(*poci)[index]->getDouble("IBOX");
  std::ostringstream o;
  o << label;
  return o.str();
}


//
/// TMT
//
int PixelLegacyManager::PixelTMTNumParts()
{
  return (*ptla)[0]->getInt("NCENT") + (*ptla)[0]->getInt("NEND") + (*ptla)[0]->getInt("NTRAP");
}

double PixelLegacyManager::PixelTMTVariable(int iPart, const std::string & varName)
{
  int ncent = (*ptla)[0]->getInt("NCENT");
  int nend  = (*ptla)[0]->getInt("NEND");
  if (iPart < ncent) {
    return (*pctr)[iPart]->getDouble(varName);
  } else if (iPart < nend+ncent) {
    return (*pftr)[iPart-ncent]->getDouble(varName);
  } else {
    return (*pttr)[iPart-ncent-nend]->getDouble(varName);
  }
}

double PixelLegacyManager::PixelTMTDzdr(int iPart)
{
  return PixelTMTVariable(iPart, "DZDR") * CLHEP::deg;
}


double PixelLegacyManager::PixelTMTPosX(int iPart)
{
  return PixelTMTVariable(iPart, "X") * CLHEP::cm;
}

double PixelLegacyManager::PixelTMTPosZ(int iPart)
{
  return PixelTMTVariable(iPart, "Z") * CLHEP::cm;
}

double PixelLegacyManager::PixelTMTLength(int iPart)
{
  return 2 * PixelTMTVariable(iPart, "DZ") * CLHEP::cm;
}

double PixelLegacyManager::PixelTMTWidthX2(int iPart)
{
  return 2 * PixelTMTVariable(iPart, "DX2") * CLHEP::cm;
}

double PixelLegacyManager::PixelTMTWidthX1(int iPart)
{
  return 2 * PixelTMTVariable(iPart, "DX1") * CLHEP::cm;
}

double PixelLegacyManager::PixelTMTWidthY(int iPart)
{
  return 2 * PixelTMTVariable(iPart, "DY") * CLHEP::cm;
}

double PixelLegacyManager::PixelTMTBaseX1(int iPart)
{
  double theta = PixelTMTDzdr(iPart);
  if (theta == 0) {
    return PixelTMTVariable(iPart, "X")*CLHEP::cm + 0.25*(PixelTMTWidthX1(iPart)+PixelTMTWidthX2(iPart));
  } else {
    return  PixelTMTVariable(iPart, "X")*CLHEP::cm - 0.5*PixelTMTLength(iPart) * tan(theta) + 0.5*PixelTMTWidthX1(iPart);
  }
}

double PixelLegacyManager::PixelTMTBaseX2(int iPart)
{
  double theta = PixelTMTDzdr(iPart);
  if (theta == 0) {
    return PixelTMTVariable(iPart, "X")*CLHEP::cm + 0.25*(PixelTMTWidthX1(iPart)+PixelTMTWidthX2(iPart));
  } else {
    return PixelTMTVariable(iPart, "X")*CLHEP::cm + 0.5*PixelTMTLength(iPart) * tan(theta) + 0.5*PixelTMTWidthX2(iPart);
  }
}

double PixelLegacyManager::PixelTMTPosY(int iPart)
{
  return PixelTMTVariable(iPart, "Y") * CLHEP::cm;
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
  int ncent =  (*ptla)[0]->getInt("NCENT");
  int nend =  (*ptla)[0]->getInt("NEND");
  return (iPart >= ncent+nend);
}

//
// Omega parameters
//
double PixelLegacyManager::PixelOmegaUpperBendX()
{
  return (*poti)[2]->getDouble("X") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaUpperBendY()
{
  return (*poti)[2]->getDouble("Y") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaUpperBendRadius()
{
  return (*poti)[2]->getDouble("RMAX") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaLowerBendX()
{
  return (*poti)[0]->getDouble("X") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaLowerBendY()
{
  return (*poti)[0]->getDouble("Y") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaLowerBendRadius()
{
  return (*poti)[0]->getDouble("RMAX") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaWallThickness()
{
  return ((*poti)[0]->getDouble("RMAX") - (*poti)[0]->getDouble("RMIN")) * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaLength()
{
  return 2. * (*poti)[0]->getDouble("DZ") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaStartY()
{
  return ((*posi)[0]->getDouble("Y") + 0.5*(*posi)[0]->getDouble("DY")) * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaEndY()
{
  return ((*posi)[1]->getDouble("Y") - 0.5*(*posi)[1]->getDouble("DY")) * CLHEP::cm;
}

//
// Al Tube
//

double PixelLegacyManager::PixelAlTubeUpperBendX()
{
  return (*poti)[5]->getDouble("X") * CLHEP::cm;
}

double PixelLegacyManager::PixelAlTubeUpperBendY()
{
  return (*poti)[5]->getDouble("Y") * CLHEP::cm;
}

double PixelLegacyManager::PixelAlTubeUpperBendRadius()
{
  return (*poti)[5]->getDouble("RMAX") * CLHEP::cm;
}

double PixelLegacyManager::PixelAlTubeLowerBendX()
{
  return (*poti)[3]->getDouble("X") * CLHEP::cm;
}

double PixelLegacyManager::PixelAlTubeLowerBendY()
{
  return (*poti)[3]->getDouble("Y") * CLHEP::cm;
}

double PixelLegacyManager::PixelAlTubeLowerBendRadius()
{
  return (*poti)[3]->getDouble("RMAX") * CLHEP::cm;
}

double PixelLegacyManager::PixelAlTubeWallThickness()
{
  return ((*poti)[3]->getDouble("RMAX") - (*poti)[3]->getDouble("RMIN")) * CLHEP::cm;
}

double PixelLegacyManager::PixelAlTubeLength()
{
  return 2 * (*poti)[3]->getDouble("DZ") * CLHEP::cm;
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
  return ((*posi)[index+2]->getDouble("X") - 0.5*(*posi)[index+2]->getDouble("DX")) * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaGlueThickness(int index)
{
  return (*posi)[index+2]->getDouble("DX") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaGlueStartY(int index)
{
  return ((*posi)[index+2]->getDouble("Y") - 0.5*(*posi)[index+2]->getDouble("DY")) * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaGlueEndY(int index)
{
  return ((*posi)[index+2]->getDouble("Y") + 0.5*(*posi)[index+2]->getDouble("DY")) * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaGlueLength(int index)
{
  return 2 * (*posi)[index+2]->getDouble("DZ") * CLHEP::cm;
}

double PixelLegacyManager::PixelOmegaGluePosZ(int index)
{
  return (*posi)[index+2]->getDouble("Z") * CLHEP::cm;
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
  double dz = (*pcff)[index%2]->getDouble("DZ")*CLHEP::cm;
  double posz = (*pcff)[index%2]->getDouble("Z")*CLHEP::cm;
  return posz-dz;
}

double PixelLegacyManager::PixelFluidZ2(int index)
{
  double dz = (*pcff)[index%2]->getDouble("DZ")*CLHEP::cm;
  double posz = (*pcff)[index%2]->getDouble("Z")*CLHEP::cm;
  return posz+dz;
}

double PixelLegacyManager::PixelFluidThick1(int index)
{
  if (index < 2) {
    return 2*(*pcff)[index%2]->getDouble("DX1")*CLHEP::cm;
  } else {
    return 2*(*pcff)[index%2]->getDouble("DX2")*CLHEP::cm;
  }
}


double PixelLegacyManager::PixelFluidThick2(int index)
{
  if (index < 2) {
    return 2*(*pcff)[index%2]->getDouble("DX2")*CLHEP::cm;
  } else {
    return 2*(*pcff)[index%2]->getDouble("DX1")*CLHEP::cm;
  }
}

double PixelLegacyManager::PixelFluidWidth(int index)
{
  return 2*(*pcff)[index%2]->getDouble("DY")*CLHEP::cm;
}


double PixelLegacyManager::PixelFluidX(int index)
{
  return (*pcff)[index%2]->getDouble("X")*CLHEP::cm;
}

double PixelLegacyManager::PixelFluidY(int index)
{
  return (*pcff)[index%2]->getDouble("Y")*CLHEP::cm;
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
  return (int)(*pcff)[index%2]->getDouble("MAT");
}

int PixelLegacyManager::PixelFluidOrient(int layer, int phi) 
{
  std::string var = "ORIENT_";
  std::ostringstream o;
  o << phi;
  var = var+o.str();
  return (int)(*pccf)[layer]->getInt(var) - 1;
}


//
// Pigtail
//
double PixelLegacyManager::PixelPigtailThickness()
{
  return (*poai)[0]->getDouble("DX") * CLHEP::cm;
}

double PixelLegacyManager::PixelPigtailStartY()
{
  return -0.5*(*poai)[0]->getDouble("DY") * CLHEP::cm;
}

double PixelLegacyManager::PixelPigtailEndY()
{
  return  0.5*(*poai)[0]->getDouble("DY") * CLHEP::cm;
}

double PixelLegacyManager::PixelPigtailFlatWidthZ()
{
  // Assume its actually the full width but in old geometry it was interpreted as a half width so we
  // multiply by 2. This gives the flat section twice the width of the curved section which I don't think was the 
  // intention.
  return 2*(*poai)[0]->getDouble("DZ") * CLHEP::cm;
}

double PixelLegacyManager::PixelPigtailWidthZ()
{
  return 2*(*pobi)[0]->getDouble("DZ") * CLHEP::cm;
}

// FIXME some weird offset
double PixelLegacyManager::PixelPigtailPosX()
{
  return (*poai)[0]->getDouble("X") * CLHEP::cm + PixelLadderConnectorOffsetX();
}

double PixelLegacyManager::PixelPigtailPosZ()
{
  return (*poai)[0]->getDouble("Z") * CLHEP::cm;
}

double PixelLegacyManager::PixelPigtailBendX()
{
  return (*pobi)[0]->getDouble("X") * CLHEP::cm + PixelLadderConnectorOffsetX();
}

double PixelLegacyManager::PixelPigtailBendY()
{
  return (*pobi)[0]->getDouble("Y") * CLHEP::cm + PixelLadderPigtailOffsetY();
}

double PixelLegacyManager::PixelPigtailBendRMin()
{
  return (*pobi)[0]->getDouble("RMIN") * CLHEP::cm;
}

double PixelLegacyManager::PixelPigtailBendRMax()
{
  return (*pobi)[0]->getDouble("RMAX") * CLHEP::cm;
}

double PixelLegacyManager::PixelPigtailBendPhiMin()
{
  return (*pobi)[0]->getDouble("PHI1") * CLHEP::deg;
}

double PixelLegacyManager::PixelPigtailBendPhiMax()
{
  return (*pobi)[0]->getDouble("PHI2") * CLHEP::deg;
}

double PixelLegacyManager::PixelPigtailEnvelopeLength()
{
  // FIXME Check
  return 2*(*posi)[9]->getDouble("DZ") * CLHEP::cm;
}

//
// Connector
//
int PixelLegacyManager::PixelNumConnectorElements()
{
  return (*pome)[0]->getInt("NPOAI") - 1;
}

double PixelLegacyManager::PixelConnectorWidthX(int index)
{
  return (*poai)[index+1]->getDouble("DX") * CLHEP::cm;
}

double PixelLegacyManager::PixelConnectorWidthY(int index)
{
  return (*poai)[index+1]->getDouble("DY") * CLHEP::cm;
}

double PixelLegacyManager::PixelConnectorWidthZ(int index)
{
  return 2*(*poai)[index+1]->getDouble("DZ") * CLHEP::cm;
}

double PixelLegacyManager::PixelConnectorPosX(int index)
{
  return (*poai)[index+1]->getDouble("X") * CLHEP::cm + PixelLadderConnectorOffsetX();
}

double PixelLegacyManager::PixelConnectorPosY(int)
{
  return 0;
}

double PixelLegacyManager::PixelConnectorPosZ(int index)
{
  // same as  (*pobi)[0]->getDouble("Z") * CLHEP::cm;
  return (*poai)[index+1]->getDouble("Z") * CLHEP::cm;
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
  return  (*pdch)[0]->getInt("NRPCHIP");
}    


int PixelLegacyManager::DesignNumChipsEta() {
  return  (*pdch)[0]->getInt("NZCHIP");
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
  return static_cast<int>((*pdch)[0]->getInt("NYCONNEC"));
}

// Ganged Pixels
int PixelLegacyManager::NumberOfEmptyRows()
{
  return static_cast<int>((*pdch)[0]->getInt("NYCONNEC"));

}

int PixelLegacyManager::EmptyRows(int index)
{
  std::ostringstream A;
  A << "_" << index;
  return static_cast<int>((*pdch)[0]->getInt("JYEMPTY"+A.str())) - 1;
}

int PixelLegacyManager::EmptyRowConnections(int index)
{
  std::ostringstream A;
  A << "_" << index;
  return static_cast<int>((*pdch)[0]->getInt("JYCONNEC"+A.str())) - 1;
}

double PixelLegacyManager::DesignRPActiveArea()
{
  return (*pxbi)[0]->getDouble("DYACTIVE")*CLHEP::cm;

}

double PixelLegacyManager::DesignZActiveArea()
{
  return (*pxbi)[0]->getDouble("DZELEB")*CLHEP::cm;
}
    
double PixelLegacyManager::DesignPitchRP(bool isBLayer)
{
  int type = designType(isBLayer);
  return (*pxbd)[type]->getDouble("PITCHRP")*CLHEP::cm;
}

// FIXME m_dc1Geometry
double PixelLegacyManager::DesignPitchZ(bool isBLayer)
{
  int type = designType(isBLayer);
  double pitchZ;
  if(isBLayer && m_dc1Geometry) {
    // Override NOVA 
    pitchZ = 300 * CLHEP::micrometer; 
  } else {
    pitchZ = (*pxbd)[type]->getDouble("PITCHZ") * CLHEP::cm;
  }
  return pitchZ;
}

double PixelLegacyManager::DesignPitchZLong(bool isBLayer) 
{
  return DesignPitchZ(isBLayer) + DesignGapZ()/2;
}


double PixelLegacyManager::DesignGapRP()
{
  return (*pdch)[0]->getDouble("GAPRP")*CLHEP::cm;
}

double PixelLegacyManager::DesignGapZ()
{
  return (*pdch)[0]->getDouble("GAPZ")*CLHEP::cm;
}

int PixelLegacyManager::DesignCircuitsEta()
{
  return DesignNumChipsEta();
}
