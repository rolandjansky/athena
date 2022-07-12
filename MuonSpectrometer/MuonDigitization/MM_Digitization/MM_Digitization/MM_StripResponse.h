/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_DIGITIZATION_MM_STRIPRESPONSE_H
#define MM_DIGITIZATION_MM_STRIPRESPONSE_H
//
// MM_MM_StripResponse.cxx
//     Simulate strip response
//

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "MM_Digitization/MM_Electron.h"
#include "MM_Digitization/MM_IonizationCluster.h"

class MM_StripResponse {
public:
    MM_StripResponse() = default;
    MM_StripResponse(std::vector<std::unique_ptr<MM_IonizationCluster>>& IonizationClusters, float timeResolution, float stripPitch,
                     int stripID, int minstripID, int maxstripID);
    void timeOrderElectrons();
    void calculateTimeSeries(float thetaD, int gasgap);

    void simulateCrossTalk(float crossTalk1, float crossTalk2);
    void calculateSummaries(float chargeThreshold);
    const std::map<int, int>& getTimeThreshold() const;
    const std::map<int, float>& getTotalCharge() const;
    const std::map<int, float>& getMaxCharge() const;
    const std::map<int, int>& getTimeMaxCharge() const;

    const std::vector<int>& getStripVec() const;
    const std::vector<std::vector<float>>& getTimeThresholdVec() const;
    const std::vector<std::vector<float>>& getTotalChargeVec() const;
    const std::vector<float>& getMaxChargeVec() const;
    const std::vector<float>& getTimeMaxChargeVec() const;

    int getNElectrons();
    float getTotalCharge();
    std::vector<std::unique_ptr<MM_Electron>>& getElectrons();

private:
    float m_timeResolution{0.f};
    float m_stripPitch{0.f};
    int m_stripID{0};
    int m_minstripID{0};
    int m_maxstripID{0};

    std::vector<std::unique_ptr<MM_Electron>> m_Electrons{};

    // First argument is time bin, second argument is strip ID
    std::map<int, std::map<int, float>> m_stripCharges{};

    // Useful info for clustering later
    std::map<int, int> m_stripTimeThreshold{};
    std::map<int, float> m_stripTotalCharge{};
    std::map<int, float> m_stripMaxCharge{};
    std::map<int, int> m_stripTimeMaxCharge{};

    // using vector for the moment -- hopefully this has better access and is not so expensive on the memory
    std::vector<int> m_v_strip{};
    std::vector<std::vector<float>> m_v_stripTimeThreshold{};
    std::vector<std::vector<float>> m_v_stripTotalCharge{};
    std::vector<float> m_v_stripMaxCharge{};
    std::vector<float> m_v_stripTimeMaxCharge{};
};

#endif
