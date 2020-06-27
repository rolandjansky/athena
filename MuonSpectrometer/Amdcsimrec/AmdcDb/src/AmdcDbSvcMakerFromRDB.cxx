/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcDb/AmdcDbSvcMakerFromRDB.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDbSvc.h"
#include "AmdcDb/AmdcDbRecordset.h"
#include "AmdcDb/AmdcDbRecord.h"
#include "boost/range/iterator_range.hpp"

namespace AmdcDb_detail {


struct VarDesc
{
  enum VarType {
    FloatCM,
    FloatMM,
    FloatAng,
    Int,
    String
  };
  VarType varType;
  const char* varName;
  const char* varComment;
};


void addVar (const VarDesc& desc,
             IRDBRecord* rec,
             AmdcDbRecord* pAmdcDbRecord,
             int precisionCM,
             int precisionMM = 0,
             int precisionAng = 0)
{
  switch (desc.varType) {
  case VarDesc::FloatCM:
    {
      double dbVal = rec->getFloat (desc.varName);
      pAmdcDbRecord->addDouble (desc.varName, desc.varComment,
                                dbVal, precisionCM);
      break;
    }

  case VarDesc::FloatMM:
    {
      double dbVal = rec->getFloat (desc.varName);
      pAmdcDbRecord->addDouble (desc.varName, desc.varComment,
                                dbVal, precisionMM);
      break;
    }

  case VarDesc::FloatAng:
    {
      double dbVal = rec->getFloat (desc.varName);
      pAmdcDbRecord->addDouble (desc.varName, desc.varComment,
                                dbVal, precisionAng);
      break;
    }

  case VarDesc::Int:
    {
      int dbVal = rec->getInt (desc.varName);
      pAmdcDbRecord->addInt (desc.varName, desc.varComment, dbVal);
      break;
    }

  case VarDesc::String:
    {
      std::string dbVal = rec->getString (desc.varName);
      pAmdcDbRecord->addString (desc.varName, desc.varComment, dbVal);
      break;
    }
  }
}


} // AmdcDb_detail


using namespace AmdcDb_detail;


namespace {


VarDesc AMDC_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::String, "VNAME", "NAME"},
  {VarDesc::Int, "LENAMDC", "STRING LENGTH"},
  {VarDesc::Int, "NLINE", "CHAR4 NUMBER"},
  {VarDesc::String, "DATA", "(NLINE)-ASCII"},
};

VarDesc AGDD_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::String, "VNAME", "NAME"},
  {VarDesc::Int, "LENAGDD", "STRING LENGTH"},
  {VarDesc::Int, "NLINE", "CHAR4 NUMBER"},
  {VarDesc::String, "DATA", "(NLINE)-ASCII"},
};

VarDesc ATYP_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JTYP", "AMDB STATION TYPE"},
  {VarDesc::String, "TYP", "AMDB STATION NAME"},
  //DRING: What is it?
  {VarDesc::Int, "NSTA", "NUMBER OF STATIONS OF THIS TYPE"},
};


VarDesc ACUT_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "I", "INDEX"},
  {VarDesc::Int, "ICUT", "CUT-OUT INDEX"},
  {VarDesc::Int, "N", "NUMBER OF SUB-CUTS IN THE STATION"},
};


VarDesc WRPC_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  //DRING: What is it?       						        
  {VarDesc::Int, "NVRS", "VERSION OF RPC TECHNOLOGY"},
  //DRING: What is it?       						        
  {VarDesc::Int, "LAYRPC", "LAYERS NUMBER"},
  {VarDesc::FloatCM, "TCKRLA", "THICK. OF AN RPC LAYER"},
  {VarDesc::FloatCM, "TOTTCK", "TOTAL THICKNESS"},
  {VarDesc::FloatCM, "TCKFSP", "THICK. OF FOAM SPACER"},
  {VarDesc::FloatCM, "ACKFSP", "THICK. OF AL PLATE OF FOAM SPACER"},
  {VarDesc::FloatCM, "TLOHCB", "THICK. OF LOWER HONEYCOMB"},
  {VarDesc::FloatCM, "ALOHCB", "THICK. OF AL PLATE OF LOWER HONEYCOMB"},
  {VarDesc::FloatCM, "TCKBAK", "THICK. OF BAKELITE"},
  {VarDesc::FloatCM, "TCKGAS", "THICK. OF GAS GAP "},
  {VarDesc::FloatCM, "TCKSSU", "THICK. OF STRIPS SUPPORT"},
  {VarDesc::FloatCM, "TCKSTR", "THICK. OF STRIPS"},
  {VarDesc::FloatCM, "SDEDMI", "S INTERNAL MID-CHBER DEAD REGION"},
  {VarDesc::FloatCM, "ZDEDMI", "Z INTERNAL MID-CHBER DEAD REGION"},
  {VarDesc::FloatCM, "SPDIAM", "SPACER DIAMETER"},
  {VarDesc::FloatCM, "SPPITC", "SPACER PITCH"},
  {VarDesc::FloatMM, "STROFF_0", "STRIP OFFSET S, FIRST Z, SECOND Z"},
  {VarDesc::FloatMM, "STROFF_1", ""},
  {VarDesc::FloatMM, "STROFF_2", ""},
};


