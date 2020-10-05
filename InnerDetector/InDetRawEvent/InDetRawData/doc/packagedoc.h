/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetRawData_page InDetRawData Package
@author David Candlin, Veronique Boisvert, Maria.Jose.Costa@cern.ch, Susumu.Oda@cern.ch

@section InDetRawData_InDetRawDataIntro Introduction

This package is meant to contain the classes defining the Inner Detector
RawDataObjects, their collections and containers. There are different
classes for LHC, DC1/DC2 and combined test beam RawDataObjects (all of them
deriving from the same base class, InDetRawData).

@section InDetRawData_InDetRawDataOverview Class Overview
  The InDetRawData package contains the following classes:

   - InDetRawData: base class of the Inner Detector RDO

   - InDetRawDataCollection: Class to contain all the Raw Data Objects of a 
   detector element (module for Pixel or wafer for SCT or layer for TRT)

   - InDetRawDataContainer: Container for Raw Data Objects collections for 
   Pixel, SCT and TRT

   - PixelRDORawData: Abstract class to implement RawData for Pixel

   - Pixel1RawData: Class to implement DC1/DC2 RawData for Pixel
   
   - PixelTB04RawData: Class to implement test beam 2004 RawData for Pixel

   - SCT_RDORawData: Abstract class to implement RawData for SCT

   - SCT1_RawData: Class to implement DC1/DC2 RawData for SCT
   
   - SCT3_RawData: Class to implement LHC (Run 1, Run 2, Run 3) RawData for SCT

   - SCT_TB03_RawData: Class to implement test beam 2003 RawData for SCT

   - SCT_TB04_RawData: Class to implement test beam 2004 RawData for SCT

   - TRT_RDORawData: Abstract class to implement RawData for TRT

   - TRT_LoLumRawData: Class to implement DC1/DC2 RawData for TRT

   - TRT_TB04_RawData: Class to implement test beam 2004 RawData for TRT
   

*/
