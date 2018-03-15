/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Ladder services (Omega + Al tube + glue + part of pigtail + connector)
//
//
#include "PixelSCTDBHelper/GeoPixelServicesDBHelper.h"
#include "PixelSCTDBHelper/GeoPixelHelperStaveTypes.h"
#include "PixelSCTDBHelper/GeoPixelHelperMaterialMap.h"
#include "PixelSCTDBHelper/GeoPixelMaterialDBHelper.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include "GeoModelKernel/GeoMaterial.h"

#include <cmath>
#include <sstream>
#include <vector>


GeoPixelServicesDBHelper::GeoPixelServicesDBHelper(const PixelGeoBuilderBasics* basics):
  PixelGeoBuilder(basics)
{

  switchTable_ptr = geoAccessor().getTable("PixelSwitches");
  pixelFrame_ptr  = geoAccessor().getTable("PixelFrame");
  pixelFrameSect_ptr = geoAccessor().getTable("PixelFrameSect");

  pixelSimpleSvc_ptr = geoAccessor().getTable("PixelSimpleService");
  pixelBarrelSvc_ptr = geoAccessor().getTable("PixelBarrelService");
  pixelEndcapSvc_ptr = geoAccessor().getTable("PixelEndcapService");
  pixelEnvelopeSvc_ptr = geoAccessor().getTable("PixelEnvelopeService");
  pixelDiskSvc_ptr = geoAccessor().getTable("PixelDisk");

  m_frameElementMap=0;

}

GeoPixelServicesDBHelper::~GeoPixelServicesDBHelper()
{
}


// ------------- Pixel switches ----------------------------------------------------

bool GeoPixelServicesDBHelper::PixelBuildServices() const
{
  int tmp=geoAccessor().getInt(switchTable_ptr,"BUILDSERVICES");

  return(tmp>0);
}


// ------------- Barrel frame ----------------------------------------------------

// For new geometry a detailed frame is built.
bool GeoPixelServicesDBHelper::detailedFrame() const
{
  return geoAccessor().getTableSize(pixelFrame_ptr);
}
  
int GeoPixelServicesDBHelper::PixelFrameSections() const
{
  return geoAccessor().getTableSize(pixelFrame_ptr);
}