VarDesc AWLN_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "JSTA TYPE NUMBER"},
  {VarDesc::FloatCM, "SPITCH", "S-STRIPS PITCH"},
  {VarDesc::FloatCM, "ZPITCH", "Z-STRIPS PITCH"},
  {VarDesc::FloatCM, "DEDSTR", "DEAD REAGION BETWEEN STRIP"},
  {VarDesc::Int, "NSREST", "NBER OF S STRIPS READOUTS"},
  {VarDesc::Int, "NZREST", "NBER OF S GAS GAPS"},
  {VarDesc::FloatMM, "SFIRST", "S-PHI STRIP OFFSET"},
  {VarDesc::FloatMM, "ZFIRST", "Z-ETA STRIP OFFSET"},
  {VarDesc::FloatMM, "DEDSEP", "DEAD SEPARATION"},
  {VarDesc::Int, "NSROST", "NUMBER OF S-PHI READOUT STRIPS"},
  {VarDesc::Int, "NZROST", "NUMBER OF Z-ETA READOUT STRIPS"},
};


VarDesc WTGC_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "JSTA INDEX"},
  {VarDesc::Int, "NBEVOL", "NUMBER OF DETAILS"},
  {VarDesc::FloatMM, "X0", "X0"},
  {VarDesc::FloatCM, "WIDCHB", "WIDTH OF THE CHBER ALONG Z"},
  {VarDesc::FloatCM, "FWIRCH", "FRAME WIDTH IN R"},
  {VarDesc::FloatCM, "FWIXCH", "FRAME WIDTH IN X"},
  {VarDesc::String, "ALLNAME_0", "MATERIAL"},
  {VarDesc::String, "ALLNAME_1", ""},
  {VarDesc::String, "ALLNAME_2", ""},
  {VarDesc::String, "ALLNAME_3", ""},
  {VarDesc::String, "ALLNAME_4", ""},
  {VarDesc::String, "ALLNAME_5", ""},
  {VarDesc::String, "ALLNAME_6", ""},
  {VarDesc::String, "ALLNAME_7", ""},
  {VarDesc::String, "ALLNAME_8", ""},
};


VarDesc GGLN_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "JSTA INDEX"},
  {VarDesc::Int, "NWGS_0", "N OF GROUPS OF WIRES (GANG) PER CH. IN E"},
  {VarDesc::Int, "NWGS_1", ""},
  {VarDesc::Int, "NWGS_2", ""},
  {VarDesc::Int, "NSPS_0", "N OF STRIPS PER CHAMBER"},
  {VarDesc::Int, "NSPS_1", ""},
  {VarDesc::Int, "NSPS_2", ""},
  {VarDesc::Int, "ROFFST_0", "OFFSET OF WIRE GROUP ADDRESS 1"},
  {VarDesc::Int, "ROFFST_1", ""},
  {VarDesc::Int, "ROFFST_2", ""},
  //DRING: What is it?  From SB reserved but never used   
  {VarDesc::FloatMM, "POFFST_0", ""},
  //DRING: What is it?  From SB reserved but never used   
  {VarDesc::FloatMM, "POFFST_1", ""},
  //DRING: What is it?  From SB reserved but never used
  {VarDesc::FloatMM, "POFFST_2", ""},
  {VarDesc::FloatMM, "WIRESP", "WIRE SPACING [MM]"},
  {VarDesc::FloatMM, "STRIPSP", "STRIP SPACING [MM]"},
  {VarDesc::FloatMM, "PDIST", "! PHYSICAL DISTANCE OF STRIPS W.R.T. BAS"},
  {VarDesc::FloatMM, "S1PP", "WIRE SUPPORT : WIDTH"},
  {VarDesc::FloatMM, "S2PP", "WIRE SUPPORT : WIDTH OF GAS CHANNEL"},
  {VarDesc::FloatMM, "WSEP", "WIRE SUPPORT : SEPARATION"},
  {VarDesc::FloatMM, "SP1WI", "WIRE SUPPORT : OFFSET OF WIRE SUPPORT LA"},
  {VarDesc::FloatMM, "SP2WI", "WIRE SUPPORT : LAYER 2"},
  {VarDesc::FloatMM, "SP3WI", "WIRE SUPPORT : LAYER 3"},
  {VarDesc::FloatMM, "TILT", "WIRE SUPPORT : TILT ANGLE (DEG)"},
  {VarDesc::FloatMM, "SP1BU", "BUTTON SUPPORT : RADIUS"},
  {VarDesc::FloatMM, "SP2BU", "BUTTON SUPPORT :SEPARATION"},
  {VarDesc::FloatMM, "SP3BU", "BUTTON SUPPORT : PITCH"},
  {VarDesc::FloatMM, "SP4BU", "BUTTON SUPPORT : ANGLE IN TRAPEZOID REGI"},
};


VarDesc ATLN_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "I", "INDEX"},
  {VarDesc::Int, "ICOVOL", "MATERIAL CODE"},
  {VarDesc::FloatCM, "ZPOVOL", "RELATIVE Z POSITION"},
  {VarDesc::FloatCM, "WIDVOL", "Z WIDTH"},
  {VarDesc::String, "NAMVOL", "MATERIAL NAME"},
  {VarDesc::Int, "JSTA", "JSTA INDEX "},
};


