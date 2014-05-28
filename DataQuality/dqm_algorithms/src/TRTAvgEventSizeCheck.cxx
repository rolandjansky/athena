/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ONLINE // can only be built in an online environment

#include "dqm_algorithms/TRTAvgEventSizeCheck.h"
#include "dqm_algorithms/tools/AlgorithmHelper.h"

#include "dqm_core/AlgorithmManager.h"
#include "dqm_core/AlgorithmRegistrator.h"
#include "dqm_core/AlgorithmConfig.h"
#include "ers/ers.h"

#include "ipc/partition.h"
#include "is/inforeceiver.h"
#include "is/callbackinfo.h"
#include "is/infoT.h"

#include <limits>
#include <cmath>

namespace /* anonymous */ {

class TRTBeamConditions : public ISInfoReceiver
{
public:
    static TRTBeamConditions &instance();
    float get();
    virtual ~TRTBeamConditions() {}

private:
    TRTBeamConditions();
    void callback(ISCallbackInfo *info);

private:
    static const char *m_name;
    boost::mutex m_mutex; // not really required here because m_value is of primitive type float
    float m_value;
};

const char *TRTBeamConditions::m_name("beamconditions.hitfractionTRT_longToT");

TRTBeamConditions &TRTBeamConditions::instance()
{
    static TRTBeamConditions s_instance;
    return s_instance;
}

float TRTBeamConditions::get()
{
    boost::mutex::scoped_lock lock(m_mutex);
    return m_value;
}

TRTBeamConditions::TRTBeamConditions() : ISInfoReceiver(IPCPartition("initial")), m_mutex(), m_value(0)
{
    try {
        subscribe(m_name, &TRTBeamConditions::callback, this);
    } catch (daq::is::Exception &ex) {
        ERS_LOG("Subscribing to " << m_name << " IS Info failed: " << ex);
    }
}

void TRTBeamConditions::callback(ISCallbackInfo *info)
{
    try {
        ISInfoFloat hitfraction;
        info->value(hitfraction);

        const long long timestamp = info->time().total_mksec_utc();
        const long long now       = OWLTime().total_mksec_utc();
        const long long maxAge    = 5 * 60 * 1000000; // five minutes

        boost::mutex::scoped_lock lock(m_mutex);
        if (now - timestamp < maxAge) {
            m_value = hitfraction.getValue();
        } else {
            m_value = 0; // reject old values
        }
    } catch (daq::is::Exception &ex) {
        ERS_LOG("Receiving " << m_name << " IS Info failed: " << ex);
    }
}
  
dqm_core::AlgorithmRegistrator<dqm_algorithms::TRTAvgEventSizeCheck> __ii__("TRTAvgEventSizeCheck");

} // namespace



dqm_algorithms::TRTAvgEventSizeCheck::TRTAvgEventSizeCheck() : lastCollisionsSeen_(boost::posix_time::min_date_time) // never
{
    (void)TRTBeamConditions::instance(); // already initiate the subscription
}

dqm_algorithms::TRTAvgEventSizeCheck *dqm_algorithms::TRTAvgEventSizeCheck::clone()
{
    return new TRTAvgEventSizeCheck();
}

