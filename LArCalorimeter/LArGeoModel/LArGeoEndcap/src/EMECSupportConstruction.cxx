/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// EMECSupportConstruction
// April 2013 J.T. Insert extra material after PS in front of emec electronics
// June 2009 AMS General code cleanup
//               Static variables avioded
//               Almost all hardcoded numbers copied to DB
// 18-Sep-2006 V.N  Updated material definitions
// 19-Dec-2005 V.N  Inversed angle alpha for filling of electronic boards
// 28-Oct-2005 J.T. bug fixes, + InnerAluCone details
// 20-July-2004 J.T. back support included as well
// 06-Oct-2003 J.T. new class EMECSupportConstruction created for GeoModel:
// 06-Oct-2003 J.T. code of LArEMECSupportConstruction revised to work in
//                  the GeoModel framework
// 06-Oct-2003 J.T. isModule parameter was introduced some time ago
//                  (when and by whom - is not documented in this header)
//                  This destroyed the geometry of the BARRETTE sections
//                  either in the case of isModule=true or isModule=false.
//                  Therefore I have recalled the previous(original) geometry
//                  for isModule=false. In case of isModule=true, I kept
//                  phistart=Position-PI/8, m_PhiSize=PI/4, such a way that
//                  in case of Position=0, there is an electrode at phi=0
//                  (what I think was a feature of the implemented module's
//                  concept as well).
//                  Back structure always should be rotated by 180deg
//                  around x axis before placement into EMECMother.
//                  Therefore in case of isModule=true, an appropriate
//                  rotation around z is to be performed afterwards as well.
//                  Cables and electronics are not included.


// **** **** **** New Version of EMECSupportConstruction **** **** ****
// 31-July-2005 Vakho
// The idea was to reorganize EMECSupportConstruction in order to make it look
// as closer as possible to LArG4EC/LArEMECSupportConstruction.
//
// Known issues
//
// 1. Front envelope. rmax of FrontInnerLongBar reduced from 574.0 to 572.0
//        in order to avoid clash with FrontMiddleRing (clash comes from G4 description)
//
// 2. Outer envelope. rmin of Wide- and NarrowStretchers increased from 2044.0 to 2048.0
//        in order to avoid clashes with TopIndexingRing (clash comes from G4 description)
//
// 3. Few polycones give a 'Solid Problem' error message when running G4 geometry test
//        on translated geometry. The reason is not clear. Visualization does not show
//        any problems for these solids and it looks like simulation is not affected either
//
//
// ToDo:
// 1. Use parameterisations wherever possible;

#include<string>
#include<sstream>
#include<cassert>

#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include "GeoModelInterfaces/AbsMaterialManager.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PhysicalConstants.h"

#include "GeoModelUtilities/DecodeVersionKey.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include "GeoSpecialShapes/LArWheelCalculator.h"

#include "LArGeoEndcap/EMECConstruction.h"
#include "LArGeoEndcap/EMECSupportConstruction.h"

using namespace LArGeo;

EMECSupportConstruction::EMECSupportConstruction
	(int t, bool m, std::string basename, double position)
	: m_Type(t), m_isModule(m), m_BaseName(basename), m_Position(position)
{

//	std::cout << "Experimental EMECSupportConstruction" << std::endl;

	ISvcLocator *svcLocator = Gaudi::svcLocator();
	StoreGateSvc *detStore;
	if(svcLocator->service("DetectorStore", detStore, false) == StatusCode::FAILURE){
		throw std::runtime_error("Error in EMECSupportConstruction, cannot access DetectorStore");
	}
	const StoredMaterialManager* materialManager = nullptr;
	if (detStore->retrieve(materialManager, std::string("MATERIALS")).isFailure()) {
          throw std::runtime_error("Error in EMECSupportConstruction, cannot access MATERIALS");
        }

	m_PhiStart = 0.;
	m_PhiSize = Gaudi::Units::twopi*Gaudi::Units::rad;

	if(m_isModule){
		m_PhiStart = m_Position - M_PI*Gaudi::Units::rad / 8.;
		m_PhiSize = M_PI*Gaudi::Units::rad / 4.;
	}

  // Get the materials from the manager 

        m_Lead = materialManager->getMaterial("std::Lead");
        if(!m_Lead) throw std::runtime_error("Error in EMECSupportConstruction, std::Lead is not found.");

	m_Alu = materialManager->getMaterial("std::Aluminium");
	if(!m_Alu) throw std::runtime_error("Error in EMECSupportConstruction, std::Aluminium is not found.");

	m_Copper = materialManager->getMaterial("std::Copper");
	if(!m_Copper) throw std::runtime_error("Error in EMECSupportConstruction, std::Copper is not found.");

	m_LAr = materialManager->getMaterial("std::LiquidArgon");
	if(!m_LAr) throw std::runtime_error("Error in EMECSupportConstruction, std::LiquidArgon is not found.");

	m_Gten = materialManager->getMaterial("LAr::G10");
	if(!m_Gten) throw std::runtime_error("Error in EMECSupportConstruction, LAr::G10 is not found.");

	m_PermaliE730 = materialManager->getMaterial("LAr::Glue");
	if(!m_PermaliE730) throw std::runtime_error("Error in EMECSupportConstruction, LAr::Glue is not found.");

	m_G10FeOuter = materialManager->getMaterial("LAr::G10FeOuter");
	if(!m_G10FeOuter) throw std::runtime_error("Error in EMECSupportConstruction, LAr::G10FeOuter is not found.");

	m_G10FeInner  = materialManager->getMaterial("LAr::G10FeInner");
	if(!m_G10FeInner) throw std::runtime_error("Error in EMECSupportConstruction, LAr::G10FeInner is not found.");

	m_Kapton_Cu  = materialManager->getMaterial("LAr::KaptonC");
	if(!m_Kapton_Cu) throw std::runtime_error("Error in EMECSupportConstruction, LAr::KaptonC is not found.");

	m_Cable  = materialManager->getMaterial("LAr::Cables");
	if(!m_Cable) throw std::runtime_error("Error in EMECSupportConstruction, LAr::Cables is not found.");

	IGeoModelSvc *geoModel;
	IRDBAccessSvc* rdbAccess;

	if(svcLocator->service("GeoModelSvc", geoModel) == StatusCode::FAILURE)
		throw std::runtime_error("Error cannot access GeoModelSvc");
	if(svcLocator->service ("RDBAccessSvc",rdbAccess) == StatusCode::FAILURE)
		throw std::runtime_error("Error cannot access RDBAccessSvc");

        //emecExtraCyl, add extra material after PS
        std::string AtlasVersion = geoModel->atlasVersion();
        std::string LArVersion   = geoModel->LAr_VersionOverride();
        std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
        std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
        m_DB_emecExtraCyl = rdbAccess->getRecordsetPtr("LArCones",detectorKey, detectorNode);
        //--


	DecodeVersionKey larVersionKey(geoModel, "LAr");

	m_DB_EmecGeometry =
		rdbAccess->getRecordsetPtr("EmecGeometry", larVersionKey.tag(), larVersionKey.node());
	if(m_DB_EmecGeometry->size() == 0){
		m_DB_EmecGeometry = rdbAccess->getRecordsetPtr("EmecGeometry", "EmecGeometry-00");
	}

	m_DB_EmecWheelParameters =
		rdbAccess->getRecordsetPtr("EmecWheelParameters", larVersionKey.tag(), larVersionKey.node());
	if(m_DB_EmecWheelParameters->size() == 0){
		m_DB_EmecWheelParameters = rdbAccess->getRecordsetPtr("EmecWheelParameters", "EmecWheelParameters-00");
	}

	m_DB_boxes = rdbAccess->getRecordsetPtr("EmecDMBoxes", larVersionKey.tag(), larVersionKey.node());
	if(m_DB_boxes->size() == 0){
		m_DB_boxes = rdbAccess->getRecordsetPtr("EmecDMBoxes", "EmecDMBoxes-00");
	}
	m_DB_numbers = rdbAccess->getRecordsetPtr("EmecDMNumbers", larVersionKey.tag(), larVersionKey.node());
	if(m_DB_numbers->size() == 0){
		m_DB_numbers = rdbAccess->getRecordsetPtr("EmecDMNumbers", "EmecDMNumbers-00");
	}
	m_DB_tubes = rdbAccess->getRecordsetPtr("EmecDMTubes", larVersionKey.tag(), larVersionKey.node());
	if(m_DB_tubes->size() == 0){
		m_DB_tubes = rdbAccess->getRecordsetPtr("EmecDMTubes", "EmecDMTubes-00");
	}
	m_DB_pcons = rdbAccess->getRecordsetPtr("EmecDMPCons", larVersionKey.tag(), larVersionKey.node());
	if(m_DB_pcons->size() == 0){
		m_DB_pcons = rdbAccess->getRecordsetPtr("EmecDMPCons", "EmecDMPCons-00");
	}

	m_DB_mn = rdbAccess->getRecordsetPtr("EmecMagicNumbers", larVersionKey.tag(), larVersionKey.node());
	if(m_DB_mn->size() == 0){
		m_DB_mn = rdbAccess->getRecordsetPtr("EmecMagicNumbers", "EmecMagicNumbers-00");
	}
}

static void printWarning(const std::ostringstream &message)
{
		ISvcLocator* svcLocator = Gaudi::svcLocator();
		IMessageSvc* msgSvc;
		StatusCode status = svcLocator->service("MessageSvc", msgSvc);
		if(status.isFailure()){
			throw std::runtime_error("EMECSupportConstruction: cannot initialze message service");
		}
		MsgStream msg(msgSvc, "EMECSupportConstruction");
		msg << MSG::WARNING << message.str() << endmsg;
}

#include<map>
typedef std::map<std::string, unsigned int> map_t;
static map_t getMap(IRDBRecordset_ptr db, std::string s)
{
	map_t result;
	for(unsigned int i = 0; i < db->size(); ++ i){
		std::string key = (*db)[i]->getString(s);
		result[key] = i;
	}
	return result;
}

static map_t getNumbersMap(IRDBRecordset_ptr db, std::string s)
{
	map_t result;
	for(unsigned int i = 0; i < db->size(); ++ i){
		std::string object = (*db)[i]->getString("OBJECTNAME");
		if(object == s){
			std::string key = (*db)[i]->getString("PARNAME");
			result[key] = i;
//std::cout << s << ": " << key << " -> " << i << std::endl;
		}
	}
	return result;
}

static double getNumber(
	IRDBRecordset_ptr db, const map_t &m, const std::string &idx,
	const char *number, double defval = 0.
)
{
	map_t::const_iterator i = m.find(idx);
	if(i == m.end()){
		std::ostringstream tmp;
		tmp << "Cannot get " << idx << "/" << number << " from DB";
		printWarning(tmp);
		return defval;
	}
	double value = (*db)[(*i).second]->getDouble(number);
	assert(value == defval);
	return value;
}

static double getNumber(
	IRDBRecordset_ptr db, const std::string &s,
	const std::string &parameter, double defval = 0.)
{
	for(unsigned int i = 0; i < db->size(); ++ i){
		std::string object = (*db)[i]->getString("OBJECTNAME");
		if(object == s){
			std::string key = (*db)[i]->getString("PARNAME");
			if(key == parameter){
				double value = (*db)[i]->getDouble("PARVALUE");
				assert(value == defval);
				return value;
			}
		}
	}
	std::ostringstream tmp;
	tmp << "Cannot get " << s << "/" << parameter << " from DB_numbers";
	tmp << ", default is " << defval;
	printWarning(tmp);
	return defval;
}

EMECSupportConstruction::~EMECSupportConstruction() {}

GeoPhysVol* EMECSupportConstruction::GetEnvelope(void) const
{
	switch(m_Type){
	case 0: return front_envelope();
	case 1: return back_envelope();
	case 2: return outer_envelope();
	case 3: return inner_envelope();
	case 4: return middle_envelope();
	case 10: return front_inner_envelope();
	case 11: return back_inner_envelope();
	case 12: return front_outer_envelope();
	case 13: return back_outer_envelope();
	default:
		std::ostringstream tmp;
		tmp << "Unknown Type " << m_Type << " in GetEnvelope,"
			<< " null pointer returned";
		printWarning(tmp);
		return 0;
	}
}

