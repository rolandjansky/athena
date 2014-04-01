#!/bin/sh
echo "Creating required links for InDetAlignExample"

ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB .
# ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200 .
cp ../share/mycool_cscasbuilt_00.db .

ln -s ../scripts/InDetCAF_Flags.py
ln -s ../scripts/InDetAlignExample_SuperScript.py
ln -s ../scripts/InDetAlignExample_IteratorForSi.py
ln -s ../scripts/beamspotExample.py
ln -s ../scripts/runBeamspotOnESD
ln -s ../scripts/InDetAlignExample_RunCog.py
ln -s ../scripts/RunTRTAlignment.py

ln -s ../share/InDetAlignBeamspotAOD.py
ln -s ../share/InDetAlignBeamspotFinder.py 
ln -s ../share/InDetAlignCog.py
ln -s ../share/InDetAlignExample_topOptions.py jobOptions.py
ln -s ../share/InDetAlignExample_Cosmic_topOptions.py jobOptions_Cosmic.py
ln -s ../share/InDetAlignBeamspotESDtoAOD.py
ln -s ../share/InDetAlignBeamspotVertexFinder.py
ln -s ../share/InDetAlignExample_Halo_topOptions.py jobOptions_Halo.py
ln -s ../share/InDetAlignExample_Gas_topOptions.py jobOptions_Gas.py

