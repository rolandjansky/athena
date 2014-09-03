# local database replica (script is protected against use outside CERN using SITEROOT)
setupLocalDBReplica_CERN.sh

# ID det descr info
# get_files -symlink TrackingGeometryMaterialDescription.DC3.root
# pool_insertFileToCatalog TrackingGeometryMaterialDescription.DC3.root
# get_files -symlink TrackingGeometryMaterialDescription.RomeInitial.root
# pool_insertFileToCatalog TrackingGeometryMaterialDescription.RomeInitial.root

#FIXME
#to be fixed source $TRIGGERRELEASEROOT/share/RecExCommon_trigger_links.sh

# echo atlfast stuff
# now use PathResolver
#too many files to be taken one by one
#ln -fs $ATLFASTALGSROOT/share/Atlfast*.tab .
#ln -fs $ATLFASTALGSROOT/share/Atlfast*.dat .

##echo btagging stuff not needed anymore 
##get_files -symlink -data  Sig_ref_xKal.root
##get_files -symlink -data  Sig_ref_iPat.root
##get_files -symlink -data  Bkg_ref_xKal.root
##get_files -symlink -data  Bkg_ref_iPat.root

#echo Eflow/atlfast stuff
# obsolete
#get_files -symlink -data FastShowerHistograms


#echo Moore stuff
#get_files -symlink -data "DeadMaterialMap*.data"
#get_files -symlink -data "Geantino*.data"
#get_files -symlink -data "DC2_*.dat"


##echo "xml Si description (need be copied)" not needed anymore
##get_files -symlink -data Materials.dtd 
##get_files -symlink -data AGDD.dtd

#still needed by PartPropSvc
#get_files -symlink -data PDGTABLE.MeV

#for b tagging, not needed anymore
#get_files -symlink -data btagcal.data
#get_files -symlink -data btagcal_his.data
#get_files -symlink -data cal_his.data