VarDesc WCSC_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "JSTA TYPE NUMBER"},
  {VarDesc::Int, "LAYCSC", "NBER OF CSCS LAYERS"},
  {VarDesc::FloatCM, "TTOTAL", "TOTAL THICKNESS"},
  {VarDesc::FloatCM, "TNOMEX", "NOMEX HONEYCOMB THICKNESS"},
  {VarDesc::FloatCM, "TLAG10", "G10 LAMINATES THICKNESS"},
  {VarDesc::FloatCM, "WISPA", "WIRE SPACING"},
  {VarDesc::FloatCM, "DANCAT", "ANODE-CATHODE DISTANCE"},
  {VarDesc::FloatCM, "PCATRE", "CATHODE READOUT PITCH"},
  {VarDesc::FloatCM, "AZCAT", "CATHODE READOUT PITCH, AZIMUTAL"},
  {VarDesc::FloatCM, "GSTRIP", "GAP BETWEEN CATHODE STRIPS"},
  {VarDesc::FloatCM, "WRESTR", "WIDTH OF READOUT STRIPS"},
  {VarDesc::FloatCM, "WFLSTR", "WIDTH OF FLOATING STRIPS"},
  {VarDesc::FloatCM, "TRRWAS", "RIGIT RECTANGULAR WASHER THICKNES"},
  {VarDesc::FloatCM, "WROXA", "ROXACELL WIDTH"},
  {VarDesc::FloatCM, "GROXWI", "ROXACELL AND WIRE BAR GAP"},
  {VarDesc::FloatCM, "WGASBA", "FULL GAS GAP BAR WIDTH"},
  {VarDesc::FloatCM, "TGASBA", "FULL GAS GAP BAR THICK."},
  {VarDesc::FloatCM, "WGASCU", "CUTS GAS GAP BAR WIDTH"},
  {VarDesc::FloatCM, "TGASCU", "CUTS GAS GAP BAR THICK."},
  {VarDesc::FloatCM, "WFIXWI", "FULL WIRE FIX. BAR WID."},
  {VarDesc::FloatCM, "TFIXWI", "FULL WIRE FIX. BAR THICK."},
  {VarDesc::FloatCM, "PBA1WI", "WIRE BAR POSITION"},
  {VarDesc::FloatCM, "PBA2WI", "WIRE BAR POSITION"},
  {VarDesc::FloatCM, "PBA3WI", "WIRE BAR POSITION"},
  //DRING: What is it?  From SB reserved but never used
  {VarDesc::FloatMM, "PSNDCO", "2ND COORDINATE PITCH"},
};


VarDesc DBAM_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "NVRS", "VERSION OF READING"},
  {VarDesc::String, "AMDB", "AMDB NAME"},
  {VarDesc::String, "TEST", "TEST NAME"},
  {VarDesc::Int, "MTYP", "MAXIMUM STATION NUMBER"},
  //DRING: I do not know what is the meaning of this variable
  {VarDesc::Int, "NUMBOX", "FILE INDEX"},
};


VarDesc ASMP_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "INDX", "STATION NUMBER (INSIDE TYPE)"},
  {VarDesc::Int, "N", "NUMBER OF ELEMENTS"},
  {VarDesc::Int, "JTYP", "AMDB STATION TYPE"},
};


VarDesc WDED_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "INDEX"},
  {VarDesc::Int, "NB", "NUMBER OF DETAILS"},
  {VarDesc::FloatMM, "X0", "X0"},
  {VarDesc::FloatCM, "AUPHCB", "HONEYCOMB THICKNESS"},
  {VarDesc::FloatCM, "TCKDED", "ALUMINIUM THICKNESS"},
};


VarDesc WLBI_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "INDEX"},
  {VarDesc::Int, "NUM", "NUMBER OF OBJECTS"},
  {VarDesc::FloatCM, "HEIGHT", "HEIGHT"},
  {VarDesc::FloatCM, "THICKNESS", "WALL THICKNESS"},
  {VarDesc::FloatCM, "LOWERTHICK", "LOWER THICK"},
  {VarDesc::FloatCM, "SHIFTYSTATION", "SHIFTY STATION"},
};


VarDesc WCRO_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "INDEX"},
  {VarDesc::Int, "NUM", "NUMBER OF OBJECTS"},
  {VarDesc::FloatCM, "HEIGHTNESS", "HEIGHT"},
  {VarDesc::FloatCM, "LARGENESS", "T-SHAPE LARGENESS"},
  {VarDesc::FloatCM, "THICKNESS", "T-SHAPE THICKNESS"},
};


VarDesc WCMI_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "INDEX"},
  {VarDesc::Int, "NUM", "NUMBER OF OBJECTS"},
  {VarDesc::FloatCM, "HEIGHTNESS", "HEIGHT"},
  {VarDesc::FloatCM, "LARGENESS", "T-SHAPE LARGENESS"},
  {VarDesc::FloatCM, "THICKNESS", "T-SHAPE THICKNESS"},
};


VarDesc WCHV_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "INDEX"},
  {VarDesc::Int, "NUM", "NUMBER OF OBJECTS"},
  {VarDesc::FloatCM, "HEIGHTNESS", "HEIGHT"},
  {VarDesc::FloatCM, "LARGENESS", "T-SHAPE LARGENESS"},
  {VarDesc::FloatCM, "THICKNESS", "T-SHAPE THICKNESS"},
};


