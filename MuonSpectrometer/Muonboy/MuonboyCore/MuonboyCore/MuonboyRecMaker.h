/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyRecMaker_H
#define MuonboyRecMaker_H

#include "MuonboyCore/MuonboyCoreMisc.h"

class MuonRecDigiStore;
class MB_TrackStore;
class MB_SegmentStore;
class MboyGenTrackStore;
class MuonboyBFAbstract;
class MuonboyRTAbstract;

class MB_TrackStoreMaker;
class MB_SegmentStoreMaker;


  /**
   @class MuonboyRecMaker

   this class performs reconstruction

  @author samusog@cern.ch
  
  */

class MuonboyRecMaker{
public:
    MuonboyRecMaker();
   ~MuonboyRecMaker();

public:
///////////////////////////////////

   /**Do initialisation */
   void initialize();
   
   /**Do initialisation */
   void initialize(
     MuonboyBFAbstract* pMuonboyBFAbstract
     );
     
   /**Do initialisation */
   void initialize(
     std::string AmdcString,
     std::string AgddString,
     MuonboyBFAbstract* pMuonboyBFAbstract
     );

   /**Do execution */
   void execute(     
     const MuonRecDigiStore* pMuonRecDigiStore,
     MB_TrackStore*          pMB_TrackStore,
     MB_SegmentStore*        pMB_SegmentStore
     );
     
   /**Do execution */
   void execute(     
     MuonboyBFAbstract* pMuonboyBFAbstract,
     MuonboyRTAbstract* pMuonboyRTAbstract,
     const MuonRecDigiStore* pMuonRecDigiStore,
     MB_TrackStore*          pMB_TrackStore,
     MB_SegmentStore*        pMB_SegmentStore
     );

   /**Do execution with To shift */
   void executeTshift(     
     double TShift ,
     MuonboyBFAbstract* pMuonboyBFAbstract,
     MuonboyRTAbstract* pMuonboyRTAbstract,
     const MuonRecDigiStore* pMuonRecDigiStore,
     MB_TrackStore*          pMB_TrackStore,
     MB_SegmentStore*        pMB_SegmentStore
     );

   /**Do finalisation */
   void finalize();

private:
///////////////////////////////////

   /**Set geo from string */
   void SetFromString(std::string AmdcString,std::string AgddString);

   MB_TrackStoreMaker*   p_MB_TrackStoreMaker   ; //!< Pointer on MB_TrackStoreMaker
   MB_SegmentStoreMaker* p_MB_SegmentStoreMaker ; //!< Pointer on MB_SegmentStoreMaker

};
#endif