GeoPcon* EMECSupportConstruction::getPcon(std::string id) const
{
	double phi_start = m_PhiStart;
	double phi_size = m_PhiSize;
	std::vector<double> zplane, rmin, rmax;

	std::string id1 = id;
	if(id.find("FrontSupportMother") == 0) id1 = "FrontSupportMother";
	else if(id.find("BackSupportMother") == 0) id1 = "BackSupportMother";
	else if(id.find("Stretchers") != std::string::npos) id1 = "Stretchers";
	else if(id == "FrontMiddleRing::LowerHole") id1 = "FrontMiddleRing::LH";
	else if(id == "FrontMiddleRing::LowerGTen") id1 = "FrontMiddleRing::LGT";
	else if(id == "FrontMiddleRing::UpperHole") id1 = "FrontMiddleRing::UH";
	else if(id == "FrontMiddleRing::UpperGTen") id1 = "FrontMiddleRing::UGT";
	else if(id == "BackMiddleRing::LowerHole") id1 = "BackMiddleRing::LH";
	else if(id == "BackMiddleRing::LowerGTen") id1 = "BackMiddleRing::LGT";
	else if(id == "BackMiddleRing::UpperHole") id1 = "BackMiddleRing::UH";
	else if(id == "BackMiddleRing::UpperGTen") id1 = "BackMiddleRing::UGT";

	std::map<int, unsigned int> pcone;
	int nzplanes = 0;
	double R0 = 0.;
	for(unsigned int i = 0; i < m_DB_pcons->size(); ++ i){
		std::string object = (*m_DB_pcons)[i]->getString("PCONNAME");
		if(object == id1){
			int key = (*m_DB_pcons)[i]->getInt("NZPLANE");
			if(pcone.find(key) != pcone.end()){
				std::ostringstream tmp;
				tmp << "Duplicate NZPLANE in " << id;
				printWarning(tmp);
				nzplanes = 0;
				break;
			}
			pcone[key] = i;
			if(key >= 0) ++ nzplanes;
			else R0 = (*m_DB_pcons)[i]->getDouble("RMIN")*Gaudi::Units::mm;
		}
	}
	if(nzplanes > 0){
		zplane.resize(nzplanes); rmin.resize(nzplanes); rmax.resize(nzplanes);
		for(int n = 0; n < nzplanes; ++ n){
			zplane[n] = (*m_DB_pcons)[pcone[n]]->getDouble("ZPOS")*Gaudi::Units::mm;
			rmin[n] = R0 + (*m_DB_pcons)[pcone[n]]->getDouble("RMIN")*Gaudi::Units::mm;
			rmax[n] = R0 + (*m_DB_pcons)[pcone[n]]->getDouble("RMAX")*Gaudi::Units::mm;
		}
		if(id1 == "FrontSupportMother"){
			if(id.find("Inner") != std::string::npos){
				zplane.resize(2); rmin.resize(2); rmax.resize(2);
				double rlim = getNumber(m_DB_numbers, id, "Inner", 614.)*Gaudi::Units::mm;
				rmax[0] = rlim;
				rmax[1] = rlim;
			} else if(id.find("Outer") != std::string::npos){
				double rlim = getNumber(m_DB_numbers, id, "Outer", 603.-1.)*Gaudi::Units::mm;
				rmin[0] = rlim;
				rmin[1] = rlim;
			}
		}
		if(id1 == "BackSupportMother"){
			if(id.find("Inner") != std::string::npos){
				zplane.resize(2); rmin.resize(2); rmax.resize(2);
				double rlim = getNumber(m_DB_numbers, id, "Inner", 699.)*Gaudi::Units::mm;
				rmax[0] = rlim;
				rmax[1] = rlim;
			} else if(id.find("Outer") != std::string::npos){
				double rlim = getNumber(m_DB_numbers, id, "Outer", 687.-1.)*Gaudi::Units::mm;
				rmin[0] = rlim;
				rmin[1] = rlim;
			}
		}
		if(id1 == "Stretchers"){
			if(id == "WideStretchers"){
				double dfiWS = 360./3./256.*24.*Gaudi::Units::deg; //this is the design variable for WS
				phi_start = m_Position - dfiWS*0.5;
				phi_size = dfiWS;
			}
			if(id == "NarrowStretchers"){
			        double lengthNS = getNumber(m_DB_numbers, id, "Width", 200.)*Gaudi::Units::mm; // transversal length of NS
				double dfiNS = lengthNS / rmax[0] * Gaudi::Units::rad;
				phi_start = m_Position - dfiNS*0.5;
				phi_size = dfiNS;
			}
		}
//#define DEBUG
#ifdef DEBUG
std::cout << "!!!! " << id << ":" << std::endl;
for(int i = 0; i < nzplanes; ++ i){
	std::cout << "\t" << i << " " << zplane[i] << " " << rmin[i] << " " << rmax[i] << std::endl;
}
#endif
	} else {
	if(id.find("FrontSupportMother") == 0){
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] =   0. *Gaudi::Units::mm; rmin[0] =  292.*Gaudi::Units::mm-1.*Gaudi::Units::mm; rmax[0] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane[1] =  61. *Gaudi::Units::mm; rmin[1] =  292.*Gaudi::Units::mm-1.*Gaudi::Units::mm; rmax[1] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane[2] =  61. *Gaudi::Units::mm; rmin[2] = 2023.*Gaudi::Units::mm-7.*Gaudi::Units::mm; rmax[2] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane[3] =  72.3*Gaudi::Units::mm; rmin[3] = 2023.*Gaudi::Units::mm-7.*Gaudi::Units::mm; rmax[3] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane[4] = 124.2*Gaudi::Units::mm; rmin[4] = 2051.*Gaudi::Units::mm-7.*Gaudi::Units::mm; rmax[4] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane[5] = 153. *Gaudi::Units::mm; rmin[5] = 2051.*Gaudi::Units::mm-7.*Gaudi::Units::mm; rmax[5] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		if(id == "FrontSupportMother::Outer"){
			rmin[0] = 603.*Gaudi::Units::mm-1.*Gaudi::Units::mm;
			rmin[1] = 603.*Gaudi::Units::mm-1.*Gaudi::Units::mm;
		}
		if(id == "FrontSupportMother::Inner"){
			zplane.resize(2); rmin.resize(2); rmax.resize(2);
			rmax[0] = 614.*Gaudi::Units::mm;
			rmax[1] = 614.*Gaudi::Units::mm;
		}
	} else if(id.find("BackSupportMother") == 0){
		zplane.resize(4);  rmin.resize(4);    rmax.resize(4);
		zplane[0] =   0.001*Gaudi::Units::mm; rmin[0] =  333.*Gaudi::Units::mm-1.*Gaudi::Units::mm; rmax[0] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane[1] =  55.   *Gaudi::Units::mm; rmin[1] =  333.*Gaudi::Units::mm-1.*Gaudi::Units::mm; rmax[1] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane[2] =  55.   *Gaudi::Units::mm; rmin[2] = 2051.*Gaudi::Units::mm-7.*Gaudi::Units::mm; rmax[2] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane[3] =  147.  *Gaudi::Units::mm; rmin[3] = 2051.*Gaudi::Units::mm-7.*Gaudi::Units::mm; rmax[3] = 2077.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		if(id == "BackSupportMother::Outer"){
			rmin[0] = 687.*Gaudi::Units::mm-1.*Gaudi::Units::mm;
			rmin[1] = 687.*Gaudi::Units::mm-1.*Gaudi::Units::mm;
		}
		if(id == "BackSupportMother::Inner"){
			zplane.resize(2); rmin.resize(2); rmax.resize(2);
			rmax[0] = 699.*Gaudi::Units::mm;
			rmax[1] = 699.*Gaudi::Units::mm;
		}
	} else if(id == "WideStretchers" || id == "NarrowStretchers"){
		double dzS = 165.*Gaudi::Units::mm;
		double dznotch = 10.*Gaudi::Units::mm; // half z extent of the notch
		double drnotch = 6.5*Gaudi::Units::mm; // deepness of the noth in radial direction
		double rmaxS = (2077. - 7.)*Gaudi::Units::mm;//ROuter+116. // -7 for cold
		double rminS = rmaxS - 26.*Gaudi::Units::mm;
		double rmidS = rminS + drnotch;
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = -dzS ; rmin[0] = rminS; rmax[0] = rmaxS;
		zplane[1] = -dznotch; rmin[1] = rminS; rmax[1] = rmaxS;
		zplane[2] = -dznotch; rmin[2] = rmidS; rmax[2] = rmaxS;
		zplane[3] =  dznotch; rmin[3] = rmidS; rmax[3] = rmaxS;
		zplane[4] = dznotch; rmin[4] = rminS; rmax[4] = rmaxS;
		zplane[5] = dzS  ; rmin[5] = rminS; rmax[5] = rmaxS;
		if(id == "WideStretchers"){
			double dfiWS = 360./3./256.*24.*Gaudi::Units::deg; //this is the design variable for WS
			phi_start = m_Position - dfiWS*0.5;
			phi_size = dfiWS;
		}
		if(id == "NarrowStretchers"){
			double lengthNS = 200.*Gaudi::Units::mm; // transversal length of NS
			double dfiNS = lengthNS / rmaxS * Gaudi::Units::rad;
			phi_start = m_Position - dfiNS*0.5;
			phi_size = dfiNS;
		}
	} else if(id == "OuterSupportMother"){
		double dzS = 165.*Gaudi::Units::mm;
		double rmaxS = (2077. - 7.)*Gaudi::Units::mm;//ROuter+116. // -7 for cold
		double rminOTB = (2034. + 2.)*Gaudi::Units::mm;
		double rmaxOTB = rminOTB + 3.*Gaudi::Units::mm;
		double dzOTB = 201.*Gaudi::Units::mm;
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = -dzOTB ; rmin[0] = rminOTB; rmax[0] = rmaxOTB;
		zplane[1] = -dzS; rmin[1] = rminOTB; rmax[1] = rmaxOTB;
		zplane[2] = -dzS; rmin[2] = rminOTB; rmax[2] = rmaxS;
		zplane[3] =  dzS; rmin[3] = rminOTB; rmax[3] = rmaxS;
		zplane[4] = dzS; rmin[4] = rminOTB; rmax[4] = rmaxOTB;
		zplane[5] = dzOTB  ; rmin[5] = rminOTB; rmax[5] = rmaxOTB;
	} else if(id == "FrontMiddleRing"){
		double r0       =614.*Gaudi::Units::mm-2.*Gaudi::Units::mm ; // RMiddle=middle radius of the ring
		zplane.resize(4);  rmin.resize(4);    rmax.resize(4);
		zplane[0] =   0. *Gaudi::Units::mm; rmin[0] = r0 - 57.*Gaudi::Units::mm; rmax[0] = r0 + 57.*Gaudi::Units::mm;
		zplane[1] =  27.5*Gaudi::Units::mm; rmin[1] = r0 - 57.*Gaudi::Units::mm; rmax[1] = r0 + 57.*Gaudi::Units::mm;
		zplane[2] =  27.5*Gaudi::Units::mm; rmin[2] = r0 - 40.*Gaudi::Units::mm; rmax[2] = r0 + 40.*Gaudi::Units::mm;
		zplane[3] =  59. *Gaudi::Units::mm; rmin[3] = r0 - 40.*Gaudi::Units::mm; rmax[3] = r0 + 40.*Gaudi::Units::mm;
	} else if(id == "FrontMiddleRing::LowerHole"){
		double r0 = 614.*Gaudi::Units::mm-2.*Gaudi::Units::mm; // RMiddle=middle radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 23. *Gaudi::Units::mm; rmin[0] = r0 - 28.3*Gaudi::Units::mm; rmax[0] = r0 - 8.*Gaudi::Units::mm;
		zplane[1] = 27.5*Gaudi::Units::mm; rmin[1] = r0 - 28.3*Gaudi::Units::mm; rmax[1] = r0 - 8.*Gaudi::Units::mm;
		zplane[2] = 27.5*Gaudi::Units::mm; rmin[2] = r0 - 40. *Gaudi::Units::mm; rmax[2] = r0 - 8.*Gaudi::Units::mm;
		zplane[3] = 48.5*Gaudi::Units::mm; rmin[3] = r0 - 40. *Gaudi::Units::mm; rmax[3] = r0 - 8.*Gaudi::Units::mm;
		zplane[4] = 48.5*Gaudi::Units::mm; rmin[4] = r0 - 28.3*Gaudi::Units::mm; rmax[4] = r0 - 8.*Gaudi::Units::mm;
		zplane[5] = 53. *Gaudi::Units::mm; rmin[5] = r0 - 28.3*Gaudi::Units::mm; rmax[5] = r0 - 8.*Gaudi::Units::mm;
	} else if(id == "FrontMiddleRing::LowerGTen"){
		double r0 = 614.*Gaudi::Units::mm - 2.*Gaudi::Units::mm; // RMiddle=middle radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 23.*Gaudi::Units::mm; rmin[0] = r0 - 28.*Gaudi::Units::mm; rmax[0] = r0 - 8.*Gaudi::Units::mm;
		zplane[1] = 28.*Gaudi::Units::mm; rmin[1] = r0 - 28.*Gaudi::Units::mm; rmax[1] = r0 - 8.*Gaudi::Units::mm;
		zplane[2] = 28.*Gaudi::Units::mm; rmin[2] = r0 - 40.*Gaudi::Units::mm; rmax[2] = r0 - 8.*Gaudi::Units::mm;
		zplane[3] = 48.*Gaudi::Units::mm; rmin[3] = r0 - 40.*Gaudi::Units::mm; rmax[3] = r0 - 8.*Gaudi::Units::mm;
		zplane[4] = 48.*Gaudi::Units::mm; rmin[4] = r0 - 28.*Gaudi::Units::mm; rmax[4] = r0 - 8.*Gaudi::Units::mm;
		zplane[5] = 53.*Gaudi::Units::mm; rmin[5] = r0 - 28.*Gaudi::Units::mm; rmax[5] = r0 - 8.*Gaudi::Units::mm;
	} else if(id == "FrontMiddleRing::UpperHole"){
		double r0       =614.*Gaudi::Units::mm-2.*Gaudi::Units::mm ; // RMiddle=middle radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 23. *Gaudi::Units::mm; rmin[0] = r0 + 8.*Gaudi::Units::mm; rmax[0] = r0 + 28.3*Gaudi::Units::mm;
		zplane[1] = 27.5*Gaudi::Units::mm; rmin[1] = r0 + 8.*Gaudi::Units::mm; rmax[1] = r0 + 28.3*Gaudi::Units::mm;
		zplane[2] = 27.5*Gaudi::Units::mm; rmin[2] = r0 + 8.*Gaudi::Units::mm; rmax[2] = r0 + 40. *Gaudi::Units::mm;
		zplane[3] = 48.5*Gaudi::Units::mm; rmin[3] = r0 + 8.*Gaudi::Units::mm; rmax[3] = r0 + 40. *Gaudi::Units::mm;
		zplane[4] = 48.5*Gaudi::Units::mm; rmin[4] = r0 + 8.*Gaudi::Units::mm; rmax[4] = r0 + 28.3*Gaudi::Units::mm;
		zplane[5] = 53. *Gaudi::Units::mm; rmin[5] = r0 + 8.*Gaudi::Units::mm; rmax[5] = r0 + 28.3*Gaudi::Units::mm;
	} else if(id == "FrontMiddleRing::UpperGTen"){
		double r0       =614.*Gaudi::Units::mm-2.*Gaudi::Units::mm ; // RMiddle=middle radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 23.*Gaudi::Units::mm; rmin[0] = r0 + 8.*Gaudi::Units::mm; rmax[0] = r0 + 28.*Gaudi::Units::mm;
		zplane[1] = 28.*Gaudi::Units::mm; rmin[1] = r0 + 8.*Gaudi::Units::mm; rmax[1] = r0 + 28.*Gaudi::Units::mm;
		zplane[2] = 28.*Gaudi::Units::mm; rmin[2] = r0 + 8.*Gaudi::Units::mm; rmax[2] = r0 + 40.*Gaudi::Units::mm;
		zplane[3] = 48.*Gaudi::Units::mm; rmin[3] = r0 + 8.*Gaudi::Units::mm; rmax[3] = r0 + 40.*Gaudi::Units::mm;
		zplane[4] = 48.*Gaudi::Units::mm; rmin[4] = r0 + 8.*Gaudi::Units::mm; rmax[4] = r0 + 28.*Gaudi::Units::mm;
		zplane[5] = 53.*Gaudi::Units::mm; rmin[5] = r0 + 8.*Gaudi::Units::mm; rmax[5] = r0 + 28.*Gaudi::Units::mm;
	} else if(id == "FrontInnerRing"){
		double r0 = 335.5*Gaudi::Units::mm-1.*Gaudi::Units::mm;  // RInner = reference radius of the inner ring
		zplane.resize(5);  rmin.resize(5);    rmax.resize(5);
		zplane[0] =  0. *Gaudi::Units::mm; rmin[0] = r0 - 22.5*Gaudi::Units::mm; rmax[0] = r0 + 51.5*Gaudi::Units::mm;
		zplane[1] =  6. *Gaudi::Units::mm; rmin[1] = r0 - 28.5*Gaudi::Units::mm; rmax[1] = r0 + 51.5*Gaudi::Units::mm;
		zplane[2] = 27.5*Gaudi::Units::mm; rmin[2] = r0 - 28.5*Gaudi::Units::mm; rmax[2] = r0 + 51.5*Gaudi::Units::mm;
		zplane[3] = 27.5*Gaudi::Units::mm; rmin[3] = r0 - 43.5*Gaudi::Units::mm; rmax[3] = r0 + 40.5*Gaudi::Units::mm;
		zplane[4] = 59. *Gaudi::Units::mm; rmin[4] = r0 - 43.5*Gaudi::Units::mm; rmax[4] = r0 + 40.5*Gaudi::Units::mm;
	} else if(id == "FrontInnerRing::Hole"){
		double r0 = 335.5*Gaudi::Units::mm-1.*Gaudi::Units::mm;  // RInner = reference radius of the inner ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 23. *Gaudi::Units::mm; rmin[0] = r0 + 6.5*Gaudi::Units::mm; rmax[0] = r0 + 29.5*Gaudi::Units::mm;
		zplane[1] = 27.5*Gaudi::Units::mm; rmin[1] = r0 + 6.5*Gaudi::Units::mm; rmax[1] = r0 + 29.5*Gaudi::Units::mm;
		zplane[2] = 27.5*Gaudi::Units::mm; rmin[2] = r0 + 6.5*Gaudi::Units::mm; rmax[2] = r0 + 40.5*Gaudi::Units::mm;
		zplane[3] = 48.5*Gaudi::Units::mm; rmin[3] = r0 + 6.5*Gaudi::Units::mm; rmax[3] = r0 + 40.5*Gaudi::Units::mm;
		zplane[4] = 48.5*Gaudi::Units::mm; rmin[4] = r0 + 6.5*Gaudi::Units::mm; rmax[4] = r0 + 29.5*Gaudi::Units::mm;
		zplane[5] = 53. *Gaudi::Units::mm; rmin[5] = r0 + 6.5*Gaudi::Units::mm; rmax[5] = r0 + 29.5*Gaudi::Units::mm;
	} else if(id == "FrontInnerRing::GTen"){
		double r0 = 335.5*Gaudi::Units::mm-1.*Gaudi::Units::mm;  // RInner = reference radius of the inner ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 23.*Gaudi::Units::mm; rmin[0] = r0 + 8.5*Gaudi::Units::mm; rmax[0] = r0 + 28.5*Gaudi::Units::mm;
		zplane[1] = 28.*Gaudi::Units::mm; rmin[1] = r0 + 8.5*Gaudi::Units::mm; rmax[1] = r0 + 28.5*Gaudi::Units::mm;
		zplane[2] = 28.*Gaudi::Units::mm; rmin[2] = r0 + 8.5*Gaudi::Units::mm; rmax[2] = r0 + 40.5*Gaudi::Units::mm;
		zplane[3] = 48.*Gaudi::Units::mm; rmin[3] = r0 + 8.5*Gaudi::Units::mm; rmax[3] = r0 + 40.5*Gaudi::Units::mm;
		zplane[4] = 48.*Gaudi::Units::mm; rmin[4] = r0 + 8.5*Gaudi::Units::mm; rmax[4] = r0 + 28.5*Gaudi::Units::mm;
		zplane[5] = 53.*Gaudi::Units::mm; rmin[5] = r0 + 8.5*Gaudi::Units::mm; rmax[5] = r0 + 28.5*Gaudi::Units::mm;
	} else if(id == "BackMiddleRing"){
		double r0       =  699.*Gaudi::Units::mm-2.5*Gaudi::Units::mm; // RMiddle radius of the ring
		zplane.resize(4);  rmin.resize(4);    rmax.resize(4);
		zplane[0] =   0. *Gaudi::Units::mm; rmin[0] = r0 - 57.*Gaudi::Units::mm; rmax[0] = r0 + 57.*Gaudi::Units::mm;
		zplane[1] =  21. *Gaudi::Units::mm; rmin[1] = r0 - 57.*Gaudi::Units::mm; rmax[1] = r0 + 57.*Gaudi::Units::mm;
		zplane[2] =  21. *Gaudi::Units::mm; rmin[2] = r0 - 40.*Gaudi::Units::mm; rmax[2] = r0 + 40.*Gaudi::Units::mm;
		zplane[3] =  52.5*Gaudi::Units::mm; rmin[3] = r0 - 40.*Gaudi::Units::mm; rmax[3] = r0 + 40.*Gaudi::Units::mm;
	} else if(id == "BackMiddleRing::LowerHole"){
		double r0       =  699.*Gaudi::Units::mm-2.5*Gaudi::Units::mm; // RMiddle radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 16.5*Gaudi::Units::mm; rmin[0] = r0 - 28.3*Gaudi::Units::mm; rmax[0] = r0 - 8.*Gaudi::Units::mm;
		zplane[1] = 21. *Gaudi::Units::mm; rmin[1] = r0 - 28.3*Gaudi::Units::mm; rmax[1] = r0 - 8.*Gaudi::Units::mm;
		zplane[2] = 21. *Gaudi::Units::mm; rmin[2] = r0 - 40. *Gaudi::Units::mm; rmax[2] = r0 - 8.*Gaudi::Units::mm;
		zplane[3] = 42. *Gaudi::Units::mm; rmin[3] = r0 - 40. *Gaudi::Units::mm; rmax[3] = r0 - 8.*Gaudi::Units::mm;
		zplane[4] = 42. *Gaudi::Units::mm; rmin[4] = r0 - 28.3*Gaudi::Units::mm; rmax[4] = r0 - 8.*Gaudi::Units::mm;
//		zplane[5] = 56.5*Gaudi::Units::mm; rmin[5] = r0 - 28.3*Gaudi::Units::mm; rmax[5] = r0 - 8.*Gaudi::Units::mm;
		zplane[5] = 46.5*Gaudi::Units::mm; rmin[5] = r0 - 28.3*Gaudi::Units::mm; rmax[5] = r0 - 8.*Gaudi::Units::mm;
	} else if(id == "BackMiddleRing::LowerGTen"){
		double r0       =  699.*Gaudi::Units::mm-2.5*Gaudi::Units::mm; // RMiddle radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 16.5*Gaudi::Units::mm; rmin[0] = r0 - 28.*Gaudi::Units::mm; rmax[0] = r0 - 8.*Gaudi::Units::mm;
		zplane[1] = 21.5*Gaudi::Units::mm; rmin[1] = r0 - 28.*Gaudi::Units::mm; rmax[1] = r0 - 8.*Gaudi::Units::mm;
		zplane[2] = 21.5*Gaudi::Units::mm; rmin[2] = r0 - 40.*Gaudi::Units::mm; rmax[2] = r0 - 8.*Gaudi::Units::mm;
		zplane[3] = 41.5*Gaudi::Units::mm; rmin[3] = r0 - 40.*Gaudi::Units::mm; rmax[3] = r0 - 8.*Gaudi::Units::mm;
		zplane[4] = 41.5*Gaudi::Units::mm; rmin[4] = r0 - 28.*Gaudi::Units::mm; rmax[4] = r0 - 8.*Gaudi::Units::mm;
		zplane[5] = 46.5*Gaudi::Units::mm; rmin[5] = r0 - 28.*Gaudi::Units::mm; rmax[5] = r0 - 8.*Gaudi::Units::mm;
	} else if(id == "BackMiddleRing::UpperHole"){
		double r0       =  699.*Gaudi::Units::mm-2.5*Gaudi::Units::mm; // RMiddle radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 16.5*Gaudi::Units::mm; rmin[0] = r0 + 8.*Gaudi::Units::mm; rmax[0] = r0 + 28.3*Gaudi::Units::mm;
		zplane[1] = 21. *Gaudi::Units::mm; rmin[1] = r0 + 8.*Gaudi::Units::mm; rmax[1] = r0 + 28.3*Gaudi::Units::mm;
		zplane[2] = 21. *Gaudi::Units::mm; rmin[2] = r0 + 8.*Gaudi::Units::mm; rmax[2] = r0 + 40. *Gaudi::Units::mm;
		zplane[3] = 42. *Gaudi::Units::mm; rmin[3] = r0 + 8.*Gaudi::Units::mm; rmax[3] = r0 + 40. *Gaudi::Units::mm;
		zplane[4] = 42. *Gaudi::Units::mm; rmin[4] = r0 + 8.*Gaudi::Units::mm; rmax[4] = r0 + 28.3*Gaudi::Units::mm;
		zplane[5] = 46.5*Gaudi::Units::mm; rmin[5] = r0 + 8.*Gaudi::Units::mm; rmax[5] = r0 + 28.3*Gaudi::Units::mm;
	} else if(id == "BackMiddleRing::UpperGTen"){
		double r0       =  699.*Gaudi::Units::mm-2.5*Gaudi::Units::mm; // RMiddle radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 16.5*Gaudi::Units::mm; rmin[0] = r0 + 8.*Gaudi::Units::mm; rmax[0] = r0 + 28.*Gaudi::Units::mm;
		zplane[1] = 21.5*Gaudi::Units::mm; rmin[1] = r0 + 8.*Gaudi::Units::mm; rmax[1] = r0 + 28.*Gaudi::Units::mm;
		zplane[2] = 21.5*Gaudi::Units::mm; rmin[2] = r0 + 8.*Gaudi::Units::mm; rmax[2] = r0 + 40.*Gaudi::Units::mm;
		zplane[3] = 41.5*Gaudi::Units::mm; rmin[3] = r0 + 8.*Gaudi::Units::mm; rmax[3] = r0 + 40.*Gaudi::Units::mm;
		zplane[4] = 41.5*Gaudi::Units::mm; rmin[4] = r0 + 8.*Gaudi::Units::mm; rmax[4] = r0 + 28.*Gaudi::Units::mm;
		zplane[5] = 46.5*Gaudi::Units::mm; rmin[5] = r0 + 8.*Gaudi::Units::mm; rmax[5] = r0 + 28.*Gaudi::Units::mm;
	} else if(id == "BackInnerRing"){
		double r0       =357.5*Gaudi::Units::mm-1.*Gaudi::Units::mm;  // RInner = reference radius of the ring
		zplane.resize(4);  rmin.resize(4);    rmax.resize(4);
		zplane[0] =   0. *Gaudi::Units::mm; rmin[0] = r0 - 22.5*Gaudi::Units::mm; rmax[0] = r0 + 53.5*Gaudi::Units::mm;
		zplane[1] =  22.5*Gaudi::Units::mm; rmin[1] = r0 - 22.5*Gaudi::Units::mm; rmax[1] = r0 + 53.5*Gaudi::Units::mm;
		zplane[2] =  22.5*Gaudi::Units::mm; rmin[2] = r0 - 24.5*Gaudi::Units::mm; rmax[2] = r0 + 40.5*Gaudi::Units::mm;
		zplane[3] =  54. *Gaudi::Units::mm; rmin[3] = r0 - 24.5*Gaudi::Units::mm; rmax[3] = r0 + 40.5*Gaudi::Units::mm;
	} else if(id == "BackInnerRing::Hole"){
		double r0       =357.5*Gaudi::Units::mm-1.*Gaudi::Units::mm;  // RInner = reference radius of the ring
 		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 18. *Gaudi::Units::mm; rmin[0] = r0 + 6.5*Gaudi::Units::mm; rmax[0] = r0 + 29.5*Gaudi::Units::mm;
		zplane[1] = 22.5*Gaudi::Units::mm; rmin[1] = r0 + 6.5*Gaudi::Units::mm; rmax[1] = r0 + 29.5*Gaudi::Units::mm;
		zplane[2] = 22.5*Gaudi::Units::mm; rmin[2] = r0 + 6.5*Gaudi::Units::mm; rmax[2] = r0 + 40.5*Gaudi::Units::mm;
		zplane[3] = 43.5*Gaudi::Units::mm; rmin[3] = r0 + 6.5*Gaudi::Units::mm; rmax[3] = r0 + 40.5*Gaudi::Units::mm;
		zplane[4] = 43.5*Gaudi::Units::mm; rmin[4] = r0 + 6.5*Gaudi::Units::mm; rmax[4] = r0 + 29.5*Gaudi::Units::mm;
		zplane[5] = 48. *Gaudi::Units::mm; rmin[5] = r0 + 6.5*Gaudi::Units::mm; rmax[5] = r0 + 29.5*Gaudi::Units::mm;
	} else if(id == "BackInnerRing::GTen"){
		double r0       =357.5*Gaudi::Units::mm-1.*Gaudi::Units::mm;  // RInner = reference radius of the ring
		zplane.resize(6);  rmin.resize(6);    rmax.resize(6);
		zplane[0] = 18.*Gaudi::Units::mm; rmin[0] = r0 + 8.5*Gaudi::Units::mm; rmax[0] = r0 + 28.5*Gaudi::Units::mm;
		zplane[1] = 23.*Gaudi::Units::mm; rmin[1] = r0 + 8.5*Gaudi::Units::mm; rmax[1] = r0 + 28.5*Gaudi::Units::mm;
		zplane[2] = 23.*Gaudi::Units::mm; rmin[2] = r0 + 8.5*Gaudi::Units::mm; rmax[2] = r0 + 40.5*Gaudi::Units::mm;
		zplane[3] = 43.*Gaudi::Units::mm; rmin[3] = r0 + 8.5*Gaudi::Units::mm; rmax[3] = r0 + 40.5*Gaudi::Units::mm;
		zplane[4] = 43.*Gaudi::Units::mm; rmin[4] = r0 + 8.5*Gaudi::Units::mm; rmax[4] = r0 + 28.5*Gaudi::Units::mm;
		zplane[5] = 48.*Gaudi::Units::mm; rmin[5] = r0 + 8.5*Gaudi::Units::mm; rmax[5] = r0 + 28.5*Gaudi::Units::mm;
	} else if(id == "FrontOuterRing"){
		double r0 = 1961.*Gaudi::Units::mm-7.*Gaudi::Units::mm; // ROuter = inner radius of the outer ring
		zplane.resize(7);  rmin.resize(7);    rmax.resize(7);
		zplane[0] =   0. *Gaudi::Units::mm; rmin[0] = r0 +  0.*Gaudi::Units::mm; rmax[0] = r0 + 111.*Gaudi::Units::mm;
		zplane[1] =   5. *Gaudi::Units::mm; rmin[1] = r0 +  0.*Gaudi::Units::mm; rmax[1] = r0 + 116.*Gaudi::Units::mm;
		zplane[2] =  20. *Gaudi::Units::mm; rmin[2] = r0 +  0.*Gaudi::Units::mm; rmax[2] = r0 + 116.*Gaudi::Units::mm;
		zplane[3] =  20. *Gaudi::Units::mm; rmin[3] = r0 + 62.*Gaudi::Units::mm; rmax[3] = r0 + 116.*Gaudi::Units::mm;
		zplane[4] =  63.3*Gaudi::Units::mm; rmin[4] = r0 + 62.*Gaudi::Units::mm; rmax[4] = r0 + 116.*Gaudi::Units::mm;
		zplane[5] = 115.2*Gaudi::Units::mm; rmin[5] = r0 + 90.*Gaudi::Units::mm; rmax[5] = r0 + 116.*Gaudi::Units::mm;
		zplane[6] = 144. *Gaudi::Units::mm; rmin[6] = r0 + 90.*Gaudi::Units::mm; rmax[6] = r0 + 116.*Gaudi::Units::mm;
	} else if(id == "FrontOuterLongBar"){
		zplane.resize(4);  rmin.resize(4);    rmax.resize(4);
		zplane[0] =  0.*Gaudi::Units::mm; rmin[0] = 1969.7*Gaudi::Units::mm; rmax[0] = 2016.*Gaudi::Units::mm;
		zplane[1] =  1.*Gaudi::Units::mm; rmin[1] = 1969.7*Gaudi::Units::mm; rmax[1] = 2016.*Gaudi::Units::mm;
		zplane[2] =  1.*Gaudi::Units::mm; rmin[2] =  652. *Gaudi::Units::mm; rmax[2] = 2016.*Gaudi::Units::mm;
		zplane[3] = 21.*Gaudi::Units::mm; rmin[3] =  652. *Gaudi::Units::mm; rmax[3] = 2016.*Gaudi::Units::mm;
                    //2020-46.3 ; RMiddle+40.//RMiddle+8+(lengthofbar=1398)
	} else if(id == "BackOuterRing"){
		double r0 = 1961.*Gaudi::Units::mm-7.*Gaudi::Units::mm;
		zplane.resize(7);  rmin.resize(7);    rmax.resize(7);
		zplane[0] =   0.*Gaudi::Units::mm; rmin[0] = r0 +  0.*Gaudi::Units::mm; rmax[0] = r0 + 111.*Gaudi::Units::mm;
		zplane[1] =   5.*Gaudi::Units::mm; rmin[1] = r0 +  0.*Gaudi::Units::mm; rmax[1] = r0 + 116.*Gaudi::Units::mm;
		zplane[2] =  15.*Gaudi::Units::mm; rmin[2] = r0 +  0.*Gaudi::Units::mm; rmax[2] = r0 + 116.*Gaudi::Units::mm;
		zplane[3] =  15.*Gaudi::Units::mm; rmin[3] = r0 + 62.*Gaudi::Units::mm; rmax[3] = r0 + 116.*Gaudi::Units::mm;
		zplane[4] =  41.*Gaudi::Units::mm; rmin[4] = r0 + 62.*Gaudi::Units::mm; rmax[4] = r0 + 116.*Gaudi::Units::mm;
		zplane[5] =  41.*Gaudi::Units::mm; rmin[5] = r0 + 90.*Gaudi::Units::mm; rmax[5] = r0 + 116.*Gaudi::Units::mm;
		zplane[6] = 139.*Gaudi::Units::mm; rmin[6] = r0 + 90.*Gaudi::Units::mm; rmax[6] = r0 + 116.*Gaudi::Units::mm;
	} else if(id == "BackOuterLongBar"){
		zplane.resize(4);  rmin.resize(4);    rmax.resize(4);
		zplane[0] =  0.*Gaudi::Units::mm; rmin[0] = 1969.7*Gaudi::Units::mm; rmax[0] = 2016.*Gaudi::Units::mm;
		zplane[1] =  1.*Gaudi::Units::mm; rmin[1] = 1969.7*Gaudi::Units::mm; rmax[1] = 2016.*Gaudi::Units::mm;
		zplane[2] =  1.*Gaudi::Units::mm; rmin[2] =  736.5*Gaudi::Units::mm; rmax[2] = 2016.*Gaudi::Units::mm;
		zplane[3] = 21.*Gaudi::Units::mm; rmin[3] =  736.5*Gaudi::Units::mm; rmax[3] = 2016.*Gaudi::Units::mm;
	} else {
		throw std::runtime_error("EMECSupportConstruction: wrong Pcone id");
	}
	std::ostringstream tmp;
	tmp << "Cannot get " << id << " polycone fom DB";
	printWarning(tmp);
	} // zplane.empty()?

	GeoPcon* shape = new GeoPcon(phi_start, phi_size);
	for(size_t i = 0; i < zplane.size(); ++ i){
		shape->addPlane(zplane[i], rmin[i], rmax[i]);
	}

	return shape;
}

