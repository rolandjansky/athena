#!/bin/bash

TARGET=~atlasdqm/dqmdisk/tier0/han_config

function get_git_rev {
  #REV=$(svn info $1 | grep "Last Changed Rev" | awk '{ print $4 }')
  REV=$(han-config-print collisions_run.hcfg | grep "Hash =" | cut -d"=" -f2 | tr -d ' ')
}

#for F in cosmics_run.config cosmics_minutes30.config cosmics_minutes10.config
for F in cosmics_run.config cosmics_minutes10.config
#for F in cosmics_run.config
do
  get_git_rev $F
  #REV=$(han-config-print collisions_run.hcfg | grep "Hash =" | cut -d"=" -f2)
  INF=$(basename $F .config).hcfg
  TARGETF=$TARGET/Cosmics/$(basename $F .config).$REV.hcfg
  echo copy $INF to $TARGETF
  cp $INF $TARGETF
  echo ln -sf $TARGETF $(dirname $TARGETF)/$(basename $F .config).current.hcfg
  ln -sf $TARGETF $(dirname $TARGETF)/$(basename $F .config).current.hcfg
done

#for F in collisions_run.config collisions_minutes30.config collisions_minutes10.config 
for F in collisions_run.config collisions_minutes10.config
#for F in collisions_run.config
do 
  get_git_rev $F
  INF=$(basename $F .config).hcfg
  TARGETF=$TARGET/Collisions/$(basename $F .config).$REV.hcfg
  echo copy $INF to $TARGETF
  cp $INF $TARGETF
  echo ln -sf $TARGETF $(dirname $TARGETF)/$(basename $F .config).current.hcfg
  ln -sf $TARGETF $(dirname $TARGETF)/$(basename $F .config).current.hcfg
done

#for F in heavyions_run.config heavyions_minutes30.config heavyions_minutes10.config                                                                                        
for F in heavyions_run.config heavyions_minutes10.config
#for F in heavyion_run.config                                                                                                                                              
do
  get_git_rev $F
  INF=$(basename $F .config).hcfg
  TARGETF=$TARGET/HeavyIons/$(basename $F .config).$REV.hcfg
  echo copy $INF to $TARGETF
  cp $INF $TARGETF
  echo ln -sf $TARGETF $(dirname $TARGETF)/$(basename $F .config).current.hcfg
  ln -sf $TARGETF $(dirname $TARGETF)/$(basename $F .config).current.hcfg
done