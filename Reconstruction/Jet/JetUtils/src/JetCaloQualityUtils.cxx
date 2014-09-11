/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUtils/JetCaloQualityUtils.h"

#include "xAODJet/Jet.h"

#include "xAODJet/JetAccessorMap.h"
#include "xAODCaloEvent/CaloCluster.h"


//#include "CaloIdentifier/CaloCell_ID.h"

//#include "CaloDetDescr/CaloDepthTool.h"
//#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloGeoHelpers/CaloSampling.h"


#include "xAODCaloEvent/CaloCluster.h"

#include "TileEvent/TileCell.h"


using xAOD::Jet;
using xAOD::CaloCluster;



namespace {

  int em_calosample[]  = { CaloSampling::PreSamplerB, CaloSampling::EMB1, CaloSampling::EMB2, CaloSampling::EMB3,
                           CaloSampling::PreSamplerE, CaloSampling::EME1, CaloSampling::EME2, CaloSampling::EME3,  
                           CaloSampling::FCAL0};
  int had_calosample[] = { CaloSampling::HEC0, CaloSampling::HEC1, CaloSampling::HEC2, CaloSampling::HEC3,          
                           CaloSampling::TileBar0, CaloSampling::TileBar1, CaloSampling::TileBar2,
                           CaloSampling::TileGap1, CaloSampling::TileGap2, CaloSampling::TileGap3,    
                           CaloSampling::TileExt0, CaloSampling::TileExt1, CaloSampling::TileExt2,    
                           CaloSampling::FCAL1, CaloSampling::FCAL2  };
  
  const double GeV = 1000.0;
}

namespace jet {


  int JetCaloQualityUtils::compute_nLeading(std::vector<double> &cell_energies,  const float& e, const float& frac)
  {  
    std::sort(cell_energies.rbegin(),cell_energies.rend());
    int counter =0;
    float sum = 0;
    for(unsigned int i=0;i<cell_energies.size();i++)
      {
        sum += cell_energies[i];
        counter++;
        if(sum>frac*e) break;
      }
    return counter;
  }



  static xAOD::JetAttributeAccessor::AccessorWrapper< std::vector<float> >  &eperSamplAcc = * xAOD::JetAttributeAccessor::accessor< std::vector<float> >(xAOD::JetAttribute::EnergyPerSampling);

  double JetCaloQualityUtils::emFraction(const std::vector<float>& e_sampling){   
    double e_EM = 0;
    for(int i=0; i<9; i++) e_EM += e_sampling[ ::em_calosample[i] ];
    
    double e_HAD = 0;
    for(int i=0; i<15; i++) e_HAD += e_sampling[ ::had_calosample[i] ];
    if( (e_EM==0) || ((e_EM+e_HAD)==0) ) return 0.;
    return (e_EM / (e_EM+e_HAD));    
  }
  

  double JetCaloQualityUtils::hecF(const Jet* jet)
  {
    
    const std::vector<float> & einsampling = eperSamplAcc(*jet);
    
    double e_hec =einsampling[CaloSampling::HEC0]
      +einsampling[CaloSampling::HEC1]
      +einsampling[CaloSampling::HEC2]
      +einsampling[CaloSampling::HEC3];
    
    double e_jet = jet->jetP4(xAOD::JetEMScaleMomentum).E();
  
    if(e_jet!=0) return  e_hec/e_jet;
    else return -999;
  }

  double JetCaloQualityUtils::presamplerFraction(const Jet* jet)
  {
    const std::vector<float> einsampling = eperSamplAcc(*jet);

    double e_pres = einsampling[CaloSampling::PreSamplerB] + einsampling[CaloSampling::PreSamplerE];

    double e_jet = jet->jetP4(xAOD::JetEMScaleMomentum).E();
  
    if(e_jet!=0) return  e_pres/e_jet;
    else return -999;
  }

  double JetCaloQualityUtils::tileGap3F(const Jet* jet)
  {
    const std::vector<float> & einsampling = eperSamplAcc(*jet);

    double e_tileGap3 =einsampling[CaloSampling::TileGap3];
    double e_jet = jet->jetP4(xAOD::JetEMScaleMomentum).E();

    if(e_jet!=0) return  e_tileGap3/e_jet;
    else return -999;
  }


  double JetCaloQualityUtils::fracSamplingMax(const Jet* jet, int& SamplingMax)
  {
    const std::vector<float> & einsampling = eperSamplAcc(*jet);

    double fracSamplingMax=-999999999.;    
    double sumE_samplings=0.;
    for ( unsigned int i(0); i < einsampling.size(); ++i )
      {
        double e = einsampling[i];
        sumE_samplings+=e;
        if (e>fracSamplingMax)
          {
            fracSamplingMax=e;
            SamplingMax=i;
          }
      }

    if(sumE_samplings!=0)
      fracSamplingMax/=sumE_samplings;
    else fracSamplingMax=0;
  
    return fracSamplingMax;
  }


