/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNTOOLS_MUONHOUGHPATTERNTOOL_H
#define MUONHOUGHPATTERNTOOLS_MUONHOUGHPATTERNTOOL_H

#include <TFile.h>
#include <TH2.h>

#include "AthenaBaseComps/AthAlgTool.h"
#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"
#include "MuonRecToolInterfaces/IMuonHoughPatternTool.h"

class MuonHoughTransformSteering;
class MuonHoughTransformer;

class MuonHoughPatternTool : virtual public IMuonHoughPatternTool, public AthAlgTool {
public:
    /** Default constructor */
    MuonHoughPatternTool(const std::string& type, const std::string& name, const IInterface* parent);
    /** Destructor */
    virtual ~MuonHoughPatternTool() = default;

    /** method that builds the patterns */
    virtual void makePatterns(const MuonHoughHitContainer& hitcontainer, MuonHoughPatternContainerShip& houghpatterns) const override;

    /** initiates private members */
    virtual StatusCode initialize() override;

    /** deletes private members */
    virtual StatusCode finalize() override;

    /** returns phipattern container in EDM */
    virtual std::unique_ptr<MuonPrdPatternCollection> getPhiMuonPatterns(MuonHoughPatternContainerShip& houghpatterns) const override;
    /** returns etapattern container in EDM */
    virtual std::unique_ptr<MuonPrdPatternCollection> getEtaMuonPatterns(MuonHoughPatternContainerShip& houghpatterns) const override;

    /** creates houghpatterns, called from FinderTool */
    virtual MuonHoughPatternContainerShip emptyHoughPattern() const override;

private:
    /** method that builds the patterns */
    void makePatterns(int id_number, double weightmdt, const MuonHoughHitContainer& event,
                      MuonHoughPatternContainerShip& houghpatterns) const;

    /** returns curvedpattern container in EDM */
    std::unique_ptr<MuonPrdPatternCollection> getCurvedMuonPatterns(MuonHoughPatternContainerShip& houghpatterns) const;

    /** reset association flag of hits in m_event */
    static void resetAssociation(const MuonHoughHitContainer& event);

    /** reduces Detector sizes for Hough Histograms to find patterns from muons from the Interaction Point (default on) */
    void useIPMuons();

    /**
     * @brief fills the hough histograms
     * @param[in] id_number The enum number corresponding to the HoughTransform
     * @param[in] level The iteration number
     * @param[in] event_to_analyse The hitcontainer which fills the histogram
     * @param[in] houghtransform The HoughTransform
     */
    void fillHistos(int id_number, int level, const MuonHoughHitContainer* event_to_analyse,
                    MuonHoughTransformSteering* houghtransform) const;
    /**
     * @brief analyses the hough histograms
     * @param[in] id_number The enum number corresponding to the HoughTransform
     * @param[in] level The iteration number
     * @param[in] event_to_analyse The hitcontainer which will be associated to the pattern
     * @param[in] houghtransform The HoughTransform
     */
    bool analyseHisto(int id_number, int level, const std::unique_ptr<MuonHoughHitContainer>& event_to_analyse,
                      std::unique_ptr<MuonHoughTransformSteering>& houghtransform, MuonHoughPatternContainerShip& houghpatterns) const;

    /** returns if there are hits left */
    static bool hitsLeft(const MuonHoughHitContainer& event);

    /** returns number of hits left (unused) */
    int numberOfHits(const MuonHoughHitContainer& event) const;

    /** checks if hit is already in one of the found houghpatterns (unused) */
    bool hitInHoughPattern(MuonHoughHit* hit, const MuonHoughPatternContainer& houghpattern) const;

    /** returns a hitcontainer with hits not yet used in pattern */
    static std::unique_ptr<MuonHoughHitContainer> hitsNotInPattern(const MuonHoughHitContainer& event, int id_number);

    /** rescales hits per iteration to reduce number of patterns when already some have been found */
    void weightRescaling(const MuonHoughHitContainer& event, int id_number, int level) const;

    /** calculates new weights based on rejection factor (1-origweight) and number of hits in event, only done for MDTs */
    void calculateWeights(const MuonHoughHitContainer& event, double weightmdt) const;

    /** returns number of hits that are in both hough patterns */
    int overlapHoughPatterns(const MuonHoughPattern& houghpattern1, const MuonHoughPattern& houghpattern2) const;

    /** selects the hitcontainer to be used for filling the histograms */
    std::unique_ptr<MuonHoughHitContainer> whichEventHough(int id, const MuonHoughHitContainer& event, double weightmdt) const;

    /** selects the hitcontainer to be used to associate to the maxima */
    std::unique_ptr<MuonHoughHitContainer> whichEventAssociation(int id, const MuonHoughHitContainer& event) const;

    /** returns the Houghtransform for the id */
    std::unique_ptr<MuonHoughTransformSteering> whichHoughTransform(int id) const;

