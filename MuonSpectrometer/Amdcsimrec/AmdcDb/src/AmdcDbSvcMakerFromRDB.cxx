/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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


VarDesc ABRT_Vars[] = {
  {VarDesc::FloatCM, "CRYORMIN", "CRYORMIN                    "},
  {VarDesc::FloatCM, "CRYORMAX", "CRYORMAX                    "},
  {VarDesc::FloatCM, "CRYOZMAX", "CRYOZMAX                    "},
  {VarDesc::FloatCM, "CRYORCUR", "CRYORCUR                    "},
  {VarDesc::FloatCM, "CRYORADI", "RADIUS                      "},
  {VarDesc::FloatCM, "CRYORADT", "RADTHICK                    "},
  {VarDesc::FloatCM, "CRYATTD0", "CRYOATTDIAM0                "},
  {VarDesc::FloatCM, "CRYATTD1", "CRYOATTDIAM1                "},
  {VarDesc::FloatCM, "CRYATTAL", "CRYOATTALFA                 "},
  {VarDesc::FloatCM, "CRYATTXH", "CRYOATTXHIGH                "},
  {VarDesc::FloatCM, "CRYATTTH", "CRYOATTTHICK                "},
  {VarDesc::FloatCM, "CRYATTRX", "CRYOATTRMAX                 "},
  {VarDesc::FloatCM, "CRYATWIY", "CRYOATTWINGY                "},
  {VarDesc::FloatCM, "CRYATWXP", "CRYOATTWINGXP               "},
  {VarDesc::FloatCM, "CRYATWXN", "CRYOATTWINGXN               "},
  {VarDesc::FloatCM, "CRYATWBO", "CRYOATTWINGBOSS             "},
  {VarDesc::FloatCM, "CRYATWTH", "CRYOATTWINGTHIC             "},
  {VarDesc::FloatCM, "CRYATWZE", "CRYOATTWINGZESP             "},
  {VarDesc::FloatCM, "CRYATWRA", "CRYOATTWINGRAD              "},
  {VarDesc::FloatCM, "CRYATWYC", "CRYOATTWINGYCUT             "},
  {VarDesc::FloatCM, "CRYRIBYW", "CRYORIBYWID                 "},
  {VarDesc::FloatCM, "CRYRIBZL", "CRYORIBZLEN                 "},
  {VarDesc::FloatCM, "CRYRIBTH", "CRYORIBTHICK                "},
  {VarDesc::FloatCM, "CRYRIWYP", "CRYORIBWINGYP               "},
  {VarDesc::FloatCM, "CRYRIWYN", "CRYORIBWINGYN               "},
  {VarDesc::FloatCM, "CRYRIWXP", "CRYORIBWINGXP               "},
  {VarDesc::FloatCM, "CRYRIWXN", "CRYORIBWINGXN               "},
  {VarDesc::FloatCM, "CRYRIWTH", "CRYORIBWINGTHIC             "},
  {VarDesc::FloatCM, "CRYRNGRM", "CRYORINGRMED                "},
  {VarDesc::FloatCM, "CRYRNGZM", "CRYORINGZMED                "},
  {VarDesc::FloatCM, "CRYRNGRA", "CRYORINGRADIUS              "},
  {VarDesc::FloatCM, "STRTRMAX", "STRTRMAX                    "},
  {VarDesc::FloatCM, "STRTRMIN", "STRTRMIN                    "},
  {VarDesc::FloatCM, "STRTYLEN", "STRTYLEN                    "},
  {VarDesc::FloatCM, "STRTZWID", "STRUTZWIDTH                 "},
  {VarDesc::FloatCM, "STRTRTHI", "STRUTRTHICK                 "},
  {VarDesc::FloatCM, "STRTZTHI", "STRUTZTHICK                 "},
  {VarDesc::FloatCM, "STRWRMAX", "STRUTWINGRMAX               "},
  {VarDesc::FloatCM, "STRWYMAX", "STRUTWINGYMAX               "},
  {VarDesc::FloatCM, "STRWRMIN", "STRUTWINGRMIN               "},
  {VarDesc::FloatCM, "STRWYMIN", "STRUTWINGYMIN               "},
  {VarDesc::FloatCM, "STRWZTHI", "STRUTWINGZTHICK             "},
  {VarDesc::FloatCM, "STRWYTHI", "STRUTWINGYTHICK             "},
  {VarDesc::FloatCM, "STRWZLEN", "STRUTWINGZLEN               "},
  {VarDesc::FloatCM, "ZVOUSS_0", "ARRAY                       "},
  {VarDesc::FloatCM, "ZVOUSS_1", "                            "},
  {VarDesc::FloatCM, "ZVOUSS_2", "                            "},
  {VarDesc::FloatCM, "ZVOUSS_3", "                            "},
  {VarDesc::FloatCM, "ZVOUSS_4", "                            "},
  {VarDesc::FloatCM, "ZVOUSS_5", "                            "},
  {VarDesc::FloatCM, "ZVOUSS_6", "                            "},
  {VarDesc::FloatCM, "ZVOUSS_7", "                            "},
  {VarDesc::FloatCM, "ZRIB_0", "ARRAY                       "},
  {VarDesc::FloatCM, "ZRIB_1", "                            "},
  {VarDesc::FloatCM, "ZRIB_2", "                            "},
  {VarDesc::FloatCM, "ZRIB_3", "                            "},
  {VarDesc::FloatCM, "ZRIB_4", "                            "},
  {VarDesc::FloatCM, "ZRIB_5", "                            "},
  {VarDesc::FloatCM, "ZRIB_6", "                            "},
  {VarDesc::FloatCM, "CRYRIXHI", "CRYORIBXHIGH                "},
  {VarDesc::FloatCM, "STRTRMOY", "STRTRMOY                    "},
  {VarDesc::FloatCM, "STRTYLNP", "STRUTYLENP                  "},
  {VarDesc::FloatCM, "CRYATTXS", "CRYOATTXSEC                 "},
  {VarDesc::FloatCM, "COMARMIN", "COLDMASSRMIN                "},
  {VarDesc::FloatCM, "COMARMAX", "COLDMASSRMAX                "},
  {VarDesc::FloatCM, "COMAZMAX", "COLDMASSZMAX                "},
  {VarDesc::FloatCM, "COMARCUI", "COLDMASSRCURVI              "},
  {VarDesc::FloatCM, "COMARTHI", "COLDMASSRTHICK              "},
  {VarDesc::FloatCM, "COMAYTHI", "COLDMASSYTHICK              "},
  {VarDesc::FloatCM, "COMARIBY", "COLDMASSRIBY                "},
  {VarDesc::FloatCM, "COMARIBZ", "COLDMASSRIBZ                "},
  {VarDesc::FloatCM, "COMABOSS", "COLDMASSBOSS                "},
  {VarDesc::FloatCM, "COMARIBX", "COLDMASSRIBX                "},
  {VarDesc::FloatCM, "BIELYHEI", "BIELLYHEIGHT                "},
  {VarDesc::FloatCM, "BIELYSO1", "BIELLYSOLE1                 "},
  {VarDesc::FloatCM, "BIELYSO2", "BIELLYSOLE2                 "},
  {VarDesc::FloatCM, "BIELXSOL", "BIELLXSOLE                  "},
  {VarDesc::FloatCM, "BIELXTH1", "BIELLXTHICK1                "},
  {VarDesc::FloatCM, "BIELXTH2", "BIELLXTHICK2                "},
  {VarDesc::FloatCM, "BIELZTH1", "BIELLZTHICK1                "},
  {VarDesc::FloatCM, "BIELRHOL", "BIELLRADHOLE                "},
  {VarDesc::FloatCM, "BIELYCHO", "BIELLYCENHOLE               "},
  {VarDesc::FloatCM, "BIELATL1", "BIELLATTLENG1               "},
  {VarDesc::FloatCM, "BIELATL2", "BIELLATTLENG2               "},
  {VarDesc::FloatCM, "BIELATTH", "BIELLATTTHICK               "},
  {VarDesc::FloatCM, "BIELATHE", "BIELLATTHEIGHT              "},
  {VarDesc::FloatCM, "VOUSBLXH", "VOUSSBLOCKXHEIGHT           "},
  {VarDesc::FloatCM, "VOUSPLIX", "VOUSSPLATEINNERXWID         "},
  {VarDesc::FloatCM, "VOUSPLOX", "VOUSSPLATEOUTERXWID         "},
  {VarDesc::FloatCM, "VOUSBLYW", "VOUSSBLOCKYWID              "},
  {VarDesc::FloatCM, "VOUBLYWS", "VOUSSBLOCKYWIDSUB           "},
  {VarDesc::FloatCM, "VOURCYWI", "VOUSSRECTCUTYWID            "},
  {VarDesc::FloatCM, "VOURPYWI", "VOUSSREINFORCEPLATYWID      "},
  {VarDesc::FloatCM, "VOUBLZLE", "VOUSSBLOCKZLEN              "},
  {VarDesc::FloatCM, "VOUBLZLS", "VOUSSBLOCKZLENSUB           "},
  {VarDesc::FloatCM, "VOUBZWTH", "VOUSSBLOCKZWALLTH           "},
  {VarDesc::FloatCM, "VOUCUTZE", "VOUSSCUTZEXT                "},
  {VarDesc::FloatCM, "VOURCUTR", "VOUSSROUNDCUTRAD            "},
  {VarDesc::FloatCM, "VOURECSL", "VOUSSRECTCUTXYSLANT         "},
  {VarDesc::FloatCM, "CNBCOYEX", "CONBOXCOREYEXTENSION        "},
  {VarDesc::FloatCM, "CNBCOXEX", "CONBOXCOREXEXTENSION        "},
  {VarDesc::FloatCM, "CNBCOXSU", "CONBOXCOREXSUP              "},
  {VarDesc::FloatCM, "CNBCOXIN", "CONBOXCOREXINF              "},
  {VarDesc::FloatCM, "CNBEAHEI", "CONBOXEARHEIGHT             "},
  {VarDesc::FloatCM, "CNBEACXI", "CONBOXEARCUTXIN             "},
  {VarDesc::FloatCM, "CNBEACTL", "CONBOXEARCUTPLATETHICKLOWOUT"},
  {VarDesc::FloatCM, "CNBEACTU", "CONBOXEARCUTPLATETHICKUP    "},
  {VarDesc::FloatCM, "CNBEACZW", "CONBOXEARCUTZWALL           "},
  {VarDesc::FloatCM, "CNBEACZE", "CONBOXEARCUTZEXTR           "},
  {VarDesc::FloatCM, "CNBEACPL", "CONBOXINNEREARCUTPLATE      "},
  {VarDesc::FloatCM, "CNBCADMA", "CONBOXCAVITYDMAX            "},
  {VarDesc::FloatCM, "CNBCADMI", "CONBOXCAVITYDMIN            "},
  {VarDesc::FloatCM, "CNBCAZEX", "CONBOXCAVITYZEXT            "},
  {VarDesc::FloatCM, "CNBOXZEX", "CONBOXZEXTENSION            "},
  {VarDesc::FloatCM, "VOUSMBRA", "VOUSSOIRMAINBLOCKRADIUS     "},
  {VarDesc::FloatCM, "CNBXMBRA", "CONBOXMAINBLOCKRADIUS       "},
  {VarDesc::FloatCM, "VOUCRCYR", "VOUSSCENTRRECTCUTYREACH     "},
  {VarDesc::FloatCM, "VOUSRCXW", "VOUSSRECTCUTXWID            "},
  {VarDesc::FloatCM, "VOUSCXOF", "VOUSSCUTXOFFSET             "},
  {VarDesc::FloatCM, "VOUSCYPO", "VOUSSOIRCUTYPOS             "},
  {VarDesc::FloatCM, "VOUSCZPO", "VOUSSOIRCUTZPOS             "},
  {VarDesc::FloatCM, "CNBEAXTP", "CONBOXEARXTOP               "},
  {VarDesc::FloatCM, "CNBEAYTP", "CONBOXEARYTOP               "},
  {VarDesc::FloatCM, "CNBEAXBT", "CONBOXEARXBOT               "},
  {VarDesc::FloatCM, "CNBEAYBT", "CONBOXEARYBOT               "},
  {VarDesc::FloatCM, "CNBECXOL", "CONBOXEARCUTXOUTLOW         "},
  {VarDesc::FloatCM, "CNBECYOL", "CONBOXEARCUTYOUTLOW         "},
  {VarDesc::FloatCM, "CNBECYIL", "CONBOXEARCUTYINLOW          "},
  {VarDesc::FloatCM, "CNBECOHE", "CONBOXEARCUTOUTHEIGHT       "},
  {VarDesc::FloatCM, "CNBECXOU", "CONBOXEARCUTXOUTUP          "},
  {VarDesc::FloatCM, "CNBECYOU", "CONBOXEARCUTYOUTUP          "},
  {VarDesc::FloatCM, "CNBECYIU", "CONBOXEARCUTYINUP           "},
  {VarDesc::FloatCM, "CNBECZPO", "CONBOXEARCUTZPOS            "},
  {VarDesc::FloatCM, "CNBIECZP", "CONBOXINNEREARCUTZPOS       "},
  {VarDesc::FloatCM, "CNBCAZIN", "CONBOXCAVITYZINTER          "},
  {VarDesc::FloatCM, "VOUSSRAD", "VOUSSOIRRADIUS              "},
  {VarDesc::FloatCM, "CNBOXRAD", "CONBOXRADIUS                "},
};