  double JetCaloQualityUtils::jetTimeClusters(const Jet*)
  {
    // Calculate the energy weighted event time for the jet

    // // time and normalization
    // double time(0);
    // double norm(0);
  
    // // If we find clusters, we use an approximation:
    // // We weigh each cluster by its energy^2
  
    // NavigationToken<CaloCluster,double> clusToken;
    // jet->fillToken(clusToken,double(1.));
  
    // if ( clusToken.size() > 0 )
    //   {
    //     // Use the constituent iterator
    //     xAOD::JetConstituentVector::iterator firstClus = JetConstituentIterator::first(jet);
    //     xAOD::JetConstituentVector::iterator lastClus = JetConstituentIterator::last(jet);
      
    //     for(; firstClus != lastClus; ++firstClus)
    //       {
    //         // We perform a naive e^2 weighting over the cluster times and energies
    //         const CaloCluster* theClus = dynamic_cast<const CaloCluster*>(firstClus.get_real_constit());

    //         // For the moment, we have to force the weight to be 1, because of navigation
    //         // difficulties with the JetConstituentIterator
    //         //double weight(firstClus.get_real_constit().getParameter());
    //         double weight(1);

    //         double thisNorm = weight * fabs(weight) * firstClus.e() * firstClus.e();
    //         double thisTime = thisNorm * theClus->getTime();

    //         time += thisTime;
    //         norm += thisNorm;
	  
    //       }
    //   }
    // if(norm>0)
    //   return (time/norm);
    // else
    //   return 0;

return 0;
  }


  bool timeAndQualityDefined(const CaloCell* theCell){
    // ensure timing information is defined
    bool timeIsDefined(false);
    if(theCell->caloDDE()->is_tile())	  
      {
        if ( (theCell->provenance()) & 0x8080)
          timeIsDefined = true;
      }
    else //is_lar
      {
        if ( (theCell->provenance() & 0x2000) &&
             !(theCell->provenance()  & 0x0800 ) // do not take masked-patched cells int
             )         timeIsDefined = true;
      }
    return timeIsDefined;
  }

