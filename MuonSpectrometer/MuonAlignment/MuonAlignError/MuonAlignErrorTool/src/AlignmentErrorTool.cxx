/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
////  Authors: Pierre-Francois Giraud, Camilla Maiani
////  Contacts: pierre-francois.giraud@cern.ch, cmaiani@cern.ch
////////////////////////////////////////////////////////////////////

#include "MuonAlignErrorTool/AlignmentErrorTool.h"

#include "MuonAlignErrorBase/AlignmentTranslationDeviation.h"
#include "MuonAlignErrorBase/AlignmentRotationDeviation.h"
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "PathResolver/PathResolver.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

#include <fstream>
#include <sstream>
#include <boost/functional/hash.hpp>

using namespace MuonAlign;

AlignmentErrorTool::AlignmentErrorTool(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p) {
  declareInterface<Trk::ITrkAlignmentDeviationTool>(this);
}

AlignmentErrorTool::deviationSummary_t::deviationSummary_t()
: traslation(0.), rotation(0.), stationName(""), sumP(Amg::Vector3D(0., 0., 0.)), sumU(Amg::Vector3D(0., 0., 0.)), sumV(Amg::Vector3D(0., 0., 0.)), sumW2(0.) { 
}

StatusCode AlignmentErrorTool::initialize() {

  ATH_MSG_INFO("*****************************************");
  ATH_MSG_INFO("AlignmentErrorTool::initialize()");

  ATH_CHECK(m_idTool.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_readKey.initialize());
    
  return StatusCode::SUCCESS;

}

