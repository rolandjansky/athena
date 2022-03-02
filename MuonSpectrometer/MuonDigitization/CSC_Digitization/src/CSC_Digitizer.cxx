/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Author: Ketevi A. Assamagan
// BNL, February 19 2004 : move to MuonGeoModel
// Digitization routine for the CSC hits
#include "CSC_Digitization/CSC_Digitizer.h"

#include "AtlasHepMC/GenParticle.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGamma.h"
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandomEngine.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "StoreGate/StoreGateSvc.h"

using namespace MuonGM;

// Constructors
CSC_Digitizer::CSC_Digitizer(CscHitIdHelper* cscHitHelper, const MuonDetectorManager* muonMgr, ICscCalibTool* pcalib) :
    m_cscHitHelper{cscHitHelper}, m_muonMgr{muonMgr}, m_cscIdHelper{m_muonMgr->cscIdHelper()}, m_pcalib{pcalib} {
    /// CSL / CSS
    m_stationDict['S'] = m_cscIdHelper->stationNameIndex("CSS");
    m_stationDict['L'] = m_cscIdHelper->stationNameIndex("CSL");
}

// Public methods
StatusCode CSC_Digitizer::initialize() {
    // initialize random number generators
    // then initialize the CSC identifier helper
    // This method must be called before looping over the hits
    // to digitize them using the method digitize_hit below
    m_bunchTime = 0.0;

    // calculate the probability densities
    static constexpr std::array<double, s_maxElectron> prob{79.4, 12., 3.4,  1.6,  0.95, 0.6,   0.44,  0.34,  0.27,  0.21, 0.17,
                                                            0.13, 0.1, 0.08, 0.06, 0.05, 0.042, 0.037, 0.033, 0.029, 0.};

    double sump = 0;
    for (int i = 0; i < s_maxElectron; ++i) {
        sump += prob[i];
        m_sprob[i] = sump;
    }

    return StatusCode::SUCCESS;
}

Identifier CSC_Digitizer::to_identifier(const CSCSimHit* cscHit) const {
    const int hitId = cscHit->CSCid();
    const std::string st_str = m_cscHitHelper->GetStationName(hitId);
    std::map<char, int>::const_iterator itr = m_stationDict.find(st_str[2]);
    if (itr == m_stationDict.end()) {
        throw std::runtime_error(Form("%s:%d Failed to convert station name %s to an valid offline identifier", __FILE__, __LINE__,
                                      st_str.c_str()));
    }
    const int stationName = itr->second;
    const int eta = m_cscHitHelper->GetZSector(hitId);
    const int phi = m_cscHitHelper->GetPhiSector(hitId);
    const int chamberLayer = m_cscHitHelper->GetChamberLayer(hitId);
    const int wireLayer = m_cscHitHelper->GetWireLayer(hitId);
    return m_cscIdHelper->channelID(stationName, eta, phi, chamberLayer, wireLayer, 0, 1, false);
}