  double JetCaloQualityUtils::jetTimeCells(const Jet*)
  {
    // // Calculate the energy weighted event time for the jet

    // // time and normalization
    // double time(0);
    // double norm(0);

    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));
  
    // if ( cellToken.size() > 0 )
    //   {
    //     // Improve performance with a PrefetchIterator
    //     CaloCellPrefetchIterator<NavigationToken<CaloCell,double>::const_iterator> it (cellToken.begin(), cellToken.end());
    //     while (it.next())
    //       {
    //         // Get weight and pointer to Cell
    //         double weight = it.getParameter();
    //         const CaloCell* theCell = (*it); 
	  
    //         // ensure timing information is defined
    //         bool timeIsDefined = timeAndQualityDefined(theCell);
          
    //         if ( timeIsDefined )
    //           {
    //             double thisNorm = weight * fabs(weight) * theCell->e() * theCell->e();
    //             double thisTime = thisNorm * theCell->time();
	      
    //             time += thisTime;
    //             norm += thisNorm;
    //           }
    //       }
    //   }
    // if(norm > 0)
    //   return (time/norm);
    // else
    //   return 0;
    return 0;
  }



  double JetCaloQualityUtils::jetTime(const Jet*)
  {

    // // Provide a default not to break packages already written to depend
    // // on this function.  We just check for existence of clusters/cells...

    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));

    // NavigationToken<CaloCluster,double> clusToken;
    // jet->fillToken(clusToken,double(1.));
  
    // if(cellToken.size() != 0)
    //   return JetCaloQualityUtils::jetTimeCells(jet);
    // else if(clusToken.size() != 0)
    //   return JetCaloQualityUtils::jetTimeClusters(jet);
    // else return 0;
    return 0;
  
  }


  double JetCaloQualityUtils::jetAverageLArQualityF(const Jet*)
  {
    // // Calculate the energy weighted event time for the jet

    // // time and normalization
    // double qf(0);
    // double norm(0);

    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));
  
    // if ( cellToken.size() > 0 )
    //   {
    //     // Improve performance with a PrefetchIterator
    //     CaloCellPrefetchIterator<NavigationToken<CaloCell,double>::const_iterator> it (cellToken.begin(), cellToken.end());
    //     while (it.next())
    //       {
    //         // Get weight and pointer to Cell
    //         double weight = it.getParameter();
    //         const CaloCell* theCell = (*it); 
	  
    //         // ensure timing information is defined ...
    //         bool timeIsDefined = timeAndQualityDefined(theCell);
    //         // ... and restrict to LAr cells
    //         if ( timeIsDefined 	&& !( theCell->caloDDE()->is_tile()) )
    //           {
    //             double thisNorm = weight * fabs(weight) * theCell->e() * theCell->e();
    //             double thisQf = thisNorm * theCell->quality();
	      
    //             qf += thisQf;
    //             norm += thisNorm;
    //           }
    //       }
    //   }
    // if(norm > 0)
    //   return (qf/norm);
    // else
    //   return 0;

    return 0;
  }


  double JetCaloQualityUtils::jetQuality(const Jet*, int, int)
  //double JetCaloQualityUtils::jetQuality(const Jet* jet, int LArQualityCut, int TileQualityCut)
  {
    // // Calculate the fraction of energy in cells flagged with a 'bad' quality
    // // The cuts used are fairly approximative, and are used to flag obviously problematic cells
    // // The quality factor might have an energy/gain dependence, so choosing a single cut
    // // is surely a coarse approximation...
  
    // double totE(0);
    // double badE(0);
  
    // // We have to navigate right back to cells in this case
    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));
  
    // if ( cellToken.size() > 0 )
    //   {
    //     NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
    //     NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
    //     for(; firstCell != lastCell; ++firstCell)
    //       {
    //         const CaloCell* theCell = (*firstCell); 

    //         if(theCell->caloDDE()->is_tile())
    //           {
    //             const TileCell* theTileCell = dynamic_cast<const TileCell*>(theCell);
    //             if(theTileCell)
    //               {
    //                 if((theCell->provenance()) & 0x8080)
    //                   {
    //                     totE += theCell->e();
    //                     int tileQuality = std::max(theTileCell->qual1(), theTileCell->qual2());
    //                     if(tileQuality > TileQualityCut)
    //                       badE += theCell->e();
    //                   }
    //               }
    //           }
    //         else
    //           {
    //             if( (theCell->provenance() & 0x2000) &&
    //                 !(theCell->provenance()  & 0x0800 ) // do not take masked-patched cells into account
    //                 )
    //               {
    //                 totE += theCell->e();
    //                 if(theCell->quality() > LArQualityCut)
    //                   badE += theCell->e();
    //               }
    //           }
    //       }
    //   }
    // if(totE > 0)
    //   return (badE/totE);
    // else
    //   return 0;

    return 0;
  }



  double JetCaloQualityUtils::jetQualityLAr(const Jet*, int)
  //double JetCaloQualityUtils::jetQualityLAr(const Jet* jet, int LArQualityCut)
  {
    // // Calculate the fraction of energy in cells flagged with a 'bad' quality
    // // The cuts used are fairly approximative, and are used to flag obviously problematic cells
    // // The quality factor might have an energy/gain dependence, so choosing a single cut
    // // is surely a coarse approximation...
  
    // double totE(0);
    // double badE(0);
  
    // // We have to navigate right back to cells in this case
    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));
  
    // if ( cellToken.size() > 0 )
    //   {
    //     NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
    //     NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
    //     for(; firstCell != lastCell; ++firstCell)
    //       {
    //         const CaloCell* theCell = (*firstCell); 

    //         if(!theCell->caloDDE()->is_tile())
    //           {
    //             // Only include cells with calculated Q-factors
    //             if( (theCell->provenance()  & 0x2000) &&
    //                 !(theCell->provenance()  & 0x0800 ) // do not take masked-patched cells into account
    //                 )            
    //               {
    //                 totE += theCell->e();
    //                 if(theCell->quality() > LArQualityCut )
    //                   badE += theCell->e(); 
    //               }	
    //           }
    //       }
    //   }
    // if(totE > 0)
    //   return (badE/totE);
    // else
    //   return 0;
    return 0;
  }

  double JetCaloQualityUtils::jetQualityTile(const Jet*, int)
  //double JetCaloQualityUtils::jetQualityTile(const Jet* jet, int TileQualityCut)
  {
    // // Calculate the fraction of energy in cells flagged with a 'bad' quality
    // // The cuts used are fairly approximative, and are used to flag obviously problematic cells
    // // The quality factor might have an energy/gain dependence, so choosing a single cut
    // // is surely a coarse approximation...
  
    // double totE(0);
    // double badE(0);
  
    // // We have to navigate right back to cells in this case
    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));
  
    // if ( cellToken.size() > 0 )
    //   {
    //     NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
    //     NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
    //     for(; firstCell != lastCell; ++firstCell)
    //       {
    //         const CaloCell* theCell = (*firstCell); 

    //         if(theCell->caloDDE()->is_tile())
    //           {
    //             const TileCell* theTileCell = dynamic_cast<const TileCell*>(theCell);
    //             if(theTileCell)
    //               {
    //                 if((theCell->provenance()) & 0x8080)
    //                   {
    //                     totE += theCell->e();
    //                     int tileQuality = std::max(theTileCell->qual1(), theTileCell->qual2());
    //                     if(tileQuality > TileQualityCut)
    //                       badE += theCell->e();
    //                   }
    //               }
    //           }
    //       }
    //   }
    // if(totE > 0)
    //   return (badE/totE);
    // else
    //   return 0;

    return 0;
  }


  double JetCaloQualityUtils::jetOutOfTimeEnergyFraction(const Jet*, const double, const bool) {
  //double JetCaloQualityUtils::jetOutOfTimeEnergyFraction(const Jet* jet, const double timecut, const bool doOnlyPos) {
    // double sumE=0;
    // double sumE_OOT=0;
  
    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));
  
    // if ( cellToken.size() > 0 ) {
    //   NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
    //   NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
    //   for(; firstCell != lastCell; ++firstCell)	{
    //     // weight is not used
    //     // double weight = firstCell.getParameter();
    //     const CaloCell* thisCell = *firstCell;

    //     double  aCellE = thisCell->e();

    //     if(doOnlyPos && aCellE<0) continue;

    //     sumE += aCellE;
      
    //     // ensure timing information is defined
    //     bool timeIsDefined(false);
      
    //     if(thisCell->caloDDE()->is_tile()) {
    //       if ( (thisCell->provenance()) & 0x8080)  timeIsDefined = true;
    //     } else { //is_lar	    
    //       if ( (thisCell->provenance()) & 0x2000)  timeIsDefined = true;
    //     }

    //     if ( timeIsDefined ) {
    //       double time = thisCell->time();
    //       if(fabs(time)>timecut)  sumE_OOT+=thisCell->e();        
    //     }
    //   }
    // }    

    // return ( (sumE!=0) ? (sumE_OOT/sumE) : 0 );

    return 0;
  }


  // Calculate the fraction of jet energy from clusters with fabs(time)>timecut
  // returns -1 if sum of cluster energy is zero and oot energy fraction otherwise
  double JetCaloQualityUtils::jetOutOfTimeEnergyFractionClusters(const Jet*, const double, const bool) {
  //double JetCaloQualityUtils::jetOutOfTimeEnergyFractionClusters(const Jet* jet, const double timecut, const bool doOnlyPos) {
    // if(dynamic_cast<const Jet*>(jet) == NULL) return -1;
  
    // double sum_all(0), sum_time(0);

    // NavigationToken<CaloCluster,double> clusToken;
    // jet->fillToken(clusToken,double(1.));
  
    // if ( clusToken.size() > 0 ) {
    //   // Use the constituent iterator
    //   xAOD::JetConstituentVector::iterator firstClus = JetConstituentIterator::first(jet);
    //   xAOD::JetConstituentVector::iterator lastClus  = JetConstituentIterator::last(jet);
      
    //   for(; firstClus != lastClus; ++firstClus) {
	//     const CaloCluster* theClus(NULL); theClus = dynamic_cast<const CaloCluster*>(firstClus.get_real_constit());
    //     if(!theClus) continue;

    //     P4SignalState::State origState = theClus->signalState();
    //     CaloClusterSignalState::setState(theClus,P4SignalState::UNCALIBRATED);
      
    //     double aClusterE    = theClus->e();
    //     double aClusterTime = theClus->getTime();

    //     CaloClusterSignalState::setState(theClus,origState);
      
    //     if(doOnlyPos && aClusterE<0) continue;

    //     sum_all += aClusterE;
    //     if(fabs(aClusterTime) > timecut) sum_time += aClusterE;
    //   }  
    // }

    // double ootEngyFrac = (fabs(sum_all)>0) ? sum_time/sum_all : -1;
    // return ootEngyFrac;
    return 0;
  }


  bool JetCaloQualityUtils::isGoodTight(const Jet* jet,const bool recalculateQuantities)
  {

    return  !(JetCaloQualityUtils::isBadTight(jet,recalculateQuantities)||
              JetCaloQualityUtils::isUgly(jet,recalculateQuantities));
  }

  bool JetCaloQualityUtils::isGoodMedium(const Jet* jet,const bool recalculateQuantities)
  {

    return  !(JetCaloQualityUtils::isBadMedium(jet,recalculateQuantities)||
              JetCaloQualityUtils::isUgly(jet,recalculateQuantities));
  }

  bool JetCaloQualityUtils::isGood(const Jet* jet,const bool recalculateQuantities)
  {

    return  !(JetCaloQualityUtils::isBad(jet,recalculateQuantities)||
              JetCaloQualityUtils::isUgly(jet,recalculateQuantities));
  }

  bool JetCaloQualityUtils::isUgly(const Jet* jet,const bool /*recalculateQuantities*/)
  {

    double fcor = jet->getAttribute<float>(xAOD::JetAttribute::BchCorrCell);
    double tileGap3f = JetCaloQualityUtils::tileGap3F(jet);

    if (fcor>0.5) return true;
    if (tileGap3f >0.5) return true;

    return false;
  }

  bool JetCaloQualityUtils::isBadTight(const Jet* jet,const bool recalculateQuantities)
  {

    double chf = jet->getAttribute<float>(xAOD::JetAttribute::SumPtTrkPt500)/jet->pt();
    double emf       = jet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
    double hecf      = JetCaloQualityUtils::hecF(jet);
    int SamplingMax  = CaloSampling::Unknown;
    double fmax = JetCaloQualityUtils::fracSamplingMax(jet,SamplingMax); 


    double jetQuality= -999.;
    double jetTime   = -999.;
    double jetHECQuality = -999.;
    double jetNegativeE = -999.;

    if(recalculateQuantities==true)
      {
        jetHECQuality= JetCaloQualityUtils::jetQualityHEC(jet); 
        jetQuality= JetCaloQualityUtils::jetQuality(jet); 
        jetTime   = JetCaloQualityUtils::jetTimeCells(jet);
        jetNegativeE = JetCaloQualityUtils::jetNegativeEnergy(jet);
      }
    else
      {
        jetQuality    = jet->getAttribute<float>("LArQuality"); 
        jetTime       = jet->getAttribute<float>("Timing"); 
        jetHECQuality = jet->getAttribute<float>("HECQuality");
        jetNegativeE = jet->getAttribute<float>("NegativeE");
      }

    if( hecf>1.-fabs(jetHECQuality) ) return true;
    if( hecf>0.5 && fabs(jetHECQuality)>0.5 ) return true;
    if( fabs(jetNegativeE)/GeV > 60.) return true;

    xAOD::JetFourMom_t jetP4Uncal = jet->jetP4(xAOD::JetEMScaleMomentum);
    double absEta = fabs(jetP4Uncal.Eta());
    if( emf>0.9 && fabs(jetQuality)>0.8 && absEta < 2.8) return true;
    if( fabs(jetQuality)>0.95) return true;
    if( emf>0.98 && fabs(jetQuality)>0.05) return true;

    if( fabs(jetTime)>10 ) return true;
    if( fmax > 0.99 and absEta < 2.0) return true;
    if( emf< 0.1 && chf < 0.2 && absEta < 2.0) return true;  
    if( emf> 0.95 && chf < 0.05 && absEta < 2.0) return true;  
    if( emf> 0.9 && chf < 0.02 && absEta < 2.0) return true;  
    if( emf< 0.1 && absEta >= 2.0) return true;  

    return false;
  }

  bool JetCaloQualityUtils::isBadMedium(const Jet* jet,const bool recalculateQuantities)
  {

    double chf = jet->getAttribute<float>(xAOD::JetAttribute::SumPtTrkPt500)/jet->pt();
    double emf       = jet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
    double hecf      = JetCaloQualityUtils::hecF(jet);
    int SamplingMax=CaloSampling::Unknown;
    double fmax = JetCaloQualityUtils::fracSamplingMax(jet,SamplingMax); 


    double jetQuality= -999.;
    double jetTime   = -999.;
    double jetHECQuality = -999.;
    double jetNegativeE = -999.;

    if(recalculateQuantities==true)
      {
        jetHECQuality= JetCaloQualityUtils::jetQualityHEC(jet); 
        jetQuality= JetCaloQualityUtils::jetQuality(jet); 
        jetTime   = JetCaloQualityUtils::jetTimeCells(jet);
        jetNegativeE = JetCaloQualityUtils::jetNegativeEnergy(jet);
      }
    else
      {
        jetQuality= jet->getAttribute<float>("LArQuality"); 
        jetTime   = jet->getAttribute<float>("Timing"); 
        jetHECQuality = jet->getAttribute<float>("HECQuality");
        jetNegativeE = jet->getAttribute<float>("NegativeE");
      }

    if( hecf>1.-fabs(jetHECQuality) ) return true;
    if( hecf>0.5 && fabs(jetHECQuality)>0.5 ) return true;
    if( fabs(jetNegativeE)/GeV > 60.) return true;

    xAOD::JetFourMom_t jetP4Uncal = jet->jetP4(xAOD::JetEMScaleMomentum);
    double absEta = fabs(jetP4Uncal.Eta());

    if( emf>0.9 && fabs(jetQuality)>0.8 && absEta < 2.8) return true;

    if( fabs(jetTime)>10 ) return true;
    if( fmax > 0.99 and absEta < 2.0) return true;
    if( emf<0.05 && chf < 0.1 && absEta < 2.0) return true;  
    if( emf>0.95 && chf < 0.05 && absEta < 2.0) return true;  
    if( emf<0.05 && absEta >= 2.0) return true;  

    return false;

  }

  bool JetCaloQualityUtils::isBad(const Jet* jet,const bool recalculateQuantities)
  {


    double chf = jet->getAttribute<float>(xAOD::JetAttribute::SumPtTrkPt500)/jet->pt();
  
    double emf       = jet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
    double hecf      = JetCaloQualityUtils::hecF(jet);
    int SamplingMax=CaloSampling::Unknown;
    double fmax = JetCaloQualityUtils::fracSamplingMax(jet,SamplingMax); 


    double jetQuality= -999.;
    double jetTime   = -999.;
    double jetHECQuality = -999.;
    double jetNegativeE = -999.;

    if(recalculateQuantities==true)
      {
        jetHECQuality= JetCaloQualityUtils::jetQualityHEC(jet); 
        jetQuality= JetCaloQualityUtils::jetQuality(jet); 
        jetTime   = JetCaloQualityUtils::jetTimeCells(jet);
        jetNegativeE = JetCaloQualityUtils::jetNegativeEnergy(jet);
      }
    else
      {
        jetQuality= jet->getAttribute<float>("LArQuality"); 
        jetTime   = jet->getAttribute<float>("Timing"); 
        jetHECQuality = jet->getAttribute<float>("HECQuality");
        jetNegativeE = jet->getAttribute<float>("NegativeE");
      }

    if( hecf>0.5 && fabs(jetHECQuality)>0.5 ) return true;
    if( fabs(jetNegativeE)/GeV > 60.) return true;

    xAOD::JetFourMom_t jetP4Uncal = jet->jetP4(xAOD::JetEMScaleMomentum);
    double absEta = fabs(jetP4Uncal.Eta());

    if( emf>0.95 && fabs(jetQuality)>0.8 && absEta < 2.8) return true;

    if( fabs(jetTime)>10 ) return true;
    if( fmax > 0.99 and absEta < 2.0) return true;
    if( emf<0.05 && chf < 0.05 && absEta < 2.0) return true;  
    if( emf<0.05 && absEta >= 2.0) return true;  

    return false;

  }

  double JetCaloQualityUtils::jetNegativeEnergy(const Jet*)
  {
    // double totE(0);
    // // We have to navigate right back to cells in this case
    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));
    // if ( cellToken.size() > 0 )
    //   {
    //     NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
    //     NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
    //     for(; firstCell != lastCell; ++firstCell)
    //       {
    //         const CaloCell* theCell = (*firstCell);       
    //         if(theCell->e()<-2500)
    //           totE += theCell->e();
    //       }
    //   }
    // return totE;
    return 0;
  }

  double JetCaloQualityUtils::jetQualityHEC(const Jet*, int, int)
  //double JetCaloQualityUtils::jetQualityHEC(const Jet* jet, int LArQualityCut, int /*TileQualityCut=254*/)
  {
    double totE(0);
    double badE(0);
    // We have to navigate right back to cells in this case
    // NavigationToken<CaloCell,double> cellToken;
    // jet->fillToken(cellToken,double(1.));
    // if ( cellToken.size() > 0 )
    //   {
    //     NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
    //     NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
    //     for(; firstCell != lastCell; ++firstCell)
    //       {
    //         const CaloCell* theCell = (*firstCell);
    //         if(theCell->caloDDE()->is_lar_hec())
    //           {
    //             if((theCell->provenance()) & 0x2000)
    //               {
    //                 totE += theCell->e();
    //                 if(theCell->quality() > LArQualityCut)
    //                   badE += theCell->e();
    //               }
    //           }
    //       }
    //   }
    if(totE > 0)
      return (badE/totE);
    else
      return 0;
  } 







  // ****************************************************************
  // JetCalcnLeadingCells *************************************************
  // ****************************************************************

  bool JetCalcnLeadingCells::setupJet(const Jet* j){
    sumE_cells=0;
    cell_energies.clear();
    if(!m_cellLevelCalculation) cell_energies.reserve(j->numConstituents());
    else cell_energies.reserve(10*j->numConstituents());
    return true;
  }

  bool JetCalcnLeadingCells::processCell(const CaloCell *thisCell, weight_t )
  {

    cell_energies.push_back(thisCell->e());
    sumE_cells+=thisCell->e();
    return true;
  }

  bool JetCalcnLeadingCells::processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t /* w */){

    double e = iter->e();
    cell_energies.push_back(e);
    sumE_cells+=e;

    return true;
  }

  double JetCalcnLeadingCells::jetCalculation(){  
    return JetCaloQualityUtils::compute_nLeading(cell_energies,sumE_cells,threshold);
  }


  // ****************************************************************
  // JetCalcOutOfTimeEnergyFraction *************************************************
  // ****************************************************************
  bool JetCalcOutOfTimeEnergyFraction::setupJet(const Jet*  ){
    sumE=0;
    sumE_OOT=0;
    return true;
  }

  bool JetCalcOutOfTimeEnergyFraction::processCell(const CaloCell *thisCell, weight_t )
  {
    sumE+=thisCell->e();
  
    // ensure timing information is defined
    bool timeIsDefined = timeAndQualityDefined(thisCell);  
    if ( timeIsDefined )
      {
        double time = thisCell->time();
        if(fabs(time)>timecut)sumE_OOT+=thisCell->e();
      }
    return true;
  }

  double JetCalcOutOfTimeEnergyFraction::jetCalculation(){  
    if( sumE== 0.) return -1;
    return sumE_OOT/sumE ;
  }

  bool JetCalcOutOfTimeEnergyFraction::processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t /* w */){
    if (iter->type() != xAOD::Type::CaloCluster ) return false;

    //double sum_all(0), sum_time(0);
    
    double aClusterE    = iter->e();
    double aClusterTime = static_cast<const xAOD::CaloCluster*>(iter->rawConstituent())->time();
    
    
    if(onlyPosEnergy && aClusterE<0) return true;
    
    sumE += aClusterE;
    if(fabs(aClusterTime) > timecut) sumE_OOT += aClusterE;

    return true;
  }

  // ****************************************************************
  // JetCalcTimeCells *************************************************
  // ****************************************************************
  bool JetCalcTimeCells::setupJet(const Jet* ){
    time = 0; norm = 0;
    return true;
  }

  bool JetCalcTimeCells::processCell(const CaloCell *theCell, weight_t weight)
  {
    // ensure timing information is defined
    bool timeIsDefined = timeAndQualityDefined(theCell);
  
    if ( timeIsDefined )
      {
        double thisNorm = weight * fabs(weight) * theCell->e() * theCell->e();
        double thisTime = thisNorm * theCell->time();
      
        time += thisTime;
        norm += thisNorm;
      }
  
    return true;
  }


  bool JetCalcTimeCells::processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t /* w */){
    const xAOD::CaloCluster* theClus(NULL);
    theClus = dynamic_cast<const xAOD::CaloCluster*>(iter->rawConstituent());
    if (!theClus) return false;

    double thisNorm = iter->e()* iter->e();
    time += theClus->time() * thisNorm ;
    norm += thisNorm;

    return true;
  }

  double JetCalcTimeCells::jetCalculation(){  
    if (norm==0) return 0;
    return time/norm ;
  }


  // ****************************************************************
  // JetCalcAverageLArQualityF *************************************************
  // ****************************************************************
  bool JetCalcAverageLArQualityF::setupJet(const Jet* ){
    qf = 0; norm = 0;   return true;
  }

  bool JetCalcAverageLArQualityF::processCell(const CaloCell *theCell, weight_t weight)
  {
    // ensure timing information is defined ...
    bool timeIsDefined = timeAndQualityDefined(theCell);
    // ... and restrict to LAr cells
    if ( timeIsDefined 	&& ( theCell->caloDDE()->is_tile() == useTile ) )
      {
        double thisNorm = weight * fabs(weight) * theCell->e() * theCell->e();
        double thisQf = thisNorm * theCell->quality();
      
        qf += thisQf;
        norm += thisNorm;
      }
  
    return true;
  }

  double JetCalcAverageLArQualityF::jetCalculation(){  
    if(norm==0) return 0;
    return  qf/norm;
  }

  bool JetCalcAverageLArQualityF::processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t /* w */){
    const xAOD::CaloCluster* theClus(NULL);
    theClus = dynamic_cast<const xAOD::CaloCluster*>(iter->rawConstituent());
    if (!theClus) return false;

    double e2 = iter->e();
    e2 = e2*e2;

    norm+= e2;
    double qual(0); bool retrieved;
    if(useTile) 
      retrieved = theClus->retrieveMoment(xAOD::CaloCluster::AVG_TILE_Q, qual);
    else
      retrieved = theClus->retrieveMoment(xAOD::CaloCluster::AVG_LAR_Q, qual);
    if(retrieved) qf += qual*e2;
    return true;
  }

  // ****************************************************************
  // JetCalcQuality *************************************************
  // ****************************************************************
  bool JetCalcQuality::setupJet(const Jet* ){
    totE=0; badE=0;   return true;
  }

  bool JetCalcQuality::processCell(const CaloCell *theCell, weight_t)
  {
    // Calculate the fraction of energy in cells flagged with a 'bad' quality
    // The cuts used are fairly approximative, and are used to flag obviously problematic cells
    // The quality factor might have an energy/gain dependence, so choosing a single cut
    // is surely a coarse approximation...

    if(theCell->caloDDE()->is_tile() && includeTile )
      {
        const TileCell* theTileCell = dynamic_cast<const TileCell*>(theCell);
        if(theTileCell)
          {
            if((theCell->provenance()) & 0x8080)
              {
                totE += theCell->e();
                int tileQuality = std::max(theTileCell->qual1(), theTileCell->qual2());
                if(tileQuality > TileQualityCut)
                  badE += theCell->e();
              }
          }
      }
    else
      {
        if( includeLAr && 
            (theCell->provenance() & 0x2000) &&
            !(theCell->provenance()  & 0x0800 ) // do not take masked-patched cells into account
            )
          {
            totE += theCell->e();
            if(theCell->quality() > LArQualityCut)
              badE += theCell->e();
          }
      }
  
    return true;
  }

  double JetCalcQuality::jetCalculation(){  
    if (totE==0) return 0;
    return badE/totE ;
  }


  bool JetCalcQuality::processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t /* w */){
    const xAOD::CaloCluster* theClus(NULL);
    theClus = dynamic_cast<const xAOD::CaloCluster*>(iter->rawConstituent());
    if (!theClus) return false;
  
    totE += iter->e(); // using iter since it is set at the expected scale by the JetCaloCalculations instance
    double f;
    bool retrieved = theClus->retrieveMoment(xAOD::CaloCluster::BADLARQ_FRAC, f);
    if(retrieved) badE += f * iter->e();
    return true;
  }


  // ****************************************************************
  // JetCalcQualityHEC *************************************************
  // ****************************************************************
  bool JetCalcQualityHEC::processCell(const CaloCell *theCell, weight_t)
  {
    if(theCell->caloDDE()->is_lar_hec())
      {
        if( ((theCell->provenance()) & 0x2000) &&
            !(theCell->provenance()  & 0x0800 ) )// do not take masked-patched cells into account
          {
            totE += theCell->e();
            if(theCell->quality() > LArQualityCut)
              badE += theCell->e();
          }
      }
  
    return true;
  }


  bool JetCalcQualityHEC::processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t /* w */){
    const xAOD::CaloCluster* theClus(NULL);
    theClus = dynamic_cast<const xAOD::CaloCluster*>(iter->rawConstituent());
    //ABc dummy implementation for now                                                                                                                      
    if (!theClus) return false;

    double clustHEC = theClus->eSample( CaloSampling::HEC0) + theClus->eSample( CaloSampling::HEC1) + 
      theClus->eSample( CaloSampling::HEC2) + theClus->eSample( CaloSampling::HEC3);
    totE += clustHEC ;
    
    double f;
    bool retrieved = theClus->retrieveMoment(xAOD::CaloCluster::BADLARQ_FRAC,f);

    if(retrieved) badE += f *clustHEC;

    return true;
  }

  // ****************************************************************
  // JetCalcNegativeEnergy *************************************************
  // ****************************************************************
  bool JetCalcNegativeEnergy::setupJet(const Jet* /*j*/){
    totE =0;
    totSig = 0;
    return true;
  }

  bool JetCalcNegativeEnergy::processCell(const CaloCell *theCell, weight_t)
  {
    if(theCell->e()<-2500)
      totE += theCell->e();
  
    return true;
  }

  double JetCalcNegativeEnergy::jetCalculation(){  

    if ( m_cellLevelCalculation )
      return totE;
    if( totSig != 0) return totE / totSig;

    return 0.;
  }

  bool JetCalcNegativeEnergy::processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t /* w */){
    const xAOD::CaloCluster* theClus(NULL);
    theClus = dynamic_cast<const xAOD::CaloCluster*>(iter->rawConstituent());
    if (!theClus) return false;
    double e = iter->e() ;  // using iter since it is set at the expected scale by the JetCaloCalculations instance
  
    double epos, sig;
    bool retrieved = theClus->retrieveMoment(xAOD::CaloCluster::ENG_POS, epos);
    retrieved = retrieved & theClus->retrieveMoment(xAOD::CaloCluster::SIGNIFICANCE, sig);
    
    totE += (e - epos ) * fabs( sig  );
    totSig += sig;

    return true;
  }

  // ****************************************************************
  // JetCalcCentroid *************************************************
  // ****************************************************************
  bool JetCalcCentroid::setupJet(const Jet* /*j*/){

    totE =0;
    centroid_x=0;  centroid_y=0;  centroid_z=0;
    return true;
  }

  bool JetCalcCentroid::processCell(const CaloCell */*theCell*/, weight_t)
  {
  
    return true;
  }

  double JetCalcCentroid::jetCalculation(){  

    if ( m_cellLevelCalculation )
      return 0;
    if (totE == 0) return 0;
  
    centroid_x = centroid_x/ totE;
    centroid_z = centroid_z/ totE;
    centroid_y = centroid_y/ totE;

    return sqrt(centroid_x*centroid_x + centroid_y*centroid_y+ centroid_z*centroid_z);
  }

  bool JetCalcCentroid::processConstituent(xAOD::JetConstituentVector::iterator& iter, weight_t /* w */){
    const xAOD::CaloCluster* theClus(NULL);
    theClus = dynamic_cast<const xAOD::CaloCluster*>(iter->rawConstituent());
    if (!theClus) return false;

    double e = iter->e() ;  // using iter since it is set at the expected scale by the JetCaloCalculations instance

    totE +=e;
    double x,y,z;
    bool retrieved = true;

    retrieved &= theClus->retrieveMoment(xAOD::CaloCluster::CENTER_X, x);
    retrieved &= theClus->retrieveMoment(xAOD::CaloCluster::CENTER_Y, y);
    retrieved &= theClus->retrieveMoment(xAOD::CaloCluster::CENTER_Z, z);

    centroid_x +=  x* e;
    centroid_y +=  y* e;
    centroid_z +=  z* e;

    return true;
  }

}
