#ifndef FATJETLABEL_H_
#define FATJETLABEL_H_

enum class FatjetTruthLabel : int {tqqb = 1, Wqq = 2, Zqq = 3, Wqq_From_t = 4, other_From_t = 5, other_From_V = 6, notruth = 7, unknown = -1};
const FatjetTruthLabel FatjetTruthLabel_types [] = {FatjetTruthLabel::tqqb, FatjetTruthLabel::Wqq, FatjetTruthLabel::Zqq, FatjetTruthLabel::Wqq_From_t, FatjetTruthLabel::other_From_t, FatjetTruthLabel::other_From_V, FatjetTruthLabel::notruth, FatjetTruthLabel::unknown};

//enum class FatjetTruthLabel : int {t = 1, W = 2, Z = 3, b = 4, other = 5, notruth = 6, unknown = -1};
//const FatjetTruthLabel FatjetTruthLabel_types [] = {FatjetTruthLabel::t, FatjetTruthLabel::W, FatjetTruthLabel::Z, FatjetTruthLabel::b, FatjetTruthLabel::other, FatjetTruthLabel::notruth, FatjetTruthLabel::unknown};

#endif
