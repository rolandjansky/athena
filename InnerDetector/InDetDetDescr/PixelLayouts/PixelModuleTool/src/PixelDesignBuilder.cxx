 /*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelModuleTool/PixelDesignBuilder.h"

#include "PixelGeoModel/PixelGeoBuilder.h"

#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "PixelReadoutGeometry/PixelDiodeMatrix.h"
#include "ReadoutGeometryBase/SiCommonItems.h"
#include "PixelReadoutGeometry/PixelDetectorManager.h"

#include "StoreGate/StoreGate.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"

#include "PathResolver/PathResolver.h"
#include "PixelLayoutUtils/DBXMLUtils.h"

#include <algorithm>

using namespace InDetDD;

std::vector<PixelModuleDesign*> PixelDesignBuilder::s_designs;

PixelDesignBuilder::PixelDesignBuilder( const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  GeoXMLUtils(),
  m_moduleMap(nullptr)
{
  s_designs.clear();
}

PixelDesignBuilder::~PixelDesignBuilder()
{
  for(int i=0; i<(int)s_designs.size(); i++) delete s_designs[i];
  if(m_moduleMap) delete m_moduleMap;
}

void PixelDesignBuilder::initModuleMap(const PixelGeoBuilderBasics* basics)
{
  m_moduleMap = new GeoDetModulePixelMap(basics);

}


StatusCode PixelDesignBuilder::initialize()
{
  return StatusCode::SUCCESS;
}


StatusCode PixelDesignBuilder::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode PixelDesignBuilder::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IPixelDesignSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IPixelDesignSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();

  return StatusCode::SUCCESS;
}

StatusCode PixelDesignBuilder::callBack(IOVSVC_CALLBACK_ARGS_P(/*I*/,/*keys*/))
{
  return StatusCode::SUCCESS;
}




PixelModuleDesign* PixelDesignBuilder::getDesign( const PixelGeoBuilderBasics* basics, int moduleIndex)
{

  if (moduleIndex > (int) s_designs.size()-1) {
    s_designs.resize( moduleIndex+1, 0); // prefill with zeros
  }

  if (s_designs[ moduleIndex] == 0) {
    PixelModuleDesign* newDesign = build( basics, moduleIndex);
    s_designs[ moduleIndex] = newDesign;
    //? newDesign->ref();
  }
  return s_designs[ moduleIndex];
}

PixelModuleDesign* PixelDesignBuilder::getDesign(const PixelGeoBuilderBasics* basics, std::string moduleType)
{
  if(!m_moduleMap) {
    basics->msgStream()<<MSG::ERROR<<"No Module Map avilable! Did you call initModuleMap?"<<endreq;
    return nullptr;
  }
  int moduleIndex = m_moduleMap->getModuleIndex(moduleType);

  return getDesign(basics, moduleIndex);
}