GeoPhysVol* EMECSupportConstruction::front_envelope(void) const
{
	std::string id = "FrontSupportMother";
	std::string name = m_BaseName + id;
	GeoPcon *motherShape = getPcon(id);
	GeoLogVol *motherLogical = new GeoLogVol(name, motherShape, m_LAr);
	GeoPhysVol *motherPhysical= new GeoPhysVol(motherLogical);

        put_front_outer_extracyl(motherPhysical); // put lead plate after PS
	put_front_outer_ring(motherPhysical);
	put_front_outer_longbar(motherPhysical);
	put_front_indexing_rings(motherPhysical);
	put_front_middle_ring(motherPhysical);
	put_front_inner_ring(motherPhysical);
	put_front_inner_longbar(motherPhysical);
	put_front_outer_barettes(motherPhysical);
	put_front_inner_barettes(motherPhysical);
	put_front_outer_electronics(motherPhysical);

	return motherPhysical;
}

GeoPhysVol* EMECSupportConstruction::back_envelope(void) const
{
	std::string id = "BackSupportMother";
	std::string name = m_BaseName + id;
	GeoPcon *motherShape = getPcon(id);
	GeoLogVol *motherLogical = new GeoLogVol(name, motherShape, m_LAr);
	GeoPhysVol *motherPhysical= new GeoPhysVol(motherLogical);

	put_back_indexing_rings(motherPhysical);
	put_back_outer_ring(motherPhysical);
	put_back_middle_ring(motherPhysical);
	put_back_inner_ring(motherPhysical);
	put_back_inner_longbar(motherPhysical);
	put_back_outer_longbar(motherPhysical);
	put_back_inner_barettes(motherPhysical);
	put_back_outer_barettes(motherPhysical);

	return motherPhysical;
}

