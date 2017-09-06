/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TRT_RawDataByteStreamCnv_page TRT_RawDataByteStreamCnv Package
@author   Paul.Keener@cern.ch, Markus.Elsing@cern.ch

@section TRT_RawDataByteStreamCnv_TRT_RawDataByteStreamCnvInfro Introduction

This package contains the classes to read TRT real data or MC byte stream information and to write MC byte stream. It is using the new IDC. The decoding classes are used by the TriOnlineSpacePointProvider and the InDetTrigRawDataByteStreamCnv for the LVL2 and Event Filter BS decoding.

@section TRT_RawDataByteStreamCnv_TRT_RawDataByteStreamCnvRea Reading Byte Stream

 - TRTRawDataProvider: The main algorithm to decode the BS event for the TRT. It uses the TRTRawDataProviderTool to decode the fragments.

 - TRTRawDataProviderTool: the main tool to decode the BS fragments. It will update the IDC using the TRT_RodDecoder tool. The IDC is cleared at the beginning of a new event. For the Event Filter it there is bookkeeping not to decode a ROB block twice. No special code is needed to handle the old TRT readout with the split collections.

 - TRT_RodDecoder: the tool to decode a ROB fragment and to fill the collections into the IDC. It decodes the full fragement, creates the RDOs and pushes them into the collections in IDC. If a collection is not yet in the IDC, it is created on the fly. Note that the calling class has to make sure that the IDC is not filled already with the data. There is an optional argument in a vector<IdHash>*, which if given does restrict the decoding to the collections which have a hash listed in this vector.

@section TRT_RawDataByteStreamCnv_TRT_RawDataByteStreamCnvWri Writing Byte Stream

The writing of the Byte Stream makes use of the old Byte Stream converter infrastructure using StoreGate converters. It is using 3 classes:

 - TRTRawContByteStreamCnv: this is the ATHENA/Storegate/BSConverter class to write the TRT RDO container into a RawEvent file. The method to write RDO into BS is createRep(...). The reverse createObj(...) is not used an returns failure.

 - TRTRawContByteStreamTool: the driving tool to really do the conversions of the whole TRT RDO container. The method used is convert(...). For each ROD block a encoder tool is created, the RDOs are pushed into it and the encoder is asked to return the fragment.

 - TRT_RodEncoder: the tool that encodes the RDOs of one ROD block.



*/
