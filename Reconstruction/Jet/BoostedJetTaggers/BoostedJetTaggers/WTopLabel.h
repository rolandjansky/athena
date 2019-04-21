#ifndef WTOPLABEL_H_
#define WTOPLABEL_H_

enum class WTopLabel : int {t = 1, W = 2, Z = 3, b = 4, other = 5, notruth = 6, unknown = -1};
const WTopLabel WTopLabel_types [] = {WTopLabel::t, WTopLabel::W, WTopLabel::Z, WTopLabel::b, WTopLabel::other, WTopLabel::notruth, WTopLabel::unknown};

#endif
