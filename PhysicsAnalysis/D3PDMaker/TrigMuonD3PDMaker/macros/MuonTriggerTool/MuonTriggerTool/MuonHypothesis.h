/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonHypotheis_h
#define MuonHypotheis_h
/**
 * @author  Toyonobu Okuyama
 * @author  Takashi Matsushita
 */


#include <map>
#include <string>

/**
 *  This class implements methods to apply pt thresholds for EF muon trigger object
 */
class MuonHypothesis {
  public:
    MuonHypothesis();
    virtual ~MuonHypothesis();

    /** tests if the combination of given pt [in MeV] and eta passes eta dependent thresholds of the given chain
     *
     * @param pt [in] pt of an EF muon trigger object in MeV
     * @param eta [in] eta of an EF muon trigger object
     * @param chain [in] EF chain name. ex) "EF_mu18"
     */
    static bool EF_isPassed(const float pt,
                            const float eta,
                            const std::string& chain);

    typedef std::pair<const std::string, const double *> EF_thresholdPair;
    typedef std::map<const std::string, const double *> EF_thresholdMap;

  private:
    static const double* getThresholds(const std::string& chain);
    static bool EF_isPassed(const float pt,
                            const float eta,
                            const double* threshold);
};
#endif
// eof
