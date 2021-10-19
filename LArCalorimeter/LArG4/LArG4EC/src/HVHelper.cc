/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHV/LArHVManager.h"
#include "LArHV/EMECHVManager.h"
#include "LArHV/EMECHVModule.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECHVDescriptor.h"

#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

#include "HVHelper.h"

#include <iostream>

using namespace LArG4::EC;

const G4double HVHelper::s_EtaLimit[s_NofEtaSection + 1] = {
    1.375, 1.5, 1.6, 1.8, 2., 2.1, 2.3, 2.5, 2.8, 3.2
};

G4bool HVHelper::s_NeedToReadMaps = true;
G4double HVHelper::s_Values[s_NofAtlasSide][s_NofEtaSection][s_NofElectrodeSide][s_NofElectrodesOut];

G4int *HVHelperV00::s_StartPhi = nullptr;

HVHelper::HVHelper(
    const LArWheelCalculator *calc,
    const G4String &version
) :  AthMessaging(Gaudi::svcLocator()->service< IMessageSvc >("MessageSvc"),
                  "EMECHVHelper")
  , m_calculator(calc)
  , m_WheelShift(calc->GetElecFocaltoWRP() + calc->GetdWRPtoFrontFace())
  , m_NofPhiSections(
        (version == "v00" || version == "v01")? 32: calc->GetNumberOfFans()
    )
  , m_EtaMin(calc->GetisInner()? 8: 1)
  , m_EtaMax(calc->GetisInner()? 9: 7)
{
    ATH_MSG_DEBUG("asked for map version = " << version);
}

void HVHelper::AcquireMaps(const G4String &version, G4bool from_DB)
{
    if(s_NeedToReadMaps){
        ReadMapFromFile(version); // always have values from file as defaults
        s_NeedToReadMaps = false;
    }
    if(from_DB) GetMapFromDB(); // update them from DB if necessary
}

FILE * HVHelper::OpenFileAndCheckVersion(const G4String &version)
{
    const G4String mapPath     = "LArG4EC";
    const G4String mapFileName = "HVEMECMap_"+ version + ".dat";
    const G4String partialPath = mapPath + "/" + mapFileName;
    const G4String mapLocation =
        PathResolver::find_file(partialPath, "ATLASCALDATA");
    const G4String localFile = "HVEMECMap.dat";

    ATH_MSG_INFO("reading maps from file: " << mapLocation);

    FILE *F = fopen(mapLocation.c_str(), "r");
    if(F == 0){
        ATH_MSG_ERROR("cannot open EMEC HV Map file " << mapLocation);
        ATH_MSG_ERROR("trying to read the map from local file"
                      << localFile);
        F = fopen(localFile.c_str(), "r");
    }
    if(F == 0){
        ATH_MSG_ERROR("Cannot open map file " << localFile);
        ATH_MSG_FATAL("Cannot obtain HV maps");
        G4Exception("LArG4::EC::HVHelper", "NoHVMap", FatalException,
                    "ReadMapFromFile: cannot open file");
    }

    const size_t buf_size = 80;
    char buf[buf_size] = { 0 };
    fgets(buf, buf_size, F);
    char *v = buf + 9;
    if(version == "v02" || version == "v99") v ++;
    v[2] = 0;
    ATH_MSG_INFO("actual HV Map Version = " << buf);
    G4int iv = atoi(v);
    G4bool version_ok = false;
    if(version == "v00" && iv == 0)  version_ok = true;
    if(version == "v01" && iv == 1)  version_ok = true;
    if(version == "v02" && iv == 2)  version_ok = true;
    if(version == "v99" && iv == 99) version_ok = true; // this is a test file
    if(!version_ok){
        ATH_MSG_FATAL("field map version in the file (" << iv
                      << ") is different from expected ("
                      << version << ")");
        G4Exception("LArG4::EC::HVHelper", "MapVersionMismatch",
                    FatalException,
                    "ReadMapFromFile: map version mismatch");
    }
    return F;
}

