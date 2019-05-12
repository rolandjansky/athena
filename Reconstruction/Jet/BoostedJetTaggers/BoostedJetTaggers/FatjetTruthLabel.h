#ifndef FATJETLABEL_H_
#define FATJETLABEL_H_

enum class FatjetTruthLabel : int {t = 1, W = 2, Z = 3, b = 4, other = 5, notruth = 6, unknown = -1};
const FatjetTruthLabel FatjetTruthLabel_types [] = {FatjetTruthLabel::t, FatjetTruthLabel::W, FatjetTruthLabel::Z, FatjetTruthLabel::b, FatjetTruthLabel::other, FatjetTruthLabel::notruth, FatjetTruthLabel::unknown};

#endif