void EMECSupportConstruction::put_front_outer_barettes(GeoPhysVol *motherPhysical) const
{
	std::string id = "FrontOuterBarrettes";
	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	map_t numbers = getNumbersMap(m_DB_numbers, id);

	std::string name = m_BaseName + id;
	double rminFOB = getNumber(m_DB_tubes, tubes, id, "RMIN", 614.-2.+40.);
	double rmaxFOB = getNumber(m_DB_tubes, tubes, id, "RMAX", 1961.-7.+62.);
	double dzFOB = getNumber(m_DB_tubes, tubes, id, "DZ", 11. / 2);
	double zposFOB = getNumber(m_DB_numbers, numbers, "Z0", "PARVALUE", 50.) + dzFOB;
	GeoTubs *shapeFOB = new GeoTubs(rminFOB, rmaxFOB, dzFOB, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFOB = new GeoLogVol(name, shapeFOB, m_LAr);
	GeoPhysVol *physFOB = new GeoPhysVol(logicalFOB);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zposFOB)));
	motherPhysical->add(physFOB);

	const int number_of_modules = 8;
	const double moduldfi = Gaudi::Units::twopi / number_of_modules;
	const int nofabs = (*m_DB_EmecWheelParameters)[1]->getInt("NABS");
	const int nofdiv = nofabs / number_of_modules;
	const double dfi = Gaudi::Units::twopi / nofabs;
  //define a fi section including one absorber and electrode
	name = m_BaseName + "FrontOuterBarrette::Module::Phidiv";
	GeoTubs *shapeFOBMP = new GeoTubs(rminFOB, rmaxFOB, dzFOB, -dfi/4., dfi);
	GeoLogVol *logicalFOBMP = new GeoLogVol(name, shapeFOBMP, m_LAr);
	GeoPhysVol *physFOBMP = new GeoPhysVol(logicalFOBMP);

  //longitudinal bar - absorber connection
	name = m_BaseName + "FrontOuterBarrette::Abs";
	double rmn = getNumber(m_DB_numbers, numbers, "R0", "PARVALUE", 613.38);         // start of abs.
	double dr = getNumber(m_DB_numbers, numbers, "DRabs", "PARVALUE", 41.4);         // start of barrette rel to start of abs.
	double dx = getNumber(m_DB_numbers, numbers, "Labs", "PARVALUE", 1290.) / 2.;         // length of the connected part
	assert(rmn + dr > rminFOB && rmn + dr + dx*2 < rmaxFOB);
	double dy = LArWheelCalculator::GetFanHalfThickness(LArG4::OuterAbsorberWheel);
	const double r0A = rmn + dr + dx;
	GeoBox *shapeFOBA = new GeoBox(dx, dy, dzFOB);
	GeoLogVol *logicalFOBA = new GeoLogVol(name, shapeFOBA, m_G10FeOuter);
	GeoPhysVol *physFOBA = new GeoPhysVol(logicalFOBA);
	physFOBMP->add(new GeoTransform(GeoTrf::TranslateX3D(r0A)));
	physFOBMP->add(physFOBA);

	name = m_BaseName + "FrontOuterBarrette::Ele";   // piece of electrode
	dr = getNumber(m_DB_numbers, numbers, "DRele", "PARVALUE", 48.4);
	dx = getNumber(m_DB_numbers, numbers, "Lele", "PARVALUE", 1283.8) / 2.;
	assert(rmn + dr > rminFOB && rmn + dr + dx*2 < rmaxFOB);
	dy = LArWheelCalculator::GetFanHalfThickness(LArG4::OuterElectrodWheel);
	const double r0E = rmn + dr + dx;
	double x0 = r0E * cos(dfi/2.);
	double y0 = r0E * sin(dfi/2.);
	GeoBox *shapeFOBE = new GeoBox(dx, dy, dzFOB);
	GeoLogVol *logicalFOBE = new GeoLogVol(name, shapeFOBE, m_Kapton_Cu);
	GeoPhysVol *physFOBE = new GeoPhysVol(logicalFOBE);
	physFOBMP->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(x0,y0,0.)*GeoTrf::RotateZ3D(dfi/2.))));
	physFOBMP->add(physFOBE);

	if(m_isModule){
		name = m_BaseName + "FrontOuterBarrette::Module::Phidiv";
		for(int i = 0; i < nofdiv - 1; ++ i){
			double fi = m_PhiStart + dfi/2. + i * dfi;
			physFOB->add(new GeoIdentifierTag(i));
			physFOB->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physFOB->add(physFOBMP);
		}
		name = m_BaseName + "FrontOuterBarrette::Abs";
		double fi = m_PhiStart + dfi/2.+ (nofdiv-1) * dfi;
		x0 = r0A*cos(fi);
		y0 = r0A*sin(fi);
		physFOB->add(new GeoIdentifierTag(nofdiv-1));
		physFOB->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(x0,y0,0.)*GeoTrf::RotateZ3D(fi))));
		physFOB->add(physFOBA);
	} else {
  // in case one wants to build the whole wheel:
  // define a (virtual)module
		name = m_BaseName + "FrontOuterBarrette::Module";
		GeoTubs *shapeFOBM = new GeoTubs(rminFOB, rmaxFOB, dzFOB, -dfi/4., moduldfi);
		GeoLogVol *logicalFOBM = new GeoLogVol(name, shapeFOBM, m_LAr);
		GeoPhysVol *physFOBM = new GeoPhysVol(logicalFOBM);
      //position the fi divisions into module
		name = m_BaseName + "FrontOuterBarrette::Module::Phidiv";
		for(int i = 0; i < nofdiv; ++ i){
			double fi = i * dfi;
			physFOBM->add(new GeoIdentifierTag(i));
			physFOBM->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physFOBM->add(physFOBMP);
		}
      //position modules into Barrette mother to create the full wheel
		name = m_BaseName + "FrontOuterBarrette::Module";
		for(int i = 0; i < number_of_modules; ++ i){
			double fi = dfi/2.+ i * moduldfi;
			physFOB->add(new GeoIdentifierTag(i));
			physFOB->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physFOB->add(physFOBM);
		}
	}
}

void EMECSupportConstruction::put_front_inner_barettes(GeoPhysVol *motherPhysical) const
{
	std::string id = "FrontInnerBarrettes";
	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	map_t numbers = getNumbersMap(m_DB_numbers, id);
	std::string name = m_BaseName + id;

	double rminFIB = getNumber(m_DB_tubes, tubes, id, "RMIN", 335.5-1.+40.5);  //RInner + 40.5// -1mm for rcold
	double rmaxFIB = getNumber(m_DB_tubes, tubes, id, "RMAX", 614.-2.-40.);    //RMiddle-40.mm // -2mm for cold
	double dzFIB = getNumber(m_DB_tubes, tubes, id, "DZ", 11. / 2);
	double zposFIB = getNumber(m_DB_numbers, numbers, "Z0", "PARVALUE", 50.) + dzFIB;

	GeoTubs *shapeFIB = new GeoTubs(rminFIB, rmaxFIB, dzFIB, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFIB = new GeoLogVol(name, shapeFIB, m_LAr);
	GeoPhysVol *physFIB = new GeoPhysVol(logicalFIB);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zposFIB)));
	motherPhysical->add(physFIB);

	const int number_of_modules = 8;
	const double moduldfi = Gaudi::Units::twopi / number_of_modules;
	const int nofabs = (*m_DB_EmecWheelParameters)[0]->getInt("NABS");
	const int nofdiv = nofabs / number_of_modules;
	const double dfi = Gaudi::Units::twopi / nofabs;

	name = m_BaseName + "FrontInnerBarrette::Module::Phidiv";
	GeoTubs *shapeFIBMP = new GeoTubs(rminFIB,rmaxFIB,dzFIB, -dfi/4., dfi);
	GeoLogVol *logicalFIBMP = new GeoLogVol(name, shapeFIBMP, m_LAr);
	GeoPhysVol *physFIBMP = new GeoPhysVol(logicalFIBMP);

	name = m_BaseName + "FrontInnerBarrette::Abs";
	double rmn = getNumber(m_DB_numbers, numbers, "R0", "PARVALUE", 302.31);     // start of abs.
	double dr = getNumber(m_DB_numbers, numbers, "DRabs", "PARVALUE", 75.6);      // start of barrette rel to start of abs.
	double dx = getNumber(m_DB_numbers, numbers, "Labs", "PARVALUE", 192.) / 2.;       // length of the connected part
	assert(rmn + dr > rminFIB && rmn + dr + dx*2 < rmaxFIB);
	double dy = LArWheelCalculator::GetFanHalfThickness(LArG4::InnerAbsorberWheel);
	const double r0A = rmn + dr + dx;
	GeoBox *shapeFIBA = new GeoBox(dx, dy, dzFIB);
	GeoLogVol *logicalFIBA = new GeoLogVol(name, shapeFIBA, m_G10FeInner);
	GeoPhysVol *physFIBA = new GeoPhysVol(logicalFIBA);
	physFIBMP->add(new GeoTransform(GeoTrf::TranslateX3D(r0A)));
	physFIBMP->add(physFIBA);

	name = m_BaseName + "FrontInnerBarrette::Ele";   // piece of electrode
	dr = getNumber(m_DB_numbers, numbers, "DRele", "PARVALUE", 106.3);
	dx = getNumber(m_DB_numbers, numbers, "Lele", "PARVALUE", 144.4) / 2.;
	assert(rmn + dr > rminFIB && rmn + dr + dx*2 < rmaxFIB);
	dy = LArWheelCalculator::GetFanHalfThickness(LArG4::InnerElectrodWheel);
	const double r0E = rmn + dr + dx;
	double x0 = r0E * cos(dfi/2.);
	double y0 = r0E * sin(dfi/2.);
	GeoBox *shapeFIBE = new GeoBox(dx, dy, dzFIB);
	GeoLogVol *logicalFIBE = new GeoLogVol(name, shapeFIBE, m_Kapton_Cu);
	GeoPhysVol *physFIBE = new GeoPhysVol(logicalFIBE);
	physFIBMP->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(x0,y0,0.)*GeoTrf::RotateZ3D(dfi/2.))));
	physFIBMP->add(physFIBE);

	if(m_isModule){
		name = m_BaseName + "FrontInnerBarrette::Phidiv";
		for(int i = 0; i < nofdiv - 1; ++ i){
			double fi = m_PhiStart + dfi/2. + i * dfi;
			physFIB->add(new GeoIdentifierTag(i));
			physFIB->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physFIB->add(physFIBMP);
		}
		name = m_BaseName + "FrontInnerBarrette::Abs";
		double fi = m_PhiStart + dfi/2.+ (nofdiv-1) * dfi;
		x0 = r0A * cos(fi);
		y0 = r0A * sin(fi);
		physFIB->add(new GeoIdentifierTag(nofdiv-1));
		physFIB->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(x0,y0,0.)*GeoTrf::RotateZ3D(fi))));
		physFIB->add(physFIBA);
	} else {
  // in case one wants to build the whole wheel:
  // define a (virtual)module
		name = m_BaseName + "FrontInnerBarrette::Module";
		GeoTubs *shapeFIBM = new GeoTubs(rminFIB, rmaxFIB, dzFIB, -dfi/4., moduldfi);
		GeoLogVol *logicalFIBM = new GeoLogVol(name, shapeFIBM, m_LAr);
		GeoPhysVol *physFIBM = new GeoPhysVol(logicalFIBM);
		name = m_BaseName + "FrontInnerBarrette::Module::Phidiv";
		for(int i = 0; i < nofdiv; ++ i){
			double fi = i * dfi;
			physFIBM->add(new GeoIdentifierTag(i));
			physFIBM->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physFIBM->add(physFIBMP);
		}
     //position modules into Barrette mother to create the full wheel
		name = m_BaseName + "FrontInnerBarrette::Module";
		for(int i = 0; i < number_of_modules; ++ i){
			double fi = dfi/2.+ i * moduldfi;
			physFIB->add(new GeoIdentifierTag(i));
			physFIB->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physFIB->add(physFIBM);
		}
	}
}

