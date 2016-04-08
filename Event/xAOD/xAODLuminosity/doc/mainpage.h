/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage xAODLuminosity package

   @author Marjorie Shapiro <mdshapiro@lbl.gov>
   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   @section xAODLuminosityOverview Overview

   This package holds the xAOD EDM code for the metadata saved into ESD/xAOD
   files about which lumiblocks, and in what percentage were processed to
   create those files. This information is then used by various pieces of code
   in the offline and analysis releases to provide luminosity measurements for
   analysis.

   @section xAODLuminosityClasses Main Classes

   The main class(es) of the package are the following:
      - xAOD::LumiBlockRange: Type describing a single continuous luminosity
        block range.
      - xAOD::LumiBlockRangeContainer: A container of luminosity block ranges.
        This is the format that the information is written into the xAOD files
        in.

   @htmlinclude used_packages.html

   @include requirements
 */