VarDesc AECT_Vars[] = {
  {VarDesc::FloatCM, "CRYOT1", "CRYOT1           "},
  {VarDesc::FloatCM, "CRYOT2", "CRYOT2           "},
  {VarDesc::FloatCM, "CRYOS1", "CRYOS1           "},
  {VarDesc::FloatCM, "CRYOS2", "CRYOS2           "},
  {VarDesc::FloatCM, "CRYOEDGE", "CRYOEDGE         "},
  {VarDesc::FloatCM, "CRYOFLTY", "FLATY            "},
  {VarDesc::FloatCM, "CRYOR0", "CRYOR0           "},
  {VarDesc::FloatCM, "CRYOZMIN", "CRYOZMIN         "},
  {VarDesc::FloatCM, "CRYOZEXT", "CRYOZEXT         "},
  {VarDesc::FloatCM, "CRYOTHI1", "THICK1           "},
  {VarDesc::FloatCM, "CRYOTHI2", "THICK2           "},
  {VarDesc::FloatCM, "CRYOTHI3", "THICK3           "},
  {VarDesc::FloatCM, "CRYOTTU0", "CRYOTTU0         "},
  {VarDesc::FloatCM, "CRYORTU0", "CRYORTU0         "},
  {VarDesc::FloatCM, "CRYORTU1", "CRYORTU1         "},
  {VarDesc::FloatCM, "CRYODPHT", "DPHITU           "},
  {VarDesc::FloatCM, "CRYOSTL0", "CRYOSTOPLENGTH0  "},
  {VarDesc::FloatCM, "CRYOSTL1", "CRYOSTOPLENGTH1  "},
  {VarDesc::FloatCM, "CRYOSTH0", "CRYOSTOPHEIGHT0  "},
  {VarDesc::FloatCM, "CRYOSTW0", "CRYOSTOPWIDTH0   "},
  {VarDesc::FloatCM, "CRYOSTW1", "CRYOSTOPWIDTH1   "},
  {VarDesc::FloatCM, "CRYOSTTH", "CRYOSTOPTHICKNESS"},
  {VarDesc::FloatCM, "CRYOSTH1", "CRYOSTOPHEIGHT1  "},
  {VarDesc::FloatCM, "CRYOSTH2", "CRYOSTOPHEIGHT2  "},
  {VarDesc::FloatCM, "CRYOSTPZ", "CRYOSTOPPOSIZ    "},
  {VarDesc::FloatCM, "CRYOSUPL", "CRYOSUPL         "},
  {VarDesc::FloatCM, "CRYOSUPH", "CRYOSUPH         "},
  {VarDesc::FloatCM, "CRYOSUPW", "CRYOSUPW         "},
  {VarDesc::FloatCM, "CRYOSUPX", "CRYOSUPX         "},
  {VarDesc::FloatCM, "CNBXZMOF", "CONDBOXZMINOFFSET"},
  {VarDesc::FloatCM, "CNBXZEXT", "CONDBOXZEXT      "},
  {VarDesc::FloatCM, "CNBXTMIN", "CONDBOXTMIN      "},
  {VarDesc::FloatCM, "CNBXTMAX", "CONDBOXTMAX      "},
  {VarDesc::FloatCM, "CNBXTHIC", "CONDBOXTHICK     "},
  {VarDesc::FloatCM, "CNBXEDGE", "CONDBOXEDGE      "},
  {VarDesc::FloatCM, "SUPPLZOF", "SUPPLATZOFFSET   "},
  {VarDesc::FloatCM, "SUPPLZEX", "SUPPLATZEXT      "},
  {VarDesc::FloatCM, "SUPPLTMI", "SUPPLATTMIN      "},
  {VarDesc::FloatCM, "SUPPLTMA", "SUPPLATTMAX      "},
  {VarDesc::FloatCM, "SUPPLTHZ", "SUPPLATTHICKZ    "},
  {VarDesc::FloatCM, "SUPPLTHT", "SUPPLATTHICKT    "},
  {VarDesc::FloatCM, "SUPPLRHO", "SUPPLATRHOLE     "},
  {VarDesc::FloatCM, "SRVTU1DZ", "SERVTUR1DZ       "},
  {VarDesc::FloatCM, "SRVTU1HE", "SERVTUR1HEIGHT   "},
  {VarDesc::FloatCM, "SRVTU1OW", "SERVTUR1OUTWIDTH "},
  {VarDesc::FloatCM, "SRVTU1IW", "SERVTUR1INNWIDTH "},
  {VarDesc::FloatCM, "SRVTU1ED", "SERVTUR1EDGE     "},
  {VarDesc::FloatCM, "SRVTU2DZ", "SERVTUR2DZ       "},
  {VarDesc::FloatCM, "SRVTU2HE", "SERVTUR2HEIGHT   "},
  {VarDesc::FloatCM, "SRVTU2OR", "SERVTUR2OUTRAD   "},
  {VarDesc::FloatCM, "SRVTU2IR", "SERVTUR2INNRAD   "},
  {VarDesc::FloatCM, "SRVTU3DZ", "SERVTUR3DZ       "},
  {VarDesc::FloatCM, "SRVTU3HE", "SERVTUR3HEIGHT   "},
  {VarDesc::FloatCM, "SIFITU", "SIFITU           "},
  {VarDesc::FloatCM, "COFITU", "COFITU           "},
  {VarDesc::FloatCM, "CRYOZMAX", "CRYOZMAX         "},
  {VarDesc::FloatCM, "CNBXZMIN", "CONDBOXZMIN      "},
  {VarDesc::FloatCM, "CNBXZMAX", "CONDBOXZMAX      "},
  {VarDesc::FloatCM, "SUPPLZMI", "SUPPLATZMIN      "},
  {VarDesc::FloatCM, "SUPPLZMA", "SUPPLATZMAX      "},
  {VarDesc::FloatCM, "CRYOZMOY", "CRYOZMOY         "},
  {VarDesc::FloatCM, "CRYOALFA", "CRYOALFA         "},
  {VarDesc::FloatCM, "CRYOBETA", "CRYOBETA         "},
  {VarDesc::FloatCM, "CRYOT1P", "CRYOT1P          "},
  {VarDesc::FloatCM, "CRYOT2P", "CRYOT2P          "},
  {VarDesc::FloatCM, "CRYOS1P", "CRYOS1P          "},
  {VarDesc::FloatCM, "CRYOS2P", "CRYOS2P          "},
  {VarDesc::FloatCM, "CRYOFLYP", "FLATYP           "},
  {VarDesc::FloatCM, "SUPPLZME", "SUPPLATZMOY      "},
  {VarDesc::FloatCM, "XTEMP_0", "ARRAY            "},
  {VarDesc::FloatCM, "XTEMP_1", "                 "},
  {VarDesc::FloatCM, "XTEMP_2", "                 "},
  {VarDesc::FloatCM, "YTEMP_0", "ARRAY            "},
  {VarDesc::FloatCM, "YTEMP_1", "                 "},
  {VarDesc::FloatCM, "YTEMP_2", "                 "},
  {VarDesc::FloatCM, "DTEMP01", "DTEMP01          "},
  {VarDesc::FloatCM, "XTIMP_0", "ARRAY            "},
  {VarDesc::FloatCM, "XTIMP_1", "                 "},
  {VarDesc::FloatCM, "XTIMP_2", "                 "},
  {VarDesc::FloatCM, "YTIMP_0", "ARRAY            "},
  {VarDesc::FloatCM, "YTIMP_1", "                 "},
  {VarDesc::FloatCM, "YTIMP_2", "                 "},
  {VarDesc::FloatCM, "DTIMP01", "DTIMP01          "},
  {VarDesc::FloatCM, "UTEMP61", "UTEMP61          "},
  {VarDesc::FloatCM, "UTEMP62", "UTEMP62          "},
  {VarDesc::FloatCM, "UTIMP61", "UTIMP61          "},
  {VarDesc::FloatCM, "UTIMP62", "UTIMP62          "},
  {VarDesc::FloatCM, "DTEMP", "DTEMP            "},
  {VarDesc::FloatCM, "SUPPLATY_0", "ARRAY            "},
  {VarDesc::FloatCM, "SUPPLATY_1", "                 "},
  {VarDesc::FloatCM, "SUPPLATY_2", "                 "},
  {VarDesc::FloatCM, "SUPPLATY_3", "                 "},
  {VarDesc::FloatCM, "SUPPLATX_0", "ARRAY            "},
  {VarDesc::FloatCM, "SUPPLATX_1", "                 "},
  {VarDesc::FloatCM, "SUPPLATX_2", "                 "},
  {VarDesc::FloatCM, "SUPPLATX_3", "                 "},
  {VarDesc::FloatCM, "X1TEMP", "X1TEMP           "},
  {VarDesc::FloatCM, "X0TEMP", "X0TEMP           "},
};


VarDesc ECST_Vars[] = {
  {VarDesc::FloatCM, "STOTHIC1", "STOTHIC1  "},
  {VarDesc::FloatCM, "STOTHIC2", "STOTHIC2  "},
  {VarDesc::FloatCM, "STOTHIC3", "STOTHIC3  "},
  {VarDesc::FloatCM, "STOTHIC4", "STOTHIC4  "},
  {VarDesc::FloatCM, "STOLENGT", "STOLENGT  "},
  {VarDesc::FloatCM, "STOLENGA", "STOLENGA  "},
  {VarDesc::FloatCM, "STOXPREF", "STOXPREF  "},
  {VarDesc::FloatCM, "STOYPREF", "STOYPREF  "},
  {VarDesc::FloatCM, "STOHEIGH", "STOHEIGH  "},
  {VarDesc::FloatCM, "STOHEIG1", "STOHEIG1  "},
  {VarDesc::FloatCM, "STOHEIG2", "STOHEIG2  "},
  {VarDesc::FloatCM, "STOHEIG3", "STOHEIG3  "},
  {VarDesc::FloatCM, "STOHEIG4", "STOHEIG4  "},
  {VarDesc::FloatCM, "STODZBOT", "STODZBOT  "},
  {VarDesc::FloatCM, "STODZIN1", "STODZIN1  "},
  {VarDesc::FloatCM, "STODZIN2", "STODZIN2  "},
  {VarDesc::FloatCM, "STODZIN3", "STODZIN3  "},
  {VarDesc::FloatCM, "STODZIN4", "STODZIN4  "},
  {VarDesc::FloatCM, "STODZIN5", "STODZIN5  "},
  {VarDesc::FloatCM, "STODZTOP", "STODZTOP  "},
  {VarDesc::FloatCM, "STOANGLE", "STOANGLE  "},
  {VarDesc::FloatCM, "STOYPOS", "STOYPOS   "},
  {VarDesc::FloatCM, "STOGECXZ", "STOGECXZ  "},
  {VarDesc::FloatCM, "STOGECHE", "STOGECHE  "},
  {VarDesc::FloatCM, "STOCLCZP", "STOCLCZP  "},
  {VarDesc::FloatCM, "STOANGD2", "STOANGD2  "},
  {VarDesc::FloatCM, "STOANGD4", "STOANGD4  "},
  {VarDesc::FloatCM, "STOLENG1", "STOLENG1  "},
  {VarDesc::FloatCM, "STOFXPOS", "STOFXPOS  "},
  {VarDesc::FloatCM, "STOZPOS", "STOZPOS   "},
  {VarDesc::FloatCM, "STOUEYP", "STOUEYP   "},
  {VarDesc::FloatCM, "STOTEYP", "STOTEYP   "},
  {VarDesc::FloatCM, "STOTEZP", "STOTEZP   "},
  {VarDesc::FloatCM, "STOFLXA", "STOFLXA   "},
  {VarDesc::FloatCM, "STOFLYA", "STOFLYA   "},
  {VarDesc::FloatCM, "STOFLXB", "STOFLXB   "},
  {VarDesc::FloatCM, "STOFLYB", "STOFLYB   "},
  {VarDesc::FloatCM, "STOFLXC", "STOFLXC   "},
  {VarDesc::FloatCM, "STOFLYC", "STOFLYC   "},
  {VarDesc::FloatCM, "STOFLXD", "STOFLXD   "},
  {VarDesc::FloatCM, "STOFLYD", "STOFLYD   "},
  {VarDesc::FloatCM, "STOFLXE", "STOFLXE   "},
  {VarDesc::FloatCM, "STOFLYE", "STOFLYE   "},
  {VarDesc::FloatCM, "STOFLXF", "STOFLXF   "},
  {VarDesc::FloatCM, "STOFLYF", "STOFLYF   "},
  {VarDesc::FloatCM, "STOFLXG", "STOFLXG   "},
  {VarDesc::FloatCM, "STOFLYG", "STOFLYG   "},
  {VarDesc::FloatCM, "STOFLXH", "STOFLXH   "},
  {VarDesc::FloatCM, "STOFLYH", "STOFLYH   "},
  {VarDesc::FloatCM, "STOFLXI", "STOFLXI   "},
  {VarDesc::FloatCM, "STOFLYI", "STOFLYI   "},
  {VarDesc::FloatCM, "STOCOXA", "STOCOXA   "},
  {VarDesc::FloatCM, "STOCOYA", "STOCOYA   "},
  {VarDesc::FloatCM, "STOCOXA1", "STOCOXA1  "},
  {VarDesc::FloatCM, "STOCOYA1", "STOCOYA1  "},
  {VarDesc::FloatCM, "STOCOXB", "STOCOXB   "},
  {VarDesc::FloatCM, "STOCOYB", "STOCOYB   "},
  {VarDesc::FloatCM, "STOCOXB1", "STOCOXB1  "},
  {VarDesc::FloatCM, "STOCOYB1", "STOCOYB1  "},
  {VarDesc::FloatCM, "STOCOXC", "STOCOXC   "},
  {VarDesc::FloatCM, "STOCOYC", "STOCOYC   "},
  {VarDesc::FloatCM, "STOCOXC1", "STOCOXC1  "},
  {VarDesc::FloatCM, "STOCOYC1", "STOCOYC1  "},
  {VarDesc::FloatCM, "STOCOXD", "STOCOXD   "},
  {VarDesc::FloatCM, "STOCOYD", "STOCOYD   "},
  {VarDesc::FloatCM, "STOCOXD1", "STOCOXD1  "},
  {VarDesc::FloatCM, "STOCOYD1", "STOCOYD1  "},
  {VarDesc::FloatCM, "STOCOXE", "STOCOXE   "},
  {VarDesc::FloatCM, "STOCOYE", "STOCOYE   "},
  {VarDesc::FloatCM, "STOCOXE1", "STOCOXE1  "},
  {VarDesc::FloatCM, "STOCOYE1", "STOCOYE1  "},
  {VarDesc::FloatCM, "STOCOXF", "STOCOXF   "},
  {VarDesc::FloatCM, "STOCOYF", "STOCOYF   "},
  {VarDesc::FloatCM, "STOCOXF1", "STOCOXF1  "},
  {VarDesc::FloatCM, "STOCOYF1", "STOCOYF1  "},
  {VarDesc::FloatCM, "STOCOXG", "STOCOXG   "},
  {VarDesc::FloatCM, "STOCOYG", "STOCOYG   "},
  {VarDesc::FloatCM, "STOCOXG1", "STOCOXG1  "},
  {VarDesc::FloatCM, "STOCOYG1", "STOCOYG1  "},
  {VarDesc::FloatCM, "STOCOXH", "STOCOXH   "},
  {VarDesc::FloatCM, "STOCOYH", "STOCOYH   "},
  {VarDesc::FloatCM, "STOCOXH1", "STOCOXH1  "},
  {VarDesc::FloatCM, "STOCOYH1", "STOCOYH1  "},
  {VarDesc::FloatCM, "STOCOXI", "STOCOXI   "},
  {VarDesc::FloatCM, "STOCOYI", "STOCOYI   "},
  {VarDesc::FloatCM, "STOCOXI1", "STOCOXI1  "},
  {VarDesc::FloatCM, "STOCOYI1", "STOCOYI1  "},
  {VarDesc::FloatCM, "STOCO3DX", "STOCO3DX  "},
  {VarDesc::FloatCM, "STOCO3DY", "STOCO3DY  "},
  {VarDesc::FloatCM, "STOCO3DZ", "STOCO3DZ  "},
  {VarDesc::FloatCM, "STOCO3ZP", "STOCO3ZP  "},
  {VarDesc::FloatCM, "STOCO3YP", "STOCO3YP  "},
  {VarDesc::FloatCM, "STOUEXA", "STOUEXA   "},
  {VarDesc::FloatCM, "STOUEYA", "STOUEYA   "},
  {VarDesc::FloatCM, "STOUEXB", "STOUEXB   "},
  {VarDesc::FloatCM, "STOUEYB", "STOUEYB   "},
  {VarDesc::FloatCM, "STOUEXC", "STOUEXC   "},
  {VarDesc::FloatCM, "STOUEYC", "STOUEYC   "},
  {VarDesc::FloatCM, "STOUECXA", "STOUECXA  "},
  {VarDesc::FloatCM, "STOUECYA", "STOUECYA  "},
  {VarDesc::FloatCM, "STOUECXB", "STOUECXB  "},
  {VarDesc::FloatCM, "STOUECYB", "STOUECYB  "},
  {VarDesc::FloatCM, "STOUECZP", "STOUECZP  "},
  {VarDesc::FloatCM, "STOUECYP", "STOUECYP  "},
};


