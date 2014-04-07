/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// 
// ////////////////////////////////////////////////////////////////
// //                                                            //
// //  Header file for class TrackHandle_FatrasTruthTrack        //
// //                                                            //
// //  Description: Specialisation of Trk::Track handles used    //
// //               for FatRas tracks.                           //
// //                                                            //
// //  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
// //  Initial version: March 2008                               //
// //                                                            //
// ////////////////////////////////////////////////////////////////
// 
// #ifndef TRACKHANDLE_FATRASTRUTHTRACK_H
// #define TRACKHANDLE_FATRASTRUTHTRACK_H
// 
// #include "VP1TrackSystems/TrackHandle_TrkTrack.h"
// namespace Fatras{ class TrackParticleState; }
// 
// class TrackHandle_FatrasTruthTrack : public TrackHandle_TrkTrack {
// public:
// 
//   TrackHandle_FatrasTruthTrack(TrackCollHandleBase*,const Fatras::TrackParticleState*);
//   virtual ~TrackHandle_FatrasTruthTrack() {};
// 
//   virtual QStringList clicked() const;
// 
//   virtual Amg::Vector3D momentum() const;
//   virtual const Amg::Vector3D * startPoint() const;
//   virtual int pdgCode() const;
//   virtual QString type() const { return QString("Fatras"); } //!< return very short word with type (maybe link with collection type?)
// 
// protected:
//   virtual double calculateCharge() const;
//   virtual Trk::ParticleHypothesis extrapolationParticleHypothesis() const;
// 
// private:
// 
//   class Imp;
//   Imp * d;
// 
// 
// };
// 
// #endif
