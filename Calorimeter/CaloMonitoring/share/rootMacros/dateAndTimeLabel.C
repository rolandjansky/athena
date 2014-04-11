////////////////////////////////////////////////////////////
// dateAndTimeLabel.C
// 
// last modified 2004/06/27
//
// Creates a TPaveLabel with date and time information 
////////////////////////////////////////////////////////////

#include <ctime>
#include <iostream>

#include "TPaveLabel.h"

using namespace std;

TPaveLabel* dateAndTimeLabel(float x1 = 0.1, float y1 = 0.905, float x2 = 0.5, float y2 = 0.945) {
  time_t now;
  time_t* timeP = &now;
  time(timeP);
  TPaveLabel* dateLabel = new TPaveLabel(x1, y1, x2, y2, ctime(timeP), "NDC");
  dateLabel->SetBorderSize(0);
  dateLabel->SetFillColor(0);
  dateLabel->SetTextAlign(11); // bottom left align
  return dateLabel;
}

