/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// 
// ////////////////////////////////////////////////////////////////
// //                                                            //
// //  Implementation of class TrackHandle_FatrasTruthTrack      //
// //                                                            //
// //  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
// //  Initial version: March 2008                               //
// //                                                            //
// ////////////////////////////////////////////////////////////////
// 
// #include "VP1TrackSystems/TrackHandle_FatrasTruthTrack.h"
// #include "FatrasEvent/TrackParticleState.h"
// #include "FatrasEvent/TruthAssociation.h"
// #include "AtlasHepMC/GenParticle.h"
// 
// //____________________________________________________________________
// class TrackHandle_FatrasTruthTrack::Imp {
// public:
//   Imp (const Fatras::TrackParticleState* s) : state(s) {}
//   const Fatras::TrackParticleState* state;
// };
// 
// //____________________________________________________________________
// TrackHandle_FatrasTruthTrack::TrackHandle_FatrasTruthTrack(TrackCollHandleBase* ch,const Fatras::TrackParticleState* state)
//   : TrackHandle_TrkTrack(ch,&(state->associatedTrack())), d(new Imp(state))
// {
// }
// 
// //____________________________________________________________________
// QStringList TrackHandle_FatrasTruthTrack::clicked() const
// {
//   QStringList l;
//   l << "Fatras Truth Trk::Track";
//   QStringList l2 = TrackHandle_TrkTrack::clicked();
//   if (l2.count()>1) {
//     l << "Track information:";
//     for(int i = 1; i < l2.count(); ++i)
//       l << l2.at(i);//we ignore the first line.
//   }
//   l << "Truth information:";
//   l << "pdgCode = "+QString::number(pdgCode());
//   return l;
// 
//   //NB: For printing, note the TruthAssociation has method:
//   //   const AssociationType associationType() const;
//   //   enum AssociationType {  direct     = 0,
//   //                           decay      = 1,
//   //                           brem       = 2,
//   //                           conversion = 3 };
// 
// }
// 
// //____________________________________________________________________
// Amg::Vector3D TrackHandle_FatrasTruthTrack::momentum() const
// {
//   return TrackHandle_TrkTrack::momentum();//FIXME: HERE SINCE NEXT LINE CRASHES FOR SECONDARIES!!!
//   //return d->state->momentum();
// }
// 
// //____________________________________________________________________
// const Amg::Vector3D * TrackHandle_FatrasTruthTrack::startPoint() const
// {
//   return &(d->state->position());
// }
// 
// //____________________________________________________________________
// int TrackHandle_FatrasTruthTrack::pdgCode() const
// {
//   Fatras::TruthAssociation * ta = d->state->truthAssociation();
//   if (!ta)
//     return 0;
//   const HepMC::GenParticle* gp = ta->genParticle();
//   return gp ? gp->pdg_id() : 0;
// }
// 
// //____________________________________________________________________
// double TrackHandle_FatrasTruthTrack::calculateCharge() const
// {
//   return TrackHandle_TrkTrack::calculateCharge();//FIXME: HERE SINCE NEXT LINE CRASHES FOR SECONDARIES!!!
//   return d->state->charge();
// }
// 
// //____________________________________________________________________
// Trk::ParticleHypothesis TrackHandle_FatrasTruthTrack::extrapolationParticleHypothesis() const
// {
//   return d->state->particleHypothesis();
// }