VarDesc FEET_Vars[] = {
  {VarDesc::FloatCM, "MAINPLXO", "MAINPLATEXORIGIN                        "},
  {VarDesc::FloatCM, "MNPLGXWI", "MAINPLATE_GROUNDXWIDTH                  "},
  {VarDesc::FloatCM, "MNPLRCDX", "MAINPLATEROUNDCUTDX                     "},
  {VarDesc::FloatCM, "MNPLDXEX", "MAINPLATEDXEXTR                         "},
  {VarDesc::FloatCM, "MNPLUPDX", "MAINPLATEUPPERDX                        "},
  {VarDesc::FloatCM, "MNPLUPXW", "MAINPLATEUPPERXWIDTH                    "},
  {VarDesc::FloatCM, "MNPLMIDX", "MAINPLATEMIDDLEDX                       "},
  {VarDesc::FloatCM, "MAINPLYO", "MAINPLATEYORIGIN                        "},
  {VarDesc::FloatCM, "MAINPLHE", "MAINPLATEHEIGHT                         "},
  {VarDesc::FloatCM, "MAINPLH1", "MAINPLATEHEIGHT_INTERMEDIATE1           "},
  {VarDesc::FloatCM, "MNPLRCYP", "MAINPLATEROUNDCUT_Y                     "},
  {VarDesc::FloatCM, "MNPLSRCY", "MAINPLATESECONDARYROUNDCUT_Y            "},
  {VarDesc::FloatCM, "MNPLUPDY", "MAINPLATEUPPERDY                        "},
  {VarDesc::FloatCM, "MNPLMIDY", "MAINPLATEMIDDLEDY                       "},
  {VarDesc::FloatCM, "MNPLPEHE", "MAINPLATE_PEDESTALHEIGHT                "},
  {VarDesc::FloatCM, "MAINPLDZ", "MAINPLATEDZ                             "},
  {VarDesc::FloatCM, "MNPLZSEP", "MAINPLATEZSEP                           "},
  {VarDesc::FloatCM, "MNPLRCRA", "MAINPLATEROUNDCUT_R                     "},
  {VarDesc::FloatCM, "MNPLSRCR", "MAINPLATESECONDARYROUNDCUT_R            "},
  {VarDesc::FloatCM, "MINCPLXW", "MINICONNPLATE_XWIDTH                    "},
  {VarDesc::FloatCM, "MINCPLYH", "MINICONNPLATE_YHEIGHT                   "},
  {VarDesc::FloatCM, "MINCPLTZ", "MINICONNPLATE_THETAZ                    "},
  {VarDesc::FloatCM, "MINCPLZL", "MINICONNPLATE_ZLENGTH                   "},
  {VarDesc::FloatCM, "SLACPLXW", "SLANTEDCONNPLATE_XWIDTH                 "},
  {VarDesc::FloatCM, "SLACPLYH", "SLANTEDCONNPLATE_YHEIGHT                "},
  {VarDesc::FloatCM, "SLACPLTZ", "SLANTEDCONNPLATE_THETAZ                 "},
  {VarDesc::FloatCM, "SLACPLZL", "SLANTEDCONNPLATE_ZLENGTH                "},
  {VarDesc::FloatCM, "GRNDPLZL", "GROUNDPLATE_ZLENGTH                     "},
  {VarDesc::FloatCM, "VCNPLXWI", "VERTICALCONNPLATE_XWIDTH                "},
  {VarDesc::FloatCM, "VCNPLYHE", "VERTICALCONNPLATE_YHEIGHT               "},
  {VarDesc::FloatCM, "VCNPLDXE", "VERTICALCONNPLATE_DX                    "},
  {VarDesc::FloatCM, "VCNPLDYE", "VERTICALCONNPLATE_DY                    "},
  {VarDesc::FloatCM, "UCNPLXWI", "UPPERCONNPLATE_XWIDTH                   "},
  {VarDesc::FloatCM, "UCNPLYHE", "UPPERCONNPLATE_YHEIGHT                  "},
  {VarDesc::FloatCM, "XWIDTH", "XWIDTH                                  "},
  {VarDesc::FloatCM, "UPPERHEY", "RAILSUPPORTUPPER_YHEIGHT                "},
  {VarDesc::FloatCM, "UPPERLEZ", "RAILSUPPORTUPPER_ZLENGTH                "},
  {VarDesc::FloatCM, "LOWERHEY", "RAILSUPPORTLOWER_YHEIGHT                "},
  {VarDesc::FloatCM, "LOWERLEZ", "RAILSUPPORTLOWER_ZLENGTH                "},
  {VarDesc::FloatCM, "TOTALHEY", "RAILSUPPORTTOTAL_YHEIGHT                "},
  {VarDesc::FloatCM, "CENTRLEZ", "RAILSUPPORTCENTRAL_ZLENGTH              "},
  {VarDesc::FloatCM, "CENTRXTH", "RAILSUPPORTCENTRAL_XTHICK               "},
  {VarDesc::FloatCM, "MIDLLYTH", "RAILSUPPORTMIDDLE_YTHICK                "},
  {VarDesc::FloatCM, "VERTIZTH", "RAILSUPPORTVERTICAL_ZTHICK              "},
  {VarDesc::FloatCM, "MIDLLYPO", "RAILSUPPORTMIDDLE_YPOS                  "},
  {VarDesc::FloatCM, "EXTREHEY", "RAILSUPPORTEXTR_YHEIGHT                 "},
  {VarDesc::FloatCM, "EXTRELEZ", "RAILSUPPORTEXTR_ZLENGTH                 "},
  {VarDesc::FloatCM, "CENTRHEY", "RAILSUPPORTCENTRAL_YHEIGHT              "},
  {VarDesc::FloatCM, "CNFEVOXW", "CONNFEETVOUSS_XWIDTH                    "},
  {VarDesc::FloatCM, "CNFEVOYH", "CONNFEETVOUSS_YHEIGHT                   "},
  {VarDesc::FloatCM, "CNFEVOZL", "CONNFEETVOUSS_ZLENGTH                   "},
  {VarDesc::FloatCM, "CNFEVOXT", "CONNFEETVOUSS_XTHICK                    "},
  {VarDesc::FloatCM, "CNFEVOYT", "CONNFEETVOUSS_YTHICK                    "},
  {VarDesc::FloatCM, "CNFEVOZT", "CONNFEETVOUSS_ZTHICK                    "},
  {VarDesc::FloatCM, "G12LPXWT", "GIRDER12_LATERALPLATE_XWIDTH_TOTAL      "},
  {VarDesc::FloatCM, "G12IPSXO", "GIRDER12_INNERPLATESMALLOFFSET_XDIST    "},
  {VarDesc::FloatCM, "G12IPSLX", "GIRDER12_INNERPLATESMALL2LARGE_XDIST    "},
  {VarDesc::FloatCM, "G12IPLLX", "GIRDER12_INNERPLATELARGE2LARGE_XDIST    "},
  {VarDesc::FloatCM, "G12IPLSX", "GIRDER12_INNERPLATELARGE2SMALL_XDIST    "},
  {VarDesc::FloatCM, "G12RPXWI", "GIRDER12_REINFORCEMENTPLATE_XWIDTH      "},
  {VarDesc::FloatCM, "G12LPXW1", "GIRDER12_LATERALPLATE_XWIDTH_INTER1     "},
  {VarDesc::FloatCM, "G12LPX12", "GIRDER12_LATERALPLATE_XWIDTH_INTER1INTER"},
  {VarDesc::FloatCM, "G12IPXWI", "GIRDER12_INNERPLATE_XWIDTH              "},
  {VarDesc::FloatCM, "G12UMPWI", "GIRDER12_UPPERMINIPLATE_XWIDTH          "},
  {VarDesc::FloatCM, "G12SMPWI", "GIRDER12_SIDEMINIPLATE_XWIDTH           "},
  {VarDesc::FloatCM, "G12YORIG", "GIRDER12_YORIGIN                        "},
  {VarDesc::FloatCM, "G12LPYHT", "GIRDER12_LATERALPLATE_YHEIGHT_TOTAL     "},
  {VarDesc::FloatCM, "G12LPYH1", "GIRDER12_LATERALPLATE_YHEIGHT_INTER1    "},
  {VarDesc::FloatCM, "G12LPYH2", "GIRDER12_LATERALPLATE_YHEIGHT_INTER2    "},
  {VarDesc::FloatCM, "G12LPYTH", "GIRDER12_LOWERPLATE_YTHICK              "},
  {VarDesc::FloatCM, "G12UPYTH", "GIRDER12_UPPERPLATE_YTHICK              "},
  {VarDesc::FloatCM, "G12RPYDI", "GIRDER12_REINFORCEMENTPLATE_YDIST       "},
  {VarDesc::FloatCM, "G12UMPYH", "GIRDER12_UPPERMINIPLATE_YHEIGHT         "},
  {VarDesc::FloatCM, "G12UPZLE", "GIRDER12_UPPERPLATE_ZLENGTH             "},
  {VarDesc::FloatCM, "G12UMPZL", "GIRDER12_UPPERMINIPLATE_ZLENGTH         "},
  {VarDesc::FloatCM, "G12BPZLE", "GIRDER12_BOTTOMPLATE_ZLENGTH            "},
  {VarDesc::FloatCM, "G12LPZLE", "GIRDER12_LATERALPLATE_ZLENGTH           "},
  {VarDesc::FloatCM, "G12RPZLE", "GIRDER12_REINFORCEMENTPLATE_ZLENGTH     "},
  {VarDesc::FloatCM, "G12SMPZL", "GIRDER12_SIDEMINIPLATE_ZLENGTH          "},
  {VarDesc::FloatCM, "G23LPXWT", "GIRDER23_LATERALPLATE_XWIDTH_TOTAL      "},
  {VarDesc::FloatCM, "G23IPSXO", "GIRDER23_INNERPLATESMALLOFFSET_XDIST    "},
  {VarDesc::FloatCM, "G23IPSLX", "GIRDER23_INNERPLATESMALL2LARGE_XDIST    "},
  {VarDesc::FloatCM, "G23IPLLX", "GIRDER23_INNERPLATELARGE2LARGE_XDIST    "},
  {VarDesc::FloatCM, "G23IPLSX", "GIRDER23_INNERPLATELARGE2SMALL_XDIST    "},
  {VarDesc::FloatCM, "G23RPXWI", "GIRDER23_REINFORCEMENTPLATE_XWIDTH      "},
  {VarDesc::FloatCM, "G23LPXW1", "GIRDER23_LATERALPLATE_XWIDTH_INTER1     "},
  {VarDesc::FloatCM, "G23LPX12", "GIRDER23_LATERALPLATE_XWIDTH_INTER1INTER"},
  {VarDesc::FloatCM, "G23IPXWI", "GIRDER23_INNERPLATE_XWIDTH              "},
  {VarDesc::FloatCM, "G23UMPWI", "GIRDER23_UPPERMINIPLATE_XWIDTH          "},
  {VarDesc::FloatCM, "G23SMPWI", "GIRDER23_SIDEMINIPLATE_XWIDTH           "},
  {VarDesc::FloatCM, "G34LPXWT", "GIRDER34_LATERALPLATE_XWIDTH_TOTAL      "},
  {VarDesc::FloatCM, "G34IPSXO", "GIRDER34_INNERPLATESMALLOFFSET_XDIST    "},
  {VarDesc::FloatCM, "G34IPSLX", "GIRDER34_INNERPLATESMALL2LARGE_XDIST    "},
  {VarDesc::FloatCM, "G34IPLLX", "GIRDER34_INNERPLATELARGE2LARGE_XDIST    "},
  {VarDesc::FloatCM, "G34IPLSX", "GIRDER34_INNERPLATELARGE2SMALL_XDIST    "},
  {VarDesc::FloatCM, "G34RPXWI", "GIRDER34_REINFORCEMENTPLATE_XWIDTH      "},
  {VarDesc::FloatCM, "G34LPXW1", "GIRDER34_LATERALPLATE_XWIDTH_INTER1     "},
  {VarDesc::FloatCM, "G34LPX12", "GIRDER34_LATERALPLATE_XWIDTH_INTER1INTER"},
  {VarDesc::FloatCM, "G34IPXWI", "GIRDER34_INNERPLATE_XWIDTH              "},
  {VarDesc::FloatCM, "G34UMPWI", "GIRDER34_UPPERMINIPLATE_XWIDTH          "},
  {VarDesc::FloatCM, "G34SMPWI", "GIRDER34_SIDEMINIPLATE_XWIDTH           "},
  {VarDesc::FloatCM, "EXMPHEIG", "EXTREMITYMAINPLATE_HEIGHT               "},
  {VarDesc::FloatCM, "EXMPRCRA", "EXTREMITYMAINPLATEROUNDCUT_R            "},
  {VarDesc::FloatCM, "EXMPRCDX", "EXTREMITYMAINPLATEROUNDCUT_DX           "},
  {VarDesc::FloatCM, "EXMPRCYP", "EXTREMITYMAINPLATEROUNDCUT_Y            "},
  {VarDesc::FloatCM, "EXMCPYHE", "EXTREMITYMINICONNPLATE_YHEIGHT          "},
  {VarDesc::FloatCM, "EXMCPZLE", "EXTREMITYMINICONNPLATE_ZLENGTH          "},
  {VarDesc::FloatCM, "EXGPZLEN", "EXTREMITYGROUNDPLATE_ZLENGTH            "},
  {VarDesc::FloatCM, "EXSCPZLE", "EXTREMITYSLANTEDCONNPLATE_ZLENGTH       "},
  {VarDesc::FloatCM, "EXRSZOFF", "EXTREMITYRAILSUPPORT_ZOFFSET            "},
  {VarDesc::FloatCM, "EXMPHEI1", "EXTREMITYMAINPLATE_HEIGHT_INTERMEDIATE1 "},
  {VarDesc::FloatCM, "EXMPHEI2", "EXTREMITYMAINPLATE_HEIGHT_INTERMEDIATE2 "},
  {VarDesc::FloatCM, "EXMPXWID", "EXTREMITYMAINPLATE_XWIDTH               "},
  {VarDesc::FloatCM, "EXMPXWI1", "EXTREMITYMAINPLATE_XWIDTH_INTER1        "},
  {VarDesc::FloatCM, "EXMPXWI2", "EXTREMITYMAINPLATE_XWIDTH_INTER2        "},
  {VarDesc::FloatCM, "EXMPYHI1", "EXTREMITYMAINPLATE_YHEIGHT_INTER1       "},
  {VarDesc::FloatCM, "EXMPYHI2", "EXTREMITYMAINPLATE_YHEIGHT_INTER2       "},
  {VarDesc::FloatCM, "EXMPYHI3", "EXTREMITYMAINPLATE_YHEIGHT_INTER3       "},
  {VarDesc::FloatCM, "EXMPYHI4", "EXTREMITYMAINPLATE_YHEIGHT_INTER4       "},
  {VarDesc::FloatCM, "EXMPXDI1", "EXTREMITYMAINPLATE_XDIST_INTER1         "},
  {VarDesc::FloatCM, "EXMPXDI2", "EXTREMITYMAINPLATE_XDIST_INTER2         "},
  {VarDesc::FloatCM, "EXMPYDI2", "EXTREMITYMAINPLATE_YDIST_INTER1         "},
  {VarDesc::FloatCM, "EXVCPLDX", "EXTREMITYVERTICALCONNPLATE_DX           "},
  {VarDesc::FloatCM, "EXVCPLDY", "EXTREMITYVERTICALCONNPLATE_DY           "},
  {VarDesc::FloatCM, "EXMPZSEP", "EXTREMITYMAINPLATE_ZSEP                 "},
  {VarDesc::FloatCM, "EXSPRCRA", "EXTREMITYSLANTEDCONNPLATEROUNDCUT_RADIUS"},
  {VarDesc::FloatCM, "EXSPRCOF", "EXTREMITYSLANTEDCONNPLATEROUNDCUT_OFFSET"},
  {VarDesc::FloatCM, "EXVCPTHI", "EXTREMITYVERTICALCONNPLATE_THICKNESS    "},
  {VarDesc::FloatCM, "EXVCPXEX", "EXTREMITYVERTICALCONNPLATE_XEXT         "},
  {VarDesc::FloatCM, "EXVCPHEI", "EXTREMITYVERTICALCONNPLATE_HEIGHT       "},
  {VarDesc::FloatCM, "EXVCPYCU", "EXTREMITYVERTICALCONNPLATE_CUTY         "},
  {VarDesc::FloatCM, "EXVCPXCU", "EXTREMITYVERTICALCONNPLATE_CUTX         "},
  {VarDesc::FloatCM, "EXCFVXWI", "EXTREMITYCONNFEETVOUSS_XWIDTH           "},
  {VarDesc::FloatCM, "EXCFVYHE", "EXTREMITYCONNFEETVOUSS_YHEIGHT          "},
  {VarDesc::FloatCM, "EXCFVZLE", "EXTREMITYCONNFEETVOUSS_ZLENGTH          "},
  {VarDesc::FloatCM, "EXRSUZLE", "EXTREMITYRAILSUPPORTUPPER_ZLENGTH       "},
  {VarDesc::FloatCM, "EXRSLDZL", "EXTREMITYRAILSUPPORTLOWER_DZLENGTH      "},
  {VarDesc::FloatCM, "EXRSTYHE", "EXTREMITYRAILSUPPORTTOTAL_YHEIGHT       "},
  {VarDesc::FloatCM, "EXRSLYHE", "EXTREMITYRAILSUPPORTLOWER_YHEIGHT       "},
  {VarDesc::FloatCM, "EXRSEYHE", "EXTREMITYRAILSUPPORTEXTR_YHEIGHT        "},
  {VarDesc::FloatCM, "EXRSCYHE", "EXTREMITYRAILSUPPORTCENTRAL_YHEIGHT     "},
  {VarDesc::FloatCM, "EXRSCZLE", "EXTREMITYRAILSUPPORTCENTRAL_ZLENGTH     "},
  {VarDesc::FloatCM, "EXRSMZLE", "EXTREMITYRAILSUPPORTMIDDLE_ZLENGTH      "},
  {VarDesc::FloatCM, "EXRSCXWI", "EXTREMITYRAILSUPPORTCENTRAL_XWIDTH      "},
  {VarDesc::FloatCM, "EXRSMXWI", "EXTREMITYRAILSUPPORTMIDDLE_XWIDTH       "},
  {VarDesc::FloatCM, "EXRSVXWI", "EXTREMITYRAILSUPPORTVERTICAL_XWIDTH     "},
  {VarDesc::FloatCM, "EXRSVZI1", "EXTREMITYRAILSUPPORTVERTICAL_ZPOS_INTER1"},
  {VarDesc::FloatCM, "EXRSEZLE", "EXTREMITYRAILSUPPORTEXTR_ZLENGTH        "},
  {VarDesc::FloatCM, "EXRSEZOF", "EXTREMITYRAILSUPPORTEXTR_ZOFFSET        "},
  {VarDesc::FloatCM, "EXRSC1DE", "EXTREMITYRAILSUPPORTCUT1_DEPTH          "},
  {VarDesc::FloatCM, "EXRSC1WI", "EXTREMITYRAILSUPPORTCUT1_WIDTH          "},
  {VarDesc::FloatCM, "EXRSC1XE", "EXTREMITYRAILSUPPORTCUT1_DUMXEXT        "},
  {VarDesc::FloatCM, "EXRSC2XE", "EXTREMITYRAILSUPPORTCUT2_DUMXEXT        "},
  {VarDesc::FloatCM, "EXRSC2Z1", "EXTREMITYRAILSUPPORTCUT1_ZOFFSET1       "},
  {VarDesc::FloatCM, "EXRSC2Z2", "EXTREMITYRAILSUPPORTCUT1_ZOFFSET2       "},
  {VarDesc::FloatCM, "EXRSMCDE", "EXTREMITYRAILSUPPORTMIDDLECUT_DEPTH     "},
  {VarDesc::FloatCM, "EXRSMCWI", "EXTREMITYRAILSUPPORTMIDDLECUT_WIDTH     "},
  {VarDesc::FloatCM, "EXRSMCZO", "EXTREMITYRAILSUPPORTMIDDLECUT_ZOFFSET   "},
  {VarDesc::FloatCM, "EXRSMCY1", "EXTREMITYRAILSUPPORTMIDDLECUT_DUMY1     "},
  {VarDesc::FloatCM, "EXRSMCY2", "EXTREMITYRAILSUPPORTMIDDLECUT_DUMY2     "},
  {VarDesc::FloatCM, "STDFOOXP", "STANDARDFOOT_XPOS                       "},
  {VarDesc::FloatCM, "STDFOOYP", "STANDARDFOOT_YPOS                       "},
  {VarDesc::FloatCM, "ZPOSFEE1", "ZPOSFEE1                                "},
  {VarDesc::FloatCM, "ZPOSFEE2", "ZPOSFEE2                                "},
  {VarDesc::FloatCM, "ZPOSFEE3", "ZPOSFEE3                                "},
  {VarDesc::FloatCM, "ZPOSFEE4", "ZPOSFEE4                                "},
  {VarDesc::FloatCM, "ZPOSFEE5", "ZPOSFEE5                                "},
  {VarDesc::FloatCM, "GIRDYHEI", "YHEIGHTGIRDER                           "},
  {VarDesc::FloatCM, "GIRDXPOS", "XPOSGIRDER                              "},
  {VarDesc::FloatCM, "MPLAHCEC", "FEETMAINPLATE_HCEC                      "},
  {VarDesc::FloatCM, "DUMZ", "DUMZ                                    "},
  {VarDesc::FloatCM, "DUMPHI", "DUMPHI                                  "},
  {VarDesc::FloatCM, "MAINPLXA", "MAINPLXA                                "},
  {VarDesc::FloatCM, "MAINPLYA", "MAINPLYA                                "},
  {VarDesc::FloatCM, "MAINPLXB", "MAINPLXB                                "},
  {VarDesc::FloatCM, "MAINPLYB", "MAINPLYB                                "},
  {VarDesc::FloatCM, "MAINPLXC", "MAINPLXC                                "},
  {VarDesc::FloatCM, "MAINPLYC", "MAINPLYC                                "},
  {VarDesc::FloatCM, "MAPLRCXC", "MAINPLATEROUNDCUT_XC                    "},
  {VarDesc::FloatCM, "MAPLRCYC", "MAINPLATEROUNDCUT_YC                    "},
  {VarDesc::FloatCM, "MAIPLXC3", "MAIPLXC3                                "},
  {VarDesc::FloatCM, "MAIPLYC3", "MAIPLYC3                                "},
  {VarDesc::FloatCM, "MPLXEXTR", "FEETMAINPLATE_XEXTR                     "},
  {VarDesc::FloatCM, "MPLYEXTR", "FEETMAINPLATE_YEXTR                     "},
  {VarDesc::FloatCM, "MPLREXTR", "FEETMAINPLATE_REXTR                     "},
  {VarDesc::FloatCM, "MAIPLXC2", "MAIPLXC2                                "},
  {VarDesc::FloatCM, "MAIPLYC2", "MAIPLYC2                                "},
  {VarDesc::FloatCM, "MPLXCEC1", "MPLXCEC1                                "},
  {VarDesc::FloatCM, "MPLYCEC1", "MPLYCEC1                                "},
  {VarDesc::FloatCM, "MPLXCEC2", "MPLXCEC2                                "},
  {VarDesc::FloatCM, "MPLYCEC2", "MPLYCEC2                                "},
  {VarDesc::FloatCM, "A", "A                                       "},
  {VarDesc::FloatCM, "B", "B                                       "},
  {VarDesc::FloatCM, "APRIME", "APRIME                                  "},
  {VarDesc::FloatCM, "BPRIME", "BPRIME                                  "},
  {VarDesc::FloatCM, "ASECONDE", "ASECONDE                                "},
  {VarDesc::FloatCM, "BSECONDE", "BSECONDE                                "},
  {VarDesc::FloatCM, "MAINPLXD", "MAINPLXD                                "},
  {VarDesc::FloatCM, "MAINPLYD", "MAINPLYD                                "},
  {VarDesc::FloatCM, "MAINPLXE", "MAINPLXE                                "},
  {VarDesc::FloatCM, "MAINPLYE", "MAINPLYE                                "},
  {VarDesc::FloatCM, "MAIPLXC1", "MAIPLXC1                                "},
  {VarDesc::FloatCM, "MAIPLYC1", "MAIPLYC1                                "},
  {VarDesc::FloatCM, "MAINPLXF", "MAINPLXF                                "},
  {VarDesc::FloatCM, "MAINPLYF", "MAINPLYF                                "},
  {VarDesc::FloatCM, "MAINPLXG", "MAINPLXG                                "},
  {VarDesc::FloatCM, "MAINPLYG", "MAINPLYG                                "},
  {VarDesc::FloatCM, "MAINPLXH", "MAINPLXH                                "},
  {VarDesc::FloatCM, "MAINPLYH", "MAINPLYH                                "},
  {VarDesc::FloatCM, "MAINPLXI", "MAINPLXI                                "},
  {VarDesc::FloatCM, "MAINPLYI", "MAINPLYI                                "},
  {VarDesc::FloatCM, "MAINPLXJ", "MAINPLXJ                                "},
  {VarDesc::FloatCM, "MAINPLYJ", "MAINPLYJ                                "},
  {VarDesc::FloatCM, "ALPHA1", "ALPHA1                                  "},
  {VarDesc::FloatCM, "COAL1", "COAL1                                   "},
  {VarDesc::FloatCM, "SIAL1", "SIAL1                                   "},
  {VarDesc::FloatCM, "MICPDIAG", "MINICONNPLATE_DIAG                      "},
  {VarDesc::FloatCM, "MICPXPOS", "MINICONNPLATE_X                         "},
  {VarDesc::FloatCM, "MICPYPOS", "MINICONNPLATE_Y                         "},
  {VarDesc::FloatCM, "ALPHA2", "ALPHA2                                  "},
  {VarDesc::FloatCM, "COAL2", "COAL2                                   "},
  {VarDesc::FloatCM, "SIAL2", "SIAL2                                   "},
  {VarDesc::FloatCM, "SLCPDIAG", "SLANTEDCONNPLATE_DIAG                   "},
  {VarDesc::FloatCM, "SLCPXPOS", "SLANTEDCONNPLATE_X                      "},
  {VarDesc::FloatCM, "SLCPYPOS", "SLANTEDCONNPLATE_Y                      "},
  {VarDesc::FloatCM, "GRNPXWID", "GROUNDPLATE_XWIDTH                      "},
  {VarDesc::FloatCM, "GRNPYHEI", "GROUNDPLATE_YHEIGHT                     "},
  {VarDesc::FloatCM, "GRNPXPOS", "GROUNDPLATE_X                           "},
  {VarDesc::FloatCM, "GRNPYPOS", "GROUNDPLATE_Y                           "},
  {VarDesc::FloatCM, "VECPXPOS", "VERTICALCONNPLATE_X                     "},
  {VarDesc::FloatCM, "VECPYPOS", "VERTICALCONNPLATE_Y                     "},
  {VarDesc::FloatCM, "VECPZLEN", "VERTICALCONNPLATE_ZLENGTH               "},
  {VarDesc::FloatCM, "UPCPXPOS", "UPPERCONNPLATE_X                        "},
  {VarDesc::FloatCM, "UPCPYPOS", "UPPERCONNPLATE_Y                        "},
  {VarDesc::FloatCM, "UPCPZLEN", "UPPERCONNPLATE_ZLENGTH                  "},
  {VarDesc::FloatCM, "MIDYPOSI", "RAILSUPPORTMIDDLE_Y                     "},
  {VarDesc::FloatCM, "POSX", "POSX                                    "},
  {VarDesc::FloatCM, "POSY", "POSY                                    "},
  {VarDesc::FloatCM, "CNFEVOXP", "CONNFEETVOUSS_X                         "},
  {VarDesc::FloatCM, "CNFEVOYP", "CONNFEETVOUSS_Y                         "},
  {VarDesc::FloatCM, "G12BOPXA", "GIRDER12_BOTTOMPLATE_XA                 "},
  {VarDesc::FloatCM, "G12BOPXB", "GIRDER12_BOTTOMPLATE_XB                 "},
  {VarDesc::FloatCM, "G12BOPXC", "GIRDER12_BOTTOMPLATE_XC                 "},
  {VarDesc::FloatCM, "G12BOPXD", "GIRDER12_BOTTOMPLATE_XD                 "},
  {VarDesc::FloatCM, "G12BOPXE", "GIRDER12_BOTTOMPLATE_XE                 "},
  {VarDesc::FloatCM, "G12BOPXF", "GIRDER12_BOTTOMPLATE_XF                 "},
  {VarDesc::FloatCM, "G12BOPYA", "GIRDER12_BOTTOMPLATE_YA                 "},
  {VarDesc::FloatCM, "G12BOPYB", "GIRDER12_BOTTOMPLATE_YB                 "},
  {VarDesc::FloatCM, "G12BOPYC", "GIRDER12_BOTTOMPLATE_YC                 "},
  {VarDesc::FloatCM, "G12BOPYD", "GIRDER12_BOTTOMPLATE_YD                 "},
  {VarDesc::FloatCM, "G12BOPYE", "GIRDER12_BOTTOMPLATE_YE                 "},
  {VarDesc::FloatCM, "G12BOPYF", "GIRDER12_BOTTOMPLATE_YF                 "},
  {VarDesc::FloatCM, "G12IPZLE", "GIRDER12_INNERPLATE_ZLENGTH             "},
  {VarDesc::FloatCM, "G12IPLYH", "GIRDER12_INNERPLATELARGE_YHEIGHT        "},
  {VarDesc::FloatCM, "G12IPLYP", "GIRDER12_INNERPLATELARGE_YPOS           "},
  {VarDesc::FloatCM, "G12IPSYH", "GIRDER12_INNERPLATESMALL_YHEIGHT        "},
  {VarDesc::FloatCM, "G12IPSYP", "GIRDER12_INNERPLATESMALL_YPOS           "},
  {VarDesc::FloatCM, "G12IPSX1", "GIRDER12_INNERPLATESMALL1_XPOS          "},
  {VarDesc::FloatCM, "G12IPLX1", "GIRDER12_INNERPLATELARGE1_XPOS          "},
  {VarDesc::FloatCM, "G12IPLX2", "GIRDER12_INNERPLATELARGE2_XPOS          "},
  {VarDesc::FloatCM, "G12IPSX2", "GIRDER12_INNERPLATESMALL2_XPOS          "},
  {VarDesc::FloatCM, "G12UPXWI", "GIRDER12_UPPERPLATE_XWIDTH              "},
  {VarDesc::FloatCM, "G12UPYHE", "GIRDER12_UPPERPLATE_YHEIGHT             "},
  {VarDesc::FloatCM, "G12UPYPO", "GIRDER12_UPPERPLATE_YPOS                "},
  {VarDesc::FloatCM, "G12REPXA", "GIRDER12_REINFORCEMENTPLATE_XA          "},
  {VarDesc::FloatCM, "G12REPXB", "GIRDER12_REINFORCEMENTPLATE_XB          "},
  {VarDesc::FloatCM, "G12REPXC", "GIRDER12_REINFORCEMENTPLATE_XC          "},
  {VarDesc::FloatCM, "G12REPXD", "GIRDER12_REINFORCEMENTPLATE_XD          "},
  {VarDesc::FloatCM, "G12REPYA", "GIRDER12_REINFORCEMENTPLATE_YA          "},
  {VarDesc::FloatCM, "G12REPYB", "GIRDER12_REINFORCEMENTPLATE_YB          "},
  {VarDesc::FloatCM, "G12REPYC", "GIRDER12_REINFORCEMENTPLATE_YC          "},
  {VarDesc::FloatCM, "G12REPYD", "GIRDER12_REINFORCEMENTPLATE_YD          "},
  {VarDesc::FloatCM, "G12REPZP", "GIRDER12_REINFORCEMENTPLATE_ZPOS        "},
  {VarDesc::FloatCM, "G12UMPYP", "GIRDER12_UPPERMINIPLATE_YPOS            "},
  {VarDesc::FloatCM, "G12UMPXP", "GIRDER12_UPPERMINIPLATE_XPOS            "},
  {VarDesc::FloatCM, "G12SMPYH", "GIRDER12_SIDEMINIPLATE_YHEIGHT          "},
  {VarDesc::FloatCM, "G12SMPYP", "GIRDER12_SIDEMINIPLATE_YPOS             "},
  {VarDesc::FloatCM, "G12SMPXP", "GIRDER12_SIDEMINIPLATE_XPOS             "},
  {VarDesc::FloatCM, "G12SMPZP", "GIRDER12_SIDEMINIPLATE_ZPOS             "},
  {VarDesc::FloatCM, "G23BOPXA", "GIRDER23_BOTTOMPLATE_XA                 "},
  {VarDesc::FloatCM, "G23BOPXB", "GIRDER23_BOTTOMPLATE_XB                 "},
  {VarDesc::FloatCM, "G23BOPXC", "GIRDER23_BOTTOMPLATE_XC                 "},
  {VarDesc::FloatCM, "G23BOPXD", "GIRDER23_BOTTOMPLATE_XD                 "},
  {VarDesc::FloatCM, "G23BOPXE", "GIRDER23_BOTTOMPLATE_XE                 "},
  {VarDesc::FloatCM, "G23BOPXF", "GIRDER23_BOTTOMPLATE_XF                 "},
  {VarDesc::FloatCM, "G23IPSX1", "GIRDER23_INNERPLATESMALL1_XPOS          "},
  {VarDesc::FloatCM, "G23IPLX1", "GIRDER23_INNERPLATELARGE1_XPOS          "},
  {VarDesc::FloatCM, "G23IPLX2", "GIRDER23_INNERPLATELARGE2_XPOS          "},
  {VarDesc::FloatCM, "G23IPSX2", "GIRDER23_INNERPLATESMALL2_XPOS          "},
  {VarDesc::FloatCM, "G23UPXWI", "GIRDER23_UPPERPLATE_XWIDTH              "},
  {VarDesc::FloatCM, "G23UPYHE", "GIRDER23_UPPERPLATE_YHEIGHT             "},
  {VarDesc::FloatCM, "G23REPXA", "GIRDER23_REINFORCEMENTPLATE_XA          "},
  {VarDesc::FloatCM, "G23REPXB", "GIRDER23_REINFORCEMENTPLATE_XB          "},
  {VarDesc::FloatCM, "G23REPXC", "GIRDER23_REINFORCEMENTPLATE_XC          "},
  {VarDesc::FloatCM, "G23REPXD", "GIRDER23_REINFORCEMENTPLATE_XD          "},
  {VarDesc::FloatCM, "G23REPYA", "GIRDER23_REINFORCEMENTPLATE_YA          "},
  {VarDesc::FloatCM, "G23REPYB", "GIRDER23_REINFORCEMENTPLATE_YB          "},
  {VarDesc::FloatCM, "G23REPYC", "GIRDER23_REINFORCEMENTPLATE_YC          "},
  {VarDesc::FloatCM, "G23REPYD", "GIRDER23_REINFORCEMENTPLATE_YD          "},
  {VarDesc::FloatCM, "G23UMPXP", "GIRDER23_UPPERMINIPLATE_XPOS            "},
  {VarDesc::FloatCM, "G23SMPXP", "GIRDER23_SIDEMINIPLATE_XPOS             "},
  {VarDesc::FloatCM, "G34BOPXA", "GIRDER34_BOTTOMPLATE_XA                 "},
  {VarDesc::FloatCM, "G34BOPXB", "GIRDER34_BOTTOMPLATE_XB                 "},
  {VarDesc::FloatCM, "G34BOPXC", "GIRDER34_BOTTOMPLATE_XC                 "},
  {VarDesc::FloatCM, "G34BOPXD", "GIRDER34_BOTTOMPLATE_XD                 "},
  {VarDesc::FloatCM, "G34BOPXE", "GIRDER34_BOTTOMPLATE_XE                 "},
  {VarDesc::FloatCM, "G34BOPXF", "GIRDER34_BOTTOMPLATE_XF                 "},
  {VarDesc::FloatCM, "G34IPSX1", "GIRDER34_INNERPLATESMALL1_XPOS          "},
  {VarDesc::FloatCM, "G34IPLX1", "GIRDER34_INNERPLATELARGE1_XPOS          "},
  {VarDesc::FloatCM, "G34IPLX2", "GIRDER34_INNERPLATELARGE2_XPOS          "},
  {VarDesc::FloatCM, "G34IPSX2", "GIRDER34_INNERPLATESMALL2_XPOS          "},
  {VarDesc::FloatCM, "G34UPXWI", "GIRDER34_UPPERPLATE_XWIDTH              "},
  {VarDesc::FloatCM, "G34UPYHE", "GIRDER34_UPPERPLATE_YHEIGHT             "},
  {VarDesc::FloatCM, "G34REPXA", "GIRDER34_REINFORCEMENTPLATE_XA          "},
  {VarDesc::FloatCM, "G34REPXB", "GIRDER34_REINFORCEMENTPLATE_XB          "},
  {VarDesc::FloatCM, "G34REPXC", "GIRDER34_REINFORCEMENTPLATE_XC          "},
  {VarDesc::FloatCM, "G34REPXD", "GIRDER34_REINFORCEMENTPLATE_XD          "},
  {VarDesc::FloatCM, "G34REPYA", "GIRDER34_REINFORCEMENTPLATE_YA          "},
  {VarDesc::FloatCM, "G34REPYB", "GIRDER34_REINFORCEMENTPLATE_YB          "},
  {VarDesc::FloatCM, "G34REPYC", "GIRDER34_REINFORCEMENTPLATE_YC          "},
  {VarDesc::FloatCM, "G34REPYD", "GIRDER34_REINFORCEMENTPLATE_YD          "},
  {VarDesc::FloatCM, "G34UMPXP", "GIRDER34_UPPERMINIPLATE_XPOS            "},
  {VarDesc::FloatCM, "G34SMPXP", "GIRDER34_SIDEMINIPLATE_XPOS             "},
  {VarDesc::FloatCM, "STDFVOYP", "STANDARDFOOTVOUSSOIR_YPOS               "},
  {VarDesc::FloatCM, "EXMPRCXC", "EXTREMITYMAINPLATEROUNDCUT_XC           "},
  {VarDesc::FloatCM, "EXMPRCYC", "EXTREMITYMAINPLATEROUNDCUT_YC           "},
  {VarDesc::FloatCM, "EXMPLXC3", "EXTREMITYMAINPLATE_XC3                  "},
  {VarDesc::FloatCM, "EXMPLYC3", "EXTREMITYMAINPLATE_YC3                  "},
  {VarDesc::FloatCM, "EXMPLXC4", "EXTREMITYMAINPLATE_XC4                  "},
  {VarDesc::FloatCM, "EXMPLYC4", "EXTREMITYMAINPLATE_YC4                  "},
  {VarDesc::FloatCM, "EXMPLXC5", "EXTREMITYMAINPLATE_XC5                  "},
  {VarDesc::FloatCM, "EXMPLYC5", "EXTREMITYMAINPLATE_YC5                  "},
  {VarDesc::FloatCM, "A2", "A2                                      "},
  {VarDesc::FloatCM, "B2", "B2                                      "},
  {VarDesc::FloatCM, "A2PRIME", "A2PRIME                                 "},
  {VarDesc::FloatCM, "B2PRIME", "B2PRIME                                 "},
  {VarDesc::FloatCM, "EXMPLAXD", "EXTREMITYMAINPLATE_XD                   "},
  {VarDesc::FloatCM, "EXMPLAYD", "EXTREMITYMAINPLATE_YD                   "},
  {VarDesc::FloatCM, "EXMPC2C1", "EXTREMITYMAINPLATE_LENGTHC2C1           "},
  {VarDesc::FloatCM, "EXMPLXC1", "EXTREMITYMAINPLATE_XC1                  "},
  {VarDesc::FloatCM, "EXMPLYC1", "EXTREMITYMAINPLATE_YC1                  "},
  {VarDesc::FloatCM, "EXMPYC1A", "EXTREMITYMAINPLATE_YC1A                 "},
  {VarDesc::FloatCM, "EXMPXC1A", "EXTREMITYMAINPLATE_XC1A                 "},
  {VarDesc::FloatCM, "EXMPXC1B", "EXTREMITYMAINPLATE_XC1B                 "},
  {VarDesc::FloatCM, "EXMPYC1B", "EXTREMITYMAINPLATE_YC1B                 "},
  {VarDesc::FloatCM, "ALPHA3", "ALPHA3                                  "},
  {VarDesc::FloatCM, "COAL3", "COAL3                                   "},
  {VarDesc::FloatCM, "SIAL3", "SIAL3                                   "},
  {VarDesc::FloatCM, "EXMCPDIA", "EXTREMITYMINICONNPLATE_DIAG             "},
  {VarDesc::FloatCM, "EXMCPXPO", "EXTREMITYMINICONNPLATE_X                "},
  {VarDesc::FloatCM, "EXMCPYPO", "EXTREMITYMINICONNPLATE_Y                "},
  {VarDesc::FloatCM, "EXMPLAXF", "EXTREMITYMAINPLATE_XF                   "},
  {VarDesc::FloatCM, "EXMPLAYF", "EXTREMITYMAINPLATE_YF                   "},
  {VarDesc::FloatCM, "EXMPLXF1", "EXTREMITYMAINPLATE_XF1                  "},
  {VarDesc::FloatCM, "EXMPLYF1", "EXTREMITYMAINPLATE_YF1                  "},
  {VarDesc::FloatCM, "EXMPLXF2", "EXTREMITYMAINPLATE_XF2                  "},
  {VarDesc::FloatCM, "EXMPLYF2", "EXTREMITYMAINPLATE_YF2                  "},
  {VarDesc::FloatCM, "EXMPLYF3", "EXTREMITYMAINPLATE_YF3                  "},
  {VarDesc::FloatCM, "EXMPLXF3", "EXTREMITYMAINPLATE_XF3                  "},
  {VarDesc::FloatCM, "EXMPLXF4", "EXTREMITYMAINPLATE_XF4                  "},
  {VarDesc::FloatCM, "EXMPLYF4", "EXTREMITYMAINPLATE_YF4                  "},
  {VarDesc::FloatCM, "EXMPLXF5", "EXTREMITYMAINPLATE_XF5                  "},
  {VarDesc::FloatCM, "EXMPLYF5", "EXTREMITYMAINPLATE_YF5                  "},
  {VarDesc::FloatCM, "EXMPLXF6", "EXTREMITYMAINPLATE_XF6                  "},
  {VarDesc::FloatCM, "EXMPLYF6", "EXTREMITYMAINPLATE_YF6                  "},
  {VarDesc::FloatCM, "EXMPLXF7", "EXTREMITYMAINPLATE_XF7                  "},
  {VarDesc::FloatCM, "EXMPLYF7", "EXTREMITYMAINPLATE_YF7                  "},
  {VarDesc::FloatCM, "EXMPLAXI", "EXTREMITYMAINPLATE_XI                   "},
  {VarDesc::FloatCM, "EXMPLAXH", "EXTREMITYMAINPLATE_XH                   "},
  {VarDesc::FloatCM, "EXMPLAYH", "EXTREMITYMAINPLATE_YH                   "},
  {VarDesc::FloatCM, "EXUCPXPO", "EXTREMITYUPPERCONNPLATE_X               "},
  {VarDesc::FloatCM, "EXUCPZLE", "EXTREMITYUPPERCONNPLATE_ZLENGTH         "},
  {VarDesc::FloatCM, "EXVCPZLE", "EXTREMITYVERTICALCONNPLATE_ZLENGTH      "},
  {VarDesc::FloatCM, "EXVCPLXA", "EXTREMITYVERTICALCONNPLATE_XA           "},
  {VarDesc::FloatCM, "EXVCPLYA", "EXTREMITYVERTICALCONNPLATE_YA           "},
  {VarDesc::FloatCM, "EXVCPLXB", "EXTREMITYVERTICALCONNPLATE_XB           "},
  {VarDesc::FloatCM, "EXVCPLYB", "EXTREMITYVERTICALCONNPLATE_YB           "},
  {VarDesc::FloatCM, "EXVCPLXC", "EXTREMITYVERTICALCONNPLATE_XC           "},
  {VarDesc::FloatCM, "EXVCPLYC", "EXTREMITYVERTICALCONNPLATE_YC           "},
  {VarDesc::FloatCM, "EXVCPLXD", "EXTREMITYVERTICALCONNPLATE_XD           "},
  {VarDesc::FloatCM, "EXVCPLYD", "EXTREMITYVERTICALCONNPLATE_YD           "},
  {VarDesc::FloatCM, "EXVCPLXE", "EXTREMITYVERTICALCONNPLATE_XE           "},
  {VarDesc::FloatCM, "EXVCPLYE", "EXTREMITYVERTICALCONNPLATE_YE           "},
  {VarDesc::FloatCM, "EXVCPLXF", "EXTREMITYVERTICALCONNPLATE_XF           "},
  {VarDesc::FloatCM, "EXVCPLYF", "EXTREMITYVERTICALCONNPLATE_YF           "},
  {VarDesc::FloatCM, "EXVCPLXP", "EXTREMITYVERTICALCONNPLATE_XPOS         "},
  {VarDesc::FloatCM, "EXVCPLYP", "EXTREMITYVERTICALCONNPLATE_YPOS         "},
  {VarDesc::FloatCM, "EXVCPLAX", "EXTREMITYVERTICALCONNPLATE_X            "},
  {VarDesc::FloatCM, "EXVCPLAY", "EXTREMITYVERTICALCONNPLATE_Y            "},
  {VarDesc::FloatCM, "EXCNFEVX", "EXTREMITYCONNFEETVOUSS_X                "},
  {VarDesc::FloatCM, "EXCNFEVY", "EXTREMITYCONNFEETVOUSS_Y                "},
  {VarDesc::FloatCM, "EXRSLZLE", "EXTREMITYRAILSUPPORTLOWER_ZLENGTH       "},
  {VarDesc::FloatCM, "EXRSVYHE", "EXTREMITYRAILSUPPORTVERTICAL_YHEIGHT    "},
  {VarDesc::FloatCM, "EXRSMXPO", "EXTREMITYRAILSUPPORTMIDDLE_XPOS         "},
  {VarDesc::FloatCM, "EXRSVXPO", "EXTREMITYRAILSUPPORTVERTICAL_XPOS       "},
  {VarDesc::FloatCM, "EXRSEYPO", "EXTREMITYRAILSUPPORTEXTR_Y              "},
  {VarDesc::FloatCM, "EXRSUYPO", "EXTREMITYRAILSUPPORTUPPER_Y             "},
  {VarDesc::FloatCM, "EXRSLYPO", "EXTREMITYRAILSUPPORTLOWER_Y             "},
  {VarDesc::FloatCM, "EXRSCYPO", "EXTREMITYRAILSUPPORTCENTRAL_Y           "},
  {VarDesc::FloatCM, "EXRSVYPO", "EXTREMITYRAILSUPPORTVERTICAL_Y          "},
  {VarDesc::FloatCM, "EXRSVZP1", "EXTREMITYRAILSUPPORTVERTICAL1_ZPOS      "},
  {VarDesc::FloatCM, "EXRSVZP2", "EXTREMITYRAILSUPPORTVERTICAL2_ZPOS      "},
  {VarDesc::FloatCM, "EXRSMZPO", "EXTREMITYRAILSUPPORTMIDDLE_ZPOS         "},
  {VarDesc::FloatCM, "EXRSEZPO", "EXTREMITYRAILSUPPORTEXTR_ZPOS           "},
  {VarDesc::FloatCM, "EXRSC1YE", "EXTREMITYRAILSUPPORTCUT1_YEXT           "},
  {VarDesc::FloatCM, "EXRSC2YE", "EXTREMITYRAILSUPPORTCUT2_YEXT           "},
  {VarDesc::FloatCM, "EXRSC1XP", "EXTREMITYRAILSUPPORTCUT1_XPOS           "},
  {VarDesc::FloatCM, "EXRSC2XP", "EXTREMITYRAILSUPPORTCUT2_XPOS           "},
  {VarDesc::FloatCM, "EXRSC1ZP", "EXTREMITYRAILSUPPORTCUT1_ZPOS           "},
  {VarDesc::FloatCM, "EXRSC3XP", "EXTREMITYRAILSUPPORTCUT3_XPOS           "},
  {VarDesc::FloatCM, "EXRSC3ZP", "EXTREMITYRAILSUPPORTCUT3_ZPOS           "},
  {VarDesc::FloatCM, "EXRSC4Z1", "EXRSC4Z1                                "},
  {VarDesc::FloatCM, "EXRSC4ZP", "EXTREMITYRAILSUPPORTCUT4_ZPOS           "},
  {VarDesc::FloatCM, "EXRSMCXA", "EXTREMITYRAILSUPPORTMIDDLECUT_XA        "},
  {VarDesc::FloatCM, "EXRSMCYA", "EXTREMITYRAILSUPPORTMIDDLECUT_YA        "},
  {VarDesc::FloatCM, "EXRSMCXB", "EXTREMITYRAILSUPPORTMIDDLECUT_XB        "},
  {VarDesc::FloatCM, "EXRSMCYB", "EXTREMITYRAILSUPPORTMIDDLECUT_YB        "},
  {VarDesc::FloatCM, "EXRSMCXC", "EXTREMITYRAILSUPPORTMIDDLECUT_XC        "},
  {VarDesc::FloatCM, "EXRSMCYC", "EXTREMITYRAILSUPPORTMIDDLECUT_YC        "},
  {VarDesc::FloatCM, "EXRSMCXP", "EXTREMITYRAILSUPPORTMIDDLE_CUT_XPOS     "},
  {VarDesc::FloatCM, "EXRSMCZ1", "EXRSMCZ1                                "},
  {VarDesc::FloatCM, "EXRSMCZP", "EXTREMITYRAILSUPPORTMIDDLE_CUT_ZPOS     "},
  {VarDesc::FloatCM, "EXRSPOSX", "EXTREMITYRAILSUPPORT_X                  "},
  {VarDesc::FloatCM, "EXRSPOSY", "EXTREMITYRAILSUPPORT_Y                  "},
  {VarDesc::FloatCM, "EXRSPOSZ", "EXTREMITYRAILSUPPORT_Z                  "},
  {VarDesc::FloatCM, "EXRSPXAP", "EXTREMITYRAILSUPPORT_ASSEMBLED_PLUS_X   "},
  {VarDesc::FloatCM, "EXRSPXAM", "EXTREMITYRAILSUPPORT_ASSEMBLED_MINUS_X  "},
  {VarDesc::FloatCM, "EXRSPYAS", "EXTREMITYRAILSUPPORT_ASSEMBLED_Y        "},
  {VarDesc::FloatCM, "GIR12ZPO", "GIR12ZPO                                "},
  {VarDesc::FloatCM, "GIR12YPO", "GIR12YPO                                "},
  {VarDesc::FloatCM, "GIR23ZPO", "GIR23ZPO                                "},
  {VarDesc::FloatCM, "GIR34ZPO", "GIR34ZPO                                "},
  };


