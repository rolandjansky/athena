/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           VxClusteringTable.h  -  Description
                             -------------------
    begin   : Februar 2007
    authors : Giacinto Piacquadio (University of Freiburg) 
              Christian Weiser (University of Freiburg)
    e-mails:  giacinto.piacquadio@physik.uni-freiburg.de)
              christian.weiser@cern.ch
    changes: new!

   (c) Atlas Collaboration 2007

   Just a debug class. Probably it will not appear in the final version of 
   the released software.


 ***************************************************************************/

#include "GaudiKernel/MsgStream.h"

namespace Trk {

  class VxJetFitterDebugInfo 
  {
  public:
    VxJetFitterDebugInfo();

    ~VxJetFitterDebugInfo() {}

   /** 
   * Output Method for MsgStream, to be overloaded by child classes 
   */
    MsgStream& dump(MsgStream& sl) const;
    
  /** 
   * Output Method for std::ostream, to be overloaded by child classes 
   */
    std::ostream& dump(std::ostream& sl) const;

    int getNumFitIterations() {
      return m_numFitIterations;
    }

    void setNumFitIterations(int num) {
      m_numFitIterations=num;
    }
 
    int getSignFlipNumFitIterations() {
      return m_numSignFlipFitIterations;
    }
    
    void setSignFlipNumFitIterations(int num) {
      m_numSignFlipFitIterations=num;
    }
    
  protected:
    int m_numFitIterations;
    int m_numSignFlipFitIterations;
    //    int m_numSignFlips;
  };

 
  
   
}
