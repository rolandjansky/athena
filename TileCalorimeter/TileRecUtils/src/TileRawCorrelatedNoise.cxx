/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// Tile includes
#include "TileRecUtils/TileRawCorrelatedNoise.h"
#include "TileEvent/TileDigits.h"
#include "TileIdentifier/TileHWID.h"

// Atlas includes
// access all RawChannels inside container
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "EventContainers/SelectAllObject.h"
#include "AthenaKernel/errorcheck.h"
#include "PathResolver/PathResolver.h"



#include <memory>

// #############################################################################
TileRawCorrelatedNoise::TileRawCorrelatedNoise(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{
// #############################################################################

  declareProperty("nRMSThreshold", m_nRMS_threshold = 2.);
  declareProperty("AlphaMatrixFilePrefix", m_alphaMatrixFilePrefix  = "AlphaMatrix");
  declareProperty("MeanFilePrefix", m_meanFilePrefix  = "Mean");
  declareProperty("Sample3RMSFilePrefix", m_sample3RMSFilePrefix  = "RMS");
  declareProperty("UseMeanFiles", m_useMeanFiles = true);
  declareProperty("PMTOrder", m_pmtOrder = false);
}

// #############################################################################
TileRawCorrelatedNoise::~TileRawCorrelatedNoise() {
// #############################################################################

}

// #############################################################################
StatusCode TileRawCorrelatedNoise::initialize() {
// #############################################################################

  int PmtToChannelBarrel[48] = {
   1,  2,  3,  4,  5,  6,  7,  8,
   9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24,
  27, 26, 25, 30, 29, 28, 33, 32,
  31, 36, 35, 34, 39, 38, 37, 42,
  41, 40, 45, 44, 43, 48, 47, 46 };

  int PmtToChannelExtendedBarrel[48] = {
   1,  2,  3,  4,  5,  6,  7,  8,
   9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24,
  27, 26, 25, 31, 32, 28, 33, 29,
  30, 36, 35, 34, 44, 38, 37, 43,
  42, 41, 45, 39, 40, 48, 47, 46 };

  m_alphaMatrix = std::make_unique<AlphaMatrix>();

  // read alpha matrix
  FILE* AlphaMatrixFile[4][64];
  char Rosstr[10];
  char buff[1000];
  // Cycle over 4 partitions and 64 modules
  for (int Ros = 1; Ros < 5; ++Ros) {
    for (int Drawer = 0; Drawer < 64; ++Drawer) {
      // open file which corresponds to AlphaMatrix[Ros][Drawer]
      if (Ros == 1) sprintf(Rosstr, "LBA");
      else if (Ros == 2) sprintf(Rosstr, "LBC");
      else if (Ros == 3) sprintf(Rosstr, "EBA");
      else sprintf(Rosstr, "EBC");

      sprintf(buff, "%s%s%02d.txt", m_alphaMatrixFilePrefix.c_str(), Rosstr, Drawer + 1);
      std::string filestr(buff);
      std::string file = PathResolver::find_file(buff, "DATAPATH");
//      ATH_MSG_INFO( "  buff=" << buff
//                   << " filestr=" << filestr.c_str()
//                   << " file=" << file.c_str() );
      if (file.size() == 0) {
        ATH_MSG_WARNING( "Could not find input file " << buff );
      } else {
        ATH_MSG_INFO( "Reading file  " << file );
        AlphaMatrixFile[Ros - 1][Drawer] = fopen(file.c_str(), "r");
      }
      if (AlphaMatrixFile[Ros - 1][Drawer] == NULL) {
        ATH_MSG_ERROR( "Can't read input Alpha Matrix files." );
        return StatusCode::FAILURE;
      }
      ATH_MSG_DEBUG( " **** Start of Alpha Matrix Read Out" );
      FILE* alpha_file = AlphaMatrixFile[Ros - 1][Drawer];
//      if(fgets(buff, sizeof(buff), alpha_file) != NULL) {
//        ATH_MSG_DEBUG( "Matrix is being loaded: " << buff );
//      }
      // load tokens to be searched for in a string
      char* word;
      const char* TOKENS = { " \t\n" };
      // read Matrix
      int dima = 48;
      for (int line = 0; line < dima; line++) {
        if (fgets(buff, sizeof(buff), alpha_file) != NULL) {
          ATH_MSG_DEBUG( "line " << line << " is " << buff );
          for (int column = 0; column < dima; column++) {
            // Check for comment lines
            if (*buff == '!' || *buff == '*') continue;
            // read value
            int error = 0;
            if (column == 0) {
              if ((word = strtok(buff, TOKENS)) == NULL) error = 1;
            } else {
              if ((word = strtok(NULL, TOKENS)) == NULL) error = 1;
            }

            double pippo;
            if (error) pippo = 0.;
            else pippo = atof(word);

            ATH_MSG_VERBOSE ( "elem " << column << " is " << pippo );
            int chline = line;
            int chcolumn = column;
            // read alpha matrix in pmt order but save it in channel order if m_pmtOrder is true
            if (m_pmtOrder) {
              if (Ros < 3) {
                chline = PmtToChannelBarrel[line] - 1;
                chcolumn = PmtToChannelBarrel[column] - 1;
              } else {
                chline = PmtToChannelExtendedBarrel[line] - 1;
                chcolumn = PmtToChannelExtendedBarrel[column] - 1;
              }
            }
            m_alphaMatrix->m[Ros - 1][Drawer][chline][chcolumn] = pippo;
          }
        }
      }
      fclose(alpha_file);
    }
  }

  if (m_useMeanFiles) {
    // read mean
    int nSamples = 7;
    FILE* MeanFile[4][64];
    // cicle over 4 partitions and 64 modules
    for (int Ros = 1; Ros < 5; ++Ros) {
      for (int Drawer = 0; Drawer < 64; ++Drawer) {
        // open file which corresponds to Mean[Ros][Drawer]
        if (Ros == 1)
          sprintf(Rosstr, "LBA");
        else if (Ros == 2)
          sprintf(Rosstr, "LBC");
        else if (Ros == 3)
          sprintf(Rosstr, "EBA");
        else
          sprintf(Rosstr, "EBC");
        sprintf(buff, "%s%s%02d.txt", m_meanFilePrefix.c_str(), Rosstr, Drawer + 1);
        std::string filestr(buff);
        std::string file = PathResolver::find_file(buff, "DATAPATH");
        if (file.size() == 0) {
          ATH_MSG_VERBOSE ( "Could not find input file " << buff );
        } else {
          ATH_MSG_INFO( "Reading file  " << file );
          MeanFile[Ros - 1][Drawer] = fopen(file.c_str(), "r");
        }
        if (MeanFile[Ros - 1][Drawer] == NULL) {
          ATH_MSG_ERROR( "Can't read input Mean files." );
          return StatusCode::FAILURE;
        }

        ATH_MSG_DEBUG( " **** Start of Means Read Out" );
        FILE* mean_file = MeanFile[Ros - 1][Drawer];
        //if(fgets(buff, sizeof(buff), mean_file) != NULL) {
        //  if (lDebug)
        //    log << MSG::DEBUG << "Vector is being loaded: "<< buff << endmsg;
        //}
        // load tokens to be searched for in a string
        char* word;
        const char* TOKENS = { " \t\n" };
        // read Vector
        int dima = 48;
        for (int line = 0; line < dima; line++) {
          if (fgets(buff, sizeof(buff), mean_file) != NULL) {
            ATH_MSG_DEBUG( "line " << line << " is " << buff );
            for (int Sample = 0; Sample < nSamples; Sample++) {
              // Check for comment lines
              if (*buff == '!' || *buff == '*') continue;
              // read value
              int error = 0;
              if (Sample == 0) {
                if ((word = strtok(buff, TOKENS)) == NULL) error = 1;
              } else {
                if ((word = strtok(NULL, TOKENS)) == NULL) error = 1;
              }
              double pippo;
              if (error)
                pippo = 0.;
              else
                pippo = atof(word);
              ATH_MSG_VERBOSE ( "elem " << Sample << " is " << pippo );
              int chline = line;
              // read lines of mean matrix in pmt order but save it in channel order if m_pmtOrder is true
              if (m_pmtOrder) {
                if (Ros < 3) chline = PmtToChannelBarrel[line] - 1;
                else chline = PmtToChannelExtendedBarrel[line] - 1;
              }
              m_meanSamples[Ros - 1][Drawer][chline][Sample] = pippo;
            }
          }
        }
        fclose(mean_file);
      }
    }
  } else {
    // Initialize mean
    int nSamples = 7;
    for (int Ros = 1; Ros < 5; ++Ros) {
      for (int Drawer = 0; Drawer < 64; ++Drawer) {
        for (int Channel = 0; Channel < 48; ++Channel) {
          for (int Sample = 0; Sample < nSamples; ++Sample) {
            //MeanSamples[Ros-1][Drawer][Channel][Sample]=50.;
            m_meanSamples[Ros - 1][Drawer][Channel][Sample] = -1.;
          }
        }
      }
    }
  }

  // read sample 3 RMS
  FILE* Sample3RMSFile[4][64];
  // cicle over 4 partitions and 64 modules
  for (int Ros = 1; Ros < 5; ++Ros) {
    for (int Drawer = 0; Drawer < 64; ++Drawer) {
      // open file which corresponds to Sample3RMS[Ros][Drawer]
      if (Ros == 1) sprintf(Rosstr, "LBA");
      else if (Ros == 2) sprintf(Rosstr, "LBC");
      else if (Ros == 3) sprintf(Rosstr, "EBA");
      else sprintf(Rosstr, "EBC");

      sprintf(buff, "%s%s%02d.txt", m_sample3RMSFilePrefix.c_str(), Rosstr, Drawer + 1);
      std::string filestr(buff);
      std::string file = PathResolver::find_file(buff, "DATAPATH");
      if (file.size() == 0) {
        ATH_MSG_VERBOSE ( "Could not find input file " << buff );
      } else {
        ATH_MSG_INFO( "Reading file  " << file );
        Sample3RMSFile[Ros - 1][Drawer] = fopen(file.c_str(), "r");
      }
      if (Sample3RMSFile[Ros - 1][Drawer] == NULL) {
        ATH_MSG_ERROR( "Can't read input sample 3 RMS files." );
        return StatusCode::FAILURE;
      }

      ATH_MSG_DEBUG( " **** Start of sample 3 RMS Read Out" );
      FILE* rms_file = Sample3RMSFile[Ros - 1][Drawer];
      //if(fgets(buff, sizeof(buff), rms_file) != NULL) {
      //  if (lDebug)
      //    log << MSG::DEBUG << "Vector is being loaded: "<< buff << endmsg;
      //}
      // load tokens to be searched for in a string
      char* word;
      const char* TOKENS = { " \t\n" };
      // read Vector
      int dima = 48;
      for (int line = 0; line < dima; line++) {
        if (fgets(buff, sizeof(buff), rms_file) != NULL) {
          ATH_MSG_DEBUG(  "line " << line << " is " << buff );
          // Check for comment lines
          if (*buff == '!' || *buff == '*') continue;
          // read value
          double pippo;
          if ((word = strtok(buff, TOKENS)) == NULL) pippo = 0;
          else pippo = atof(word);
          // read value
          ATH_MSG_VERBOSE ( "elem is " << pippo );
          int chline = line;
          // read rms vector in pmt order but save it in channel order if m_pmtOrder is true
          if (m_pmtOrder) {
            if (Ros < 3) chline = PmtToChannelBarrel[line] - 1;
            else chline = PmtToChannelExtendedBarrel[line] - 1;
          }
          m_sample3RMS[Ros - 1][Drawer][chline] = pippo;
        }
      }
      fclose(rms_file);
    }
  }

  ATH_CHECK( m_inputDigitsContainerKey.initialize() );
  ATH_CHECK( m_outputDigitsContainerKey.initialize() );

  if (!m_useMeanFiles) {
    ATH_CHECK( m_tileToolNoiseSample.retrieve() );
  }

  ATH_MSG_INFO( "Initialization completed successfully" );

  return StatusCode::SUCCESS;
}

// #############################################################################
StatusCode TileRawCorrelatedNoise::execute() {
// #############################################################################

  // get named TileDigitsContaner from TES
  SG::ReadHandle<TileDigitsContainer> inputDigitsContainer(m_inputDigitsContainerKey);

  ATH_MSG_DEBUG( "Got TileDigitsContainer '" << m_inputDigitsContainerKey.key() << "'" );

  const TileHWID* tileHWID;
  CHECK( detStore()->retrieve(tileHWID, "TileHWID") );

  const TileDigits* OriginalDigits[4][64][48];

  // go through ALL TileDigits in container
  SelectAllObject<TileDigitsContainer> selAll(inputDigitsContainer.cptr());
  SelectAllObject<TileDigitsContainer>::const_iterator digItr = selAll.begin();
  SelectAllObject<TileDigitsContainer>::const_iterator lastDig = selAll.end();

  // read digits
  for (; digItr != lastDig; ++digItr) {
    const HWIdentifier adc_HWID = (*digItr)->adc_HWID();
    int Ros = tileHWID->ros(adc_HWID);
    int Drawer = tileHWID->drawer(adc_HWID);
    int Channel = tileHWID->channel(adc_HWID);
    OriginalDigits[Ros - 1][Drawer][Channel] = (*digItr);

    if (!m_useMeanFiles) {
      // read pedestal value and use it as mean
      unsigned int drawerIdx = TileCalibUtils::getDrawerIdx(Ros, Drawer);
      int adc = tileHWID->adc(adc_HWID);
      double ped = m_tileToolNoiseSample->getPed(drawerIdx, Channel, adc);
      int nSamples = 7;
      for (int Sample = 0; Sample < nSamples; ++Sample) {
        m_meanSamples[Ros - 1][Drawer][Channel][Sample] = ped;
      }
    }
  }

  // prepare new samples
  const int nSamples = 7;
  float NewSamples[4][64][48][nSamples];
  for (int Ros = 1; Ros < 5; ++Ros) {
    for (int Drawer = 0; Drawer < 64; ++Drawer) {
      for (int Channel = 0; Channel < 48; ++Channel) {
        for (int Sample = 0; Sample < nSamples; ++Sample) {
          NewSamples[Ros - 1][Drawer][Channel][Sample] =
              ((OriginalDigits[Ros - 1][Drawer][Channel])->samples())[Sample];
        }
      }
    }
  }

  // apply method
  for (int Ros = 1; Ros < 5; ++Ros) {
    for (int Drawer = 0; Drawer < 64; ++Drawer) {
      for (int Channel = 0; Channel < 48; ++Channel) {
        if (OriginalDigits[Ros - 1][Drawer][Channel]) {
          int nSamples = (OriginalDigits[Ros - 1][Drawer][Channel])->nsamples();
          std::vector<float> digits(nSamples);
          for (int jCh = 0; jCh < 48; ++jCh) {
            if (OriginalDigits[Ros - 1][Drawer][jCh]) {
              if (Channel != jCh
                  && fabs(((OriginalDigits[Ros - 1][Drawer][jCh])->samples())[3]
                          - m_meanSamples[Ros - 1][Drawer][jCh][3])
                      < m_nRMS_threshold * m_sample3RMS[Ros - 1][Drawer][jCh]) {

                for (int Sample = 0; Sample < nSamples; ++Sample)
                  NewSamples[Ros - 1][Drawer][Channel][Sample] =
                      NewSamples[Ros - 1][Drawer][Channel][Sample]
                           - m_alphaMatrix->m[Ros - 1][Drawer][Channel][jCh]
                              * (((OriginalDigits[Ros - 1][Drawer][jCh])->samples())[Sample]
                                  - m_meanSamples[Ros - 1][Drawer][jCh][Sample]);
              }
            }
          }
        }
      }
    }
  }

  // create new container
  TileDigits* NewDigits[4][64][48];
  SG::WriteHandle<TileDigitsContainer> outputDigitsContainer(m_outputDigitsContainerKey);
  ATH_CHECK( outputDigitsContainer.record( std::make_unique<TileDigitsContainer>() ) );

  // fill new container
  for (int Ros = 1; Ros < 5; ++Ros) {
    for (int Drawer = 0; Drawer < 64; ++Drawer) {
      for (int Channel = 0; Channel < 48; ++Channel) {
        if (OriginalDigits[Ros - 1][Drawer][Channel]) {
          int nSamples = (OriginalDigits[Ros - 1][Drawer][Channel])->nsamples();
          std::vector<float> digits(nSamples);
          for (int Sample = 0; Sample < nSamples; ++Sample) {
            digits[Sample] = NewSamples[Ros - 1][Drawer][Channel][Sample];
          }
          NewDigits[Ros - 1][Drawer][Channel] = new TileDigits(
              (OriginalDigits[Ros - 1][Drawer][Channel])->adc_HWID(), digits);
          outputDigitsContainer->push_back(NewDigits[Ros - 1][Drawer][Channel]);
        }
      }
    }
  }


  ATH_MSG_DEBUG( "execute completed successfully" );

  return StatusCode::SUCCESS;
}

// #############################################################################
StatusCode TileRawCorrelatedNoise::finalize() {
// #############################################################################

  ATH_MSG_INFO( " finalize completed successfully" );

  return StatusCode::SUCCESS;
}