PixelModuleDesign* PixelDesignBuilder::build( const PixelGeoBuilderBasics* basics, int moduleIndex)
{

  std::string fileName="PixelModules.xml";
  if(const char* env_p = std::getenv("PIXEL_SILICONMODULES_GEO_XML")) fileName = std::string(env_p);

  bool readXMLfromDB = basics->ReadInputDataFromDB();
  bool bParsed=false;
  if(readXMLfromDB)
    {
      msg(MSG::DEBUG)<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      DBXMLUtils dbUtils(basics);
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      msg(MSG::DEBUG)<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      InitializeXML();
      bParsed = ParseFile(file);
    }

  if(!bParsed){
    msg(MSG::ERROR)<<"XML file "<<fileName<<" not found"<<endmsg;
    return nullptr;
  }

  std::string moduleName = getChildValue("Module", moduleIndex, "moduleName");
  std::string chipName = getChildValue("Module", moduleIndex, "chipName");

  int lengthChip = getInt("Module", moduleIndex, "lengthInChips");
  int circuitsEta = lengthChip;

  int widthChipMax = getInt("Module", moduleIndex, "widthMaxInChips", 0);

  int widthChip=widthChipMax;
  int circuitsPhi = widthChip;

  msg(MSG::DEBUG)<<"Readout geo parameters - ChipWidth : "<<moduleName<<"  "<<widthChipMax<<endmsg;

  double thick = getDouble("Module", moduleIndex, "sensorThickness");

  int readoutIndex = getChildValue_Index("FrontEndChip","name",-1,chipName);
  double phiPitch = getDouble( "FrontEndChip", readoutIndex,"pitchPhi");
  double etaPitch = getDouble( "FrontEndChip",  readoutIndex,"pitchEta");

  double etaPitchLong = etaPitch;
  double etaPitchLongEnd = etaPitch;

  int rowsPerChip = getInt( "FrontEndChip",  readoutIndex,"rows");
  int emptyRows = 0;
  int rowsPerSensor = circuitsPhi*rowsPerChip + (circuitsPhi-1)*emptyRows; // FIXME check that the matrix does the right thing

  int colsPerChip = getInt( "FrontEndChip",  readoutIndex,"columns");
  int readoutSide = 1;


  msg(MSG::DEBUG)<<"readout geo : ------------------------------------------------------------------------"<<endmsg;
  msg(MSG::DEBUG)<<"readout geo : "<<chipName<<endmsg;
  msg(MSG::DEBUG)<<"readout geo : "<<moduleName<<" phi : "<<circuitsPhi<<" "<<rowsPerChip<<" empty "<<emptyRows<<endmsg;

  msg(MSG::DEBUG)<<"readout geo : "<<moduleName<<" eta : "<<circuitsEta<<" "<<colsPerChip<<endmsg;

  msg(MSG::DEBUG)<<"readout geo : "<< moduleName<<" "<< phiPitch<<" "<< etaPitch<<" "<< etaPitchLong<<" "<< etaPitchLongEnd<<" "<<
    circuitsPhi<<" "<< circuitsEta<<" "<< rowsPerSensor<<" "<< colsPerChip<<" *"<<circuitsEta<<endmsg;

  msg(MSG::DEBUG)<<"readout geo : ------------------------------------------------------------------------"<<endmsg;

  double cellRowPerCirc = circuitsPhi*rowsPerChip;


  // -----------------------------------------------------------------------------------------------------------------
  // For compatibility with LoI/ATLAS geometries ( etaPithLong, ganged pixels, ...)

  TerminateXML();

  fileName="PixelModules.xml";
  if(const char* env_p = std::getenv("PIXEL_SILICONREADOUT_GEO_XML")) fileName = std::string(env_p);
  msg(MSG::DEBUG)<<"Readout geometry file name : "<<fileName<<endmsg;

  bParsed=false;
  if(readXMLfromDB)
    {
      msg(MSG::DEBUG)<<"XML input : DB CLOB "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      DBXMLUtils dbUtils(basics);
      std::string XMLtext = dbUtils.readXMLFromDB(fileName);
      InitializeXML();
      bParsed = ParseBuffer(XMLtext,std::string(""));
    }
  else
    {
      msg(MSG::DEBUG)<<"XML input : from file "<<fileName<<"  (DB flag : "<<readXMLfromDB<<")"<<endmsg;
      InitializeXML();
      std::string file = PathResolver::find_file (fileName, "DATAPATH");
      bParsed = ParseFile(file);
    }

  if(!bParsed){
    msg(MSG::ERROR)<<"XML file "<<fileName<<" not found"<<endmsg;
    return nullptr;
  }

  int readoutIndexGeo = getChildValue_Index("FrontEndChipGeo","moduleName",-1,moduleName);
  if(readoutIndexGeo>-1) {

    msg(MSG::DEBUG)<<"Readout module : "<<moduleName<<"  "<<chipName<<"  "<<readoutIndexGeo<<endmsg;

    etaPitchLong = getDouble( "FrontEndChipGeo", readoutIndexGeo,"pitchEtaLong");
    etaPitchLongEnd = getDouble( "FrontEndChipGeo",  readoutIndexGeo,"pitchEtaEnd");
    emptyRows = getInt( "FrontEndChipGeo",readoutIndexGeo,"emptyRows");
    rowsPerSensor = circuitsPhi*rowsPerChip + (circuitsPhi-1)*emptyRows; // FIXME check that the matrix does the right thing

    readoutSide = getInt( "FrontEndChipGeo", readoutIndexGeo,"readoutSide");

    msg(MSG::DEBUG)<<"readout geo - old geo : "<< moduleName<<" "<< phiPitch<<" "<< etaPitch<<" "<< etaPitchLong<<" "<< etaPitchLongEnd<<" "<<
      circuitsPhi<<" "<< circuitsEta<<" "<< rowsPerSensor<<" "<< colsPerChip<<" *"<<circuitsEta<<"   empty "<<emptyRows<<endmsg;

  }

  msg(MSG::DEBUG)<<"readout geo : ------------------------------------------------------------------------"<<endmsg;

  // see PixelGeoModel/OraclePixGeoManager
  // This should be (*pdch)[0]->getDouble("NRPCHIP"), but in the current
  // design we prefer to have one chip in the rphi direction
  // and define the connections for the pixels in the gap
  circuitsPhi =1;


  msg(MSG::DEBUG)<<"readout geo - matrix : "<< phiPitch<<" "<< etaPitch<<" "<< etaPitchLong<<" "<< etaPitchLongEnd<<" "<<
    circuitsPhi<<" "<< circuitsEta<<" "<< rowsPerSensor<<" "<< colsPerChip<<endmsg;

  PixelDiodeMatrix * fullMatrix = buildMatrix( phiPitch, etaPitch, etaPitchLong, etaPitchLongEnd,
					       circuitsPhi, circuitsEta, rowsPerSensor, colsPerChip);

  msg(MSG::DEBUG) << "matrix that was build:\n" << fullMatrix->createDebugStringRepr() << endmsg;


  msg(MSG::DEBUG)<<"readout geo - design " << thick<<" "<<
    circuitsPhi<<" "<<
    circuitsEta<<" "<<
    colsPerChip<<" "<<
    cellRowPerCirc<<" "<<
    colsPerChip<<" "<<
    rowsPerSensor<<" "<<
    electrons<<" "<<
    readoutSide<<endmsg;

  PixelModuleDesign* design = new PixelModuleDesign( thick,
						     circuitsPhi,
						     circuitsEta,
						     colsPerChip,
						     cellRowPerCirc,
						     colsPerChip,
						     rowsPerSensor,
						     fullMatrix,
						     electrons,
						     readoutSide);
  fullMatrix->unref(); // ownership of fullMatrix was transfered to design

  msg(MSG::DEBUG)<<"readout geo - design : "<<design->width()<<" "<<design->length()<<" "<<design->thickness()<<"    "<<design->rows()<<" "<<design->columns()<<endmsg;

  // Multiple connections (ganged pixels)
  if (emptyRows>0){

    int gangedType = getInt( "FrontEndChipGeo",  readoutIndexGeo,"gangedType");
    int gangedIndex = getChildValue_Index("GangedType", "type", gangedType);
    std::vector<int> v_emptyrows = getVectorInt("GangedType",gangedIndex,"emptyrow");
    std::vector<int> v_connectrows = getVectorInt("GangedType",gangedIndex,"connectrow");

    msg(MSG::DEBUG)<<"readout geo - emptyrows>0 : "<< gangedType<<" "<< gangedIndex<<endmsg;

    int minRow = v_emptyrows[0];
    int maxRow = minRow;

    for (int iConnect = 0; iConnect < emptyRows; iConnect++){
      minRow = std::min(minRow, v_emptyrows[iConnect]);
      minRow = std::min(minRow, v_connectrows[iConnect]);
      maxRow = std::max(maxRow, v_emptyrows[iConnect]);
      maxRow = std::max(maxRow, v_connectrows[iConnect]);
    }

    std::vector <int> connections(maxRow-minRow+1);

    // We fill them all with a one to one correspondence first.
    for (unsigned int iRow = 0; iRow < connections.size(); iRow++){
      connections[iRow] = iRow +  minRow;
    }

    // Now make the connections.
    for (int iConnect = 0; iConnect < emptyRows; iConnect++){
      connections[v_emptyrows[iConnect]-minRow] = v_connectrows[iConnect];
    }

    design->addMultipleRowConnection(minRow, connections);
  }

  basics->getDetectorManager()->addDesign(design);

  return design;
}


