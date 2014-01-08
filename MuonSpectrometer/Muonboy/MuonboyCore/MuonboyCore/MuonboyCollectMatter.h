/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyCollectMatter_H
#define MuonboyCollectMatter_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MuonboyBFAbstract;

  /**
   @class MuonboyCollectMatter

   this class collects matter

  @author samusog@cern.ch
  
  */


class  MuonboyComusc ;
class  MbMuSc ;

class MuonboyCollectMatter {
public:
   MuonboyCollectMatter();
  ~MuonboyCollectMatter();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();

   /**Collect in Spectro */
   void execute(
                MuonboyBFAbstract* pMuonboyBFAbstract,
                int AtCosmMatter,
                double XposI,
                double YposI,
                double ZposI,
                double XmomI,
                double YmomI,
                double ZmomI,
                double Charge,
                std::vector<MbMuSc*>* pVectorOfMbMuSc
	       );

   /**Do finalisation */
   void finalize();

private:
///////////////////////////////////

  void CopyMuonboyComusc2MbMuSc(MuonboyComusc* pMuonboyComusc,MbMuSc* pMbMuSc,double Pmom,int Index);

  void SelectAndOrder(
                      double XrefStart,
                      double YrefStart,
                      double ZrefStart,
                      std::vector<MbMuSc>&  VectorLocalOfMbMuSc,
                      std::vector<MbMuSc*>* pVectorOfMbMuSc
                     );


}
;
#endif
