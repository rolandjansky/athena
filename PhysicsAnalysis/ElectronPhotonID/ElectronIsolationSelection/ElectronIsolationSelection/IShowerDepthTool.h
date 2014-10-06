// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IShowerDepthTool.h 605167 2014-07-07 14:11:27Z morrisj $
#ifndef ELECTRONISOLATIONSELECTION_ISHOWERDEPTHTOOL_H
#define ELECTRONISOLATIONSELECTION_ISHOWERDEPTHTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include <TH1.h>

namespace CP {

   /// Interface for ShowerDepthTool
   ///
   /// @author Bruno Lenzi <Bruno.Lenzi@cern.ch>
   /// @author John Morris <john.morris@cern.ch>
   ///
   /// $Revision: 605167 $
   /// $Date: 2014-07-07 16:11:27 +0200 (Mon, 07 Jul 2014) $
   ///
   class IShowerDepthTool : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IShowerDepthTool )

   public:
      /** Shower depth (in mm) on EM1 vs. eta, considering misalignments **/
      virtual float getCorrectedShowerDepthEM1(const float& etas1,const float& phi,const bool& isData = true) const = 0;

      /** Shower depth (in mm) on EM2 vs. eta, considering misalignments **/
      virtual float getCorrectedShowerDepthEM2(const float& etas2,const float& phi,const bool& isData = true) const = 0;

      /** Return the shower depth in R,Z considering misalignments **/
      virtual std::pair<float, float> getCorrectedRZ(const float& eta,const float& phi,const bool& isData = true,const int& sampling = 1) const = 0;
      
       /** Return the calorimeter displacement in R(Z) for barrel (endcap) **/
      virtual float getRZCorrection(const float& eta,const float& phi,const bool& isData = true) const = 0;     
      
      /** Eta direction from zvertex to the shower in the given sampling **/
      virtual float getCorrectedEtaDirection(const float& zvertex,const float& eta,const float& phi,const bool& isData=true,const int& sampling = 1) const = 0;
      
      /** Eta direction from samplings 1 and 2 (pointing) **/
      virtual float getCaloPointingEta(const float& etas1,const float& etas2,const float& phi,const bool& isData=true) const = 0;

      /** Shower depth (in mm) vs. eta on EM1 **/
      virtual float getShowerDepthEM1(const float& etas1) const = 0;
      
      /** Shower depth (in mm) vs. eta on EM2 **/
      virtual float getShowerDepthEM2(const float& etas2) const = 0;

      /** Shower depth in R,Z for the given sampling **/
      virtual std::pair<float, float> getRZ(const float& eta,const int& sampling) const = 0;

      virtual float getEtaDirection(const float& zvertex,const float& R,const float& z) const = 0;
      
   }; // class IShowerDepthTool

} // namespace CP

#endif // ELECTRONISOLATIONSELECTION_ISHOWERDEPTHTOOL_H
    