void HVHelperV00::ReadMapFromFile(const G4String &version)
{
    if(version != "v00" && version != "v01" ){
        ATH_MSG_ERROR("wrong HV helper version");
        return;
    }
    FILE *F = OpenFileAndCheckVersion(version);
    for(G4int i = 0; i < s_NofAtlasSide; ++ i){
        const size_t buf_size = 80;
        char buf[buf_size] = { 0 };
        if(fscanf(F, "%79s", buf) < 1){
            ATH_MSG_ERROR("ReadMapFromFile: Error reading map file");
        }
        ATH_MSG_DEBUG("AtlasSide = " << buf);
        for(G4int j = 0; j < s_NofEtaSection; ++ j){
            for(G4int k = 0; k < s_NofElectrodeSide; ++ k){
                char buf1[buf_size] = { 0 };
                if(fscanf(F, "%79s%79s", buf, buf1) < 2){
                    ATH_MSG_ERROR("ReadMapFromFile: Error reading map file");
                }
                const G4String EtaSection = buf;
                const G4String ElectrodeSide = buf1;
                ATH_MSG_DEBUG("EtaSection = " << EtaSection
                            << " ElectrodeSide = " << ElectrodeSide);
                if(fscanf(F, "%i", &(StartPhi(i, j, k))) < 1){
                    ATH_MSG_ERROR("ReadMapFromFile: Error reading map file");
                }
                ATH_MSG_DEBUG("i, j, k = " << i << ", " << j << ", " << k
                            << " " <<" HV_Start_phi = " << StartPhi(i, j, k));
                for(G4int l = 0; l < m_NofPhiSections; ++ l){
                    if(fscanf(F, "%lg", &s_Values[i][j][k][l]) < 1){
                        ATH_MSG_ERROR("ReadMapFromFile: Error reading map file");
                    }
                    if(l == 0){
                        ATH_MSG_DEBUG("HV_Values = " << s_Values[i][j][k][l]);
                    }
                } // phi section
            } // electrode side
        } // eta section
    } // atlas side

    fclose(F);
}

void HVHelperV02::ReadMapFromFile(const G4String &version)
{
    if(version != "v02" && version != "v99"){
        ATH_MSG_ERROR("wrong HV helper version");
        return;
    }
    FILE *F = OpenFileAndCheckVersion(version);

    const size_t buf_size = 200;
    char buffer[buf_size];

// TODO: regularize printout when it will be necessary
    for(G4int i = 0; i < s_NofAtlasSide; ++ i){
        for(G4int j = 0; j < 3; ++ j){ // read header lines
            fgets(buffer, buf_size, F);
//            printf("%s", buffer);
        }
        for(G4int j = 0; j < s_NofElectrodesOut; ++ j){
            G4int electrodenumber = 0;
            if(fscanf(F, "%i", &electrodenumber) < 1){
                ATH_MSG_ERROR("ReadMapFromFile: Error reading map file");
            }
/*            if (j<iprmx || j==s_NofElectrodesOut-1) {
                printf("%3i",electrodenumber);
            }
            if (j==iprmx) {
                printf("...\n");
            }*/
            for(G4int k = 0; k < s_NofEtaSection; ++ k){
                for(G4int l = 0; l < s_NofElectrodeSide; ++ l){
                    if(fscanf(F, "%lg", &s_Values[i][k][l][j]) < 1){
                        ATH_MSG_ERROR("ReadMapFromFile: Error reading map file");
                    }
/*                    if(j <iprmx || j==s_NofElectrodesOut-1) {
                        printf("%8.2f", s_Values[i][k][l][j]);
                    }*/
                }
            }
/*            if(j<iprmx || j==s_NofElectrodesOut-1) {
                printf("\n");
            }*/
        }
        fgets(buffer, buf_size, F); // read leftover end of line
    }

    fclose(F);
}

