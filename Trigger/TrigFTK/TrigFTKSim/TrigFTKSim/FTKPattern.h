/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKPattern_H
#define FTKPattern_H

#include <stdint.h>

#include <TObject.h>
#include <TClonesArray.h>

#include <vector>
#include <list>

/** implementation of a class storing a Pattern, with respect to a road
    a pattern, with the SSIDs */
class FTKPattern : public TObject {
private:
  int m_patternID; // pattern identification
  int m_patternDBID; // pattern identification

  int m_nplanes; // number of planes
  int *m_ssid; //[m_nplanes] SS id on each plane

  int m_sectorID; // sector containing this pattern
  int m_coverage; // number of events that created this pattern

  int m_dcmask; // DC mask for this pattern
  int m_hbmask; // half-bin mask

  int m_nsubpatterns; // numebr of sub-patterns
  TClonesArray *m_subpatterns; //-> Array of sub-patterns

  unsigned int *m_hashes; //! if generated store the hashes
  static uint32_t crc32_tab[];
  static unsigned int crc32(const void *buf, size_t, unsigned int);

  static bool m_orderByCoverage; //! static property to define if a bank is ordered by the coverage or ids content

public:
  FTKPattern();
  FTKPattern(int);
  FTKPattern(const FTKPattern&);
  virtual ~FTKPattern();

  static bool OrderByCoverage(bool flag) { return m_orderByCoverage = flag; }
  static bool OrderByCoverage() { return m_orderByCoverage; }

  void setPatternID(int id) { m_patternID = id; }
  const int& getPatternID() const { return m_patternID; }

  void setPatternDBID(int id) { m_patternDBID = id; }
  const int& getPatternDBID() const { return m_patternDBID; }

  void setSectorID(int id) { m_sectorID = id; }
  const int& getSectorID() const { return m_sectorID; }

  void setNPlanes(int);
  const int& getNPlanes() const { return m_nplanes; }

  void setCoverage(int val) { m_coverage = val; }
  void addCoverage(int val) { m_coverage += val; }
  const int& getCoverage() const { return m_coverage; }

  void setSSID(int plane, int ssid) { m_ssid[plane] = ssid; }
  const int& getSSID(int plane) const { return m_ssid[plane]; }
  const int* getSSID() const { return m_ssid; }

  void clearSubPatterns();
  int getNSubPatterns() const { return m_nsubpatterns; }
  FTKPattern* addSubPattern(FTKPattern&);
  const FTKPattern* getSubPattern(int i) const { return dynamic_cast<FTKPattern*>(m_subpatterns->At(i)); }

  const int &getDCMask() const { return m_dcmask; }
  void setDCMask(int mask) { m_dcmask = mask; }

  const int &getHBMask() const { return m_hbmask; }
  void setHBMask(int mask) { m_hbmask = mask; }

  bool operator<(const FTKPattern&);

  FTKPattern& operator=(const FTKPattern&);

  friend std::istream& operator>>(std::istream&,FTKPattern&);

  friend std::ostream& operator<<(std::ostream&,FTKPattern&);

  friend class FTKTSPBank;

  void generateHashes();
  unsigned int getHash(int);
  
  void dump(std::ostream&);

  virtual void Print(const Option_t *opt="") const;

  static bool doStaticRoadWarrior(const FTKPattern &,
				  const FTKPattern &,
				  const FTKPattern &,
				  const FTKPattern &);

  ClassDef(FTKPattern,2) 
};



class FTKPatternKDTree {
private:
  int m_dim; // dimension of the space
  int m_depth; // depth level, 0 is the top level
  int m_splitplane; // coordinate compated at this level
  FTKPattern *m_node; // element
  int m_pos; // position
  FTKPatternKDTree *m_parent; // parent element
  FTKPatternKDTree *m_left; // left sub-tree
  FTKPatternKDTree *m_right; // right sub-tree

  int m_maxdepth; // max depth
  int m_maxdepth_l; 
  int m_maxdepth_r; 

public:
  FTKPatternKDTree(int, FTKPattern*, int, int depth=0, FTKPatternKDTree *parent=0);
  ~FTKPatternKDTree();

  FTKPatternKDTree* addNode(FTKPattern*, int);
  FTKPattern* getNode() const { return m_node; }
  int getPosition() const { return m_pos; }
  const int &getDepth() const { return m_depth; }
  const int &getSplitPlane() const { return m_splitplane; }
  const int &getMaxDepth() const { return m_maxdepth; }
  const int &getMaxDepthLeft() const { return m_maxdepth_l; }
  const int &getMaxDepthRight() const { return m_maxdepth_r; }

  int makePatternList(std::list<FTKPattern> &);
};

#endif // FTKPattern_H