    /** returns the maximum iteration, not in use */
    std::vector<int> maxLevelHoughPattern(const MuonHoughPatternContainerShip& houghpattern) const;

    /** returns the maximum iteration, not in use */
    /** @todo should be in houghpattern? */
    int maxLevelHoughPattern(const MuonHoughPatternContainerShip& houghpattern, int id_number) const;

    /** corrects the maximum of the histogram with a factor (not in use anymore, used for old rz transform) */
    void transformCoordsMaximum(std::pair<double, double>& coordsmaximum, double r0_true) const;

    /** converts hough pattern to EDM eta patterns */
    std::unique_ptr<Muon::MuonPrdPattern> houghPatternToEtaPattern(const MuonHoughPattern& houghpattern) const;
    /** converts hough pattern to EDM phi patterns */
    std::unique_ptr<Muon::MuonPrdPattern> houghPatternToPhiPattern(const MuonHoughPattern& houghpattern) const;
    /** converts and combines two hough patterns to one EDM phi pattern */
    std::unique_ptr<Muon::MuonPrdPattern> houghPatternsToOneEtaPattern(const MuonHoughPattern& houghpattern1,
                                                                       const MuonHoughPattern& houghpattern2) const;
    /** converts and combines two hough patterns to one EDM phi pattern */
    std::unique_ptr<Muon::MuonPrdPattern> houghPatternsToOnePhiPattern(const MuonHoughPattern& houghpattern1,
                                                                       const MuonHoughPattern& houghpattern2) const;
    /** converts hough pattern to EDM phi patterns and cleans it from outliers */
    std::unique_ptr<Muon::MuonPrdPattern> houghPatternToCleanPhiPattern(MuonHoughPattern& houghpattern) const;

    /** returns minimum number of hits a hough pattern can contain */
    unsigned int getThresholdHoughPattern(int id_number) const;
    /** returns minimum number for the maximum of a hough transform */
    double getThresholdHisto(int id_number) const;

    /** calculates the mdt weight cut value */
    void setWeightMdtCutValue(const MuonHoughHitContainer& event, double& weightmdt) const;

    /** hit through weight cut? */
    bool hitThroughCut(MuonHoughHit* hit, double weightmdt) const;

    /** pointer to the file name for the hough histograms */
    std::unique_ptr<TFile> m_file;
    /** object for use of mathematical formulas for trackmodels */
    MuonHoughMathUtils m_muonhoughmathutils;

    /** output histograms (false) */
    Gaudi::Property<bool> m_use_histos{this, "UseHistos", false};
    /** use cosmic settings (false) */
    Gaudi::Property<bool> m_use_cosmics{this, "UseCosmics", false};
    /** use csc hits in association / pattern (true) */
    Gaudi::Property<bool> m_use_csc_in_pattern{this, "UseCscInPattern", true};
    /** use csc hits in histogram (false) */
    Gaudi::Property<bool> m_use_csc_in_hough{this, "UseCscInHough", true};
    /** use negative weights (false) */
    Gaudi::Property<bool> m_use_negative_weights{this, "UseNegativeWeights", false};
    /** use curved hough transformation for eta patterns (true) */
    Gaudi::Property<bool> m_use_curvedhough{this, "UseCurvedHough", true};

    /** number of hough transforms currently supported (7) */
    int m_number_of_ids{7};

    /** number of iterations (5) */
    Gaudi::Property<int> m_number_of_maxima{this, "NumberOfMaximaPerIterations", 5};

    /** use rpc phi strips in phi-patterns (true) */
    // const bool m_use_rpc_measures_phi;
    /** use rpc eta strips in eta-patterns (true) */
    static constexpr bool m_use_rpc_measures_eta{true};
    /** use interaction point constraint (true) */
    bool m_use_ip{false};

    /** minimal size for a phi pattern (1) */
    Gaudi::Property<unsigned int> m_thresholdpattern_xyz{this, "SetThresholdPatternRPhi", 1};
    /** minimal size for a eta pattern (3) */
    Gaudi::Property<unsigned int> m_thresholdpattern_rz{this, "SetThresholdPatternREta", 3};

    // hittosegment association:

    /** distance hits are associated with pattern in mm */
    static constexpr double m_maximum_residu_mm{500.};
    /** distance hits are associated with pattern in mm for cosmics */
    static constexpr double m_maximum_residu_mm_cosmics{2000.};
    /** distance hits are associated with pattern in degrees */
    static constexpr double m_maximum_residu_angle{3.};

    /** // number of maximum iterations over houghtransform */
    static constexpr int m_maximum_level{5};
    // for hough_correction:
    /** use hough correction
     * to correct the maximum found in rz-plane slightly as there is a bias in the houghtransform
     */
    // const bool m_use_hough_correction;
    /** constant 1 for z for hough correction */
    static constexpr double m_z_cor_constant{-10000.};
    /** constant 2 for z for hough correction */
    static constexpr double m_z_cor_constant2{6000.};
    /** constant 1 for theta for hough correction */
    static constexpr double m_theta_cor_constant{-0.042};
    /** constant 2 for theta for hough correction */
    static constexpr double m_theta_cor_constant2{4000.};