void EMECSupportConstruction::put_back_outer_barettes(GeoPhysVol *motherPhysical) const
{
	std::string id = "BackOuterBarrettes";

	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	map_t numbers = getNumbersMap(m_DB_numbers, id);

	std::string name = m_BaseName + id;

	double rminBOB = getNumber(m_DB_tubes, tubes, id, "RMIN", 699.-2.5+40.);    //RMiddle+40. // -2.5 for cold
	double rmaxBOB = getNumber(m_DB_tubes, tubes, id, "RMAX", 1961.-7.+62.);    //ROuter+62. // -7 for cold
	double dzBOB = getNumber(m_DB_tubes, tubes, id, "DZ", 11. / 2);
	double zposBOB = getNumber(m_DB_numbers, numbers, "Z0", "PARVALUE", 44.) + dzBOB;
	GeoTubs *shapeBOB = new GeoTubs(rminBOB, rmaxBOB, dzBOB, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalBOB = new GeoLogVol(name, shapeBOB, m_LAr);
	GeoPhysVol *physBOB = new GeoPhysVol(logicalBOB);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zposBOB)));
	motherPhysical->add(physBOB);

	const int number_of_modules = 8;
	const double moduldfi = Gaudi::Units::twopi / number_of_modules;
	int nofabs = (*m_DB_EmecWheelParameters)[1]->getInt("NABS");
	int nofdiv = nofabs / number_of_modules;
	double dfi = Gaudi::Units::twopi / nofabs;

	name = m_BaseName + "BackOuterBarrette::Module::Phidiv";
	GeoTubs *shapeBOBMP = new GeoTubs(rminBOB, rmaxBOB, dzBOB, -dfi/4., dfi);
	GeoLogVol *logicalBOBMP = new GeoLogVol(name, shapeBOBMP, m_LAr);
	GeoPhysVol *physBOBMP = new GeoPhysVol(logicalBOBMP);

	name = m_BaseName + "BackOuterBarrette::Abs";  //longitudinal bar - absorber connection
	double rmn = getNumber(m_DB_numbers, numbers, "R0", "PARVALUE", 698.4);        // start of abs.
	double dr = getNumber(m_DB_numbers, numbers, "DRabs", "PARVALUE", 42.1);        // start of barrette rel to start of abs
	double dx = getNumber(m_DB_numbers, numbers, "Labs", "PARVALUE", 1229.) / 2.;        // length of the connected part
	assert(rmn + dr > rminBOB && rmn + dr + dx * 2 < rmaxBOB);
	double dy = LArWheelCalculator::GetFanHalfThickness(LArG4::OuterAbsorberWheel);
	const double r0A = rmn + dr + dx;
	GeoBox *shapeBOBA = new GeoBox(dx, dy, dzBOB);
	GeoLogVol *logicalBOBA = new GeoLogVol(name, shapeBOBA, m_G10FeOuter);
	GeoPhysVol *physBOBA = new GeoPhysVol(logicalBOBA);
	physBOBMP->add(new GeoTransform(GeoTrf::TranslateX3D(r0A)));
	physBOBMP->add(physBOBA);

	name = m_BaseName + "BackOuterBarrette::Ele";   // piece of electrode
	dr = getNumber(m_DB_numbers, numbers, "DRele", "PARVALUE", 41.);
	dx = getNumber(m_DB_numbers, numbers, "Lele", "PARVALUE", 1246.9) / 2.;
	assert(rmn + dr > rminBOB && rmn + dr + dx*2 < rmaxBOB);
	dy = LArWheelCalculator::GetFanHalfThickness(LArG4::OuterElectrodWheel);
	double r0E = rmn + dr + dx;
	double y0 = r0E * sin(dfi/2.);
	double x0 = r0E * cos(dfi/2.);
	GeoBox *shapeBOBE = new GeoBox(dx, dy, dzBOB);
	GeoLogVol *logicalBOBE = new GeoLogVol(name, shapeBOBE, m_Kapton_Cu);
	GeoPhysVol *physBOBE = new GeoPhysVol(logicalBOBE);
	physBOBMP->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(x0,y0,0.)*GeoTrf::RotateZ3D(dfi/2.))));
	physBOBMP->add(physBOBE);

	if(m_isModule){
  // Put phi divisions directly to Barrette Mother
		name = m_BaseName + "BackOuterBarrette::Module::Phidiv";
		for(int i = 0; i < nofdiv - 1; ++ i){
			double fi = m_PhiStart + dfi/2. + i * dfi;
			physBOB->add(new GeoIdentifierTag(i));
			physBOB->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physBOB->add(physBOBMP);
		}
		name = m_BaseName + "BackOuterBarrette::Abs";
		double fi = m_PhiStart + dfi/2.+ (nofdiv - 1) * dfi;
		x0 = r0A * cos(fi);
		y0 = r0A * sin(fi);
		physBOB->add(new GeoIdentifierTag(nofdiv - 1));
		physBOB->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(x0,y0,0.)*GeoTrf::RotateZ3D(fi))));
		physBOB->add(physBOBA);
   } else {
  // in case one wants to build the whole wheel:
  // define a (virtual)module
		name = m_BaseName + "BackOuterBarrette::Module";
		GeoTubs *shapeBOBM = new GeoTubs(rminBOB, rmaxBOB, dzBOB, -dfi/4.,moduldfi);
		GeoLogVol *logicalBOBM = new GeoLogVol(name, shapeBOBM, m_LAr);
		GeoPhysVol *physBOBM = new GeoPhysVol(logicalBOBM);
     //position the fi divisions into module
		name = m_BaseName + "BackOuterBarrette::Module::Phidiv";
		for(int i = 0; i < nofdiv; ++ i){
			double fi = dfi * i;
			physBOBM->add(new GeoIdentifierTag(i));
			physBOBM->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physBOBM->add(physBOBMP);
		}
     //position modules into Barrette mother to create the full wheel
		name = m_BaseName + "BackOuterBarrette::Module";
		for(int i = 0; i < number_of_modules; ++ i){
			double fi = dfi/2.+ i * moduldfi;
			physBOB->add(new GeoIdentifierTag(i));
			physBOB->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physBOB->add(physBOBM);
		}
	}
}

void EMECSupportConstruction::put_back_inner_barettes(GeoPhysVol *motherPhysical) const
{
	std::string id = "BackInnerBarrettes";

	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	map_t numbers = getNumbersMap(m_DB_numbers, id);

	std::string name = m_BaseName + id;
	double rminBIB = getNumber(m_DB_tubes, tubes, id, "RMIN", 357.5-1.+40.5);    //RInner +40.5// -1.Gaudi::Units::mm for cold
	double rmaxBIB = getNumber(m_DB_tubes, tubes, id, "RMAX", 699.-2.5-40.);     //RMiddle-40   //-2.5mm for cold
	double dzBIB = getNumber(m_DB_tubes, tubes, id, "DZ", 11. / 2);
	double zposBIB = getNumber(m_DB_numbers, numbers, "Z0", "PARVALUE", 44.) + dzBIB;
	GeoTubs *shapeBIB = new GeoTubs(rminBIB, rmaxBIB, dzBIB, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalBIB = new GeoLogVol(name, shapeBIB, m_LAr);
	GeoPhysVol *physBIB = new GeoPhysVol(logicalBIB);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(zposBIB)));
	motherPhysical->add(physBIB);

	const int number_of_modules = 8;
	const double moduldfi = Gaudi::Units::twopi / number_of_modules;
	const int nofabs = (*m_DB_EmecWheelParameters)[0]->getInt("NABS");
	const int nofdiv = nofabs / number_of_modules;
	const double dfi = Gaudi::Units::twopi / nofabs;

	name = m_BaseName + "BackInnerBarrette::Module::Phidiv";
	GeoTubs *shapeBIBMP = new GeoTubs(rminBIB, rmaxBIB, dzBIB, -dfi/4., dfi);
	GeoLogVol *logicalBIBMP = new GeoLogVol(name, shapeBIBMP, m_LAr);
	GeoPhysVol *physBIBMP = new GeoPhysVol(logicalBIBMP);

  //longitudinal bar - absorber connection
	id = "BackInnerBarrette::Abs";
	name = m_BaseName + id;
	double rmn = getNumber(m_DB_numbers, numbers, "R0", "PARVALUE", 344.28); // start of abs.
	double dr = getNumber(m_DB_numbers, numbers, "DRabs", "PARVALUE", 56.1); // start of barrette rel to start of abs.
	double dx = getNumber(m_DB_numbers, numbers, "Labs", "PARVALUE", 255.) / 2.;
	double dy = LArWheelCalculator::GetFanHalfThickness(LArG4::InnerAbsorberWheel);
	assert(rmn+dr>rminBIB && rmn+dr+dx*2.<rmaxBIB);
	const double r0A = rmn + dr + dx;
	GeoBox *shapeBIBA = new GeoBox(dx, dy, dzBIB);
	GeoLogVol *logicalBIBA = new GeoLogVol(name, shapeBIBA, m_G10FeInner);
	GeoPhysVol *physBIBA = new GeoPhysVol(logicalBIBA);
	physBIBMP->add(new GeoTransform(GeoTrf::TranslateX3D(r0A)));
	physBIBMP->add(physBIBA);

	id = "BackInnerBarrette::Ele";   // piece of electrode
	name = m_BaseName + id;
	dr = getNumber(m_DB_numbers, numbers, "DRele", "PARVALUE", 76.6);
	dx = getNumber(m_DB_numbers, numbers, "Lele", "PARVALUE", 208.9) / 2.;
	assert(rmn + dr > rminBIB && rmn + dr + dx * 2. < rmaxBIB);
	dy = LArWheelCalculator::GetFanHalfThickness(LArG4::InnerElectrodWheel);
	const double r0E = rmn + dr + dx;
	double y0 = r0E * sin(dfi * 0.5);
	double x0 = r0E * cos(dfi * 0.5);
	GeoBox *shapeBIBE = new GeoBox(dx, dy, dzBIB);
	GeoLogVol *logicalBIBE = new GeoLogVol(name, shapeBIBE, m_Kapton_Cu);
	GeoPhysVol *physBIBE = new GeoPhysVol(logicalBIBE);
	physBIBMP->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(x0, y0, 0.)*GeoTrf::RotateZ3D(dfi*0.5))));
	physBIBMP->add(physBIBE);

	if(m_isModule){
/*
  Put phi divisions directly to Barrette Mother
  which goes from Phistart to m_PhiStart+m_PhiSize.
  This  is the barrette volume of the Module itself in fact.
  Positioning will be done such a way, that in case of
  m_Position=0, there should be an electrode at phi=0.
  Only nofdiv-1 section can be positioned,
  otherwise a phi section will leak out of the phi boundary
  of the Module.
  The abs. and electr. pieces at the phi limits of the Module
  could be positioned individually(not done);
*/
		name = m_BaseName + "BackInnerBarrette::Module::Phidiv";
		for(int i = 0; i < nofdiv - 1; ++ i){
			double fi = m_PhiStart + dfi/2. + i * dfi;
			physBIB->add(new GeoIdentifierTag(i));
			physBIB->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physBIB->add(physBIBMP);
		}
		name = m_BaseName + "BackInnerBarrette::Abs";
		double fi = m_PhiStart + dfi/2.+ (nofdiv - 1) * dfi;
		x0 = r0A*cos(fi);
		y0 = r0A*sin(fi);
		physBIB->add(new GeoIdentifierTag(nofdiv - 1));
		physBIB->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(x0, y0, 0.)*GeoTrf::RotateZ3D(fi))));
		physBIB->add(physBIBA);
	} else {
  // in case one wants to build the whole wheel:
  // define a (virtual)module
		name = m_BaseName + "BackInnerBarrette::Module";
		GeoTubs *shapeBIBM = new GeoTubs(rminBIB, rmaxBIB, dzBIB, -dfi/4., moduldfi);
		GeoLogVol *logicalBIBM = new GeoLogVol(name, shapeBIBM, m_LAr);
		GeoPhysVol *physBIBM = new GeoPhysVol(logicalBIBM);
     //position the fi divisions into module
		name = m_BaseName + "BackInnerBarrette::Module::Phidiv";
		for(int i = 0; i < nofdiv; ++ i){
			double fi = dfi * i;
			physBIBM->add(new GeoIdentifierTag(i));
			physBIBM->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
			physBIBM->add(physBIBMP);
		}
     //position modules into Barrette mother to create the full wheel
		name = m_BaseName + "BackInnerBarrette::Module";
		for(int i = 0; i < number_of_modules; ++ i){
			double fi = dfi*0.5 + i * moduldfi;
			physBIB->add(new GeoIdentifierTag(i));
			physBIB->add(new GeoTransform(GeoTrf::RotateZ3D(fi)));
		physBIB->add(physBIBM);
		}
	}
}