void HVHelper::GetMapFromDB(void)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StoreGateSvc* pDetStore;

  StatusCode status = svcLocator->service("DetectorStore", pDetStore);
  if(status.isFailure()){
    ATH_MSG_WARNING("unable to get Detector Store! Use default HV values");
    return;
  }

  const G4bool isInner = lwc()->GetisInner();
  // get EMECHV Manager
  const LArHVManager *manager = 0;
  if(pDetStore->retrieve(manager) == StatusCode::SUCCESS){
    const EMECHVManager& hvManager = manager->getEMECHVManager(
      isInner? EMECHVModule::INNER: EMECHVModule::OUTER
    );
    const EMECHVManager::EMECHVData hvdata = hvManager.getDataSim();
    ATH_MSG_INFO("got LAr HV Manager for "
                 << (isInner? "inner": "outer") << " wheel");
    const EMECHVDescriptor& dsc = hvManager.getDescriptor();
    const unsigned int nFans = lwc()->GetNumberOfFans();
    unsigned int counter = 0;
  // loop over HV modules
    for(unsigned int iSide = hvManager.beginSideIndex();
        iSide < hvManager.endSideIndex(); ++ iSide
    ){
      unsigned short jSide = 1 - iSide; // local numbering is inverse
      for(unsigned int iEta = hvManager.beginEtaIndex();
          iEta < hvManager.endEtaIndex(); ++ iEta
      ){
        unsigned int jEta = iEta;
        if(isInner) jEta += 7;
        for(unsigned int iPhi = hvManager.beginPhiIndex();
            iPhi < hvManager.endPhiIndex(); ++ iPhi
        ){
          for(unsigned int iSector = hvManager.beginSectorIndex();
              iSector < hvManager.endSectorIndex(); ++ iSector
          ){
            const EMECHVModule& hvMod = hvManager.getHVModule(iSide, iEta, iPhi, iSector);
            unsigned int nElec = hvMod.getNumElectrodes();
            for(unsigned int iElec = 0; iElec < nElec; ++ iElec){
              const EMECHVElectrode& electrode = hvMod.getElectrode(iElec);
              unsigned int jElec = iElec;
              jElec += iSector*nElec;
              jElec += iPhi*nElec*dsc.getSectorBinning().getNumDivisions();
              if(jSide == 1){
                jElec = nFans + nFans / 2 - jElec;
                if(jElec >= nFans) jElec -= nFans;
              }
              for(unsigned int iGap = 0; iGap < 2; ++ iGap){
                double hv = hvdata.voltage (electrode, iGap);
                ATH_MSG_DEBUG("Side, Eta, Elec, Gap, hv "
                              << jSide << " " << jEta << " "
                              << jElec << " " << iGap << " "
                              << s_Values[jSide][jEta][iGap][jElec]
                              << " -> " << hv);
                if(fabs((s_Values[jSide][jEta][iGap][jElec] - hv)/s_Values[jSide][jEta][iGap][jElec]) > 0.05){
                  ATH_MSG_INFO("eta: " << dsc.getEtaBinning().binCenter(iEta) * (jSide == 0? 1: -1) << " "
                               << "phi: " << dsc.getPhiBinning().binCenter(iPhi) << " "
                               << "ele phi: " << electrode.getPhi()
                               << " side " << iGap
                               << " change HV from "
                               << s_Values[jSide][jEta][iGap][jElec]
                               << " to " << hv);
                }
                if(hv > -999.){
                  s_Values[jSide][jEta][iGap][jElec] = hv;
                  ++ counter;
                }
              }
            }
          }
        }
      }
    }
    ATH_MSG_INFO(counter << "HV values updated from DB");
  } else {
    ATH_MSG_WARNING("Unable to find LAr HV Manager");
  }
}

G4double HVHelper::GetVoltage(const G4ThreeVector& p) const
{
  // pickup HV value from the data of power supplies;
  // everything positioned in the Wheel's coord.system;
  // if it is not the same as the Atlas's one, adjustment is needed
  // either in this code or in the data file

    const std::pair<G4int, G4int> gap = lwc()->GetPhiGapAndSide(p);
    return GetVoltage(p, gap);
}

G4double HVHelper::GetVoltageBarrett(
    G4double phi, // phi should be adjused in the calling function
    G4int compartment,
    G4int eta_bin
) const
{
    const auto atlas_side = (lwc()->GetAtlasZside() > 0) ? 0 : 1;

    G4int eta_section = -1;
  // just return 0. if the point is not in the sens. region
    switch(compartment){
    case 8:
        if(eta_bin <   0){ return 0.; }
        if(eta_bin <=  3){ eta_section = 0; break; }
        if(eta_bin <=  7){ eta_section = 1; break; }
        if(eta_bin <= 15){ eta_section = 2; break; }
        if(eta_bin <= 23){ eta_section = 3; break; }
        if(eta_bin <= 27){ eta_section = 4; break; }
        if(eta_bin <= 35){ eta_section = 5; break; }
        if(eta_bin <= 43){ eta_section = 6; break; }
        return 0.;
        break;
    case 9:
        if(eta_bin <   0){ return 0.; }
        if(eta_bin <=  1){ eta_section = 1; break; }
        if(eta_bin <=  5){ eta_section = 2; break; }
        if(eta_bin <=  9){ eta_section = 3; break; }
        if(eta_bin <= 11){ eta_section = 4; break; }
        if(eta_bin <= 15){ eta_section = 5; break; }
        if(eta_bin <= 19){ eta_section = 6; break; }
        return 0.;
        break;
    default:
        return 0.;
        break;
    }

    G4int igap, electrode_side;
    if(phi > Gaudi::Units::twopi - lwc()->GetFanStepOnPhi()*0.5){
        igap = 0;
        electrode_side = 0;
    } else {
        igap = G4int((phi + lwc()->GetFanStepOnPhi()*0.5) / lwc()->GetFanStepOnPhi());
        const G4double elephi = igap*lwc()->GetFanStepOnPhi();
        const G4double dphi = phi - elephi;
        electrode_side = dphi <= 0.? 0: 1;
    }

    const G4int phi_section = GetPhiSection(
        igap, atlas_side, eta_section, electrode_side
    );

    return s_Values[atlas_side][eta_section][electrode_side][phi_section];
}

