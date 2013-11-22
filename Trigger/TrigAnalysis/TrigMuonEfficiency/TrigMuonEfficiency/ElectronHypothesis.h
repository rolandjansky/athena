/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElectronHypotheis_h
#define ElectronHypotheis_h
/**
 * @author  Takashi Matsushita
 */


#include <map>
#include <string>

/**
 *  This class implements methods to apply pt thresholds for EF muon trigger object
 */
class ElectronHypothesis {
  public:
    ElectronHypothesis() {};
    virtual ~ElectronHypothesis() {};

    /** tests if the combination of given pt [in MeV] and eta passes eta dependent thresholds of the given chain
     *
     * @param pt [in] et of an EF electron trigger object in MeV
     * @param chain [in] EF chain name.
     */
    static bool EF_isPassed(const float et,
                            const std::string& chain);

    typedef std::pair<const std::string, const double> EF_thresholdPair;
    typedef std::map<const std::string, const double> EF_thresholdMap;

    static bool EF_isPassed(const float et,
                            const double threshold);

  private:
    static double getThresholds(const std::string& chain);
};
#endif
// eof