GeoPhysVol* EMECSupportConstruction::outer_envelope(void) const
{
	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	std::string id = "OuterTransversalBars";
	std::string name = m_BaseName + id;
	double rminOTB = getNumber(m_DB_tubes, tubes, id, "RMIN", (2034. + 2.)*Gaudi::Units::mm);
	double rmaxOTB = getNumber(m_DB_tubes, tubes, id, "RMAX", rminOTB + 3.*Gaudi::Units::mm);
	double dzOTB = getNumber(m_DB_tubes, tubes, id, "DZ", 201.*Gaudi::Units::mm);
	GeoTubs* shapeOTB = new GeoTubs(rminOTB, rmaxOTB, dzOTB, m_PhiStart, m_PhiSize);
	GeoLogVol* logicalOTB = new GeoLogVol(name, shapeOTB, m_Gten);
	GeoPhysVol* physOTB = new GeoPhysVol(logicalOTB);

	id = "TopIndexingRing";
	name = m_BaseName + id;
	double rminTIR = getNumber(m_DB_tubes, tubes, id, "RMIN", rmaxOTB);
	double rmaxTIR = getNumber(m_DB_tubes, tubes, id, "RMAX", rminTIR + 9.*Gaudi::Units::mm);
	double dzTIR = getNumber(m_DB_tubes, tubes, id, "DZ", 10.*Gaudi::Units::mm);
	GeoTubs* shapeTIR = new GeoTubs(rminTIR, rmaxTIR, dzTIR, m_PhiStart, m_PhiSize);
	GeoLogVol* logicalTIR = new GeoLogVol(name, shapeTIR, m_Alu);
	GeoPhysVol* physTIR = new GeoPhysVol(logicalTIR);
	id += "::Hole";
	name = m_BaseName + id;
	double dzTIRH = getNumber(m_DB_tubes, tubes, id, "DZ", 4.5*Gaudi::Units::mm);
	double rmaxTIRH = getNumber(m_DB_tubes, tubes, id, "RMAX", rmaxTIR);
	double rminTIRH = getNumber(m_DB_tubes, tubes, id, "RMIN", rmaxTIRH - 2.*Gaudi::Units::mm);
	GeoTubs* shapeTIRH = new GeoTubs(rminTIRH, rmaxTIRH, dzTIRH, m_PhiStart, m_PhiSize);
	GeoLogVol* logicalTIRH = new GeoLogVol(name, shapeTIRH, m_LAr);
	GeoPhysVol* physTIRH = new GeoPhysVol(logicalTIRH);
	physTIR->add(physTIRH);

	id = "WideStretchers";
	name = m_BaseName + id;
	GeoPcon* shapeWS = getPcon(id);
	GeoLogVol* logicalWS = new GeoLogVol(name, shapeWS, m_Alu);
	GeoPhysVol* physWS = new GeoPhysVol(logicalWS);

	id = "NarrowStretchers";
	name = m_BaseName + id;
	GeoPcon* shapeNS = getPcon(id);
	GeoLogVol* logicalNS = new GeoLogVol(name, shapeNS, m_Alu);
	GeoPhysVol* physNS = new GeoPhysVol(logicalNS);

	id = "OuterSupportMother";
	name = m_BaseName + id;
	GeoPcon *motherShape = getPcon(id);
	GeoLogVol *motherLogical = new GeoLogVol(name, motherShape, m_LAr);
	GeoPhysVol *motherPhysical= new GeoPhysVol(motherLogical);

	motherPhysical->add(physTIR);
	motherPhysical->add(physOTB);

	const int number_of_stretchers = 8; // for full wheel
	if(m_isModule){
		motherPhysical->add(new GeoIdentifierTag(0));
		motherPhysical->add(physNS);
  // place two narrow stretchers on edges instead of one wide
		double dfi = M_PI / number_of_stretchers;
		double dfiNS = shapeNS->getDPhi();
		motherPhysical->add(new GeoIdentifierTag(1));
		motherPhysical->add(new GeoTransform(GeoTrf::RotateZ3D(dfi - dfiNS*0.5)));
		motherPhysical->add(physNS);
		motherPhysical->add(new GeoIdentifierTag(2));
		motherPhysical->add(new GeoTransform(GeoTrf::RotateZ3D(-dfi + dfiNS*0.5)));
		motherPhysical->add(physNS);
	} else {
		double dfi = Gaudi::Units::twopi / number_of_stretchers;
		int copyno = 0;
		for(int i = 0; i < number_of_stretchers; ++ i, ++ copyno){
			double fiW = i * dfi;
			motherPhysical->add(new GeoIdentifierTag(copyno));
			motherPhysical->add(new GeoTransform(GeoTrf::RotateZ3D(fiW)));
			motherPhysical->add(physWS);
			double fiN = (i + 0.5) * dfi;
			motherPhysical->add(new GeoIdentifierTag(copyno));
			motherPhysical->add(new GeoTransform(GeoTrf::RotateZ3D(fiN)));
			motherPhysical->add(physNS);
		}
	}

	return motherPhysical;
}

GeoPhysVol* EMECSupportConstruction::inner_envelope(void) const
{
	std::string id = "InnerAluCone";
	map_t numbers = getNumbersMap(m_DB_numbers, id);
	std::string name0 = m_BaseName + id;

//	double dz = LArWheelCalculator::GetWheelThickness() * 0.5; //257.*Gaudi::Units::mm;     //zWheelThickness/2.
	double dz = 0.5 * (*m_DB_mn)[0]->getDouble("ACTIVELENGTH")*Gaudi::Units::mm;
	try {
		dz += (*m_DB_mn)[0]->getDouble("STRAIGHTSTARTSECTION")*Gaudi::Units::mm;
	}
	catch(...){
		dz += 2.*Gaudi::Units::mm;
		std::ostringstream tmp("cannot get STRAIGHTSTARTSECTION from DB");
		printWarning(tmp);
	}

	double r1min = getNumber(m_DB_numbers, numbers, "R1MIN", "PARVALUE", (292.-1.)*Gaudi::Units::mm); //lower radius of front inner ring, -1mm for cold
	double r2min = getNumber(m_DB_numbers, numbers, "R2MIN", "PARVALUE", (333.-1.)*Gaudi::Units::mm); //lower radius of back  inner ring, -1mm for cold
                                  //RInnerFront-43.5;RInnerBack-24.5
	const double talpha = (r2min - r1min)*0.5/dz;
	const double calpha = 2.*dz/sqrt(pow(2.*dz,2.)+pow(r2min-r1min,2.));
        const double inv_calpha = 1. / calpha;
	const double alpha = atan(talpha);
	double surfthick = getNumber(m_DB_numbers, numbers, "surfthick", "PARVALUE", 1.*Gaudi::Units::mm);       // thickness of the cone shell
	double barthick = getNumber(m_DB_numbers, numbers, "barthick", "PARVALUE", 5.*Gaudi::Units::mm);       // thickness of the Alu bars
	double r1max     = pow(barthick/2.,2.)+ pow(r1min+(surfthick+barthick)*inv_calpha,2.);
			r1max     = sqrt(r1max)+surfthick*inv_calpha;
	double r2max     = r2min+(r1max-r1min);

	GeoCons*    shapeIAC    = new GeoCons ( r1min ,r2min,
                                          r1max, r2max,
                                          dz, m_PhiStart, m_PhiSize);

	GeoLogVol* logicalIAC = new GeoLogVol (name0, shapeIAC, m_LAr);
	GeoPhysVol* physIAC = new GeoPhysVol(logicalIAC);

	if(m_isModule) return physIAC; // keep simplified shape

  // otherwise get the details: (9 Alu bars/module, between 2 shells)

	std::string name = name0 + "::InnerShell";
	GeoCons*  shapeIACIS  = new GeoCons(
                               r1min ,                 r2min,
                               r1min+surfthick*inv_calpha, r2min+surfthick*inv_calpha,
                               dz, m_PhiStart, m_PhiSize);
  GeoLogVol* logicalIACIS = new GeoLogVol (name,shapeIACIS,m_Alu);
  GeoPhysVol*   physIACIS = new GeoPhysVol(logicalIACIS);
  physIAC->add(physIACIS);

  name = name0 + "::OuterShell";
//-----------------------------/

  GeoCons*     shapeIACOS  = new GeoCons (
                              r1max-surfthick*inv_calpha,  r2max-surfthick*inv_calpha,
                              r1max,                   r2max,
                              dz, m_PhiStart, m_PhiSize);
  GeoLogVol* logicalIACOS = new GeoLogVol (name,shapeIACOS,m_Alu);
  GeoPhysVol*   physIACOS = new GeoPhysVol(logicalIACOS);
  physIAC->add(physIACOS);

  name = name0 + "::Phidiv";
//-------------------------/

	const int    nofmodul  =   8;
	const double moduldphi = Gaudi::Units::twopi / nofmodul;
  GeoCons*     shapeIACP  = new GeoCons(
       	                      r1min+surfthick*inv_calpha,r2min+surfthick*inv_calpha,
                              r1max-surfthick*inv_calpha,r2max-surfthick*inv_calpha,
                              dz, -moduldphi/2.,moduldphi);
  GeoLogVol* logicalIACP = new GeoLogVol (name,shapeIACP,m_LAr);
  GeoPhysVol*   physIACP = new GeoPhysVol(logicalIACP);

  name = name0 + "::AluBar";
//-------------------------/

  GeoPara* shapeIACAB    = new GeoPara(
                                  barthick/2.*inv_calpha,barthick/2.,dz,
                                  0.,alpha,0.);
  GeoLogVol* logicalIACAB= new GeoLogVol (name,shapeIACAB, m_Alu);
  GeoPhysVol*   physIACAB= new GeoPhysVol(logicalIACAB);

  const double dphi = Gaudi::Units::twopi / 256.;
  const int    nbar = 9;
  const double phi[9]={-15.,-11.,-7.5,-4.,0.,4.,7.5,11.,15.}; // phipos of the bars
  const double r0=r1min+(surfthick+barthick/2.)*inv_calpha+dz*talpha;

	for(int i = 0; i < nbar; ++ i){  // put the Alu bars into the module
		double fi=phi[i]*dphi;
		double cfi=cos(fi);
		double sfi=sin(fi);
		physIACP->add(new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(r0*cfi,r0*sfi,0.)*GeoTrf::RotateZ3D(fi))));
		physIACP->add(physIACAB);
	}

  name = name0 + "::Phidiv";
  for(int i=0;i<nofmodul;i++){ // put modules into wheel
    double fi=(i+0.5)*moduldphi;
    physIAC->add( new GeoIdentifierTag(i));
    physIAC->add( new GeoTransform(GeoTrf::RotateZ3D(fi)));
    physIAC->add(physIACP);
  }

  return physIAC;
}

//!!!!
GeoPhysVol* EMECSupportConstruction::middle_envelope(void) const
{
	double dMechFocaltoWRP = (*m_DB_EmecGeometry)[0]->getDouble("Z1") *Gaudi::Units::cm;
	double LArEMECHalfCrack = (*m_DB_EmecGeometry)[0]->getDouble("DCRACK") *Gaudi::Units::cm;
	double LArTotalThickness = (*m_DB_EmecGeometry)[0]->getDouble("ETOT") *Gaudi::Units::cm;

	double eta_mid = (*m_DB_EmecWheelParameters)[0]->getDouble("ETAEXT");

	double tanThetaMid = 2. * exp(-eta_mid) / (1. - exp(-2.*eta_mid));
	const double cosThetaMid = (1. - exp(2.*-eta_mid)) / (1. + exp(-2.*eta_mid));
        const double inv_cosThetaMid = 1. / cosThetaMid;

	double z0 = LArTotalThickness * 0.5 + dMechFocaltoWRP;
	double length = 462.*Gaudi::Units::mm;
	double rthickness = 1.5*Gaudi::Units::mm * inv_cosThetaMid;

	std::string name = m_BaseName + "InnerTransversalBars";
	double dz = length * cosThetaMid * 0.5;
	double rmin0 = (z0 - dz) * tanThetaMid - LArEMECHalfCrack + inv_cosThetaMid;
	double rmin1 = (z0 + dz) * tanThetaMid - LArEMECHalfCrack + inv_cosThetaMid;

	GeoCons* shapeITB = new GeoCons(rmin0, rmin1, rmin0 + rthickness, rmin1 + rthickness,
				                    dz, m_PhiStart, m_PhiSize);

	GeoLogVol* logicalITB  = new GeoLogVol (name,shapeITB,m_Gten);
	GeoPhysVol* physITB = new GeoPhysVol(logicalITB);

	return physITB;
}

void EMECSupportConstruction::put_front_middle_ring(GeoPhysVol *motherPhysical) const
{

	std::string id = "FrontMiddleRing";
	std::string name = m_BaseName + id;

	double z0 = getNumber(m_DB_numbers, id, "Z0", 2.);
	GeoPcon *shapeFMR = getPcon(id);
	GeoLogVol *logicalFMR = new GeoLogVol(name, shapeFMR, m_PermaliE730);
	GeoPhysVol *physFMR = new GeoPhysVol(logicalFMR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physFMR);

	id = "FrontMiddleRing::LowerHole";
	name = m_BaseName + id;
	GeoPcon *shapeFMRLH = getPcon(id);
	GeoLogVol *logicalFMRLH = new GeoLogVol(name, shapeFMRLH, m_LAr);
	GeoPhysVol *physFMRLH = new GeoPhysVol(logicalFMRLH);
	physFMR->add(physFMRLH);

  // endpiece of the inner longitudinal bar  embedded into middle ring
	id = "FrontMiddleRing::LowerGTen";
	name = m_BaseName + id;
	GeoPcon *shapeFMRLG = getPcon(id);
	GeoLogVol *logicalFMRLG = new GeoLogVol(name, shapeFMRLG, m_Gten);
	GeoPhysVol *physFMRLG = new GeoPhysVol(logicalFMRLG);
	physFMRLH->add(physFMRLG);

	id = "FrontMiddleRing::UpperHole";
	name = m_BaseName + id;
	GeoPcon *shapeFMRUH = getPcon(id);
	GeoLogVol *logicalFMRUH = new GeoLogVol(name, shapeFMRUH, m_LAr);
	GeoPhysVol *physFMRUH = new GeoPhysVol(logicalFMRUH);
	physFMR->add(physFMRUH);

  // endpiece of the outer longitudinal bar embedded into middle ring
	id = "FrontMiddleRing::UpperGTen";
	name = m_BaseName + id;
	GeoPcon *shapeFMRUG = getPcon(id);
	GeoLogVol *logicalFMRUG = new GeoLogVol(name, shapeFMRUG, m_Gten);
	GeoPhysVol *physFMRUG = new GeoPhysVol(logicalFMRUG);
	physFMRUH->add(physFMRUG);
}

void EMECSupportConstruction::put_front_inner_ring(GeoPhysVol *motherPhysical) const
{
	std::string id = "FrontInnerRing";
	double z0 = getNumber(m_DB_numbers, id, "Z0", 2.);    // z pos. of front face of the ring rel. to front of envelope
	std::string name = m_BaseName + id;
	GeoPcon *shapeFIR = getPcon(id);
	GeoLogVol *logicalFIR = new GeoLogVol(name, shapeFIR, m_Alu);
	GeoPhysVol *physFIR = new GeoPhysVol(logicalFIR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physFIR);

	id = "FrontInnerRing::Hole";
	name = m_BaseName + id;
	GeoPcon *shapeFIRH = getPcon(id);
	GeoLogVol *logicalFIRH = new GeoLogVol(name, shapeFIRH, m_LAr);
	GeoPhysVol *physFIRH = new GeoPhysVol(logicalFIRH);
	physFIR->add(physFIRH);

  //endpiece of the inner longitudinal embedded into inner ring
	id = "FrontInnerRing::GTen";
	name = m_BaseName + id;
	GeoPcon *shapeFIRG = getPcon(id);
	GeoLogVol *logicalFIRG = new GeoLogVol(name, shapeFIRG, m_Gten);
	GeoPhysVol *physFIRG = new GeoPhysVol(logicalFIRG);
	physFIRH->add(physFIRG);
}

void EMECSupportConstruction::put_front_inner_longbar(GeoPhysVol *motherPhysical) const
{
	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	std::string id = "FrontInnerLongBar";
	std::string name = m_BaseName + id;
  //  double   rmin=376.;  //RInner +40.5
	double rmin = getNumber(m_DB_tubes, tubes, id, "RMIN", 375.); // Suggested by Jozsef  (vakho)
  //  double   rmax=574.;  //RMiddle-40.
	double rmax = getNumber(m_DB_tubes, tubes, id, "RMAX", 572.);  // To avoid clash with FrontMiddleRing (vakho)
	double dz = getNumber(m_DB_tubes, tubes, id, "DZ", 20./2);
	double z0 = getNumber(m_DB_numbers, id, "Z0", 30.) + dz;
	GeoTubs *shapeFILB = new GeoTubs(rmin, rmax, dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFILB = new GeoLogVol(name,shapeFILB,m_Gten);
	GeoPhysVol *physFILB = new GeoPhysVol(logicalFILB);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physFILB);
}

