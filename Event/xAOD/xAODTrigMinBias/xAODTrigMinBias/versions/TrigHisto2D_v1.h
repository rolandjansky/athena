/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigHisto2D_V1_H
#define xAODTrigMinBias_TrigHisto2D_V1_H

#include "AthContainers/AuxElement.h"

namespace xAOD {
  
  namespace TrigHistoCutType {
    enum CutTypeEnum {
      BELOW_X,
      ABOVE_X,
      BELOW_X_BELOW_Y,
      ABOVE_X_BELOW_Y,
      BELOW_X_ABOVE_Y,
      ABOVE_X_ABOVE_Y};
  }
//   class MsgStream;
  
  class TrigHisto2D_v1 : public SG::AuxElement {
  public:
    TrigHisto2D_v1();
    
    /** Destructor */
    ~TrigHisto2D_v1();
    
    /** Standard constructor used by FEX algorithms. */ 
    TrigHisto2D_v1(unsigned int nbins_x, float min_x, float max_x, unsigned int nbins_y, float min_y, float max_y);
    /** Fill a 2D histogram */
    
    /** Return the bin contents of the histogram, including 
     * the under and overflow bins. */
    const std::vector<float>& contents() const;
    
    unsigned int nbinsX() const;
    
    /** Return the minimum along the x-axis. */
    float minX()const ; 
    
    /** Return the maximum along the x-axis. */
    float maxX() const; 
    
    /** Return the number of bins along the y-axis, not 
     * including the under and overflow. */
    unsigned int nbinsY() const;
    
    /** Return the minimum along the y-axis. */
    float minY() const;
    
    /** Return the maximum along the y-axis. */
    float maxY() const;
    
    /**creates empty histogram**/
    void initialize(unsigned int nbins_x, float min_x, float max_x, unsigned int nbins_y, float min_y, float max_y);
    
    /**fill histogram**/
    void fill(float value_x, float value_y, float weight);
    
    /** Sum the number of entries within the cut range */
    //double sumEntries(float value_x, float value_y, int cutType);
    
    /** Collapse the y-axis and return a profile from the x-axis */
    // it's not very efficient ;/ 
    std::vector<float> profileX() const;
    
    /** Collapse the x-axis and return a profile from the y-axis */
    std::vector<float> profileY() const;
    
    /** Sum the number of entries within the cut range */
    double sumEntries(float value_x, float value_y, int cutType) const;
    
    /** clear m_contents vector **/
    void clear();
    
    /**dump() function, for testing**/
    void dump();
   
//     void setMsgStream(MsgStream&);
    
    /**contents setter**/
    void setContents(const std::vector<float> &cont);
    
    void setMinX(float val);
    
    void setMaxX(float val);
    
    /**NbinsX setter**/
    void setNbinsX(unsigned int nx);
    
    void setMinY(float val);
    
    void setMaxY(float val);
    
    /**NbinsY setter**/
    void setNbinsY(unsigned int ny);
    
      private:
    /**returns x bin index **/
    unsigned int findBinX(float val) const;
    
    /**returns y bin index **/
    unsigned int findBinY(float val) const;
    
    /**additional bins for underflow and overflow bins **/
    static const int EXTRA_BINS=2;
    
    /*width of x bins*/
    float m_binWidthX = 0.0F;
   
    /*width of y bins*/
    float m_binWidthY = 0.0F;
  };
  
}

#endif
