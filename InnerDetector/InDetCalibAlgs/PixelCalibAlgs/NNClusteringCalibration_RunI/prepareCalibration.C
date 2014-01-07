/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TTrainedNetwork.h"
#include "TNetworkToHistoTool.h"
#include "TFile.h"
#include <vector>
#include <iostream>
#include "TString.h"
#include "TDirectory.h"
#include "TH1.h"

using namespace std;

void prepareClusteringCalibrationFile(TString outputFileName="outputClusterCalibIBL_v1.root")
{
  
  vector<TString> nameFiles;
  vector<TString> nameCalibrationDirectory;
  
/*
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/number/noTrack_20/weights_nPat20_rate3_rdec95_mom3/weightMinimum.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/positions/noTrack_20_1/weights_nPat20_rate3_rdec95_mom3_1/Weights1030.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/positions/noTrack_200_2/weights_nPat200_rate30_rdec99_mom10_2/Weights4880.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/positions/noTrack_200_3/weights_nPat200_rate30_rdec99_mom10_3/Weights4780.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/noTrack_10faster_1_X/weights_nPat10_rate20_rdec98_mom5_1_X/Weights1250.root");//BEST
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/noTrack_10_2_X/weights_nPat10_rate10_rdec99_mom5_2_X/Weights8110.root");//UNFINISHED (LAST)
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/noTrack_10_3_X/weights_nPat10_rate10_rdec99_mom5_3_X/weightMinimum.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/noTrack_10faster_1_Y/weights_nPat10_rate20_rdec98_mom5_1_Y/Weights860.root");//UNFINISHED (LAST)
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/noTrack_10faster_2_Y/weights_nPat10_rate20_rdec98_mom5_2_Y/weightMinimum.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/noTrack_10faster_3_Y/weights_nPat10_rate20_rdec98_mom5_3_Y/weightMinimum.root");

  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/number/track_200/weights_nPat200_rate30_rdec99_mom10_withTracks/Weights3020.root");//TRAINING UNFINISHED
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/positions/track_200_1/weights_nPat200_rate30_rdec99_mom10_1_withTracks/Weights690.root");//BEST
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/positions/track_20_2/weights_nPat20_rate3_rdec95_mom3_2_withTracks/Weights900.root");//BEST
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/positions/track_200_3/weights_nPat200_rate30_rdec99_mom10_3_withTracks/Weights3160.root");//BEST
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/track_10faster_1_X/weights_nPat10_rate20_rdec98_mom5_1_X_withTracks/Weights1300.root");//BEST B
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/track_10_2_X/weights_nPat10_rate10_rdec99_mom5_2_X_withTracks/Weights7130.root");//UNFINISHED (LAST)
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/track_10faster_3_X/weights_nPat10_rate20_rdec98_mom5_3_X_withTracks/weightMinimum.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/track_10faster_1_Y/weights_nPat10_rate20_rdec98_mom5_1_Y_withTracks/Weights1250.root");//BEST 
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/track_10_2_Y/weights_nPat10_rate10_rdec99_mom5_2_Y_withTracks/weightMinimum.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/jetNet/jetNetFebruary2011/errors/track_10faster_3_Y/weights_nPat10_rate20_rdec98_mom5_3_Y_withTracks/weightMinimum.root");
*/
  
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/Weights.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightsOneTrack.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightsTwoTracks.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightsThreeTracks.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightOneTrackErrorX.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightTwoTracksErrorX.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightThreeTracksErrorX.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightOneTrackErrorY.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightTwoTracksErrorY.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withoutTracks/WeightThreeTracksErrorY.root");

  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/Weights.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightsOneTrack.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightsTwoTracks.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightsThreeTracks.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightOneTrackErrorX.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightTwoTracksErrorX.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightThreeTracksErrorX.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightOneTrackErrorY.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightTwoTracksErrorY.root");
  nameFiles.push_back("/afs/cern.ch/user/g/giacinto/scratch0/PixelClusterisationTF/prepareJetNetNtuple/withTracks/WeightThreeTracksErrorY.root");

  nameCalibrationDirectory.push_back("NumberParticles_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPoints1P_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPoints2P_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPoints3P_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPointErrorsX1_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPointErrorsX2_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPointErrorsX3_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPointErrorsY1_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPointErrorsY2_NoTrack");
  nameCalibrationDirectory.push_back("ImpactPointErrorsY3_NoTrack");
  
  nameCalibrationDirectory.push_back("NumberParticles");
  nameCalibrationDirectory.push_back("ImpactPoints1P");
  nameCalibrationDirectory.push_back("ImpactPoints2P");
  nameCalibrationDirectory.push_back("ImpactPoints3P");
  nameCalibrationDirectory.push_back("ImpactPointErrorsX1");
  nameCalibrationDirectory.push_back("ImpactPointErrorsX2");
  nameCalibrationDirectory.push_back("ImpactPointErrorsX3");
  nameCalibrationDirectory.push_back("ImpactPointErrorsY1");
  nameCalibrationDirectory.push_back("ImpactPointErrorsY2");
  nameCalibrationDirectory.push_back("ImpactPointErrorsY3");

  TFile* outputFile=new TFile(outputFileName,"recreate");

  TNetworkToHistoTool myHistoTool;

  for (int i=0;i<nameFiles.size();i++)
  {
    
    TFile* inputFile=new TFile(nameFiles[i]);
    TTrainedNetwork* trainedNetwork=(TTrainedNetwork*)inputFile->Get("TTrainedNetwork");
    
    vector<TH1*> histoVector=myHistoTool.fromTrainedNetworkToHisto(trainedNetwork);

    outputFile->cd("/");
    gDirectory->mkdir(nameCalibrationDirectory[i]);
    gDirectory->cd(nameCalibrationDirectory[i]);

    std::vector<TH1*>::const_iterator histoBegin=histoVector.begin();
    std::vector<TH1*>::const_iterator histoEnd=histoVector.end();

    for (std::vector<TH1*>::const_iterator histoIter=histoBegin;histoIter!=histoEnd;
         ++histoIter)
    {
      cout << " Iterator pointer: " << *histoIter << endl;
      if ((*histoIter)->GetName()!="TObject")
      {
        cout << "--> writing out  histogram: " << (*histoIter)->GetName() << endl;
        (*histoIter)->Write();
      }
    }
    
  }//end nameFiles
  
  outputFile->Write();

}