VarDesc WSUP_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "INDEX"},
  {VarDesc::Int, "NXXSUP", "MAX NB. FOR X FRAGMENTS"},
  {VarDesc::Int, "NZZSUP", "MAX NB. FOR Z FRAGMENTS"},
  {VarDesc::FloatMM, "X0", "X0"},
  {VarDesc::FloatMM, "THICKN", "THICKNESS"},
  {VarDesc::FloatCM, "XXSUP_0", "X DIMENSION"},
  {VarDesc::FloatCM, "XXSUP_1", ""},
  {VarDesc::FloatCM, "XXSUP_2", ""},
  {VarDesc::FloatCM, "XXSUP_3", ""},
  {VarDesc::FloatCM, "ZZSUP_0", "Z DIMENSION"},
  {VarDesc::FloatCM, "ZZSUP_1", ""},
  {VarDesc::FloatCM, "ZZSUP_2", ""},
  {VarDesc::FloatCM, "ZZSUP_3", ""},
};


VarDesc WSPA_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "JSTA", "JSTA INDEX"},
  {VarDesc::Int, "NB", "NUMBER OF DETAILS"},
  {VarDesc::FloatMM, "X0", "X0"},
  {VarDesc::FloatCM, "TCKSPA", "THICKNESS OF SPACER"},
};


VarDesc WMDT_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::String, "TYP", "NAME"},
  {VarDesc::Int, "IW", "INDEX"},
  {VarDesc::FloatMM, "X0", "X0"},
  {VarDesc::Int, "LAYMDT", "MAXIMUM LAYER NUMBER"},
  {VarDesc::FloatCM, "TUBPIT", "PITCH BETWEEN TUBE"},
  {VarDesc::FloatCM, "TUBRAD", "RADIUS OF TUBE"},
  {VarDesc::FloatCM, "TUBSTA", "THICKNESS OF TUBE"},
  {VarDesc::FloatCM, "TUBDEA", "DEAD LENGTH IN TUBES"},
  {VarDesc::FloatCM, "TUBXCO_0", "Y TUBE POSITION"},
  {VarDesc::FloatCM, "TUBXCO_1", ""},
  {VarDesc::FloatCM, "TUBXCO_2", ""},
  {VarDesc::FloatCM, "TUBXCO_3", ""},
  {VarDesc::FloatCM, "TUBYCO_0", "X TUBE POSITION"},
  {VarDesc::FloatCM, "TUBYCO_1", ""},
  {VarDesc::FloatCM, "TUBYCO_2", ""},
  {VarDesc::FloatCM, "TUBYCO_3", ""},
  {VarDesc::FloatCM, "TUBWAL", "TUBE WALL THICKNESS"},
};


VarDesc ALIN_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::FloatCM, "DX", "X RELATIVE POSITION OF THE SUB-CUT"},
  {VarDesc::FloatCM, "DY", "Y RELATIVE POSITION OF THE SUB-CUT"},
  {VarDesc::Int, "I", "SERIAL NB. OF THE OBJECT IN WHICH THE SU"},
  {VarDesc::FloatCM, "WIDTH_XS", "S DIMENSIONS OF THE SUB-CUT"},
  {VarDesc::FloatCM, "WIDTH_XL", "L DIMENSIONS OF THE SUB-CUT"},
  {VarDesc::FloatCM, "LENGTH_Y", "Y DIMENSIONS OF THE SUB-CUT"},
  {VarDesc::FloatCM, "EXCENT", "EXC DIMENSIONS OF THE SUB-CUT"},
  {VarDesc::FloatMM, "DEAD1", "D1 DIMENSIONS OF THE SUB-CUT"},
  {VarDesc::Int, "JTYP", "STATION TYPE"},
  {VarDesc::Int, "INDX", "INDEX"},
  {VarDesc::Int, "ICUT", "CUT-OUT INDEX"},
};


VarDesc ALMN_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::Int, "I", "ELEMENT NUMBER"},
  {VarDesc::FloatCM, "DX", "X RELATIVE POSITION OF THE OBJECT"},
  {VarDesc::FloatCM, "DY", "Y RELATIVE POSITION OF THE OBJECT"},
  {VarDesc::FloatCM, "DZ", "Z RELATIVE POSITION OF THE OBJECT"},
  {VarDesc::Int, "JOB", "OBJECT SERIAL NUMBER IN THE STATION"},
  {VarDesc::String, "TEC", "TYPE OF TECHNOLOGY USED FOR THE OBJECT"},
  {VarDesc::Int, "IW", "INNER STRUCTURE TYPE INDEX, ISTA"},
  {VarDesc::Int, "ISPLIT_X", "NUMBER OF SECTIONS IN X"},
  {VarDesc::Int, "ISPLIT_Y", "NUMBER OF SECTIONS IN Y"},
  {VarDesc::Int, "ISHAPE", "TYPE OF GEOMETRICAL SHAPE 0-TRAPEZOIDAL,"},
  {VarDesc::FloatCM, "WIDTH_XS", "S WIDTH, WS"},
  {VarDesc::FloatCM, "WIDTH_XL", "L WIDTH, WL"},
  {VarDesc::FloatCM, "LENGTH_Y", "Y LENGTH, LE"},
  {VarDesc::FloatCM, "EXCENT", "ADDITIONAL INFORMATION, EX"},
  {VarDesc::FloatCM, "DEAD1", "FIRST DEAD MATERIAL, D1"},
  {VarDesc::FloatCM, "DEAD2", "SECOND DEAD MATERIAL, D2"},
  {VarDesc::FloatCM, "DEAD3", "STEPS, D3"},
  {VarDesc::Int, "JTYP", "STATION TYPE"},
  {VarDesc::Int, "INDX", "ELEMENT NUMBER"},
};


