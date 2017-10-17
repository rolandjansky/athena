/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1PRDSystems/PRDDetTypes.h"

//______________________________________________
QString PRDDetType::typeToString(const Type& t)
{
  switch (t) {
  case Pixel: return "Pixel";
  case SCT: return "SCT";
  case TRT: return "TRT";
  case CSC: return "CSC";
  case CSCstrip: return "CSCstrip";
  case RPC: return "RPC";
  case TGC: return "TGC";
  case sTGC: return "sTGC";
  case MDT: return "MDT";
  case MM: return "MM";
  case SpacePoints: return "SpacePoints";
  default: return "UNKNOWN";
  }
}

//______________________________________________
PRDDetType::Type PRDDetType::stringToType(const QString&str, bool & status)
{
  status = true;
  if (str=="Pixel") return Pixel;
  if (str=="SCT") return SCT;
  if (str=="TRT") return TRT;
  if (str=="CSC") return CSC;
  if (str=="CSCstrip") return CSCstrip;
  if (str=="RPC") return RPC;
  if (str=="TGC") return TGC;
  if (str=="sTGC") return sTGC;
  if (str=="MDT") return MDT;
  if (str=="MM") return MM;
  if (str=="SpacePoints") return SpacePoints;
  status = false;
  return Pixel;//people better watch the return status!
}


//______________________________________________
qint32 PRDDetType::typeToInt(const Type& t)
{
  switch (t) {
  case Pixel: return 0;
  case SCT: return 1;
  case TRT: return 2;
  case CSC: return 3;
  case RPC: return 4;
  case TGC: return 5;
  case MDT: return 6;
  case SpacePoints: return 7;
  case CSCstrip: return 8;
  case sTGC: return 9;
  case MM: return 10;
  default: return -1;
  }
}

//______________________________________________
PRDDetType::Type PRDDetType::intToType(const qint32&i, bool & status)
{
  status = true;
  switch (i) {
  case 0: return Pixel;
  case 1: return SCT;
  case 2: return TRT;
  case 3: return CSC;
  case 4: return RPC;
  case 5: return TGC;
  case 6: return MDT;
  case 7: return SpacePoints;
  case 8: return CSCstrip;
  case 9: return sTGC;
  case 10: return MM;
  default:
    status = false;
    return Pixel;//people better watch the return status!
  }
}