void AlignmentErrorTool::makeAlignmentDeviations (const Trk::Track& track, std::vector<Trk::AlignmentDeviation*>& deviations) const {

  ATH_MSG_DEBUG("AlignmentErrorTool::makeAlignmentDeviations()");

  SG::ReadCondHandle<MuonAlignmentErrorData> readHandle{m_readKey};
  const MuonAlignmentErrorData* readCdo{*readHandle};
  if(!readCdo){
    ATH_MSG_ERROR("nullptr to the read conditions object");
    return;
  }
  std::vector<deviationStr> devStrVec;
  readCdo->getVec(devStrVec);
  std::vector<deviationSummary_t*> devSumVec;
  for(unsigned int i=0; i < devStrVec.size(); i++) {
    deviationSummary_t* tmp = new deviationSummary_t;
    tmp->traslation = devStrVec[i].traslation;
    tmp->rotation = devStrVec[i].rotation;
    tmp->stationName = devStrVec[i].stationName;
    tmp->multilayer = devStrVec[i].multilayer;
    tmp->hits.clear();
    Amg::Vector3D nullvec(0., 0., 0.);
    tmp->sumP = nullvec;
    tmp->sumU = nullvec;
    tmp->sumV = nullvec;
    tmp->sumW2 = 0.;
    devSumVec.push_back(tmp);
  }

  typedef DataVector< const Trk::TrackStateOnSurface > tsosc_t;
  const tsosc_t* tsosc = track.trackStateOnSurfaces();

  // LOOP ON HITS ON TRACK //
  unsigned int nPrecisionHits = 0;
  for (tsosc_t::const_iterator it=tsosc->begin(), end=tsosc->end(); it!=end; it++) {

    const Trk::TrackStateOnSurface* tsos = *it;

    if (tsos->type(Trk::TrackStateOnSurface::Measurement)) {

      const Trk::MeasurementBase* meas = tsos->measurementOnTrack();
      const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*> (meas);

      if (!rot) {
	const Trk::CompetingRIOsOnTrack* crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*> (meas);
	if (crot) {
	  unsigned int index = crot->indexOfMaxAssignProb();
	  rot = &(crot->rioOnTrack(index));
	}
      }
      if (!rot) continue;

      Identifier channelId = rot->identify();
      if (!m_idHelperSvc->isMuon(channelId)) {
        // the RIO_OnTrack Identifiers could also come from ID or Calo, but this tool is only interested in MS hits
        ATH_MSG_VERBOSE("Given Identifier "<<channelId.get_compact()<<" is no muon Identifier, continuing");
        continue;
      }
      if (m_idHelperSvc->isMM(channelId)||m_idHelperSvc->issTgc(channelId)) continue; // needs to be still implemented for the NSW
      
      MuonCalib::MuonFixedId calibId = m_idTool->idToFixedId(channelId);
      if (!calibId.isValid()) continue;

      // GATHERING INFORMATION TO PUT TOGETHER THE STATION NAME //
      std::string completename = hardwareName(calibId);
      std::stringstream multilayer_stream;
      multilayer_stream << calibId.mdtMultilayer();
      std::string multilayer_sstring = multilayer_stream.str();

      if ( m_idHelperSvc->isMdt(channelId) || ( m_idHelperSvc->isCsc(channelId) && m_idHelperSvc->cscIdHelper().measuresPhi(channelId) == 0 ) ) { 

        ATH_MSG_DEBUG("Hit is in station " << completename << " multilayer " << multilayer_sstring);
	++nPrecisionHits;
 
        // FOR CROSS-CHECK
        bool is_matched = false;

        // LOOP ON STATION DEVIATIONS EXTRACTED FROM INPUT FILE //
        for(unsigned int iDev=0; iDev < devSumVec.size(); iDev++) {

           // find a match to the reg exp //
	   boost::regex tmp_stationName = devSumVec[iDev]->stationName;

           if (  boost::regex_match(completename, tmp_stationName) ) {

              if( !boost::regex_match(multilayer_sstring, devSumVec[iDev]->multilayer) && !m_idHelperSvc->isCsc(channelId) ) {
                 continue;
              }

              // ASSOCIATE EACH NUISANCE TO A LIST OF HITS
              devSumVec[iDev]->hits.push_back(rot);
              
              // COMPUTE RELEVANT NUMBERS
              const Trk::PrepRawData* prd = rot->prepRawData();
              const Trk::Surface& sur = prd->detectorElement()->surface(prd->identify());
              double w2 = sqrt(rot->localCovariance()(Trk::loc1,Trk::loc1));
              w2 = 1./(w2*w2);
              devSumVec[iDev]->sumW2 += w2;
              devSumVec[iDev]->sumP += w2 * tsos->trackParameters()->position();
              devSumVec[iDev]->sumU += w2 * tsos->trackParameters()->momentum().unit();
              
              // CHECK 1 //
              Amg::Vector3D zATLAS(0., 0., 1.);
              Amg::Vector3D v1 = (tsos->trackParameters()->position()).cross(zATLAS);
              v1 /= v1.mag();
              Amg::Vector3D v2 = sur.transform().rotation().col(2)/(sur.transform().rotation().col(2)).mag();
              double sign = (v1.dot(v2) > 0.) ? 1. : -1.;

              // ARTIFICIALLY ORIENTATE EVERYTHING TOWARDS THE SAME DIRECTION
              devSumVec[iDev]->sumV += sign * w2 * sur.transform().rotation().col(2);

              // FOR CROSS-CHECK
              is_matched = true;
          
           } // match the station name to the deviation regexp

        } // LOOP ON DEVIATIONS

        if (is_matched == false)
           ATH_MSG_WARNING("The hits in the station " << completename << ", multilayer " << multilayer_sstring << " couldn't be matched to any deviation regexp in the list.");

     } //CHECK IF IT'S MDT OR CSC HIT

    } // CHECK TSOS OF TYPE 'MEASUREMENT'

  } //LOOP ON TSOS

  // Nuisance parameters covering the complete track are not wanted. (MS/ID
  // error treated differently for now). Removing the deviations covering the
  // full track in further processing.
  for (deviationSummary_t* dev : devSumVec) {
    if (dev->hits.size() == nPrecisionHits)
      dev->hits.clear();
  }

  // CHECK HIT LISTS OVERLAP
  // FIRST CREATE AN INDEPENDENT COPY OF THE STRUCT VECTOR
  std::vector<deviationSummary_t*> new_deviationsVec;
  for(unsigned int iDev=0; iDev < devSumVec.size(); iDev++) {
     deviationSummary_t* tmp = new deviationSummary_t();
     (*tmp) = (*devSumVec[iDev]);
     new_deviationsVec.push_back(tmp);
  }
  
  for(unsigned int iDev=0; iDev < devSumVec.size(); iDev++) {

     if (!new_deviationsVec[iDev])
        continue;

     std::vector<const Trk::RIO_OnTrack*> v1 = devSumVec[iDev]->hits;
     if ( v1.empty() )
        continue;

     std::stable_sort(v1.begin(), v1.end());

     for(unsigned int jDev=iDev+1; jDev < devSumVec.size(); jDev++) {

        if (!new_deviationsVec[jDev])
           continue;

        bool match = false;

        if ( devSumVec[iDev]->hits.size() != devSumVec[jDev]->hits.size() )
           continue;

        std::vector<const Trk::RIO_OnTrack*> v2 = devSumVec[jDev]->hits;
        std::stable_sort(v2.begin(), v2.end());

        match = (v1 == v2); 

        if ( match ) {

           ATH_MSG_DEBUG("Found deviations " << iDev << " and " << jDev << " related to the same list of hits. Merging...");
           ATH_MSG_DEBUG("old (traslation, rotation) systematic uncertainties for " << iDev << ": " << new_deviationsVec[iDev]->traslation << ", " << new_deviationsVec[iDev]->rotation);
           ATH_MSG_DEBUG("old (traslation, rotation) systematic uncertainties for " << jDev << ": " << devSumVec[jDev]->traslation << ", " << devSumVec[jDev]->rotation);

           // MERGE THE TWO DEVIATIONS ASSOCIATED TO THE SAME LIST OF HITS //
           double new_traslation = sqrt(new_deviationsVec[iDev]->traslation*new_deviationsVec[iDev]->traslation + devSumVec[jDev]->traslation*devSumVec[jDev]->traslation);
           double new_rotation = sqrt(new_deviationsVec[iDev]->rotation*new_deviationsVec[iDev]->rotation + devSumVec[jDev]->rotation*devSumVec[jDev]->rotation);

           // NOW PREPARE TO ERASE ONE OF THE TWO COPIES //
	   delete new_deviationsVec[jDev];
           new_deviationsVec[jDev] = nullptr;

           // ASSIGN NEW TRASLATION/ROTATION TO THE REMAINING COPY //
           new_deviationsVec[iDev]->traslation = new_traslation;
           new_deviationsVec[iDev]->rotation = new_rotation;
           ATH_MSG_DEBUG("New combined (traslation, rotation) systematic uncertainties: " << new_traslation << ", " << new_rotation);

        } // FIND AN OVERLAP IN THE HITS LISTS
     } // SECOND LOOP ON DEVIATIONS
  } // FIRST LOOP ON DEVIATIONS

  // NOW BUILD THE DEVIATIONS
  deviations.clear();
  ATH_MSG_DEBUG("************************************");
  ATH_MSG_DEBUG("FINAL LIST OF DEVIATIONS");
  for(unsigned int iDev=0; iDev < new_deviationsVec.size(); iDev++) {

     // THIS HAPPENS IF A MERGING HAD BEEN DONE
     if (!new_deviationsVec[iDev]) {
        continue;
     }

     // SKIP IF NO HITS ARE ASSOCIATED TO THIS DEVIATION
     if ( new_deviationsVec[iDev]->hits.size() == 0) {
        //ATH_MSG_DEBUG("No hits found associated to the rule " << new_deviationsVec[iDev]->stationName.str() << ", skip");
        continue;
     }

     double rotation = new_deviationsVec[iDev]->rotation;
     double traslation = new_deviationsVec[iDev]->traslation;

     Amg::Vector3D sumP = new_deviationsVec[iDev]->sumP;
     Amg::Vector3D sumU = new_deviationsVec[iDev]->sumU;
     Amg::Vector3D sumV = new_deviationsVec[iDev]->sumV;
     double sumW2       = new_deviationsVec[iDev]->sumW2;

     sumP *= (1./sumW2);
     sumU *= (1./sumW2);
     sumV *= (1./sumW2);

     if ( traslation >= 0.001*Gaudi::Units::mm ) { 

        std::size_t hitshash = 0;
        for(auto it : new_deviationsVec[iDev]->hits ) boost::hash_combine( hitshash , (it->identify()).get_compact() );
        deviations.push_back(new AlignmentTranslationDeviation(sumU.cross(sumV), traslation*Gaudi::Units::mm, new_deviationsVec[iDev]->hits));
        deviations.back()->setHashOfHits(hitshash);

        ATH_MSG_DEBUG("A translation along (" << sumU.x() << ", " << sumU.y() << ", " << sumU.z() << ") with sigma=" << traslation*Gaudi::Units::mm << " mm was applied to " << new_deviationsVec[iDev]->hits.size() << " hits matching the station: " << new_deviationsVec[iDev]->stationName.str() << " and the multilayer " << new_deviationsVec[iDev]->multilayer.str());

     }
     if ( rotation >= 0.000001*Gaudi::Units::rad ) {

        std::size_t hitshash = 0;
        for(auto it : new_deviationsVec[iDev]->hits ) boost::hash_combine( hitshash , (it->identify()).get_compact() );
        deviations.push_back(new AlignmentRotationDeviation(sumP, sumV, rotation*Gaudi::Units::rad, new_deviationsVec[iDev]->hits));
        deviations.back()->setHashOfHits(hitshash);

        ATH_MSG_DEBUG("A rotation around the center = (" << sumP.x() << ", " << sumP.y() << ", " << sumP.z() << ") and axis = (" << sumV.x() << ", " << sumV.y() << ", " << sumV.z() << ") with sigma=" << rotation/Gaudi::Units::mrad << " mrad was applied to " << new_deviationsVec[iDev]->hits.size() << " hits matching the station " << new_deviationsVec[iDev]->stationName.str() << " and the multilayer " << new_deviationsVec[iDev]->multilayer.str());

     }

  } // LOOP ON NUISANCES

  ATH_MSG_DEBUG("******************************");
  ATH_MSG_DEBUG("FINAL CHECKUP");
  ATH_MSG_DEBUG("Found " << deviations.size() << " nuisances after duplicates merging");
  ATH_MSG_DEBUG("******************************");

  // NOW EMPTY THE LOCAL DEVIATIONS VECTOR //
  for(unsigned int iDev=0; iDev < new_deviationsVec.size(); iDev++) {
     delete new_deviationsVec[iDev];
  }

  for(unsigned int iDev=0; iDev < devSumVec.size(); iDev++) {
    delete devSumVec[iDev];
  }

  //ATH_MSG_DEBUG("Currently we have " << AlignmentErrorTool::deviationSummary_t::i_instance << " deviation vectors");

  return;

}