VarDesc APTP_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  //DRING: What is it?
  {VarDesc::Int, "LINE", "LINE NUMBER"},
  {VarDesc::String, "TYP", "STATION TYPE"},
  {VarDesc::Int, "I", "STATION AMDB INDEX"},
  {VarDesc::Int, "ICUT", "CUT-OUT INDEX,ZERO IF MISSING"},
  {VarDesc::Int, "IZ", "Z (FOR BARREL) OR R (FOR END-CAPS) POS."},
  {VarDesc::Int, "IPHI_0", "PHI INDICATES OF OCTANTS"},
  {VarDesc::Int, "IPHI_1", ""},
  {VarDesc::Int, "IPHI_2", ""},
  {VarDesc::Int, "IPHI_3", ""},
  {VarDesc::Int, "IPHI_4", ""},
  {VarDesc::Int, "IPHI_5", ""},
  {VarDesc::Int, "IPHI_6", ""},
  {VarDesc::Int, "IPHI_7", ""},
  {VarDesc::FloatCM, "Z", "Z POSITION OF THE LOWEST Z EDGE OF THE S"},
  {VarDesc::FloatCM, "R", "RADIAL POSITION OF ITS INNERMOST EDGE"},
  {VarDesc::FloatCM, "S", "ORTHO-RADIAL POSITION OF THE CENTER OF T"},
  {VarDesc::FloatAng, "DPHI", "RELATIVE PHI POSITION OF THE STATION IN"},
  {VarDesc::FloatAng, "ALFA", "ALFA ANGLE DEFINING THE DEVIATION [GRAD]"},
  {VarDesc::FloatAng, "BETA", "BETA ANGLE DEFINING THE DEVIATION"},
  {VarDesc::FloatAng, "GAMMA", "GAMMA ANGLE DEFINING THE DEVIATION"},
};


VarDesc ASZT_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::String, "TYP", "STATION TYPE"},
  {VarDesc::Int, "JFF", "PHI POSITION"},
  {VarDesc::Int, "JZZ", "Z POSITION"},
  {VarDesc::Int, "JOB", "JOB POSITION"},
  {VarDesc::FloatCM, "TRAS", "S TRANSLATION [MM]"},
  {VarDesc::FloatCM, "TRAZ", "Z TRANSLATION"},
  {VarDesc::FloatCM, "TRAT", "T TRANSLATION"},
  {VarDesc::FloatAng, "ROTS", "S ROTATION [RAD]"},
  {VarDesc::FloatAng, "ROTZ", "Z ROTATION"},
  {VarDesc::FloatAng, "ROTT", "T ROTATION"},
};


VarDesc ISZT_Vars[] = {
  {VarDesc::Int, "VERS", "VERSION"},
  {VarDesc::String, "TYP", "STATION TYPE"},
  {VarDesc::Int, "JFF", "PHI POSITION"},
  {VarDesc::Int, "JZZ", "Z POSITION"},
  {VarDesc::Int, "JOB", "JOB POSITION"},
  {VarDesc::Int, "JLAY", "JLAY POSITION"},
  {VarDesc::FloatCM, "TRAS", "S TRANSLATION [MM]"},
  {VarDesc::FloatCM, "TRAZ", "Z TRANSLATION"},
  {VarDesc::FloatCM, "TRAT", "T TRANSLATION"},
  {VarDesc::FloatAng, "ROTS", "S ROTATION [RAD]"},
  {VarDesc::FloatAng, "ROTZ", "Z ROTATION"},
  {VarDesc::FloatAng, "ROTT", "T ROTATION"},
};


VarDesc HwSwId_Vars[] = {
  {VarDesc::String, "HARDNAME", "Hard name"},
  {VarDesc::String, "SOFTNAME", "Soft name"},
  {VarDesc::Int, "SOFTOCTANT", "Soft octant"},
  {VarDesc::Int, "SOFTIZ", "Soft Iz"},
};


