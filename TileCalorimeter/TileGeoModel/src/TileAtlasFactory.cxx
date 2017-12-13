/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TileAtlasFactory.h"
#include "TileGeoSectionBuilder.h"
#include "TileGeoCutBuilder.h"
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDddbManager.h"
#include "TileDetDescr/TileDetDescriptor.h"

#include "CaloIdentifier/TileID.h"

#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoIdentifierTag.h"

//#include "GeoModelKernel/GeoCutVolAction.h"
//#include "GeoModelKernel/GeoSimplePolygonBrep.h"

#include "CLHEP/GenericFunctions/AbsFunction.hh"
#include "CLHEP/GenericFunctions/Variable.hh"
#include "GeoModelKernel/GeoXF.h" 
#include "GeoModelKernel/GeoSerialTransformer.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm> 
#include <cmath>
#include <string>

using namespace Genfun;
using namespace GeoXF;

// Constructor:
TileAtlasFactory::TileAtlasFactory(StoreGateSvc *pDetStore,
                                   TileDetDescrManager *manager,
                                   bool addPlates,
                                   int ushape,
                                   int glue,
                                   MsgStream *log,
				   bool fullGeo)
  : m_detectorStore(pDetStore)
  , m_detectorManager(manager)
  , m_log(log) 
  , m_addPlatesToCellVolume(addPlates)
  , m_Ushape(ushape)
  , m_Glue(glue)
  , m_testbeamGeometry(false)
  , m_verbose(log->level()<=MSG::VERBOSE) 
  , m_fullGeo(fullGeo)
{
  /** addPlatesToCellVolume should be the same as PlateToCell  
      flag in TileSimInfoLoader if we want to have self-consistent
      cell volumes for calibration hits, but it's not
      possible to read this parameter from TileSimInfoLoader 
      because TileSimInfoLoader is in another project (AtlasSimulation)

     Ushape value should ne the same as in TileSimInfoLoader 
     but it's not possible to read this parameter from TileSimInfoLoader 
     because TileSimInfoLoader is in another project (AtlasSimulation)
  */
}
  
// Destructor: 
TileAtlasFactory::~TileAtlasFactory(){}
  