////////////////////////////
// RECOGNIZE STATION NAME //
////////////////////////////

inline std::string AlignmentErrorTool::hardwareName(MuonCalib::MuonFixedId calibId) const {
  if (sector(calibId)==13) {
    if (calibId.eta()== 7 && calibId.stationName()==5) return "BOE1A13"; // BOE1A13 not BOL7A13
    if (calibId.eta()==-7 && calibId.stationName()==5) return "BOE1C13"; // BOE1C13 not BOL7C13
  }
  std::string etaString = "0";
  etaString[0] += std::abs(hardwareEta(calibId));
  return calibId.stationNameString() + etaString + side(calibId) + sectorString(calibId);
}

inline std::string AlignmentErrorTool::side(MuonCalib::MuonFixedId calibId) const {
  return calibId.eta()>0 ? "A" : calibId.eta()<0 ? "C" : "B";
}

inline std::string AlignmentErrorTool::sectorString(MuonCalib::MuonFixedId calibId) const  {
  std::string ret = "00";
  int sec = sector(calibId);
  if (sec >= 10) {
    ret[0] = '1';
    sec -= 10;
  }
  ret[1] += sec;
  return ret;
}

inline int AlignmentErrorTool::sector (MuonCalib::MuonFixedId calibId) const {
  return isSmallSector(calibId) ? 2*calibId.phi() : 2*calibId.phi()-1;
}