VarDesc Xtomo_Vars[] = {
  {VarDesc::String, "XtomoChberName", "XtomoChberName"},
  {VarDesc::String, "XtomoSite", "XtomoSite     "},
  {VarDesc::Int, "XtomoSiteId", "XtomoSiteId   "},
  {VarDesc::Int, "XtomoTime", "XtomoTime     "},
  {VarDesc::Int, "XtomoPassed", "XtomoPassed   "},
  {VarDesc::String, "XtomoSide", "XtomoSide     "},
  {VarDesc::Int, "XtomoNberTube1", "XtomoNberTube1"},
  {VarDesc::Int, "XtomoNberTube2", "XtomoNberTube2"},
  {VarDesc::Int, "XtomoNberML", "XtomoNberML   "},
  {VarDesc::Int, "XtomoNberLayer", "XtomoNberLayer"},
  {VarDesc::FloatMM, "XtomoML1NYtub", "XtomoML1NYtub "},
  {VarDesc::FloatMM, "XtomoML1NZtub", "XtomoML1NZtub "},
  {VarDesc::FloatMM, "XtomoML1NDely", "XtomoML1NDely "},
  {VarDesc::FloatMM, "XtomoML1NDelz", "XtomoML1NDelz "},
  {VarDesc::FloatMM, "XtomoML1NDela", "XtomoML1NDela "},
  {VarDesc::FloatMM, "XtomoML1NYpit", "XtomoML1NYpit "},
  {VarDesc::FloatMM, "XtomoML1NZpit", "XtomoML1NZpit "},
  {VarDesc::FloatMM, "XtomoML1PYtub", "XtomoML1PYtub "},
  {VarDesc::FloatMM, "XtomoML1PZtub", "XtomoML1PZtub "},
  {VarDesc::FloatMM, "XtomoML1PDely", "XtomoML1PDely "},
  {VarDesc::FloatMM, "XtomoML1PDelz", "XtomoML1PDelz "},
  {VarDesc::FloatMM, "XtomoML1PDela", "XtomoML1PDela "},
  {VarDesc::FloatMM, "XtomoML1PYpit", "XtomoML1PYpit "},
  {VarDesc::FloatMM, "XtomoML1PZpit", "XtomoML1PZpit "},
  {VarDesc::FloatMM, "XtomoML2NYtub", "XtomoML2NYtub "},
  {VarDesc::FloatMM, "XtomoML2NZtub", "XtomoML2NZtub "},
  {VarDesc::FloatMM, "XtomoML2NDely", "XtomoML2NDely "},
  {VarDesc::FloatMM, "XtomoML2NDelz", "XtomoML2NDelz "},
  {VarDesc::FloatMM, "XtomoML2NDela", "XtomoML2NDela "},
  {VarDesc::FloatMM, "XtomoML2NYpit", "XtomoML2NYpit "},
  {VarDesc::FloatMM, "XtomoML2NZpit", "XtomoML2NZpit "},
  {VarDesc::FloatMM, "XtomoML2PYtub", "XtomoML2PYtub "},
  {VarDesc::FloatMM, "XtomoML2PZtub", "XtomoML2PZtub "},
  {VarDesc::FloatMM, "XtomoML2PDely", "XtomoML2PDely "},
  {VarDesc::FloatMM, "XtomoML2PDelz", "XtomoML2PDelz "},
  {VarDesc::FloatMM, "XtomoML2PDela", "XtomoML2PDela "},
  {VarDesc::FloatMM, "XtomoML2PYpit", "XtomoML2PYpit "},
  {VarDesc::FloatMM, "XtomoML2PZpit", "XtomoML2PZpit "},
};


} // anonymous namespace


#define RANGE(a) std::begin(a), std::end(a)


AmdcDbSvcMakerFromRDB::AmdcDbSvcMakerFromRDB(){

 m_EpsLengthMM = 4 ;
 m_EpsLengthCM = 5 ;
 m_EpsAngle    = 6 ;

 m_Map_Set_EpsLengthMM["Dummy"] = 0 ; m_Map_Set_EpsLengthMM.clear() ;
 m_Map_Set_EpsLengthCM["Dummy"] = 0 ; m_Map_Set_EpsLengthCM.clear() ;
 m_Map_Set_EpsAngle   ["Dummy"] = 0 ; m_Map_Set_EpsAngle.clear()    ;
  
}