double GeoPixelServicesDBHelper::PixelFrameRMinSide(int sectionIndex) const
{
  return geoAccessor().getDouble(pixelFrame_ptr, "RMINSIDE", sectionIndex) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelFrameRMaxSide(int sectionIndex) const
{
  return geoAccessor().getDouble(pixelFrame_ptr, "RMAXSIDE", sectionIndex) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelFrameSideWidth(int sectionIndex) const
{
  return geoAccessor().getDouble(pixelFrame_ptr, "SIDEWIDTH", sectionIndex) * CLHEP::mm;
} 
 
double GeoPixelServicesDBHelper::PixelFrameZMin(int sectionIndex) const
{ 
  return geoAccessor().getDouble(pixelFrame_ptr, "ZMIN", sectionIndex) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelFrameZMax(int sectionIndex) const
{ 
  return geoAccessor().getDouble(pixelFrame_ptr, "ZMAX", sectionIndex) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelFramePhiStart(int sectionIndex) const
{
  return geoAccessor().getDouble(pixelFrame_ptr, "PHISTART", sectionIndex) * CLHEP::deg;
}
 
int GeoPixelServicesDBHelper::PixelFrameNumSides(int sectionIndex) const
{
  return geoAccessor().getInt(pixelFrame_ptr, "NUMSIDES", sectionIndex);
}

bool GeoPixelServicesDBHelper::PixelFrameMirrorSides(int sectionIndex) const
{
  return geoAccessor().getInt(pixelFrame_ptr, "MIRRORSIDES", sectionIndex);
}
									
std::string GeoPixelServicesDBHelper::PixelFrameSideMaterial(int sectionIndex) const
{
  return geoAccessor().getString(pixelFrame_ptr, "SIDEMATERIAL", sectionIndex);
}

std::string GeoPixelServicesDBHelper::PixelFrameCornerMaterial(int sectionIndex) const
{
  return geoAccessor().getString(pixelFrame_ptr, "CORNERMATERIAL", sectionIndex);
} 

int GeoPixelServicesDBHelper::PixelFrameSectionFromIndex(int sectionIndex) const
{
  return geoAccessor().getInt(pixelFrame_ptr,"SECTION",sectionIndex);
}
  
void 
GeoPixelServicesDBHelper::makeFrameIndexMap() 
{
  if (!m_frameElementMap) {
    m_frameElementMap = new std::map<int,std::vector<int> >;
    for (unsigned int i = 0; i < geoAccessor().getTableSize(pixelFrameSect_ptr); ++i) {
      int section = geoAccessor().getInt(pixelFrameSect_ptr,"SECTION",i);
      (*m_frameElementMap)[section].push_back(i);
    }
  }
}

int GeoPixelServicesDBHelper::getFrameElementIndex(int sectionIndex, int element) 
{
  // make map if it is not already made.
  makeFrameIndexMap();

  int section = PixelFrameSectionFromIndex(sectionIndex);

  int newIndex = -1;
  std::map<int,std::vector<int> >::const_iterator iter = m_frameElementMap->find(section);
  if (iter ==  m_frameElementMap->end()) {
    // Should never be the case as PixelFrameNumSideElements should generally be called first
    msg(MSG::ERROR) << "Frame section " << section << " has no elements." << endreq;
  } else {
    const std::vector<int> & vec = iter->second;
    if (static_cast<unsigned int>(element) >= vec.size()) {
      msg(MSG::ERROR) << "Element index " << element << " for section " << section << " out of range." << endreq;
    } else {
      newIndex = vec[element];
    }
  }
  return newIndex;
}


int GeoPixelServicesDBHelper::PixelFrameNumSideElements(int sectionIndex) 
{ 
  // make map if it is not already made.
  makeFrameIndexMap();

  std::cout<<"NumElement "<<sectionIndex<<std::endl;

  int section = PixelFrameSectionFromIndex(sectionIndex);
  int numElements = 0;

  std::map<int,std::vector<int> >::const_iterator iter = m_frameElementMap->find(section);
  if (iter ==  m_frameElementMap->end()) {
    msg(MSG::DEBUG) << "Frame section " << section << " has no elements." << endreq;
  } else {
    numElements = iter->second.size();
  }
  return numElements;
}
 
double GeoPixelServicesDBHelper::PixelFrameElementZMin1(int sectionIndex, int element)
{
  int index = getFrameElementIndex(sectionIndex, element);
  if (index < 0) return 0; // Error message already printed in getFrameElementIndex.
  return geoAccessor().getDouble(pixelFrameSect_ptr, "ZMIN1", index) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelFrameElementZMin2(int sectionIndex, int element)
{
  int index = getFrameElementIndex(sectionIndex, element);
  if (index < 0) return 0; // Error message already printed in getFrameElementIndex.
  return geoAccessor().getDouble(pixelFrameSect_ptr, "ZMIN2", index) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelFrameElementZMax1(int sectionIndex, int element)
{
  int index = getFrameElementIndex(sectionIndex, element);
  if (index < 0) return 0; // Error message already printed in getFrameElementIndex.
  return geoAccessor().getDouble(pixelFrameSect_ptr, "ZMAX1", index) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelFrameElementZMax2(int sectionIndex, int element)
{
  int index = getFrameElementIndex(sectionIndex, element);
  if (index < 0) return 0; // Error message already printed in getFrameElementIndex.
  return geoAccessor().getDouble(pixelFrameSect_ptr, "ZMAX2", index) * CLHEP::mm;
}



// --------------------- Pixel simple services 


double GeoPixelServicesDBHelper:: IBLServiceGetMinRadialPosition(const std::string& srvName, const std::string& srvType, 
									double srvZmin, double srvZmax) const
{
  
  double rmin=99999.;
  
  int numServices =  PixelServiceElements(srvType);
  int nbSrv=0;
  for(int ii = 0; ii < numServices; ii++) {
    //    Retrieve/calculate the parameters for the volume.
    
    std::string name;
    if(srvType=="simple")
      name=geoAccessor().getString(pixelSimpleSvc_ptr,"NAME",ii);
    else
      name=PixelServiceName(srvType,ii);

    if(name.find(srvName)!=std::string::npos){
      double zmin, zmax, r;
      int symm;
      if(srvType=="simple"){
	zmin=geoAccessor().getDouble(pixelSimpleSvc_ptr,"ZMIN",ii)*CLHEP::mm;
	zmax=geoAccessor().getDouble(pixelSimpleSvc_ptr,"ZMAX",ii)*CLHEP::mm;
	symm=geoAccessor().getInt(pixelSimpleSvc_ptr,"ZSYMM",ii);
	r=geoAccessor().getDouble(pixelSimpleSvc_ptr,"RMAX",ii)*CLHEP::mm;
      }
      else {
	zmin=PixelServiceZMin(srvType, ii);
	zmax=PixelServiceZMax(srvType, ii);
	symm=PixelServiceZsymm(srvType, ii);
	r=PixelServiceRMin(srvType, ii);
      }

      bool bZintervalle = false;
      if( (srvZmin-zmin)*(srvZmin-zmax)<0 || (srvZmax-zmin)*(srvZmax-zmax)<0 ) bZintervalle=true; 
      if( symm==1 && ((-srvZmin-zmin)*(-srvZmin-zmax)<0 || (-srvZmax-zmin)*(-srvZmax-zmax)<0) ) bZintervalle=true; 
      
      if(bZintervalle){
	if(r<rmin) rmin=r;
	nbSrv++;
      }
    }
  }
  
 if(nbSrv<1)return -1;
  return rmin;

}

double GeoPixelServicesDBHelper:: IBLServiceGetMaxRadialPosition(const std::string& srvName, const std::string& srvType, 
									double srvZmin, double srvZmax) const
{
  
  double rmax=-1.;
  int numServices =  PixelServiceElements(srvType);

  int nbSrv=0;
  for(int ii = 0; ii < numServices; ii++) {
    // Retrieve/calculate the parameters for the volume.
    //
    std::string name;
    if(srvType=="simple")
      name=geoAccessor().getString(pixelSimpleSvc_ptr,"NAME",ii);
    else
      name=PixelServiceName(srvType,ii);

    if(name.find(srvName)!=std::string::npos){

      double zmin, zmax, r;
      int symm;
      if(srvType=="simple"){
	zmin=geoAccessor().getDouble(pixelSimpleSvc_ptr,"ZMIN",ii)*CLHEP::mm;
	zmax=geoAccessor().getDouble(pixelSimpleSvc_ptr,"ZMAX",ii)*CLHEP::mm;
	symm=geoAccessor().getInt(pixelSimpleSvc_ptr,"ZSYMM",ii);
	r=geoAccessor().getDouble(pixelSimpleSvc_ptr,"RMAX",ii)*CLHEP::mm;
      }
      else {
	zmin=PixelServiceZMin(srvType, ii);
	zmax=PixelServiceZMax(srvType, ii);
	symm=PixelServiceZsymm(srvType, ii);
	r=PixelServiceRMax(srvType, ii);
      }
      
      bool bZintervalle = false;
      if( (srvZmin-zmin)*(srvZmin-zmax)<0 || (srvZmax-zmin)*(srvZmax-zmax)<0 ) bZintervalle=true; 
      if( symm==1 && ((-srvZmin-zmin)*(-srvZmin-zmax)<0 || (-srvZmax-zmin)*(-srvZmax-zmax)<0) ) bZintervalle=true; 
      
      if(bZintervalle && r>rmax){
	rmax=r;
	nbSrv++;
      }
    }
  }
  
 if(nbSrv<1)return -1;
  return rmax;

}

/////////////////////////////////////////////////////////
//
// Central Services
//
/////////////////////////////////////////////////////////
//
// This is complicated in the DB...
// If Rmin < 0 this means that the cylinder has to be placed only once
// So I return RMin with its sign, for further processing by the service
// methods.
// If RMax is <0 the thickness is given in % of r.l. which have to be 
// calculated by the method calculatethickness
//
// If Zmin and Zmax have DIFFERENT sign, then the thickness is given in 
// % of r.l....
//

int GeoPixelServicesDBHelper::PixelServiceElements(const std::string & type) const
{
  // FIXME
  /*
  if (dbVersion() < 3) {
    if(isBarrel() ) {
      if(a == "Inside") return (*PixelBarrelGeneral)[0]->getInt("NFRAMEIN");
      if(a == "Outside") return (*PixelBarrelGeneral)[0]->getInt("NFRAMEOUT");
    }
    if(isEndcap() ) {
      if(a == "Inside") return (*PixelEndcapGeneral)[0]->getInt("NFRAMEIN");
      if(a == "Outside") return (*PixelEndcapGeneral)[0]->getInt("NFRAMEOUT");   
    }
    return 0;
  } else {
  */
    // a is ignored. Use frame num to distinguish between inside (<1000) and ouside (>=1000). 
  if(type == "simple") return geoAccessor().getTableSize(pixelSimpleSvc_ptr);
  if(type == "barrel") return geoAccessor().getTableSize(pixelBarrelSvc_ptr);
  if(type == "endcap") return geoAccessor().getTableSize(pixelEndcapSvc_ptr);
  if(type == "envelope") return geoAccessor().getTableSize(pixelEnvelopeSvc_ptr);
  return 0;
  //}
}

// Name used when naming G4/Geo volumes 
std::string GeoPixelServicesDBHelper::PixelServiceName(const std::string & type, int index) const
{

    return getPixelServiceRecordString("VOLNAME",type,index); 
}


// Flag to say whether volume should be described in both positive and
// negative halves.
bool GeoPixelServicesDBHelper::PixelServiceZsymm(const std::string & type, int index) const
{
  return getPixelServiceRecordInt("ZSYMM",type,index);
}


double GeoPixelServicesDBHelper::PixelServiceRMin(const std::string & type, int index) const
{
  return std::abs(getPixelServiceRecordDouble("RIN",type,index)) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelServiceRMax(const std::string & type, int index) const
{
  double rtmp =  getPixelServiceRecordDouble("ROUT",type,index);
  // If this is negative this is the thickness of the cyl in % of r.l.
  double rmin = PixelServiceRMin(type,index);
  double rmax = 0;
  if(rtmp > 0) {
    rmax = rtmp * CLHEP::mm;
  } else {
    std::string material = PixelServiceMaterial(type,index);
    rmax = rmin + CalculateThickness(rtmp, material);
  }
  return rmax;
}

double GeoPixelServicesDBHelper::PixelServiceRMin2(const std::string & type, int index) const
{
  if (!getPixelServiceRecordTestField("RIN2",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordDouble("RIN2",type,index) * CLHEP::mm;
  }
}

double GeoPixelServicesDBHelper::PixelServiceRMax2(const std::string & type, int index) const
{
  if (!getPixelServiceRecordTestField("ROUT2",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordDouble("ROUT2",type,index) * CLHEP::mm;
  }
}

double GeoPixelServicesDBHelper::PixelServiceZMin(const std::string & type, int index) const
{
  return getPixelServiceRecordDouble("ZIN",type,index) * CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelServiceZMax(const std::string & type, int index) const
{
  double zout =  getPixelServiceRecordDouble("ZOUT",type,index);
  double zmin  =  PixelServiceZMin(type,index);
  double zmax = 0;
  // If zin and zout are opposite sign then zout is the thickness of the cyl in % of r.l.
  if(zmin * zout > -0.000000001) {
    zmax = zout * CLHEP::mm;
  } else {
    std::string material = PixelServiceMaterial(type,index);
    double sign = (zmin > 0) ? 1: -1;
    zmax = zmin + sign*CalculateThickness(zout, material);
  }
  return zmax;
}

double GeoPixelServicesDBHelper::PixelServicePhiLoc(const std::string & type, int index) const
{
  if (!getPixelServiceRecordTestField("PHI",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordDouble("PHI",type,index) * CLHEP::degree; 
  }
}

double GeoPixelServicesDBHelper::PixelServiceWidth(const std::string & type, int index) const
{
  if (!getPixelServiceRecordTestField("WIDTH",type,index)) {
    return 0;
  } else {
    // Can be in degree or CLHEP::mm. Leave it to GeoPixelServices to interpret.    
    return getPixelServiceRecordDouble("WIDTH",type,index); 
  }
}

int GeoPixelServicesDBHelper::PixelServiceRepeat(const std::string & type, int index) const
{
  if (!getPixelServiceRecordTestField("REPEAT",type,index)) {
    return 0;
  } else {
    return getPixelServiceRecordInt("REPEAT",type,index); 
  }
}

std::string GeoPixelServicesDBHelper::PixelServiceShape(const std::string & type, int index) const
{
  if (type == "simple") return "TUBE";
  if (!getPixelServiceRecordTestField("SHAPE",type,index)) {
    return "TUBE";
  } else {
    return getPixelServiceRecordString("SHAPE",type,index); 
  }
}


int GeoPixelServicesDBHelper::PixelServiceLD(const std::string & type, int index) const
{
  return getPixelServiceRecordInt("LAYERNUM",type,index)-1;
}

std::string GeoPixelServicesDBHelper::PixelServiceMaterial(const std::string & type, int index) const
{

  int imat = 0;
  if (type != "simple") {
    imat = getPixelServiceRecordInt("MATERIAL",type,index);
  }
  std::string materialName;
  if (!imat) {
    materialName = getPixelServiceRecordString("MATERIALNAME",type,index);
  } else {
    // Old
    if(type == "barrel") {
      std::string mat[11] = {
	"std::Berillia",
	"std::Carbon",
	"pix::Services",
	"pix::Titanium",
	"pix::MatPP11",
	"pix::MatPP12",
	"pix::MatPP13",
	"pix::MatPP14",
	"pix::MatPP15",
	"pix::MatPP16",
	"pix::MatPP17"};
      materialName =  mat[imat-1];
    } 
    if(type == "endcap") {
      std::string mat[4] = {
	"std::Berillia",
	"std::Carbon",
	"pix::ECServices",
	"pix::Disk"};
      materialName =  mat[imat-1];
    }
  }
  return materialName;
}


int GeoPixelServicesDBHelper::PixelServiceFrameNum(const std::string & type, int index) const
{
  // In older version frame num indicated "inside" or "outside"
  // 0-999:  Inside
  // >=1000: Outside
  // No frame number in simple table.
  if (type == "simple") return index+1;
  int framenum = getPixelServiceRecordInt("FRAMENUM",type,index);
  if (framenum <= 0) return index+1;
  return framenum;
  // FIXME
  /*
  if (dbVersion() < 3) return framenum;
  if (type == "Inside" && framenum < 1000) return framenum;
  if (type == "Outside" && framenum >= 1000) return framenum%1000;
  return -1; 
  */
}

// Access child/envelope service parameters
int GeoPixelServicesDBHelper::PixelServiceEnvelopeNum(const std::string & type, int index) const
{

  if (type != "envelope") return 0;

  try{
    int envnum = getPixelServiceRecordInt("ENVNUM",type,index);
    return envnum;
  }
  catch(...)
    {}

  return 0;
}

int GeoPixelServicesDBHelper::PixelServiceParentEnvelopeNum(const std::string & type, int index) const
{

  //  if (type == "envelope") return 0;

  if (type == "envelope"){
    try{
      int envnum = getPixelServiceRecordInt("ENVPARENT",type,index);
      return envnum;
    }
    catch(...)
      {}
  }
  else {

    try{
      int envnum = getPixelServiceRecordInt("ENVELOPE",type,index);
      return envnum;
    }
    catch(...)
      {
      }
  }


  return 0;
}

std::string GeoPixelServicesDBHelper::getPixelServiceRecordString(const std::string & name, const std::string & type, int index) const
{
  IRDBRecordset_ptr recordSet = getPixelServiceRecordset(type);
  return geoAccessor().getString(recordSet, name, index);
}

int GeoPixelServicesDBHelper::getPixelServiceRecordInt(const std::string & name, const std::string & type, int index) const
{
  IRDBRecordset_ptr recordSet = getPixelServiceRecordset(type);
  return geoAccessor().getInt(recordSet, name, index);
}


double GeoPixelServicesDBHelper::getPixelServiceRecordDouble(const std::string & name, const std::string & type, int index) const
{
  IRDBRecordset_ptr recordSet = getPixelServiceRecordset(type);
  return geoAccessor().getDouble(recordSet, name, index);
}

bool GeoPixelServicesDBHelper::getPixelServiceRecordTestField(const std::string & name, const std::string & type, int index) const
{
  IRDBRecordset_ptr recordSet = getPixelServiceRecordset(type);
  return geoAccessor().testField(recordSet, name, index);
}


// Returns IRDBRecordset
IRDBRecordset_ptr  GeoPixelServicesDBHelper::getPixelServiceRecordset(const std::string & type) const
{
  // m_barrelInFrames and m_endcapInFrames should be zero in dbVersion >= 3
  IRDBRecordset_ptr recordSet;
  if (type == "simple") {
    recordSet = pixelSimpleSvc_ptr;
  } else if(type == "barrel") {
    recordSet = pixelBarrelSvc_ptr;
    //if(type != "Inside") index += m_barrelInFrames;
  } else if(type == "endcap") {
    recordSet = pixelEndcapSvc_ptr;
    //if(type != "Inside") index += m_endcapInFrames;
  } else if(type == "envelope") {
    recordSet = pixelEnvelopeSvc_ptr;
    //if(type != "Inside") index += m_endcapInFrames;
  } else {
    msg(MSG::ERROR) << "ERROR:  getPixelServiceRecord(), neither Barrel of Endcap selected!" << endreq;
  }
  return recordSet;
} 

/////////////////////////////////////////////////////////
//
// Calculate Thickness. This is used for the materials
// which thickness is given in % of r.l.
//
/////////////////////////////////////////////////////////
double GeoPixelServicesDBHelper::CalculateThickness(double tck,std::string mat) const
{
  const GeoMaterial* material =  matMgr()->getMaterial(mat);
  double rl = material->getRadLength();
  material->ref();
  material->unref();
  return -1.*rl*tck/100.;
}


// ------------- EC cable ----------------------------------------------------

double GeoPixelServicesDBHelper::PixelECCablesThickness(int iDisk) const
{
  double tck = geoAccessor().getDouble(pixelDiskSvc_ptr,"CABLETHICK",iDisk);
  if( tck > 0.) {
    return tck*CLHEP::mm;
  } else {    

    GeoPixelMaterialDBHelper matDBHelper(getBasics());
    std::string matName =  matDBHelper.getMaterialName("DiskCable", iDisk);
    return CalculateThickness(tck,matName);
  }
}

double GeoPixelServicesDBHelper::PixelECCablesRMin(int iDisk) const
{
  return geoAccessor().getDouble(pixelDiskSvc_ptr,"RMINCABLE",iDisk)*CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelECCablesRMax(int iDisk) const
{
  return geoAccessor().getDouble(pixelDiskSvc_ptr,"RMAXCABLE",iDisk)*CLHEP::mm;
}


double GeoPixelServicesDBHelper::PixelECCablesDistance(int iDisk) const
{
  return geoAccessor().getDouble(pixelDiskSvc_ptr,"ZCABLE",iDisk)*CLHEP::mm;
}


// -------------- Disk services -------------------------------------------------

int GeoPixelServicesDBHelper::PixelDiskNumSupports() const
{
  // Hardwire for now
  return 3;
}

double GeoPixelServicesDBHelper::PixelDiskSupportRMin(int idisk, int isup) const
 {
  std::ostringstream field;
  field <<"SUP"<< isup+1 <<"RMIN";
  return geoAccessor().getDouble(pixelDiskSvc_ptr,field.str(),idisk)*CLHEP::mm;
}

double GeoPixelServicesDBHelper::PixelDiskSupportRMax(int idisk, int isup) const
{
  std::ostringstream field;
  field <<"SUP"<< isup+1 <<"RMAX";
  return geoAccessor().getDouble(pixelDiskSvc_ptr,field.str(),idisk)*CLHEP::mm;
}

// SLHC only
double GeoPixelServicesDBHelper::PixelDiskSupportThickness(int idisk, int isup) const
{

  std::ostringstream prefix;
  prefix <<"SUP"<< isup+1 <<"THICK";

  bool found = false;
  double tck = 0;
  // First check text file
  // default support thickness
  if (geoAccessor().testFieldTxt(pixelDiskSvc_ptr,"SUP_THICK")) {
    tck = geoAccessor().getDouble(pixelDiskSvc_ptr,"SUP_THICK");
    found = true;
  } 
  // overwrites if found
  if (geoAccessor().testFieldTxt(pixelDiskSvc_ptr,prefix.str(),idisk)) {
    tck = geoAccessor().getDouble(pixelDiskSvc_ptr,prefix.str(),idisk);
    found = true;
  }

  // Now check database
  if (!found) tck = geoAccessor().getDouble(pixelDiskSvc_ptr,prefix.str(),idisk);

  if(tck>0.) {
    return tck * CLHEP::mm;
  } else { // radlen
    int typeNum = PixelDiskSupportMaterialTypeNum(idisk,isup);
    GeoPixelMaterialDBHelper matDBHelper(getBasics());
    std::string matName = matDBHelper.getMaterialName("DiskSupport", idisk, typeNum);
    return CalculateThickness(tck, matName);
  }
}

int GeoPixelServicesDBHelper::PixelDiskSupportMaterialTypeNum(int idisk, int isup) const
{
 
  std::ostringstream prefix;
  prefix <<"SUP"<< isup+1 <<"MAT";

  int imat = 0;
  bool found = false;
  // default material type
  if (geoAccessor().testFieldTxt(pixelDiskSvc_ptr,"SUP_MAT")) {
    imat = geoAccessor().getInt(pixelDiskSvc_ptr,"SUP_MAT");
    found = true;
  } 
  // overwrites if found
  if (geoAccessor().testFieldTxt(pixelDiskSvc_ptr,prefix.str(),idisk)) {
    imat = geoAccessor().getInt(pixelDiskSvc_ptr,prefix.str(),idisk);
    found = true;
  }

  if (!found) {
    imat = geoAccessor().getInt(pixelDiskSvc_ptr,prefix.str(),idisk);
  }
  return imat;
}
