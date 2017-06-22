// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHRIVETTOOLS_ENUMS_H
#define TRUTHRIVETTOOLS_ENUMS_H

/// Higgs Template Cross Section namespace
namespace HTXS {

   /// Error code: whether the classification was successful or failed
   enum ErrorCode {

      /// Undefined/unknown error
      UNDEFINED=-99,
      /// Successful classification
      SUCCESS = 0,
      /// Production mode not defined
      PRODMODE_DEFINED = 1,
      /// Failed momentum conservation
      MOMENTUM_CONSERVATION = 2,
      /// Failed to identify Higgs boson
      HIGGS_IDENTIFICATION = 3,
      /// Failed to identify Higgs boson decay products
      HIGGS_DECAY_IDENTIFICATION = 4,
      /// Failed to identify hard scatter vertex
      HS_VTX_IDENTIFICATION = 5,
      /// Failed to identify associated vector boson
      VH_IDENTIFICATION = 6,
      /// Failed to identify associated vector boson decay products
      VH_DECAY_IDENTIFICATION = 7,
      /// Failed to identify top decay
      TOP_W_IDENTIFICATION = 8

   }; // enum ErrorCode

   /// Higgs production modes, corresponding to input sample
   enum HiggsProdMode {

      UNKNOWN = 0,
      GGF = 1,
      VBF = 2,
      WH = 3,
      QQ2ZH = 4,
      GG2ZH = 5,
      TTH = 6,
      BBH = 7,
      TH = 8 

   }; // enum HiggsProdMode
  
   /// Additional identifier flag for TH production modes
   enum tH_type {

      noTH=0,
      THQB=1,
      TWH=2

   }; // enum tH_type

   /// Namespace for Stage-0 categorization
   namespace Stage0 {

      /// Stage-0 categorization
      ///
      /// Two-digit number of format PF, with P for process,
      /// and F being 0 for |yH|>2.5 and 1 for |yH|<2.5.
      ///
      enum Category {

         UNKNOWN  = 0,
         GG2H_FWDH = 10,
         GG2H = 11,
         VBF_FWDH = 20,
         VBF = 21,
         VH2HQQ_FWDH = 22,
         VH2HQQ = 23,
         QQ2HLNU_FWDH = 30,
         QQ2HLNU = 31,
         QQ2HLL_FWDH = 40,
         QQ2HLL = 41,
         GG2HLL_FWDH = 50,
         GG2HLL = 51,
         TTH_FWDH = 60,
         TTH = 61,
         BBH_FWDH = 70,
         BBH = 71,
         TH_FWDH = 80,
         TH = 81

      }; // enum Category

   } // namespace Stage0
 
  /// Namespace for Stage-1 categorization
  namespace Stage1 {

     /// Stage-1 categorization
     ///
     /// Three digit integer of format PF.
     /// Where P is a digit representing the process
     /// F is a unique integer ( F < 99 ) corresponding to each Stage1
     /// phase-space region (bin)
     ///
     enum Category {

        UNKNOWN  = 0,

        /// @name Gluon fusion
        /// @{
        GG2H_FWDH = 100,
        GG2H_VBFTOPO_JET3VETO = 101,
        GG2H_VBFTOPO_JET3 = 102,
        GG2H_0J = 103,
        GG2H_1J_PTH_0_60 = 104,
        GG2H_1J_PTH_60_120 = 105,
        GG2H_1J_PTH_120_200 = 106,
        GG2H_1J_PTH_GT200 = 107,
        GG2H_GE2J_PTH_0_60 = 108,
        GG2H_GE2J_PTH_60_120 = 109,
        GG2H_GE2J_PTH_120_200 = 110,
        GG2H_GE2J_PTH_GT200 = 111,
        /// @}

        /// @name VBF
        /// @{
        QQ2HQQ_FWDH = 200,
        QQ2HQQ_VBFTOPO_JET3VETO = 201,
        QQ2HQQ_VBFTOPO_JET3 = 202,
        QQ2HQQ_VH2JET = 203,
        QQ2HQQ_REST = 204,
        QQ2HQQ_PTJET1_GT200 = 205,
        /// @}

        /// @name qq -> WH
        /// @{
        QQ2HLNU_FWDH = 300,
        QQ2HLNU_PTV_0_150 = 301,
        QQ2HLNU_PTV_150_250_0J = 302,
        QQ2HLNU_PTV_150_250_GE1J = 303,
        QQ2HLNU_PTV_GT250 = 304,
        /// @}

        /// @name qq -> ZH
        /// @{
        QQ2HLL_FWDH = 400,
        QQ2HLL_PTV_0_150 = 401,
        QQ2HLL_PTV_150_250_0J = 402,
        QQ2HLL_PTV_150_250_GE1J = 403,
        QQ2HLL_PTV_GT250 = 404,
        /// @}

        /// @name gg -> ZH
        /// @{
        GG2HLL_FWDH = 500,
        GG2HLL_PTV_0_150 = 501,
        GG2HLL_PTV_GT150_0J = 502,
        GG2HLL_PTV_GT150_GE1J = 503,
        /// @}

        /// @name ttH
        /// @{
        TTH_FWDH = 600,
        TTH = 601,
        /// @}

        /// @name bbH
        /// @{
        BBH_FWDH = 700,
        BBH = 701,
        /// @}

        /// @name tH
        /// @{
        TH_FWDH = 800,
        TH = 801
        /// @}

     }; // enum Category

  } // namespace Stage1

} // namespace HTXS

#endif // TRUTHRIVETTOOLS_ENUMS_H