// Set a AmdcDbSvc
void AmdcDbSvcMakerFromRDB::SetEpsLengthMM(int EpsLengthMM){ m_EpsLengthMM  = EpsLengthMM ;}
void AmdcDbSvcMakerFromRDB::SetEpsLengthCM(int EpsLengthCM){ m_EpsLengthCM  = EpsLengthCM ;}
void AmdcDbSvcMakerFromRDB::SetEpsAngle(int EpsAngle){ m_EpsAngle  = EpsAngle ;}
void AmdcDbSvcMakerFromRDB::SetEpsLengthMM(std::string NameOfTheSet, int EpsLengthMM) { m_Map_Set_EpsLengthMM[NameOfTheSet] = EpsLengthMM ; }
void AmdcDbSvcMakerFromRDB::SetEpsLengthCM(std::string NameOfTheSet, int EpsLengthCM) { m_Map_Set_EpsLengthCM[NameOfTheSet] = EpsLengthCM ; }
void AmdcDbSvcMakerFromRDB::SetEpsAngle   (std::string NameOfTheSet, int EpsAngle   ) { m_Map_Set_EpsAngle   [NameOfTheSet] = EpsAngle    ; }
void AmdcDbSvcMakerFromRDB::Set(
            std::string detectorKey  ,
            std::string detectorNode ,
            IRDBAccessSvc* pIRDBAccessSvc,
	    AmdcDbSvc* pAmdcDbSvc
){ 

  m_detectorKey  = detectorKey  ;
  m_detectorNode = detectorNode ;
  
  AMDC(pIRDBAccessSvc,pAmdcDbSvc);
  AGDD(pIRDBAccessSvc,pAmdcDbSvc);
  ATYP(pIRDBAccessSvc,pAmdcDbSvc);
  ACUT(pIRDBAccessSvc,pAmdcDbSvc);
  WRPC(pIRDBAccessSvc,pAmdcDbSvc);
  AWLN(pIRDBAccessSvc,pAmdcDbSvc);
  WTGC(pIRDBAccessSvc,pAmdcDbSvc);
  GGLN(pIRDBAccessSvc,pAmdcDbSvc);
  ATLN(pIRDBAccessSvc,pAmdcDbSvc);
  WCSC(pIRDBAccessSvc,pAmdcDbSvc);
  DBAM(pIRDBAccessSvc,pAmdcDbSvc);
  ASMP(pIRDBAccessSvc,pAmdcDbSvc);
  WDED(pIRDBAccessSvc,pAmdcDbSvc);
  WLBI(pIRDBAccessSvc,pAmdcDbSvc);
  WCRO(pIRDBAccessSvc,pAmdcDbSvc);
  WCMI(pIRDBAccessSvc,pAmdcDbSvc);
  WCHV(pIRDBAccessSvc,pAmdcDbSvc);
  WSUP(pIRDBAccessSvc,pAmdcDbSvc);
  WSPA(pIRDBAccessSvc,pAmdcDbSvc);
  WMDT(pIRDBAccessSvc,pAmdcDbSvc);
  ALIN(pIRDBAccessSvc,pAmdcDbSvc);
  ALMN(pIRDBAccessSvc,pAmdcDbSvc);
  APTP(pIRDBAccessSvc,pAmdcDbSvc);
  ASZT(pIRDBAccessSvc,pAmdcDbSvc);
  ISZT(pIRDBAccessSvc,pAmdcDbSvc);

  HwSwIdMapping(pIRDBAccessSvc,pAmdcDbSvc);

  XtomoData(pIRDBAccessSvc,pAmdcDbSvc);

}

int AmdcDbSvcMakerFromRDB::GetEpsLengthMM(std::string NameOfTheSet){ if (m_Map_Set_EpsLengthMM.find(NameOfTheSet) != m_Map_Set_EpsLengthMM.end()) return m_Map_Set_EpsLengthMM.find(NameOfTheSet)->second ; return m_EpsLengthMM; }
int AmdcDbSvcMakerFromRDB::GetEpsLengthCM(std::string NameOfTheSet){ if (m_Map_Set_EpsLengthCM.find(NameOfTheSet) != m_Map_Set_EpsLengthCM.end()) return m_Map_Set_EpsLengthCM.find(NameOfTheSet)->second ; return m_EpsLengthCM; }
int AmdcDbSvcMakerFromRDB::GetEpsAngle   (std::string NameOfTheSet){ if (m_Map_Set_EpsAngle.find(NameOfTheSet)    != m_Map_Set_EpsAngle.end()   ) return m_Map_Set_EpsAngle.find(NameOfTheSet)->second    ; return m_EpsAngle   ; }


