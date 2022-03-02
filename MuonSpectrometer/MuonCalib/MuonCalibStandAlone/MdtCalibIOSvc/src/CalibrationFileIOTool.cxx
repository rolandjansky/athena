/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// this
#include "MdtCalibIOSvc/CalibrationFileIOTool.h"

// MdtCalibUtils
#include "MdtCalibUtils/RtDataFromFile.h"
#include "MdtCalibUtils/TubeDataFromFile.h"

// MdtCalibData
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/MdtTubeFitContainer.h"
#include "MdtCalibData/RtChebyshev.h"
#include "MdtCalibData/RtFromPoints.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibData/RtResolutionFromPoints.h"

// MdtCalibRt
#include "MdtCalibRt/RtCalibrationOutput.h"

// MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"

// MuonCalibMath
#include "MuonCalibMath/SamplePoint.h"

// c - c++
#include <dirent.h>
#include <sys/types.h>

#include "fstream"
#include "iostream"
#include "string"

namespace MuonCalib {

    CalibrationFileIOTool::CalibrationFileIOTool(const std::string &t, const std::string &n, const IInterface *p) :
        AthAlgTool(t, n, p), m_calib_dir("calibration"), m_use_fixed_id(false), m_rt_lookup(true) {
        declareInterface<ICalibrationIOTool>(this);
        declareProperty("outputLocation", m_calib_dir);
        declareProperty("UseFixedId", m_use_fixed_id);
        declareProperty("RtRelationLookup", m_rt_lookup);
    }

    StatusCode CalibrationFileIOTool::WriteT0(MdtTubeFitContainer *t0_output, const NtupleStationId &station_id, int /*iov_start*/,
                                              int /*iov_end*/) {
        // create directory
        system(("mkdir -p " + m_calib_dir + "/t0s").c_str());
        // open file
        std::string t0_file_name(m_calib_dir + "/t0s/T0" + station_id.regionId() + ".dat");
        std::ofstream output_file(t0_file_name.c_str());
        if (output_file.fail()) {
            ATH_MSG_ERROR("Could not open t0 file " << t0_file_name << "!");
            return StatusCode::FAILURE;
        }
        // write
        TubeDataFromFile t0_file;
        t0_file.setNRegions(1);
        t0_file.addTubes(0, t0_output);
        t0_file.write(output_file);
        return StatusCode::SUCCESS;
    }

    StatusCode CalibrationFileIOTool::WriteRt(const RtCalibrationOutput *rt_relation, std::shared_ptr<const IRtResolution> resolution,
                                              const NtupleStationId &station_id, int /*iov_start*/, int /*iov_end*/, bool /*real_rt*/,
                                              bool /*real_resolution*/) {
        RtDataFromFile rt_data;
        std::shared_ptr<const IRtRelation> new_rt = rt_relation->rt();
        std::unique_ptr<RtDataFromFile::RtRelation> rt = std::make_unique<RtDataFromFile::RtRelation>();
        if (m_use_fixed_id) {
            rt->setRegionId(station_id.FixedId());
            rt_data.setVersion(2, 0);
        } else {
            if (station_id.RegionHash() != 0) { rt->setRegionId(station_id.RegionHash()); }
        }
        int rt_region_id(0);
        rt_data.setNRts(1);
        rt_data.addRt(rt_region_id, rt.get(), rt_relation->fullInfo().get());
        if (!fill_rt(rt, new_rt, resolution)) return StatusCode::FAILURE;
        // write out the r-t relationship //
        system(("mkdir -p " + m_calib_dir + "/rts").c_str());
        std::string rt_file_name(m_calib_dir + "/rts/Rt_" + station_id.regionId() + ".dat");
        std::ofstream output_file(rt_file_name.c_str());
        rt_data.write(output_file, rt_region_id);
        return StatusCode::SUCCESS;
    }