// Originally copied  from GeoPixelSiCrystal::makeMatrix release 17.3.1
// FIXME avoid duplication!
PixelDiodeMatrix *  PixelDesignBuilder::buildMatrix( double phiPitch, double etaPitch,
						     double etaPitchLong, double etaPitchEnd,
						     int circuitsPhi, int circuitsEta,
						     int diodeRowPerCirc, int diodeColPerCirc)
{
    double phiPitchLong = phiPitch;
    double phiPitchEnd = phiPitch;
    int nPhiLong = 0;
    int nPhiEnd = 0;
    int nEtaLong = etaPitch == etaPitchLong ? 0 : 1;
    int nEtaEnd = etaPitch == etaPitchEnd ? 0 : 1;

    return buildMatrix( phiPitch, etaPitch,
			phiPitchLong, phiPitchEnd,
			etaPitchLong, etaPitchEnd,
			nPhiLong, nPhiEnd,
			nEtaLong, nEtaEnd,
			circuitsPhi, circuitsEta,
			diodeRowPerCirc, diodeColPerCirc);
}


// Re-written and overloaded here to be more general purpose
// FIXME PixelDiodeMatrix inherits from RCBase, which uses ref() and unref()
// for lifetime management (count starts at 0, so we need to call ref() one
// time before calling unref()); modern C++ should use std::shared_ptr for this
PixelDiodeMatrix* PixelDesignBuilder::buildMatrix(  double phiPitch, double etaPitch,
			                            double phiPitchLong, double phiPitchEnd,
			                            double etaPitchLong, double etaPitchEnd,
			                            int nPhiLong, int nPhiEnd,
			                            int nEtaLong, int nEtaEnd,
			                            int circuitsPhi, int circuitsEta,
			                            int diodeRowPerCirc, int diodeColPerCirc)
{
  // checking for unlogical values
  if (circuitsPhi < 1 or circuitsEta < 1) {
    msg(MSG::WARNING) << "Number of circuits is 0" << endreq;
    return nullptr;
  }
  if (diodeRowPerCirc < 1 or diodeColPerCirc < 1) {
    msg(MSG::WARNING) << "Number of diodes per circuit is 0" << endreq;
    return nullptr;
  }
  if (nPhiLong < 0 or nPhiEnd < 0 or nEtaLong < 0 or nEtaEnd < 0) {
    msg(MSG::WARNING) << "Number of long/end cells per circuit is below 0" << endreq;
    return nullptr;
  }

  // checking and correcting inconsistent values
  if (nPhiLong == 0 and (phiPitchLong == 0.0 or phiPitchLong == phiPitch)) {
    msg(MSG::WARNING) << "nPhiLong is set to 0, but phiPitchLong is neither 0 nor phiPitch! Setting nPhiLong to 1" << endreq;
    nPhiLong = 1;
  }
  if (nPhiEnd == 0 and (phiPitchEnd == 0.0 or phiPitchEnd == phiPitch)) {
    msg(MSG::WARNING) << "nPhiEnd is set to 0, but phiPitchEnd is neither 0 nor phiPitch! Setting nPhiEnd to 1" << endreq;
    nPhiEnd = 1;
  }
  if (nEtaLong == 0 and (etaPitchLong == 0.0 or etaPitchLong == etaPitch)) {
    msg(MSG::WARNING) << "nEtaLong is set to 0, but etaPitchLong is neither 0 nor etaPitch! Setting nEtaLong to 1" << endreq;
    nEtaLong = 1;
  }
  if (nEtaEnd == 0 and (etaPitchEnd == 0.0 or etaPitchEnd == etaPitch)) {
    msg(MSG::WARNING) << "nEtaEnd is set to 0, but etaPitchEnd is neither 0 nor etaPitch! Setting nEtaEnd to 1" << endreq;
    nEtaEnd = 1;
  }

  /*
   The naming of internal PixelDiodeMatrix cell objects here follows the
   convention of cell_XX, where X is N for normal, L for long or E for end.
   The first index denotes the phi direction, the second eta.
  */

  // creation of individual pixels
  PixelDiodeMatrix * cell_NN = nullptr;
  PixelDiodeMatrix * cell_NL = nullptr;
  PixelDiodeMatrix * cell_NE = nullptr;
  PixelDiodeMatrix * cell_LN = nullptr;
  PixelDiodeMatrix * cell_LL = nullptr;
  PixelDiodeMatrix * cell_LE = nullptr;
  PixelDiodeMatrix * cell_EN = nullptr;
  PixelDiodeMatrix * cell_EL = nullptr;
  PixelDiodeMatrix * cell_EE = nullptr;

  // only filling long/end pixels if needed
  cell_NN = new PixelDiodeMatrix(phiPitch, etaPitch);
  if (nEtaLong > 0) {cell_NL = new PixelDiodeMatrix(phiPitch, etaPitchLong);}
  if (nEtaEnd > 0)  {cell_NE = new PixelDiodeMatrix(phiPitch, etaPitchEnd);}

  if (nPhiLong > 0) {
    cell_LN = new PixelDiodeMatrix(phiPitchLong, etaPitch);
    if (nEtaLong > 0) {cell_LL = new PixelDiodeMatrix(phiPitchLong, etaPitchLong);}
    if (nEtaEnd > 0)  {cell_LE = new PixelDiodeMatrix(phiPitchLong, etaPitchEnd);}
  }
  if (nPhiEnd > 0) {
    cell_EN = new PixelDiodeMatrix(phiPitchEnd, etaPitch);
    if (nEtaLong > 0) {cell_EL = new PixelDiodeMatrix(phiPitchEnd, etaPitchLong);}
    if (nEtaEnd > 0)  {cell_EE = new PixelDiodeMatrix(phiPitchEnd, etaPitchEnd);}
  }

  // creation of long/end cell blocks (in case there are more then one long/end per cicuit)
  if (nPhiLong > 1) {
    if (cell_LN) {cell_LN = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, nullptr, cell_LN, nPhiLong, nullptr);}
    if (cell_LL) {cell_LL = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, nullptr, cell_LL, nPhiLong, nullptr);}
    if (cell_LE) {cell_LE = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, nullptr, cell_LE, nPhiLong, nullptr);}
  }
  if (nPhiEnd > 1) {
    if (cell_EN) {cell_EN = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, nullptr, cell_EN, nPhiEnd, nullptr);}
    if (cell_EL) {cell_EL = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, nullptr, cell_EL, nPhiEnd, nullptr);}
    if (cell_EE) {cell_EE = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, nullptr, cell_EE, nPhiEnd, nullptr);}
  }
  if (nEtaLong > 1) {
    if (cell_NL) {cell_NL = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, nullptr, cell_NL, nEtaLong, nullptr);}
    if (cell_LL) {cell_LL = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, nullptr, cell_LL, nEtaLong, nullptr);}
    if (cell_EL) {cell_EL = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, nullptr, cell_EL, nEtaLong, nullptr);}
  }
  if (nEtaEnd > 1) {
    if (cell_NE) {cell_NE = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, nullptr, cell_NE, nEtaEnd, nullptr);}
    if (cell_LE) {cell_LE = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, nullptr, cell_LE, nEtaEnd, nullptr);}
    if (cell_EE) {cell_EE = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, nullptr, cell_EE, nEtaEnd, nullptr);}
  }

  /*
   The naming of internal PixelDiodeMatrix cell objects here follows the
   convention of row_XY, where X is for phi N, L or E as before.
   Y is for eta:
   - L for a lower chip
   - M for a middle chip
   - U for an upper chip
   The first index denotes the phi direction, the second eta.
   If just one index is given, it is phi and eta is a full row.
  */

  // putting together the single chip rows (eta direction)
  PixelDiodeMatrix * fullChipRow_N = nullptr;
  PixelDiodeMatrix * fullChipRow_L = nullptr;
  PixelDiodeMatrix * fullChipRow_E = nullptr;

  if (circuitsEta == 1) {
    // special case of just one circuit in eta direction (no long cells, just end)
    fullChipRow_N = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_NE, cell_NN, diodeColPerCirc - 2*nEtaEnd, cell_NE);
    if (cell_LN) {fullChipRow_L = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_LE, cell_LN, diodeColPerCirc - 2*nEtaEnd, cell_LE);}
    if (cell_EN) {fullChipRow_E = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_EE, cell_EN, diodeColPerCirc - 2*nEtaEnd, cell_EE);}
  } else {
    // rows of individual chips
    PixelDiodeMatrix * singleChipRow_NL = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_NE, cell_NN, diodeColPerCirc -nEtaEnd  -nEtaLong, cell_NL);
    PixelDiodeMatrix * singleChipRow_NM = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_NL, cell_NN, diodeColPerCirc -nEtaLong -nEtaLong, cell_NL);
    PixelDiodeMatrix * singleChipRow_NU = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_NL, cell_NN, diodeColPerCirc -nEtaLong -nEtaEnd,  cell_NE);

    PixelDiodeMatrix * singleChipRow_LL = nullptr;
    PixelDiodeMatrix * singleChipRow_LM = nullptr;
    PixelDiodeMatrix * singleChipRow_LU = nullptr;
    if (cell_LN) {
      singleChipRow_LL = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_LE, cell_LN, diodeColPerCirc -nEtaEnd  -nEtaLong, cell_LL);
      singleChipRow_LM = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_LL, cell_LN, diodeColPerCirc -nEtaLong -nEtaLong, cell_LL);
      singleChipRow_LU = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_LL, cell_LN, diodeColPerCirc -nEtaLong -nEtaEnd,  cell_LE);
    }

    PixelDiodeMatrix * singleChipRow_EL = nullptr;
    PixelDiodeMatrix * singleChipRow_EM = nullptr;
    PixelDiodeMatrix * singleChipRow_EU = nullptr;
    if (cell_EN) {
      singleChipRow_EL = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_EE, cell_EN, diodeColPerCirc -nEtaEnd  -nEtaLong, cell_EL);
      singleChipRow_EM = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_EL, cell_EN, diodeColPerCirc -nEtaLong -nEtaLong, cell_EL);
      singleChipRow_EU = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, cell_EL, cell_EN, diodeColPerCirc -nEtaLong -nEtaEnd,  cell_EE);
    }

    // putting together the single chip rows
    if (circuitsEta == 2) {
      // special case of no middle chips in eta (just lower and upper)
      fullChipRow_N = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, singleChipRow_NL, singleChipRow_NU, 1, nullptr);
      if (cell_LN) {fullChipRow_L = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, singleChipRow_LL, singleChipRow_LU, 1, nullptr);}
      if (cell_EN) {fullChipRow_E = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, singleChipRow_EL, singleChipRow_EU, 1, nullptr);}
    } else {
      fullChipRow_N = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, singleChipRow_NL, singleChipRow_NM, circuitsEta-2, singleChipRow_NU);
      if (cell_LN) {fullChipRow_L = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, singleChipRow_LL, singleChipRow_LM, circuitsEta-2, singleChipRow_LU);}
      if (cell_EN) {fullChipRow_E = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir, singleChipRow_EL, singleChipRow_EM, circuitsEta-2, singleChipRow_EU);}
    }

    // unref calls to avoid memory leaks
    if (singleChipRow_NL) {singleChipRow_NL->ref(); singleChipRow_NL->unref();}
    if (singleChipRow_NM) {singleChipRow_NM->ref(); singleChipRow_NM->unref();}
    if (singleChipRow_NU) {singleChipRow_NU->ref(); singleChipRow_NU->unref();}
    if (singleChipRow_LL) {singleChipRow_LL->ref(); singleChipRow_LL->unref();}
    if (singleChipRow_LM) {singleChipRow_LM->ref(); singleChipRow_LM->unref();}
    if (singleChipRow_LU) {singleChipRow_LU->ref(); singleChipRow_LU->unref();}
    if (singleChipRow_EL) {singleChipRow_EL->ref(); singleChipRow_EL->unref();}
    if (singleChipRow_EM) {singleChipRow_EM->ref(); singleChipRow_EM->unref();}
    if (singleChipRow_EU) {singleChipRow_EU->ref(); singleChipRow_EU->unref();}
  }

  // combining the full eta rows to the full Matrix
  PixelDiodeMatrix * fullMatrix = nullptr;
  if (circuitsPhi == 1) {
    // special case of just one circuit in eta direction (no long cells, just end)
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, fullChipRow_E, fullChipRow_N, diodeRowPerCirc - 2*nPhiEnd, fullChipRow_E);
  } else {
    // columns of individual chips
    PixelDiodeMatrix * singleChipCol_L = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, fullChipRow_E, fullChipRow_N, diodeRowPerCirc -nPhiEnd  -nPhiLong, fullChipRow_L);
    PixelDiodeMatrix * singleChipCol_M = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, fullChipRow_L, fullChipRow_N, diodeRowPerCirc -nPhiLong -nPhiLong, fullChipRow_L);
    PixelDiodeMatrix * singleChipCol_U = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, fullChipRow_L, fullChipRow_N, diodeRowPerCirc -nPhiLong -nPhiEnd,  fullChipRow_E);

    // putting together the single chip rows
    if (circuitsPhi == 2) {
      // special case of no middle chips in phi (just lower and upper)
      fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, singleChipCol_L, singleChipCol_U, 1, nullptr);
    } else {
      fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir, singleChipCol_L, singleChipCol_M, circuitsPhi-2, singleChipCol_U);
    }

    // ref and unref calls to avoid memory leaks
    if(singleChipCol_L) {singleChipCol_L->ref(); singleChipCol_L->unref();}
    if(singleChipCol_M) {singleChipCol_M->ref(); singleChipCol_M->unref();}
    if(singleChipCol_U) {singleChipCol_U->ref(); singleChipCol_U->unref();}

  }

  // ref and unref calls to avoid memory leaks
  if (cell_NN) {cell_NN->ref(); cell_NN->unref();}
  if (cell_NL) {cell_NL->ref(); cell_NL->unref();}
  if (cell_NE) {cell_NE->ref(); cell_NE->unref();}
  if (cell_LN) {cell_LN->ref(); cell_LN->unref();}
  if (cell_LL) {cell_LL->ref(); cell_LL->unref();}
  if (cell_LE) {cell_LE->ref(); cell_LE->unref();}
  if (cell_EN) {cell_EN->ref(); cell_EN->unref();}
  if (cell_EL) {cell_EL->ref(); cell_EL->unref();}
  if (cell_EE) {cell_EE->ref(); cell_EE->unref();}
  if (fullChipRow_N) {fullChipRow_N->ref(); fullChipRow_N->unref();}
  if (fullChipRow_L) {fullChipRow_L->ref(); fullChipRow_L->unref();}
  if (fullChipRow_E) {fullChipRow_E->ref(); fullChipRow_E->unref();}
  // no unref for fullMatrix, since ownership is transfered by returning the pointer
  if (fullMatrix) {fullMatrix->ref();}

  return fullMatrix;
}