// Creation of geometry:
void TileAtlasFactory::create(GeoPhysVol *world)  
{ 
  // Global geometri definition for debugging
  bool  Filling = true;
  bool  EBC = true /* Negative */, BAR = true /* Barrel */, EBA = true /* Positive */;
  int   NcpFrom = 1, NcpPlus = 63; // Default is all [1-63]
 
  //int   NcpFrom = 34, NcpPlus = 29; // ext.barrel, special

  double deltaPhi = 360./64; // we know apriory that 64 modules makes full cylinder 
  double AnglMin = (NcpFrom-1)*deltaPhi*CLHEP::deg, AnglMax = (NcpPlus+1)*deltaPhi*CLHEP::deg;

  // phi range of modules with special C10
  // double AnglMin1 = 38. * deltaPhi*CLHEP::deg;
  // double AnglMax1 = 42. * deltaPhi*CLHEP::deg;
  // double AnglMin2 = 54. * deltaPhi*CLHEP::deg;
  // double AnglMax2 = 58. * deltaPhi*CLHEP::deg;

  (*m_log) << MSG::INFO <<" Entering TileAtlasFactory::create()" << endmsg;

  // -------- -------- MATERIAL MANAGER -------- ----------
  DataHandle<StoredMaterialManager> theMaterialManager;
  if (StatusCode::SUCCESS != m_detectorStore->retrieve(theMaterialManager, "MATERIALS")) 
  {  
    (*m_log) << MSG::ERROR << "Could not find Material Manager MATERIALS" << endmsg; 
    return; 
  }
  const GeoMaterial* matAir = theMaterialManager->getMaterial("std::Air");
  const GeoMaterial* matIron = theMaterialManager->getMaterial("std::Iron");
  //const GeoMaterial* matAlu = theMaterialManager->getMaterial("std::Aluminium");

  // -------- -------- SECTION BUILDER  -------- ----------
  TileDddbManager* dbManager = m_detectorManager->getDbManager();
  TileGeoSectionBuilder* sectionBuilder = new TileGeoSectionBuilder(theMaterialManager,dbManager,m_Ushape,m_Glue,m_log);

  double DzSaddleSupport = 0, RadiusSaddle = 0;
  if (dbManager->BoolSaddle())
    {(*m_log) << MSG::INFO << " Tile Geometry with Saddle supports, starting from TileCal-CSC-02 xxx"<< endmsg;

     dbManager->SetCurrentSaddle(0);

     DzSaddleSupport = dbManager->DzSaddleSupport()*CLHEP::cm;
     RadiusSaddle = dbManager->RadiusSaddle()*CLHEP::cm;
     if(m_log->level()<=MSG::DEBUG)
       (*m_log) << MSG::DEBUG << " DzSaddleSupport()= "<<DzSaddleSupport<<" RadiusSaddle= "<<RadiusSaddle
		<< endmsg;
    }

  if(m_log->level()<=MSG::DEBUG)
    (*m_log) <<MSG::DEBUG << "TileAtlasFactory. addPlates = " <<m_addPlatesToCellVolume<<endmsg;
  // -------- -------- CUT BUILDER  -------- ----------
  //TileGeoCutBuilder* CutBuilder = new TileGeoCutBuilder(theMaterialManager,dbManager,m_log);

  // --------------- TILE  -------  TILE  --------- TILE ---------- TILE ------------
  // Envelope creation. Building three tree tops for standard setup and only one for commissioning
  GeoLogVol  *lvTileEnvelopeBarrel =0, *lvTileEnvelopePosEndcap =0, *lvTileEnvelopeNegEndcap =0;
  GeoPhysVol *pvTileEnvelopeBarrel =0, *pvTileEnvelopePosEndcap =0, *pvTileEnvelopeNegEndcap =0;

  // radius for minimization overlap volumes
  double rless   =0.15; // 150 [mkm] 

  double dzITC1 =0, rMinITC1 =0, rMaxITC1 =0;
  double dzITC2 =0, rMinITC2 =0, rMaxITC2 =0;

  double thicknessWedgeMother =0, heightWedgeMother =0, dy1WedgeMother =0, dy2WedgeMother =0;
  double Glue =0, dzGlue =0; //Thickness of glue layer in the absorber
  int NbPeriod =0;
  double zEndSection =0;

  // Positioning coordinates
  double X =0, Z =0;
  double zITCStandard =0;

  double ZLength[]  = {0.0,0.0,0.0,0.0,0.0,0.0};
  double EnvDZPos[] = {0.0,0.0,0.0,0.0,0.0,0.0};
  double PhiMax[]   = {-360.0,-360.0,-360.0,-360.0,-360.0,-360.0};
  double PhiMin[]   = {+360.0,+360.0,+360.0,+360.0,+360.0,+360.0};
  double RInMin[]   = {99999.9,99999.9,99999.9,99999.9,99999.9,99999.9};
  double ROutMax[]  = {0.0,0.0,0.0,0.0,0.0,0.0};
  double FingerRmax  =0;
  //unsigned int ienv_size = 6;

  // set default finger length
  double PosEnvThickness  =0;
  double NegEnvThickness  =0;
  double BFingerLength    =0;
  double BFingerLengthNeg =0;
  double BFingerLengthPos =0;

  // Barrel finger 
  dbManager->SetCurrentTifg(1);
  BFingerLength  = BFingerLengthNeg  = BFingerLengthPos  = dbManager->TIFGdz()*CLHEP::cm;

  double EBFingerLength =0;
  double EBFingerLengthNeg =0;
  double EBFingerLengthPos =0;

  // EBarrel finger 
  dbManager->SetCurrentTifg(2);
  EBFingerLength = EBFingerLengthNeg = EBFingerLengthPos = dbManager->TIFGdz()*CLHEP::cm;

  int n_env = dbManager->GetNumberOfEnv();
  
  //std::cerr <<std::cout.setf(std::ios::right)<<std::setiosflags(std::ios::fixed)<<std::setw(9)<<std::setprecision(2);

  if(m_log->level()<=MSG::DEBUG)
    (*m_log) << MSG::DEBUG << "n_env " << n_env << endmsg;

  for(int i = 0; i < n_env ; ++i)
  {
    dbManager->SetCurrentEnvByIndex(i);
    int Type = dbManager->GetEnvType();

    /*
    if (Type == 1) BAR = true;
    if (Type == 2) EBC = true; 
    if (Type == 3) EBA = true;
    */

    ZLength  [Type] = dbManager->GetEnvZLength()*CLHEP::cm;
    EnvDZPos [Type] = dbManager->GetEnvDZ()*CLHEP::cm;
    PhiMin   [Type] = std::min(PhiMin[Type], dbManager->GetEnvDPhi());
    PhiMax   [Type] = std::max(PhiMax[Type], dbManager->GetEnvNModules()*deltaPhi + dbManager->GetEnvDPhi());
    RInMin   [Type] = std::min(RInMin[Type], dbManager->GetEnvRin()*CLHEP::cm);
    ROutMax  [Type] = std::max(ROutMax[Type],dbManager->GetEnvRout()*CLHEP::cm);
    FingerRmax         = std::max(FingerRmax,dbManager->GetEnvRout()*CLHEP::cm);

    //std::cout << "# Type " <<Type<< " ZLength " <<ZLength [Type]<< " EnvDZPos "<<EnvDZPos [Type]<< "\n"; 
  }

  //
  // recalculate length of positive barrel finger length if Ext.Barrel is present 
  //

  double PosDelta =0;
  PosDelta = EnvDZPos[3] - EnvDZPos[1];
  if(m_log->level()<=MSG::DEBUG)
    (*m_log) << MSG::DEBUG <<" BFingerLengthPos "<<BFingerLengthPos<<" PosDelta "<<PosDelta;
  if (fabs(PosDelta) < fabs(EBFingerLength - BFingerLength) ) {
    BFingerLengthPos += PosDelta;
    (*m_log) <<" => New BFingerLengthPos "<<BFingerLengthPos<<endmsg;
  } else {
    /** Relative shift of EBpos is too big, This is expected behaviour for testbeam and commissioning configurations */
    BFingerLengthPos = BFingerLength;
    PosDelta =0;
    (*m_log) <<" => New PosDelta "<<PosDelta<<endmsg;
  }

  //
  // recalculate length of negative barrel finger length if Ext.Barrel is present 
  //

  double NegDelta =0;
  NegDelta = (-EnvDZPos[2] + EnvDZPos[1]); // negative shift - bigger finger
  if(m_log->level()<=MSG::DEBUG)
    (*m_log) << MSG::DEBUG <<" BFingerLengthNeg "<<BFingerLengthNeg<<" NegDelta "<<NegDelta;
  if (fabs(NegDelta) < fabs(EBFingerLength - BFingerLength) ) {
    BFingerLengthNeg += NegDelta;
    (*m_log) <<" => New BFingerLengthNeg "<<BFingerLengthNeg<<endmsg;
  } else {
    /** Relative shift of EBpos is too big, This is expected behaviour for testbeam and commissioning configurations */
    BFingerLengthNeg = BFingerLength;
    NegDelta =0;
    (*m_log) <<" => New NegDelta "<<NegDelta<<endmsg;
  }

  // extra flag sayubg that special C10 goes outside envelope for normal C10
  bool spC10 = false;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
  double rMinC10 = dbManager->TILBrminimal();
  if ( dbManager->SetCurrentSection(10+TileDddbManager::TILE_PLUG2, false) ) {
    double rMinC10sp = dbManager->TILBrminimal();
    spC10 = (rMinC10sp < rMinC10);
    if (spC10) {
      (*m_log) << MSG::DEBUG <<" Special C10, changing Rmin from "<<rMinC10<<" to "<< rMinC10sp << endmsg;
      rMinC10 = rMinC10sp;
    }
  }

  // extra flag sayubg that E4' present on negative eta side
  bool spE4 = false;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
  double rMinE4pos = dbManager->TILBrminimal();
  double rMinE4neg = rMinE4pos;
  if ( dbManager->SetCurrentSection(10+TileDddbManager::TILE_PLUG4, false) ) {
    double rMinE4sp = dbManager->TILBrminimal();
    spE4 = (rMinE4sp < rMinE4neg);
    if (spE4) {
      (*m_log) << MSG::DEBUG <<" E4' present, changing Rmin for negative crack from "<<rMinE4neg<<" to "<< rMinE4sp << endmsg;
      rMinE4neg = rMinE4sp;
    }
  }
  
  /** setfinger length */
  double BFingerRmin=0, EFingerRmin=0;
  dbManager->SetCurrentSection(TileDddbManager::TILE_BARREL);
  BFingerRmin = dbManager->TILBrmax()*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
  EFingerRmin = dbManager->TILBrmax()*CLHEP::cm;
  
  /** calculation PosEnvThickness enlarge mother volume by extra distance between barrel and positive ext.barrel */
  dbManager->SetCurrentEnvByType(3);
  PosEnvThickness += dbManager->GetEnvDZ()*dbManager->GetEnvSide();

  /** enlarge mother volume by extra distance between barrel and negative ext.barrel */
  dbManager->SetCurrentEnvByType(2);
  NegEnvThickness += dbManager->GetEnvDZ()*dbManager->GetEnvSide();


  //
  // Central Barrel 
  //

  // Z planes 
  double endCentralBarrel = ZLength[1]/2 - BFingerLength; // nominal position of end plate
  double endEnvelopeNeg = endCentralBarrel + BFingerLengthNeg;
  double endEnvelopePos = endCentralBarrel + BFingerLengthPos;

  // R minimal 
  double rminBarrel = RInMin[1];
    
  // R maximal
  double rmaxTotal = ROutMax[1];
    
  GeoPcon* tileEnvPconeBarrel = new GeoPcon(PhiMin[1]*CLHEP::deg, PhiMax[1]*CLHEP::deg);

   tileEnvPconeBarrel->addPlane(-endEnvelopeNeg,                   BFingerRmin,             rmaxTotal);
   tileEnvPconeBarrel->addPlane(-endCentralBarrel-DzSaddleSupport, BFingerRmin,             rmaxTotal);
  if (dbManager->BoolSaddle())
  {tileEnvPconeBarrel->addPlane(-endCentralBarrel-DzSaddleSupport, BFingerRmin-RadiusSaddle,rmaxTotal);
   tileEnvPconeBarrel->addPlane(-endCentralBarrel,                 BFingerRmin-RadiusSaddle,rmaxTotal);
  }
   tileEnvPconeBarrel->addPlane(-endCentralBarrel,                 rminBarrel,              rmaxTotal);
   tileEnvPconeBarrel->addPlane( endCentralBarrel,                 rminBarrel,              rmaxTotal);
  if (dbManager->BoolSaddle())
  {tileEnvPconeBarrel->addPlane( endCentralBarrel,                 BFingerRmin-RadiusSaddle,rmaxTotal);
   tileEnvPconeBarrel->addPlane( endCentralBarrel+DzSaddleSupport, BFingerRmin-RadiusSaddle,rmaxTotal);
  }tileEnvPconeBarrel->addPlane( endCentralBarrel+DzSaddleSupport, BFingerRmin,             rmaxTotal);
   tileEnvPconeBarrel->addPlane( endEnvelopePos,                   BFingerRmin,             rmaxTotal);

  lvTileEnvelopeBarrel = new GeoLogVol("TileCentralBarrel",tileEnvPconeBarrel,matAir);
  pvTileEnvelopeBarrel = new GeoPhysVol(lvTileEnvelopeBarrel);

  //
  // Pos Ext. Barrel 
  // 

  // Z planes 
  double PosEndBarrelFinger = ZLength[1]/2; // nominal end of barrel finger
  double PosEndITC = PosEndBarrelFinger + ZLength[5];
  double PosEndExBarrelFinger = PosEndITC + ZLength[3];
  double PosEndExBarrel = PosEndExBarrelFinger - EBFingerLengthPos;

  if(m_log->level()<=MSG::DEBUG) {
    (*m_log) << MSG::DEBUG 
	     << " EBPos EnvDZPos[3] " << EnvDZPos[3] << " ZLength[5] " <<ZLength[5]<<"+"<<ZLength[3]
	     << " = " << ZLength[3]+ZLength[5] << " EBFingerLengthPos = " <<EBFingerLengthPos
	     <<endmsg;
    
    (*m_log) << MSG::DEBUG << " PosEndBarrelFinger = " << PosEndBarrelFinger
	     << " PosEndITC = " << PosEndITC
	     << " PosEndExBarrel = " << PosEndExBarrel
	     << " PosEndExtBarrelFinger = " << PosEndExBarrelFinger
	     << endmsg;
  }

  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
  double PosEndITC1 = (dbManager->TILBzoffset() + dbManager->TILEzshift() + dbManager->TILBdzmodul()/2)*CLHEP::cm;
  double corr = PosEndITC - PosEndITC1;
  if (fabs(corr)>0.01) {
     (*m_log) << MSG::WARNING
              << "Discrepancy between TileGlobals and TILB tables in GeoModel DB "
              << PosEndITC << " != " << PosEndITC1 << "; take this into account" 
              <<endmsg;
  }

  //double beginITC1   = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()-dbManager->TILBdzmodul()/2)*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
  double PosBeginITC2  = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()-dbManager->TILBdzmodul()/2)*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);
  double PosBeginGap = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()-dbManager->TILBdzmodul()/2)*CLHEP::cm;
  double PosEndGap   = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()+dbManager->TILBdzmodul()/2)*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
  double PosBeginCrack = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()-dbManager->TILBdzmodul()/2)*CLHEP::cm;
  double PosEndCrack   = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()+dbManager->TILBdzmodul()/2)*CLHEP::cm;

  if (!spC10) { // recovering old (wrong) positioning of gap scintillators
    double GapWidth = PosEndGap - PosBeginGap;
    PosBeginGap = PosBeginCrack + 0.9 * CLHEP::cm;
    PosEndGap   = PosBeginGap + GapWidth;
  }
  
  if(m_log->level()<=MSG::DEBUG)
    (*m_log) << MSG::DEBUG << " PosBeginITC2  = " << PosBeginITC2
	     << " PosBeginGap  = " << PosBeginGap
	     << " PosEndGap  = " << PosEndGap
	     << " PosBeginCrack  = " << PosBeginCrack
	     << " PosEndCrack  = " << PosEndCrack
	     << endmsg;
  
  // R minimals 
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
  double PosRminITC1  = dbManager->TILBrminimal()*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
  double PosRminITC   = dbManager->TILBrminimal()*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
  double PosRminCrack = rMinE4pos*CLHEP::cm;

  double PosRminExt = RInMin[3];
    
  // R maximal
  double PosRmaxTotal = ROutMax[3];
    
  GeoPcon* tileEnvPconePosEndcap = new GeoPcon(PhiMin[3]*CLHEP::deg, PhiMax[3]*CLHEP::deg);

  // Positive Endcap 
  tileEnvPconePosEndcap->addPlane(PosEndBarrelFinger,             PosRminITC1,              PosRmaxTotal);
  tileEnvPconePosEndcap->addPlane(PosBeginITC2,                   PosRminITC1,              PosRmaxTotal);
  tileEnvPconePosEndcap->addPlane(PosBeginITC2,                   PosRminITC,               PosRmaxTotal);
  tileEnvPconePosEndcap->addPlane(PosBeginCrack,                  PosRminITC,               PosRmaxTotal);
  tileEnvPconePosEndcap->addPlane(PosBeginCrack,                  PosRminCrack,             PosRmaxTotal);
  tileEnvPconePosEndcap->addPlane(PosEndCrack,                    PosRminCrack,             PosRmaxTotal);
  tileEnvPconePosEndcap->addPlane(PosEndCrack,                    PosRminExt,               PosRmaxTotal);
  tileEnvPconePosEndcap->addPlane(PosEndExBarrel,                 PosRminExt,               PosRmaxTotal);
  if (dbManager->BoolSaddle())
  { tileEnvPconePosEndcap->addPlane(PosEndExBarrel,                 EFingerRmin-RadiusSaddle, PosRmaxTotal);
    tileEnvPconePosEndcap->addPlane(PosEndExBarrel+DzSaddleSupport, EFingerRmin-RadiusSaddle, PosRmaxTotal);
  }
  tileEnvPconePosEndcap->addPlane(PosEndExBarrel+DzSaddleSupport, EFingerRmin,              PosRmaxTotal);
  tileEnvPconePosEndcap->addPlane(PosEndExBarrelFinger,           EFingerRmin,              PosRmaxTotal);

  lvTileEnvelopePosEndcap = new GeoLogVol("TileEndcapPos",tileEnvPconePosEndcap,matAir);
  pvTileEnvelopePosEndcap = new GeoPhysVol(lvTileEnvelopePosEndcap);

  //
  // Neg Ex Barrel
  // 

  // Z planes
  double NegEndBarrelFinger = ZLength[1]/2; // nominal end of barrel finger
  double NegEndITC = NegEndBarrelFinger + ZLength[4];
  double NegEndExtBarrelFinger = NegEndITC + ZLength[2];
  double NegEndExBarrel = NegEndExtBarrelFinger - EBFingerLengthNeg;

  if(m_log->level()<=MSG::DEBUG) {
    (*m_log) << MSG::DEBUG 
	     << " EBNeg EnvDZPos[2] " << EnvDZPos[2] << " ZLength[4] " <<ZLength[4]<<"+"<<ZLength[2]
	     << " = " << ZLength[2]+ZLength[4] << " EBFingerLengthNeg = " <<EBFingerLengthNeg
	     <<endmsg;
    
    (*m_log) << MSG::DEBUG << " NegEndBarrelFinger = " << NegEndBarrelFinger
	     << " NegEndITC = " << NegEndITC
	     << " NegEndExBarrel = " << NegEndExBarrel
	     << " NegEndExtBarrelFinger = " << NegEndExtBarrelFinger
	     << endmsg;
  }

  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
  double NegEndITC1 = (dbManager->TILBzoffset() + dbManager->TILEzshift() + dbManager->TILBdzmodul()/2)*CLHEP::cm;
  corr = NegEndITC - NegEndITC1;

  if (fabs(corr)>0.01) { (*m_log) << MSG::WARNING
                                  << "Discrepancy between TileGlobals and TILB tables in GeoModel DB "
                                  << NegEndITC << " != " << NegEndITC1 << "; take this into account" 
                                  << endmsg;
    }
  //double NegBeginITC1 = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()-dbManager->TILBdzmodul()/2)*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
  double NegBeginITC2   = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()-dbManager->TILBdzmodul()/2)*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);
  double NegBeginGap    = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()-dbManager->TILBdzmodul()/2)*CLHEP::cm;
  double NegEndGap      = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()+dbManager->TILBdzmodul()/2)*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
  double NegBeginCrack  = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()-dbManager->TILBdzmodul()/2)*CLHEP::cm;
  double NegEndCrack    = corr + (dbManager->TILBzoffset() + dbManager->TILEzshift()+dbManager->TILBdzmodul()/2)*CLHEP::cm;

  if (!spC10) { // recovering old (wrong) positioning of gap scintillators
    double GapWidth = NegEndGap - NegBeginGap;
    NegBeginGap = NegBeginCrack + 0.9 * CLHEP::cm;
    NegEndGap = NegBeginGap + GapWidth;
  }
  
  if(m_log->level()<=MSG::DEBUG)
    (*m_log) << MSG::DEBUG << " NegBeginITC2  = " << NegBeginITC2
	     << " NegBeginGap  = " << NegBeginGap
	     << " NegEndGap  = " << NegEndGap
	     << " NegBeginCrack  = " << NegBeginCrack
	     << " NegEndCrack  = " << NegEndCrack
	     << endmsg;

  // R minimals 
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
  double NegRminITC1 = dbManager->TILBrminimal()*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);
  double NegRminITC = dbManager->TILBrminimal()*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
  double NegRminCrack = rMinE4neg*CLHEP::cm;
  dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);

  double NegRminExt = RInMin[2];
    
  // R maximal 
  double NegRmaxTotal = ROutMax[2];
    
  GeoPcon* tileEnvPconeNegEndcap = new GeoPcon(PhiMin[2]*CLHEP::deg, PhiMax[2]*CLHEP::deg);

  // Negative Endcap 
  tileEnvPconeNegEndcap->addPlane(-NegEndExtBarrelFinger,          EFingerRmin,              NegRmaxTotal);
  tileEnvPconeNegEndcap->addPlane(-NegEndExBarrel-DzSaddleSupport, EFingerRmin,              NegRmaxTotal);
  if (dbManager->BoolSaddle())
  { tileEnvPconeNegEndcap->addPlane(-NegEndExBarrel-DzSaddleSupport, EFingerRmin-RadiusSaddle, NegRmaxTotal);
    tileEnvPconeNegEndcap->addPlane(-NegEndExBarrel,                 EFingerRmin-RadiusSaddle, NegRmaxTotal);
  }
  tileEnvPconeNegEndcap->addPlane(-NegEndExBarrel,                 NegRminExt,               NegRmaxTotal);
  tileEnvPconeNegEndcap->addPlane(-NegEndCrack,                    NegRminExt,               NegRmaxTotal);
  tileEnvPconeNegEndcap->addPlane(-NegEndCrack,                    NegRminCrack,             NegRmaxTotal);
  tileEnvPconeNegEndcap->addPlane(-NegBeginCrack,                  NegRminCrack,             NegRmaxTotal);
  tileEnvPconeNegEndcap->addPlane(-NegBeginCrack,                  NegRminITC,               NegRmaxTotal);
  tileEnvPconeNegEndcap->addPlane(-NegBeginITC2,                   NegRminITC,               NegRmaxTotal);
  tileEnvPconeNegEndcap->addPlane(-NegBeginITC2,                   NegRminITC1,              NegRmaxTotal);
  tileEnvPconeNegEndcap->addPlane(-NegEndBarrelFinger,             NegRminITC1,              NegRmaxTotal);

  lvTileEnvelopeNegEndcap = new GeoLogVol("TileEndcapNeg",tileEnvPconeNegEndcap,matAir);
  pvTileEnvelopeNegEndcap = new GeoPhysVol(lvTileEnvelopeNegEndcap);

  // --------------- Configure Section Builder for the Reco geometry --------------------
  if(!m_fullGeo) {
    // Central barrel part
    dbManager->SetCurrentSectionByNumber(1);
    
    dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() 
	      - (dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()) 
		 -  dbManager->TILBdzmast()))/(2.*(2.*dbManager->TILBnperiod() - 1));

    sectionBuilder->setBarrelPeriodThickness(2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac() + 2.*dzGlue)*CLHEP::cm);
    sectionBuilder->setBarrelGlue(dzGlue*CLHEP::cm);

    // Ext barrel part
    dbManager->SetCurrentSectionByNumber(2);

    dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() 
	      - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()))/(4.*dbManager->TILBnperiod());

    sectionBuilder->setExtendedPeriodThickness(2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac() + 2.*dzGlue)*CLHEP::cm);
  }

  GeoPhysVol *pvBarrelMother =NULL,     *pvFingerMotherNeg =NULL, *pvFingerMotherPos =NULL, 
             *pvSaddleMotherNeg =NULL,  *pvSaddleMotherPos =NULL;
  GeoPhysVol *pvEBarrelMotherNeg =NULL, *pvEBarrelMotherPos =NULL;
  GeoPhysVol *pvEFingerMotherNeg =NULL, *pvEFingerMotherPos =NULL,
             *pvESaddleMotherNeg =NULL, *pvESaddleMotherPos =NULL;
  GeoPhysVol *pvITCMotherNeg =NULL,     *pvITCMotherPos =NULL;
  GeoPhysVol *pvGapMotherNeg =NULL,     *pvGapMotherPos =NULL;
  GeoPhysVol *pvCrackMotherNeg =NULL,   *pvCrackMotherPos =NULL;

  GeoLogVol  *lvEBarrelModuleMotherPos =NULL, *lvEBarrelModuleMotherNeg =NULL;
  GeoPhysVol *pvEBarrelModuleMotherPos =NULL, *pvEBarrelModuleMotherNeg =NULL;

  /*
  GeoPhysVol *pvTmp_EBarrelModuleMotherPos =0,*pvTmL_EBarrelModuleMotherPos =0,*pvTmR_EBarrelModuleMotherPos =0;
  GeoPhysVol *pvTmp_EBarrelModuleMotherNeg =0,*pvTmL_EBarrelModuleMotherNeg =0,*pvTmR_EBarrelModuleMotherNeg =0;
  */

  //GeoCutVolAction *action1 =0, *action2 =0, *action3 =0;
  // -----------------------------------------------------------------------------------------------------------------
  //
  // All volumes which absolute forme EnvCounter loop
  //
  // -----------------------------------------------------------------------------------------------------------------
  double dX1 =0, dX2 =0, dY1 =0, dY2 =0, dZ1 =0, dZ2 =0;
  double dxIron =0, dyIron =0, dxIr =0, dyIr =0;
  std::string volname ="";

  // Cuting positioning
  double PosXcut =0, PosYcut =0, PosY =0;
  double modl_length =0;

  const GeoShapeUnion *CutA= 0;
  GeoShape *CutB= 0;

  GeoLogVol  *lvIrUp =0, *lvIrDw =0, *lvIron3 =0, *lvIron2 =0, *lvIron1 =0, *lvIrBox =0;
  GeoPhysVol *pvIrUp =0, *pvIrDw =0, *pvIron3 =0, *pvIron2 =0, *pvIron1 =0, *pvIrBox =0;

  if(m_fullGeo) {

    // ext. barrel Cuts description
    if (dbManager->BoolCuts()) {
      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG << " Tile Geometry with Ext.Barrel CutOuts and Iron plates, starting form TileCal-CSC-01"
		 << endmsg;

      volname = "CutB"; dbManager->SetCurrentCuts(volname); 
      PosXcut = dbManager->CutsXpos(); 
      PosYcut = dbManager->CutsYpos(); 
      modl_length = 4*dbManager->CutsDX1();

      // Inert materials, CutB1
      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ1 = dbManager->CutsDZ1(); 

      checking("CutB1", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* CutB1 = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);
      //const GeoShape& CutB = *CutB1;
      CutB = CutB1;

      // Inert materials which are in cuts, special modules 
      // Materials are in cuting region, up Iron plate 
      volname = "IrUp"; dbManager->SetCurrentCuts(volname); 
      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ1 = dbManager->CutsDZ1(); 

      checking("IrUp", false, 1,dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* IrUp = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);
      lvIrUp = new GeoLogVol("IrUp",IrUp,matIron);
      pvIrUp = new GeoPhysVol(lvIrUp); 
      
      // Materials are in cuting region, down Iron plate 
      volname = "IrDw"; dbManager->SetCurrentCuts(volname); 
      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ1 = dbManager->CutsDZ1(); 

      checking("IrDw", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* IrDw = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);
      lvIrDw = new GeoLogVol("IrDw",IrDw,matIron);
      pvIrDw = new GeoPhysVol(lvIrDw); 

      // Materials are in cuting region, 1up Iron plate
      volname = "Cut1up"; dbManager->SetCurrentCuts(volname); //>>
      PosY =  dbManager->CutsYpos();

      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ1 = dbManager->CutsDZ1(); 

      checking("Cut1up", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* Cut1up = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);     

      volname = "Cut2down"; dbManager->SetCurrentCuts(volname); //>>
      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ2 = dbManager->CutsDZ1(); 

      checking("Cut2down", false, 1, dX1,dX2,dY1,dY2,dZ2);
      GeoTrd* Cut1down = new GeoTrd(dX1,dX2,dY1,dY2,dZ2);
 
      HepGeom::Translate3D yPosA(0.,0.,-dZ1-dZ2);
      
      const GeoShapeUnion& CutA1 = Cut1up->add(*Cut1down<<yPosA);
      CutA = &CutA1;

    /*                               |----------> X
                  |---------------------------------------|     (Xdown, -Ydown)
                  |                                   (3) |
                  |---------------------------------------| (2) (Xdown, -Ymiddle) 
                 /                 CutA                    \
                |-------------------------------------------| (1) (Xup,   -Yup)
                                                            
      GeoSimplePolygonBrep *BREP = new GeoSimplePolygonBrep(length/2);
      BREP->addVertex( Xup,   -Yup    );  
      BREP->addVertex( Xdown, -Ymiddle);  
      BREP->addVertex( Xdown, -Ydown  );  
      BREP->addVertex(-Xdown, -Ydown  );  
      BREP->addVertex(-Xdown, -Ymiddle);  
      BREP->addVertex(-Xup,   -Yup    );  
      const GeoShape& ShapeCut2 = *BREP;
     */
      
      // Inert materials which are in cuts, special modules 
      // Materials are in cuting region, down Iron plate (3)
      volname = "Iron3"; dbManager->SetCurrentCuts(volname); //>>
      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ1 = dbManager->CutsDZ1(); 

      checking("Iron3", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* Iron3 = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);
      lvIron3 = new GeoLogVol("Iron3",Iron3,matIron);
      pvIron3 = new GeoPhysVol(lvIron3); 
      
      // Materials are in cuting region, down Iron plate (2)
      volname = "Iron2"; dbManager->SetCurrentCuts(volname); //>>
      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ1 = dbManager->CutsDZ1(); 

      checking("Iron2", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* Iron2 = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);
      lvIron2 = new GeoLogVol("Iron2",Iron2,matIron);
      pvIron2 = new GeoPhysVol(lvIron2); 
      
      // Materials are in cuting region, down Iron plate (1)
      volname = "Iron1"; dbManager->SetCurrentCuts(volname); //>>
      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ1 = dbManager->CutsDZ1(); 

      checking("Iron1", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* Iron1 = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);
      lvIron1 = new GeoLogVol("Iron1",Iron1,matIron);
      pvIron1 = new GeoPhysVol(lvIron1); 

      // Materials are in cuting region, Heavy Iron Box
      volname = "IrBox"; dbManager->SetCurrentCuts(volname); //>>
      dX1 = dbManager->CutsDX1(); 
      dX2 = dbManager->CutsDX2(); 
      dY1 = dbManager->CutsDY1(); 
      dY2 = dbManager->CutsDY2(); 
      dZ1 = dbManager->CutsDZ1(); 

      checking("IrBox", false, 1, dX1,dX2,dY1,dY2,dZ1);
      GeoTrd* IrBox = new GeoTrd(dX1,dX2,dY1,dY2,dZ1);
      lvIrBox = new GeoLogVol("IrBox",IrBox,matIron);
      pvIrBox = new GeoPhysVol(lvIrBox); 

    /*                                           |<-->|----- |Xdown - Xup|
                                 (Xdown,-Ymiddle)    /|   ^ 
                                                    / |   |
                                 (Xup,             /  |   |   CutB
                  |----------> X                  |   |   |
                                 (Xup,-Yup)       |   |   |
                                                   \  |   |
                                                    \ |   |- |Ydown - Ymiddle|
                                 (Xdown,-Ydown)      \|  ---
      (Ydown+|Ydown-Ymiddle|/2)-Yup 
      Y position = Ydown+|Ydown-Ymiddle|/2

      GeoSimplePolygonBrep *BREP = new GeoSimplePolygonBrep(length/4);
      BREP->addVertex( Xup,   -Yup    );
      BREP->addVertex( Xdown, -Ydown  );
      BREP->addVertex( Xdown, -Ymiddle);
      BREP->addVertex(-Xdown, -Ymiddle);    
      BREP->addVertex(-Xdown, -Ydown  );   
      BREP->addVertex(-Xup,   -Yup    );   
      const GeoShape& ShapeCut1 = *BREP;
    */
    } // end if,  BoolCuts
  }

  int NumberOfEnv = dbManager->GetNumberOfEnv();

  for(int EnvCounter = 0; EnvCounter < NumberOfEnv ; ++EnvCounter)
  { // Loop over Envelopes

    dbManager->SetCurrentEnvByIndex(EnvCounter);
    int EnvType = dbManager->GetEnvType();
    dbManager->SetCurrentEnvByType(EnvType);
    int NumberOfMod = dbManager->GetEnvNModules();

    // Break for debugging
    //if(EnvType != 3) continue;

    if(m_log->level()<=MSG::DEBUG)
      (*m_log) << MSG::DEBUG << " EnvCounter " << EnvCounter << " EnvType " << EnvType 
	       << " EnvNModules " << NumberOfMod << endmsg;

    if(EnvType == 1) 
     {
      // nominal position of end plate
      zEndSection = ZLength[1]/2 - BFingerLength;

      GeoTubs* GeneralMother = new GeoTubs(dbManager->GetEnvRin()*CLHEP::cm,
					   dbManager->GetEnvRout()*CLHEP::cm,
					   zEndSection,
					   AnglMin, AnglMax);

      GeoTubs* barrelMother = GeneralMother;      
      GeoLogVol* lvBarrelMother = new GeoLogVol("Barrel",barrelMother,matAir);
      pvBarrelMother = new GeoPhysVol(lvBarrelMother);

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG << "Barrel envelope parameters: " 
		 << " Length=" << zEndSection
		 << " Rmin=" << dbManager->GetEnvRin()*CLHEP::cm
		 << " Rmax=" << dbManager->GetEnvRout()*CLHEP::cm
		 << endmsg;
      
      // Envelopes for two barrel fingers, positive finger 
      GeoTubs* fingerMotherPos = new GeoTubs(BFingerRmin,
                                             dbManager->GetEnvRout()*CLHEP::cm,
                                             BFingerLengthPos/2,
                                             AnglMin, AnglMax);

      GeoLogVol* lvFingerMotherPos = new GeoLogVol("FingerPos",fingerMotherPos,matAir);
      pvFingerMotherPos = new GeoPhysVol(lvFingerMotherPos);

      if (dbManager->BoolSaddle())
       { // Envelopes for barrel saddle supports, positive
         GeoTubs* SaddleMotherPos = new GeoTubs(BFingerRmin-RadiusSaddle,
                                                BFingerRmin,
                                                DzSaddleSupport/2,
                                                AnglMin, AnglMax);

         GeoLogVol* lvSaddleMotherPos = new GeoLogVol("SaddlePos",SaddleMotherPos,matAir);
         pvSaddleMotherPos = new GeoPhysVol(lvSaddleMotherPos);
       }

       // Negative finger
       GeoTubs* fingerMotherNeg = new GeoTubs(BFingerRmin,
                                              dbManager->GetEnvRout()*CLHEP::cm,
                                              BFingerLengthNeg/2,
                                              AnglMin, AnglMax);

       GeoLogVol* lvFingerMotherNeg = new GeoLogVol("FingerNeg",fingerMotherNeg,matAir);
       pvFingerMotherNeg = new GeoPhysVol(lvFingerMotherNeg);

       if(m_log->level()<=MSG::DEBUG)
	 (*m_log) << MSG::DEBUG << "Barrel finger envelope parameters: " 
		  << " length Pos/Neg=" << BFingerLengthPos << "/" << BFingerLengthNeg
		  << " Rmin=" << BFingerRmin << " Rmax=" << dbManager->GetEnvRout()*CLHEP::cm
		  << endmsg;

       // Envelopes for two barrel saddle supports, positive
       if (dbManager->BoolSaddle())
        {          GeoTubs* SaddleMotherNeg = new GeoTubs(BFingerRmin-RadiusSaddle,
                                                          BFingerRmin,
                                                          DzSaddleSupport/2,
                                                          AnglMin, AnglMax);

         GeoLogVol* lvSaddleMotherNeg = new GeoLogVol("SaddleNeg",SaddleMotherNeg,matAir);
         pvSaddleMotherNeg = new GeoPhysVol(lvSaddleMotherNeg);
        }
     }
    
    if(EnvType == 3) {   

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" EBarrelPos DZ "<<(dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthPos)/2<< endmsg;

      checking("EBarrel (+)", false, 0, 
               dbManager->GetEnvRin()*CLHEP::cm,dbManager->GetEnvRout()*CLHEP::cm,
               AnglMin,AnglMax,(dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthPos)/2);

      GeoTubs* GeneralMother = new GeoTubs(dbManager->GetEnvRin()*CLHEP::cm,
					   dbManager->GetEnvRout()*CLHEP::cm,
					   (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthPos)/2,
					   AnglMin, AnglMax);

      GeoTubs* ebarrelMotherPos = GeneralMother;
      GeoLogVol* lvEBarrelMotherPos = new GeoLogVol("EBarrel",ebarrelMotherPos,matAir);
      pvEBarrelMotherPos = new GeoPhysVol(lvEBarrelMotherPos);

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG << "Positive ext.barrel envelope parameters: " 
		 << " length=" << (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLength)
		 << " Rmin=" << dbManager->GetEnvRin()*CLHEP::cm
		 << " Rmax=" << dbManager->GetEnvRout()*CLHEP::cm
		 << endmsg;

      // Envelope for finger separately
      checking("EBarrel (+)", false, 0, 
               EFingerRmin,dbManager->GetEnvRout()*CLHEP::cm,
               AnglMin,AnglMax, EBFingerLength/2);

      GeoTubs* fingerMother = new GeoTubs(EFingerRmin,
					  dbManager->GetEnvRout()*CLHEP::cm,
					  EBFingerLength/2,
					  AnglMin, AnglMax);

      GeoLogVol* lvEFingerMother = new GeoLogVol("EFinger",fingerMother,matAir);
      pvEFingerMotherPos = new GeoPhysVol(lvEFingerMother);

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG << "Positive ext.barrel finger envelope parameters: " 
		 << " length=" << EBFingerLength
		 << " Rmin=" << EFingerRmin
		 << " Rmax=" << (dbManager->GetEnvRout())*CLHEP::cm
		 << endmsg;

      if (dbManager->BoolSaddle())
      { //Envelopes for two barrel saddle supports, positive
        checking("ESaddle (+)", false, 0, 
	         EFingerRmin-RadiusSaddle,EFingerRmin,AnglMin,AnglMax, DzSaddleSupport/2);

        GeoTubs* ESaddleMother = new GeoTubs(EFingerRmin-RadiusSaddle,
	                                     EFingerRmin,
					     DzSaddleSupport/2,
					     AnglMin, AnglMax);

        GeoLogVol* lvESaddleMother = new GeoLogVol("ESaddlePos",ESaddleMother,matAir);
        pvESaddleMotherPos = new GeoPhysVol(lvESaddleMother);
      }
    }

    // Negative Ext.Barrel
    if(EnvType == 2) { 
      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" EBarrelNeg DZ "<<(dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthNeg)/2<< endmsg;

      GeoTubs* GeneralMother = new GeoTubs(dbManager->GetEnvRin()*CLHEP::cm,
					   dbManager->GetEnvRout()*CLHEP::cm,
					   (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthNeg)/2,
					   AnglMin, AnglMax);

      GeoTubs* ebarrelMotherNeg = GeneralMother;
      GeoLogVol* lvEBarrelMotherNeg = new GeoLogVol("EBarrel",ebarrelMotherNeg,matAir);  
      pvEBarrelMotherNeg = new GeoPhysVol(lvEBarrelMotherNeg);
      
      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG << "Nevative ext.barrel envelope parameters: " 
		 << " length=" << (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLength)
		 << " Rmin=" << dbManager->GetEnvRin()*CLHEP::cm
		 << " Rmax=" << dbManager->GetEnvRout()*CLHEP::cm
		 << endmsg;

      // Envelope for finger separately
      GeoTubs* fingerMother = new GeoTubs(EFingerRmin,
					  dbManager->GetEnvRout()*CLHEP::cm,
					  EBFingerLengthNeg/2,
					  AnglMin, AnglMax);
      
      GeoLogVol* lvEFingerMother = new GeoLogVol("EFinger",fingerMother,matAir);
      pvEFingerMotherNeg = new GeoPhysVol(lvEFingerMother);

      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<"Negative ext.barrel finger envelope parameters: " 
		 << " length=" << EBFingerLengthNeg
		 << " Rmin=" << EFingerRmin
		 << " Rmax=" << dbManager->GetEnvRout()*CLHEP::cm
		 << endmsg;

      if (dbManager->BoolSaddle())
      { //Envelopes for Ext. barrel saddle supports, positive 
        checking("ESaddle (-)", false, 0, 
	       EFingerRmin-RadiusSaddle,EFingerRmin,AnglMin,AnglMax, DzSaddleSupport/2);

        GeoTubs* ESaddleMother = new GeoTubs(EFingerRmin-RadiusSaddle,
	                                     EFingerRmin,
					     DzSaddleSupport/2,
					     AnglMin, AnglMax);

        GeoLogVol* lvESaddleMother = new GeoLogVol("ESaddlePos",ESaddleMother,matAir);
        pvESaddleMotherNeg = new GeoPhysVol(lvESaddleMother);
      }
    }
    
    // ITC Positive
    if(EnvType == 5) {

      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);

      rMinITC2 = rMinC10; // dbManager->TILBrminimal();
      rMaxITC2 = dbManager->TILBrmaximal();
      dzITC2   = dbManager->TILBdzmodul();

      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);

      rMinITC1 = dbManager->TILBrminimal();
      rMaxITC1 = dbManager->TILErmam(); 
      dzITC1   = dbManager->TILBdzmodul();
      
      (*m_log) << MSG::INFO << " Positive ITC envelope parameters: PLUG1 " 
               <<" Rmin= "<<rMinITC1*CLHEP::cm<<" Rmax= "<<rMaxITC1*CLHEP::cm<<" dzITC1= "<<dzITC1/2*CLHEP::cm<< endmsg;
      (*m_log) << MSG::INFO << "                                   PLUG2 "
               <<" Rmin= "<<rMinITC2*CLHEP::cm<<" Rmax= "<<rMaxITC2*CLHEP::cm<<" dzITC2= "<<dzITC2/2*CLHEP::cm<< endmsg;

      checking("ITC itcWheel1 (+)", false, 0, 
               rMinITC1*CLHEP::cm,rMaxITC1*CLHEP::cm, AnglMin,AnglMax, dzITC1/2*CLHEP::cm);

      GeoTubs* itcWheel1 = new GeoTubs(rMinITC1*CLHEP::cm,
                                       rMaxITC1*CLHEP::cm,
                                       dzITC1/2*CLHEP::cm,
                                       AnglMin, AnglMax);

      checking("ITC itcWheel2 (+)", false, 0,
               rMinITC1*CLHEP::cm,rMaxITC1*CLHEP::cm, AnglMin,AnglMax, dzITC1/2*CLHEP::cm);

      GeoTubs* itcWheel2 = new GeoTubs(rMinITC2*CLHEP::cm,
                                       rMaxITC2*CLHEP::cm,
                                       dzITC2/2*CLHEP::cm,
                                       AnglMin, AnglMax);
      
      Z = ( dzITC1 - dzITC2)/2*CLHEP::cm;
      HepGeom::Translate3D itcWheel2OffsetPos(0.,0., Z);

      zITCStandard = Z;

      const GeoShapeUnion& itcMotherPos = itcWheel1->add(*itcWheel2<<itcWheel2OffsetPos);

      GeoLogVol* lvITCMotherPos = new GeoLogVol("ITC",&itcMotherPos,matAir);
      pvITCMotherPos = new GeoPhysVol(lvITCMotherPos);

      // Gap Positive
      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);

      checking("Gap (+)", false, 0, 
              dbManager->TILBrminimal()*CLHEP::cm,
              dbManager->TILBrmaximal()*CLHEP::cm/cos(deltaPhi/2*CLHEP::deg),
              AnglMin,AnglMax, dbManager->TILBdzmodul()/2*CLHEP::cm);

      GeoTubs* GapMotherPos = new GeoTubs(dbManager->TILBrminimal()*CLHEP::cm,
	                                  dbManager->TILBrmaximal()*CLHEP::cm/cos(deltaPhi/2*CLHEP::deg),
                                          dbManager->TILBdzmodul()/2*CLHEP::cm,
                                          AnglMin, AnglMax);

      GeoLogVol* lvGapMotherPos = new GeoLogVol("Gap",GapMotherPos,matAir);
      pvGapMotherPos = new GeoPhysVol(lvGapMotherPos);

      // Crack Positive
      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);

      checking("Crack (+)", spE4, 0, 
              rMinE4pos*CLHEP::cm,
              dbManager->TILBrmaximal()*CLHEP::cm/cos(deltaPhi/2*CLHEP::deg),
              AnglMin,AnglMax, dbManager->TILBdzmodul()/2*CLHEP::cm);

      GeoTubs* crackMotherPos = new GeoTubs(rMinE4pos*CLHEP::cm,
				            dbManager->TILBrmaximal()*CLHEP::cm/cos(deltaPhi/2*CLHEP::deg),
				            dbManager->TILBdzmodul()/2*CLHEP::cm,
                                            AnglMin, AnglMax);
    
      GeoLogVol* lvCrackMotherPos = new GeoLogVol("Crack",crackMotherPos,matAir);
      pvCrackMotherPos = new GeoPhysVol(lvCrackMotherPos);

    } 
    // ITC Negative
    if(EnvType == 4) { 

      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG2);

      rMinITC2 = rMinC10; // dbManager->TILBrminimal();
      rMaxITC2 = dbManager->TILBrmaximal();
      dzITC2   = dbManager->TILBdzmodul();

      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);

      rMinITC1 = dbManager->TILBrminimal();
      rMaxITC1 = dbManager->TILErmam();
      dzITC1   = dbManager->TILBdzmodul();
      
      (*m_log) << MSG::INFO << " Negative ITC envelope parameters: PLUG1 " 
               <<" Rmin= "<<rMinITC1*CLHEP::cm<<" Rmax= "<<rMaxITC1*CLHEP::cm<<" dzITC1= "<<dzITC1/2*CLHEP::cm<<endmsg;
      (*m_log) << MSG::INFO << "                                   PLUG2 "
               <<" Rmin= "<<rMinITC2*CLHEP::cm<<" Rmax= "<<rMaxITC2*CLHEP::cm<<" dzITC2= "<<dzITC2/2*CLHEP::cm<<endmsg;

      checking("ITC itcWheel1 (-)", false, 0, 
               rMinITC1*CLHEP::cm,rMaxITC1*CLHEP::cm, AnglMin,AnglMax, dzITC1/2*CLHEP::cm);

      GeoTubs* itcWheel1 = new GeoTubs(rMinITC1*CLHEP::cm,
                                       rMaxITC1*CLHEP::cm,
                                       dzITC1/2*CLHEP::cm,
                                       AnglMin, AnglMax);

      checking("ITC itcWheel2 (-)", false, 0,
               rMinITC1*CLHEP::cm,rMaxITC1*CLHEP::cm, AnglMin,AnglMax, dzITC1/2*CLHEP::cm);

      GeoTubs* itcWheel2 = new GeoTubs(rMinITC2*CLHEP::cm,
                                       rMaxITC2*CLHEP::cm,
                                       dzITC2/2*CLHEP::cm,
                                       AnglMin, AnglMax);

      Z = (-dzITC1 + dzITC2)/2*CLHEP::cm;
      HepGeom::Translate3D itcWheel2OffsetNeg(0.,0., Z);

      zITCStandard = Z;

      const GeoShapeUnion& itcMotherNeg = itcWheel1->add(*itcWheel2<<itcWheel2OffsetNeg);

      GeoLogVol* lvITCMotherNeg = new GeoLogVol("ITC",&itcMotherNeg,matAir);
      pvITCMotherNeg = new GeoPhysVol(lvITCMotherNeg);

      // Gap Negative
      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);

      checking("Gap (-)", false, 1, 
              dbManager->TILBrminimal()*CLHEP::cm,
              dbManager->TILBrmaximal()*CLHEP::cm/cos(deltaPhi/2*CLHEP::deg),
              AnglMin,AnglMax, dbManager->TILBdzmodul()/2*CLHEP::cm);

      GeoTubs* GapMotherNeg = new GeoTubs(dbManager->TILBrminimal()*CLHEP::cm,
	                                  dbManager->TILBrmaximal()*CLHEP::cm/cos(deltaPhi/2*CLHEP::deg),
                                          dbManager->TILBdzmodul()/2*CLHEP::cm,
                                          AnglMin, AnglMax);

      GeoLogVol* lvGapMotherNeg = new GeoLogVol("Gap",GapMotherNeg,matAir);
      pvGapMotherNeg = new GeoPhysVol(lvGapMotherNeg);

      // Crack Negative
      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
  
      checking("Crack (-)", spE4, 0, 
              rMinE4neg*CLHEP::cm,
              dbManager->TILBrmaximal()*CLHEP::cm/cos(deltaPhi/2*CLHEP::deg),
              AnglMin,AnglMax, dbManager->TILBdzmodul()/2*CLHEP::cm);

      GeoTubs* crackMotherNeg = new GeoTubs(rMinE4neg*CLHEP::cm,
				            dbManager->TILBrmaximal()*CLHEP::cm/cos(deltaPhi/2*CLHEP::deg),
				            dbManager->TILBdzmodul()/2*CLHEP::cm,
                                            AnglMin, AnglMax);
    
      GeoLogVol* lvCrackMotherNeg = new GeoLogVol("Crack",crackMotherNeg,matAir);
      pvCrackMotherNeg = new GeoPhysVol(lvCrackMotherNeg);

    }
    
    if(m_fullGeo) {   

    // ModuleNumber[NumberOfMod] array for independent of positrion numeration
    int ModuleIndex[64];
    for(int i=0; i < NumberOfMod; i++){
      ModuleIndex[i] = i+1;
    }

    // the main loop around all phi modules position
    int ModuleNcp =0;

    GeoTransform* yrotMod = new GeoTransform(HepGeom::RotateY3D(90*CLHEP::deg));
    yrotMod->ref();
    GeoTransform* XYrtMod = new GeoTransform(HepGeom::RotateX3D(180*CLHEP::deg) * HepGeom::RotateY3D(90*CLHEP::deg));
    XYrtMod->ref();

    for(int ModCounter = 0; ModCounter < NumberOfMod; ModCounter++){

      ModuleNcp = ModuleIndex[ModCounter];

      // General rotation and transformations
      double phi = (double(ModuleNcp-1) + 0.5)*deltaPhi;
      double ph1 = (double(ModuleNcp-1))*deltaPhi;

      GeoTransform* zrotMod = new GeoTransform(HepGeom::RotateZ3D(phi*CLHEP::deg));
      zrotMod->ref();
      GeoTransform* zrotSaddle =  new GeoTransform(HepGeom::RotateZ3D(ph1*CLHEP::deg));
      zrotSaddle->ref();

      dbManager->SetCurrentModuleByIndex(ModuleNcp-1);
      int ModType = dbManager->GetModType();
      int ModFingpattern = dbManager->TILBfingpattern(); 

      // Debuging code for cuted modules
      if(!(ModuleNcp>=NcpFrom && ModuleNcp<=NcpFrom+NcpPlus)) continue;

      // special Modules with Cuts
      //if(ModuleNcp<35 || ModuleNcp>62) continue; if(ModuleNcp>37 && ModuleNcp<60) continue;

      // Saddle Supports
      //if(ModuleNcp<39 || ModuleNcp>58) continue; if(ModuleNcp>42 && ModuleNcp<55) continue;
      
      if(m_log->level()<=MSG::DEBUG)
	(*m_log) << MSG::DEBUG <<" ModuleNcp= "<< ModuleNcp <<" ModType "<< ModType <<" Phi "<< phi << endmsg;

      //
      // ------------------- BARREL BLOCKS ------------------------ 
      //    
 
      if( EnvType == 1 ) { // normal barrel module or module zero 

	dbManager->SetCurrentSectionByNumber(ModType);

        thicknessWedgeMother = dbManager->TILBdzmodul() * CLHEP::cm; 
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * CLHEP::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
       
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() 
               - (dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()) 
               -  dbManager->TILBdzmast()))/(2.*(2.*dbManager->TILBnperiod() - 1));
        
        GeoTrd* barrelModuleMother = new GeoTrd(thicknessWedgeMother/2,
                                                thicknessWedgeMother/2,
                                                dy1WedgeMother,
                                                dy2WedgeMother,
                                                heightWedgeMother/2);
       
        GeoLogVol* lvBarrelModuleMother = new GeoLogVol("BarrelModule",barrelModuleMother,matAir);
        GeoPhysVol* pvBarrelModuleMother = new GeoPhysVol(lvBarrelModuleMother);
       
        // Fill the section
	if(Filling){ 
	  sectionBuilder->fillSection(pvBarrelModuleMother, 1,
				      dbManager->TILBrmaximal(),
				      dbManager->TILBrminimal(),
				      dzGlue,
				      deltaPhi); 
	}
       
        GeoTransform* xtraMod = new GeoTransform(HepGeom::TranslateX3D(
                                    (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2 * CLHEP::cm));

        pvBarrelMother->add(zrotMod);
        pvBarrelMother->add(xtraMod);
        pvBarrelMother->add(XYrtMod);

        pvBarrelMother->add(new GeoIdentifierTag(ModuleNcp));
        pvBarrelMother->add(pvBarrelModuleMother);
       
        // --------------------------BARREL FINGERS MAKING---------------------------- 
       
	dbManager->SetCurrentTifg(1);  // Barrel finger 
        zEndSection = dbManager->TILBzoffset() + dbManager->TILBdzmodul()/2;

        heightWedgeMother = (dbManager->TILErmax() - dbManager->TILBrmax())*CLHEP::cm;
        dy1WedgeMother = dbManager->TILBrmax() * tan(deltaPhi/2*CLHEP::deg)*CLHEP::cm;
        dy2WedgeMother = dbManager->TILErmax() * tan(deltaPhi/2*CLHEP::deg)*CLHEP::cm;

	// Finger Positive, positioning (only Left ModFingpattern == 10) 
	if ( ModFingpattern != 10 ) {
	  GeoTrd* fingerModuleMotherPos = new GeoTrd(BFingerLengthPos/2,
						     BFingerLengthPos/2,
						     dy1WedgeMother,
						     dy2WedgeMother,
						     heightWedgeMother/2);
	  
	  GeoLogVol* lvFingerModuleMotherPos = new GeoLogVol("FingerModule",fingerModuleMotherPos,matAir);
	  GeoPhysVol* pvFingerModuleMotherPos = new GeoPhysVol(lvFingerModuleMotherPos);

	  if(Filling)
	    sectionBuilder->fillFinger(pvFingerModuleMotherPos, 1,
				       dbManager->TILErmax(),
				       dbManager->TILBrmax(),
				       deltaPhi,
				       ModuleNcp,
				       BFingerLengthPos*(1./CLHEP::cm)); 

	  GeoTransform* xtraModFingerPos  = new GeoTransform(HepGeom::TranslateX3D((dbManager->TILErmax() + dbManager->TILBrmax())/2*CLHEP::cm));

          //(*m_log) << MSG::DEBUG << "R  Index " << ModuleNcp << " Fingpattern "<< ModFingpattern << endmsg;
 
          pvFingerMotherPos->add(zrotMod);
          pvFingerMotherPos->add(xtraModFingerPos);
          pvFingerMotherPos->add(XYrtMod);

          pvFingerMotherPos->add(new GeoIdentifierTag(ModuleNcp));
          pvFingerMotherPos->add(pvFingerModuleMotherPos);
	} 
       
        // Finger Negative, positioning (only Left ModFingpattern == 01) 
        if ( ModFingpattern != 1 ) {

	  GeoTrd* fingerModuleMotherNeg = new GeoTrd(BFingerLengthNeg/2,
						     BFingerLengthNeg/2,
						     dy1WedgeMother,
						     dy2WedgeMother,
						     heightWedgeMother/2);
	  
	  GeoLogVol* lvFingerModuleMotherNeg = new GeoLogVol("FingerModule",fingerModuleMotherNeg,matAir);
	  GeoPhysVol* pvFingerModuleMotherNeg = new GeoPhysVol(lvFingerModuleMotherNeg);

	  if(Filling)
	    sectionBuilder->fillFinger(pvFingerModuleMotherNeg, 1,
				       dbManager->TILErmax(),
				       dbManager->TILBrmax(),
				       deltaPhi,
				       ModuleNcp*100,
				       BFingerLengthNeg*(1./CLHEP::cm));

	  GeoTransform* xtraModFingerNeg  = new GeoTransform(HepGeom::TranslateX3D((dbManager->TILErmax() + dbManager->TILBrmax())/2*CLHEP::cm));

          // (*m_log) << MSG::DEBUG << "L  Index " << ModuleNcp << " Fingpattern "<< ModFingpattern << endmsg;

          pvFingerMotherNeg->add(zrotMod);
          pvFingerMotherNeg->add(xtraModFingerNeg);
          pvFingerMotherNeg->add(yrotMod);

          pvFingerMotherNeg->add(new GeoIdentifierTag(ModuleNcp));
          pvFingerMotherNeg->add(pvFingerModuleMotherNeg);
        }
       
        // --------------------------BARREL SADDLE MAKING----------------------------
        if (dbManager->BoolSaddle())
        { if ( (ModuleNcp >=40 && ModuleNcp <=41) || (ModuleNcp >=56 && ModuleNcp <=57) )
	  { 
            GeoTubs* SaddleModule = new GeoTubs(BFingerRmin-RadiusSaddle,
                                                BFingerRmin,
                                                DzSaddleSupport/2,
                                                0.,deltaPhi*CLHEP::deg);
     
            GeoLogVol* lvSaddleModule = new GeoLogVol("SaddleModule",SaddleModule,matIron);
            GeoPhysVol* pvSaddleModule = new GeoPhysVol(lvSaddleModule);
       
            pvSaddleMotherPos->add(zrotSaddle);
            pvSaddleMotherPos->add(new GeoIdentifierTag(ModuleNcp));
            pvSaddleMotherPos->add(pvSaddleModule); 

            pvSaddleMotherNeg->add(zrotSaddle);
            pvSaddleMotherNeg->add(new GeoIdentifierTag(ModuleNcp));
            pvSaddleMotherNeg->add(pvSaddleModule); 
          }
         }
      }

      //------------------- Extended Barrel Module Positive ----------------------------------------------------------

      if (EnvType == 3) {

	dbManager->SetCurrentSectionByNumber(ModType);

        // Mother module 
        thicknessWedgeMother = dbManager->TILBdzmodul() * CLHEP::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * CLHEP::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() 
                - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() + dbManager->TILBdzspac()))/(4.*
                  dbManager->TILBnperiod());
        
        double Radius = (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2 * CLHEP::cm;

        checking("EBarrelModule (+)", false, 1, 
                 thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

        GeoTrd* ebarrelModuleMotherPos = new GeoTrd(thicknessWedgeMother/2,
                                                    thicknessWedgeMother/2,
                                                    dy1WedgeMother,
                                                    dy2WedgeMother,
                                                    heightWedgeMother/2);

        if (!dbManager->BoolCuts())
	 { 
	   if(m_log->level()<=MSG::DEBUG)
	     (*m_log) << MSG::DEBUG << " BoolCuts NO "<< dbManager->BoolCuts() << endmsg;

           lvEBarrelModuleMotherPos = new GeoLogVol("EBarrelModule",ebarrelModuleMotherPos,matAir);
           pvEBarrelModuleMotherPos = new GeoPhysVol(lvEBarrelModuleMotherPos);

         } else {

           //gdb
	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG << " BoolCuts YES "<< dbManager->BoolCuts() << endmsg;
 
           double PoZ2 =0, PoZ1 =0, thicknessEndPlate =dbManager->TILBdzend1()*CLHEP::cm;

           PoZ1 = thicknessEndPlate + modl_length/4 - (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthPos)/2;
           PoZ2 = PoZ1 + modl_length/4;

           if ((ModuleNcp>=35 && ModuleNcp<=37) || (ModuleNcp>=60 && ModuleNcp<=62))
	    { HepGeom::Transform3D  TransCut2 = HepGeom::TranslateZ3D(-Radius) * HepGeom::RotateX3D((90-phi)*CLHEP::deg) * HepGeom::RotateY3D(180*CLHEP::deg)
                                        * HepGeom::Translate3D(-PoZ2,0.,-PosY);
 
              if (ModuleNcp>=60 && ModuleNcp<=62)
	       { HepGeom::Transform3D TransCutL = HepGeom::TranslateZ3D(-Radius)
                                          * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateX3D(phi*CLHEP::deg)
                                          * HepGeom::Translate3D(PoZ1,PosYcut,-PosXcut);

                 // Cuting of module (Left)
                 const GeoShape& TmL_EBarrelModuleMotherPos = ebarrelModuleMotherPos->subtract((*CutA)<<TransCut2).
                                                                                      subtract((*CutB)<<TransCutL);

                 lvEBarrelModuleMotherPos = new GeoLogVol("EBarrelModule",&TmL_EBarrelModuleMotherPos,matAir);

               } else if (ModuleNcp>=35 && ModuleNcp<=37) 
	       { HepGeom::Transform3D TransCutR = HepGeom::TranslateZ3D(-Radius)
                                          * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateX3D(phi*CLHEP::deg)
                                          * HepGeom::Translate3D(PoZ1,PosYcut,PosXcut) * HepGeom::RotateY3D(180*CLHEP::deg);

                 // Cuting of module (Right)
                 const GeoShape& TmR_EBarrelModuleMotherPos = ebarrelModuleMotherPos->subtract((*CutA)<<TransCut2).
                                                                                      subtract((*CutB)<<TransCutR);

                 lvEBarrelModuleMotherPos = new GeoLogVol("EBarrelModule",&TmR_EBarrelModuleMotherPos,matAir);
	       }

              pvEBarrelModuleMotherPos = new GeoPhysVol(lvEBarrelModuleMotherPos);

	    } else {
              lvEBarrelModuleMotherPos = new GeoLogVol("EBarrelModule",ebarrelModuleMotherPos,matAir);
              pvEBarrelModuleMotherPos = new GeoPhysVol(lvEBarrelModuleMotherPos);
            } // end special modules
	 } // end if, BoolCuts()

        // Fill the section
        if(Filling){
	  sectionBuilder->fillSection(pvEBarrelModuleMotherPos, 2,
                                      dbManager->TILBrmaximal(),
                                      dbManager->TILBrminimal(),
                                      dzGlue,
                                      deltaPhi,
                                      ModuleNcp);
	}

        GeoTransform* xtraModPos = new GeoTransform(HepGeom::TranslateX3D(Radius));

        pvEBarrelMotherPos->add(zrotMod);
        pvEBarrelMotherPos->add(xtraModPos);
        pvEBarrelMotherPos->add(XYrtMod);

        pvEBarrelMotherPos->add(new GeoIdentifierTag(ModuleNcp));
        pvEBarrelMotherPos->add(pvEBarrelModuleMotherPos);

        /*
        if (!dbManager->BoolCuts())
	 { pvEBarrelMotherPos->add(pvEBarrelModuleMotherPos);
           (*m_log) << MSG::DEBUG << " BoolCuts NO "<< dbManager->BoolCuts() << endmsg;

         } else {

           (*m_log) << MSG::DEBUG << " BoolCuts YES "<< dbManager->BoolCuts() << endmsg;
 
           double PoZ2 =0, PoZ1 =0;
           double thicknessEndPlate = dbManager->TILBdzend1()*CLHEP::cm;

           PoZ1 = thicknessEndPlate + modl_length/4 - (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthPos)/2;
           PoZ2 = modl_length/4 + PoZ1;

           if ((ModuleNcp>=35 && ModuleNcp<=37) || (ModuleNcp>=60 && ModuleNcp<=62))
	    { HepGeom::Transform3D  TransCut2 = HepGeom::TranslateZ3D(-Radius) * HepGeom::RotateX3D((90-phi)*CLHEP::deg) * HepGeom::RotateY3D(180*CLHEP::deg)
                                        * HepGeom::Translate3D(-PoZ2,0.,-PosY);
 
              // Cuting of pvEBarrelModuleMotherPos (-)
              GeoCutVolAction action1(*CutA, TransCut2);
              pvEBarrelModuleMotherPos->apply(&action1);
              pvTmp_EBarrelModuleMotherPos = action1.getPV();
              pvEBarrelModuleMotherPos->unref(); // Cleaning useless volume

              if (ModuleNcp>=60 && ModuleNcp<=62)
	       { HepGeom::Transform3D TransCutL = HepGeom::TranslateZ3D(-Radius)
                                          * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateX3D(phi*CLHEP::deg)
                                          * HepGeom::Translate3D(PoZ1,PosYcut,-PosXcut);

                 // Cuting of pvEBarrelModuleMotherPos (Left)
                 GeoCutVolAction action2(*CutB, TransCutL);
                 pvTmp_EBarrelModuleMotherPos->apply(&action2);
                 pvTmL_EBarrelModuleMotherPos = action2.getPV();
                 pvTmp_EBarrelModuleMotherPos->unref();// Cleaning useless volume

                 pvEBarrelMotherPos->add(pvTmL_EBarrelModuleMotherPos);

               } else if (ModuleNcp>=35 && ModuleNcp<=37) 
	       { HepGeom::Transform3D TransCutR = HepGeom::TranslateZ3D(-Radius)
                                          * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateX3D(phi*CLHEP::deg)
                                          * HepGeom::Translate3D(PoZ1,PosYcut,PosXcut) * HepGeom::RotateY3D(180*CLHEP::deg);

                 // Cuting of pvEBarrelModuleMotherPos (Right)
                 GeoCutVolAction action3(*CutB, TransCutR);
                 pvTmp_EBarrelModuleMotherPos->apply(&action3);
                 pvTmR_EBarrelModuleMotherPos = action3.getPV();
                 pvTmp_EBarrelModuleMotherPos->unref();// Cleaning useless volume
 
                 pvEBarrelMotherPos->add(pvTmR_EBarrelModuleMotherPos);
	       }
	    } 	
            else  
            { pvEBarrelMotherPos->add(pvEBarrelModuleMotherPos);
            } // end special modules
              pvEBarrelMotherPos->add(pvEBarrelModuleMotherPos);
         } // end if, BoolCuts()
	*/  

        //--------------------------EBARREL FINGERS MAKING------------------------------
  
        dbManager->SetCurrentTifg(2);  //barrel efinger (small)
        
        // Trd - one finger mother
        thicknessWedgeMother = dbManager->TIFGdz() * CLHEP::cm;
        heightWedgeMother = (dbManager->TILErmax() - dbManager->TILBrmax()) * CLHEP::cm;
        dy1WedgeMother = dbManager->TILBrmax() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        dy2WedgeMother = dbManager->TILErmax() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        
        checking("EFingerModule (+)", false,  1, 
                 thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

        GeoTrd* efingerModuleMother = new GeoTrd(thicknessWedgeMother/2,
                                                 thicknessWedgeMother/2,
                                                 dy1WedgeMother,
                                                 dy2WedgeMother,
                                                 heightWedgeMother/2);
        
        GeoLogVol* lvEFingerModuleMother = new GeoLogVol("EFingerModule",efingerModuleMother,matAir);
        GeoPhysVol* pvEFingerModuleMother = new GeoPhysVol(lvEFingerModuleMother);
        
        // Fill the section
        if (Filling)
         { sectionBuilder->fillFinger(pvEFingerModuleMother, 2,
                                      dbManager->TILErmax(),
                                      dbManager->TILBrmax(),
                                      deltaPhi,
                                      ModuleNcp);
	 }
        GeoTransform* xtraModFingerPos  = new GeoTransform(HepGeom::TranslateX3D(
                                              (dbManager->TILErmax() + dbManager->TILBrmax())/2*CLHEP::cm));
        pvEFingerMotherPos->add(zrotMod);
        pvEFingerMotherPos->add(xtraModFingerPos);
        pvEFingerMotherPos->add(XYrtMod);

        pvEFingerMotherPos->add(new GeoIdentifierTag(ModuleNcp));
        pvEFingerMotherPos->add(pvEFingerModuleMother);

        // --------------------------BARREL SADDLE MAKING----------------------------
        if (dbManager->BoolSaddle())
        { if ( (ModuleNcp >=39 && ModuleNcp <=42) || (ModuleNcp >=55 && ModuleNcp <=58) )
	  { 
            GeoTubs* SaddleModule = new GeoTubs(BFingerRmin-RadiusSaddle,
                                                BFingerRmin,
                                                DzSaddleSupport/2,
                                                0.,deltaPhi*CLHEP::deg);
     
            GeoLogVol* lvSaddleModule = new GeoLogVol("SaddleModule",SaddleModule,matIron);
            GeoPhysVol* pvSaddleModule = new GeoPhysVol(lvSaddleModule);

            pvESaddleMotherPos->add(zrotSaddle);
            pvESaddleMotherPos->add(new GeoIdentifierTag(ModuleNcp));
            pvESaddleMotherPos->add(pvSaddleModule); 
          }
         }

      } // end if (EnvType == 3)
      
      //------------------- Extended Barrel Module Negative ----------------------------------------------------------
 
      if (EnvType == 2) {

	dbManager->SetCurrentSectionByNumber(ModType);

        // Mother module
        thicknessWedgeMother = dbManager->TILBdzmodul() * CLHEP::cm;
        heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * CLHEP::cm;
        dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        
        dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() 
                - dbManager->TILBnperiod()*2.*(dbManager->TILBdzmast() 
                + dbManager->TILBdzspac()))/(4.*dbManager->TILBnperiod());
  
        double Radius = (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2 * CLHEP::cm;

        checking("EBarrelModule (-)", false, 1, 
                 thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

        GeoTrd* ebarrelModuleMotherNeg = new GeoTrd(thicknessWedgeMother/2,
                                                    thicknessWedgeMother/2,
                                                    dy1WedgeMother,
                                                    dy2WedgeMother,
                                                    heightWedgeMother/2);
        
         if (!dbManager->BoolCuts())
	 { 
	   if(m_log->level()<=MSG::DEBUG)
	     (*m_log) << MSG::DEBUG << " BoolCuts NO "<< dbManager->BoolCuts() << endmsg;

           lvEBarrelModuleMotherNeg = new GeoLogVol("EBarrelModule",ebarrelModuleMotherNeg,matAir);
           pvEBarrelModuleMotherNeg = new GeoPhysVol(lvEBarrelModuleMotherNeg);

         } else {

           //gdb
	   if(m_log->level()<=MSG::DEBUG)
	     (*m_log) << MSG::DEBUG << " BoolCuts YES "<< dbManager->BoolCuts() << endmsg;
 
           double PoZ2 =0, PoZ1 =0, thicknessEndPlate = dbManager->TILBdzend1()*CLHEP::cm;

           PoZ1 = thicknessEndPlate + modl_length/4 - (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthNeg)/2;
           PoZ2 = PoZ1 + modl_length/4;

           if ((ModuleNcp>=35 && ModuleNcp<=37) || (ModuleNcp>=60 && ModuleNcp<=62))
	    { HepGeom::Transform3D  TransCut2 = HepGeom::TranslateZ3D(-Radius) 
                                        * HepGeom::RotateX3D((phi-90)*CLHEP::deg) * HepGeom::RotateY3D(180*CLHEP::deg)
                                        * HepGeom::Translate3D(-PoZ2,0,-PosY);
 
              if (ModuleNcp>=60 && ModuleNcp<=62)
	       { HepGeom::Transform3D TransCutL = HepGeom::TranslateZ3D(-Radius)
                                          * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateX3D(-phi*CLHEP::deg)
                                          * HepGeom::Translate3D(PoZ1,-PosYcut,-PosXcut);

                 // Cuting of module (Left)
                 const GeoShape& TmL_EBarrelModuleMotherNeg = ebarrelModuleMotherNeg->subtract((*CutA)<<TransCut2).
                                                                                      subtract((*CutB)<<TransCutL);

                 lvEBarrelModuleMotherNeg = new GeoLogVol("EBarrelModule",&TmL_EBarrelModuleMotherNeg,matAir);

               } else if (ModuleNcp>=35 && ModuleNcp<=37)
	       { HepGeom::Transform3D TransCutR = HepGeom::TranslateZ3D(-Radius)
                                          * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateX3D(-phi*CLHEP::deg)
                                          * HepGeom::Translate3D(PoZ1,-PosYcut,PosXcut) * HepGeom::RotateY3D(180*CLHEP::deg);

                 // Cuting of module (Right)
                 const GeoShape& TmR_EBarrelModuleMotherNeg = ebarrelModuleMotherNeg->subtract((*CutA)<<TransCut2).
                                                                                      subtract((*CutB)<<TransCutR);

                 lvEBarrelModuleMotherNeg = new GeoLogVol("EBarrelModule",&TmR_EBarrelModuleMotherNeg,matAir);
	       }

              pvEBarrelModuleMotherNeg = new GeoPhysVol(lvEBarrelModuleMotherNeg);

	    } else {
              lvEBarrelModuleMotherNeg = new GeoLogVol("EBarrelModule",ebarrelModuleMotherNeg,matAir);
              pvEBarrelModuleMotherNeg = new GeoPhysVol(lvEBarrelModuleMotherNeg);
            } // end special modules
	 } // end if, BoolCuts()

        // Fill the section
        if (Filling)
         {sectionBuilder->fillSection(pvEBarrelModuleMotherNeg, 2,
                                      dbManager->TILBrmaximal(),
                                      dbManager->TILBrminimal(),
                                      dzGlue,
                                      deltaPhi,
                                      ModuleNcp,
				      0.,true);
         }

        GeoTransform* xtraModNeg = new GeoTransform(HepGeom::TranslateX3D(Radius));

        pvEBarrelMotherNeg->add(zrotMod);
        pvEBarrelMotherNeg->add(xtraModNeg);
        pvEBarrelMotherNeg->add(yrotMod);

        pvEBarrelMotherNeg->add(new GeoIdentifierTag(ModuleNcp));
        pvEBarrelMotherNeg->add(pvEBarrelModuleMotherNeg);

        /*
        if (!dbManager->BoolCuts()) 
         { pvEBarrelMotherNeg->add(pvEBarrelModuleMotherNeg);
           (*m_log) << MSG::DEBUG << " BoolCuts NO "<< dbManager->BoolCuts() << endmsg;

         } else {
           (*m_log) << MSG::DEBUG << " BoolCuts YES "<< dbManager->BoolCuts() << endmsg;

           double PoZ2 =0, PoZ1 =0;
           double thicknessEndPlate = dbManager->TILBdzend1()*CLHEP::cm;

           PoZ1 = thicknessEndPlate + modl_length/4 - (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthPos)/2;
           PoZ2 = modl_length/4 + PoZ1;

          if ((ModuleNcp>=35 && ModuleNcp<=37) || (ModuleNcp>=60 && ModuleNcp<=62))
	   { HepGeom::Transform3D  TransCut2 = HepGeom::TranslateZ3D(-Radius) 
                                       * HepGeom::RotateX3D((phi-90)*CLHEP::deg) * HepGeom::RotateY3D(180*CLHEP::deg)   
                                       * HepGeom::Translate3D(-PoZ2,0.,-PosY);

             // Cuting of pvEBarrelModuleMotherNeg (-)
             GeoCutVolAction action1(*CutA, TransCut2);
             pvEBarrelModuleMotherNeg->apply(&action1);
             pvTmp_EBarrelModuleMotherNeg = action1.getPV();
             pvEBarrelModuleMotherNeg->unref(); // Cleaning useless volume

             if (ModuleNcp>=60 && ModuleNcp<=62)
	      { HepGeom::Transform3D TransCutL = HepGeom::TranslateZ3D(-Radius)
                                         * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateX3D(-phi*CLHEP::deg)
                                         * HepGeom::Translate3D(PoZ1,-PosYcut,-PosXcut);

                // Cuting of pvEBarrelModuleMotherNeg (Left)
                GeoCutVolAction action2(*CutB, TransCutL);
                pvTmp_EBarrelModuleMotherNeg->apply(&action2);
                pvTmL_EBarrelModuleMotherNeg = action2.getPV();
                pvTmp_EBarrelModuleMotherNeg->unref();// Cleaning useless volume

                pvEBarrelMotherNeg->add(pvTmL_EBarrelModuleMotherNeg);

              } else if (ModuleNcp>=35 && ModuleNcp<=37) 
	      { HepGeom::Transform3D TransCutR = HepGeom::TranslateZ3D(-Radius)
                                         * HepGeom::RotateY3D(180*CLHEP::deg) * HepGeom::RotateX3D(-phi*CLHEP::deg)
                                         * HepGeom::Translate3D(PoZ1,-PosYcut,PosXcut) * HepGeom::RotateY3D(180*CLHEP::deg);

                // Cuting of pvEBarrelModuleMotherNeg (Right)
                GeoCutVolAction action3(*CutB, TransCutR);
                pvTmp_EBarrelModuleMotherNeg->apply(&action3);
                pvTmR_EBarrelModuleMotherNeg = action3.getPV();
                pvTmp_EBarrelModuleMotherNeg->unref();// Cleaning useless volume

                pvEBarrelMotherNeg->add(pvTmR_EBarrelModuleMotherNeg);
              }
	   } else  
           { pvEBarrelMotherNeg->add(pvEBarrelModuleMotherNeg);
           } // end special modules
             pvEBarrelMotherNeg->add(pvEBarrelModuleMotherNeg);
	} // end if, BoolCuts()
        */

        //--------------------------EBARREL FINGERS MAKING------------------------------
        
        dbManager->SetCurrentTifg(2);  //barrel efinger (small)
        
        //zEndSection = extOffset + dbManager->TILBdzmodul()/2 + dbManager->TILEzshift();

        // Trd - one finger mother
        thicknessWedgeMother = dbManager->TIFGdz() * CLHEP::cm;
        heightWedgeMother = (dbManager->TILErmax() - dbManager->TILBrmax()) * CLHEP::cm;
        dy1WedgeMother = dbManager->TILBrmax() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        dy2WedgeMother = dbManager->TILErmax() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
        
        checking("EFingerModule (-)", false, 1, 
                 thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

        GeoTrd* efingerModuleMother = new GeoTrd(thicknessWedgeMother/2,
                                                 thicknessWedgeMother/2,
                                                 dy1WedgeMother,
                                                 dy2WedgeMother,
                                                 heightWedgeMother/2);
        
        GeoLogVol* lvEFingerModuleMother = new GeoLogVol("EFingerModule",efingerModuleMother,matAir);
        GeoPhysVol* pvEFingerModuleMother = new GeoPhysVol(lvEFingerModuleMother);
        
        // Fill the section
        if (Filling)
         { sectionBuilder->fillFinger(pvEFingerModuleMother, 2,
                                      dbManager->TILErmax(),
                                      dbManager->TILBrmax(),
                                      deltaPhi,
                                      ModuleNcp*100);
         }
        GeoTransform* xtraModFingerNeg  = new GeoTransform(HepGeom::TranslateX3D(
                                              (dbManager->TILErmax() + dbManager->TILBrmax())/2*CLHEP::cm));
        pvEFingerMotherNeg->add(zrotMod);
        pvEFingerMotherNeg->add(xtraModFingerNeg); 
        pvEFingerMotherNeg->add(yrotMod);

        pvEFingerMotherNeg->add(new GeoIdentifierTag(ModuleNcp));
        pvEFingerMotherNeg->add(pvEFingerModuleMother);

        // --------------------------BARREL SADDLE MAKING----------------------------
        if (dbManager->BoolSaddle())
        { if ( (ModuleNcp >=39 && ModuleNcp <=42) || (ModuleNcp >=55 && ModuleNcp <=58) )
	  { 
            GeoTubs* SaddleModule = new GeoTubs(BFingerRmin-RadiusSaddle,
                                                BFingerRmin,
                                                DzSaddleSupport/2,
                                                0.,deltaPhi*CLHEP::deg);
     
            GeoLogVol* lvSaddleModule = new GeoLogVol("SaddleModule",SaddleModule,matIron);
            GeoPhysVol* pvSaddleModule = new GeoPhysVol(lvSaddleModule);

            pvESaddleMotherNeg->add(zrotSaddle);
            pvESaddleMotherNeg->add(new GeoIdentifierTag(ModuleNcp));
            pvESaddleMotherNeg->add(pvSaddleModule); 
          }
         }

      } // end if (EnvType == 2)

      //------------------- D4/C10/Gap/Crack

      if((EnvType == 4) || (EnvType == 5)){

        int Id4 = ModType%100;
        int Ic10 = (ModType/100)%100;
        int Igap = (ModType/10000)%100;
        int Icrack = (ModType/1000000)%100;

        bool Ifd4    = ( Id4 != 0);
        bool Ifc10   = ( Ic10 != 0);
        bool Ifgap   = ( Igap != 0);
        bool Ifcrack = ( Icrack != 0);

	bool Ifspecialgirder = (Id4 == 7);

	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG <<" ITC : EnvType "<<EnvType<<" Size = "<<dbManager->GetModTypeSize()
		   <<" Ncp= "<<ModuleNcp<<"  D4 "<<Id4<<" specialflag = "<<Ifspecialgirder<<" C10 "<<Ic10<<" Gap "<<Igap<<" Crack "<<Icrack
		   << endmsg;

        if (Ifc10) 
         { dbManager->SetCurrentSectionByNumber(Ic10); // TILE_PLUG2
           rMinITC2 = dbManager->TILBrminimal();
           rMaxITC2 = dbManager->TILBrmaximal();
           dzITC2   = dbManager->TILBdzmodul();
         } else { 
	   if(m_log->level()<=MSG::DEBUG)
	     (*m_log) << MSG::DEBUG <<" C10 unavailable "<<endmsg;
         }

        if (Ifd4) 
         { dbManager->SetCurrentSectionByNumber(Id4); // TILE_PLUG1
           rMinITC1 = dbManager->TILBrminimal();
           rMaxITC1 = dbManager->TILBrmaximal();
           dzITC1   = dbManager->TILBdzmodul();
	   if (Ifspecialgirder)
	     dzITC1 = dbManager->TILBdzgir();
         } else
         { (*m_log) << MSG::INFO <<" D4 unavailable "<<endmsg;
	   dzITC1 = 9.485; //sb [CLHEP::cm]
         }

        bool specialC10 = (Ifd4 && Ifc10 && rMaxITC2 < rMinITC1);
        double rMaxITC20 = rMaxITC2; // remember real max radius of C10 cell to position it properly
        if (specialC10) {
          rMaxITC2 = rMinITC1; // for special C10 make outer radius equal to inner radius of D4
        }
        
        //------------------- ITC BLOCKS Negative --------------------------------    
	if (EnvType == 4) {

          // Common mother for ITC1/2 modules
          if (Ifd4 || Ifc10) {

            //  The first sub shape
            thicknessWedgeMother = dzITC1 * CLHEP::cm;
            heightWedgeMother = (rMaxITC1 - rMinITC1) * CLHEP::cm;
            dy1WedgeMother = rMinITC1 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
            dy2WedgeMother = rMaxITC1 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

            checking("ITCModule tcModuleSub1Neg (-) ", false, 1, 
                 thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

            GeoTrd* itcModuleSub1Neg = new GeoTrd(thicknessWedgeMother/2,
  	                                          thicknessWedgeMother/2,
                                                  dy1WedgeMother,
                                                  dy2WedgeMother,
                                                  heightWedgeMother/2);
            // The second sub shape
            thicknessWedgeMother = dzITC2 * CLHEP::cm;
            heightWedgeMother    = (rMaxITC2 - rMinITC2) * CLHEP::cm;
            dy1WedgeMother = rMinITC2 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
            dy2WedgeMother = rMaxITC2 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

            checking("ITCModule itcModuleSub2Neg (-)", false, 1, 
                 thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

            GeoTrd* itcModuleSub2Neg = new GeoTrd(thicknessWedgeMother/2,
	                                          thicknessWedgeMother/2,
                                                  dy1WedgeMother,
                                                  dy2WedgeMother,
                                                  heightWedgeMother/2 );

            X = (dzITC1 - dzITC2)/2*CLHEP::cm; 
            Z = ((rMinITC2+rMaxITC2)-(rMaxITC1 + rMinITC1))/2*CLHEP::cm;
	    if(m_log->level()<=MSG::DEBUG)
	      (*m_log) << MSG::DEBUG <<"  ITCModule Negative, position X= "<<X<<" Z= "<<Z<< endmsg;

            HepGeom::Translate3D itcModule_SubShiftNeg(X, 0., Z);
            const GeoShapeUnion& itcModuleMotherNeg = itcModuleSub1Neg->add(*itcModuleSub2Neg<<itcModule_SubShiftNeg);

            if (specialC10) Z -= (rMaxITC2-rMaxITC20)/2*CLHEP::cm;
            HepGeom::Translate3D itcModuleSubShiftNeg(X, 0., Z);

            GeoLogVol* lvITCModuleMotherNeg = new GeoLogVol("ITCModule",&itcModuleMotherNeg,matAir);
            GeoPhysVol* pvITCModuleMotherNeg = new GeoPhysVol(lvITCModuleMotherNeg);

            //Mother volume for ITC1
            //In plug1 it's necessary to produce GeoShapeUnion for mother volume that is composed by two parts:
            // 1. Mother for absorber and girder
            // 2. Mother for frontplate (since it's short)
          
            // The D4, PLUG1
            dbManager->SetCurrentSectionByNumber(Id4);

            thicknessWedgeMother = dzITC1 * CLHEP::cm;
            heightWedgeMother = (rMaxITC1 - dbManager->TILBrmin()) * CLHEP::cm;
            dy1WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
            dy2WedgeMother = rMaxITC1 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
	    // ps changes dzITC1 -> dzmodul
            Glue =  dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2();
            NbPeriod = dbManager->TILBnperiod();

	    if(m_log->level()<=MSG::DEBUG)
	      (*m_log) << MSG::DEBUG <<"  Plug1Module : Glue= "<<Glue<<" dzITC1= "<<dzITC1
                     <<" TILBdzend1= "<<dbManager->TILBdzend1()
	             <<" TILBdzend2= "<<dbManager->TILBdzend2()
                     <<endmsg; 

            if (NbPeriod > 6)
             { dzGlue = (Glue - 2*NbPeriod * (dbManager->TILBdzmast() + dbManager->TILBdzspac())) / (4.*NbPeriod);
             }  else {
               dzGlue = (Glue - (2*(NbPeriod-1) * (dbManager->TILBdzmast() + dbManager->TILBdzspac()) + dbManager->TILBdzspac())) / (4.*(NbPeriod-1));
             }

	    if(m_log->level()<=MSG::DEBUG)
	      (*m_log) << MSG::DEBUG <<"  Plug1Module : Glue= "<<Glue<<" dzGlue= "<<dzGlue<<" NbPeriod= "<<NbPeriod
		       <<" TILBdzmast= "<<dbManager->TILBdzmast()<<" TILBdzspac= "<<dbManager->TILBdzspac()
		       <<endmsg; 

            if (dzGlue <= 0.)
             { (*m_log) << MSG::WARNING <<"  Plug1Module warning: "<<" dzGlue= "<<dzGlue 
	                <<endmsg;
             }

            checking("Plug1Module plug1SubMotherNeg (-)", false, 2, 
                 thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

            GeoTrd* plug1SubMotherNeg = new GeoTrd(thicknessWedgeMother/2,
                                                   thicknessWedgeMother/2,
                                                   dy1WedgeMother,
                                                   dy2WedgeMother,
                                                   heightWedgeMother/2);
            //Second submother for frontplate
            double dzITC2Bis = (specialC10) ? 0.0 : dzITC2; // for special C10 D4 and C10 do not overlap
            thicknessWedgeMother = (dbManager->TILBdzmodul() - dzITC2Bis) * CLHEP::cm;
	    if(m_log->level()<=MSG::DEBUG)
              if (specialC10)
                (*m_log) << MSG::DEBUG <<" Separate C10 and D4 in module " << ModuleNcp << endmsg;
          
            GeoLogVol *lvPlug1ModuleMotherNeg=0;
            if (thicknessWedgeMother > rless)
             { 
               heightWedgeMother = (dbManager->TILBrmin() - dbManager->TILBrminimal()) * CLHEP::cm;
               dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
               dy2WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
    
               checking("Plug1Module plug2SubMotherNeg (-)", false, 2, 
                   thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

               GeoTrd* plug2SubMotherNeg = new GeoTrd(thicknessWedgeMother/2,
                                                      thicknessWedgeMother/2,
                                                      dy1WedgeMother,
                                                      dy2WedgeMother,
                                                      heightWedgeMother/2);
    
               HepGeom::Translate3D plug1SubOffsetNeg(-dzITC2Bis*CLHEP::cm/2, 0.,
                                               (dbManager->TILBrminimal()-dbManager->TILBrmaximal())*CLHEP::cm/2);
    
               const GeoShapeUnion& plug1ModuleMotherNeg = 
                                    plug1SubMotherNeg->add(*plug2SubMotherNeg<<plug1SubOffsetNeg);

               lvPlug1ModuleMotherNeg = new GeoLogVol("Plug1Module",&plug1ModuleMotherNeg,matAir);
	     } else
	     { lvPlug1ModuleMotherNeg = new GeoLogVol("Plug1Module",plug1SubMotherNeg,matAir);
             }

            GeoPhysVol* pvPlug1ModuleMotherNeg = new GeoPhysVol(lvPlug1ModuleMotherNeg);
        
            // Fill the section
            if (Filling)
             { sectionBuilder->fillSection(pvPlug1ModuleMotherNeg, 3,
                                           dbManager->TILBrmaximal(),
                                           dbManager->TILBrminimal(),
                                           dzGlue,
                                           deltaPhi,
                                           ModuleNcp,
                                           dzITC2Bis); 
             }
        
            Z = (dbManager->TILBrmin()-dbManager->TILBrminimal())*CLHEP::cm/2;
            GeoTransform* tfPlug1ModuleMotherNeg = new GeoTransform(HepGeom::Translate3D(0.,0.,Z));

            pvITCModuleMotherNeg->add(tfPlug1ModuleMotherNeg);
            pvITCModuleMotherNeg->add(pvPlug1ModuleMotherNeg);
      
            //Mother volume for ITC2
            if (Ifc10) {  

              // TILE_PLUG2
              dbManager->SetCurrentSectionByNumber(Ic10);

              thicknessWedgeMother = dbManager->TILBdzmodul() * CLHEP::cm;
              heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * CLHEP::cm;
              dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
              dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

              if (dbManager->TILBnperiod() > 1)
               { dzGlue = (dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2() 
                        - ((dbManager->TILBnperiod()-1)*2*(dbManager->TILBdzmast() + dbManager->TILBdzspac()) 
                        + dbManager->TILBdzspac()))/(4.*(dbManager->TILBnperiod() - 1));
               } else { dzGlue = 0; // add for special missing C10 modules
               }

              checking("Plug2Module (-)", false, 2, 
                   thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

              GeoTrd* plug2ModuleMotherNeg = new GeoTrd(thicknessWedgeMother/2,
                                                        thicknessWedgeMother/2,
                                                        dy1WedgeMother,
                                                        dy2WedgeMother,
                                                        heightWedgeMother/2);
    
              GeoLogVol* lvPlug2ModuleMotherNeg = new GeoLogVol("Plug2Module",plug2ModuleMotherNeg,matAir);
              GeoPhysVol* pvPlug2ModuleMotherNeg = new GeoPhysVol(lvPlug2ModuleMotherNeg);
    
              // Fill the section
              if (Filling)
	       { sectionBuilder->fillSection(pvPlug2ModuleMotherNeg, Ic10,
                                             dbManager->TILBrmaximal(),
                                             dbManager->TILBrminimal(),
                                             dzGlue,
                                             deltaPhi);
               }

              // TILE_PLUG1, D4        
              if (Ifd4) {
                dbManager->SetCurrentSectionByNumber(Id4);

                GeoTransform* tfPlug2ModuleMotherNeg = new GeoTransform(itcModuleSubShiftNeg);

                pvITCModuleMotherNeg->add(tfPlug2ModuleMotherNeg);
                pvITCModuleMotherNeg->add(pvPlug2ModuleMotherNeg); 
              }
            }//if C10

          double zShift = 0;
          NbPeriod = dbManager->TILBnperiod();
	  //          Z = (dbManager->TILBdzmodul()-dzITC2)/2*CLHEP::cm;
	  // ps Zshift calculated from length of volumes rather than modules (account for special modules) 
	  //
          Z = (dzITC1 - dzITC2)/2*CLHEP::cm;

          if (fabs(Z) <= fabs(zITCStandard) && NbPeriod == 6) zShift = zITCStandard*(1./CLHEP::cm);

	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG <<"  ITCModule Negative, position X= "<<X<<" Z= "<<Z
		     <<" zStandard= "<<zITCStandard<< " zShift= " <<zShift
		     <<endmsg;

          GeoTransform* xtraITCNeg = new GeoTransform(HepGeom::TranslateX3D(
                                         (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2*CLHEP::cm));
	  GeoTransform* ztraITCNeg = new GeoTransform(HepGeom::TranslateZ3D(zShift*CLHEP::cm));

          pvITCMotherNeg->add(zrotMod);
          pvITCMotherNeg->add(xtraITCNeg);
          pvITCMotherNeg->add(ztraITCNeg);
          pvITCMotherNeg->add(yrotMod);

          pvITCMotherNeg->add(new GeoIdentifierTag(ModuleNcp));
	  pvITCMotherNeg->add(pvITCModuleMotherNeg);

	}//if(Ifd4 || Ifc10)

        //-------------------------- G A P Negative ---------------------------------
        if (Ifgap) {

          // TILE_PLUG3
          dbManager->SetCurrentSectionByNumber(Igap); 

          // Trd - module mother
          thicknessWedgeMother = dbManager->TILBdzmodul() * CLHEP::cm;
          heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * CLHEP::cm;
          dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
          dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

          dzGlue = 0.;

          checking("GapModule (-)", false, 2, 
               thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

          GeoTrd* gapModuleMotherNeg = new GeoTrd(thicknessWedgeMother/2,
	                                          thicknessWedgeMother/2,
                                                  dy1WedgeMother,
					          dy2WedgeMother,
					          heightWedgeMother/2);

          GeoLogVol* lvGapModuleMotherNeg = new GeoLogVol("GapModule",gapModuleMotherNeg,matAir);
          GeoPhysVol* pvGapModuleMotherNeg = new GeoPhysVol(lvGapModuleMotherNeg);

          // Fill the section
          if (Filling)
            { sectionBuilder->fillSection(pvGapModuleMotherNeg, 5,  // probably better to have Igap instead of 5
					 dbManager->TILBrmaximal(),
					 dbManager->TILBrminimal(),
					 dzGlue,
					 deltaPhi);
           }

          // Module position inside mother
          GeoTransform* xtraGapNeg = new GeoTransform(HepGeom::TranslateX3D(
                                         (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2*CLHEP::cm));
          pvGapMotherNeg->add(zrotMod);
          pvGapMotherNeg->add(xtraGapNeg);
          pvGapMotherNeg->add(yrotMod);

          pvGapMotherNeg->add(new GeoIdentifierTag(ModuleNcp));
	  pvGapMotherNeg->add(pvGapModuleMotherNeg); 
       	}

        //-------------------------- Crack Negative ---------------------------------
        if (Ifcrack) {

          // TILE_PLUG4
          dbManager->SetCurrentSectionByNumber(Icrack);

          // mother
          thicknessWedgeMother = dbManager->TILBdzmodul() * CLHEP::cm;
          heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * CLHEP::cm;
          dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
          dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
    
          dzGlue = 0.;

          checking("CrackModule (-)", spE4, 2, 
               thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

          GeoTrd* crackModuleMotherNeg = new GeoTrd(thicknessWedgeMother/2,
					            thicknessWedgeMother/2,
						    dy1WedgeMother,
						    dy2WedgeMother,
						    heightWedgeMother/2);
    
          GeoLogVol* lvCrackModuleMotherNeg = new GeoLogVol("CrackModule",crackModuleMotherNeg,matAir);
          GeoPhysVol* pvCrackModuleMotherNeg = new GeoPhysVol(lvCrackModuleMotherNeg);
    
          // Fill the section
          if (Filling)
           { sectionBuilder->fillSection(pvCrackModuleMotherNeg, 6,
					 dbManager->TILBrmaximal(),
					 dbManager->TILBrminimal(),
					 dzGlue,
					 deltaPhi);
           }
          // Module position inside mother
          GeoTransform* xtraCrackNeg = new GeoTransform(HepGeom::TranslateX3D(
                                           (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2*CLHEP::cm));
          pvCrackMotherNeg->add(zrotMod);
          pvCrackMotherNeg->add(xtraCrackNeg);
          pvCrackMotherNeg->add(yrotMod);

          pvCrackMotherNeg->add(new GeoIdentifierTag(ModuleNcp));
	  pvCrackMotherNeg->add(pvCrackModuleMotherNeg); 

        }
      }

      //-------------------------------------- ITC BLOCKS Positive --------------------------------------    
      if(EnvType == 5) {

        // Common mother for ITC1/2 modules  
        if(Ifd4 || Ifc10) {

          // The first sub shape
          thicknessWedgeMother = dzITC1 * CLHEP::cm;
          heightWedgeMother = (rMaxITC1 - rMinITC1) * CLHEP::cm;
          dy1WedgeMother = rMinITC1 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
          dy2WedgeMother = rMaxITC1 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

          checking("ITCModule itcModuleSub2Pos (+)", false, 1,
	        thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

          GeoTrd* itcModuleSub1Pos = new GeoTrd(thicknessWedgeMother/2,
	                                        thicknessWedgeMother/2,
                                                dy1WedgeMother        ,
                                                dy2WedgeMother        ,
                                                heightWedgeMother/2  );
          // The second sub shape
          thicknessWedgeMother = dzITC2 * CLHEP::cm;
          heightWedgeMother    = (rMaxITC2 - rMinITC2) * CLHEP::cm;
          dy1WedgeMother = rMinITC2 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
          dy2WedgeMother = rMaxITC2 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

          checking("ITCModule itcModuleSub2Pos (+)", false, 1,
	        thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

          GeoTrd* itcModuleSub2Pos = new GeoTrd(thicknessWedgeMother/2,
	                                        thicknessWedgeMother/2,
                                                dy1WedgeMother        ,
                                                dy2WedgeMother        ,
                                                heightWedgeMother/2  );

          X = (dzITC1 - dzITC2)/2*CLHEP::cm; 
          Z = ((rMinITC2+rMaxITC2)-(rMaxITC1 + rMinITC1))/2*CLHEP::cm;
	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG <<"  ITCModule Positive, position X= "<<X<<" Z= "<<Z<< endmsg;

          HepGeom::Translate3D itcModule_SubShiftPos(X, 0., Z);
          const GeoShapeUnion& itcModuleMotherPos = itcModuleSub1Pos->add(*itcModuleSub2Pos<<itcModule_SubShiftPos);

          if (specialC10) Z -= (rMaxITC2-rMaxITC20)/2*CLHEP::cm;
          HepGeom::Translate3D itcModuleSubShiftPos(X, 0., Z);

          GeoLogVol* lvITCModuleMotherPos = new GeoLogVol("ITCModule",&itcModuleMotherPos,matAir);
          GeoPhysVol* pvITCModuleMotherPos = new GeoPhysVol(lvITCModuleMotherPos);

          // Mother volume for ITC1 
          // In plug1 it's necessary to produce GeoShapeUnion for mother volume that is composed by two parts:
          // 1. Mother for absorber and girder 
          // 2. Mother for frontplate (since it's short) 
        
          // The D4, PLUG1
          dbManager->SetCurrentSectionByNumber(Id4);

          thicknessWedgeMother = dzITC1 * CLHEP::cm;
          dy1WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
          dy2WedgeMother = rMaxITC1 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
          heightWedgeMother = (rMaxITC1 - dbManager->TILBrmin()) * CLHEP::cm;

	  // ps changes dzITC1 -> dzmodul
	  Glue =  dbManager->TILBdzmodul() - dbManager->TILBdzend1() - dbManager->TILBdzend2();
          NbPeriod = dbManager->TILBnperiod();

	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG <<"  Plug1Module : Glue= "<<Glue<<" dzITC1= "<<dzITC1
		     <<" TILBdzend1= "<<dbManager->TILBdzend1()
		     <<" TILBdzend2= "<<dbManager->TILBdzend2()
		     <<endmsg; 

          if (NbPeriod > 6)
           { dzGlue = (Glue - 2*NbPeriod * (dbManager->TILBdzmast() + dbManager->TILBdzspac())) / (4.*NbPeriod);
           } else {
             dzGlue = (Glue - (2*(NbPeriod-1) * (dbManager->TILBdzmast() + dbManager->TILBdzspac()) 
                      + dbManager->TILBdzspac())) / (4.*(NbPeriod-1));
           }

	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG <<"  Plug1Module : Glue= "<<Glue<<" dzGlue= "<<dzGlue<<" NbPeriod= "<<NbPeriod
		     <<" TILBdzmast= "<<dbManager->TILBdzmast()<<" TILBdzspac= "<<dbManager->TILBdzspac()
		     <<endmsg; 

          if (dzGlue <= 0.)
             {(*m_log) << MSG::WARNING <<"  Plug1Module warning: "<<" dzGlue= "<<dzGlue 
	               <<endmsg;
             }

          checking("Plug1Module plug1SubMotherPos (+)", false, 2, 
              thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);
        
          GeoTrd* plug1SubMotherPos = new GeoTrd(thicknessWedgeMother/2,
                                                 thicknessWedgeMother/2,
                                                 dy1WedgeMother        ,
                                                 dy2WedgeMother        , 
                                                 heightWedgeMother/2   );

          //Second submother C10, PLUG2
          double dzITC2Bis = (specialC10) ? 0.0 : dzITC2; // for special C10 D4 and C10 do not overlap
          thicknessWedgeMother = (dbManager->TILBdzmodul() - dzITC2Bis) * CLHEP::cm;
          if(m_log->level()<=MSG::DEBUG)
            if (specialC10)
              (*m_log) << MSG::DEBUG <<" Separate C10 and D4 in module " << ModuleNcp << endmsg;

          GeoLogVol *lvPlug1ModuleMotherPos=0;
          if (thicknessWedgeMother > rless)
           { 
             heightWedgeMother = (dbManager->TILBrmin() - dbManager->TILBrminimal()) * CLHEP::cm;
             dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
             dy2WedgeMother = dbManager->TILBrmin() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

             checking("Plug1Module plug2SubMotherPos (+)", false, 2, 
                 thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

             GeoTrd* plug2SubMotherPos = new GeoTrd(thicknessWedgeMother/2,
                                                    thicknessWedgeMother/2,
                                                    dy1WedgeMother        ,
                                                    dy2WedgeMother        ,
                                                    heightWedgeMother/2   );

             HepGeom::Translate3D plug1SubOffsetPos(-dzITC2Bis/2*CLHEP::cm, 0.,
                                        (dbManager->TILBrminimal()-dbManager->TILBrmaximal())/2*CLHEP::cm);
        
             const GeoShapeUnion& plug1ModuleMotherPos = plug1SubMotherPos->add(*plug2SubMotherPos<<plug1SubOffsetPos);

             lvPlug1ModuleMotherPos = new GeoLogVol("Plug1Module",&plug1ModuleMotherPos,matAir);
	   } else
	   { lvPlug1ModuleMotherPos = new GeoLogVol("Plug1Module",plug1SubMotherPos,matAir);
           }

          GeoPhysVol* pvPlug1ModuleMotherPos = new GeoPhysVol(lvPlug1ModuleMotherPos);
        
          // Fill the section
	  if (Filling)
	    { sectionBuilder->fillSection(pvPlug1ModuleMotherPos, 3,
		                         dbManager->TILBrmaximal(), 
                                         dbManager->TILBrminimal(),
                                         dzGlue,
                                         deltaPhi,
					 ModuleNcp,
                                         dzITC2Bis);
           }
        
          Z = (dbManager->TILBrmin()-dbManager->TILBrminimal())*CLHEP::cm/2;
          GeoTransform* tfPlug1ModuleMotherPos = new GeoTransform(HepGeom::Translate3D(0.,0.,Z));

          pvITCModuleMotherPos->add(tfPlug1ModuleMotherPos);
          pvITCModuleMotherPos->add(pvPlug1ModuleMotherPos);

          //Mother volume for ITC2 Positive
          if (Ifc10) { 

            // TILE_PLUG2, C10
            dbManager->SetCurrentSectionByNumber(Ic10); 

            thicknessWedgeMother = dzITC2 * CLHEP::cm;
            heightWedgeMother    = (rMaxITC2 - rMinITC2) * CLHEP::cm;
            dy1WedgeMother = rMinITC2 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
            dy2WedgeMother = rMaxITC2 * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

            if (dbManager->TILBnperiod() > 1)
             { dzGlue = (dzITC2 - dbManager->TILBdzend1() - dbManager->TILBdzend2() 
                      - ((dbManager->TILBnperiod()-1)*2*(dbManager->TILBdzmast() 
                      + dbManager->TILBdzspac()) + dbManager->TILBdzspac()))/(4*(dbManager->TILBnperiod()-1));
             } else {  dzGlue = 0; // add for special missing C10 modules
             }

            checking("Plug2Module (+)", false, 2,
	        thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

            GeoTrd* plug2ModuleMotherPos = new GeoTrd(thicknessWedgeMother/2,
                                                      thicknessWedgeMother/2,
                                                      dy1WedgeMother,
                                                      dy2WedgeMother,
                                                      heightWedgeMother/2);
        
            GeoLogVol* lvPlug2ModuleMotherPos = new GeoLogVol("Plug2Module",plug2ModuleMotherPos,matAir);
            GeoPhysVol* pvPlug2ModuleMotherPos = new GeoPhysVol(lvPlug2ModuleMotherPos);
        
            // Fill the section
            if (Filling)
	     { sectionBuilder->fillSection(pvPlug2ModuleMotherPos, Ic10,
                                           dbManager->TILBrmaximal(),
                                           dbManager->TILBrminimal(),
                                           dzGlue,
                                           deltaPhi);
             }
         
            // TILE_PLUG1, D4
            if ((Ifd4)) {
              dbManager->SetCurrentSectionByNumber(Id4);

              GeoTransform* tfPlug2ModuleMotherPos = new GeoTransform(itcModuleSubShiftPos);

              pvITCModuleMotherPos->add(tfPlug2ModuleMotherPos);
              pvITCModuleMotherPos->add(pvPlug2ModuleMotherPos);
            } 
          }//if C10

          double zShift = 0;
          NbPeriod = dbManager->TILBnperiod();
	  //ps          Z = (dbManager->TILBdzmodul()-dzITC2)/2*CLHEP::cm;
          Z = (dzITC1 - dzITC2)/2*CLHEP::cm;

          if (fabs(Z) <= fabs(zITCStandard) && NbPeriod == 6) zShift = zITCStandard*(1./CLHEP::cm);

	  if(m_log->level()<=MSG::DEBUG)
	    (*m_log) << MSG::DEBUG <<"  ITCModule Positive, position X= "<<X<<" Z= "<<Z
		     <<" zStandard= "<<zITCStandard<< " zShift= " <<zShift
		     <<endmsg;

	  GeoTransform* xtraITCPos = new GeoTransform(HepGeom::TranslateX3D(
                                        (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2*CLHEP::cm));
	  GeoTransform* ztraITCPos = new GeoTransform(HepGeom::TranslateZ3D(zShift*CLHEP::cm));

          pvITCMotherPos->add(zrotMod);
          pvITCMotherPos->add(xtraITCPos);
          pvITCMotherPos->add(ztraITCPos);
          pvITCMotherPos->add(XYrtMod);

	  pvITCMotherPos->add(new GeoIdentifierTag(ModuleNcp));
          pvITCMotherPos->add(pvITCModuleMotherPos);

        }//if(Ifd4 || Ifc10)


        //-------------------------- G A P Positive ---------------------------------
        if (Ifgap) {

          // TILE_PLUG3
          dbManager->SetCurrentSectionByNumber(Igap); 

          // Mother
          thicknessWedgeMother = dbManager->TILBdzmodul() * CLHEP::cm;
          heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * CLHEP::cm;
          dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
          dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;

          dzGlue = 0;

          checking("Plug2Module (+)", false, 2,
              thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

          GeoTrd* gapModuleMotherPos = new GeoTrd(thicknessWedgeMother/2,
	                                          thicknessWedgeMother/2,
                                                  dy1WedgeMother,
					          dy2WedgeMother,
					          heightWedgeMother/2);

          GeoLogVol* lvGapModuleMotherPos = new GeoLogVol("GapModule",gapModuleMotherPos,matAir);
          GeoPhysVol* pvGapModuleMotherPos = new GeoPhysVol(lvGapModuleMotherPos);

          // Fill the section
          if (Filling)
           { sectionBuilder->fillSection(pvGapModuleMotherPos, 5,
                                         dbManager->TILBrmaximal(),
                                         dbManager->TILBrminimal(),
                                         dzGlue,
                                         deltaPhi);
           }

          // Module position inside mother
          GeoTransform* xtraGapPos = new GeoTransform(HepGeom::TranslateX3D(
                                         (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2*CLHEP::cm));
          pvGapMotherPos->add(zrotMod);
          pvGapMotherPos->add(xtraGapPos);
          pvGapMotherPos->add(XYrtMod);

          pvGapMotherPos->add(new GeoIdentifierTag(ModuleNcp));
	  pvGapMotherPos->add(pvGapModuleMotherPos); 
       	} 

        //-------------------------- Crack Positive ---------------------------------
        if (Ifcrack) {

          // TILE_PLUG4
          dbManager->SetCurrentSectionByNumber(Icrack); 

          // Trd - module mother
          thicknessWedgeMother = dbManager->TILBdzmodul() * CLHEP::cm;
          heightWedgeMother = (dbManager->TILBrmaximal() - dbManager->TILBrminimal()) * CLHEP::cm;
          dy1WedgeMother = dbManager->TILBrminimal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
          dy2WedgeMother = dbManager->TILBrmaximal() * tan(deltaPhi/2*CLHEP::deg) * CLHEP::cm;
    
          dzGlue = 0.;
    
          checking("CrackModule (+)", spE4, 2, 
               thicknessWedgeMother/2,thicknessWedgeMother/2,dy1WedgeMother,dy2WedgeMother,heightWedgeMother/2);

          GeoTrd* crackModuleMotherPos = new GeoTrd(thicknessWedgeMother/2,
					            thicknessWedgeMother/2,
						    dy1WedgeMother,
						    dy2WedgeMother,
						    heightWedgeMother/2);
    
          GeoLogVol* lvCrackModuleMotherPos = new GeoLogVol("CrackModule",crackModuleMotherPos,matAir);
          GeoPhysVol* pvCrackModuleMotherPos = new GeoPhysVol(lvCrackModuleMotherPos);
    
          // Fill the section
          if (Filling)
           { sectionBuilder->fillSection(pvCrackModuleMotherPos, 6,
                                         dbManager->TILBrmaximal(),
                                         dbManager->TILBrminimal(),
                                         dzGlue,
                                         deltaPhi);
           }
    
          // Module position inside mother
          GeoTransform* xtraCrackPos = new GeoTransform(HepGeom::TranslateX3D(
                                          (dbManager->TILBrmaximal() + dbManager->TILBrminimal())/2*CLHEP::cm));
          pvCrackMotherPos->add(zrotMod);
          pvCrackMotherPos->add(xtraCrackPos);
          pvCrackMotherPos->add(XYrtMod);

          pvCrackMotherPos->add(new GeoIdentifierTag(ModuleNcp));
	  pvCrackMotherPos->add(pvCrackModuleMotherPos); 

        }
      } 

      } // if(EnvType == 4 || EnvType == 5)

      zrotMod->unref();
      zrotSaddle->unref();
    }// ModCounter, end
    
    

    yrotMod->unref();
    XYrtMod->unref();

    }

    //
    // Global Envelop positioning, BARREL Positive
    //

    //
    // ---------------------------------------- BARREL Positive -----------------------------------------------------
    //

    double ztrans =0;
    dbManager->SetCurrentEnvByType(EnvType);

    if(EnvType == 1) { 

      GeoTransform* tfBarrelMother;

      // consider 3 options - with/without ext.barrels and take into account DZ correction
      ztrans = 0;

      tfBarrelMother = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::RotateZ3D((dbManager->GetEnvDPhi())*CLHEP::deg));

      (*m_log) << MSG::INFO <<" Positioning barrel with translation "<<ztrans*CLHEP::cm<< endmsg;

      GeoNameTag* ntBarrelModuleMother = new GeoNameTag("Barrel"); 

      pvTileEnvelopeBarrel->add(tfBarrelMother);
      pvTileEnvelopeBarrel->add(ntBarrelModuleMother);
      pvTileEnvelopeBarrel->add(pvBarrelMother);
      
      GeoTransform* tfFingerMotherPos;

      ztrans = (dbManager->GetEnvZLength()*CLHEP::cm/2 - BFingerLengthPos/2 + PosDelta)*(1./CLHEP::cm);

      tfFingerMotherPos = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)*HepGeom::RotateZ3D((dbManager->GetEnvDPhi())*CLHEP::deg));

      (*m_log) << MSG::INFO <<" Positioning positive barrel finger with translation "<<ztrans*CLHEP::cm<< endmsg;

      GeoNameTag* ntFingerMotherPos = new GeoNameTag("TileFingerPos");

      pvTileEnvelopeBarrel->add(tfFingerMotherPos);
      pvTileEnvelopeBarrel->add(ntFingerMotherPos);
      pvTileEnvelopeBarrel->add(pvFingerMotherPos);

      if (dbManager->BoolSaddle())
      { GeoTransform* tfSaddleMotherPos;

        ztrans = (dbManager->GetEnvZLength()*CLHEP::cm/2 - BFingerLengthPos + DzSaddleSupport/2 + PosDelta)*(1./CLHEP::cm);

        tfSaddleMotherPos = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)*HepGeom::RotateZ3D((dbManager->GetEnvDPhi())*CLHEP::deg));

        GeoNameTag* ntSaddleMotherPos = new GeoNameTag("TileSaddlePos");

        pvTileEnvelopeBarrel->add(tfSaddleMotherPos);
        pvTileEnvelopeBarrel->add(ntSaddleMotherPos);
        pvTileEnvelopeBarrel->add(pvSaddleMotherPos);
      }

      //*> negative barrel

      GeoTransform* tfFingerMotherNeg;

      ztrans = (-dbManager->GetEnvZLength()*CLHEP::cm/2 + BFingerLengthNeg/2 - NegDelta)*(1./CLHEP::cm);

      tfFingerMotherNeg = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)*HepGeom::RotateZ3D((dbManager->GetEnvDPhi())*CLHEP::deg));

      (*m_log) << MSG::INFO <<" Positioning negative barrel finger with translation "<<ztrans*CLHEP::cm<< endmsg;

      GeoNameTag* ntFingerMotherNeg = new GeoNameTag("TileFingerNeg");
      pvTileEnvelopeBarrel->add(tfFingerMotherNeg);
      pvTileEnvelopeBarrel->add(ntFingerMotherNeg);
      pvTileEnvelopeBarrel->add(pvFingerMotherNeg); 

      if (dbManager->BoolSaddle())
      { GeoTransform* tfSaddleMotherNeg;

        ztrans = (-dbManager->GetEnvZLength()*CLHEP::cm/2 + BFingerLengthNeg - DzSaddleSupport/2 - NegDelta)*(1./CLHEP::cm);

        tfSaddleMotherNeg = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)*HepGeom::RotateZ3D((dbManager->GetEnvDPhi())*CLHEP::deg));

        GeoNameTag* ntSaddleMotherNeg = new GeoNameTag("TileSaddleNeg");

        pvTileEnvelopeBarrel->add(tfSaddleMotherNeg);
        pvTileEnvelopeBarrel->add(ntSaddleMotherNeg);
        pvTileEnvelopeBarrel->add(pvSaddleMotherNeg);
      }
    } 

    //  
    //----------------------------------------- EBarrel Positive -----------------------------------------------------

    if(EnvType == 3) { // positive ext.barrel is always at positive boundary, after finger

      dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
      double thicknessEndPlate = dbManager->TILBdzend1()*CLHEP::cm;

      double PoZ1 = thicknessEndPlate + modl_length/4 - (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthPos)/2;
      double PoZ2 = modl_length/4 + PoZ1;

      //--------------------------------------------------------------------------------------------------------------
      // CutA in Tile (Positive) (close center)
      if(m_fullGeo) {
      if (dbManager->BoolCuts()){

        // Iron1, down
        volname = "Iron1"; dbManager->SetCurrentCuts(volname); //>>
        dxIron = dbManager->CutsXpos();
	dyIron = dbManager->CutsYpos();
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG << " Iron1: " << dxIron << " " << dyIron << endmsg;

        GeoTransform* tfIron1 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(dxIron,PosY-dyIron,PoZ2) 
	                                       * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherPos->add(tfIron1);
        pvEBarrelMotherPos->add(new GeoIdentifierTag(1));
        pvEBarrelMotherPos->add(pvIron1);

                      tfIron1 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(-dxIron,PosY-dyIron,PoZ2) 
					       * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Right
        pvEBarrelMotherPos->add(tfIron1);
        pvEBarrelMotherPos->add(pvIron1);

        // Iron2, middle
        volname = "Iron2"; dbManager->SetCurrentCuts(volname); //>>
        dxIron = dbManager->CutsXpos();
	dyIron = dbManager->CutsYpos();
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG << " Iron2: " << dxIron << " " << dyIron << endmsg;

        GeoTransform* tfIron2 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(dxIron,PosY+dyIron,PoZ2) 
	                      * HepGeom::RotateZ3D(-84.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherPos->add(tfIron2);
        pvEBarrelMotherPos->add(new GeoIdentifierTag(2));
        pvEBarrelMotherPos->add(pvIron2);

                      tfIron2 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(-dxIron,PosY+dyIron,PoZ2) 
	                      * HepGeom::RotateZ3D(84.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherPos->add(tfIron2);
        pvEBarrelMotherPos->add(pvIron2);

        // Iron3, middle 
        volname = "Iron3"; dbManager->SetCurrentCuts(volname); //>>
        dxIron = dbManager->CutsXpos();
	dyIron = dbManager->CutsYpos();
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG << " Iron3: " << dxIron << " " << dyIron << endmsg;

        GeoTransform* tfIron3 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(dxIron,PosY+dyIron,PoZ2) 
	                      * HepGeom::RotateZ3D(90.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherPos->add(tfIron3);
        pvEBarrelMotherPos->add(new GeoIdentifierTag(3));
        pvEBarrelMotherPos->add(pvIron3);

                      tfIron3 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(-dxIron,PosY+dyIron,PoZ2) 
	                      * HepGeom::RotateZ3D(90.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Right
        pvEBarrelMotherPos->add(tfIron3);
        pvEBarrelMotherPos->add(pvIron3);

        // Heavy Iron Box 
        volname = "IrBox"; dbManager->SetCurrentCuts(volname); //>>
        dxIron = dbManager->CutsXpos();
	dyIron = dbManager->CutsYpos();

        GeoTransform* tfIrBoxL = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(dxIron,PosY-dyIron,PoZ2) 
					       * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherPos->add(tfIrBoxL);
        pvEBarrelMotherPos->add(new GeoIdentifierTag(4));
        pvEBarrelMotherPos->add(pvIrBox);

        GeoTransform* tfIrBoxR = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(-dxIron,PosY-dyIron,PoZ2)
                                                * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Right
        pvEBarrelMotherPos->add(tfIrBoxR);
        pvEBarrelMotherPos->add(pvIrBox);

        // CutB in Tile (Positive) (outer center) 
        volname = "IrUp"; dbManager->SetCurrentCuts(volname); //>>
        dxIr = dbManager->CutsXpos();
	dyIr = dbManager->CutsYpos();
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG << " IrUp: " <<dxIr<< " " <<dyIr<< endmsg;

        GeoTransform* tfIrUp = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) 
                             * HepGeom::Translate3D(PosXcut+dxIr,-PosYcut+dyIr,-PoZ1) 
		             * HepGeom::RotateZ3D(90.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherPos->add(tfIrUp);
        pvEBarrelMotherPos->add(new GeoIdentifierTag(5));
        pvEBarrelMotherPos->add(pvIrUp);

                      tfIrUp = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) 
                             * HepGeom::Translate3D(-PosXcut-dxIr,-PosYcut+dyIr,-PoZ1) 
		             * HepGeom::RotateZ3D(90.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Right
        pvEBarrelMotherPos->add(tfIrUp);
        pvEBarrelMotherPos->add(pvIrUp);

        // CutB in Tile (Positive) (outer center) 
        volname = "IrDw"; dbManager->SetCurrentCuts(volname); //>>
        dxIr = dbManager->CutsXpos();
	dyIr = dbManager->CutsYpos();
	if(m_log->level()<=MSG::DEBUG)
	  (*m_log) << MSG::DEBUG << " IrDw: " <<dxIr<< " " <<dyIr<< endmsg;

        GeoTransform* tfIrDw = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) 
                             * HepGeom::Translate3D(PosXcut+dxIr,-PosYcut+dyIr,-PoZ1) 
		             * HepGeom::RotateZ3D(70.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherPos->add(tfIrDw);
        pvEBarrelMotherPos->add(new GeoIdentifierTag(6));
        pvEBarrelMotherPos->add(pvIrDw);

                      tfIrDw = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) 
                             * HepGeom::Translate3D(-PosXcut+dxIr,-PosYcut+dyIr,-PoZ1) 
		             * HepGeom::RotateZ3D(-70.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left

        pvEBarrelMotherPos->add(tfIrDw);
        pvEBarrelMotherPos->add(pvIrDw);
      } // end if BoolCuts
      }
      //--------------------------------------------------------------------------------------------------------------
      // Ext.Barrel
      ztrans = (PosEndCrack + (dbManager->GetEnvZLength()*CLHEP::cm - EBFingerLengthPos)/2 + 19.5)*(1./CLHEP::cm); 

      GeoTransform* tfEBarrelMotherPos = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * 
                                             HepGeom::RotateZ3D(dbManager->GetEnvDPhi()*CLHEP::deg));

      (*m_log) << MSG::INFO <<" Positioning positive ext.barrel with translation "<< ztrans*CLHEP::cm << endmsg; 

      //
      GeoNameTag* ntEBarrelMotherPos = new GeoNameTag("EBarrelPos");
      pvTileEnvelopePosEndcap->add(tfEBarrelMotherPos);
      pvTileEnvelopePosEndcap->add(ntEBarrelMotherPos); 
      pvTileEnvelopePosEndcap->add(new GeoIdentifierTag(3));
      pvTileEnvelopePosEndcap->add(pvEBarrelMotherPos);
      //

      //--------------------------------------------------------------------------------------------------------------
      // Finger 
      ztrans = (PosEndExBarrel + EBFingerLengthPos/2)*(1./CLHEP::cm); 

      GeoTransform* tfEFingerMotherPos = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * 
                                                          HepGeom::RotateZ3D(dbManager->GetEnvDPhi() * CLHEP::deg));

      (*m_log) << MSG::INFO <<" Positioning positive ext.barrel finger with translation ztrans= "<<ztrans*CLHEP::cm<<endmsg;

      GeoNameTag* ntEFingerMotherPos = new GeoNameTag("TileEFingerPos");

      pvTileEnvelopePosEndcap->add(tfEFingerMotherPos); 
      pvTileEnvelopePosEndcap->add(ntEFingerMotherPos);
      pvTileEnvelopePosEndcap->add(pvEFingerMotherPos); 

      //--------------------------------------------------------------------------------------------------------------
      // Ext. Saddle Support
      if (dbManager->BoolSaddle())
      { ztrans = (PosEndExBarrel + DzSaddleSupport/2)*(1./CLHEP::cm); 

        GeoTransform* tfESaddleMotherPos = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * 
                                                            HepGeom::RotateZ3D(dbManager->GetEnvDPhi() * CLHEP::deg));

        (*m_log) << MSG::INFO <<" Positioning positive ext.barrel saddle with translation ztrans= "<<ztrans*CLHEP::cm
                 << endmsg;

        GeoNameTag* ntESaddleMotherPos = new GeoNameTag("TileESaddlePos");

        pvTileEnvelopePosEndcap->add(tfESaddleMotherPos); 
        pvTileEnvelopePosEndcap->add(ntESaddleMotherPos); 
        pvTileEnvelopePosEndcap->add(pvESaddleMotherPos); 
      }

    }


    //----------------------------------------- EBarrel Negative -----------------------------------------------------
    //
    if(EnvType == 2) { // negative ext.barrel is always at negative boundary, after finger

      dbManager->SetCurrentSection(TileDddbManager::TILE_EBARREL);
      double thicknessEndPlate = dbManager->TILBdzend1()*CLHEP::cm;

      double PoZ1 = thicknessEndPlate + modl_length/4 - (dbManager->GetEnvZLength()*CLHEP::cm- EBFingerLengthNeg)/2;
      double PoZ2 = modl_length/4 + PoZ1;

      //*>------------------------------------------------------------------------------------------------------
      // LArPart in Tile (Negative) (close center)
      if(m_fullGeo) {
      if (dbManager->BoolCuts()){
        // Iron1, down
        volname = "Iron1"; dbManager->SetCurrentCuts(volname); //>>
        dxIron = dbManager->CutsXpos();
	dyIron = dbManager->CutsYpos();

        GeoTransform* tfIron1 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(dxIron,PosY-dyIron,-PoZ2) 
					       * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherNeg->add(tfIron1);
        pvEBarrelMotherNeg->add(new GeoIdentifierTag(1));
        pvEBarrelMotherNeg->add(pvIron1);

                      tfIron1 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(-dxIron,PosY-dyIron,-PoZ2)
					       * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Right
        pvEBarrelMotherNeg->add(tfIron1);
        pvEBarrelMotherNeg->add(new GeoIdentifierTag(2));
        pvEBarrelMotherNeg->add(pvIron1);

        // Iron2, middle
        volname = "Iron2"; dbManager->SetCurrentCuts(volname); //>>
        dxIron = dbManager->CutsXpos();
	dyIron = dbManager->CutsYpos();

        GeoTransform* tfIron2 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(dxIron,PosY+dyIron,-PoZ2) 
	                      * HepGeom::RotateZ3D(-84.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherNeg->add(tfIron2);
        pvEBarrelMotherNeg->add(new GeoIdentifierTag(2));
        pvEBarrelMotherNeg->add(pvIron2);

                      tfIron2 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(-dxIron,PosY+dyIron,-PoZ2)
                              * HepGeom::RotateZ3D(84.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherNeg->add(tfIron2);
        pvEBarrelMotherNeg->add(pvIron2);

        // Iron3, middle 
        volname = "Iron3"; dbManager->SetCurrentCuts(volname); //>>
        dxIron = dbManager->CutsXpos();
	dyIron = dbManager->CutsYpos();

        GeoTransform* tfIron3 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(dxIron,PosY+dyIron,-PoZ2) 
	                      * HepGeom::RotateZ3D(90.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherNeg->add(tfIron3);
        pvEBarrelMotherNeg->add(new GeoIdentifierTag(3));
        pvEBarrelMotherNeg->add(pvIron3); 

                      tfIron3 = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(-dxIron,PosY+dyIron,-PoZ2)
                              * HepGeom::RotateZ3D(90.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Right
        pvEBarrelMotherNeg->add(tfIron3);
        pvEBarrelMotherNeg->add(pvIron3);

        // Heavy Iron Box
        volname = "IrBox"; dbManager->SetCurrentCuts(volname); //>>
        dxIron = dbManager->CutsXpos();
	dyIron = dbManager->CutsYpos();
        GeoTransform* tfIrBoxL = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * HepGeom::Translate3D(dxIron,PosY-dyIron,-PoZ2)
                               * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherNeg->add(tfIrBoxL); 
        pvEBarrelMotherNeg->add(new GeoIdentifierTag(4));
        pvEBarrelMotherNeg->add(pvIrBox); 

        GeoTransform* tfIrBoxR = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) *HepGeom::Translate3D(-dxIron,PosY-dyIron,-PoZ2)
		               * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Right
        pvEBarrelMotherNeg->add(tfIrBoxR);
        pvEBarrelMotherNeg->add(pvIrBox);

        // LArPart in Tile (Positive) (outer center) 
        volname = "IrUp"; dbManager->SetCurrentCuts(volname); //>>
        dxIr = dbManager->CutsXpos();
	dyIr = dbManager->CutsYpos();

        GeoTransform* tfIrUp = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) 
                             * HepGeom::Translate3D(PosXcut+dxIr,-PosYcut+dyIr,PoZ1) 
		             * HepGeom::RotateZ3D(90.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherNeg->add(tfIrUp);
        pvEBarrelMotherNeg->add(new GeoIdentifierTag(5));
        pvEBarrelMotherNeg->add(pvIrUp);

                      tfIrUp = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) 
                             * HepGeom::Translate3D(-PosXcut-dxIr,-PosYcut+dyIr,PoZ1) 
		             * HepGeom::RotateZ3D(90.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Right
        pvEBarrelMotherNeg->add(tfIrUp);
        pvEBarrelMotherNeg->add(pvIrUp);

        // CutB in Tile (Positive) (outer center) 
        volname = "IrDw"; dbManager->SetCurrentCuts(volname); //>>
        dxIr = dbManager->CutsXpos();
	dyIr = dbManager->CutsYpos();

        GeoTransform* tfIrDw = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) 
                             * HepGeom::Translate3D(PosXcut+dxIr,-PosYcut+dyIr,PoZ1) 
		             * HepGeom::RotateZ3D(70.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left
        pvEBarrelMotherNeg->add(tfIrDw);
        pvEBarrelMotherNeg->add(new GeoIdentifierTag(6));
        pvEBarrelMotherNeg->add(pvIrDw);

                      tfIrDw = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) 
                             * HepGeom::Translate3D(-PosXcut+dxIr,-PosYcut+dyIr,PoZ1) 
		             * HepGeom::RotateZ3D(-70.*CLHEP::deg) * HepGeom::RotateX3D(90.*CLHEP::deg) * HepGeom::RotateZ3D(90.*CLHEP::deg)); // Left

        pvEBarrelMotherNeg->add(tfIrDw);
        pvEBarrelMotherNeg->add(pvIrDw);
      } // end if BoolCuts
      }
      //
      //*>------------------------------------------------------------------------------------------------------
      // Ext.Barrel
      ztrans = (-NegEndCrack - (dbManager->GetEnvZLength()*CLHEP::cm - EBFingerLengthNeg)/2 - 19.5)*(1./CLHEP::cm);

      GeoTransform* tfEBarrelMotherNeg = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * 
                                             HepGeom::RotateZ3D(dbManager->GetEnvDPhi()*CLHEP::deg));

      (*m_log) << MSG::INFO <<" Positioning negative ext.barrel with translation ztrans "<<ztrans*CLHEP::cm<<endmsg;
      
      GeoNameTag* ntEBarrelMotherNeg = new GeoNameTag("EBarrelNeg");

      pvTileEnvelopeNegEndcap->add(tfEBarrelMotherNeg);
      pvTileEnvelopeNegEndcap->add(ntEBarrelMotherNeg);
      pvTileEnvelopeNegEndcap->add(new GeoIdentifierTag(2));
      pvTileEnvelopeNegEndcap->add(pvEBarrelMotherNeg);

      //*>------------------------------------------------------------------------------------------------------
      // Finger
      ztrans = (-NegEndExBarrel - EBFingerLengthPos/2)*(1./CLHEP::cm); 

      GeoTransform* tfEFingerMotherNeg = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * 
                                             HepGeom::RotateZ3D(dbManager->GetEnvDPhi() * CLHEP::deg));

      (*m_log) << MSG::INFO <<" Positioning negative ext.barrel finger with translation ztrans= "<<ztrans*CLHEP::cm<< endmsg;

      GeoNameTag* ntEFingerMotherNeg = new GeoNameTag("TileEFingerNeg");

      pvTileEnvelopeNegEndcap->add(tfEFingerMotherNeg);
      pvTileEnvelopeNegEndcap->add(ntEFingerMotherNeg);
      pvTileEnvelopeNegEndcap->add(pvEFingerMotherNeg);

      //*>------------------------------------------------------------------------------------------------------
      // Ext. Saddle Support
      if (dbManager->BoolSaddle())
      { ztrans = (-NegEndExBarrel - DzSaddleSupport/2)*(1./CLHEP::cm); 

        GeoTransform* tfESaddleMotherNeg = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * 
                                               HepGeom::RotateZ3D(dbManager->GetEnvDPhi() * CLHEP::deg));

        (*m_log) << MSG::INFO <<" Positioning negative ext.barrel saddle with translation ztrans= "<<ztrans*CLHEP::cm
                 << endmsg;

        GeoNameTag* ntESaddleMotherNeg = new GeoNameTag("TileESaddleNeg");

        pvTileEnvelopeNegEndcap->add(tfESaddleMotherNeg); 
        pvTileEnvelopeNegEndcap->add(ntESaddleMotherNeg);
        pvTileEnvelopeNegEndcap->add(pvESaddleMotherNeg);
      }
    }

    //
    // ---------------------------------------- ITC Positive -----------------------------------------------------
    //

    if(EnvType == 5) { // positive ITC attached to positive ext.barrel

      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);

      ztrans = PosEndBarrelFinger*(1./CLHEP::cm) + dbManager->TILBdzmodul()/2;

      //std::cout <<" ztrans "<<ztrans<<" PosEndBarrelFinger/CLHEP::cm "<<PosEndBarrelFinger/CLHEP::cm
      //          <<" dbManager->TILBdzmodul()/2*CLHEP::cm"<<dbManager->TILBdzmodul()/2<<"\n";
      
      (*m_log) << MSG::INFO <<" Positioning positive ITC with translation "<<ztrans*CLHEP::cm<< endmsg;

      GeoTransform* tfITCMotherPos = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm) * 
                                         HepGeom::RotateZ3D(dbManager->GetEnvDPhi() * CLHEP::deg));

      GeoNameTag* ntITCMotherPos = new GeoNameTag("ITCPos");

      pvTileEnvelopePosEndcap->add(tfITCMotherPos);
      pvTileEnvelopePosEndcap->add(ntITCMotherPos);
      pvTileEnvelopePosEndcap->add(pvITCMotherPos);

      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);
      ztrans = PosBeginGap*(1./CLHEP::cm) + dbManager->TILBdzmodul()/2;

      (*m_log) << MSG::INFO <<" Positioning positive Gap with translation "<<ztrans*CLHEP::cm<<endmsg;

      GeoTransform* tfGapMotherPos = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)* 
                                         HepGeom::RotateZ3D(dbManager->GetEnvDPhi()*CLHEP::deg));

      GeoNameTag* ntGapMotherPos = new GeoNameTag("GapPos");

      pvTileEnvelopePosEndcap->add(tfGapMotherPos);
      pvTileEnvelopePosEndcap->add(ntGapMotherPos);
      pvTileEnvelopePosEndcap->add(pvGapMotherPos);

      // Crack
      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
      ztrans = PosBeginCrack*(1./CLHEP::cm) + dbManager->TILBdzmodul()/2;

      (*m_log) << MSG::INFO <<" Positioning positive Crack with translation "<<ztrans*CLHEP::cm<<endmsg;

      GeoTransform* tfCrackMotherPos = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)* 
                                         HepGeom::RotateZ3D(dbManager->GetEnvDPhi()*CLHEP::deg));
 
      GeoNameTag* ntCrackMotherPos = new GeoNameTag("CrackPos");

      pvTileEnvelopePosEndcap->add(tfCrackMotherPos);
      pvTileEnvelopePosEndcap->add(ntCrackMotherPos);
      pvTileEnvelopePosEndcap->add(pvCrackMotherPos);
      }
    
    //
    // ---------------------------------------- ITC Negative -----------------------------------------------------
    //

    // negative ITC attached to negative ext.barrel
    if(EnvType == 4) { 

      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG1);
      ztrans = -NegEndBarrelFinger*(1./CLHEP::cm) - dbManager->TILBdzmodul()/2;

      (*m_log) << MSG::INFO <<" Positioning negative ITC with translation "<<ztrans*CLHEP::cm<<endmsg;

      GeoTransform* tfITCMotherNeg = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)* 
                                         HepGeom::RotateZ3D(dbManager->GetEnvDPhi()*CLHEP::deg));

      GeoNameTag* ntITCMotherNeg = new GeoNameTag("ITCNeg");

      pvTileEnvelopeNegEndcap->add(tfITCMotherNeg); 
      pvTileEnvelopeNegEndcap->add(ntITCMotherNeg);
      pvTileEnvelopeNegEndcap->add(pvITCMotherNeg);

      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG3);
      ztrans = -NegBeginGap*(1./CLHEP::cm) - dbManager->TILBdzmodul()/2;

      (*m_log) << MSG::INFO <<" Positioning negative Gap with translation "<<ztrans*CLHEP::cm<<endmsg;

      GeoTransform* tfGapMotherNeg = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)* 
                                         HepGeom::RotateZ3D(dbManager->GetEnvDPhi()*CLHEP::deg));

      GeoNameTag* ntGapMotherNeg = new GeoNameTag("GapNeg");

      pvTileEnvelopeNegEndcap->add(tfGapMotherNeg);
      pvTileEnvelopeNegEndcap->add(ntGapMotherNeg);
      pvTileEnvelopeNegEndcap->add(pvGapMotherNeg); 

      // Crack
      dbManager->SetCurrentSection(TileDddbManager::TILE_PLUG4);
      ztrans = -NegBeginCrack*(1./CLHEP::cm) - dbManager->TILBdzmodul()/2;

      (*m_log) << MSG::INFO <<" Positioning negative Crack with translation "<<ztrans*CLHEP::cm<<endmsg;

      GeoTransform* tfCrackMotherNeg = new GeoTransform(HepGeom::TranslateZ3D(ztrans*CLHEP::cm)* 
                                           HepGeom::RotateZ3D(dbManager->GetEnvDPhi()*CLHEP::deg));

      GeoNameTag* ntCrackMotherNeg = new GeoNameTag("CrackNeg");

      pvTileEnvelopeNegEndcap->add(tfCrackMotherNeg);
      pvTileEnvelopeNegEndcap->add(ntCrackMotherNeg);
      pvTileEnvelopeNegEndcap->add(pvCrackMotherNeg);

    }
  }  // EnvCounter, end

     //
     // creating Descriptiors and CaloDetDescrElements 
     // 

     int nModulesInSection[6] = {0,0,0,0,0,0};
     double zShiftInSection[6] = {0.0,0.0,0.0,0.0,0.0,0.0,};

     // int NumberOfEnv = dbManager->GetNumberOfEnv();


     for(int EnvCounter = 0; EnvCounter < dbManager->GetNumberOfEnv(); ++EnvCounter){ //Loop over Envelopes

       dbManager->SetCurrentEnvByIndex(EnvCounter);
       int EnvType = dbManager->GetEnvType();
       int NumberOfMod = dbManager->GetEnvNModules();
       double Zshift = dbManager->GetEnvZShift()*CLHEP::cm;

       if(m_log->level()<=MSG::DEBUG)
	 (*m_log) << MSG::DEBUG 
		  << " EnvCounter is " << EnvCounter
		  << " EnvType is " << EnvType
		  << " Zshift is " << Zshift*(1./CLHEP::cm) << " cm"
		  << endmsg;

       // Central barrel 
       if(EnvType == 1 || EnvType == 0) { 
         nModulesInSection[0] = nModulesInSection[1] = NumberOfMod;
         zShiftInSection[0] = zShiftInSection[1] = Zshift;
       } else if(EnvType < 6) {
         nModulesInSection[EnvType] = NumberOfMod;
         zShiftInSection[EnvType] = Zshift;
       }
     }

   const TileID* tileID = m_detectorManager->get_id();
 
   unsigned int dete[6] = {TILE_REGION_CENTRAL,TILE_REGION_CENTRAL,TILE_REGION_EXTENDED,TILE_REGION_EXTENDED,
                           TILE_REGION_GAP,TILE_REGION_GAP};
   int side[6] = {0,1,0,1,0,1};
   
   for (int ii=0; ii<6; ++ii) {
     if (ii%2 == 0) {
        sectionBuilder->computeCellDim(m_detectorManager, dete[ii],
                                       m_addPlatesToCellVolume,
                                       zShiftInSection[ii+1], // zShiftPos
                                       zShiftInSection[ii]);  // zShiftNeg
     }
     
     TileDetDescriptor* descriptor = new TileDetDescriptor();
     sectionBuilder->fillDescriptor(descriptor, dete[ii], side[ii],
                                    m_testbeamGeometry,      // set to false - ATLAS geometry
                                    m_addPlatesToCellVolume, // add front/end plates to cell volume
                                    nModulesInSection[ii],   // 0-64 modules
                                    zShiftInSection[ii]);    // Z-shift
     
     Identifier idRegion = tileID->region_id(ii);
     descriptor->set(idRegion);
     m_detectorManager->add(descriptor); 
     m_detectorManager->add(new TileDetDescrRegion(idRegion, descriptor));
   }

  // --------- ----------- --------- -------- ------ --------- ------- ---------- 
  GeoNameTag *nTag = new GeoNameTag("Tile");

  if(BAR)
  {
    world->add(nTag);

    // Top transform for the Central barrel
    dbManager->SetCurrentEnvByType(1);
    HepGeom::Transform3D mz = HepGeom::RotateZ3D(dbManager->GetEnvDPhi());
    HepGeom::Transform3D my = HepGeom::RotateY3D(dbManager->GetEnvDTheta());
    HepGeom::Transform3D mx = HepGeom::RotateZ3D(dbManager->GetEnvDPsi());
    HepGeom::Transform3D vpos = HepGeom::Translate3D(dbManager->GetEnvDX()*CLHEP::cm,dbManager->GetEnvDY()*CLHEP::cm,dbManager->GetEnvDZ()*CLHEP::cm);
    GeoTransform* barrelTT = new GeoTransform(HepGeom::Transform3D(vpos*(mx*(my*(mz)))));

    (*m_log) << MSG::INFO << " Global positioning of barrel with rotation ("
             << dbManager->GetEnvDPhi() << "," << dbManager->GetEnvDTheta() << "," << dbManager->GetEnvDPsi() << ")"
             << ") and translation (" << dbManager->GetEnvDX() << "," << dbManager->GetEnvDY() << "," << dbManager->GetEnvDZ()
             << ") CLHEP::cm" << endmsg;

    world->add(barrelTT);
    world->add(pvTileEnvelopeBarrel);
    m_detectorManager->addTreeTop(pvTileEnvelopeBarrel);
  }
 
  if(EBA)
  {
    world->add(nTag);
 
    // Top transform for the Positive endcap
    dbManager->SetCurrentEnvByType(3);
    HepGeom::Transform3D mz = HepGeom::RotateZ3D(dbManager->GetEnvDPhi());
    HepGeom::Transform3D my = HepGeom::RotateY3D(dbManager->GetEnvDTheta());
    HepGeom::Transform3D mx = HepGeom::RotateZ3D(dbManager->GetEnvDPsi());
    HepGeom::Transform3D vpos = HepGeom::Translate3D(dbManager->GetEnvDX()*CLHEP::cm,dbManager->GetEnvDY()*CLHEP::cm,dbManager->GetEnvDZ()*CLHEP::cm);
    GeoTransform*  posEcTT = new GeoTransform(HepGeom::Transform3D(vpos*(mx*(my*(mz)))));

    (*m_log) << MSG::INFO << " Global positioning of positive ext.barrel with rotation ("
             << dbManager->GetEnvDPhi() << "," << dbManager->GetEnvDTheta() << "," << dbManager->GetEnvDPsi() << ")"
             << ") and translation (" << dbManager->GetEnvDX() << "," << dbManager->GetEnvDY() << "," << dbManager->GetEnvDZ()
             << ") CLHEP::cm" << endmsg;

    world->add(posEcTT);
    world->add(pvTileEnvelopePosEndcap);
    m_detectorManager->addTreeTop(pvTileEnvelopePosEndcap);
  }
 
  if(EBC)
  {
    world->add(nTag);

    // Top transform for the Negative endcap
    dbManager->SetCurrentEnvByType(2);
    HepGeom::Transform3D mz = HepGeom::RotateZ3D(dbManager->GetEnvDPhi());
    HepGeom::Transform3D my = HepGeom::RotateY3D(dbManager->GetEnvDTheta());
    HepGeom::Transform3D mx = HepGeom::RotateZ3D(dbManager->GetEnvDPsi());
    HepGeom::Transform3D vpos = HepGeom::Translate3D(dbManager->GetEnvDX()*CLHEP::cm,dbManager->GetEnvDY()*CLHEP::cm,dbManager->GetEnvDZ()*CLHEP::cm);
    GeoTransform* negEcTT = new GeoTransform(HepGeom::Transform3D(vpos*(mx*(my*(mz)))));

    (*m_log) << MSG::INFO << " Global positioning of negative ext.barrel with rotation ("
             << dbManager->GetEnvDPhi() << "," << dbManager->GetEnvDTheta() << "," << dbManager->GetEnvDPsi() << ")"
             << ") and translation (" << dbManager->GetEnvDX() << "," << dbManager->GetEnvDY() << "," << dbManager->GetEnvDZ()
             << ") CLHEP::cm" << endmsg;

    world->add(negEcTT);
    world->add(pvTileEnvelopeNegEndcap);
    m_detectorManager->addTreeTop(pvTileEnvelopeNegEndcap);
  }

  delete sectionBuilder;
  //delete CutBuilder;

}


// Checking geometry dimensions for all directions

void TileAtlasFactory::checking(std::string Name, bool print, int level,
                                double X1, double X2, double Y1, double Y2, double Z) 
{
  double rless = .150; //150 [mkm]
  std::string Step[8] = {" ","  ","   ","    ","     ","      ","       ","        "};

  if (print)
   {
     if(m_log->level()<=MSG::DEBUG)
       (*m_log) << MSG::DEBUG <<Step[level]<<Name<<"-"<<level
		<<" dX1,dX2= "<<X1<<","<<X2<<" dY1,dY2= "<<Y1<<","<<Y2<<",dZ= "<<Z
		<<endmsg;
   }
  if (X1 < rless && X2 < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, X1 or X2<0 "<<endmsg;
   }
  if (Y1 < rless && Y2 < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, Y1 or Y2<0 "<<endmsg;
   }
  if (Z < rless)
   { (*m_log) << MSG::WARNING <<" volume "<<Name<<" is empty, Z<0   "<<endmsg;
   } 
} 