    StatusCode CalibrationFileIOTool::LoadT0(std::map<NtupleStationId, MdtStationT0Container *> &t0s, int /*iov_id*/) {
        ATH_MSG_INFO("Reading calibration from '" << (m_calib_dir + "/t0s") << "'");
        t0s.clear();
        DIR *directory(opendir((m_calib_dir + "/t0s").c_str()));
        if (directory == nullptr) return StatusCode::SUCCESS;
        struct dirent *dent;
        // loop on all files in directory
        while ((dent = readdir(directory)) != nullptr) {
            std::string nm(dent->d_name);
            std::string station_str;
            int eta, phi, ml;
            // interpret filename and check if it is an t0 file
            if (!interpret_chamber_name(nm, "T0", station_str, eta, phi, ml)) continue;
            // store
            MuonCalib::NtupleStationId id;
            if (!id.Initialize(station_str, eta, phi, ml)) continue;
            ATH_MSG_INFO("Reading t0 for " << station_str << "_" << eta << "_" << phi);
            MuonCalib::MdtStationT0Container *t0_container = new MuonCalib::MdtStationT0Container((m_calib_dir + "/t0s/" + nm).c_str());
            if (t0_container->t0_loaded()) {
                t0s[id] = t0_container;
                ATH_MSG_INFO("Read t0 for " << id.regionId());
            } else {
                ATH_MSG_ERROR("t0 file for " << id.regionId() << " is corrupted!");
                delete t0_container;
            }
        }
        closedir(directory);
        return StatusCode::SUCCESS;
    }

    StatusCode CalibrationFileIOTool::LoadRt(std::map<NtupleStationId, IRtRelation *> &rts, std::map<NtupleStationId, IRtResolution *> &res,
                                             int /*iov_id*/) {
        rts.clear();
        res.clear();
        DIR *directory(opendir((m_calib_dir + "/rts").c_str()));
        if (directory == nullptr) return StatusCode::SUCCESS;
        struct dirent *dent;
        // loop on all files in directory
        while ((dent = readdir(directory)) != nullptr) {
            std::string nm(dent->d_name);
            std::string station_str;
            int eta, phi, ml;
            // interpret filename and check if it is an t0 file
            if (!interpret_chamber_name(nm, "Rt_", station_str, eta, phi, ml)) continue;
            MuonCalib::NtupleStationId id;
            if (!id.Initialize(station_str, eta, phi, ml)) continue;
            ATH_MSG_INFO("Reading rt for " << station_str << "_" << eta << "_" << phi << "_" << ml);
            // read rt ralation
            read_rt_relation((m_calib_dir + "/rts/" + nm), rts, res, id);
            ATH_MSG_INFO("Read rt for " << id.regionId());
        }
        closedir(directory);
        return StatusCode::SUCCESS;
    }

    bool CalibrationFileIOTool::fill_rt(std::unique_ptr<RtDataFromFile::RtRelation> &rt, const std::shared_ptr<const IRtRelation>& new_rt,
                                        const std::shared_ptr<const MuonCalib::IRtResolution>& resolut) {
        ///////////////
        // VARIABLES //
        ///////////////
        const CalibFunc::ParVec &rt_param = new_rt->parameters();
        // parameters of the r-t relationship to be copied
        std::shared_ptr<const RtChebyshev> rt_Chebyshev = std::dynamic_pointer_cast<const RtChebyshev>(new_rt);
        std::shared_ptr<const RtRelationLookUp> rt_lookup = std::dynamic_pointer_cast<const RtRelationLookUp>(new_rt);

        ////////////////////////
        // FILL THE r-t CLASS //
        ////////////////////////

        // case 1: r-t relationship is stored in the class RtChebyshev //
        if (rt_Chebyshev) {
            unsigned int nb_points(100);
            double t_length(rt_Chebyshev->tUpper() - rt_Chebyshev->tLower());
            double bin_size(t_length / static_cast<double>(nb_points - 1));
            for (unsigned int k = 0; k < nb_points; k++) {
                double time(rt_Chebyshev->tLower() + k * bin_size);
                double radius(rt_Chebyshev->radius(time));
                double resol(0.0);
                resol = resolut->resolution(time);
                if (std::isnan(time) || std::isnan(radius) || std::isnan(resol)) {
                    ATH_MSG_FATAL("Filling nan into rt relation!");
                    //				return false;
                }
                rt->addEntry(time, radius, resol);
            }
        }

        // case 2: r-t relationship is stored in the class RtRelationLookUp //
        if (rt_lookup) {
            double t_min(rt_param[0]);
            double bin_size = rt_param[1];
            unsigned int nb_points(rt_lookup->nPar() - 2);
            for (unsigned int k = 0; k < nb_points; k++) {
                double radius(rt_param[k + 2]);
                double resol(0.0);

                resol = resolut->resolution(t_min + bin_size * k);
                if (std::isnan(radius) || std::isnan(resol)) {
                    ATH_MSG_FATAL("Filling nan into rt relation!");
                    //				return false;
                }
                rt->addEntry(t_min + bin_size * k, rt_param[k + 2], resol);
            }
        }

        ////////////////////////////////////////////
        // DECLARE THE NEW r-t RELATIONSHIP VALID //
        ////////////////////////////////////////////
        if (new_rt->HasTmaxDiff()) rt->addEntry(new_rt->GetTmaxDiff(), -9999, 0);
        rt->isValid(1);

        return true;
    }  // end CalibrationFileIOTool::fill_rt

