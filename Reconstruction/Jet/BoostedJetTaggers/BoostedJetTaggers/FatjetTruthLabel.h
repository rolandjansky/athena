#ifndef FATJETLABEL_H_
#define FATJETLABEL_H_

enum class FatjetTruthLabel : int {tqqb = 1, Wqq = 2, Zqq = 3, Wqq_From_t = 4, other_From_t = 5, other_From_V = 6, notruth = 7, unknown = -1};
const FatjetTruthLabel FatjetTruthLabel_types [] = {FatjetTruthLabel::tqqb, FatjetTruthLabel::Wqq, FatjetTruthLabel::Zqq, FatjetTruthLabel::Wqq_From_t, FatjetTruthLabel::other_From_t, FatjetTruthLabel::other_From_V, FatjetTruthLabel::notruth, FatjetTruthLabel::unknown};

#endif