StatusCode CSC_Digitizer::digitize_hit(const CSCSimHit* cscHit, std::vector<IdentifierHash>& hashVec,
                                       std::map<IdentifierHash, std::vector<float>>& data_SampleMap,
                                       std::map<IdentifierHash, std::vector<float>>& data_SampleMapOddPhase,
                                       CLHEP::HepRandomEngine* rndmEngine) {
    // method to digitize a single hit. Must be called in a loop over hits
    // after a call to the method "StatusCode CSC_Digitize::initialize()"
    // get the step vector from the begining and end vectors of the G4 step

    Amg::Vector3D startHit = cscHit->getHitStart();
    Amg::Vector3D endHit = cscHit->getHitEnd();

    // decode the hit ID
    const Identifier HitId = to_identifier(cscHit);
    const int chamberLayer = m_cscHitHelper->GetChamberLayer(cscHit->CSCid());

    // find the detector descriptor
    const CscReadoutElement* descriptor = m_muonMgr->getCscReadoutElement(HitId);
    if (!descriptor) {
        std::cout << "CSC_Digitizer::ERROR : Cannot get descriptor for GeoModel = " << std::endl;
        return StatusCode::FAILURE;
    }

    // particle ID
    int ipart = cscHit->particleID();

    Amg::Vector3D stepHit = endHit - startHit;
    double step = stepHit.mag();  // path length in a layer

    if (step == 0) return StatusCode::SUCCESS;

    // get average number of interaction::one interaction for photons
    int nInter = 0;
    if (ipart == 1)
        nInter = 1;
    else {
        if (m_NInterFromEnergyLoss) {
            double energyLoss = cscHit->energyDeposit();  // MeV peaking at 0.001MeV for on time track (25ns from IP)
            double elecEnergy = m_electronEnergy *
                                0.000001;  // each ionized electron has 66eV (66 is from 0.001MeV/15 (average interaction per layer 0.5cm)
            nInter = energyLoss / elecEnergy;
        } else {
            double average_int = 30;  // average interactions per cm
            double pois = CLHEP::RandPoisson::shoot(rndmEngine, average_int);
            nInter = int(step * pois / 10.0 + 0.5);  //  number of interaction according to Poisson
            if (m_debug)
                std::cout << "[CSC_Digitizer::digitize_hit(NEW)] nInter info from random number pois:step:nInter  " << pois << " " << step
                          << " " << nInter << std::endl;
        }
    }

    if (nInter <= 0) return StatusCode::SUCCESS;

    double wireCharge{0.};
    int nElectrons{0};

    // loop over number of interactions and do digitization
    for (int i = 0; i < nInter; ++i) {
        double t = 0.0;
        if (ipart == 1 || ipart == 999)
            t = step * 0.50;  // one interaction for photons & geantinos
        else
            t = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0);  // for other particles

        const Amg::Vector3D pos_vec = startHit + t * stepHit;

        // pileup - check if within time window
        double preDriftTime = getDriftTime(descriptor, pos_vec);  // before geant hit time addition.
        double driftTime = m_bunchTime + cscHit->meanTime() + preDriftTime;

        if (m_debug) {
            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] totTime:driftTime:hitTime:bunchTime (ns) = " << driftTime << " "
                      << cscHit->meanTime() << " " << preDriftTime << " " << m_bunchTime << std::endl;
            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] start hit coordinates (xc,yc,zc) / step hit and t / the hit are " << startHit
                      << "    " << stepHit << "   " << t << "    " << pos_vec << std::endl;
        }

        // Prompt muon case::
        // CSL 25.02ns   CSS 26.3ns :: Dec03 2009  geant hit time was not added but it's added now.
        // so previous +- 50ns time window may not work. We need to tune it. Temporally, I put -inf +inf.

        // it should be decided by bunchtime. -400 to +200
        if (outsideWindow(m_bunchTime)) continue;

        // number of electrons in this interaction
        double flat = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0);
        double p = m_sprob[s_maxElectron - 1] * flat;
        for (int k = 0; k < s_maxElectron; ++k) {
            if (p <= m_sprob[k]) {
                nElectrons = k + 1;
                break;
            }
        }

        // find the charge on the wire in electron-equivalent charge

        m_Polia = 0.38;  // parmeter for charge loss:: random gamma function
        double gammaDist = CLHEP::RandGamma::shoot(rndmEngine, (1.0 + m_Polia), 1.0);
        wireCharge = qWire(nElectrons, gammaDist);

        if (m_debug) {
            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] flat:p:maxEle:nEle:gammaDist:wireCharge " << flat << " " << p << " "
                      << s_maxElectron << " " << nElectrons << " " << gammaDist << " " << wireCharge << std::endl;
        }

        // now digitize hits for R- and phi-strips
        for (int measuresPhi = 0; measuresPhi <= 1; ++measuresPhi) {
            float stripWidth = descriptor->cathodeReadoutPitch(chamberLayer, measuresPhi);
            const int maxStrip = descriptor->maxNumberOfStrips(measuresPhi, stripWidth);

            if (m_debug) {
                std::cout << "[CSC_Digitizer::digitize_hit(NEW)] CSC_Digitizer::digitize_hit: Chamber Parameters"
                          << " measuresPhi= " << measuresPhi << " stripWidth= " << stripWidth << " maxStrip= " << maxStrip << std::endl;
            }

            // now digitize hits for R-strips/phi-strips

            if (measuresPhi == 0) {                                     // R-strips :: find the charge+noise
                double zz = pos_vec.z() + maxStrip * stripWidth * 0.50;  // assuming zero offset
                int strip = int(zz / stripWidth) + 1;
                // find the charges induced on the phi strips
                for (int j = strip - 4; j <= strip + 4; ++j) {
                    double zpos = stripWidth * j - stripWidth * 0.50;
                    if (j > 0 && j <= maxStrip) {
                        double stripCharge = wireCharge * qStripR((zpos - zz) / stripWidth, HitId) * 0.50;
                        //	    if (stripCharge < 0) stripCharge = 0.0;

                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] zpos = " << zpos << " zz = " << zz
                                      << " diff = " << std::abs(zz - zpos) << " strip = " << j << " charge = " << stripCharge << std::endl;

                        IdentifierHash hashId = getHashId(HitId, j, measuresPhi);
                        fillSampleMaps(hashId, driftTime, stripCharge, hashVec, data_SampleMap);
                        fillSampleMaps(hashId, driftTime, stripCharge, hashVec, data_SampleMapOddPhase, 1);
                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] DriftTimeVSstripCharge " << driftTime << " " << stripCharge
                                      << std::endl;
                    }
                }
            } else {  // Phi-strips :: find the charge+noise
                double yy = 0.0;
                int strip = 0;

                yy = pos_vec.y() + maxStrip * stripWidth * 0.5;  // non-pointing phi-strips :: assuming zero offset
                if (m_cscHitHelper->GetZSector(cscHit->CSCid()) > 0)
                    yy = -pos_vec.y() + maxStrip * stripWidth * 0.5;  // non-pointing phi-strips :: assuming zero offset
                strip = int(yy / stripWidth) + 1;

                // find the charges induced on the phi strips
                for (int j = strip - 1; j <= strip + 1; ++j) {
                    if (j > 0 && j <= maxStrip) {
                        double ypos = stripWidth * (j - 0.5);
                        double stripCharge = wireCharge * qStripPhi(ypos - yy, HitId) * 0.50;
                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] ypos = " << ypos << " yy = " << yy
                                      << " diff = " << std::abs(yy - ypos) << " strip = " << j << " charge = " << stripCharge << std::endl;

                        IdentifierHash hashId = getHashId(HitId, j, measuresPhi);
                        fillSampleMaps(hashId, driftTime, stripCharge, hashVec, data_SampleMap);
                        fillSampleMaps(hashId, driftTime, stripCharge, hashVec, data_SampleMapOddPhase, 1);
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode CSC_Digitizer::digitize_hit(const CSCSimHit* cscHit, std::vector<IdentifierHash>& hashVec,
                                       std::map<IdentifierHash, std::vector<float>>& data_SampleMap, CLHEP::HepRandomEngine* rndmEngine) {
    // method to digitize a single hit. Must be called in a loop over hits
    // after a call to the method "StatusCode CSC_Digitize::initialize()"
    // get the step vector from the begining and end vectors of the G4 step
    Amg::Vector3D startHit = cscHit->getHitStart();
    Amg::Vector3D endHit = cscHit->getHitEnd();

    // decode the hit ID
    const Identifier HitId = to_identifier(cscHit);
    const int chamberLayer = m_cscHitHelper->GetChamberLayer(cscHit->CSCid());

    // find the detector descriptor
    const CscReadoutElement* descriptor = m_muonMgr->getCscReadoutElement(HitId);
    if (!descriptor) {
        std::cout << "CSC_Digitizer::ERROR : Cannot get descriptor for GeoModel = " << std::endl;
        return StatusCode::FAILURE;
    }

    // particle ID
    int ipart = cscHit->particleID();

    Amg::Vector3D stepHit = endHit - startHit;
    double step = stepHit.mag();

    if (step == 0) return StatusCode::SUCCESS;

    // get average number of interaction::one interaction for photons
    //  std::cout << "The particle ID = " << ipart << std::endl;

    int nInter = 0;
    if (ipart == 1)
        nInter = 1;
    else {
        double average_int = 30;  // average interactions per cm
        double pois = CLHEP::RandPoisson::shoot(rndmEngine, average_int);
        nInter = int(step * pois / 10.0 + 0.5);  //  number of interaction according to Poisson
        if (m_debug)
            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] nInter info from random number pois:step:nInter  " << pois << " " << step
                      << " " << nInter << std::endl;
    }

    if (nInter <= 0) return StatusCode::SUCCESS;

    double wireCharge{0};
    int nElectrons = 0;

    // loop over number of interactions and do digitization
    for (int i = 0; i < nInter; ++i) {
        double t = 0.0;
        if (ipart == 1 || ipart == 999)
            t = step * 0.50;  // one interaction for photons & geantinos
        else
            t = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0);  // for other particles

        Amg::Vector3D pos_vec = startHit + t * stepHit;

        // pileup - check if within time window
        double preDriftTime = getDriftTime(descriptor, pos_vec);  // before geant hit time addition.
        double driftTime = m_bunchTime + cscHit->meanTime() + preDriftTime;

        if (m_debug) {
            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] totTime:driftTime:hitTime:bunchTime (ns) = " << driftTime << " "
                      << cscHit->meanTime() << " " << preDriftTime << " " << m_bunchTime << std::endl;
            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] start hit coordinates (xc,yc,zc) / step hit and t / the hit are " << startHit
                      << "    " << stepHit << "   " << t << "    " << pos_vec << std::endl;
        }

        // Prompt muon case::
        // CSL 25.02ns   CSS 26.3ns :: Dec03 2009  geant hit time was not added but it's added now.
        // so previous +- 50ns time window may not work. We need to tune it. Temporally, I put -inf +inf.

        // it should be decided by bunchtime. -400 to +200
        if (outsideWindow(m_bunchTime)) continue;

        // number of electrons in this interaction
        double flat = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0);
        double p = m_sprob[s_maxElectron - 1] * flat;
        for (int k = 0; k < s_maxElectron; ++k) {
            if (p <= m_sprob[k]) {
                nElectrons = k + 1;
                break;
            }
        }

        // find the charge on the wire in electron-equivalent charge

        m_Polia = 0.38;  // parmeter for charge loss:: random gamma function
        double gammaDist = CLHEP::RandGamma::shoot(rndmEngine, (1.0 + m_Polia), 1.0);
        wireCharge = qWire(nElectrons, gammaDist);

        if (m_debug) {
            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] flat:p:maxEle:nEle:gammaDist:wireCharge " << flat << " " << p << " "
                      << s_maxElectron << " " << nElectrons << " " << gammaDist << " " << wireCharge << std::endl;
        }

        // now digitize hits for R- and phi-strips
        for (int measuresPhi = 0; measuresPhi <= 1; ++measuresPhi) {
            float stripWidth = descriptor->cathodeReadoutPitch(chamberLayer, measuresPhi);
            const int maxStrip = descriptor->maxNumberOfStrips(measuresPhi, stripWidth);

            if (m_debug) {
                std::cout << "[CSC_Digitizer::digitize_hit(NEW)] CSC_Digitizer::digitize_hit: Chamber Parameters"
                          << " measuresPhi= " << measuresPhi << " stripWidth= " << stripWidth << " maxStrip= " << maxStrip << std::endl;
            }

            // now digitize hits for R-strips/phi-strips

            if (measuresPhi == 0) {                                     // R-strips :: find the charge+noise
                double zz = pos_vec.z() + maxStrip * stripWidth * 0.50;  // assuming zero offset
                int strip = int(zz / stripWidth) + 1;
                // find the charges induced on the phi strips
                for (int j = strip - 4; j <= strip + 4; ++j) {
                    double zpos = stripWidth * j - stripWidth * 0.50;
                    if (j > 0 && j <= maxStrip) {
                        double stripCharge = wireCharge * qStripR((zpos - zz) / stripWidth, HitId) * 0.50;

                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] zpos = " << zpos << " zz = " << zz
                                      << " diff = " << std::abs(zz - zpos) << " strip = " << j << " charge = " << stripCharge << std::endl;

                        IdentifierHash hashId = getHashId(HitId, j, measuresPhi);
                        fillSampleMaps(hashId, driftTime, stripCharge, hashVec, data_SampleMap);

                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] DriftTimeVSstripCharge " << driftTime << " " << stripCharge
                                      << std::endl;
                    }
                }
            } else {  // Phi-strips :: find the charge+noise
                double yy = 0.0;
                int strip = 0;

                yy = pos_vec.y() + maxStrip * stripWidth * 0.5;  // non-pointing phi-strips :: assuming zero offset
                if (m_cscHitHelper->GetZSector(cscHit->CSCid()) > 0)
                    yy = -pos_vec.y() + maxStrip * stripWidth * 0.5;  // non-pointing phi-strips :: assuming zero offset
                strip = int(yy / stripWidth) + 1;

                // find the charges induced on the phi strips
                for (int j = strip - 1; j <= strip + 1; j++) {
                    if (j > 0 && j <= maxStrip) {
                        double ypos = stripWidth * (j - 0.5);
                        double stripCharge = wireCharge * qStripPhi(ypos - yy, HitId) * 0.50;
                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit(NEW)] ypos = " << ypos << " yy = " << yy
                                      << " diff = " << std::abs(yy - ypos) << " strip = " << j << " charge = " << stripCharge << std::endl;

                        IdentifierHash hashId = getHashId(HitId, j, measuresPhi);
                        fillSampleMaps(hashId, driftTime, stripCharge, hashVec, data_SampleMap);
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

////////////////////////////////////////////
/// Old way before 10/2010
//////////////////
StatusCode CSC_Digitizer::digitize_hit(const CSCSimHit* cscHit, std::vector<IdentifierHash>& hashVec,
                                       std::map<IdentifierHash, std::pair<double, double>>& data_map, CLHEP::HepRandomEngine* rndmEngine) {
    // method to digitize a single hit. Must be called in a loop over hits
    // after a call to the method "StatusCode CSC_Digitize::initialize()"
    // get the step vector from the begining and end vectors of the G4 step
    Amg::Vector3D startHit = cscHit->getHitStart();
    Amg::Vector3D endHit = cscHit->getHitEnd();

    // decode the hit ID
    const Identifier HitId = to_identifier(cscHit);
    const int chamberLayer = m_cscHitHelper->GetChamberLayer(cscHit->CSCid());
    // find the detector descriptor
    const CscReadoutElement* descriptor = m_muonMgr->getCscReadoutElement(HitId);
    if (!descriptor) {
        std::cout << "CSC_Digitizer::ERROR : Cannot get descriptor for GeoModel = " << std::endl;
        return StatusCode::FAILURE;
    }

    // particle ID
    int ipart = cscHit->particleID();

    Amg::Vector3D stepHit = endHit - startHit;
    double step = stepHit.mag();

    if (step == 0) return StatusCode::SUCCESS;

    // get average number of interaction::one interaction for photons
    //  std::cout << "The particle ID = " << ipart << std::endl;

    int nInter = 0;
    if (ipart == 1)
        nInter = 1;
    else {
        double average_int = 30;  // average interactions per cm
        double pois = CLHEP::RandPoisson::shoot(rndmEngine, average_int);
        nInter = int(step * pois / 10.0 + 0.5);  //  number of interaction according to Poisson
        if (m_debug)
            std::cout << "[CSC_Digitizer::digitize_hit] nInter info from random number pois:step:nInter  " << pois << " " << step << " "
                      << nInter << std::endl;
    }

    if (nInter <= 0) return StatusCode::SUCCESS;

    double wireCharge;
    int nElectrons = 0;

    // loop over number of interactions and do digitization
    for (int i = 0; i < nInter; ++i) {
        double t = 0.0;
        if (ipart == 1 || ipart == 999)
            t = step * 0.50;  // one interaction for photons & geantinos
        else
            t = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0);  // for other particles

        const Amg::Vector3D pos_vec = startHit + t * stepHit;
        // pileup - check if within time window
        double preDriftTime = getDriftTime(descriptor, pos_vec);  // before geant hit time addition.
        double driftTime = m_bunchTime + cscHit->meanTime() + preDriftTime;

        if (m_debug) {
            std::cout << "[CSC_Digitizer::digitize_hit] totTime:driftTime:hitTime:bunchTime (ns) = " << driftTime << " "
                      << cscHit->meanTime() << " " << preDriftTime << " " << m_bunchTime << std::endl;
            std::cout << "[CSC_Digitizer::digitize_hit] start hit coordinates (xc,yc,zc) / step hit and t / the hit are " << startHit
                      << "    " << stepHit << "   " << t << "    " << pos_vec << std::endl;
        }

        // Prompt muon case::
        // CSL 25.02ns   CSS 26.3ns :: Dec03 2009  geant hit time was not added but it's added now.
        // so previous +- 50ns time window may not work. We need to tune it. Temporally, I put -inf +inf.

        // it should be decided by bunchtime. -400 to +200
        if (outsideWindow(m_bunchTime)) continue;

        // number of electrons in this interaction
        double flat = CLHEP::RandFlat::shoot(rndmEngine, 0.0, 1.0);
        double p = m_sprob[s_maxElectron - 1] * flat;
        for (int k = 0; k < s_maxElectron; ++k) {
            if (p <= m_sprob[k]) {
                nElectrons = k + 1;
                break;
            }
        }

        // find the charge on the wire in electron-equivalent charge
        m_Polia = 0.38;  // parmeter for charge loss:: random gamma function
        double gammaDist = CLHEP::RandGamma::shoot(rndmEngine, (1.0 + m_Polia), 1.0);
        wireCharge = qWire(nElectrons, gammaDist);

        if (m_debug) {
            std::cout << "[CSC_Digitizer::digitize_hit] flat:p:maxEle:nEle:gammaDist:wireCharge " << flat << " " << p << " "
                      << s_maxElectron << " " << nElectrons << " " << gammaDist << " " << wireCharge << std::endl;
        }

        // now digitize hits for R- and phi-strips
        for (int measuresPhi = 0; measuresPhi <= 1; ++measuresPhi) {
            float stripWidth = descriptor->cathodeReadoutPitch(chamberLayer, measuresPhi);
            const int maxStrip = descriptor->maxNumberOfStrips(measuresPhi, stripWidth);

            // now digitize hits for R-strips/phi-strips

            if (measuresPhi == 0) {                                     // R-strips :: find the charge+noise
                double zz = pos_vec.z() + maxStrip * stripWidth * 0.50;  // assuming zero offset
                int strip = int(zz / stripWidth) + 1;
                // find the charges induced on the phi strips

                for (int j = strip - 4; j <= strip + 4; j++) {
                    double zpos = stripWidth * j - stripWidth * 0.50;
                    if (j > 0 && j <= maxStrip) {
                        double stripCharge = wireCharge * qStripR((zpos - zz) / stripWidth, HitId) * 0.50;

                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit] zpos = " << zpos << " zz = " << zz
                                      << " diff = " << std::abs(zz - zpos) << " strip = " << j << " charge = " << stripCharge << std::endl;

                        IdentifierHash hashId = getHashId(HitId, j, measuresPhi);

                        fillMaps(hashId, driftTime, stripCharge, hashVec, data_map);

                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit] DriftTimeVSstripCharge " << driftTime << " " << stripCharge
                                      << std::endl;
                    }
                }
            } else {  // Phi-strips :: find the charge+noise
                double yy = 0.0;
                int strip = 0;

                yy = pos_vec.y() + maxStrip * stripWidth * 0.5;  // non-pointing phi-strips :: assuming zero offset
                if (m_cscHitHelper->GetZSector(cscHit->CSCid()) > 0)
                    yy = -pos_vec.y() + maxStrip * stripWidth * 0.5;  // non-pointing phi-strips :: assuming zero offset
                strip = int(yy / stripWidth) + 1;

                // find the charges induced on the phi strips
                for (int j = strip - 1; j <= strip + 1; ++j) {
                    if (j > 0 && j <= maxStrip) {
                        double ypos = stripWidth * (j - 0.5);
                        double stripCharge = wireCharge * qStripPhi(ypos - yy, HitId) * 0.50;
                        if (m_debug)
                            std::cout << "[CSC_Digitizer::digitize_hit] ypos = " << ypos << " yy = " << yy
                                      << " diff = " << std::abs(yy - ypos) << " strip = " << j << " charge = " << stripCharge << std::endl;

                        IdentifierHash hashId = getHashId(HitId, j, measuresPhi);

                        fillMaps(hashId, driftTime, stripCharge, hashVec, data_map);
                    }
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

// public methods
// 10/2010
// New way of adding charges...
// only care for samples...
// charges are represented by samples...
// and then in the end we need to convert them into one pair of time and charge (short term goal)
// Long term goal, we need to change EDM to carry over sample charges to RDOCollection...
void CSC_Digitizer::fillSampleMaps(const IdentifierHash hashId, const double driftTime, const double stripCharge,
                                   std::vector<IdentifierHash>& hashVec, std::map<IdentifierHash, std::vector<float>>& data_map,
                                   bool phase) {
    if (stripCharge == 0.0) return;

    // fill the CSC specific charge map the conversion hash -. id is done later !
    if (data_map.find(hashId) != data_map.end()) {
        float phaseOffset = (phase) ? +25 : 0;
        std::vector<float> samples = m_pcalib->getSamplesFromBipolarFunc(driftTime + phaseOffset, stripCharge);
        std::vector<float> existingSamples = data_map[hashId];  // assuming that there are four elements...

        if (m_debug) {
            std::cout << "[CSC_Digitizer::fillSampleMaps] ";

            for (unsigned int i = 0; i < samples.size(); ++i) std::cout << samples[i] << " ";
            std::cout << " + ";

            for (unsigned int i = 0; i < existingSamples.size(); ++i) std::cout << existingSamples[i] << " ";

            std::cout << "  ==>  ";
        }

        for (unsigned int i = 0; i < samples.size(); ++i) { existingSamples[i] += samples[i]; }

        if (m_debug) {
            for (unsigned int i = 0; i < existingSamples.size(); ++i) std::cout << existingSamples[i] << " ";
            std::cout << std::endl;
        }

        data_map[hashId] = existingSamples;

    } else {
        data_map.insert(std::pair<IdentifierHash, std::vector<float>>(hashId, m_pcalib->getSamplesFromBipolarFunc(driftTime, stripCharge)));
        hashVec.push_back(hashId);
    }
}

void CSC_Digitizer::setWindow(const double t1, const double t2) {
    m_timeWindowLowerOffset = t1;
    m_timeWindowUpperOffset = t2;   
}

void CSC_Digitizer::set(const double bunchTime) { m_bunchTime = bunchTime; }

// Private methods d

double CSC_Digitizer::qWire(const int& nElectrons, const double& gammaDist) const {
    // find the charge on the wire

    //  double amplification = 0.58e5;
    double amplification = m_amplification;
    double wireCharge = 0.0;

    for (int i = 0; i < nElectrons; ++i) {
        double RNDpol = 0.0;
        if (m_Polia > -1.0) { RNDpol = gammaDist / (1.0 + m_Polia); }
        wireCharge += amplification * RNDpol;
    }
    return wireCharge;
}

// old implementation before Jan 2012:
double CSC_Digitizer::qStripR(const double x) const {
    // induced charge on the r-strip
    // parametrization based on experimental data

    double rStripCharge = 0.0;
    static constexpr std::array<double,4> Par{0.185, 2.315, 0.48, 5.2};  // obtained from test beam
    double xx = std::abs(x) / 10.0;             // divide by 10 to convert to cm!

    rStripCharge = Par[0] * exp(-Par[1] * xx) + Par[2] * exp(-Par[3] * (xx * xx));

    return rStripCharge;
}

/** Charge sharing profile.
New implementation Jan 2012 from fit to segment hits in run 186049.
@param x distance between strip and hit center in strip widths
@param stationType small or large chamber
@return the charge fraction for this strip
*/
double CSC_Digitizer::qStripR(const double x, const Identifier& id) const {
    double stripCharge = 0.0;
    double xx = x * x;  // distance in strip widths squared

    if (m_cscIdHelper->isSmall(id)) {  // small chamber
        stripCharge = 0.7001 / (1.0 + 1.38 * xx + 2.255 * xx * xx);
    } else {  // large chamber
        stripCharge = 0.7 / (1.0 + 1.381 * xx + 2.248 * xx * xx);
    }
    return stripCharge;
}

double CSC_Digitizer::qStripPhi(const double x, const Identifier& id) const {
    // parameters obtained from test beam
    static constexpr std::array<double, 9> pSmall{.9092, .0634, -1.051, 8.05, -35.477, 58.883, -46.111, 17.446, -2.5824};
    static constexpr std::array<double, 9> pLarge{.98, -.36, 5.07, -27.81, 72.257, -99.456, 72.778, -26.779, 3.9054};
    double xx = std::abs(x) / 10.0;  // divide by 10 to convert to cm!
    double phiStripCharge = 0.0;

    if (xx < 1.75) {
        if (m_cscIdHelper->isSmall(id)) {
            phiStripCharge = fparamPhi(xx, pSmall);
        } else {
            phiStripCharge = fparamPhi(xx, pLarge);
        }
    }

    return phiStripCharge;
}

double CSC_Digitizer::fparamPhi(const double x, const std::array<double, 9>& p) const {
    double fparam{0}, pow {1.};
    for (size_t i = 0; i < p.size(); ++i) {
        fparam += p[i] * pow;
        pow *= x;
    }
    return fparam;
}

bool CSC_Digitizer::outsideWindow(double driftTime) const {
    // m_bunchTime is included already....updated one..
    return driftTime < m_timeWindowLowerOffset || driftTime > m_timeWindowUpperOffset;
}

double CSC_Digitizer::getDriftTime(const MuonGM::CscReadoutElement* descriptor, const Amg::Vector3D& pos) const {
    // need to be calculated correct - Garfield?
    // assumption on the field lines not good but of for pileup stuff!
    double time = -1000.0;
    // tested with 60, 45, 30 and 60 looks the best 12/7/2009
    double v0 = m_driftVelocity * 1.e-3;  // mm/microsecond   6 cm/microsecond -> mm/ns
    float anodeCathodeDist = descriptor->anodeCathodeDistance();
    float roxacellWidth = descriptor->roxacellWidth();
    float beta = std::atan((descriptor->longWidth() - descriptor->shortWidth()) / (2. * descriptor->lengthUpToMaxWidth()));
    float longWidth = descriptor->longWidth() - 2 * roxacellWidth * (1 + std::sin(beta)) / std::cos(beta);
    float yy = pos.y() + longWidth * 0.5;
    float x0 = pos.x();
    int wire = int(yy / anodeCathodeDist) + 1;
    float y0 = (yy - wire * anodeCathodeDist);
    if (std::abs(y0) > anodeCathodeDist / 2) y0 = std::abs(y0) - anodeCathodeDist * 0.5;
    float driftDist = std::hypot(x0, y0);  // Lorentz effect small
    time = driftDist / v0;
    return time;
}

// public methods
// old way to add electron charges...
// * represent electron charge with bipolar function
// * add two charges with bipolar functions
// * then extract charge and time out of function...
//
// Then, try to speed up (09/2010)
// but it turns out to be vulernable for pileup simulation...
void CSC_Digitizer::fillMaps(const IdentifierHash hashId, const double driftTime, const double stripCharge,
                             std::vector<IdentifierHash>& hashVec, std::map<IdentifierHash, std::pair<double, double>>& data_map) {
    if (stripCharge == 0.0) return;

    // fill the CSC specific charge map the conversion hash -. id is done later !
    if (data_map.find(hashId) != data_map.end()) {
        /*
         http://indico.cern.ch/getFile.py/access?contribId=9&resId=0&materialId=slides&confId=106902
        */
        std::pair<double, double> result = m_pcalib->addBipfunc(data_map[hashId].first, data_map[hashId].second, driftTime, stripCharge);
        //  To check out conversion is correct...
        if (m_debug)
            std::cout << "[CSC_Digitizer::fillMaps] (" << data_map[hashId].first << ":" << int(data_map[hashId].second) << ")"
                      << "+(" << driftTime << ":" << int(stripCharge) << ")"
                      << " ==> " << result.first << ":" << int(result.second) << " e-  which was "
                      << int(data_map[hashId].second + stripCharge) << std::endl;

        data_map[hashId].first = result.first;
        data_map[hashId].second = result.second;

    } else {
        data_map[hashId].first = driftTime;
        data_map[hashId].second = stripCharge;
        hashVec.push_back(hashId);
    }
}

IdentifierHash CSC_Digitizer::getHashId(const Identifier& input_id, const int stripId, const int measuresPhi) const {
    const int chamberLayer = m_cscIdHelper->chamberLayer(input_id);
    const int wireLayer = m_cscIdHelper->wireLayer(input_id);
    Identifier realHitId = m_cscIdHelper->channelID(input_id, chamberLayer, wireLayer, measuresPhi, stripId);
    IdentifierHash hashId;
    if (m_cscIdHelper->get_channel_hash(realHitId, hashId)) {
        throw std::runtime_error(
            Form("File: %s, Line: %d\nCSC_Digitizer::getHashId() - Failed to retrieve channel hash from identifier %llu", __FILE__,
                 __LINE__, realHitId.get_compact()));
    }
    return hashId;
}