    inline bool CalibrationFileIOTool::interpret_chamber_name(const std::string &nm, const char *prefix, std::string &station, int &eta,
                                                              int &phi, int &ml) const {
        // check if name begins with the prefix
        std::string prefix_st(prefix);
        if (nm.find(prefix_st) != 0) return false;
        // check if filename ends in .dat
        if (static_cast<int>(nm.find(".dat")) < 0 || nm.find(".dat") != nm.size() - 4) return false;
        // cut prefix and suffix from filename
        std::string cutout(nm, prefix_st.size(), nm.size() - 4 - prefix_st.size());
        // extract station name
        int uscore_pos(cutout.find('_'));
        if (uscore_pos <= 0) return false;
        station = std::string(cutout, 0, uscore_pos);
        // get eta, phi, and multilayer
        std::string cutout2(cutout, uscore_pos + 1);
        int count_items(sscanf(cutout2.c_str(), "%80d_%80d_%80d", &phi, &eta, &ml));
        if (count_items < 2) return false;
        if (count_items != 3) ml = 0;
        return true;
    }

    inline void CalibrationFileIOTool::read_rt_relation(const std::string &fname, std::map<NtupleStationId, IRtRelation *> &rts,
                                                        std::map<NtupleStationId, IRtResolution *> &res_map,
                                                        const MuonCalib::NtupleStationId &id) {
        // open file
        std::ifstream infile(fname.c_str());
        if (infile.fail()) {
            ATH_MSG_ERROR("Cannot open file '" << fname << "' for reading!");
            return;
        }
        // sample points
        std::vector<double> r, t, res;
        std::string sdummy;
        double dummy;
        // ignore the first line
        getline(infile, sdummy);
        // read table
        float multilayer_diff(9e9);
        while (!(infile.eof() || infile.fail())) {
            infile >> dummy;
            if (infile.eof() || infile.fail() || dummy > 15.0) break;
            if (dummy < -9998) {
                infile >> multilayer_diff;
                infile >> dummy;
            } else {
                r.push_back(dummy);
                infile >> dummy;
                t.push_back(dummy);
                infile >> dummy;
                res.push_back(dummy);
            }
        }
        if (r.size() < 3) {
            ATH_MSG_WARNING("Not enough good rt points for " << id.regionId() << "!");
            return;
        }
        std::vector<MuonCalib::SamplePoint> point(r.size());
        for (unsigned int k = 0; k < point.size(); k++) {
            point[k].set_x1(t[k]);
            point[k].set_x2(r[k]);
            point[k].set_error(1.0);
        }
        // convert rt relation
        MuonCalib::RtFromPoints rt_from_points;
        if (m_rt_lookup) {
            rts[id] = new MuonCalib::RtRelationLookUp(rt_from_points.getRtRelationLookUp(point));
        } else {
            rts[id] = new MuonCalib::RtChebyshev((rt_from_points.getRtChebyshev(point, 10)));
        }
        if (multilayer_diff < 8e8) rts[id]->SetTmaxDiff(multilayer_diff);
        // create resolution function
        MuonCalib::RtResolutionFromPoints res_from_points;
        for (unsigned int k = 0; k < point.size(); k++) {
            point[k].set_x1(t[k]);
            point[k].set_x2(res[k]);
            point[k].set_error(1.0);
        }
        res_map[id] = new MuonCalib::RtResolutionChebyshev(res_from_points.getRtResolutionChebyshev(point, 8));
    }  // end CalibrationFileIOTool::read_rt_relation

}  // namespace MuonCalib
