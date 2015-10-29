/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/Property.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"

// Athena includes
// AttributeList
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaKernel/errorcheck.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

// Tile includes
#include "TileCalibBlobObjs/TileCalibUtils.h" 
#include "TileConditions/TileDCSSvc.h"
#include "TileConditions/TileCondDCS_Data.h"
#include "TileConditions/TileCablingService.h"


#include "PathResolver/PathResolver.h"

#include <fstream>
#include <iomanip>

static const InterfaceID IID_TileDCSSvc("TileDCSSvc", 1, 0);
const InterfaceID& TileDCSSvc::interfaceID() {
  return IID_TileDCSSvc;
}

TileDCSSvc::TileDCSSvc(const std::string& name, ISvcLocator* pSvcLocator)
    : AthService(name, pSvcLocator)
  , m_detStore("DetectorStore", name)
  , m_IOVDbSvc("IOVDbSvc", name)
  , m_tileCablingSvc("TileCablingSvc", name)
  , m_cabling(0)
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_version(2)
{
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("Version", m_version); // 1 - RUN1 conditions DB, 2 - RUN2 conditions

  m_goodDrawer = 212222;               // good status for the drawer

  m_warningDrawer.push_back(202221);   // module in emergency mode

  m_warningDrawer.push_back(202222); // 830V HV is on, no communication on HV can-bus, status is unknown (assume that it's good)
  m_warningDrawer.push_back(2222); // no communication on HV can-bus, status is unknown (assume that it's good)

  m_warningDrawer.push_back(212020); // no communication on LV can-bus, status is unknown (assume that it's good)
  m_warningDrawer.push_back(202020); // 830V HV is on, no communication on LV and HV can-bus, status is unknown (assume that it's good)
  m_warningDrawer.push_back(2020); // no communication on LV and HV can-bus, status is unknown (assume that it's good)

  // two statuses which considered as warning in 2010
  //m_warningDrawer.push_back(  2221); // emergency mode but without info about 830V HV - can't be good in 2011
  //m_warningDrawer.push_back( 12222); // 830V HV is off, can't be good status

  m_partName.push_back("AUX");
  m_partName.push_back("LBA");
  m_partName.push_back("LBC");
  m_partName.push_back("EBA");
  m_partName.push_back("EBC");

  //declare variables which will be filled by jobOptions
  // note that reading of HVSET is disabled by default
  // and HV reference is taken from CES folder in COOL
  declareProperty("ReadAnyFolder", m_readAnyFolder = true); // single flag to disable everything
  declareProperty("ReadHV", m_readHV = true);        // reading HV
  declareProperty("ReadHVSET", m_readHVSET = false);    // reading HVSET
  declareProperty("ReadSTATES", m_readSTATES = true);    // reading drawer state
  declareProperty("FolderLocation", m_folderPrefix = "/TILE/DCS"); // global prefix

  declareProperty("GoodHVLimit", m_goodHVlimit = 2.0); // status=OK if HV diff below this limit
  declareProperty("WarningHVLimit", m_warningHVlimit = 10.0); // mask channels with HV diff above limit

  declareProperty("GoodDrawer", m_goodDrawer);     // status=OK if drawer status is like this
  declareProperty("WarningDrawer", m_warningDrawer); // status=WARNING if drawer status is like this
  m_unknownValue = -30; // should be negative

  m_ncalls = 0;

  for (int i = 0; i < 256; i++)
    m_STATES[i] = 0;
}