inline bool AlignmentErrorTool::isSmallSector (MuonCalib::MuonFixedId calibId) const {
  switch (calibId.stationName()) {
    case 2: return true;
    case 4: return true;
    case 6: return true;
    case 7: return true;
    case 9: return true;
    case 10: return true;
    case 11: return true;
    case 17: return true;
    case 20: return true;
    case 23: return true;
    case 24: return true;
    case 33: return true;
    default: return false;
  }
}

inline int AlignmentErrorTool::hardwareEta(MuonCalib::MuonFixedId calibId) const {
  switch (calibId.stationName()) {
    case 3:
    {
       if (sector(calibId)==13) {
          switch (calibId.eta()) {
             case 4: return 5;
             case 5: return 6;
             case 6: return 7;
             case -4: return -5;
             case -5: return -6;
             case -6: return -7;
          }
       }
       return calibId.eta();
     }
     case 5:
     {
       if (sector(calibId)==13) {
       if (calibId.eta()== 7) return 1; // BOE1A13 not BOL7A13
       if (calibId.eta()==-7) return -1; // BOE1C13 not BOL7C13
     }
     return calibId.eta();
    }
    case 10: return calibId.eta()>0 ? calibId.eta()*2-1 : calibId.eta()*2+1;
    case 11: return calibId.eta()*2;
    case 15:
      {
        if ((sector(calibId) == 1) || (sector(calibId) == 9)) {
          switch (calibId.eta()) {
            case 4: return 5;
            case 5: return 4;
            case -4: return -5;
            case -5: return -4;
          }
        }
        return calibId.eta();
      }
    case 16:
      {
        if ((sector(calibId) == 5) && (calibId.eta() == 1)) return 2;
        if ((sector(calibId) == 5) && (calibId.eta() == -1)) return -2;
        return calibId.eta();
      }
    default: return calibId.eta();
  }
}
