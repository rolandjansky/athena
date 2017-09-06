/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMSTEERING_H
#define MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMSTEERING_H

#include "MuonHoughPatternEvent/MuonHoughPatternCollection.h"

class MuonHoughPattern;
class MuonHoughTransformer;


class MuonHoughTransformSteering
{
  /** Class is build as Strategy, Context pattern */

 public:
  /** constructor */
  MuonHoughTransformSteering(MuonHoughTransformer*);
  /** destructor */
  ~MuonHoughTransformSteering();

  /** construct hough patterns 
   * @param[in] event Hitcontainer
   * @param[in] residu_mm maximum residu for hit to be associated to pattern 
   * @param[in] residu_grad maximum residu for hit to be associated to pattern
   * @param[in] max_patterns maximum number of patterns to be built
   * @param[in] which_segment upper (1) or lower (0) segment, this option is off by default
   * @param[in] printlevel outputlevel between 0-10
   * @param[out] HoughPatternCollection
   */
  MuonHoughPatternCollection constructHoughPatterns(const MuonHoughHitContainer* event, double residu_mm, double residu_grad, int max_patterns=1, bool which_segment=0, int printlevel=0)const;
  /** construct hough pattern on a certain maxima number of histogram */   
  MuonHoughPattern* constructHoughPattern(const MuonHoughHitContainer* event, double residu_mm, double residu_grad, int maximum_number=0, bool which_segment=0, int printlevel=0)const;
  /** construct hough pattern at a certain coordinate (maximum) in certain sector*/   
  MuonHoughPattern* constructHoughPattern(const MuonHoughHitContainer* event,std::pair <double,double> coordsmaximum, double residu_mm,double residu_grad, int sector=0, bool which_segment=0, int printlevel=0)const;
  /** construct hough pattern at a certain binnumber (maximum) in certain sector*/   
  MuonHoughPattern* constructHoughPattern(const MuonHoughHitContainer* event,int binnumber, double residu_mm,double residu_grad, int sector=0, bool which_segment=0, int printlevel=0)const;

  /** fill histograms */
  void fill(const MuonHoughHitContainer* event, bool subtract = false );

  /** reset histograms */
  void resetHisto();

  /** access to histograms */
  const MuonHoughHisto2DContainer& histos() const {
    return m_houghtransformer->histos();
  }

  
  const MuonHoughTransformer* transformer() const {
    return m_houghtransformer;
  }

 private:

  /** the actual houghtransform */
  MuonHoughTransformer* m_houghtransformer;

  /** maximum residu for hit to be associated to pattern */
  bool m_maximum_residu_mm;
};

#endif // MUONHOUGHPATTERNEVENT_MUONHOUGHTRANSFORMSTEERING_H
