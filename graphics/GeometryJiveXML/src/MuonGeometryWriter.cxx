/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeometryJiveXML/MuonGeometryWriter.h"
#include "JiveXML/DataType.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include <fstream>

namespace JiveXML {
 
  const double MuonGeometryWriter::m_smallAngle    = 0.05;
  const double MuonGeometryWriter::m_smallDistance = 100.;

  StatusCode MuonGeometryWriter::writeGeometry() {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "writeGeometry()" << endreq;

    std::ofstream outputFile("AMuonGeometry.xml");
    writeHeader(outputFile);

    if ( detStore()->retrieve(m_muon_manager).isFailure() ) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not retrieve MuonGM::MuonDetectorManager" << endreq;
      m_muon_manager = 0;
      return StatusCode::FAILURE;
    } else {
      writeStations(outputFile);
    }

    writeFooter(outputFile);
    outputFile.close();

    return StatusCode::SUCCESS;
  }

  void MuonGeometryWriter::writeHeader(std::ofstream &out) const {

    out << "<?xml version=\"1.0\"?>"                               << std::endl
	<< "<!DOCTYPE AMuonGeometry ["                             << std::endl
	<< "<!ELEMENT AMuonGeometry (ABox | ATBx | ATrd)*>"        << std::endl
	<< "<!ELEMENT ABox   EMPTY >"                              << std::endl
	<< "<!ATTLIST ABox"                                        << std::endl
	<< "          n          CDATA #REQUIRED"                  << std::endl
	<< "          zi         CDATA #REQUIRED"                  << std::endl
	<< "          zo         CDATA #REQUIRED"                  << std::endl
	<< "          ri         CDATA #REQUIRED"                  << std::endl
	<< "          ro         CDATA #REQUIRED"                  << std::endl
	<< "          w          CDATA #REQUIRED"                  << std::endl
	<< "          eta        CDATA #REQUIRED"                  << std::endl
	<< "          phi        CDATA #REQUIRED"                  << std::endl
	<< "          dphi       CDATA \"0\""                      << std::endl
	<< "          sh         CDATA \"0\""                      << std::endl
	<< "          RPCi       CDATA \"0\""                      << std::endl
	<< "          RPCo       CDATA \"0\">"                     << std::endl
	<< "<!ELEMENT ATBx   EMPTY >"                              << std::endl
	<< "<!ATTLIST ATBx"                                        << std::endl
	<< "          n          CDATA #REQUIRED"                  << std::endl
	<< "          zi         CDATA #REQUIRED"                  << std::endl
	<< "          zo         CDATA #REQUIRED"                  << std::endl
	<< "          ri         CDATA #REQUIRED"                  << std::endl
	<< "          ro         CDATA #REQUIRED"                  << std::endl
	<< "          w          CDATA #REQUIRED"                  << std::endl
	<< "          eta        CDATA #REQUIRED"                  << std::endl
	<< "          phi        CDATA #REQUIRED"                  << std::endl
	<< "          sh         CDATA \"0\""                      << std::endl
	<< "          dphi       CDATA \"0\""                      << std::endl
	<< "          RPCi       CDATA \"0\""                      << std::endl
	<< "          RPCo       CDATA \"0\""                      << std::endl
	<< "          zis        CDATA #REQUIRED"                  << std::endl
	<< "          zos        CDATA #REQUIRED"                  << std::endl
	<< "          ws         CDATA #REQUIRED"                  << std::endl
	<< "          or         CDATA \"0\">"                     << std::endl
	<< "<!ELEMENT ATrd   EMPTY >"                              << std::endl
	<< "<!ATTLIST ATrd"                                        << std::endl
	<< "          n          CDATA #REQUIRED"                  << std::endl
	<< "          zi         CDATA #REQUIRED"                  << std::endl
	<< "          zo         CDATA #REQUIRED"                  << std::endl
	<< "          ri         CDATA #REQUIRED"                  << std::endl
	<< "          ro         CDATA #REQUIRED"                  << std::endl
	<< "          wi         CDATA #REQUIRED"                  << std::endl
	<< "          wo         CDATA #REQUIRED"                  << std::endl
	<< "          eta        CDATA #REQUIRED"                  << std::endl
	<< "          phi        CDATA #REQUIRED"                  << std::endl
	<< "          dphi       CDATA \"0\""                      << std::endl
	<< "          sh         CDATA \"0\""                      << std::endl
	<< "          a          CDATA \"0\">"                     << std::endl
	<< "]>"                                                    << std::endl
	<< "<AMuonGeometry>"                                       << std::endl;
  }

  void MuonGeometryWriter::writeStations(std::ofstream &out) const {

    // While we retrieve the MdtIdHelper, we use the functionality provided by its parent class MuonIdHelper 
    // this is not MDT specific and any of the other IdHelpers would have worked as well.
    const MdtIdHelper *mdtIdHelper = m_muon_manager->mdtIdHelper();
    int snMax = mdtIdHelper->stationNameIndexMax();

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Station types: " << snMax << endreq;

    // Loop over all station types.
    for (int sn=0; sn<=snMax; sn++) {
      std::string stationName = mdtIdHelper->stationNameString(sn);
      // some stationNames no longer exist
      if (!stationName.compare(MuonIdHelper::BAD_NAME)) continue;

      // Determine the type of chamber from the stationName string.
      std::string stationTech;
      switch(stationName[0]) {
      case 'B':
      case 'E':
	stationTech = "MDT";
	break;
      case 'C':
	stationTech = "CSC";
	break;
      case 'T':
	stationTech = "TGC";
	break;
      default:
	stationTech = "???";
	break;
      }

      // The TGCs contain several stations in one sector. As this would create several stations with the same 
      // identifier, people apparently chose to let the phi index of the stations continue up to 3 or even 6 
      // times the "real" maximum value. We need to determine this maximum value in order to calculate the real
      // phi index.
      int maxPhi;
      if (stationTech == "TGC") {
	if (stationName[2] == 'E' && stationName[1] != '4') {
	  maxPhi = 48;
	} else {
	  maxPhi = 24;
	}
      } else {
	maxPhi = 8;
      }

      // Loop over all eta values.
      for (int eta=0; eta<=16; eta++) {
	std::vector<const MuonGM::MuonStation *> *stations = new std::vector<const MuonGM::MuonStation *>;

	// And loop over all possible phi values.
	for (int phi=maxPhi; phi>0; phi--) {

	  // Try to retrieve the station, it might not exist, but this seems to be the most 
	  // reliable way to find out.
	  const MuonGM::MuonStation *station = m_muon_manager->getMuonStation(stationName, eta, phi);

	  // If station != 0, the station exists and we add it to our vector.
	  if (station) stations->push_back(station);
	}

        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Station size: " << stations->size() << endreq;

	// While there are stations that haven't been written to XML, stay in this loop.
	while (stations->size() > 0) {

	  // Take the parameters of a station and store them in the variables with the "1" suffix.
	  const MuonGM::MuonStation *station1 = *(stations->end()-1);

	  HepGeom::Point3D<double> pos1 = getPosition(station1, maxPhi);
	  int phi1 = station1->getPhiIndex();
	  double dphi1 = getDeltaPhi(pos1, maxPhi);
	  double shift1 = getShift(pos1, dphi1);
	  double alpha1 = getAlpha(station1->getTransform());

	  // Now determine the dimensions of a station of this station.
	  double zi1 = pos1.z() - station1->Zsize()/2.;    // inner z
	  double zo1 = pos1.z() + station1->Zsize()/2.;    // outer z
	  double ri1 = pos1.perp() - station1->Rsize()/2.; // inner r
	  double ro1 = pos1.perp() + station1->Rsize()/2.; // outer r
	  double wi1 = station1->Ssize();                  // width at inner r
	  double wo1 = station1->LongSsize();              // width at outer r

	  // Create the string containing the phi sectors this station is in.
	  std::string phiString = DataType(phi1).toString();

	  // Remove this station from the to-be-processed list.
	  stations->erase(stations->end()-1, stations->end());

	  // Now loop over the other stations and see if any of them have the same parameters. Do 
	  // this in reverse order to allow the current station to be erased from the vector.
	  std::vector<const MuonGM::MuonStation *>::iterator it;
	  for (it=stations->end()-1; it>=stations->begin(); it--) {
	    HepGeom::Point3D<double> pos2 = getPosition(*it, maxPhi);
	    int phi2 = (*it)->getPhiIndex();
	    double dphi2 = getDeltaPhi(pos2, maxPhi);
	    double shift2 = getShift(pos2, dphi2);
	    double alpha2 = getAlpha((*it)->getTransform());

	    double zi2 = pos2.z() - (*it)->Zsize()/2.;    // inner z
	    double zo2 = pos2.z() + (*it)->Zsize()/2.;    // outer z
	    double ri2 = pos2.perp() - (*it)->Rsize()/2.; // inner r
	    double ro2 = pos2.perp() + (*it)->Rsize()/2.; // outer r
	    double wi2 = (*it)->Ssize();                  // width at inner r
	    double wo2 = (*it)->LongSsize();              // width at outer r

	    // If the parameters are indeed the same (within reasonable limits), then we 
	    // can simply add this chamber to the sectors attribute of the first one.
	    if (pos1.distance(pos2) < m_smallDistance
		&& fabs(dphi1 - dphi2) < m_smallAngle
		&& fabs(shift1 - shift2) < m_smallDistance
		&& fabs(alpha1 - alpha2) < m_smallAngle
		&& fabs(zi1 - zi2) < m_smallDistance
		&& fabs(zo1 - zo2) < m_smallDistance
		&& fabs(ri1 - ri2) < m_smallDistance
		&& fabs(ro1 - ro2) < m_smallDistance
		&& fabs(wi1 - wi2) < m_smallDistance
		&& fabs(wo1 - wo2) < m_smallDistance) {

	      // Add the station to the phi sector string and remove it from the 
	      // to-be-processed list.
	      phiString += " " + DataType(phi2).toString();

              if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " phiString " << phiString << endreq;

	      stations->erase(it, it+1);
	    }
	  }

	  // From here onwards we need to treat barrel chambers and endcap chambers differently.
	  if (stationName[0] == 'B') {

	    // Barrel chambers can have inner and/or outer RPCs.
	    // Let's take a default of 0. (no RPCs).
	    double rpci = 0.;
	    double rpco = 0.;

	    // Determine the thickness of the RPC layer on this station.
	    if (stationName[1] == 'M') {
	      // Middle (BM*) stations have RPCs on both sides.
	      rpci = rpco = 15.;
	    } else if (stationName[1] == 'O') {
	      // Outer (BO*) stations have RPCs on one side.
	      if (stationName[2] == 'S') {
		// On the side facing the IP for small sectors (BOS).
		rpci = 15.;
	      } else {
		// On the outside for large sectors (BOL, BOF, BOG, BOH).
		rpco = 15.;
	      }
	    }

	    // Barrel chambers are written as <ABox> elements.
	    out << "<ABox n=\"" << stationTech << "_" << stationName << "\""
		<< " zi=\"" << zi1/10. << "\"" << " zo=\"" << zo1/10. << "\""
		<< " ri=\"" << ri1/10. << "\"" << " ro=\"" << ro1/10. << "\""
		<< " w=\"" << wi1/10. << "\""
		<< " eta=\"" << eta << "\""
		<< " phi=\"" << phiString << "\"";

	    // A rotation with respect to the large sector.
	    if (fabs(dphi1) > m_smallAngle)
	      out << " dphi=\"" << 180/M_PI * dphi1 << "\"";

	    // A shift perpendicular to r in the xy-plane.
	    if (fabs(shift1) > m_smallDistance)
	      out << " sh=\"" << shift1/10. << "\"";

	    // RPCs.
	    if (rpci > 0.)
	      out << " RPCi=\"" << rpci << "\"";
	    if (rpco > 0.)
	      out << " RPCo=\"" << rpco << "\"";
	    out << " />" << std::endl;

	  } else {

	    // Endcap chambers are written as <ATrd> elements, parameters are similar to <ABox>.
	    out << "<ATrd n=\"" << stationTech << "_" << stationName << eta << "\""
		<< " zi=\"" << zi1/10. << "\"" << " zo=\"" << zo1/10. << "\""
		<< " ri=\"" << ri1/10. << "\"" << " ro=\"" << ro1/10. << "\""
		<< " wi=\"" << wi1/10. << "\"" << " wo=\"" << wo1/10. << "\""
		<< " eta=\"" << eta << "\""
		<< " phi=\"" << phiString << "\"";
	    if (fabs(dphi1) > m_smallAngle)
	      out << " dphi=\"" << 180/M_PI * dphi1 << "\"";
	    if (fabs(shift1) > m_smallDistance)
	      out << " sh=\"" << shift1/10. << "\"";

	    // A tilt in the rz-plane, for the CSCs.
	    if (fabs(alpha1) > m_smallAngle)
	      out << " a=\"" << 180/M_PI * alpha1 << "\"";
	    out << " />" << std::endl;
	  }
	}

	delete stations;
      }
    }
  }

  HepGeom::Point3D<double> MuonGeometryWriter::getPosition(const MuonGM::MuonStation *station, int maxPhi) const {

    // Take the position of the station.
    HepGeom::Point3D<double> pos = station->getTransform() * HepGeom::Point3D<double>(0., 0., 0.);

    double phi = 2.*M_PI * ((double) station->getPhiIndex()-1.) / maxPhi;

    // Rotate it to sector 1.
    //return HepGeom::RotateZ3D((1-phi) * M_PI/4.) * pos;
    return HepGeom::RotateZ3D(-phi) * pos;
  }

  double MuonGeometryWriter::getDeltaPhi(const HepGeom::Point3D<double> &pos, int maxPhi) const {

    if (maxPhi > 8) {
      // For TGCs there is no shift, so we can just return the angle.
      return pos.phi();
    } else if (fabs(pos.phi() - M_PI/8.) < m_smallAngle) {
      // For the others, we consider it to be the angle if it's reasonably close to pi/8. 
      // Any further deviation will be put in as a shift.
      return M_PI/8.;
    } else {
      // No rotation at all.
      return 0.;
    }
  }

  double MuonGeometryWriter::getAlpha(const HepGeom::Transform3D &trans) const {

    // Extract the rotation from the transformation.
    CLHEP::HepRotation rot = trans.getRotation();

    // The theta component is what we're interested in.
    return M_PI/2. - rot.getTheta();
  }

  double MuonGeometryWriter::getShift(const HepGeom::Point3D<double> &pos, double dphi) const {

    HepGeom::Point3D<double> rotpos;

    // First we remove the shift caused by the rotation over dphi.
    if (fabs(dphi) < m_smallAngle) {
      rotpos = pos;
    } else {
      rotpos = HepGeom::RotateZ3D(-dphi) * pos;
    }

    // Then we return what is left as the shift.
    if (fabs(rotpos.y()) < m_smallDistance) {
      return 0.;
    } else {
      return rotpos.y();
    }
  }

  void MuonGeometryWriter::writeFooter(std::ofstream &out) const {

    out << "</AMuonGeometry>" << std::endl;

  }
}
