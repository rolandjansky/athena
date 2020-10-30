/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LabelIndex.h

#ifndef jet__LabelIndex
#define jet__LabelIndex

// David Adams
// January 13, 2014
//
// Bi-directional map assigning indices to string labels.

#include <vector>
#include <string>
#include "xAODCore/CLASS_DEF.h"
#include "xAODJet/JetTypes.h"

namespace jet {

class LabelIndex {

public:

  typedef std::string Label;
  typedef unsigned int Index;

  /// Return a global shared map by name.
  /// E.g. LabelIndex& labidx = LabelIndex::instance("ConstituentLabels");
  static LabelIndex& mutable_instance(Label name);
  static const LabelIndex& instance(Label name);

  /// Ctor.
  explicit LabelIndex(Label nam);

  /// Add a label and get its index.
  /// Current index is returned if the label is already indexed.
  /// Return 0 for error.
  Index addLabel(Label lab);

  /// Return the name of this map.
  Label name() const;

  /// Fetch the label for an index.
  /// Returns "" for error, e.g. unused index.
  Label label(Index idx) const;

  /// Fetch the index for a label.
  /// Returns "" for error, e.g. unknown label.
  Index index(Label lab) const;

  /// Fetch the constit scale corresponding to the index
  xAOD::JetConstitScale constitScale(Index idx) const ;

  /// Number of label stored in this map. WARNING the index starts at 1, so range is [1...size()+1]
  Index size() const ;

private:

  // Hide ctors and copy.
  explicit LabelIndex(const LabelIndex& nam);
  LabelIndex& operator=(const Label& nam);

  // Data.
  Label m_name;
  std::vector<Label> m_labs;
  std::vector<xAOD::JetConstitScale> m_constitScales;

};

}  // end namespace jet

CLASS_DEF( jet::LabelIndex , 94979360 , 1 )

#endif
