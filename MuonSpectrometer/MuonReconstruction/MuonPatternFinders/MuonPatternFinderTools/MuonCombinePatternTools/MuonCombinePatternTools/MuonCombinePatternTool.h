/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEPATTERNTOOLS_MUONCOMBINEPATTERNTOOL_H
#define MUONCOMBINEPATTERNTOOLS_MUONCOMBINEPATTERNTOOL_H

#include "MuonRecToolInterfaces/IMuonCombinePatternTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonHoughPatternEvent/MuonHoughMathUtils.h"

#include <cmath>

class MuonCombinePatternTool : public AthAlgTool, virtual public Muon::IMuonCombinePatternTool 
{
 private:
  struct ChamberInfo {
    ChamberInfo() : neta(0),nphi(0),ninside(0),noutside(0),ninsidePat(0),noutsidePat(0),phiMin(1e9),phiMax(-1e9) {}
    int neta;
    int nphi;
    int ninside;
    int noutside;
    int ninsidePat;
    int noutsidePat;
    double phiMin;
    double phiMax;
  };
  typedef std::map<Identifier, ChamberInfo> IdChMap;
  typedef IdChMap::iterator                 IdChIt;

 public:
  MuonCombinePatternTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~MuonCombinePatternTool()=default;

  virtual StatusCode initialize(); 

  /** Combines phi and eta pattern collection into a new combined pattern collection */
  virtual const MuonPrdPatternCollection* combineEtaPhiPatterns(const MuonPrdPatternCollection* phiPatternCollection, const MuonPrdPatternCollection* etaPatternCollection,
								const std::map <const Trk::PrepRawData*, std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >* phiEtaHitAssMap)const;

  /** Combines phi and eta pattern into a new combined pattern */ 
  virtual Muon::MuonPrdPattern* makeCombinedPattern(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern)const;

  /** converts MuonPrdPatterns into MuonPatternCombinationCollection
   * MuonPatternCombinationCollection are default output for PatternFinder */
  virtual MuonPatternCombinationCollection* makePatternCombinations(const MuonPrdPatternCollection* muonpatterns)const;

 private: 

  /** make combined pattern from all candidates, removes duplicates with phi when no overlap with eta pattern */
  const MuonPrdPatternCollection* makeCombinedPatterns(std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*> > &candidates)const;

  /** number of associated hits between patterns, returns by reference the phi hits to be added based on association with eta hits */
  int overlap(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern, std::vector<const Trk::PrepRawData*> &associated_phihits_notonphipattern,
	      const std::map <const Trk::PrepRawData*, std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >* phiEtaHitAssMap)const;

  /** is pattern1 a complete subset of other pattern2? */
  bool subset(const Muon::MuonPrdPattern* pattern1, const Muon::MuonPrdPattern* pattern2)const;

  /** is candidate1 a complete subset of other candidate2? */
  bool subset(std::pair<std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess>, std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >& candidate1, std::pair<std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess>, std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >& candidate2)const;

  /** add associated phi hits to phi pattern (factory: builds new Muon::MuonPrdPattern) */
  const Muon::MuonPrdPattern* updatePhiPattern(const Muon::MuonPrdPattern* phipattern, std::vector<const Trk::PrepRawData*> missedphihits)const;

  /** clean phi pattern, similar as in MuonHoughPatternTool, used for newly created phi patterns based on hit association */
  Muon::MuonPrdPattern* cleanPhiPattern(const Muon::MuonPrdPattern* phipattern)const;

  /** clean combined pattern, remove outliers */
  Muon::MuonPrdPattern* cleanupCombinedPattern(Muon::MuonPrdPattern* &combinedpattern)const;

  /** split patterns in two at point closest to IP in rphi */
  std::vector <std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*> > splitPatterns2D(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern)const;

  /** split patterns in two at point closest to IP in 3D */
  std::vector <std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*> > splitPatterns3D(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern)const;

  /** split patterns in two when crossing calorimeter at point closest to IP in 3D (should be same as splitting at calorimeter)
      if not split empty vector is returned */
  std::vector <std::pair<Muon::MuonPrdPattern*, Muon::MuonPrdPattern*> > splitPatternsCylinder(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern)const;

  /** make combined phi pattern by associating phi hits to noncombined eta pattern, return 0 if no phi measurements added, 2nd argument is if checking that added phi hits are already on pattern (not necessary for uncombined etapattern) */
  Muon::MuonPrdPattern* makeAssPhiPattern(const Muon::MuonPrdPattern* pattern, 
					  const std::map <const Trk::PrepRawData*, std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >* phiEtaHitAssMap,
					  bool check = false)const;

  /** calculate new track parameters of match (only for cosmics!) returns [r0, phi, rz0, theta]*/
  double* updateParametersForCosmics(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern)const;

  /** calculate phi and r0 for cosmic patterns, phi estimate needs to be given */
  std::pair<double,double> calculateR0Phi(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern, double phi_estimate=-M_PI_2)const;

  /** calculate rz0 for cosmic pattern */
  double calculateRz0(const Muon::MuonPrdPattern* pattern, double phi, double theta)const;

  /** update patterns based on new track parameters (used only for cosmics) builds 2 new prd patterns */
  std::pair<const Muon::MuonPrdPattern*,const Muon::MuonPrdPattern*> updatePatternsForCosmics(const Muon::MuonPrdPattern* phipattern, const Muon::MuonPrdPattern* etapattern, double* new_pars)const;

  /** calculates global position of prd */
  const Amg::Vector3D& globalPrdPos( const Trk::PrepRawData* prd ) const;

  /** adds eta,phi pair to candidate vector, also performs splitting and associated pattern (only for cosmics!)*/
  void addCandidate(const Muon::MuonPrdPattern* etapattern, const Muon::MuonPrdPattern* phipattern, std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*> > &candidates, bool add_asspattern, std::vector<const Muon::MuonPrdPattern*>& patternsToDelete,
		    const std::map <const Trk::PrepRawData*, std::set<const Trk::PrepRawData*,Muon::IdentifierPrdLess> >* phiEtaHitAssMap)const; 

  /** clean candidates from subsets or duplicates */
  void cleanCandidates(std::vector<std::pair<const Muon::MuonPrdPattern*, const Muon::MuonPrdPattern*> > &candidates)const;

  /** print out pattern hits */
  void printPattern(const Muon::MuonPrdPattern* muonpattern)const;

  /** object for use of mathematical formulas for trackmodels */
  MuonHoughMathUtils m_muonHoughMathUtils;

  /** distance cut in xy for hits */
  const double m_maximum_xydistance;
  /** distance cut in rz for hits */
  const double m_maximum_rzdistance;

  /** use cosmic settings */
  bool m_use_cosmics;

  /** split patterns (only for cosmics) */
  bool m_splitpatterns;

  /** don't discard any candidates based on gasgap assocation (true by default) */
  bool m_nodiscarding;

  /** take only best phi match as candidate or take all phi matches (false by default, but true for cosmics) */
  bool m_bestphimatch;

  /** flip direction for cosmics after splitting as if coming from IP (false by default) */
  bool m_flipdirectionforcosmics;

  bool m_useTightAssociation;
  unsigned int m_maxSizePhiPatternLoose;
  unsigned int m_maxSizeEtaPatternLoose;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

};

#endif //MUONCOMBINEPATTERNTOOLS_MUONCOMBINEPATTERNTOOL_H
