/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Mainpage for doxygen documentation
/**
@page KtJet_page KtJet package
 *
 * @author  <a href="http://www.hep.ucl.ac.uk/~jmb">J.Butterworth</a>,&nbsp;  
 *          <a href="http://www.hep.ucl.ac.uk/~jpc">J.Couchman</a>,&nbsp;
 *          <a href="http://www.apolloschildren.com/brian">B.Cox</a>,&nbsp;  
 *          <a href="http://www.hep.ucl.ac.uk/~waugh">B.Waugh</a> 
 *
@section KtJet_@section intro Introduction
 * This library package Contains the KtJet Clustering Algorithms and associated files. 
 * It has no Athena algorithms of its own and is designed to be used by other packages.
 *  
 * 
 * @subsection kc Known Clients
 * offline/Reconstruction/JetRec<br>
 * offline/Simulation/Atlfast/AtlfastAlgs
 *
 *<hr>
@section KtJet_@section pc Package Contents
 * @subsection mclasses Main Classes
 *   - KtJet::KtEvent 
 *       -# KtJet/KtEvent.h
 *       -# KtJet/KtEvent.icc
 *       -# src/KtEvent.cxx
 *   - KtJet::KtLorentzVector
 *       -# KtJet/KtLorentzVector.h
 *       -# KtJet/KtLorentzVector.icc
 *       -# src/KtLorentzVector.cxx
 *   - KtJet::KtJetTable
 *       -# KtJet/KtJetTable.h
 *       -# KtJet/KtJetTable.icc
 *       -# src/KtJetTable.cxx
 *   - KtJet::KtDistance
 *       -# KtJet/KtDistanceInterface.h
 *   - KtJet::KtRecom
 *       -# KtJet/KtRecomInterface.h
 *
 * @subsection oclasses Other Classes
 *
 *   - KtJet::KtDistanceAngle
 *       -# KtJet/KtDistance.h
 *       -# src/KtDistance.cxx
 *   - KtJet::KtDistanceDeltaR
 *       -# KtJet/KtDistance.h
 *       -# src/KtDistance.cxx
 *   - KtJet::KtDistanceQCD
 *       -# KtJet/KtDistance.h
 *       -# src/KtDistance.cxx
 *   - KtJet::KtRecomE
 *       -# KtJet/KtRecom.h
 *       -# src/KtRecom.cxx
 *   - KtJet::KtRecomEt
 *       -# KtJet/KtRecom.h
 *       -# src/KtRecom.cxx
 *   - KtJet::KtRecomEt2
 *       -# KtJet/KtRecom.h
 *       -# src/KtRecom.cxx
 *   - KtJet::KtRecomPt
 *       -# KtJet/KtRecom.h
 *       -# src/KtRecom.cxx
 *   - KtJet::KtRecomPt2
 *       -# KtJet/KtRecom.h
 *       -# src/KtRecom.cxx
 *
 * @subsection of Other Files
 *   - KtJet/KtJet.h
 *   - KtJet/KtUtil.h
 *   - src/KtUtil.cxx
 *   - cmt/requirements
 *   - ChangeLog
 *
 * <HR>
@section KtJet_@section ug Users Guide
 * More information can be found at
 * <a href="http://www.ktjet.org"> 
 * www.ktjet.org</a>
 * <hr>
@section KtJet_@section jobOptions jobOptions
 * None
 *
 */

/**
* @namespace KtJet
*
*  The KtJet namespace: Constains all the code needed to perform
* kt clustering   
*
* Authors:
*    J.Butterworth, J.Couchman, B.Cox, B.Waugh
*
* Written:
*    September-2002
* Revised:
*    November-2002
*/