VarDesc GENV_Vars[] = {
  {VarDesc::FloatCM, "PI",     "PI     "},
  {VarDesc::FloatCM, "EPS",    "EPS    "},
  {VarDesc::FloatCM, "PIS180", "PIS180 "},
  {VarDesc::FloatCM, "SI5",    "SI5    "},
  {VarDesc::FloatCM, "CO5",    "CO5    "},
  {VarDesc::FloatCM, "TA15",   "TA15   "},
  {VarDesc::FloatCM, "TA20",   "TA20   "},
  {VarDesc::FloatCM, "SI225",  "SI225  "},
  {VarDesc::FloatCM, "CO225",  "CO225  "},
  {VarDesc::FloatCM, "TA225",  "TA225  "},
  {VarDesc::FloatCM, "TA375",  "TA375  "},
  {VarDesc::FloatCM, "SI45",   "SI45   "},
  {VarDesc::FloatCM, "CO45",   "CO45   "},
  {VarDesc::FloatCM, "TA45",   "TA45   "},
  {VarDesc::FloatCM, "SI525",  "SI525  "},
  {VarDesc::FloatCM, "CO525",  "CO525  "},
  {VarDesc::FloatCM, "TA55",   "TA55   "},
  {VarDesc::FloatCM, "SI675",  "SI675  "},
  {VarDesc::FloatCM, "CO675",  "CO675  "},
  {VarDesc::FloatCM, "TA675",  "TA675  "},
  {VarDesc::FloatCM, "TA77",   "TA77   "},
};