dqm_core::Result *dqm_algorithms::TRTAvgEventSizeCheck::execute(const std::string &name, const TObject &object, const dqm_core::AlgorithmConfig &config)
{
    if (!object.IsA()->InheritsFrom("TH1")) {
        throw dqm_core::BadConfig(ERS_HERE, name, "histogram does not inherit from TH1");
    }
    const TH1 &histogram = dynamic_cast<const TH1 &>(object); // should be safe after the previous check
    if (histogram.GetDimension() > 1) {
        throw dqm_core::BadConfig(ERS_HERE, name, "histogram has more than one dimension");
    }

    const double noiseLimit = dqm_algorithms::tools::GetFirstFromMap("NoiseLimit", config.getParameters());
    const double delaySeconds = dqm_algorithms::tools::GetFirstFromMap("DelayTime", config.getParameters(), 60);
    const boost::posix_time::time_duration delayTime = boost::posix_time::seconds(delaySeconds); // same value

    const double hitFraction = TRTBeamConditions::instance().get();
    const bool collisions = (hitFraction > noiseLimit);
    const bool unknown = (hitFraction == 0); // the beam monitoring is just starting or the published value is too old

    const boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    if (collisions) lastCollisionsSeen_ = now;
    const bool acceptLargeEvents = (now <= (lastCollisionsSeen_ + delayTime) || unknown); // also accept large events if the beam monitoring is not working

    const char *avgThresholdName = acceptLargeEvents ? "Avg_Collisions" : "Avg_Noise"; // select which set of thresholds to use
    const double avgG = dqm_algorithms::tools::GetFromMap(avgThresholdName, config.getGreenThresholds());
    const double avgR = dqm_algorithms::tools::GetFromMap(avgThresholdName, config.getRedThresholds());

    double sum  = 0;
    double sum2 = 0;
    int spyReadoutEnabled  = 0;
    int spyReadoutDisabled = 0;

    for (int i = 1; i <= histogram.GetNbinsX(); ++i) {
        const double binContent = histogram.GetBinContent(i);
        if (binContent) { // skip zero bins
            sum  += binContent;
            sum2 += binContent * binContent;
            ++spyReadoutEnabled;
        } else {
            ++spyReadoutDisabled;
        }
    }

    const int &N = spyReadoutEnabled;                           // just a shorter name for this
    const double avg = N ? sum / N                         : 0; // protect against division by zero
    const double rms = N ? std::sqrt(sum2 / N - avg * avg) : 0; // protect against division by zero

    dqm_core::Result *result = new dqm_core::Result;

    if      (avg <=    0) result->status_ = dqm_core::Result::Undefined; // negative values should never appear
    else if (avg <= avgG) result->status_ = dqm_core::Result::Green;
    else if (avg <  avgR) result->status_ = dqm_core::Result::Yellow;
    else                  result->status_ = dqm_core::Result::Red;

    result->tags_["Avg"] = avg;
    result->tags_["RMS"] = rms;
    result->tags_["SpyReadoutDisabled"] = spyReadoutDisabled;
    result->tags_["_HitFraction_"] = hitFraction;
    result->tags_["_Collisions_"] = collisions;
    result->tags_["_AcceptLargeEvents_"] = acceptLargeEvents;
    return result;
}

void dqm_algorithms::TRTAvgEventSizeCheck::printDescription(std::ostream& out)
{
    out << "TRTAvgEventSizeCheck: checks the average event size from the TRT RODs.\n"
        "The algorithm applies two different sets of thresholds, depending on whether we have collisions or not.\n"
        "Mandatory parameters:\n"
        "    NoiseLimit: if \"hitfractionTRT_longToT\" is above this value, the algorithm assumes we are having collisions.\n"
        "Optional parameters:\n"
        "    DelayTime: how long after collisions the algorithm still accepts large events (in seconds, default = 60).\n"
        "Mandatory thresholds:\n"
        "    Avg_Collisions: the allowed average event size when we are having collisions.\n"
        "    Avg_Noise: the allowed average event size when we are not having collisions.\n"
        "Results:\n"
        "    Avg: the average event size, averaged over all RODs in the histogram, excluding the ones with disabled spy readout.\n"
        "    RMS: the RMS of the average event size distribution, excluding the ones with disabled spy readout.\n"
        "    SpyReadoutDisabled: the number of RODs with disabled spy readout.\n"
        "    _HitFraction_: the value of \"hitfractionTRT_longToT\", mirrored as a DQMF result.\n"
        "    _Collisions_: is \"hitfractionTRT_longToT\" currently above the NoiseLimit?\n"
        "    _AcceptLargeEvents_: are we still in the DelayTime after collisions?" << std::endl;
}

#endif // ONLINE