void AmdcDbSvcMakerFromRDB::AMDC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("AMDC", RANGE(AMDC_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}

void AmdcDbSvcMakerFromRDB::AGDD(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("AGDD", RANGE(AGDD_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}

void AmdcDbSvcMakerFromRDB::ATYP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ATYP", RANGE(ATYP_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::ACUT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ACUT", RANGE(ACUT_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WRPC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WRPC", RANGE(WRPC_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::AWLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("AWLN", RANGE(AWLN_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WTGC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WTGC", RANGE(WTGC_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::GGLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "GGLN";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  IRDBRecordset_ptr pIRDBRecordset = pIRDBAccessSvc->getRecordsetPtr(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();
  std::string StrDbId = NameOfTheSet+"_DATA_ID" ;

  for (IRDBRecord* rec : *pIRDBRecordset) {
    long DbId	  = rec->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    for (const VarDesc& d : GGLN_Vars) {
      addVar (d, rec, pAmdcDbRecord, 0, LocalEpsLengthMM);
    }
  
    for (int DB_IWGS=1; DB_IWGS<=3 ; DB_IWGS++){
      for (int DB_Item=1; DB_Item<=130 ; DB_Item++){
        std::string DbVar = "IWGS" ;
    	std::ostringstream Aostringstream_F1;
    	Aostringstream_F1 << DB_IWGS ;
    	DbVar = DbVar+Aostringstream_F1.str();
    	DbVar = DbVar+"_";
    	std::ostringstream Aostringstream_F2;
    	int DB_Item_Shifted = DB_Item - 1 ;
    	Aostringstream_F2 << DB_Item_Shifted ;
        DbVar = DbVar+Aostringstream_F2.str();
    	int iDbVal = rec->getInt(DbVar);
        std::string DbVarComment="";
        if ( DB_IWGS==1 && DB_Item==1 ) DbVarComment="IWGS1 WIRE GANGING";
        if ( DB_IWGS==2 && DB_Item==1 ) DbVarComment="IWGS2 WIRE GANGING";
        if ( DB_IWGS==3 && DB_Item==1 ) DbVarComment="IWGS3 WIRE GANGING";

    	pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      }
    }

  
    for (int DB_Item=1; DB_Item<=33 ; DB_Item++){
      std::string DbVar = "SLARGE" ;
      DbVar = DbVar+"_";
      std::ostringstream Aostringstream;
      int DB_Item_Shifted = DB_Item - 1 ;
      Aostringstream << DB_Item_Shifted ;
      DbVar = DbVar+Aostringstream.str();
      double dDbVal = rec->getFloat(DbVar);
      std::string DbVarComment="";
      if (DB_Item==1) DbVarComment="STRIPS LAYOUT";

      pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    }
  
    for (int DB_Item=1; DB_Item<=33 ; DB_Item++){
      std::string DbVar = "SHORT" ;
      DbVar = DbVar+"_";
      std::ostringstream Aostringstream;
      int DB_Item_Shifted = DB_Item - 1 ;
      Aostringstream << DB_Item_Shifted ;
      DbVar = DbVar+Aostringstream.str();
      double dDbVal = rec->getFloat(DbVar);
      std::string DbVarComment="";
      if (DB_Item==1) DbVarComment="START OF STRIP ON LARGE BASE 1ST PLANE";

      pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    }

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);
}


void AmdcDbSvcMakerFromRDB::ATLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ATLN", RANGE(ATLN_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WCSC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WCSC", RANGE(WCSC_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::DBAM(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "DBAM";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  IRDBRecordset_ptr pIRDBRecordset = pIRDBAccessSvc->getRecordsetPtr(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();
  std::string StrDbId = NameOfTheSet+"_DATA_ID" ;

  for (IRDBRecord* rec : *pIRDBRecordset) {
    long DbId	  = rec->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);
  
    for (const VarDesc& d : DBAM_Vars) {
      addVar (d, rec, pAmdcDbRecord, 0);
    }

    int DB_MTYP = rec->getInt("MTYP")  ;
    for (int DB_Item=1; DB_Item<=DB_MTYP ; DB_Item++){
      std::string DbVar = "NAME" ;
      DbVar = DbVar+"_";
      std::ostringstream Aostringstream;
      int DB_Item_Shifted = DB_Item - 1 ;
      Aostringstream << DB_Item_Shifted ;
      DbVar = DbVar+Aostringstream.str();
      std::string sDbVal = rec->getString(DbVar);
      std::string DbVarComment="" ;
      if (DB_Item==1) DbVarComment="STATION NAME" ;

      pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
    }
  
    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);
}


void AmdcDbSvcMakerFromRDB::ASMP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ASMP", RANGE(ASMP_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WDED(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WDED", RANGE(WDED_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WLBI(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WLBI", RANGE(WLBI_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WCRO(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WCRO", RANGE(WCRO_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WCMI(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WCMI", RANGE(WCMI_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WCHV(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WCHV", RANGE(WCHV_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WSUP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WSUP", RANGE(WSUP_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WSPA(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WSPA", RANGE(WSPA_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::WMDT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("WMDT", RANGE(WMDT_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::ALIN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ALIN", RANGE(ALIN_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::ALMN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ALMN", RANGE(ALMN_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::APTP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("APTP", RANGE(APTP_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::ASZT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ASZT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  IRDBRecordset_ptr pIRDBRecordset = pIRDBAccessSvc->getRecordsetPtr(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();
  std::string StrDbId = NameOfTheSet+"_DATA_ID" ;

  for (IRDBRecord* rec : *pIRDBRecordset) {
    long DbId	  = rec->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);


//DRING: What is it?	
//  DbVar = "LINE"  ; DbVarComment="LINE NUMBER"	; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    pAmdcDbRecord->addInt("LINE", "LINE NUMBER", 0);

    for (const VarDesc& d : ASZT_Vars) {
      addVar (d, rec, pAmdcDbRecord,
              LocalEpsLengthCM, 0, LocalEpsAngle);
    }

//DRING: What is it? From SB error	
//  DbVar = "I"     ; DbVarComment="STATION AMDB INDEX" ; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    pAmdcDbRecord->addInt("I", "STATION AMDB INDEX", 0);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);
}


void AmdcDbSvcMakerFromRDB::ISZT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ISZT", RANGE(ISZT_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::HwSwIdMapping(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("HwSwIdMapping", RANGE(HwSwId_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::XtomoData(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("XtomoData", RANGE(Xtomo_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::addVars (const std::string& NameOfTheSet,
                                     const VarDesc* beg,
                                     const VarDesc* end,
                                     IRDBAccessSvc* pIRDBAccessSvc,
                                     AmdcDbSvc* pAmdcDbSvc)
{
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet);
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet);
  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet);

  IRDBRecordset_ptr pIRDBRecordset =
    pIRDBAccessSvc->getRecordsetPtr (NameOfTheSet,
                                     m_detectorKey,
                                     m_detectorNode);

  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::string NameOfTheSetKAPITAL = NameOfTheSet ;
  std::transform(NameOfTheSet.begin(), NameOfTheSet.end(),
                 NameOfTheSetKAPITAL.begin(), (int(*)(int)) toupper);
  std::string StrDbId = NameOfTheSetKAPITAL+"_DATA_ID" ;

  for (IRDBRecord* rec : *pIRDBRecordset) {
    long DbId	  = rec->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    for (; beg < end; ++beg) {
      addVar (*beg, rec, pAmdcDbRecord,
              LocalEpsLengthCM, LocalEpsLengthMM, LocalEpsAngle);
    }

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);
}