VarDesc JDSH_Vars[] = {
  {VarDesc::FloatCM, "XYZREFPT", "XYZREFERENCEPOINT         "},
  {VarDesc::FloatCM, "ROTA180D", "180DEGROTATION            "},
  {VarDesc::FloatCM, "NOMIZPOS", "NOMINAL_ZPOS              "},
  {VarDesc::FloatCM, "FDINNRAD", "FRONTDISK_INNERRADIUS     "},
  {VarDesc::FloatCM, "FDOUTRAD", "FRONTDISK_OUTERRADIUS     "},
  {VarDesc::FloatCM, "FDTHICKN", "FRONTDISK_THICKNESS       "},
  {VarDesc::FloatCM, "BDINNRAD", "BACKDISK_INNERRADIUS      "},
  {VarDesc::FloatCM, "BDOUTRAD", "BACKDISK_OUTERRADIUS      "},
  {VarDesc::FloatCM, "BDTHICKN", "BACKDISK_THICKNESS        "},
  {VarDesc::FloatCM, "CABCINRA", "BRASSCONE_INNERRADIUS     "},
  {VarDesc::FloatCM, "CABCOURA", "BRASSCONE_OUTERRADIUS     "},
  {VarDesc::FloatCM, "CABCTHI1", "BRASSCONE_THICKNESS_INTER1"},
  {VarDesc::FloatCM, "CABCTHIC", "BRASSCONE_THICKNESS       "},
  {VarDesc::FloatCM, "CABCSLAN", "BRASSCONE_SLOPEANGLE      "},
  {VarDesc::FloatCM, "CABCSLLE", "BRASSCONE_SLOPELENGTH     "},
  {VarDesc::FloatCM, "CALCTHIC", "LEADCONE_THICKNESS        "},
  {VarDesc::FloatCM, "CALCLENG", "LEADCONE_LENGTH           "},
  {VarDesc::FloatCM, "CALCWIDT", "LEADCONE_WIDTH            "},
  {VarDesc::FloatCM, "CAPCTHIC", "POLYCONE_THICKNESS        "},
  {VarDesc::FloatCM, "CAPCLENG", "POLYCONE_LENGTH           "},
  {VarDesc::FloatCM, "CAPCWIDT", "POLYCONE_WIDTH            "},
  {VarDesc::FloatCM, "CACOPRAN", "CONE_PROJECTIVEANGLE      "},
  {VarDesc::FloatCM, "CACOANGZ_0", "ARRAY                     "},
  {VarDesc::FloatCM, "CACOANGZ_1", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_2", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_3", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_4", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_5", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_6", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_7", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_8", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_9", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_10", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_11", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_12", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_13", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_14", "                          "},
  {VarDesc::FloatCM, "CACOANGZ_15", "                          "},
  {VarDesc::FloatCM, "CACOANGX", "CONEASSEMBLY_ANGX         "},
  {VarDesc::FloatCM, "TUBEINRA", "TUBE_INNERRADIUS          "},
  {VarDesc::FloatCM, "TUBEOUR1", "TUBE_OUTERRADIUS1         "},
  {VarDesc::FloatCM, "TUBEOUR2", "TUBE_OUTERRADIUS2         "},
  {VarDesc::FloatCM, "TUBESLEN", "TUBE_SHORTLENGTH          "},
  {VarDesc::FloatCM, "TUBELENG", "TUBE_LENGTH               "},
  {VarDesc::FloatCM, "PLUGINR1", "PLUG_INNERRADIUS1         "},
  {VarDesc::FloatCM, "PLUGOURA", "PLUG_OUTERRADIUS          "},
  {VarDesc::FloatCM, "PLUGLENG", "PLUG_LENGTH               "},
  {VarDesc::FloatCM, "PLUGTIAN", "PLUG_TILTANGLE            "},
  {VarDesc::FloatCM, "SWHINNRA", "SMALLWHEELHUB_INNERRADIUS "},
  {VarDesc::FloatCM, "SWHOUTR1", "SMALLWHEELHUB_OUTERRADIUS1"},
  {VarDesc::FloatCM, "SWHOUTR2", "SMALLWHEELHUB_OUTERRADIUS2"},
  {VarDesc::FloatCM, "SWHLENGT", "SMALLWHEELHUB_LENGTH      "},
  {VarDesc::FloatCM, "FDZPOSIT", "FRONTDISK_Z               "},
  {VarDesc::FloatCM, "BDZPOSIT", "BACKDISK_Z                "},
  {VarDesc::FloatCM, "CAZPOSIT", "CONEASSEMBLY_Z            "},
  {VarDesc::FloatCM, "TUBEZPOS", "TUBE_Z                    "},
  {VarDesc::FloatCM, "PLUGZPOS", "PLUG_Z                    "},
  {VarDesc::FloatCM, "SWHZPOSI", "SMALLWHEELHUB_Z           "},
  {VarDesc::FloatCM, "CABCOUR1", "BRASSCONE_OUTERRADIUS1    "},
  {VarDesc::FloatCM, "CABCTHI2", "BRASSCONE_THICKNESS2      "},
  {VarDesc::FloatCM, "CABCOUR2", "BRASSCONE_OUTERRADIUS2    "},
  {VarDesc::FloatCM, "LCAXBPOS", "LEADCONEASSEMBLY_XB       "},
  {VarDesc::FloatCM, "LCAYBPOS", "LEADCONEASSEMBLY_YB       "},
  {VarDesc::FloatCM, "LCAXAPOS", "LEADCONEASSEMBLY_XA       "},
  {VarDesc::FloatCM, "LCAYAPOS", "LEADCONEASSEMBLY_YA       "},
  {VarDesc::FloatCM, "PCAXBPOS", "POLYCONEASSEMBLY_XB       "},
  {VarDesc::FloatCM, "PCAYBPOS", "POLYCONEASSEMBLY_YB       "},
  {VarDesc::FloatCM, "PCAXAPOS", "POLYCONEASSEMBLY_XA       "},
  {VarDesc::FloatCM, "PCAYAPOS", "POLYCONEASSEMBLY_YA       "},
  {VarDesc::FloatCM, "CALCRADI", "LEADCONE_RADIUS           "},
  {VarDesc::FloatCM, "CALCPOSZ", "LEADCONE_POSZ             "},
  {VarDesc::FloatCM, "CAPCRADI", "POLYCONE_RADIUS           "},
  {VarDesc::FloatCM, "CAPCPOSZ", "POLYCONE_POSZ             "},
  {VarDesc::FloatCM, "I1", "I1                        "},
  {VarDesc::FloatCM, "LCAPX_0", "ARRAY                     "},
  {VarDesc::FloatCM, "LCAPX_1", "                          "},
  {VarDesc::FloatCM, "LCAPX_2", "                          "},
  {VarDesc::FloatCM, "LCAPX_3", "                          "},
  {VarDesc::FloatCM, "LCAPX_4", "                          "},
  {VarDesc::FloatCM, "LCAPX_5", "                          "},
  {VarDesc::FloatCM, "LCAPX_6", "                          "},
  {VarDesc::FloatCM, "LCAPX_7", "                          "},
  {VarDesc::FloatCM, "LCAPX_8", "                          "},
  {VarDesc::FloatCM, "LCAPX_9", "                          "},
  {VarDesc::FloatCM, "LCAPX_10", "                          "},
  {VarDesc::FloatCM, "LCAPX_11", "                          "},
  {VarDesc::FloatCM, "LCAPX_12", "                          "},
  {VarDesc::FloatCM, "LCAPX_13", "                          "},
  {VarDesc::FloatCM, "LCAPX_14", "                          "},
  {VarDesc::FloatCM, "LCAPX_15", "                          "},
  {VarDesc::FloatCM, "LCAPY_0", "ARRAY                     "},
  {VarDesc::FloatCM, "LCAPY_1", "                          "},
  {VarDesc::FloatCM, "LCAPY_2", "                          "},
  {VarDesc::FloatCM, "LCAPY_3", "                          "},
  {VarDesc::FloatCM, "LCAPY_4", "                          "},
  {VarDesc::FloatCM, "LCAPY_5", "                          "},
  {VarDesc::FloatCM, "LCAPY_6", "                          "},
  {VarDesc::FloatCM, "LCAPY_7", "                          "},
  {VarDesc::FloatCM, "LCAPY_8", "                          "},
  {VarDesc::FloatCM, "LCAPY_9", "                          "},
  {VarDesc::FloatCM, "LCAPY_10", "                          "},
  {VarDesc::FloatCM, "LCAPY_11", "                          "},
  {VarDesc::FloatCM, "LCAPY_12", "                          "},
  {VarDesc::FloatCM, "LCAPY_13", "                          "},
  {VarDesc::FloatCM, "LCAPY_14", "                          "},
  {VarDesc::FloatCM, "LCAPY_15", "                          "},
  {VarDesc::FloatCM, "I2", "I2                        "},
  {VarDesc::FloatCM, "PCAPX_0", "ARRAY                     "},
  {VarDesc::FloatCM, "PCAPX_1", "                          "},
  {VarDesc::FloatCM, "PCAPX_2", "                          "},
  {VarDesc::FloatCM, "PCAPX_3", "                          "},
  {VarDesc::FloatCM, "PCAPX_4", "                          "},
  {VarDesc::FloatCM, "PCAPX_5", "                          "},
  {VarDesc::FloatCM, "PCAPX_6", "                          "},
  {VarDesc::FloatCM, "PCAPX_7", "                          "},
  {VarDesc::FloatCM, "PCAPX_8", "                          "},
  {VarDesc::FloatCM, "PCAPX_9", "                          "},
  {VarDesc::FloatCM, "PCAPX_10", "                          "},
  {VarDesc::FloatCM, "PCAPX_11", "                          "},
  {VarDesc::FloatCM, "PCAPX_12", "                          "},
  {VarDesc::FloatCM, "PCAPX_13", "                          "},
  {VarDesc::FloatCM, "PCAPX_14", "                          "},
  {VarDesc::FloatCM, "PCAPX_15", "                          "},
  {VarDesc::FloatCM, "PCAPY_0", "ARRAY                     "},
  {VarDesc::FloatCM, "PCAPY_1", "                          "},
  {VarDesc::FloatCM, "PCAPY_2", "                          "},
  {VarDesc::FloatCM, "PCAPY_3", "                          "},
  {VarDesc::FloatCM, "PCAPY_4", "                          "},
  {VarDesc::FloatCM, "PCAPY_5", "                          "},
  {VarDesc::FloatCM, "PCAPY_6", "                          "},
  {VarDesc::FloatCM, "PCAPY_7", "                          "},
  {VarDesc::FloatCM, "PCAPY_8", "                          "},
  {VarDesc::FloatCM, "PCAPY_9", "                          "},
  {VarDesc::FloatCM, "PCAPY_10", "                          "},
  {VarDesc::FloatCM, "PCAPY_11", "                          "},
  {VarDesc::FloatCM, "PCAPY_12", "                          "},
  {VarDesc::FloatCM, "PCAPY_13", "                          "},
  {VarDesc::FloatCM, "PCAPY_14", "                          "},
  {VarDesc::FloatCM, "PCAPY_15", "                          "},
  {VarDesc::FloatCM, "PLUGINR2", "PLUG_INNERRADIUS2         "},
};


VarDesc JFSH_Vars[] = {
  {VarDesc::FloatCM, "XYZREF", "XYZREF                                      "},
  {VarDesc::FloatCM, "ROT180", "ROT180                                      "},
  {VarDesc::FloatCM, "AFBBLENG", "AFRAME_BOTTOMBAR_LENGTH                     "},
  {VarDesc::FloatCM, "AFBHEIGH", "AFRAME_BAR_HEIGHT                           "},
  {VarDesc::FloatCM, "AFRWIDTH", "AFRAME_WIDTH                                "},
  {VarDesc::FloatCM, "AFBTHIC1", "AFRAME_BAR_THICKNESS1                       "},
  {VarDesc::FloatCM, "AFBTHIC2", "AFRAME_BAR_THICKNESS2                       "},
  {VarDesc::FloatCM, "AFLANGLE", "AFRAME_LEG_ANGLE                            "},
  {VarDesc::FloatCM, "AFSLENGT", "AFRAME_SPACER_LENGTH                        "},
  {VarDesc::FloatCM, "AFSHEIGH", "AFRAME_SPACER_HEIGHT                        "},
  {VarDesc::FloatCM, "AFWEBBX1", "AFRAME_WEB_ELEMENT_BOTTOMBAR_X1             "},
  {VarDesc::FloatCM, "AFWEBBX2", "AFRAME_WEB_ELEMENT_BOTTOMBAR_X2             "},
  {VarDesc::FloatCM, "AFWELENG", "AFRAME_WEB_ELEMENT_LENGTH                   "},
  {VarDesc::FloatCM, "AFWEBLX1", "AFRAME_WEB_ELEMENT_LEG_DX1                  "},
  {VarDesc::FloatCM, "AFWEBLX2", "AFRAME_WEB_ELEMENT_LEG_DX2                  "},
  {VarDesc::FloatCM, "AFFTHICK", "AFRAME_FLANGE_THICKNESS                     "},
  {VarDesc::FloatCM, "AFFLENGT", "AFRAME_FLANGE_LENGTH                        "},
  {VarDesc::FloatCM, "AFTPLENG", "AFRAME_TOPPLATE_LENGTH                      "},
  {VarDesc::FloatCM, "AFTPHEIG", "AFRAME_TOPPLATE_HEIGHT                      "},
  {VarDesc::FloatCM, "AFWHEIGH", "AFRAME_WEDGE_HEIGHT                         "},
  {VarDesc::FloatCM, "AFWEXTEN", "AFRAME_WEDGE_EXTENSION                      "},
  {VarDesc::FloatCM, "AFWTHICK", "AFRAME_WEDGE_THICKNESS                      "},
  {VarDesc::FloatCM, "AFWZOFFS", "AFRAME_WEDGE_ZOFFSET                        "},
  {VarDesc::FloatCM, "AFFEELEN", "AFRAME_FEET_LENGTH                          "},
  {VarDesc::FloatCM, "AFFEETHI", "AFRAME_FEET_THICKNESS                       "},
  {VarDesc::FloatCM, "AFFEESLE", "AFRAME_FEET_SHORTLENGTH                     "},
  {VarDesc::FloatCM, "AFFEEHEI", "AFRAME_FEET_HEIGHT                          "},
  {VarDesc::FloatCM, "AFFEEEXT", "AFRAME_FEET_X_EXTENSION                     "},
  {VarDesc::FloatCM, "AFTOTHEI", "AFRAME_TOTAL_HEIGHT                         "},
  {VarDesc::FloatCM, "AFECTCLE", "CLEARANCE_ECT_AFRAME                        "},
  {VarDesc::FloatCM, "ECTNOMIZ", "ECT_NOMINAL_ZPOS                            "},
  {VarDesc::FloatCM, "AFYPOSIT", "AFRAME_YPOS                                 "},
  {VarDesc::FloatCM, "AFZPOSIT", "AFRAME_ZPOS                                 "},
  {VarDesc::FloatCM, "AFBLHPXA", "AFBLHPXA                                    "},
  {VarDesc::FloatCM, "AFBLHPYA", "AFBLHPYA                                    "},
  {VarDesc::FloatCM, "AFBLHPYB", "AFBLHPYB                                    "},
  {VarDesc::FloatCM, "AFBLHPXB", "AFBLHPXB                                    "},
  {VarDesc::FloatCM, "AFBBVPXA", "AFBBVPXA                                    "},
  {VarDesc::FloatCM, "AFBBVPYA", "AFBBVPYA                                    "},
  {VarDesc::FloatCM, "AFBBVPYB", "AFBBVPYB                                    "},
  {VarDesc::FloatCM, "AFBBVPXB", "AFBBVPXB                                    "},
  {VarDesc::FloatCM, "AFBUHPXA", "AFBUHPXA                                    "},
  {VarDesc::FloatCM, "AFBUHPYA", "AFBUHPYA                                    "},
  {VarDesc::FloatCM, "AFBUHPYB", "AFBUHPYB                                    "},
  {VarDesc::FloatCM, "AFBUHPXB", "AFBUHPXB                                    "},
  {VarDesc::FloatCM, "AFLLHPXA", "AFLLHPXA                                    "},
  {VarDesc::FloatCM, "AFLLHPYA", "AFLLHPYA                                    "},
  {VarDesc::FloatCM, "AFLLHPXB", "AFLLHPXB                                    "},
  {VarDesc::FloatCM, "AFLLHPYB", "AFLLHPYB                                    "},
  {VarDesc::FloatCM, "AFLLHPXC", "AFLLHPXC                                    "},
  {VarDesc::FloatCM, "AFLLHPYC", "AFLLHPYC                                    "},
  {VarDesc::FloatCM, "AFLLHPXD", "AFLLHPXD                                    "},
  {VarDesc::FloatCM, "AFLLHPYD", "AFLLHPYD                                    "},
  {VarDesc::FloatCM, "AFLVPXA", "AFLVPXA                                     "},
  {VarDesc::FloatCM, "AFLVPYA", "AFLVPYA                                     "},
  {VarDesc::FloatCM, "AFLVPXB", "AFLVPXB                                     "},
  {VarDesc::FloatCM, "AFLVPYB", "AFLVPYB                                     "},
  {VarDesc::FloatCM, "AFLVPXC", "AFLVPXC                                     "},
  {VarDesc::FloatCM, "AFLVPYC", "AFLVPYC                                     "},
  {VarDesc::FloatCM, "AFLVPXE", "AFLVPXE                                     "},
  {VarDesc::FloatCM, "AFLVPYE", "AFLVPYE                                     "},
  {VarDesc::FloatCM, "AFLVPYD", "AFLVPYD                                     "},
  {VarDesc::FloatCM, "AFLVPXD", "AFLVPXD                                     "},
  {VarDesc::FloatCM, "AFLUHPXA", "AFLUHPXA                                    "},
  {VarDesc::FloatCM, "AFLUHPYA", "AFLUHPYA                                    "},
  {VarDesc::FloatCM, "AFLUHPXB", "AFLUHPXB                                    "},
  {VarDesc::FloatCM, "AFLUHPYB", "AFLUHPYB                                    "},
  {VarDesc::FloatCM, "AFLUHPXC", "AFLUHPXC                                    "},
  {VarDesc::FloatCM, "AFLUHPYC", "AFLUHPYC                                    "},
  {VarDesc::FloatCM, "AFLUHPXD", "AFLUHPXD                                    "},
  {VarDesc::FloatCM, "AFLUHPYD", "AFLUHPYD                                    "},
  {VarDesc::FloatCM, "AFTPLAXA", "AFTPLAXA                                    "},
  {VarDesc::FloatCM, "AFTPLAYA", "AFTPLAYA                                    "},
  {VarDesc::FloatCM, "AFTPLAXB", "AFTPLAXB                                    "},
  {VarDesc::FloatCM, "AFTPLAYB", "AFTPLAYB                                    "},
  {VarDesc::FloatCM, "AFSPACXA", "AFSPACXA                                    "},
  {VarDesc::FloatCM, "AFSPACYA", "AFSPACYA                                    "},
  {VarDesc::FloatCM, "AFSPACXB", "AFSPACXB                                    "},
  {VarDesc::FloatCM, "AFSPACYB", "AFSPACYB                                    "},
  {VarDesc::FloatCM, "AFWEBWID", "AFRAME_WEB_ELEMENT_WIDTH                    "},
  {VarDesc::FloatCM, "AFWEBHEI", "AFRAME_WEB_ELEMENT_HEIGHT                   "},
  {VarDesc::FloatCM, "AFWEBBBY", "AFRAME_WEB_ELEMENT_BOTTOMBAR_Y              "},
  {VarDesc::FloatCM, "AFWEBZPO", "AFRAME_WEB_ELEMENT_Z                        "},
  {VarDesc::FloatCM, "AFWELXI1", "AFWELXI1                                    "},
  {VarDesc::FloatCM, "AFWELYI1", "AFWELYI1                                    "},
  {VarDesc::FloatCM, "AFWELXP1", "AFWELXP1                                    "},
  {VarDesc::FloatCM, "AFWELYP1", "AFWELYP1                                    "},
  {VarDesc::FloatCM, "AFWELXI2", "AFWELXI2                                    "},
  {VarDesc::FloatCM, "AFWELYI2", "AFWELYI2                                    "},
  {VarDesc::FloatCM, "AFWELXP2", "AFWELXP2                                    "},
  {VarDesc::FloatCM, "AFWELYP2", "AFWELYP2                                    "},
  {VarDesc::FloatCM, "AFWELXI3", "AFWELXI3                                    "},
  {VarDesc::FloatCM, "AFWELYI3", "AFWELYI3                                    "},
  {VarDesc::FloatCM, "AFWELXP3", "AFWELXP3                                    "},
  {VarDesc::FloatCM, "AFWELYP3", "AFWELYP3                                    "},
  {VarDesc::FloatCM, "AFFWIDTH", "AFRAME_FLANGE_WIDTH                         "},
  {VarDesc::FloatCM, "AFFZPOSI", "AFRAME_FLANGE_Z                             "},
  {VarDesc::FloatCM, "AFFPOSX1", "AFRAME_FLANGE_X1                            "},
  {VarDesc::FloatCM, "AFFPOSY1", "AFRAME_FLANGE_Y1                            "},
  {VarDesc::FloatCM, "AFFPOSX2", "AFRAME_FLANGE_X2                            "},
  {VarDesc::FloatCM, "AFFPOSY2", "AFRAME_FLANGE_Y2                            "},
  {VarDesc::FloatCM, "AFWPOSXA", "AFRAME_WEDGE_XA                             "},
  {VarDesc::FloatCM, "AFWPOSYA", "AFRAME_WEDGE_YA                             "},
  {VarDesc::FloatCM, "AFWPOSXB", "AFRAME_WEDGE_XB                             "},
  {VarDesc::FloatCM, "AFWPOSYB", "AFRAME_WEDGE_YB                             "},
  {VarDesc::FloatCM, "AFWPOSXC", "AFRAME_WEDGE_XC                             "},
  {VarDesc::FloatCM, "AFWPOSYC", "AFRAME_WEDGE_YC                             "},
  {VarDesc::FloatCM, "AFWPOSXD", "AFRAME_WEDGE_XD                             "},
  {VarDesc::FloatCM, "AFWPOSYD", "AFRAME_WEDGE_YD                             "},
  {VarDesc::FloatCM, "AFWPOSX0", "AFRAME_WEDGE_X0                             "},
  {VarDesc::FloatCM, "AFWPOSY0", "AFRAME_WEDGE_Y0                             "},
  {VarDesc::FloatCM, "AFWPOSIX", "AFRAME_WEDGE_X                              "},
  {VarDesc::FloatCM, "AFWPOSIY", "AFRAME_WEDGE_Y                              "},
  {VarDesc::FloatCM, "AFWPOSIZ", "AFRAME_WEDGE_Z                              "},
  {VarDesc::FloatCM, "AFFEETXA", "AFRAME_FEET_XA                              "},
  {VarDesc::FloatCM, "AFFEETYA", "AFRAME_FEET_YA                              "},
  {VarDesc::FloatCM, "AFFEETXB", "AFRAME_FEET_XB                              "},
  {VarDesc::FloatCM, "AFFEETYB", "AFRAME_FEET_YB                              "},
  {VarDesc::FloatCM, "AFFEETXC", "AFRAME_FEET_XC                              "},
  {VarDesc::FloatCM, "AFFEETYC", "AFRAME_FEET_YC                              "},
  {VarDesc::FloatCM, "AFFEETXD", "AFRAME_FEET_XD                              "},
  {VarDesc::FloatCM, "AFFEETYD", "AFRAME_FEET_YD                              "},
  {VarDesc::FloatCM, "AFFEETXE", "AFRAME_FEET_XE                              "},
  {VarDesc::FloatCM, "AFFEETYE", "AFRAME_FEET_YE                              "},
  {VarDesc::FloatCM, "AFFEETXF", "AFRAME_FEET_XF                              "},
  {VarDesc::FloatCM, "AFFEETYF", "AFRAME_FEET_YF                              "},
  {VarDesc::FloatCM, "AFFEETPX", "AFRAME_FEET_X                               "},
  {VarDesc::FloatCM, "AFASPOSY", "AFRAME_ASSEMBLY_Y                           "},
  {VarDesc::FloatCM, "AFASPOSZ", "AFRAME_ASSEMBLY_Z                           "},
  {VarDesc::FloatCM, "JFCMCLEN", "JFCMAINCYLINDER_LENGTH                      "},
  {VarDesc::FloatCM, "JFCMCIR1", "JFCMAINCYLINDER_INNERRADIUS1                "},
  {VarDesc::FloatCM, "JFCMCOUR", "JFCMAINCYLINDER_OUTERRADIUS                 "},
  {VarDesc::FloatCM, "JFCMCHSL", "JFCMAINCYLINDER_HOLESLOPE                   "},
  {VarDesc::FloatCM, "JFCMCZOF", "JFCMAINCYLINDER_ZOFFSET                     "},
  {VarDesc::FloatCM, "PLUGLENG", "PLUG_LENGTH                                 "},
  {VarDesc::FloatCM, "PLUGINRA", "PLUG_INNERRADIUS                            "},
  {VarDesc::FloatCM, "PLUGOURA", "PLUG_OUTERRADIUS                            "},
  {VarDesc::FloatCM, "JFSOCMRA", "JFSOCTOGON_MAINRADIUS                       "},
  {VarDesc::FloatCM, "JFSOCLEN", "JFSOCTOGON_LENGTH                           "},
  {VarDesc::FloatCM, "JFSOCZOF", "JFSOCTOGON_ZOFFSET                          "},
  {VarDesc::FloatCM, "AFOCONLE", "AFRAME_TO_JFSOCTOGON_CONNECTION_LENGTH      "},
  {VarDesc::FloatCM, "AFOCONWI", "AFRAME_TO_JFSOCTOGON_CONNECTION_WIDTH       "},
  {VarDesc::FloatCM, "AFOCONHE", "AFRAME_TO_JFSOCTOGON_CONNECTION_HEIGHT      "},
  {VarDesc::FloatCM, "TX1E1ORA", "TX1E1ORA                                    "},
  {VarDesc::FloatCM, "TX1E1IRA", "TX1E1IRA                                    "},
  {VarDesc::FloatCM, "TX1E1ILE", "TX1E1ILE                                    "},
  {VarDesc::FloatCM, "TX1E2ORA", "TX1E2ORA                                    "},
  {VarDesc::FloatCM, "TX1E2IRA", "TX1E2IRA                                    "},
  {VarDesc::FloatCM, "TX1E2ILE", "TX1E2ILE                                    "},
  {VarDesc::FloatCM, "TX1STZOF", "TX1STZOF                                    "},
  {VarDesc::FloatCM, "JFCMCIR2", "JFCMAINCYLINDER_INNERRADIUS2                "},
  {VarDesc::FloatCM, "JFCMCZPO", "JFCMAINCYLINDER_Z                           "},
  {VarDesc::FloatCM, "JFSOCZPO", "JFSOCTOGON_Z                                "},
  {VarDesc::FloatCM, "TX1E1ZPO", "TX1STM_ELEMENT1_Z                           "},
  {VarDesc::FloatCM, "TX1E2ZPO", "TX1STM_ELEMENT2_Z                           "},
  {VarDesc::FloatCM, "PLUGZPOS", "PLUG_POSZ                                   "},
  {VarDesc::FloatCM, "AFMCCONY", "AFRAME_TO_JFCMAINCYLINDER_CONNECTION_Y      "},
  {VarDesc::FloatCM, "AFMCCONZ", "AFRAME_TO_JFCMAINCYLINDER_CONNECTION_Z      "},
  {VarDesc::FloatCM, "JFSOCTXA", "JFSOCTOGON_XA                               "},
  {VarDesc::FloatCM, "JFSOCTYA", "JFSOCTOGON_YA                               "},
  {VarDesc::FloatCM, "JFSOCTXB", "JFSOCTOGON_XB                               "},
  {VarDesc::FloatCM, "JFSOCTYB", "JFSOCTOGON_YB                               "},
};


VarDesc JTSH_Vars[] = {
  {VarDesc::FloatCM, "OPLENGTH", "OUTERPLUGS_LENGTH            "},
  {VarDesc::FloatCM, "OPINNRAD", "OUTERPLUGS_INNERRADIUS       "},
  {VarDesc::FloatCM, "OPOUTRAD", "OUTERPLUGS_OUTERRADIUS       "},
  {VarDesc::FloatCM, "IPLENGTH", "INNERPLUGS_LENGTH            "},
  {VarDesc::FloatCM, "IPINNRA1", "INNERPLUGS_INNERRADIUS1      "},
  {VarDesc::FloatCM, "IPOUTRAD", "INNERPLUGS_OUTERRADIUS       "},
  {VarDesc::FloatCM, "IPTILTAN", "INNERPLUGS_TILTANGLE         "},
  {VarDesc::FloatCM, "IPZOFFSE", "INNERPLUGS_ZOFFSET           "},
  {VarDesc::FloatCM, "PRLENGTH", "POLYRINGS_LENGTH             "},
  {VarDesc::FloatCM, "PRINNRAD", "POLYRINGS_INNERRADIUS        "},
  {VarDesc::FloatCM, "PROUTRAD", "POLYRINGS_OUTERRADIUS        "},
  {VarDesc::FloatCM, "PREXTTAN", "POLYRINGS_EXTENSION_TILTANGLE"},
  {VarDesc::FloatCM, "PRIPEXLE", "POLYRINGS_IPEXTENSION_LENGTH "},
  {VarDesc::FloatCM, "ZPOSITIO", "SHIELD_Z                     "},
  {VarDesc::FloatCM, "YROTATIO", "SHIELD_ROTY                  "},
  {VarDesc::FloatCM, "IPINNRA2", "INNERPLUGS_INNERRADIUS2      "},
  {VarDesc::FloatCM, "PREXTZPO", "POLYRINGS_EXTENSION_Z        "},
  {VarDesc::FloatCM, "OPZPOSI1", "OUTERPLUGS_Z1                "},
  {VarDesc::FloatCM, "OPZPOSI2", "OUTERPLUGS_Z2                "},
  {VarDesc::FloatCM, "IPZPOSI1", "INNERPLUGS_Z1                "},
  {VarDesc::FloatCM, "IPZPOSI2", "INNERPLUGS_Z2                "},
  {VarDesc::FloatCM, "PRZPOSI1", "POLYRINGS_Z1                 "},
  {VarDesc::FloatCM, "PRZPOSI2", "POLYRINGS_Z2                 "},
  {VarDesc::FloatCM, "PRZPOSI3", "POLYRINGS_Z3                 "},
};


VarDesc MBAP_Vars[] = {
  {VarDesc::FloatCM, "P12S1DZ1", "1353 P12S1DZ1  "},
  {VarDesc::FloatCM, "P12S1DZ2", "1354 P12S1DZ2  "},
  {VarDesc::FloatCM, "P23S1DZ1", "1355 P23S1DZ1  "},
  {VarDesc::FloatCM, "P23S1DZ2", "1356 P23S1DZ2  "},
  {VarDesc::FloatCM, "P34S1DZ1", "1357 P34S1DZ1  "},
  {VarDesc::FloatCM, "P34S1DZ2", "1358 P34S1DZ2  "},
  {VarDesc::FloatCM, "P14S3DZ", "1359 P14S3DZ   "},
  {VarDesc::FloatCM, "P14S3DX", "1360 P14S3DX   "},
  {VarDesc::FloatCM, "PLAS1DX1", "1361 PLAS1DX1  "},
  {VarDesc::FloatCM, "PLAS1DX2", "1362 PLAS1DX2  "},
  {VarDesc::FloatCM, "PFLHE", "1363 PFLHE     "},
  {VarDesc::FloatCM, "PS01POSX", "1364 PS01POSX  "},
  {VarDesc::FloatCM, "PS01POSY", "1365 PS01POSY  "},
  {VarDesc::FloatCM, "PS11POSX", "1366 PS11POSX  "},
  {VarDesc::FloatCM, "PS11POSY", "1367 PS11POSY  "},
  {VarDesc::FloatCM, "PS14POSX", "1368 PS14POSX  "},
  {VarDesc::FloatCM, "PS14POSY", "1369 PS14POSY  "},
  {VarDesc::FloatCM, "PS14ANGZ", "1370 PS14ANGZ  "},
  {VarDesc::FloatCM, "P12POSZ", "1371 P12POSZ   "},
  {VarDesc::FloatCM, "P23POSZ", "1372 P23POSZ   "},
  {VarDesc::FloatCM, "P34POSZ", "1373 P34POSZ   "},
  {VarDesc::FloatCM, "XYZREF", "1374 XYZREF    "},
  {VarDesc::FloatCM, "PFL12XA", "1375 PFL12XA   "},
  {VarDesc::FloatCM, "PFL23XA", "1376 PFL23XA   "},
  {VarDesc::FloatCM, "PFL34XA", "1377 PFL34XA   "},
  {VarDesc::FloatCM, "PFLYA", "1378 PFLYA     "},
  {VarDesc::FloatCM, "PFL12XB", "1379 PFL12XB   "},
  {VarDesc::FloatCM, "PFL23XB", "1380 PFL23XB   "},
  {VarDesc::FloatCM, "PFL34XB", "1381 PFL34XB   "},
  {VarDesc::FloatCM, "PFLYB", "1382 PFLYB     "},
  {VarDesc::FloatCM, "PFL12XC", "1383 PFL12XC   "},
  {VarDesc::FloatCM, "PFL23XC", "1384 PFL23XC   "},
  {VarDesc::FloatCM, "PFL34XC", "1385 PFL34XC   "},
  {VarDesc::FloatCM, "PFLYC", "1386 PFLYC     "},
  {VarDesc::FloatCM, "PFL12XD", "1387 PFL12XD   "},
  {VarDesc::FloatCM, "PFL23XD", "1388 PFL23XD   "},
  {VarDesc::FloatCM, "PFL34XD", "1389 PFL34XD   "},
  {VarDesc::FloatCM, "PFLYD", "1390 PFLYD     "},
  {VarDesc::FloatCM, "PLATRARO", "1391 PLATRARO  "},
  {VarDesc::FloatCM, "PLATRARI", "1392 PLATRARI  "},
  {VarDesc::FloatCM, "PLATRAHE", "1393 PLATRAHE  "},
  {VarDesc::FloatCM, "PL12RAZE", "1394 PL12RAZE  "},
  {VarDesc::FloatCM, "PL23RAZE", "1395 PL23RAZE  "},
  {VarDesc::FloatCM, "PL34RAZE", "1396 PL34RAZE  "},
  {VarDesc::FloatCM, "PLATRHYP", "1397 PLATRHYP  "},
  {VarDesc::FloatCM, "PLATBTLE", "1398 PLATBTLE  "},
  {VarDesc::FloatCM, "PRLS01DX", "1399 PRLS01DX  "},
  {VarDesc::FloatCM, "PRLS11DX", "1400 PRLS11DX  "},
  {VarDesc::FloatCM, "PRLS01DY", "1401 PRLS01DY  "},
  {VarDesc::FloatCM, "PRLS11DY", "1402 PRLS11DY  "},
  {VarDesc::FloatCM, "PSSS3DY1", "1403 PSSS3DY1  "},
  {VarDesc::FloatCM, "PSSS3DY2", "1404 PSSS3DY2  "},
  {VarDesc::FloatCM, "PSSS3DX1", "1405 PSSS3DX1  "},
  {VarDesc::FloatCM, "PSSS3DX2", "1406 PSSS3DX2  "},
  {VarDesc::FloatCM, "PSSS3DX3", "1407 PSSS3DX3  "},
  {VarDesc::FloatCM, "PSSS3AN", "1408 PSSS3AN   "},
  {VarDesc::FloatCM, "PSSS3ZP1", "1409 PSSS3ZP1  "},
  {VarDesc::FloatCM, "PSSS3ZP2", "1410 PSSS3ZP2  "},
  {VarDesc::FloatCM, "PSSS3ZP3", "1411 PSSS3ZP3  "},
  {VarDesc::FloatCM, "PSSS3ZP4", "1412 PSSS3ZP4  "},
  {VarDesc::FloatCM, "PSSS3ZP5", "1413 PSSS3ZP5  "},
  {VarDesc::FloatCM, "PSSS3ZP6", "1414 PSSS3ZP6  "},
  {VarDesc::FloatCM, "PLATRAZ1", "1415 PLATRAZ1  "},
  {VarDesc::FloatCM, "PLATRAY1", "1416 PLATRAY1  "},
  {VarDesc::FloatCM, "PLATRAZ2", "1417 PLATRAZ2  "},
  {VarDesc::FloatCM, "PLATRAY2", "1418 PLATRAY2  "},
  {VarDesc::FloatCM, "PL12RAZ3", "1419 PL12RAZ3  "},
  {VarDesc::FloatCM, "PL23RAZ3", "1420 PL23RAZ3  "},
  {VarDesc::FloatCM, "PL34RAZ3", "1421 PL34RAZ3  "},
  {VarDesc::FloatCM, "PLATRAY3", "1422 PLATRAY3  "},
  {VarDesc::FloatCM, "PL12RAZ4", "1423 PL12RAZ4  "},
  {VarDesc::FloatCM, "PL23RAZ4", "1424 PL23RAZ4  "},
  {VarDesc::FloatCM, "PL34RAZ4", "1425 PL34RAZ4  "},
  {VarDesc::FloatCM, "PLATRAY4", "1426 PLATRAY4  "},
  {VarDesc::FloatCM, "PL12RAZ0", "1427 PL12RAZ0  "},
  {VarDesc::FloatCM, "PL23RAZ0", "1428 PL23RAZ0  "},
  {VarDesc::FloatCM, "PL34RAZ0", "1429 PL34RAZ0  "},
  {VarDesc::FloatCM, "PLATRAY0", "1430 PLATRAY0  "},
  {VarDesc::FloatCM, "PL12RHZP", "1431 PL12RHZP  "},
  {VarDesc::FloatCM, "PL23RHZP", "1432 PL23RHZP  "},
  {VarDesc::FloatCM, "PL34RHZP", "1433 PL34RHZP  "},
  {VarDesc::FloatCM, "PLATRVZE", "1434 PLATRVZE  "},
  {VarDesc::FloatCM, "PLATRVYP", "1435 PLATRVYP  "},
  {VarDesc::FloatCM, "PLATCRYP", "1436 PLATCRYP  "},
  {VarDesc::FloatCM, "PL12CRZ1", "1437 PL12CRZ1  "},
  {VarDesc::FloatCM, "PL12CRZ2", "1438 PL12CRZ2  "},
  {VarDesc::FloatCM, "PL23CRZ1", "1439 PL23CRZ1  "},
  {VarDesc::FloatCM, "PL23CRZ2", "1440 PL23CRZ2  "},
  {VarDesc::FloatCM, "PL34CRZ1", "1441 PL34CRZ1  "},
  {VarDesc::FloatCM, "PL34CRZ2", "1442 PL34CRZ2  "},
  {VarDesc::FloatCM, "PL12CRAN", "1443 PL12CRAN  "},
  {VarDesc::FloatCM, "PL23CRAN", "1444 PL23CRAN  "},
  {VarDesc::FloatCM, "PL34CRAN", "1445 PL34CRAN  "},
  {VarDesc::FloatCM, "PL12CRLE", "1446 PL12CRLE  "},
  {VarDesc::FloatCM, "PL23CRLE", "1447 PL23CRLE  "},
  {VarDesc::FloatCM, "PL34CRLE", "1448 PL34CRLE  "},
  {VarDesc::FloatCM, "PLAT2RAX", "1449 PLAT2RAX  "},
  {VarDesc::FloatCM, "PL12BTZ1", "1450 PL12BTZ1  "},
  {VarDesc::FloatCM, "PL23BTZ1", "1451 PL23BTZ1  "},
  {VarDesc::FloatCM, "PL34BTZ1", "1452 PL34BTZ1  "},
  {VarDesc::FloatCM, "PL12BTZ2", "1453 PL12BTZ2  "},
  {VarDesc::FloatCM, "PL23BTZ2", "1454 PL23BTZ2  "},
  {VarDesc::FloatCM, "PL34BTZ2", "1455 PL34BTZ2  "},
  {VarDesc::FloatCM, "PLATBTXP", "1456 PLATBTXP  "},
  {VarDesc::FloatCM, "PRLS01XP", "1457 PRLS01XP  "},
  {VarDesc::FloatCM, "PRLS11XP", "1458 PRLS11XP  "},
  {VarDesc::FloatCM, "PRLS01YP", "1459 PRLS01YP  "},
  {VarDesc::FloatCM, "PRLS11YP", "1460 PRLS11YP  "},
  {VarDesc::FloatCM, "PL12RLZ1", "1461 PL12RLZ1  "},
  {VarDesc::FloatCM, "PL23RLZ1", "1462 PL23RLZ1  "},
  {VarDesc::FloatCM, "PL34RLZ1", "1463 PL34RLZ1  "},
  {VarDesc::FloatCM, "PL12RLZ2", "1464 PL12RLZ2  "},
  {VarDesc::FloatCM, "PL23RLZ2", "1465 PL23RLZ2  "},
  {VarDesc::FloatCM, "PL34RLZ2", "1466 PL34RLZ2  "},
  {VarDesc::FloatCM, "PRLS01AN", "1467 PRLS01AN  "},
  {VarDesc::FloatCM, "PRLS11AN", "1468 PRLS11AN  "},
  {VarDesc::FloatCM, "PRLS01LE", "1469 PRLS01LE  "},
  {VarDesc::FloatCM, "PRLS11LE", "1470 PRLS11LE  "},
  {VarDesc::FloatCM, "PLATBSXP", "1471 PLATBSXP  "},
  {VarDesc::FloatCM, "PLATBTLS", "1472 PLATBTLS  "},
  {VarDesc::FloatCM, "PL12FRZP", "1473 PL12FRZP  "},
  {VarDesc::FloatCM, "PL23FRZP", "1474 PL23FRZP  "},
  {VarDesc::FloatCM, "PL34FRZP", "1475 PL34FRZP  "},
  {VarDesc::FloatCM, "PLATFRYP", "1476 PLATFRYP  "},
  {VarDesc::FloatCM, "PLATFRXP", "1477 PLATFRXP  "},
  {VarDesc::FloatCM, "PSSS3DYT", "1478 PSSS3DYT  "},
  {VarDesc::FloatCM, "PSSS3DXT", "1479 PSSS3DXT  "},
  {VarDesc::FloatCM, "PSSS3X0", "1480 PSSS3X0   "},
  {VarDesc::FloatCM, "PSSS3Y0", "1481 PSSS3Y0   "},
  {VarDesc::FloatCM, "PSSS3X1", "1482 PSSS3X1   "},
  {VarDesc::FloatCM, "PSSS3Y1", "1483 PSSS3Y1   "},
  {VarDesc::FloatCM, "PSSS3X2", "1484 PSSS3X2   "},
  {VarDesc::FloatCM, "PSSS3Y2", "1485 PSSS3Y2   "},
  {VarDesc::FloatCM, "PSSS3X3", "1486 PSSS3X3   "},
  {VarDesc::FloatCM, "PSSS3Y3", "1487 PSSS3Y3   "},
  {VarDesc::FloatCM, "PSSS3X4", "1488 PSSS3X4   "},
  {VarDesc::FloatCM, "PSSS3Y4", "1489 PSSS3Y4   "},
  {VarDesc::FloatCM, "PSSS3X5", "1490 PSSS3X5   "},
  {VarDesc::FloatCM, "PSSS3Y5", "1491 PSSS3Y5   "},
  {VarDesc::FloatCM, "PSSS3X6", "1492 PSSS3X6   "},
  {VarDesc::FloatCM, "PSSS3Y6", "1493 PSSS3Y6   "},
  {VarDesc::FloatCM, "PSSRAXP", "1494 PSSRAXP   "},
  {VarDesc::FloatCM, "PSSRAYP", "1495 PSSRAYP   "},
  {VarDesc::FloatCM, "PSSZRP1", "1496 PSSZRP1   "},
  {VarDesc::FloatCM, "PSSZRP2", "1497 PSSZRP2   "},
  {VarDesc::FloatCM, "PSSZRP3", "1498 PSSZRP3   "},
  {VarDesc::FloatCM, "PSSZRP4", "1499 PSSZRP4   "},
  {VarDesc::FloatCM, "PSSZRP5", "1500 PSSZRP5   "},
  {VarDesc::FloatCM, "PSSZRP6", "1501 PSSZRP6   "},
  {VarDesc::FloatCM, "PRHS3ZP1", "1502 PRHS3ZP1  "},
  {VarDesc::FloatCM, "PRHS3ZP2", "1503 PRHS3ZP2  "},
  {VarDesc::FloatCM, "PRHS3ZP3", "1504 PRHS3ZP3  "},
  {VarDesc::FloatCM, "PRHS3ZP4", "1505 PRHS3ZP4  "},
  {VarDesc::FloatCM, "PRHS3ZP5", "1506 PRHS3ZP5  "},
  {VarDesc::FloatCM, "PRHS3ZP6", "1507 PRHS3ZP6  "},
  {VarDesc::FloatCM, "PRHS3ZP7", "1508 PRHS3ZP7  "},
  {VarDesc::FloatCM, "PL14RAZE", "1509 PL14RAZE  "},
  {VarDesc::FloatCM, "PL14RAZP", "1510 PL14RAZP  "},
  {VarDesc::FloatCM, "PL14RAY1", "1511 PL14RAY1  "},
  {VarDesc::FloatCM, "PL14RAY2", "1512 PL14RAY2  "},
  {VarDesc::FloatCM, "PL14RVZE", "1513 PL14RVZE  "},
  {VarDesc::FloatCM, "PL14RVYP", "1514 PL14RVYP  "},
  {VarDesc::FloatCM, "PL14CRY1", "1515 PL14CRY1  "},
  {VarDesc::FloatCM, "PL14CRY2", "1516 PL14CRY2  "},
  {VarDesc::FloatCM, "P12S3CRA", "1517 P12S3CRA  "},
  {VarDesc::FloatCM, "P12S3CRL", "1518 P12S3CRL  "},
  {VarDesc::FloatCM, "P23S3CRA", "1519 P23S3CRA  "},
  {VarDesc::FloatCM, "P23S3CRL", "1520 P23S3CRL  "},
  {VarDesc::FloatCM, "P34S3CRA", "1521 P34S3CRA  "},
  {VarDesc::FloatCM, "P34S3CRL", "1522 P34S3CRL  "},
  {VarDesc::FloatCM, "PL14CRZ1", "1523 PL14CRZ1  "},
  {VarDesc::FloatCM, "PL14CRZ2", "1524 PL14CRZ2  "},
  {VarDesc::FloatCM, "PL14CRZ3", "1525 PL14CRZ3  "},
  {VarDesc::FloatCM, "PL14CRZ4", "1526 PL14CRZ4  "},
  {VarDesc::FloatCM, "PL14CRZ5", "1527 PL14CRZ5  "},
  {VarDesc::FloatCM, "PL14CRZ6", "1528 PL14CRZ6  "},
  {VarDesc::FloatCM, "PL14CRZ7", "1529 PL14CRZ7  "},
  {VarDesc::FloatCM, "PL14CRZ8", "1530 PL14CRZ8  "},
  {VarDesc::FloatCM, "PL14CRZ9", "1531 PL14CRZ9  "},
  {VarDesc::FloatCM, "PL14CRZA", "1532 PL14CRZA  "},
  {VarDesc::FloatCM, "P14S3BFL", "1533 P14S3BFL  "},
  {VarDesc::FloatCM, "P14S3BFA", "1534 P14S3BFA  "},
  {VarDesc::FloatCM, "P14S3BFX", "1535 P14S3BFX  "},
  {VarDesc::FloatCM, "P14S3BFY", "1536 P14S3BFY  "},
  {VarDesc::FloatCM, "P14S3FLA", "1537 P14S3FLA  "},
  {VarDesc::FloatCM, "P14S3FLX", "1538 P14S3FLX  "},
  {VarDesc::FloatCM, "P14S3FLY", "1539 P14S3FLY  "},
  {VarDesc::FloatCM, "P14S3FLZ", "1540 P14S3FLZ  "},
};


VarDesc AgddXML_Vars[] = {
  {VarDesc::String, "NameOfVariable", "Name Of Variable"},
  {VarDesc::FloatCM, "ValueOfVariable", "Value Of Variable"},
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

AmdcDbSvcMakerFromRDB::~AmdcDbSvcMakerFromRDB(){}

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

//  ABRT(pIRDBAccessSvc,pAmdcDbSvc);
//  AECT(pIRDBAccessSvc,pAmdcDbSvc);
//  ECST(pIRDBAccessSvc,pAmdcDbSvc);
//  FEET(pIRDBAccessSvc,pAmdcDbSvc);
//  GENV(pIRDBAccessSvc,pAmdcDbSvc);
//  JDSH(pIRDBAccessSvc,pAmdcDbSvc);
//  JFSH(pIRDBAccessSvc,pAmdcDbSvc);
//  JTSH(pIRDBAccessSvc,pAmdcDbSvc);
//  MBAP(pIRDBAccessSvc,pAmdcDbSvc);

//   AgddXMLVariables(pIRDBAccessSvc,pAmdcDbSvc);

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


void AmdcDbSvcMakerFromRDB::ABRT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ABRT", RANGE(ABRT_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::AECT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("AECT", RANGE(AECT_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::ECST(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("ECST", RANGE(ECST_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::FEET(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("FEET", RANGE(FEET_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::GENV(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("GENV", RANGE(GENV_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::JDSH(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("JDSH", RANGE(JDSH_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::JFSH(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("JFSH", RANGE(JFSH_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::JTSH(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("JTSH", RANGE(JTSH_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::MBAP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("MBAP", RANGE(MBAP_Vars), pIRDBAccessSvc, pAmdcDbSvc);
}


void AmdcDbSvcMakerFromRDB::AgddXMLVariables(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc)
{
  addVars ("AgddXMLVariables", RANGE(AgddXML_Vars), pIRDBAccessSvc, pAmdcDbSvc);
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