    // detectorsizes:

    /** acceptancy of patterns in xy (phi) in mm */
    double m_detectorsize_xy{0.};
    /** acceptancy of patterns in yz (not used) in mm */
    double m_detectorsize_yz{0.};
    /** acceptancy of patterns in rz (eta) in mm */
    double m_detectorsize_rz{0.};
    /** size of full detector in xy (phi) in mm, used as acceptancy for cosmics */
    static constexpr double m_detectorsize_xy_full{15000.};
    /** size of full detector in yz (not used) in mm, used as acceptancy for cosmics */
    static constexpr double m_detectorsize_yz_full{25000.};
    /** size of full detector in rz (eta) in mm, used as acceptancy for cosmics */
    static constexpr double m_detectorsize_rz_full{27750.};

    /** acceptancy of patterns for ip in xy (phi) in mm */
    static constexpr double m_detectorsize_xy_ip{600.};
    /** acceptancy of patterns for ip in yz (not used) in mm */
    static constexpr double m_detectorsize_yz_ip{1000.};
    /** acceptancy of patterns for ip in rz (eta) in mm */
    static constexpr double m_detectorsize_rz_ip{1500.};

    /** max range of angle in xyz in degrees (360) */
    static constexpr double m_detectorsize_angle_xyz{360.};
    /** max range of angle in rz in degrees (180) */
    static constexpr double m_detectorsize_angle_rz{180.};
    /** max range of 1/sqrt(curvature) for curved transform, corresponds to 0.02 ~ 2,5m ~ 1.6GeV */
    // const double m_detectorsize_inv_sqrt_curvature;

    // properties of histograms:

    /** bin width for xy */
    static constexpr double m_stepsize_xy{75.};
    /** bin width for yz */
    static constexpr double m_stepsize_yz{250.};
    /** bin width for rz */
    static constexpr double m_stepsize_rz{75.};
    /** bin width for xy cosmics */
    static constexpr double m_stepsize_xy_cosmics{150.};
    /** bin width for rzcosmics */
    static constexpr double m_stepsize_rz_cosmics{150.};
    /** bin width for angle in xyz */
    static constexpr double m_stepsize_per_angle_xyz{0.25};
    /** bin width for angle in rz */
    static constexpr double m_stepsize_per_angle_rz{0.25};
    /** bin width for angle in xy cosmics */
    static constexpr double m_stepsize_per_angle_xy_cosmics{1.};
    /** bin width for angle in rzcosmics */
    static constexpr double m_stepsize_per_angle_rz_cosmics{2.};
    /** bin width for 1/sqrt(curvature) */
    // const double m_stepsize_per_inv_sqrt_curvature;

    /** new curved properties */
    static constexpr int m_nbins_curved{160};

    /** weight_cut for hits in hough */
    Gaudi::Property<bool> m_weightcut{this, "ApplyWeightCut", true};

    /** value of weight cut */
    Gaudi::Property<double> m_weight{this, "WeightCut", 0.25};  // cut all hits under 0.25

    /** weight_cut for mdt hits in hough */
    Gaudi::Property<bool> m_weightcutmdt{this, "ApplyWeightCutMdt",
                                         true};  // cut all mdt hits under a certain weight (dependent on number of mdt hits in event

    /** threshold histogram in xyz */
    Gaudi::Property<double> m_thresholdhisto_xyz{this, "SetThresholdHistoRPhi", 0.9};
    /** threshold histogram in rz */
    Gaudi::Property<double> m_thresholdhisto_rz{this, "SetThresholdHistoREta", 2.1};

    /** number of sectors (different regions in which patterns can be found in the same iteration) in xyz */
    Gaudi::Property<int> m_number_of_sectors_xyz{this, "SetNumberOfSectorsRPhi", 12};
    /** number of sectors (different regions in which patterns can be found in the same iteration) in rz */
    Gaudi::Property<int> m_number_of_sectors_rz{this, "SetNumberOfSectorsREta", 16};
    /** number of sectors (different regions in which patterns can be found in the same iteration) in rzcosmics */
    int m_number_of_sectors_rz_cosmics{12};

    /** output level (range 0-10) (default 0) */
    Gaudi::Property<int> m_printlevel{this, "Printlevel", 0};

    mutable std::atomic_uint m_ncalls{0};

    /** maximum number of phi hits to do pattern recognition, if small zero no cut is applied */
    Gaudi::Property<int> m_maxNumberOfPhiHits{this, "MaximumNumberOfPhiHits", -1};

    /** print out pattern hits */
    void printPattern(Muon::MuonPrdPattern* muonpattern) const;
};

#endif  // MUONHOUGHPATTERNTOOLS_MUONHOUGHPATTERNTOOL_H