G4double HVHelper::GetVoltage(
    const G4ThreeVector& p, G4int phigap, G4int phihalfgap
) const
{
    const auto atlas_side = (lwc()->GetAtlasZside() > 0) ? 0 : 1;

    const auto eta_section = GetEtaSection(p);

    /*(right side of e large phi)*/ /*left side of electrode(small phi)*/
    const G4int electrode_side = (phihalfgap%2 == 0 ) ?   1   :   0  ;

    const G4int phi_section = GetPhiSection(
        phigap - 1, atlas_side, eta_section, electrode_side
    );

    return s_Values[atlas_side][eta_section][electrode_side][phi_section];
}

G4int HVHelperV00::GetPhiSection(
    G4int gap,
    G4int atlas_side,
    G4int eta_section,
    G4int electrode_side
) const
{
    const G4int &first_electrode =
        StartPhi(atlas_side, eta_section, electrode_side);
/* TODO: if necessary make this check at the initialization stage
    if(first_electrode < 0 || first_electrode >= lwc()->GetNumberOfFans()){
    ATH_MSG_FATAL(" get_HV_value: first_electrode number is out of range");
    G4Exception("EnergyCalculator", "ElectrodeOutOfRange", FatalException,
                "get_HV_value: first_electrode number is out of range");
    }
*/
    G4int phi_section = gap - first_electrode;
    if(phi_section < 0) phi_section += lwc()->GetNumberOfFans();
    phi_section /= m_NumberOfElectrodesInPhiSection;

    if(phi_section < 0 || phi_section >= m_NofPhiSections){
        ATH_MSG_FATAL("phi section number is out of range");
        G4Exception("LArG4::EC::HVHelper", "PhiSectionOutOfRange",
                    FatalException,
                    "Phi Section number is out of range"
        );
    }

    return phi_section;
}

G4int HVHelperV02::GetPhiSection(
    G4int gap, G4int, G4int, G4int
) const
{
    G4int phi_section = gap;
// TODO: check if this check is necessary
// doesn't have it for barrettes
    if(phi_section < 0) phi_section += lwc()->GetNumberOfFans();
    return phi_section;
}

G4double HVHelper::GetVoltage(
    const G4ThreeVector& p, const std::pair<G4int, G4int> &gap
) const
{
    const auto atlas_side = (lwc()->GetAtlasZside() > 0) ? 0 : 1;

    const auto eta_section = GetEtaSection(p);

    /*(right side of e large phi)*/   /*left side of electrode(small phi)*/
    const G4int electrode_side = gap.second > 0? 1: 0;

    const auto phi_section = GetPhiSection(
        lwc()->PhiGapNumberForWheel(gap.first),
        atlas_side, eta_section, electrode_side
    );

    return s_Values[atlas_side][eta_section][electrode_side][phi_section];
}

G4int HVHelper::GetEtaSection(const G4ThreeVector &p) const
{
    const G4ThreeVector p1(p.x(), p.y(), p.z() + m_WheelShift);
    const G4double eta = p1.pseudoRapidity();

    G4int eta_section = -1;
    for(G4int i = m_EtaMin; i <= m_EtaMax; ++ i){
        if(eta <= s_EtaLimit[i]){
            eta_section = i - 1;
            break;
        }
    }

    if(eta_section < 0) throw std::runtime_error("Index out of range");

    return eta_section;
}

HVHelper *HVHelper::CreateHelper(
    const LArWheelCalculator *calc,
    const G4String &version,
    G4bool fromDB
) {
    if(version == "v00" || version == "v01"){
        return new HVHelperV00(calc, version, fromDB);
    } else {
        return new HVHelperV02(calc, version, fromDB);
    }
}
