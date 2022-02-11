/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GeometryJiveXML/MuonGeometryWriter.h"
#include "JiveXML/DataType.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelUtilities/GeoModelExperiment.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoShapeShift.h"

#include <fstream>

namespace JiveXML {
 
  const double MuonGeometryWriter::m_smallAngle    = 0.05;
  const double MuonGeometryWriter::m_smallDistance = 100.;

  StatusCode MuonGeometryWriter::writeGeometry() {

    ATH_MSG_DEBUG( "writeGeometry()" );

    std::ofstream outputFile("AMuonGeometry.xml");
    writeHeader(outputFile);

    if ( detStore()->retrieve(m_muon_manager).isFailure() ) {
      ATH_MSG_ERROR( "Could not retrieve MuonGM::MuonDetectorManager" );
      m_muon_manager = 0;
      return StatusCode::FAILURE;
    } else {
      writeStations(outputFile);
    }

    processNSW(outputFile);

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

    ATH_MSG_DEBUG( " Station types: " << snMax );

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
      for (int eta=-16; eta<=16; eta++) {
	std::vector<const MuonGM::MuonStation *> *stations = new std::vector<const MuonGM::MuonStation *>;

	// And loop over all possible phi values.
	for (int phi=maxPhi; phi>0; phi--) {

	  // Try to retrieve the station, it might not exist, but this seems to be the most 
	  // reliable way to find out.
	  const MuonGM::MuonStation *station = m_muon_manager->getMuonStation(stationName, eta, phi);

	  // If station != 0, the station exists and we add it to our vector.
	  if (station) stations->push_back(station);
	}

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
	  double signed_dz = station1->Zsize()/2.;
	  if (pos1.z()<0) signed_dz *= -1;
	  double zi1 = pos1.z() - signed_dz;    // inner z
	  double zo1 = pos1.z() + signed_dz;    // outer z
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

	    double signed_dz2 = (*it)->Zsize()/2.;
	    if (pos2.z()<0) signed_dz2 *= -1;
	    double zi2 = pos2.z() - signed_dz2;    // inner z
	    double zo2 = pos2.z() + signed_dz2;    // outer z
	    double ri2 = pos2.perp() - (*it)->Rsize()/2.; // inner r
	    double ro2 = pos2.perp() + (*it)->Rsize()/2.; // outer r
	    double wi2 = (*it)->Ssize();                  // width at inner r
	    double wo2 = (*it)->LongSsize();              // width at outer r

	    // If the parameters are indeed the same (within reasonable limits), then we 
	    // can simply add this chamber to the sectors attribute of the first one.
	    if (pos1.distance(pos2) < m_smallDistance
		&& equalAngle(dphi1, dphi2)
		&& equalLength(shift1, shift2)
		&& equalAngle(alpha1, alpha2)
		&& equalLength(zi1, zi2)
		&& equalLength(zo1, zo2)
		&& equalLength(ri1, ri2)
		&& equalLength(ro1, ro2)
		&& equalLength(wi1, wi2)
		&& equalLength(wo1, wo2)) {

	      // Add the station to the phi sector string and remove it from the 
	      // to-be-processed list.
	      phiString += " " + DataType(phi2).toString();

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
	    } else if (stationName[1] == 'I' && eta==7 && wi1>1800) {
        // Run3 BIS7/8 stations have RPCs on the inner side and they are wider than the Run2 BIS7 and BIS8 stations.
        rpci = 4.;
        }

	    // Barrel chambers are written as <ABox> elements.
	    out << "<ABox n=\"" << stationTech << "_" << stationName << "\""
		<< " zi=\"" << zi1/10. << "\"" << " zo=\"" << zo1/10. << "\""
		<< " ri=\"" << ri1/10. << "\"" << " ro=\"" << ro1/10. << "\""
		<< " w=\"" << wi1/10. << "\""
		<< " eta=\"" << eta << "\""
		<< " phi=\"" << phiString << "\"";

	    // A rotation with respect to the large sector.
	    if (std::abs(dphi1) > m_smallAngle)
	      out << " dphi=\"" << 180/M_PI * dphi1 << "\"";

	    // A shift perpendicular to r in the xy-plane.
	    if (std::abs(shift1) > m_smallDistance)
	      out << " sh=\"" << shift1/10. << "\"";

	    // RPCs.
	    if (rpci > 0.)
	      out << " RPCi=\"" << rpci << "\"";
	    if (rpco > 0.)
	      out << " RPCo=\"" << rpco << "\"";
	    out << " />" << std::endl;

	  } else {

	    // Endcap chambers are written as <ATrd> elements, parameters are similar to <ABox>.
        writeATrd(out, stationTech, stationName, zi1, zo1, ri1, ro1, wi1, wo1, eta, phiString, dphi1, shift1, alpha1);
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
    } else if (std::abs(pos.phi() - M_PI/8.) < M_PI/16.) {
      // For the others, rotate to the next sector if it's reasonably close to pi/8. 
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
    if (std::abs(dphi) < m_smallAngle) {
      rotpos = pos;
    } else {
      rotpos = HepGeom::RotateZ3D(-dphi) * pos;
    }

    // Then we return what is left as the shift.
    if (std::abs(rotpos.y()) < m_smallDistance) {
      return 0.;
    } else {
      return rotpos.y();
    }
  }

  void MuonGeometryWriter::writeFooter(std::ofstream &out) const {

    out << "</AMuonGeometry>" << std::endl;

  }

  void MuonGeometryWriter::writeATrd(std::ofstream &out,
        std::string stationTech, std::string stationName,
        double zi, double zo, double ri, double ro, double wi, double wo,
        int eta, std::string phiString,
        double dphi, double shift, double alpha) const {

     out << "<ATrd n=\"" << stationTech << "_" << stationName << std::abs(eta) << "\""
        << " zi=\"" << zi/10. << "\"" << " zo=\"" << zo/10. << "\""
        << " ri=\"" << ri/10. << "\"" << " ro=\"" << ro/10. << "\""
        << " wi=\"" << wi/10. << "\"" << " wo=\"" << wo/10. << "\""
        << " eta=\"" << eta << "\""
        << " phi=\"" << phiString << "\"";

     // A rotation with respect to the large sector.
     if (std::abs(dphi) > m_smallAngle)
        out << " dphi=\"" << 180/M_PI * dphi << "\"";

     // A shift perpendicular to r in the xy-plane.
     if (std::abs(shift) > m_smallDistance)
        out << " sh=\"" << shift/10. << "\"";

     // A tilt in the rz-plane, for the CSCs.
     if (std::abs(alpha) > m_smallAngle)
        out << " a=\"" << 180/M_PI * alpha << "\"";

     out << " />" << std::endl;


  }

  void MuonGeometryWriter::processNSW(std::ofstream &out) {

     int maxPhi = 8;

     // Check if NSW exists in the GeoModel tree, and process it if exists
     const GeoModelExperiment * theExpt = nullptr;
     if (detStore()->retrieve(theExpt, "ATLAS").isFailure()) {
        ATH_MSG_ERROR( "Could not retrieve the ATLAS GeoModelExperiment from detector store" );
        return;
     }

     PVConstLink world(theExpt->getPhysVol());
     GeoVolumeCursor av(world);
     while (!av.atEnd()) {
        if ( av.getName()=="Muon") {
           GeoVolumeCursor pv(av.getVolume());
           while (!pv.atEnd()) { // Loop over Muon stations
              if (pv.getVolume()->getLogVol()->getName()=="NewSmallWheel") {
                 ATH_MSG_INFO( "Found New Small Wheel geometry." );
                 GeoVolumeCursor pvnsw(pv.getVolume());

                 while (!pvnsw.atEnd()){
                    std::string stationName = pvnsw.getVolume()->getLogVol()->getName();

                    // Process MicroMegas chambers
                    if (stationName=="NSW_MM"){
                       ATH_MSG_DEBUG( "Processing NSW micromegas chambers." );

                       GeoVolumeCursor pvnswsub(pvnsw.getVolume());
                       bool newChamber = true;
                       std::string phiString = "";
                       std::string phiString_mirrorEta = "";
                       double dphi=0, shift=0, zi=0, zo=0, ri=0, ro=0, wi=0, wo=0;
                       std::string chamberName="";
                       HepGeom::Point3D<double> pos_rot;

                       while (!pvnswsub.atEnd()){
                          if (((pvnswsub.getVolume()->getLogVol())->getShape())->typeID() == GeoTrd::getClassTypeID() ) { // MicroMega

                             if (newChamber){

                                int phiIndex;
                                readNSWMMPars(&pvnswsub, maxPhi, chamberName, pos_rot, zi, zo, ri, ro, wi, wo, dphi, shift, phiIndex);
                                phiString = DataType(phiIndex).toString();

                                newChamber = false;
                                pvnswsub.next();
                             } // end of processing the first chamber

                             else{
                                std::string chamberName2;
                                HepGeom::Point3D<double> pos_rot2;
                                double zi2, zo2, ri2, ro2, wi2, wo2, dphi2, shift2;
                                int phiIndex2;
                                readNSWMMPars(&pvnswsub, maxPhi, chamberName2, pos_rot2, zi2, zo2, ri2, ro2, wi2, wo2, dphi2, shift2, phiIndex2);

                                if (chamberName != chamberName2
                                      || !equalAngle(dphi, dphi2)
                                      || !equalLength(shift, shift2)
                                      || !equalLength(ri, ri2)
                                      || !equalLength(ro, ro2)
                                      || !equalLength(wi, wi2)
                                      || !equalLength(wo, wo2)) {
                                   // This is a different chamber.
                                   // Reset the new chamber flag so that it can be processed as a new chamber in the next loop
                                   newChamber = true;
                                }
                                else if (pos_rot.distance(pos_rot2) < m_smallDistance
                                      && equalLength(zi, zi2)
                                      && equalLength(zo, zo2)
                                      ) {
                                   // same chamber in different phi sector, add it to the existing phi index list
                                   std::string stationPhi = DataType(phiIndex2).toString();
                                   if (phiString.find(stationPhi) == std::string::npos) phiString += " " + stationPhi;
                                   pvnswsub.next();
                                }
                                else if (pos_rot.distance(HepGeom::Point3D<double>(pos_rot2.x(), pos_rot2.y(), -pos_rot2.z())) < m_smallDistance
                                      && equalLength(zi, -zi2)
                                      && equalLength(zo, -zo2)
                                      ) {
                                   // same chamber in oppposite eta region, add it to a separate phi index list
                                   std::string stationPhi = DataType(phiIndex2).toString();
                                   if (phiString_mirrorEta.find(stationPhi) == std::string::npos) phiString_mirrorEta += " " + stationPhi;
                                   pvnswsub.next();
                                }
                                else {
                                   // This is a different chamber.
                                   // Reset the new chamber flag so that it can be processed as a new chamber in the next loop
                                   newChamber = true;
                                }

                             } // end of processing another chamber and comparing it to the first chamber


                             if (phiString!="" && (newChamber || pvnswsub.atEnd())){
                                // if the next chamber is a different chamber, or this is the last chamber, write the geometry to output
                                ATH_MSG_DEBUG( "Writing " << chamberName );

                                std::string stationTech = "MM";
                                std::string stationName = "MM"+chamberName.substr(7,1); // MMS: small sector. MML: large sector
                                int eta = std::stoi(chamberName.substr(8,1));
                                writeATrd(out, stationTech, stationName, zi, zo, ri, ro, wi, wo, eta, phiString, dphi, shift, 0);
                                if (phiString_mirrorEta!="") {
                                   writeATrd(out, stationTech, stationName, zi, zo, ri, ro, wi, wo, -eta, phiString_mirrorEta, dphi, shift, 0);
                                }

                                phiString = ""; // reset for new chambers
                                phiString_mirrorEta = ""; // reset for new chambers
                             }
                          }
                          else { // not MicroMegas; Move to the next chamber
                             pvnswsub.next();
                          }

                       }
                    }

                    // Process sTGC chambers
                    else if (stationName=="NSW_sTGC"){

                       ATH_MSG_DEBUG( "Processing NSW sTGC chambers." );

                       GeoVolumeCursor pvnswsub(pvnsw.getVolume());
                       bool newChamber = true;
                       std::string phiString = "";
                       std::string phiString_mirrorEta = "";
                       int nvtx=0;
                       double dz=0, dphi=0, shift=0;
                       std::string chamberName="";
                       HepGeom::Point3D<double> pos_rot;
                       const GeoSimplePolygonBrep* theBrep = nullptr;

                       while (!pvnswsub.atEnd()){
                          if (((pvnswsub.getVolume()->getLogVol())->getShape())->typeID() == GeoShapeShift::getClassTypeID() ) {// sTGC


                             if (newChamber){

                                int phiIndex;
                                readNSWSTGCPars(&pvnswsub, maxPhi, chamberName, pos_rot, theBrep, nvtx, dz, dphi, shift, phiIndex);
                                phiString = DataType(phiIndex).toString();

                                newChamber = false;
                                pvnswsub.next();
                             } // end of processing the first chamber

                             else{

                                std::string chamberName2;
                                HepGeom::Point3D<double> pos_rot2;
                                const GeoSimplePolygonBrep* theBrep2;
                                int nvtx2, phiIndex2;
                                double dz2, dphi2, shift2;
                                readNSWSTGCPars(&pvnswsub, maxPhi, chamberName2, pos_rot2, theBrep2, nvtx2, dz2, dphi2, shift2, phiIndex2);

                                // Check if it is the same shape as the first chamber
                                bool isSameShape = true;
                                if (nvtx == nvtx2 && equalLength(dz, dz2) ) { // Same Nvtx and thickness. Check vertices coordinates.
                                   for (int i=0; i<nvtx; ++i){
                                      if ( !equalLength(theBrep->getXVertex(i), theBrep2->getXVertex(i))
                                            || !equalLength(theBrep->getYVertex(i), theBrep2->getYVertex(i)) )
                                      {
                                         isSameShape = false;
                                      }
                                   }
                                }
                                else { // Different Nvtx or thickness
                                   isSameShape = false;
                                }

                                // Check if it has the same name, offset and shape as the first chamber
                                if (chamberName != chamberName2
                                      || !equalAngle(dphi, dphi2)
                                      || !equalLength(shift, shift2)
                                      || !isSameShape)
                                {
                                   // This is a different chamber.
                                   // Reset the new chamber flag so that it can be processed as a new chamber in the next loop
                                   newChamber = true;
                                }
                                // check chamber position
                                else if (pos_rot.distance(pos_rot2) < m_smallDistance) {
                                   // same chamber in different phi sector, add it to the existing phi index list
                                   std::string stationPhi = DataType(phiIndex2).toString();
                                   if (phiString.find(stationPhi) == std::string::npos) phiString += " " + stationPhi;
                                   pvnswsub.next();
                                }
                                else if (pos_rot.distance(HepGeom::Point3D<double>(pos_rot2.x(), pos_rot2.y(), -pos_rot2.z())) < m_smallDistance) {
                                   // same chamber in oppposite eta region, add it to a separate phi index list
                                   std::string stationPhi = DataType(phiIndex2).toString();
                                   if (phiString_mirrorEta.find(stationPhi) == std::string::npos) phiString_mirrorEta += " " + stationPhi;
                                   pvnswsub.next();
                                }
                                else {
                                   // This is a different chamber.
                                   // Reset the new chamber flag so that it can be processed as a new chamber in the next loop
                                   newChamber = true;
                                }
                             } // end of processing another chamber and comparing it to the first chamber


                             if (phiString!="" && (newChamber || pvnswsub.atEnd())){
                                // if the next chamber is a different chamber, or this is the last chamber, write the geometry to output
                                ATH_MSG_DEBUG( "Writing " << chamberName );

                                std::string stationTech = "STGC";
                                std::string stationName = "ST"+chamberName.substr(8,1); // STS: small sector. STL: large sector
                                int eta = std::stoi(chamberName.substr(9,1));
                                double signed_dz = dz;
                                if (pos_rot.z()<0) dz *= -1;
                                double zi = pos_rot.z() - signed_dz;
                                double zo = pos_rot.z() + signed_dz;
                                double rho = pos_rot.perp();
                                double ri, ro, wi, wo;

                                if (nvtx==4){ // write as a single ATrd
                                   // vtx1-----vtx0 (outer)
                                   //   \       /
                                   //  vtx2--vtx3 (inner)
                                   const int vtxList[] = {0, 1, 2, 3};
                                   readBrepAsATrd(theBrep, rho, vtxList, ri, ro, wi, wo);
                                   writeATrd(out, stationTech, stationName, zi, zo, ri, ro, wi, wo, eta, phiString, dphi, shift, 0);
                                   if (phiString_mirrorEta!="") {
                                      writeATrd(out, stationTech, stationName, zi, zo, ri, ro, wi, wo, -eta, phiString_mirrorEta, dphi, shift, 0);
                                   }
                                }

                                else if (nvtx==6){ // print as two ATrds
                                   // vtx1-----vtx0 (outer)
                                   //  |         |
                                   // vtx2     vtx5
                                   //   \       /
                                   //  vtx3--vtx4 (inner)

                                   // First ATrd (inner part): vertex 2, 3, 4, 5
                                   const int vtxList1[] = {5, 2, 3, 4};
                                   readBrepAsATrd(theBrep, rho, vtxList1, ri, ro, wi, wo);
                                   writeATrd(out, stationTech, stationName, zi, zo, ri, ro, wi, wo, eta, phiString, dphi, shift, 0);
                                   if (phiString_mirrorEta!="") {
                                      writeATrd(out, stationTech, stationName, zi, zo, ri, ro, wi, wo, -eta, phiString_mirrorEta, dphi, shift, 0);
                                   }

                                   // Second ATrd (outter part): vertex 0, 1, 2, 5
                                   const int vtxList2[] = {0, 1, 2, 5};
                                   readBrepAsATrd(theBrep, rho, vtxList2, ri, ro, wi, wo);
                                   writeATrd(out, stationTech, stationName, zi, zo, ri, ro, wi, wo, eta, phiString, dphi, shift, 0);
                                   if (phiString_mirrorEta!="") {
                                      writeATrd(out, stationTech, stationName, zi, zo, ri, ro, wi, wo, -eta, phiString_mirrorEta, dphi, shift, 0);
                                   }

                                }

                                else{
                                   ATH_MSG_ERROR( "Shape not supported by GeometryJiveXML: polygon shape with "<<nvtx <<" verticies in NSW sTGC." );
                                }
                                phiString = ""; // reset for new chambers
                                phiString_mirrorEta = ""; // reset for new chambers
                             }

                          }
                          else { // not sTGC; Move to the next chamber
                             pvnswsub.next();
                          }

                       }
                    }
                    pvnsw.next();
                 }
                 return; // Done with NSW. Skip other subdetectors
              }

              pv.next();
           }
        }
        av.next(); // increment volume cursor.
     }
  }

  int MuonGeometryWriter::getPhiIndex(double phi, int maxPhi) const {
     if (phi<0) phi += 2.*M_PI;
     int phiIndex = std::round(phi * maxPhi / (2.*M_PI) - 0.1) + 1;
     return phiIndex;
  }

  HepGeom::Point3D<double> MuonGeometryWriter::getPositionNSW(Amg::Vector3D pos, int maxPhi) const {
     // get phi index of the sector
     int phiIndex = getPhiIndex(pos.phi(), maxPhi);
     // calculate phi of the sector center
     double sectorPhi = 2.*M_PI * ((double) phiIndex-1.) / maxPhi;
     // rotate to first sector
     HepGeom::Point3D<double> pos_rot = HepGeom::RotateZ3D(-sectorPhi) * HepGeom::Point3D<double>(pos.x(), pos.y(), pos.z());

     return pos_rot;
  }

  void MuonGeometryWriter::readNSWMMPars(const GeoVolumeCursor *pv, int maxPhi, std::string& chamberName, HepGeom::Point3D<double>& pos_rot,
        double& zi, double& zo, double& ri, double& ro, double& wi, double& wo, double& dphi, double& shift, int& phiIndex) const {

     chamberName = pv->getVolume()->getLogVol()->getName();
     const GeoTrd* theTrd = dynamic_cast<const GeoTrd*> ((pv->getVolume()->getLogVol())->getShape());
     Amg::Vector3D pos = pv->getTransform().translation();

     pos_rot = getPositionNSW(pos, maxPhi);
     dphi = getDeltaPhi(pos_rot, maxPhi);
     shift = getShift(pos_rot, dphi);
     double signed_dz = theTrd->getXHalfLength1();
     if (pos_rot.z()<0) signed_dz *= -1;

     zi = pos_rot.z() - signed_dz;
     zo = pos_rot.z() + signed_dz;
     ri = pos_rot.perp() - theTrd->getZHalfLength();
     ro = pos_rot.perp() + theTrd->getZHalfLength();
     wi = 2.0 * theTrd->getYHalfLength1();
     wo = 2.0 * theTrd->getYHalfLength2();

     phiIndex = getPhiIndex(pos.phi(), maxPhi);

     return;
  }

  void MuonGeometryWriter::readNSWSTGCPars(const GeoVolumeCursor *pv, int maxPhi,
        std::string& chamberName, HepGeom::Point3D<double>& pos_rot, const GeoSimplePolygonBrep*& theBrep,
        int& nvtx, double& dz, double& dphi, double& shift, int& phiIndex) const {

     chamberName = pv->getVolume()->getLogVol()->getName();

     const GeoShapeShift* theShift = dynamic_cast<const GeoShapeShift*> ((pv->getVolume()->getLogVol())->getShape());
     theBrep = dynamic_cast<const GeoSimplePolygonBrep*> (theShift->getOp());
     nvtx = theBrep->getNVertices();
     dz = theBrep->getDZ();

     Amg::Vector3D pos = pv->getTransform().translation();
     pos_rot = getPositionNSW(pos, maxPhi);
     dphi = getDeltaPhi(pos_rot, maxPhi);
     shift = getShift(pos_rot, dphi);

     phiIndex = getPhiIndex(pos.phi(), maxPhi);

     return;
  }

  void MuonGeometryWriter::readBrepAsATrd(const GeoSimplePolygonBrep* theBrep, double rho, const int *vtx,
        double& ri, double& ro, double& wi, double& wo) const {
     // vtx1-----vtx0 (outer)
     //   \       /
     //  vtx2--vtx3 (inner)
     ri = rho + theBrep->getYVertex(vtx[3]);
     ro = rho + theBrep->getYVertex(vtx[0]);
     wi = theBrep->getXVertex(vtx[3]) - theBrep->getXVertex(vtx[2]);
     wo = theBrep->getXVertex(vtx[0]) - theBrep->getXVertex(vtx[1]);
     return;
  }

  bool MuonGeometryWriter::equalAngle(double a, double b) const {
     return std::abs(a - b) < m_smallAngle;
  }

  bool MuonGeometryWriter::equalLength(double a, double b) const {
     return std::abs(a - b) < m_smallDistance;
  }

}