//_____________________________________________________________________________
StatusCode TileDCSSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if (TileDCSSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = dynamic_cast<TileDCSSvc*>(this);
  } else {
    return AthService::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TileDCSSvc::initialize() {

  CHECK( m_detStore.retrieve());

  int cnt = 0;
  if (m_readAnyFolder) {
    std::string name;
    if (m_readHV) {
      name = m_folderPrefix + std::string("/HV");
      if (m_detStore->regFcn(&TileDCSSvc::fillData, this, m_DCSData_HV, name).isSuccess()) {
        ATH_MSG_INFO( "DCS_TILE Registered callback for key: " << name);
        ++cnt;
      } else {
        ATH_MSG_ERROR( "DCS_TILE Cannot register callback function for key " << name);
      }
    }
    if (m_readHVSET) {
      name = m_folderPrefix + std::string("/HVSET");
      if (m_detStore->regFcn(&TileDCSSvc::fillData, this, m_DCSData_HVSET, name).isSuccess()) {
        ATH_MSG_INFO( "DCS_TILE Registered callback for key: " << name);
        ++cnt;
      } else {
        ATH_MSG_ERROR( "DCS_TILE Cannot register callback function for key " << name);
      }
    }
    if (m_readSTATES) {
      name = m_folderPrefix + std::string("/STATES");
      if (m_detStore->regFcn(&TileDCSSvc::fillData, this, m_DCSData_STATES, name).isSuccess()) {
        ATH_MSG_INFO( "DCS_TILE Registered callback for key: " << name);
        ++cnt;
      } else {
        ATH_MSG_ERROR( "DCS_TILE Cannot register callback function for key " << name);
      }
    }
  }

  ATH_MSG_INFO( "Will read " << cnt << " folders in total");

  //=== get TileCablingSvc
  CHECK( m_tileCablingSvc.retrieve());

  //=== cache pointers to cabling helpers
  m_cabling = m_tileCablingSvc->cablingService();
  if (!m_cabling) {
    ATH_MSG_ERROR( "Pointer to TileCablingService is zero: " << m_cabling);
    return StatusCode::FAILURE;
  }

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve());

  for (int i = 0; i < 2; ++i) {
    int ros = i + 2;
    int chan = 0;
    for (; chan < 48; ++chan) {
      int pmt = m_cabling->channel2hole(ros, chan);
      m_channel_used[i][chan] = (pmt > 0); // positive pmt - means channel is connected
      if (i) m_channel_used[3][chan] = m_channel_used[2][chan] = m_channel_used[1][chan];
      pmt = abs(pmt);
      m_channel2pmt[i][chan] = pmt - 1;
      m_pmt2channel[i][pmt - 1] = chan; // note - pmt-1 is used as index
    }
    for (; chan < 68; ++chan) {
      m_channel2pmt[i][chan] = chan;
      m_pmt2channel[i][chan] = chan;
    }
  }
  // description of EBA15 and EBC18 channel 0-3 do not exist, channel 18,19 used
  m_channel_used[2][0] = false;
  m_channel_used[2][1] = false;
  m_channel_used[2][2] = false;
  m_channel_used[2][3] = false;
  m_channel_used[2][18] = true;
  m_channel_used[2][19] = true;
  // description of EB with special C10 - channel 4 (pmt 5) is not used
  m_channel_used[3][4] = false;

  // initialize all arrays with HV and drawer status
  for (int ros = 1; ros < 5; ++ros) {
    for (int drawer = 0; drawer < 64; ++drawer) {
      int Drawer = RosDrawer(ros,drawer);
      m_STATES[Drawer] = m_unknownValue;
      for (int channel = 0; channel < 55; ++channel) {
        m_HV[Drawer][channel] = m_unknownValue;
      }
      for (int channel = 0; channel < 68; ++channel) {
        m_HVSET[Drawer][channel] = m_unknownValue;
      }
      for (int channel = 0; channel < 48; ++channel) { // to put either non_existing or unknown status
        m_STATUS[Drawer][channel] = getDCSHVSTATUS(ros, drawer, channel);
      }
    }
  }
  // read special deltas for few unstable PMTs
  if (m_readHVSET) read_badhv("BadHVChannels.dat");

  // not needed - all cool channels are the same as in RUN1 now
  //if (m_version == 2) {
  //  ATH_MSG_INFO( "Reading config for RUN2");
  //  if (read_config("DCSChStatesInCOOLR2.dat", "_LVPS_", m_pModulesSTATES)) return StatusCode::FAILURE;
  //  if (read_config("DCSChHVInCOOLR2.dat", "Drawer", m_pModulesHV)) return StatusCode::FAILURE;
  //  if (read_config("DCSChHVSETInCOOLR2.dat", "Drawer", m_pModulesHVSET)) return StatusCode::FAILURE;
  //} else {
  //  ATH_MSG_INFO( "Reading config for RUN1");
    if (read_config("DCSChStatesInCOOL.dat", "_LVPS_", m_pModulesSTATES)) return StatusCode::FAILURE;
    if (read_config("DCSChHVInCOOL.dat", "Drawer", m_pModulesHV)) return StatusCode::FAILURE;
    if (read_config("DCSChHVSETInCOOL.dat", "Drawer", m_pModulesHVSET)) return StatusCode::FAILURE;
  //}
  
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
StatusCode TileDCSSvc::finalize() {
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
TileDCSSvc::TileDCSStatus TileDCSSvc::getDCSHVSTATUS(int ros, int drawer, int channel) const {
  int Drawer = RosDrawer(ros,drawer);
  int lb_eb = (ros > 2) ? 1 : 0;
  int pmt0 = m_channel2pmt[lb_eb][channel]; // this is pmt-1 value
  if (lb_eb) {
    if ((ros == 3 && drawer == 14) || (ros == 4 && drawer == 17))
      lb_eb = 2; // EBA15 EBC18
    else if (!m_cabling->C10_connected(drawer)) lb_eb = 3; // EB modules with special C10
  }

  TileDCSStatus status(NON_EXISTING);

  if (m_channel_used[lb_eb][channel]) { // including only existing (and actually used) pmts
    if (m_HVSET[Drawer][pmt0] < 1.0 || m_HV[Drawer][pmt0] < 0.0    // HV can't be negative
        || m_HVSET[Drawer][pmt0] > 999.9 || m_HV[Drawer][pmt0] > 999.9) { // HV can't be greater 930V
      status = UNKNOWN; // ignore zero "set" and negative "measured" HV, set status to unknown
    } else {
      std::map<int, std::pair<float, float> >::const_iterator itr = m_knownBadHV.find(
          PmtIndex(Drawer,pmt0));
      if (itr != m_knownBadHV.end()) { // known unstable PMTs
        float HVgap = (m_readHVSET) ? // take into account offset if HVSET is used
            fabs(m_HVSET[Drawer][pmt0] + itr->second.second - m_HV[Drawer][pmt0]) : fabs(m_HVSET[Drawer][pmt0] - m_HV[Drawer][pmt0]);
        if (HVgap <= m_goodHVlimit)
          status = OK_KNOWNBADPMT;
        else if (HVgap <= m_warningHVlimit)
          status = WARNING_KNOWNBADPMT;
        else
          status = ALERT_KNOWNBADPMT;
      } else {
        float HVgap = fabs(m_HVSET[Drawer][pmt0] - m_HV[Drawer][pmt0]);
        if (HVgap <= m_goodHVlimit)
          status = OK;
        else if (HVgap <= m_warningHVlimit)
          status = WARNING;
        else
          status = ALERT;
      }
    }
  }

  return status;
}

//_____________________________________________________________________________
TileDCSSvc::TileDCSStatus TileDCSSvc::getDCSSTATUS(int ros, int drawer) const {
  TileDCSStatus status(ALERT_DRAWER);

  int state = m_STATES[RosDrawer(ros,drawer)];
  if (state == m_goodDrawer) status = OK_DRAWER;
  else if (state == m_unknownValue) status = UNKNOWN;
  else {
    for (std::vector<int>::const_iterator itr = m_warningDrawer.begin(); itr != m_warningDrawer.end(); ++itr) {
      if ((*itr) == state) {
        status = WARNING_DRAWER;
        break;
      }
    }
  }
  return status;
}

//_____________________________________________________________________________
StatusCode TileDCSSvc::fillData(int& /* i */, std::list<std::string>& keys) {

  if (!m_ncalls && !m_readHVSET) fillHVref();
  ++m_ncalls;

  ATH_MSG_VERBOSE( "fillData called " << m_ncalls <<" times");

  std::list<std::string>::const_iterator itr_key(keys.begin());
  // Retrieve CondAttrListCollection
  const CondAttrListCollection* attrListCollection;
  for (; itr_key != keys.end(); ++itr_key) {
    const std::string & thisFolder = *itr_key;
    if (m_detStore->retrieve(attrListCollection, thisFolder).isSuccess()) {
      bool case1 = (thisFolder == "/TILE/DCS/HV");
      bool case2 = (thisFolder == "/TILE/DCS/HVSET");
      // loop over collection
      CondAttrListCollection::const_iterator attrList(attrListCollection->begin());
      CondAttrListCollection::const_iterator end(attrListCollection->end());
      for (; attrList != end; ++attrList) {
        //A CondAttrListCollection is a map of ChanNum and AttributeList
        CondAttrListCollection::ChanNum drawerNumber = (*attrList).first;
        try {
          int Drawer = (case1) ? m_pModulesHV.at(drawerNumber)
                               : ((case2) ? m_pModulesHVSET.at(drawerNumber)
                                          : m_pModulesSTATES.at(drawerNumber));
          if (Drawer < 256) {
            CondAttrListCollection::AttributeList payload = (*attrList).second;
            ATH_MSG_VERBOSE( "DrawerInDCS " << Drawer
                            << " DrawerInCOOL " << drawerNumber
                            << " payload " << payload);

            //loop over AttributeListSpecification
            coral::AttributeList::const_iterator attrspecb = payload.begin();
            coral::AttributeList::const_iterator attrspece = payload.end();
            int pmt0 = 0; // runs over paylod parameters 
            for (; attrspecb != attrspece; ++attrspecb) {
              std::string param = (*attrspecb).specification().name();
              if (case1) {
                try {
                  m_HV[Drawer][pmt0] = (float) (*attrList).second[param].data<float>();
                  ATH_MSG_VERBOSE( "HV[" << pmt0 << "]= " << (float)(*attrList).second[param].data<float>() );
                } catch (...) {
                }
              } else if (case2) {
                try {
                  m_HVSET[Drawer][pmt0] = (float) (*attrList).second[param].data<float>();
                  ATH_MSG_VERBOSE( "HVSET[" << pmt0 << "]= " << (float)(*attrList).second[param].data<float>() );
                } catch (...) {
                }
              } else if (param == "FORDAQ_MBHV") {
                try {
                  m_STATES[Drawer] = (*attrList).second[param].data<int>();
                  ATH_MSG_VERBOSE( "STATES= " << (*attrList).second[param].data<int>() );
                } catch (...) {
                }
              }
              ++pmt0;
            }
          }
        } catch (...) {
        }
      }
    }
  }

  for (int ros = 1; ros < 5; ++ros) {
    int lb_eb = (ros > 2) ? 1 : 0;
    for (int drawer = 0; drawer < 64; ++drawer) {
      int Drawer = RosDrawer(ros,drawer);
      TileDCSStatus drawerStatus = getDCSSTATUS(ros, drawer);
      if (msgLvl(MSG::VERBOSE)) {
        msg(MSG::VERBOSE) << "Module=" << m_partName[ros] << std::setw(2) << std::setfill('0') << drawer + 1
                          << " Status=" << drawerStatus << endmsg;

        std::string ignored = ((drawerStatus > OK && !m_readHVSET) ? " ignored" : "");
        for (int channel = 0; channel < 48; ++channel) {
          int pmt0 = m_channel2pmt[lb_eb][channel];
          TileDCSStatus status = getDCSHVSTATUS(ros, drawer, channel);
          msg(MSG::VERBOSE) << "Module=" << m_partName[ros] << std::setw(2) << std::setfill('0') << drawer + 1
                            << " channel=" << channel
                            << " pmt=" << pmt0 + 1
                            << " HV=" << m_HV[Drawer][pmt0]
                            << " HVSET=" << m_HVSET[Drawer][pmt0]
                            << " Status=" << status
                            << ignored << endmsg;
        }
      }
      if (drawerStatus > OK && !m_readHVSET) {
        for (int channel = 0; channel < 48; ++channel) {
          if (m_STATUS[Drawer][channel] != NON_EXISTING) m_STATUS[Drawer][channel] = drawerStatus;
        }
      } else {
        for (int channel = 0; channel < 48; ++channel) {
          if (m_STATUS[Drawer][channel] != NON_EXISTING)
            m_STATUS[Drawer][channel] = std::max(drawerStatus, getDCSHVSTATUS(ros, drawer, channel));
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

//_____________________________________________________________________________
int TileDCSSvc::read_config(std::string filename, std::string substr,
    std::vector<int> & pModules) {
  std::string file_name = PathResolver::find_file(filename, "DATAPATH");
  std::ifstream file(file_name.c_str());
  if (!file.is_open()) {
    ATH_MSG_ERROR( "Can't find " << filename);
    return 2;
  } else {
    ATH_MSG_DEBUG( "Reading file " << file_name);
  }

  pModules.resize(257, 0xFFFF);

  std::string compName[5] = { "XXX", "ATLTILLV01", "ATLTILLV02", "ATLTILLV00", "ATLTILLV03" };

  unsigned int ind, ros, module;
  std::string s, line;
  size_t pos;

  while (true) {

    file >> s >> ind >> line;

    if (file.eof() || file.fail()) break;

    for (ros = 1; ros < 5; ++ros) {
      pos = line.find(compName[ros]);
      if (pos != std::string::npos) break;
    }
    if (ros == 5) {
      msg(MSG::WARNING) << "File " << filename << " Wrong computer name in " << line << endmsg;
      continue;
    }
    pos = line.find(substr);
    if (pos == std::string::npos) {
      ATH_MSG_WARNING( "File " << filename << " Word " << substr << " not found in " << line);
      continue;
    }
    module = atoi(&line[pos + substr.size()]);
    if (module < 1 || module > 64) {
      ATH_MSG_WARNING( "File " << filename << " Wrong module number in " << line);
      continue;
    }

    ATH_MSG_VERBOSE(
        "Module="<<m_partName[ros]<<std::setw(2)<<std::setfill('0')<<module<<" ind="<<ind);
    if (pModules.size() <= ind) pModules.resize(ind + 1, 0xFFFF);
    pModules[ind] = RosDrawer(ros,(module-1));
  }
  file.close();

  return 0;
}

//_____________________________________________________________________________
int TileDCSSvc::read_badhv(std::string filename) {
  std::string file_name = PathResolver::find_file(filename, "DATAPATH");
  std::ifstream file(file_name.c_str());
  if (!file.is_open()) {
    ATH_MSG_WARNING( "Can't find " << filename);
    return 1;
  } else {
    ATH_MSG_DEBUG("Reading file " << file_name);
  }

  std::string partName[5] = { "AUX", "LBA", "LBC", "EBA", "EBC" };

  int ros, module, pmt;
  float delta, offset;
  char ll[256];
  std::string line;
  size_t pos = 0;

  while (true) {

    file.getline(ll, sizeof(ll));

    if (file.eof() || file.fail()) break;

    line = std::string(ll);

    for (ros = 1; ros < 5; ++ros) {
      pos = line.find(partName[ros]);
      if (pos != std::string::npos) break;
    }
    if (ros == 5) {
      ATH_MSG_WARNING( "File " << filename << " Partition name not found in " << line);
      continue;
    }
    module = atoi(&line[pos + 3]);
    if (module < 1 || module > 64) {
      ATH_MSG_WARNING( "File " << filename << " Wrong module number in " << line);
      continue;
    }

    pos = line.find("PMT number");
    if (pos == std::string::npos) {
      ATH_MSG_WARNING( "File " << filename << " Word 'PMT number' not found in " << line);
      continue;
    }
    pmt = atoi(&line[pos + 10]);
    if (pmt < 1 || pmt > 48) {
      ATH_MSG_WARNING( "File " << filename << " Wrong pmt number in " << line);
      continue;
    }

    pos = line.find("Step Value is");
    if (pos == std::string::npos) {
      ATH_MSG_WARNING( "File " << filename << " Word 'Step Value is' not found in " << line);
      continue;
    }
    delta = atof(&line[pos + 13]);
    if (delta <= 0.0 || delta > 1000.0) {
      ATH_MSG_WARNING( "File " << filename << " Wrong delta number in " << line);
      continue;
    }

    pos = line.find("Offset is:");
    if (pos == std::string::npos) {
      ATH_MSG_WARNING( "File " << filename << " Word 'Offset is:' not found in " << line);
      continue;
    }
    offset = atof(&line[pos + 10]);
    if (offset < -1000.0 || offset > 1000.0) {
      ATH_MSG_WARNING( "File " << filename << " Wrong offset number in " << line);
      continue;
    }

    ATH_MSG_WARNING( "Module=" << m_partName[ros] << std::setw(2) << std::setfill('0') << module
                    << " pmt=" << pmt
                    << " delta=" << delta
                    << " offset=" << offset);

    m_knownBadHV[PmtIndex(RosDrawer(ros,(module-1)),(pmt-1))] = std::make_pair(delta, offset);
  }
  file.close();

  return 0;
}

void TileDCSSvc::fillHVref() {

  float HV_las = -5.;
  float HV_ces = -5.;

  for (int ros = 1; ros < 5; ++ros) {
    int lb_eb = (ros > 2) ? 1 : 0;
    for (int drawer = 0; drawer < 64; ++drawer) {
      int Drawer = RosDrawer(ros,drawer);
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(ros, drawer);
      for (int channel = 0; channel < 48; ++channel) {
        int pmt0 = m_channel2pmt[lb_eb][channel];
        HV_ces = m_tileToolEmscale->getCesRefHv(drawerIdx, channel);
        HV_las = m_tileToolEmscale->getLasRefHv(drawerIdx, channel);
        if (HV_las >= 0.)
          m_HVSET[Drawer][pmt0] = HV_las;
        else
          m_HVSET[Drawer][pmt0] = HV_ces;

        if (msgLvl(MSG::VERBOSE)) {
          msg(MSG::VERBOSE) << "Module=" << m_partName[ros] << std::setw(2) << std::setfill('0') << drawer + 1
                            << " channel=" << channel
                            << " pmt=" << pmt0 + 1
                            << " HVref=" << m_HVSET[Drawer][pmt0];

          if (HV_las >= 0.)
            msg(MSG::VERBOSE) << " Taken from Laser run (replace " << HV_ces << " cesium value)" << endmsg;
          else
            msg(MSG::VERBOSE) << endmsg;
        }
      }
    }
  }
}