void EMECSupportConstruction::put_back_middle_ring(GeoPhysVol *motherPhysical) const
{
	std::string id = "BackMiddleRing";
	double z0 =  getNumber(m_DB_numbers, id, "Z0", 2.5);
	std::string name = m_BaseName + id;
	GeoPcon *shapeBMR = getPcon(id);
	GeoLogVol *logicalBMR = new GeoLogVol(name, shapeBMR, m_Alu);
	GeoPhysVol *physBMR = new GeoPhysVol(logicalBMR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physBMR);

	id = "BackMiddleRing::LowerHole";
	name = m_BaseName + id;
	GeoPcon *shapeBMRLH = getPcon(id);
	GeoLogVol *logicalBMRLH = new GeoLogVol(name, shapeBMRLH, m_LAr);
	GeoPhysVol *physBMRLH = new GeoPhysVol(logicalBMRLH);
	physBMR->add( physBMRLH);

  //endpiece of the inner longitudinal
	id = "BackMiddleRing::LowerGTen";
	name = m_BaseName + id;
	GeoPcon *shapeBMRLG = getPcon(id);
	GeoLogVol *logicalBMRLG = new GeoLogVol(name, shapeBMRLG, m_Gten);
	GeoPhysVol *physBMRLG = new GeoPhysVol(logicalBMRLG);
	physBMRLH->add(physBMRLG);

	id = "BackMiddleRing::UpperHole";
	name = m_BaseName + id;
	GeoPcon *shapeBMRUH = getPcon(id);
	GeoLogVol *logicalBMRUH = new GeoLogVol(name, shapeBMRUH, m_LAr);
	GeoPhysVol *physBMRUH = new GeoPhysVol(logicalBMRUH);
	physBMR->add( physBMRUH);

  //endpiece of the outer longitudinal bar embedded into middle ring
	id = "BackMiddleRing::UpperGTen";
	name = m_BaseName + id;
	GeoPcon *shapeBMRUG = getPcon(id);
	GeoLogVol *logicalBMRUG = new GeoLogVol(name, shapeBMRUG, m_Gten);
	GeoPhysVol *physBMRUG = new GeoPhysVol(logicalBMRUG);
	physBMRUH->add(physBMRUG);
}

void EMECSupportConstruction::put_back_inner_ring(GeoPhysVol *motherPhysical) const
{
	std::string id = "BackInnerRing";
	double z0 = getNumber(m_DB_numbers, id, "Z0", 1.);   // z pos. of back face of the ring rel. to back of envelope
	std::string name = m_BaseName + id;
	GeoPcon *shapeBIR = getPcon(id);
	GeoLogVol *logicalBIR = new GeoLogVol(name, shapeBIR, m_Alu);
	GeoPhysVol *physBIR = new GeoPhysVol(logicalBIR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physBIR);

	id = "BackInnerRing::Hole";
	name = m_BaseName + id;
	GeoPcon *shapeBIRH = getPcon(id);
	GeoLogVol *logicalBIRH = new GeoLogVol(name, shapeBIRH, m_LAr);
	GeoPhysVol *physBIRH = new GeoPhysVol(logicalBIRH);
	physBIR->add(physBIRH);

  //endpiece of the inner longitudinal bar
	id = "BackInnerRing::GTen";
	name = m_BaseName + id;
	GeoPcon *shapeBIRG = getPcon(id);
	GeoLogVol *logicalBIRG = new GeoLogVol(name, shapeBIRG, m_Gten);
	GeoPhysVol *physBIRG = new GeoPhysVol(logicalBIRG);
	physBIRH->add(physBIRG);
}

void EMECSupportConstruction::put_back_inner_longbar(GeoPhysVol *motherPhysical) const
{
	std::string id = "BackInnerLongBar";
	std::string name = m_BaseName + id;
	map_t tubes = getMap(m_DB_tubes, "TUBENAME");

	double rmin = getNumber(m_DB_tubes, tubes, id, "RMIN", 357.5-1.+40.5);//RInner +40.5
	double rmax = getNumber(m_DB_tubes, tubes, id, "RMAX", 699.-2.5-40.);//RMiddle-40.
	double dz = getNumber(m_DB_tubes, tubes, id, "DZ", 20./2.);
	double z0 = getNumber(m_DB_numbers, id, "Z0", 24.) + dz;
	GeoTubs *shapeBILB = new GeoTubs(rmin, rmax, dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalBILB = new GeoLogVol(name, shapeBILB, m_Gten);
	GeoPhysVol *physBILB = new GeoPhysVol(logicalBILB);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physBILB);
}

void EMECSupportConstruction::put_front_outer_ring(GeoPhysVol *motherPhysical) const
{
	std::string id = "FrontOuterRing";
	double z0 = getNumber(m_DB_numbers, id, "Z0", 9.); //position of the front face of the ring rel. to front of envelope
	std::string name = m_BaseName + id;
	GeoPcon *shapeFOR = getPcon(id);
	GeoLogVol *logicalFOR = new GeoLogVol(name, shapeFOR, m_Alu);
	GeoPhysVol *physFOR = new GeoPhysVol(logicalFOR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physFOR);
}

void EMECSupportConstruction::put_front_outer_longbar(GeoPhysVol *motherPhysical) const
{
	std::string id = "FrontOuterLongBar";
	double z0 = getNumber(m_DB_numbers, id, "Z0", 29.);//rel to front of envelope
	std::string name = m_BaseName + id;
	GeoPcon *shapeFOLB = getPcon(id);
	GeoLogVol *logicalFOLB = new GeoLogVol(name, shapeFOLB, m_Gten);
	GeoPhysVol *physFOLB = new GeoPhysVol(logicalFOLB);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physFOLB);
}

