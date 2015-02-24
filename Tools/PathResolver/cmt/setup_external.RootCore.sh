if test -z "${CALIBPATH:+x}"
then
   export CALIBPATH
fi

export CALIBPATH="$HOME/AtlasCalib:$ROOTCOREBIN/download:$ROOTCOREBIN/data:/cvmfs/atlas.cern.ch/repo/sw/database/GroupData:/afs/cern.ch/atlas/www/GROUPS/DATABASE/GroupData:http//atlas.web.cern.ch/Atlas/GROUPS/DATABASE/GroupData:$CALIBPATH"