void EMECSupportConstruction::put_front_indexing_rings(GeoPhysVol *motherPhysical) const
{
	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	map_t numbers = getNumbersMap(m_DB_numbers, "FrontIndexingRings");

	double r0 = getNumber(m_DB_numbers, numbers, "Router", "PARVALUE", 1565.-5.);
	double r1 = getNumber(m_DB_numbers, numbers, "Rinner", "PARVALUE", 1025.-4.);
	std::string id = "FrontIndexingRing";
	double ring_rmin = getNumber(m_DB_tubes, tubes, id, "RMIN", -15.);
	double ring_rmax = getNumber(m_DB_tubes, tubes, id, "RMAX", 15.);
	double ring_dz = getNumber(m_DB_tubes, tubes, id, "DZ", 21./2.);
	double z_hole = -ring_dz;
	double z0 = getNumber(m_DB_numbers, numbers, "Z0", "PARVALUE", 9.) + ring_dz;
	double gten_dz = getNumber(m_DB_numbers, numbers, "GTenDZ", "PARVALUE", (16. - 11.) / 2);
	id += "::Hole";
	double hole_rmin = getNumber(m_DB_tubes, tubes, id, "RMIN", -6.5);
	double hole_rmax = getNumber(m_DB_tubes, tubes, id, "RMAX", 6.5);
	double hole_dz = getNumber(m_DB_tubes, tubes, id, "DZ", 5.55/2.);

	std::string name = m_BaseName + "FrontHighRIndexingRing";
	GeoTubs *shapeFHIR = new GeoTubs(r0 + ring_rmin, r0 + ring_rmax, ring_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFHIR = new GeoLogVol(name, shapeFHIR, m_Alu);
	GeoPhysVol *physFHIR = new GeoPhysVol(logicalFHIR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physFHIR);

	name = m_BaseName + "FrontHighRIndexingRing::Hole";
	GeoTubs *shapeFHIRH = new GeoTubs(r0 + hole_rmin, r0 + hole_rmax, hole_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFHIRH = new GeoLogVol(name, shapeFHIRH, m_LAr);
	GeoPhysVol *physFHIRH = new GeoPhysVol(logicalFHIRH);
	physFHIR->add(new GeoTransform(GeoTrf::TranslateZ3D(z_hole + hole_dz)));
	physFHIR->add(physFHIRH);

  // the piece of long.bar on which indexing alu ring is sitting
	name = m_BaseName + "FrontHighRIndexingRing::GTen";
//	GeoTubs *shapeFHIRG = new GeoTubs(r0 + ring_rmin, r0 + ring_rmax, ring_dz, m_PhiStart, m_PhiSize);
	GeoTubs *shapeFHIRG = new GeoTubs(r0 + ring_rmin, r0 + ring_rmax, gten_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFHIRG = new GeoLogVol(name, shapeFHIRG, m_Gten);
	GeoPhysVol *physFHIRG = new GeoPhysVol(logicalFHIRG);
	physFHIR->add(new GeoTransform(GeoTrf::TranslateZ3D(ring_dz - gten_dz)));
	physFHIR->add(physFHIRG);

	name = m_BaseName + "FrontLowRIndexingRing";
	GeoTubs *shapeFLIR = new GeoTubs(r1 + ring_rmin, r1 + ring_rmax, ring_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFLIR = new GeoLogVol(name, shapeFLIR, m_Alu);
	GeoPhysVol *physFLIR = new GeoPhysVol(logicalFLIR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physFLIR);

	name = m_BaseName + "FrontLowRIndexingRing::Hole";
	GeoTubs *shapeFLIRH = new GeoTubs(r1 + hole_rmin, r1 + hole_rmax, hole_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFLIRH = new GeoLogVol(name, shapeFLIRH, m_LAr);
	GeoPhysVol *physFLIRH = new GeoPhysVol(logicalFLIRH);
	physFLIR->add(new GeoTransform(GeoTrf::TranslateZ3D(z_hole + hole_dz)));
	physFLIR->add(physFLIRH);

	name = m_BaseName + "FrontLowRIndexingRing::GTen"; //piece of long.bar
//	GeoTubs *shapeFLIRG = new GeoTubs(r1 + ring_rmin, r1 + ring_rmax, ring_dz, m_PhiStart, m_PhiSize);
	GeoTubs *shapeFLIRG = new GeoTubs(r1 + ring_rmin, r1 + ring_rmax, gten_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalFLIRG = new GeoLogVol(name, shapeFLIRG, m_Gten);
	GeoPhysVol *physFLIRG = new GeoPhysVol(logicalFLIRG);
	physFLIR->add(new GeoTransform(GeoTrf::TranslateZ3D(ring_dz - gten_dz)));
	physFLIR->add(physFLIRG);
}

void EMECSupportConstruction::put_back_indexing_rings(GeoPhysVol *motherPhysical) const
{
	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	map_t numbers = getNumbersMap(m_DB_numbers, "BackIndexingRings");

	double r0 = getNumber(m_DB_numbers, numbers, "Router", "PARVALUE", 1437.-5.);
	double r1 = getNumber(m_DB_numbers, numbers, "Rinner", "PARVALUE", 1051.-4.);
	std::string id = "BackIndexingRing";
	double ring_rmin = getNumber(m_DB_tubes, tubes, id, "RMIN", -15.);
	double ring_rmax = getNumber(m_DB_tubes, tubes, id, "RMAX", 15.);
	double ring_dz = getNumber(m_DB_tubes, tubes, id, "DZ", 16./2.);
	double z_hole = -ring_dz;
	double z0 = getNumber(m_DB_numbers, numbers, "Z0", "PARVALUE", 8.) + ring_dz;
	double gten_dz = getNumber(m_DB_numbers, numbers, "GTenDZ", "PARVALUE", (16. - 11.) / 2);
	id += "::Hole";
	double hole_rmin = getNumber(m_DB_tubes, tubes, id, "RMIN", -6.5);
	double hole_rmax = getNumber(m_DB_tubes, tubes, id, "RMAX", 6.5);
	double hole_dz = getNumber(m_DB_tubes, tubes, id, "DZ", 5.55/2.);

	std::string name = m_BaseName + "BackHighRIndexingRing";
	GeoTubs *shapeBHIR = new GeoTubs(r0 + ring_rmin, r0 + ring_rmax, ring_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalBHIR = new GeoLogVol(name, shapeBHIR, m_Alu);
	GeoPhysVol *physBHIR = new GeoPhysVol(logicalBHIR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physBHIR);

  // the piece of long.bar on which indexing alu ring is sitting
	name += "::GTen";
//	GeoTubs *shapeBHIRG = new GeoTubs(r0 + ring_rmin, r0 + ring_rmax, ring_dz, m_PhiStart, m_PhiSize);
	GeoTubs *shapeBHIRG = new GeoTubs(r0 + ring_rmin, r0 + ring_rmax, gten_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalBHIRG = new GeoLogVol(name, shapeBHIRG, m_Gten);
	GeoPhysVol *physBHIRG = new GeoPhysVol(logicalBHIRG);
	physBHIR->add(new GeoTransform(GeoTrf::TranslateZ3D(ring_dz - gten_dz)));
	physBHIR->add(physBHIRG);

	name = m_BaseName + "BackHighRIndexingRing::Hole";
	GeoTubs* shapeBHIRH = new GeoTubs(r0 + hole_rmin, r0 + hole_rmax, hole_dz, m_PhiStart, m_PhiSize);
	GeoLogVol* logicalBHIRH = new GeoLogVol(name, shapeBHIRH, m_LAr);
	GeoPhysVol* physBHIRH = new GeoPhysVol(logicalBHIRH);
	physBHIR->add(new GeoTransform(GeoTrf::TranslateZ3D(z_hole + hole_dz)));
	physBHIR->add(physBHIRH);

	name = m_BaseName + "BackLowRIndexingRing";
	GeoTubs *shapeBLIR = new GeoTubs(r1 + ring_rmin, r1 + ring_rmax, ring_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalBLIR = new GeoLogVol(name, shapeBLIR, m_Alu);
	GeoPhysVol *physBLIR = new GeoPhysVol(logicalBLIR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physBLIR);

	name += "::GTen";
//	GeoTubs *shapeBLIRG = new GeoTubs(r1 + ring_rmin, r1 + ring_rmax, ring_dz, m_PhiStart, m_PhiSize);
	GeoTubs *shapeBLIRG = new GeoTubs(r1 + ring_rmin, r1 + ring_rmax, gten_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalBLIRG = new GeoLogVol(name, shapeBLIRG, m_Gten);
	GeoPhysVol *physBLIRG = new GeoPhysVol(logicalBLIRG);
	physBLIR->add(new GeoTransform(GeoTrf::TranslateZ3D(ring_dz - gten_dz)));
	physBLIR->add(physBLIRG);

	name = m_BaseName + "BackLowRIndexingRing::Hole";
	GeoTubs *shapeBLIRH = new GeoTubs(r1 + hole_rmin, r1 + hole_rmax, hole_dz, m_PhiStart, m_PhiSize);
	GeoLogVol *logicalBLIRH = new GeoLogVol(name, shapeBLIRH, m_LAr);
	GeoPhysVol *physBLIRH = new GeoPhysVol(logicalBLIRH);
	physBLIR->add(new GeoTransform(GeoTrf::TranslateZ3D(z_hole + hole_dz)));
	physBLIR->add(physBLIRH);
}

void EMECSupportConstruction::put_back_outer_ring(GeoPhysVol *motherPhysical) const
{
	std::string id = "BackOuterRing";
	double z0 = getNumber(m_DB_numbers, id, "Z0", 8.);
	std::string name = m_BaseName + id;
	GeoPcon *shapeBOR = getPcon(id);
	GeoLogVol *logicalBOR = new GeoLogVol(name, shapeBOR, m_Alu);
	GeoPhysVol *physBOR = new GeoPhysVol(logicalBOR);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physBOR);
}

void EMECSupportConstruction::put_back_outer_longbar(GeoPhysVol *motherPhysical) const
{
	std::string id = "BackOuterLongBar";
	double z0 = getNumber(m_DB_numbers, id, "Z0", 23.);
	std::string name = m_BaseName + id;
	GeoPcon *shapeBOLB = getPcon(id);
	GeoLogVol *logicalBOLB = new GeoLogVol(name, shapeBOLB, m_Gten);
	GeoPhysVol *physBOLB = new GeoPhysVol(logicalBOLB);
	motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(z0)));
	motherPhysical->add(physBOLB);
}

void EMECSupportConstruction::put_front_outer_extracyl(GeoPhysVol *motherPhysical) const{

  // put extra material after PS

  unsigned int nextra=m_DB_emecExtraCyl->size();
  if(nextra>0){
      bool finloop=false;
      double dzmax=6.6;
      for(unsigned int i=0;i<nextra;i++){
        std::string name=(*m_DB_emecExtraCyl)[i]->getString("CONE");
          if(name.find("EmecCylAfterPS") != std::string::npos){
            double rmin=(*m_DB_emecExtraCyl)[i]->getDouble("RMIN1"); //PS rmin
            double rmax=(*m_DB_emecExtraCyl)[i]->getDouble("RMAX1"); //PS rmax
            double dz = (*m_DB_emecExtraCyl)[i]->getDouble("DZ");    //leadthickness
            if(dz>0. && dz<= dzmax){

              ISvcLocator *svcLocator = Gaudi::svcLocator();
              StoreGateSvc *detStore;
              if(svcLocator->service("DetectorStore", detStore, false) == StatusCode::FAILURE){
                throw std::runtime_error("Error in EMECSupportConstruction/extracyl, cannot access DetectorStore");
              }
              const StoredMaterialManager* materialManager = nullptr;
              if (detStore->retrieve(materialManager, std::string("MATERIALS")).isFailure()) {
                throw std::runtime_error("Error in EMECSupportConstruction: cannot find MATERIALS.");
              }

              std::string material=(*m_DB_emecExtraCyl)[i]->getString("MATERIAL"); //lead
              const GeoMaterial *mat = materialManager->getMaterial(material);
              if (!mat) {
                throw std::runtime_error("Error in EMECSupportConstruction/extracyl,material for CylBeforePS is not found.");
              }

              std::string id = "ExtraCyl_afterPS";
              std::string name = m_BaseName + id;

              GeoTubs    *solidCyl  = new GeoTubs(rmin, rmax, dz/2., m_PhiStart, m_PhiSize);
              GeoLogVol  *logicCyl  = new GeoLogVol(name, solidCyl, mat);
              GeoPhysVol *physCyl   = new GeoPhysVol(logicCyl);

              motherPhysical->add(new GeoTransform(GeoTrf::TranslateZ3D(dz/2.)));
              motherPhysical->add(physCyl);

              std::cout<<"******************************************************"<<std::endl;
              std::cout<<" EMECSupportConstruction insert extra material after PS"<<std::endl;
              std::cout<<" ExtraCyl params: name,mat= "<<name<<" "<<mat->getName()
                       <<" rmin,rmax,dzthick,zpos="<<rmin<<" "<<rmax<<" "<<dz<<" "<<dz/2.
                       <<" PhiStart,PhiSize="<<m_PhiStart<<" "<<m_PhiSize
                       <<std::endl;
              std::cout<<"******************************************************"<<std::endl;

              finloop=true;
            } // cehck on dz thickness
          } // cehck on EmecCylAfterPS exist
          if(finloop)break;
      } // loop for records
  } // check on record length
} // end of put_front_outer_extracyl

void EMECSupportConstruction::put_front_outer_electronics(GeoPhysVol *motherPhysical) const
{
	map_t boxes = getMap(m_DB_boxes, "BOXNAME");
	map_t tubes = getMap(m_DB_tubes, "TUBENAME");
	map_t fbn = getNumbersMap(m_DB_numbers, "FrontBoard");
	map_t mbn = getNumbersMap(m_DB_numbers, "MotherBoard");
	map_t fcson = getNumbersMap(m_DB_numbers, "FrontCables::SideOuter");

	std::string id = "FrontBoard";

	std::string idx = id + "I";
	std::string name = m_BaseName + idx;
 // up to low indexing ring // -4. for cold
	double rmax = getNumber(m_DB_tubes, tubes, idx, "RMAX", 1025.-4. - 15.);
 // up to middle ring, //-2mm for cold
	double rmin = getNumber(m_DB_tubes, tubes, idx, "RMIN", 614.-2. + 57.);
	double dz_boards = getNumber(m_DB_tubes, tubes, idx, "DZ", 1.);
	double phi_size = M_PI / getNumber(m_DB_numbers, fbn, "PhiSizeDiv", "PARVALUE", 16.);
	double phi_start = -0.5 * phi_size;
	GeoTubs *bi_shape = new GeoTubs(rmin, rmax, dz_boards, phi_start, phi_size);
	GeoLogVol *bi_l = new GeoLogVol(name, bi_shape, m_Gten);
	GeoPhysVol *bi_phys = new GeoPhysVol(bi_l);
	double z_boards = getNumber(m_DB_numbers, fbn, "Zdist", "PARVALUE", 29.) - dz_boards;// 29 - start of longbar

	idx = id + "M";
	name = m_BaseName + idx;
	rmin = getNumber(m_DB_tubes, tubes, idx, "RMIN", 1025.-4. + 15.);  // -4. for cold
	rmax = getNumber(m_DB_tubes, tubes, idx, "RMAX", (1565.-5. - 15.));  // -5. for cold
	dz_boards = getNumber(m_DB_tubes, tubes, idx, "DZ", 1.);
	GeoTubs *bm_shape = new GeoTubs(rmin, rmax, dz_boards, phi_start, phi_size);
	GeoLogVol *bm_l = new GeoLogVol(name, bm_shape, m_Gten);
	GeoPhysVol *bm_phys = new GeoPhysVol(bm_l);

	idx = id + "O";
	name = m_BaseName + idx;
	rmin = getNumber(m_DB_tubes, tubes, idx, "RMIN", (1565.-5. + 15.));  // -5. for cold
	rmax = getNumber(m_DB_tubes, tubes, idx, "RMAX", (1961.-7.));  // -7mm for cold
	dz_boards = getNumber(m_DB_tubes, tubes, idx, "DZ", 1.);
	GeoTubs *bo_shape = new GeoTubs(rmin, rmax, dz_boards, phi_start, phi_size);
	GeoLogVol *bo_l = new GeoLogVol(name, bo_shape, m_Gten);
	GeoPhysVol *bo_phys = new GeoPhysVol(bo_l);

  // !!!To be checked whether the MBs do not cross the indexing ring in cold
	id = "MotherBoard";
	double z_mb = z_boards - dz_boards;
	std::string mb_n[5] = { "F1",      "F2",     "F3",     "F4",    "F5" };
	const double mb_dy[5] = {   74.,    57.,   57.,   60.,  44. };
	const double mb_dx[5] = {   38.5,   60.,  138.5, 100., 165. };
	const double mb_dz_cu[5] = {  .1,     .15,   .15,   .15,  .2 };
  // non-const arrays are updated from DB
	double mb_r[5]  = { (1739.-5.) , (1640.-5.), (1400.-4.), (1140. - 4.), (835.-1.)};
	double mb_dz[5] = {    1.4,    1.1,   1.25,  1.1,  1.25 };

	GeoPhysVol *mb_p[5];
	for(int i = 0; i < 5; ++ i){
		idx = id + "::" + mb_n[i];
		name = m_BaseName + idx;
		double dx = getNumber(m_DB_boxes, boxes, idx, "HLEN", mb_dx[i]);
		double dy = getNumber(m_DB_boxes, boxes, idx, "HWDT", mb_dy[i]);
		mb_dz[i] = getNumber(m_DB_boxes, boxes, idx, "HHGT", mb_dz[i]);
		GeoBox *mb_s = new GeoBox(dx, dy, mb_dz[i]);
		GeoLogVol *mb_l = new GeoLogVol(name, mb_s, m_Gten);
		mb_p[i] = new GeoPhysVol(mb_l);

		name += "::Cu";
		std::ostringstream tmp;
		tmp << "Cu" << (i + 1) << "z";
		double dz1 = getNumber(m_DB_numbers, mbn, tmp.str(), "PARVALUE", mb_dz_cu[i]);
		GeoBox *cu = new GeoBox(dx, dy, dz1);
		GeoLogVol *cul = new GeoLogVol(name, cu, m_Copper);
		GeoPhysVol *cup = new GeoPhysVol(cul);
		mb_p[i]->add(new GeoTransform(GeoTrf::TranslateZ3D(dz1 - mb_dz[i])));
		mb_p[i]->add(cup);

		tmp.str("");
		tmp << "R" << (i + 1);
		mb_r[i] = getNumber(m_DB_numbers, mbn, tmp.str(), "PARVALUE", mb_r[i]);
	}

	id = "FrontCables";

	idx = id + "::Outer";
	name = m_BaseName + idx;

	double dz_oc = getNumber(m_DB_tubes, tubes, idx, "DZ", 5.);// 1cm = about 0.14 RL
	rmin = getNumber(m_DB_tubes, tubes, idx, "RMIN", 1780.); // ?? what is it for cold?
	rmax = getNumber(m_DB_tubes, tubes, idx, "RMAX", 1961.-7.); // -7mm for cold; To be checked
	GeoTubs *oc_s = new GeoTubs(rmin, rmax, dz_oc, phi_start, phi_size);
	GeoLogVol *oc_l = new GeoLogVol(name, oc_s, m_Cable);
	GeoPhysVol *oc_p = new GeoPhysVol(oc_l);
	double z_oc = z_boards - dz_boards - dz_oc;

	idx = id + "::SideOuter";
	name = m_BaseName + idx;
	double dz_soc = getNumber(m_DB_tubes, tubes, idx, "DZ", 1.2);
	rmin = getNumber(m_DB_tubes, tubes, idx, "RMIN", 1000.);
	rmax = getNumber(m_DB_tubes, tubes, idx, "RMAX", 1780.);
	double dphi_sc = getNumber(m_DB_numbers, fcson, "Width", "PARVALUE", 100.) / rmax;
	GeoTubs *soc_s = new GeoTubs(rmin, rmax, dz_soc, -0.5 * dphi_sc, dphi_sc);
	GeoLogVol *soc_l = new GeoLogVol(name, soc_s, m_Cable);
	GeoPhysVol *soc_p = new GeoPhysVol(soc_l);
   // relative to indexing rings
	double z_soc = getNumber(m_DB_numbers, fcson, "Zdist", "PARVALUE", 9.) - dz_soc;

	const int number_of_sectors = m_isModule? 4: 32;
	for(int i = 0; i < number_of_sectors; ++ i){
		double phi = m_Position + (i - 2) * phi_size - phi_start;

		GeoIdentifierTag* iTag = new GeoIdentifierTag(i);
		GeoTransform* xf = new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(0., 0., z_boards)*GeoTrf::Transform3D(GeoTrf::RotateZ3D(phi))));
		GeoTransform* xf1 = new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(0., 0., z_oc)*GeoTrf::RotateZ3D(phi)));

		motherPhysical->add(iTag);
		motherPhysical->add(xf);
		motherPhysical->add(bi_phys);

		motherPhysical->add(iTag);
		motherPhysical->add(xf);
		motherPhysical->add(bm_phys);

		motherPhysical->add(iTag);
		motherPhysical->add(xf);
		motherPhysical->add(bo_phys);

		motherPhysical->add(iTag);
		motherPhysical->add(xf1);
		motherPhysical->add(oc_p);

		for(int j = 0; j < 5; ++ j){
			GeoTransform* xf2 = new GeoTransform(
				GeoTrf::Transform3D(GeoTrf::RotateZ3D(phi)*GeoTrf::Translate3D(mb_r[j], 0., z_mb - mb_dz[j])));
			motherPhysical->add(iTag);
			motherPhysical->add(xf2);
			motherPhysical->add(mb_p[j]);
		}

		GeoTransform* xf3 = new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(0., 0., z_soc)*GeoTrf::RotateZ3D(m_Position + (i - 2) * phi_size + 0.5 * dphi_sc)));;

		GeoTransform* xf4 = new GeoTransform(GeoTrf::Transform3D(GeoTrf::Translate3D(0., 0., z_soc)*GeoTrf::RotateZ3D(m_Position + (i - 1) * phi_size - 0.5 * dphi_sc)));

		motherPhysical->add(new GeoIdentifierTag(i * 2));
		motherPhysical->add(xf3);
		motherPhysical->add(soc_p);

		motherPhysical->add(new GeoIdentifierTag(i * 2 + 1));
		motherPhysical->add(xf4);
		motherPhysical->add(soc_p);
	}
}

GeoPhysVol* EMECSupportConstruction::front_inner_envelope(void) const
{
	std::string id = "FrontSupportMother";
	std::string name = m_BaseName + id;
	GeoPcon *motherShape = getPcon(id + "::Inner");
	GeoLogVol *motherLogical = new GeoLogVol(name, motherShape, m_LAr);
	GeoPhysVol *motherPhysical = new GeoPhysVol(motherLogical);

	put_front_inner_ring(motherPhysical);
	put_front_inner_longbar(motherPhysical);
	put_front_inner_barettes(motherPhysical);

	return motherPhysical;
}

GeoPhysVol* EMECSupportConstruction::back_inner_envelope(void) const
{
	std::string id = "BackSupportMother";
	std::string name = m_BaseName + id;
	GeoPcon *motherShape = getPcon(id + "::Inner");
	GeoLogVol *motherLogical = new GeoLogVol(name, motherShape, m_LAr);
	GeoPhysVol *motherPhysical = new GeoPhysVol(motherLogical);

	put_back_inner_ring(motherPhysical);
	put_back_inner_longbar(motherPhysical);
	put_back_inner_barettes(motherPhysical);

	return motherPhysical;
}

GeoPhysVol* EMECSupportConstruction::front_outer_envelope(void) const
{
	std::string id = "FrontSupportMother";
	std::string name = m_BaseName + id;
	GeoPcon *motherShape = getPcon(id + "::Outer");
	GeoLogVol *motherLogical = new GeoLogVol(name, motherShape, m_LAr);
	GeoPhysVol *motherPhysical= new GeoPhysVol(motherLogical);

	put_front_outer_ring(motherPhysical);
	put_front_outer_longbar(motherPhysical);
	put_front_indexing_rings(motherPhysical);
	put_front_middle_ring(motherPhysical);
	put_front_outer_barettes(motherPhysical);
	put_front_outer_electronics(motherPhysical);

	return motherPhysical;
}

GeoPhysVol* EMECSupportConstruction::back_outer_envelope(void) const
{
	std::string id = "BackSupportMother";
	std::string name = m_BaseName + id;
	GeoPcon *motherShape = getPcon(id + "::Outer");
	GeoLogVol *motherLogical = new GeoLogVol(name, motherShape, m_LAr);
	GeoPhysVol *motherPhysical= new GeoPhysVol(motherLogical);

	put_back_indexing_rings(motherPhysical);
	put_back_outer_ring(motherPhysical);
	put_back_middle_ring(motherPhysical);
	put_back_outer_longbar(motherPhysical);
	put_back_outer_barettes(motherPhysical);

	return motherPhysical;
}
